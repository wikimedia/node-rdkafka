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
    RdKafka::Conf *conf = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);
    if (conf->set("group.id", "test_test_test11", errstr) != RdKafka::Conf::CONF_OK) {
        Nan::ThrowError(errstr.c_str());
    }
    if (conf->set("metadata.broker.list", "127.0.0.1:9092", errstr) != RdKafka::Conf::CONF_OK) {
        Nan::ThrowError(errstr.c_str());
    };

    this->impl = RdKafka::KafkaConsumer::create(conf, errstr);
    // TODO: Proper Error handling
    if (!this->impl) {
        Nan::ThrowError(errstr.c_str());
    }

    this->consumeRequestQueue = new JobQueue<Consumption>();
    uv_thread_create(&this->consumerThread, KafkaConsumerBind::ConsumerLoop, this);
}

KafkaConsumerBind::~KafkaConsumerBind() {
    delete this->impl;
    delete this->consumeRequestQueue;
    //must_stop = true; TODO
    //uv_thread_join(&work_thread);
};

NAN_METHOD(KafkaConsumerBind::Consume) {
    REQUIRE_ARGUMENTS(1);
    REQUIRE_ARGUMENT_FUNCTION(0, jsCallback);

    KafkaConsumerBind* obj = ObjectWrap::Unwrap<KafkaConsumerBind>(info.Holder());
    Consumption* consumption = new Consumption(jsCallback, &KafkaConsumerBind::ConsumerCallback);
    obj->consumeRequestQueue->pushJob(consumption);

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
    std::vector<Consumption*> consumerWork;
    // todo stop it
    while(true) {
        consumerWork = consumerBind->consumeRequestQueue->pullJobs();

        for(std::vector<Consumption*>::iterator it = consumerWork.begin(); it != consumerWork.end(); ++it) {
            Consumption* consumption = *it;
            // TODO: Fugire out shutdown
            consumption->message = consumerBind->impl->consume(-1);
            consumption->finishSignal.data = (void*) consumption;
            uv_async_send(&consumption->finishSignal);
        }

        consumerWork.clear();
    }
}

// Message consumed callback
void KafkaConsumerBind::ConsumerCallback(uv_async_t* handle) {
    Isolate* isolate = Isolate::GetCurrent();
    HandleScope handleScope(isolate);
    Consumption* result = static_cast<Consumption*> (handle->data);

    Local<Value> argv[] = { MessageBind::FromImpl(result->message) };
    Local<Function> jsCallback = Nan::New(*result->callback);
    Nan::MakeCallback(Nan::GetCurrentContext()->Global(), jsCallback, 1, argv);
    delete result;
}