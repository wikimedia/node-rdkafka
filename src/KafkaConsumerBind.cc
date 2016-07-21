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

    REQUIRE_ARGUMENTS(2);
    REQUIRE_ARGUMENT_OBJECT(0, jsConf);
    REQUIRE_ARGUMENT_FUNCTION(1, jsEmitCb);

    KafkaConsumerBind* obj = new KafkaConsumerBind(ConfHelper::CreateConfig(jsConf),
        new Nan::Persistent<Function>(jsEmitCb));

    obj->Wrap(info.This());
    info.GetReturnValue().Set(info.This());
};

KafkaConsumerBind::KafkaConsumerBind(RdKafka::Conf* conf, Nan::Persistent<Function>* jsEmitCb) : running(true) {
    std::string errstr;
    CONF_SET_PROPERTY(conf, "event_cb", this);

    this->jsEmitCb = jsEmitCb;
    this->impl = RdKafka::KafkaConsumer::create(conf, errstr);
    delete conf;

    if (!this->impl) {
        Nan::ThrowError(errstr.c_str());
    }

    this->consumeJobQueue = new Queue<Nan::Persistent<Function>>(Blocking::BLOCKING);
    this->resultQueue = new Queue<Result>(Blocking::NON_BLOCKING);

    uv_async_init(uv_default_loop(), &this->resultNotifier, &KafkaConsumerBind::ConsumerCallback);
    this->resultNotifier.data = this;
    uv_thread_create(&this->consumerThread, KafkaConsumerBind::ConsumerLoop, this);
};

KafkaConsumerBind::~KafkaConsumerBind() {
    if (this->running) {
        this->doClose();
    }
    delete this->impl;
    delete this->consumeJobQueue;
    delete this->resultQueue;
};

NAN_METHOD(KafkaConsumerBind::Consume) {
    REQUIRE_ARGUMENTS(1);
    REQUIRE_ARGUMENT_FUNCTION(0, jsCallback);

    KafkaConsumerBind* obj = ObjectWrap::Unwrap<KafkaConsumerBind>(info.Holder());

    if (obj->running) {
        obj->consumeJobQueue->push(new Nan::Persistent<Function>(jsCallback));
    } else {
        // TODO: call a callback?
    }

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
    RdKafka::ErrorCode err = obj->doClose();
    if (err != RdKafka::ErrorCode::ERR_NO_ERROR) {
        Nan::ThrowError(RdKafka::err2str(err).c_str());
    }
}

RdKafka::ErrorCode KafkaConsumerBind::doClose() {
    if (this->running) {
        this->running = false;
        this->consumeJobQueue->stop();
        uv_thread_join(&this->consumerThread);

        this->resultQueue->stop();
        uv_close((uv_handle_t*) &this->resultNotifier, NULL);


        RdKafka::ErrorCode err = this->impl->close();

        this->jsEmitCb->Reset();
        delete this->jsEmitCb;

        return err;
    }
    return RdKafka::ErrorCode::ERR_NO_ERROR;
}

void KafkaConsumerBind::event_cb (RdKafka::Event &event) {
    this->resultQueue->push(new EventResult(&event));
    uv_async_send(&this->resultNotifier);
}

