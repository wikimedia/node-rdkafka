#include "MessageBind.h"

using namespace v8;

MessageBind::MessageBind() {};
MessageBind::~MessageBind() {};

Nan::Persistent<FunctionTemplate> MessageBind::constructor_template;

NAN_MODULE_INIT(MessageBind::Init) {
    Nan::HandleScope scope;

    Local<FunctionTemplate> t = Nan::New<FunctionTemplate>(New);
    t->InstanceTemplate()->SetInternalFieldCount(1);
    t->SetClassName(Nan::New("Message").ToLocalChecked());

    constructor_template.Reset(t);
    Nan::Set(target, Nan::New("Message").ToLocalChecked(),
        Nan::GetFunction(t).ToLocalChecked());
}

NAN_METHOD(MessageBind::New) {
    if (!info.IsConstructCall()) {
        return Nan::ThrowError("Non-constructor invocation not supported");
    }

    MessageBind* obj = new MessageBind();
    obj->Wrap(info.This());

    info.GetReturnValue().Set(info.This());
}
