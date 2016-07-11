#include "MessageBind.h"
#include "macros.h"

using namespace v8;

Nan::Persistent<Function> MessageBind::constructor;

NAN_MODULE_INIT(MessageBind::Init) {
    Nan::HandleScope scope;

    Local<FunctionTemplate> t = Nan::New<FunctionTemplate>(New);
    t->InstanceTemplate()->SetInternalFieldCount(1);
    t->SetClassName(Nan::New("Message").ToLocalChecked());

    // Register all prototype methods
    Nan::SetPrototypeMethod(t, "errStr", ErrStr);
    Nan::SetPrototypeMethod(t, "err", Err);
    Nan::SetPrototypeMethod(t, "payload", Payload);

    constructor.Reset(t->GetFunction());
    Nan::Set(target, Nan::New("Message").ToLocalChecked(),
        Nan::GetFunction(t).ToLocalChecked());
}

NAN_METHOD(MessageBind::New) {
    if (!info.IsConstructCall()) {
        return Nan::ThrowError("Non-constructor invocation not supported");
    }

    // TODO: Prohibit direct instantiation
    REQUIRE_ARGUMENTS(1);
    REQUIRE_ARGUMENT_EXTERNAL(0, impl, RdKafka::Message*);
    MessageBind* obj = new MessageBind(impl);
    obj->Wrap(info.This());

    info.GetReturnValue().Set(info.This());
}

Local<Object> MessageBind::FromImpl(RdKafka::Message* impl) {
    int argc = 1;
    Local<Value> argv[] = {
        External::New(Isolate::GetCurrent(), impl)
    };
    return Nan::NewInstance(Nan::New(MessageBind::constructor), argc, argv).ToLocalChecked();
}

MessageBind::MessageBind(RdKafka::Message* impl) {
    this->impl = impl;
};
MessageBind::~MessageBind() {};

NAN_METHOD(MessageBind::ErrStr) {
   MessageBind* obj = ObjectWrap::Unwrap<MessageBind>(info.Holder());
    info.GetReturnValue().Set(Nan::New(obj->impl->errstr()).ToLocalChecked());
}

NAN_METHOD(MessageBind::Err) {
    MessageBind* obj = ObjectWrap::Unwrap<MessageBind>(info.Holder());
    info.GetReturnValue().Set(Nan::New(obj->impl->err()));
}

NAN_METHOD(MessageBind::Payload) {
    MessageBind* obj = ObjectWrap::Unwrap<MessageBind>(info.Holder());
    info.GetReturnValue().Set(Nan::NewBuffer((char*) obj->impl->payload(), (uint32_t) obj->impl->len()).ToLocalChecked());
}