// Consumer loop
void KafkaConsumerBind::ConsumerLoop(void* context) {
    KafkaConsumerBind* consumerBind = static_cast<KafkaConsumerBind*> (context);
    while(consumerBind->running) {
        // TODO: peek one element, not clone the whole thing
        std::vector<Nan::Persistent<Function>*>* consumerWork = consumerBind->consumeJobQueue->pull();

        if (!consumerWork) {
            // Nothing it returned, that means we were stopped.
            return;
        }
        for(std::vector<Nan::Persistent<Function>*>::iterator it = consumerWork->begin(); it != consumerWork->end(); ++it) {
            Nan::Persistent<Function>* consumption = *it;
            RdKafka::Message* message = consumerBind->impl->consume(500);
            while (message->err() == RdKafka::ErrorCode::ERR__PARTITION_EOF
                    || message->err() == RdKafka::ErrorCode::ERR__TIMED_OUT) {

                if (!consumerBind->running) {
                    delete message;
                    delete consumerWork;
                    return;
                }

                delete message;
                message = consumerBind->impl->consume(500);
            }

            if (!consumerBind->running) {
                delete message;
                delete consumerWork;
                return;
            }

            consumerBind->resultQueue->push(new MessageResult(consumption, message));
            delete message;
            uv_async_send(&consumerBind->resultNotifier);
        }
        delete consumerWork;
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
    std::vector<Result*>* results = consumerBind->resultQueue->pull();

    if (!results) {
        return;
    }

    for(std::vector<Result*>::iterator it = results->begin(); it != results->end(); ++it) {
        Result* result = *it;
        if (result->type == ResultType::MESSAGE) {
            MessageResult* msgResult = static_cast<MessageResult*>(result);
            Local<Function> jsCallback = Nan::New(*msgResult->callback);
            if (msgResult->err == RdKafka::ErrorCode::ERR_NO_ERROR) {
                Local<Value> argv[] = { Nan::Null(), msgResult->toJSObject() };
                Nan::MakeCallback(Nan::GetCurrentContext()->Global(), jsCallback, 2, argv);
            } else {
                // Got at error, return it as the first callback arg
                Local<Value> argv[] = { msgResult->toJSError(), Nan::Null() };
                Nan::MakeCallback(Nan::GetCurrentContext()->Global(), jsCallback, 2, argv);
            }
            delete msgResult;
        } else {
            Local<Function> emit = Nan::New(*consumerBind->jsEmitCb);
            EventResult* eventResult = static_cast<EventResult*>(result);
            switch (eventResult->type) {
                case RdKafka::Event::EVENT_ERROR: {
                    Local<Value> argv[] = { Nan::New("error").ToLocalChecked(), eventResult->toJSError() };
                    Nan::MakeCallback(Nan::GetCurrentContext()->Global(), emit, 2, argv);
                    break;
                }
                case RdKafka::Event::EVENT_LOG: {
                    Local<Value> argv[] = { Nan::New("log").ToLocalChecked(), eventResult->toJSLog() };
                    Nan::MakeCallback(Nan::GetCurrentContext()->Global(), emit, 2, argv);
                    break;
                }
                case RdKafka::Event::EVENT_THROTTLE: {
                    Local<Value> argv[] = { Nan::New("throttle").ToLocalChecked(), eventResult->toJSThrottle() };
                    Nan::MakeCallback(Nan::GetCurrentContext()->Global(), emit, 2, argv);
                    break;
                }
                case RdKafka::Event::EVENT_STATS: {
                    Local<Value> argv[] = {
                        Nan::New("stats").ToLocalChecked(),
                        Nan::New(eventResult->str.c_str()).ToLocalChecked()
                    };
                    Nan::MakeCallback(Nan::GetCurrentContext()->Global(), emit, 2, argv);
                    break;
                }
            }
            delete eventResult;
        }
    }
    delete results;
};

// Helper classes to hold the results of the invocation
// Construct the message JS object

// MessageResult
KafkaConsumerBind::MessageResult::MessageResult(Nan::Persistent<Function>* c, RdKafka::Message* message)
        : Result(ResultType::MESSAGE) {
    this->callback = c;
    this->payload = (char*) malloc(message->len());
    memcpy(this->payload, message->payload(), message->len());
    this->len = (uint32_t) message->len();

    this->topic = message->topic_name();
    this->partition = message->partition();
    this->offset = message->offset();
    this->key = message->key();

    this->err = message->err();
    this->errStr = message->errstr();
}
KafkaConsumerBind::MessageResult::~MessageResult() {
    this->callback->Reset();
    delete this->callback;
    if (this->key) {
        delete this->key;
    }
    // Don't delete the payload here - it's passed to the v8 Buffer
    // without making a copy, so the memory is handled by v8 GC.
}
Local<Object> KafkaConsumerBind::MessageResult::toJSObject() {
    Local<Object> jsMessage = Nan::New<Object>();
    jsMessage->Set(Nan::New("errStr").ToLocalChecked(), Nan::New(this->errStr).ToLocalChecked());
    jsMessage->Set(Nan::New("err").ToLocalChecked(), Nan::New(this->err));
    jsMessage->Set(Nan::New("topicName").ToLocalChecked(), Nan::New(this->topic).ToLocalChecked());
    jsMessage->Set(Nan::New("partition").ToLocalChecked(), Nan::New(this->partition));
    jsMessage->Set(Nan::New("payload").ToLocalChecked(),
        Nan::NewBuffer(this->payload, this->len).ToLocalChecked());
    const std::string* key = this->key;
    if (key) {
        jsMessage->Set(Nan::New("key").ToLocalChecked(), Nan::New(*key).ToLocalChecked());
    }
    jsMessage->Set(Nan::New("offset").ToLocalChecked(), Nan::New(this->offset));
    return jsMessage;
}
Local<Object> KafkaConsumerBind::MessageResult::toJSError() {
    Local<Object> error = Nan::Error(this->errStr.c_str()).As<Object>();
    error->Set(Nan::New("code").ToLocalChecked(), Nan::New(this->err));
    return error;
}

// EventResult
KafkaConsumerBind::EventResult::EventResult(RdKafka::Event* event): Result(ResultType::EVENT) {
    type = event->type();
    err = event->err();
    severity = event->severity();
    fac = event->fac();
    str = event->str();
    throttleTime = event->throttle_time();
    brokerName = event->broker_name();
    brokerId = event->broker_id();
}
Local<Object> KafkaConsumerBind::EventResult::toJSError() {
    Local<Object> error = Nan::Error(this->str.c_str()).As<Object>();
    error->Set(Nan::New("code").ToLocalChecked(), Nan::New(this->err));
    return error;
}
Local<Object> KafkaConsumerBind::EventResult::toJSLog() {
    Local<Object> jsLog = Nan::New<Object>();
    jsLog->Set(Nan::New("severity").ToLocalChecked(), Nan::New(this->severity));
    jsLog->Set(Nan::New("fac").ToLocalChecked(), Nan::New(this->fac.c_str()).ToLocalChecked());
    jsLog->Set(Nan::New("str").ToLocalChecked(), Nan::New(this->str.c_str()).ToLocalChecked());
    return jsLog;
}
Local<Object> KafkaConsumerBind::EventResult::toJSThrottle() {
    Local<Object> jsThrottle = Nan::New<Object>();
    jsThrottle->Set(Nan::New("throttleTime").ToLocalChecked(), Nan::New(this->throttleTime));
    jsThrottle->Set(Nan::New("brokerName").ToLocalChecked(), Nan::New(this->brokerName.c_str()).ToLocalChecked());
    jsThrottle->Set(Nan::New("broker_id").ToLocalChecked(), Nan::New(this->brokerId));
    return jsThrottle;
}