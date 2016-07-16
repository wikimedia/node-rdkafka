#include "KafkaConsumerBind.h"

#include "TopicPartitionBind.h"
#include "ConfHelper.h"
#include "macros.h"

using namespace v8;

Nan::Persistent<Function> KafkaConsumerBind::constructor;

NAN_MODULE_INIT(KafkaConsumerBind::Init) {
    Nan::HandleScope scope;

    Local<FunctionTemplate> t = Nan::New<FunctionTemplate>(New);
    t->InstanceTemplate()->SetInternalFieldCount(3); // TODO: figure out this number
    t->SetClassName(Nan::New("KafkaConsumer").ToLocalChecked());

    // Register all prototype methods
    Nan::SetPrototypeMethod(t, "consume", Consume);
    Nan::SetPrototypeMethod(t, "subscribe", Subscribe);
    Nan::SetPrototypeMethod(t, "commit", Commit);
    Nan::SetPrototypeMethod(t, "close", Close);

    constructor.Reset(t->GetFunction());

    Nan::Set(target, Nan::New("KafkaConsumer").ToLocalChecked(),
        Nan::GetFunction(t).ToLocalChecked());
};

NAN_METHOD(KafkaConsumerBind::New) {
    if (!info.IsConstructCall()) {
        return Nan::ThrowError("Non-constructor invocation not supported");
    }

    REQUIRE_ARGUMENTS(1);
    REQUIRE_ARGUMENT_OBJECT(0, jsConf);

    KafkaConsumerBind* obj = new KafkaConsumerBind(ConfHelper::CreateConfig(jsConf));

    obj->Wrap(info.This());
    info.GetReturnValue().Set(info.This());
};

KafkaConsumerBind::KafkaConsumerBind(RdKafka::Conf* conf) : running(true) {
    std::string errstr;
    this->impl = RdKafka::KafkaConsumer::create(conf, errstr);
    delete conf;

    // TODO: Proper Error handling
    if (!this->impl) {
        Nan::ThrowError(errstr.c_str());
    }

    // TODO: make blocking variable a enum
    this->consumeJobQueue = new Queue<Nan::Persistent<Function>>(true);
    this->consumeResultQueue = new Queue<ConsumeResult>(false);

    uv_async_init(uv_default_loop(), &this->resultNotifier, &KafkaConsumerBind::ConsumerCallback);
    this->resultNotifier.data = this;
    uv_thread_create(&this->consumerThread, KafkaConsumerBind::ConsumerLoop, this);
};

KafkaConsumerBind::~KafkaConsumerBind() {
    delete this->impl;
    delete this->consumeJobQueue;
    delete this->consumeResultQueue;
};

NAN_METHOD(KafkaConsumerBind::Consume) {
    REQUIRE_ARGUMENTS(1);
    REQUIRE_ARGUMENT_FUNCTION(0, jsCallback);

    KafkaConsumerBind* obj = ObjectWrap::Unwrap<KafkaConsumerBind>(info.Holder());
    obj->consumeJobQueue->push(new Nan::Persistent<Function>(jsCallback));

    info.GetReturnValue().Set(Nan::Undefined());
};

NAN_METHOD(KafkaConsumerBind::Subscribe) {
    REQUIRE_ARGUMENTS(1);

    // TODO: Find a more efficient way to cas the JS array to std::vector
    Local<Array> jsArray = Local<Array>::Cast(info[0]);
    std::vector<std::string> topics(jsArray->Length());
    for (int i = 0; i < jsArray->Length(); i++) {
        topics[i] = std::string(*Nan::Utf8String(jsArray->Get(i)));
    }

    KafkaConsumerBind* obj = ObjectWrap::Unwrap<KafkaConsumerBind>(info.Holder());
    RdKafka::ErrorCode err = obj->impl->subscribe( topics );
    if (err != RdKafka::ErrorCode::ERR_NO_ERROR) {
        Nan::ThrowError(RdKafka::err2str(err).c_str());
    }
};

// TODO: Make it take the callbacks
NAN_METHOD(KafkaConsumerBind::Commit) {
    REQUIRE_ARGUMENTS(1);

    KafkaConsumerBind* obj = ObjectWrap::Unwrap<KafkaConsumerBind>(info.Holder());

    if (info[0]->IsArray()) {
        // TODO: Find a more efficient way to cas the JS array to std::vector
        Local<Array> jsArray = Local<Array>::Cast(info[0]);
        std::vector<RdKafka::TopicPartition*> topicPartitions(jsArray->Length());
        for (int i = 0; i < jsArray->Length(); i++) {
            TopicPartitionBind* topicPartition = Nan::ObjectWrap::Unwrap<TopicPartitionBind>(Local<Object>::Cast(jsArray->Get(i)));
            topicPartitions[i] = topicPartition->impl;
        }
        obj->impl->commitAsync(topicPartitions);
    } else {
        Nan::ThrowError("Argument 0 must be an array");
    }
}

