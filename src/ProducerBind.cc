#include "ProducerBind.h"
#include "macros.h"

using namespace v8;

Nan::Persistent<Function> ProducerBind::constructor;

NAN_MODULE_INIT(ProducerBind::Init) {
    Nan::HandleScope scope;

    Local<FunctionTemplate> t = Nan::New<FunctionTemplate>(New);
    t->InstanceTemplate()->SetInternalFieldCount(1);
    t->SetClassName(Nan::New("Producer").ToLocalChecked());

    // Register all prototype methods
    Nan::SetPrototypeMethod(t, "produce", Produce);

    constructor.Reset(t->GetFunction());
    Nan::Set(target, Nan::New("Producer").ToLocalChecked(),
        Nan::GetFunction(t).ToLocalChecked());
}

NAN_METHOD(ProducerBind::New) {
    if (!info.IsConstructCall()) {
        return Nan::ThrowError("Non-constructor invocation not supported");
    }

    // TODO: handle configuration
    ProducerBind* obj = new ProducerBind();
    obj->Wrap(info.This());

    info.GetReturnValue().Set(info.This());
}

ProducerBind::ProducerBind() {
    std::string errstr;

    RdKafka::Conf *conf = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);
    if (conf->set("metadata.broker.list", "127.0.0.1:9092", errstr) != RdKafka::Conf::CONF_OK) {
        Nan::ThrowError(errstr.c_str());
    };
     if (conf->set("queue.buffering.max.messages", "1", errstr) != RdKafka::Conf::CONF_OK) {
        Nan::ThrowError(errstr.c_str());
    };

    this->impl = RdKafka::Producer::create(conf, errstr);
    // TODO: Proper Error handling
    if (!this->impl) {
        Nan::ThrowError(errstr.c_str());
    }
}

ProducerBind::~ProducerBind() {
    delete this->impl;
}

NAN_METHOD(ProducerBind::Produce) {
    std::string errstr;

    REQUIRE_ARGUMENTS(2);
    REQUIRE_ARGUMENT_STRING(0, topic_name);
    REQUIRE_ARGUMENT_STRING(1, payload);

    ProducerBind* obj = ObjectWrap::Unwrap<ProducerBind>(info.Holder());

    // TODO: We don't want to recreate it all the time, take it as argument
    RdKafka::Topic *topic = RdKafka::Topic::create(obj->impl, topic_name, NULL, errstr);
    if (!topic) {
        Nan::ThrowError(errstr.c_str());
    }

    RdKafka::ErrorCode resp = obj->impl->produce(topic, 0,
    			  RdKafka::Producer::RK_MSG_COPY /* TODO: think about it Copy payload */,
    			  const_cast<char *>(payload.c_str()), payload.size(),
    			  NULL, NULL);
    obj->impl->poll(0);
    if (resp != RdKafka::ErrorCode::ERR_NO_ERROR) {
        printf(" %d ", resp);
    }
    info.GetReturnValue().Set(Nan::Undefined());
}

