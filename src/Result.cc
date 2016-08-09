#include "Result.h"

using namespace v8;

// Helper classes to hold the results of the invocation

// MessageResult
MessageResult::MessageResult(Nan::Persistent<Function>* callback,
    RdKafka::Message* message) : Result(ResultType::MESSAGE),
        callback(callback),
        topic(message->topic_name()),
        partition(message->partition()),
        offset(message->offset()),
        err(message->err()),
        errStr(message->errstr()) {

    this->payload = (char*) malloc(message->len());
    memcpy(this->payload, message->payload(), message->len());
    this->len = (uint32_t) message->len();

    if (message->key_pointer() == NULL) {
        this->key = NULL;
    } else {
        this->key = new std::string((char*) message->key_pointer(), message->key_len());
    }
}
MessageResult::~MessageResult() {
    this->callback->Reset();
    delete this->callback;
}
Local<Object> MessageResult::toJSError() {
    Local<Object> error = Nan::Error(this->errStr.c_str()).As<Object>();
    error->Set(Nan::New("code").ToLocalChecked(), Nan::New(this->err));
    return error;
}

// EventResult
EventResult::EventResult(RdKafka::Event* event): Result(ResultType::EVENT) {
    type = event->type();
    err = event->err();
    severity = event->severity();
    fac = event->fac();
    str = event->str();
    throttleTime = event->throttle_time();
    brokerName = event->broker_name();
    brokerId = event->broker_id();
}
Local<Object> EventResult::toJSError() {
    Local<Object> error = Nan::Error(this->str.c_str()).As<Object>();
    error->Set(Nan::New("code").ToLocalChecked(), Nan::New(this->err));
    return error;
}

// TODO: Make Nan::ObjectWrap for the log
Local<Object> EventResult::toJSLog() {
    Local<Object> jsLog = Nan::New<Object>();
    jsLog->Set(Nan::New("severity").ToLocalChecked(), Nan::New(this->severity));
    jsLog->Set(Nan::New("fac").ToLocalChecked(), Nan::New(this->fac.c_str()).ToLocalChecked());
    jsLog->Set(Nan::New("str").ToLocalChecked(), Nan::New(this->str.c_str()).ToLocalChecked());
    return jsLog;
}
// TODO: Make Nan::ObjectWrap for the throttle event
Local<Object> EventResult::toJSThrottle() {
    Local<Object> jsThrottle = Nan::New<Object>();
    jsThrottle->Set(Nan::New("throttleTime").ToLocalChecked(), Nan::New(this->throttleTime));
    jsThrottle->Set(Nan::New("brokerName").ToLocalChecked(),
        Nan::New(this->brokerName.c_str()).ToLocalChecked());
    jsThrottle->Set(Nan::New("broker_id").ToLocalChecked(), Nan::New(this->brokerId));
    return jsThrottle;
}