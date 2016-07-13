#include "KafkaConsumerBind.h"
#include "MessageBind.h"
#include "macros.h"

using namespace v8;

Nan::Persistent<Function> KafkaConsumerBind::constructor;

NAN_MODULE_INIT(KafkaConsumerBind::Init) {
    Nan::HandleScope scope;

    Local<FunctionTemplate> t = Nan::New<FunctionTemplate>(New);
    t->InstanceTemplate()->SetInternalFieldCount(3);
    t->SetClassName(Nan::New("KafkaConsumer").ToLocalChecked());

    // Register all prototype methods
    Nan::SetPrototypeMethod(t, "consume", Consume);
    Nan::SetPrototypeMethod(t, "subscribe", Subscribe);

    constructor.Reset(t->GetFunction());

    Nan::Set(target, Nan::New("KafkaConsumer").ToLocalChecked(),
        Nan::GetFunction(t).ToLocalChecked());
}

NAN_METHOD(KafkaConsumerBind::New) {
    if (!info.IsConstructCall()) {
        return Nan::ThrowError("Non-constructor invocation not supported");
    }

    KafkaConsumerBind* obj = new KafkaConsumerBind();

    obj->Wrap(info.This());
    info.GetReturnValue().Set(info.This());
}

KafkaConsumerBind::KafkaConsumerBind() {
    // TODO: Handle configuration and errors
    std::string errstr;

    RdKafka::Conf *tconf = RdKafka::Conf::create(RdKafka::Conf::CONF_TOPIC);
    if (tconf->set("auto.offset.reset", "smallest", errstr) != RdKafka::Conf::CONF_OK) {
        Nan::ThrowError(errstr.c_str());
    };

    RdKafka::Conf *conf = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);
    if (conf->set("group.id", "dtest_test_test8", errstr) != RdKafka::Conf::CONF_OK) {
        Nan::ThrowError(errstr.c_str());
    }
    if (conf->set("metadata.broker.list", "127.0.0.1:9092", errstr) != RdKafka::Conf::CONF_OK) {
        Nan::ThrowError(errstr.c_str());
    };
    if (conf->set("fetch.wait.max.ms", "0", errstr) != RdKafka::Conf::CONF_OK) {
        Nan::ThrowError(errstr.c_str());
    };
    if (conf->set("default_topic_conf", tconf, errstr) != RdKafka::Conf::CONF_OK) {
        Nan::ThrowError(errstr.c_str());
    };

    this->impl = RdKafka::KafkaConsumer::create(conf, errstr);
    // TODO: Proper Error handling
    if (!this->impl) {
        Nan::ThrowError(errstr.c_str());
    }

    this->consumeRequestQueue = new JobQueue<Nan::Persistent<Function>, ConsumeResult>();
    uv_async_init(uv_default_loop(), &this->resultNotifier, &KafkaConsumerBind::ConsumerCallback);
    this->resultNotifier.data = this;
    uv_thread_create(&this->consumerThread, KafkaConsumerBind::ConsumerLoop, this);
}

KafkaConsumerBind::~KafkaConsumerBind() {
    delete this->impl;
    delete this->consumeRequestQueue;
    uv_close((uv_handle_t*) &this->resultNotifier, NULL);
    //must_stop = true; TODO
    //uv_thread_join(&work_thread);
};

NAN_METHOD(KafkaConsumerBind::Consume) {
    REQUIRE_ARGUMENTS(1);
    REQUIRE_ARGUMENT_FUNCTION(0, jsCallback);

    KafkaConsumerBind* obj = ObjectWrap::Unwrap<KafkaConsumerBind>(info.Holder());
    obj->consumeRequestQueue->pushJob(new Nan::Persistent<Function>(jsCallback));

    info.GetReturnValue().Set(Nan::Undefined());
}

NAN_METHOD(KafkaConsumerBind::Subscribe) {
    REQUIRE_ARGUMENTS(1);

    // TODO: Find a more efficient way to cas the JS array to std::vector
    Local<Array> jsArray = Local<Array>::Cast(info[0]);
    std::vector<std::string> topics(jsArray->Length());
    for (int i = 0; i < jsArray->Length(); i++) {
        topics[i] = std::string(*Nan::Utf8String(jsArray->Get(i)));
    }

    KafkaConsumerBind* obj = ObjectWrap::Unwrap<KafkaConsumerBind>(info.Holder());
    obj->impl->subscribe( topics );
    // TODO: Error handling
}


// Consumer loop
void KafkaConsumerBind::ConsumerLoop(void* context) {
    KafkaConsumerBind* consumerBind = static_cast<KafkaConsumerBind*> (context);
    // todo stop it
    while(true) {
        std::vector<Nan::Persistent<Function>*>* consumerWork = consumerBind->consumeRequestQueue->pullJobs();

        for(std::vector<Nan::Persistent<Function>*>::iterator it = consumerWork->begin(); it != consumerWork->end(); ++it) {
            Nan::Persistent<Function>* consumption = *it;

            // TODO: Fugire out shutdown
            RdKafka::Message* message = consumerBind->impl->consume(-1);
            while (message->err() == RdKafka::ErrorCode::ERR__PARTITION_EOF) {
                // This 'error' is thrown when there's no more messages in the queue to consume.
                // It doesn't make any sense, thus ignore it.
                delete message;
                message = consumerBind->impl->consume(-1);
            }

            consumerBind->consumeRequestQueue->pushResult(new ConsumeResult(consumption, message));
        }

        delete consumerWork;
        uv_async_send(&consumerBind->resultNotifier);
    }
}

// TODO: then callback might be executed not once/not executed etc.
// Message consumed callback
void KafkaConsumerBind::ConsumerCallback(uv_async_t* handle) {
    Isolate* isolate = Isolate::GetCurrent();
    HandleScope handleScope(isolate);

    KafkaConsumerBind* consumerBind = static_cast<KafkaConsumerBind*> (handle->data);
    std::vector<ConsumeResult*>* results = consumerBind->consumeRequestQueue->pullResults();
    for(std::vector<ConsumeResult*>::iterator it = results->begin(); it != results->end(); ++it) {
        ConsumeResult* result = *it;
        Local<Function> jsCallback = Nan::New(*result->callback);
        if (result->message->err() == RdKafka::ErrorCode::ERR_NO_ERROR) {
            Local<Value> argv[] = { Nan::Undefined(), MessageBind::FromImpl(result->message) };
            Nan::MakeCallback(Nan::GetCurrentContext()->Global(), jsCallback, 2, argv);
        } else {
            // Got at error, return it as the first callback arg
            Local<Object> error = Nan::Error(result->message->errstr().c_str()).As<Object>();
            error->Set(Nan::New("code").ToLocalChecked(), Nan::New(result->message->err()));
            Local<Value> argv[] = { error.As<Value>() };
            Nan::MakeCallback(Nan::GetCurrentContext()->Global(), jsCallback, 1, argv);
        }
        delete result;
    }
    delete results;
}