NAN_METHOD(KafkaConsumerBind::Close) {
    KafkaConsumerBind* obj = ObjectWrap::Unwrap<KafkaConsumerBind>(info.Holder());
    obj->running = false;
    obj->consumeJobQueue->stop();
    uv_thread_join(&obj->consumerThread);
    uv_close((uv_handle_t*) &obj->resultNotifier, NULL);

    RdKafka::ErrorCode err = obj->impl->close();
    if (err != RdKafka::ErrorCode::ERR_NO_ERROR) {
        Nan::ThrowError(RdKafka::err2str(err).c_str());
    }
}

// Consumer loop
void KafkaConsumerBind::ConsumerLoop(void* context) {
    KafkaConsumerBind* consumerBind = static_cast<KafkaConsumerBind*> (context);
    // todo stop it
    while(consumerBind->running) {
        std::vector<Nan::Persistent<Function>*>* consumerWork = consumerBind->consumeJobQueue->pull();

        if (!consumerWork) {
            // Nothing it returned, that means we were stopped.
            return;
        }
        for(std::vector<Nan::Persistent<Function>*>::iterator it = consumerWork->begin(); it != consumerWork->end(); ++it) {
            Nan::Persistent<Function>* consumption = *it;

            // TODO: Fugire out shutdown
            RdKafka::Message* message = consumerBind->impl->consume(500);
            while (message->err() == RdKafka::ErrorCode::ERR__PARTITION_EOF
                    || message->err() == RdKafka::ErrorCode::ERR__TIMED_OUT) {
                // This 'error' is thrown when there's no more messages in the queue to consume.
                // It doesn't make any sense, thus ignore it.
                delete message;
                message = consumerBind->impl->consume(500);

                if (!consumerBind->running) {
                    return;
                }
            }

            consumerBind->consumeResultQueue->push(new ConsumeResult(consumption, message));
        }

        delete consumerWork;
        uv_async_send(&consumerBind->resultNotifier);
    }
};

// Message consumed callback.
// The uv_async_t is notified after each message consumption, but libuv might coerce the callback
// execution.
// Called on event loop thread.
void KafkaConsumerBind::ConsumerCallback(uv_async_t* handle) {
    Isolate* isolate = Isolate::GetCurrent();
    HandleScope handleScope(isolate);

    KafkaConsumerBind* consumerBind = static_cast<KafkaConsumerBind*> (handle->data);
    std::vector<ConsumeResult*>* results = consumerBind->consumeResultQueue->pull();
    for(std::vector<ConsumeResult*>::iterator it = results->begin(); it != results->end(); ++it) {
        ConsumeResult* result = *it;
        Local<Function> jsCallback = Nan::New(*result->callback);
        if (result->message->err() == RdKafka::ErrorCode::ERR_NO_ERROR) {
            // Construct the message JS object
            Local<Object> jsMessage = Nan::New<Object>();
            jsMessage->Set(Nan::New("errStr").ToLocalChecked(),
                Nan::New(result->message->errstr()).ToLocalChecked());
            jsMessage->Set(Nan::New("err").ToLocalChecked(),
                Nan::New(result->message->err()));
            jsMessage->Set(Nan::New("topicName").ToLocalChecked(),
                Nan::New(result->message->topic_name()).ToLocalChecked());
            jsMessage->Set(Nan::New("partition").ToLocalChecked(),
                Nan::New(result->message->partition()));
            jsMessage->Set(Nan::New("payload").ToLocalChecked(),
                Nan::CopyBuffer((char*) result->message->payload(), (uint32_t) result->message->len()).ToLocalChecked());
            const std::string* key = result->message->key();
            if (key) {
                jsMessage->Set(Nan::New("key").ToLocalChecked(), Nan::New(*key).ToLocalChecked());
            }
            jsMessage->Set(Nan::New("offset").ToLocalChecked(), Nan::New((double) result->message->offset()));

            Local<Value> argv[] = { Nan::Null(), jsMessage };
            Nan::MakeCallback(Nan::GetCurrentContext()->Global(), jsCallback, 2, argv);
        } else {
            // Got at error, return it as the first callback arg
            Local<Object> error = Nan::Error(result->message->errstr().c_str()).As<Object>();
            error->Set(Nan::New("code").ToLocalChecked(), Nan::New(result->message->err()));
            Local<Value> argv[] = { error.As<Value>(), Nan::Null() };
            Nan::MakeCallback(Nan::GetCurrentContext()->Global(), jsCallback, 2, argv);
        }
        delete result;
    }
    delete results;
};