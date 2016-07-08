#include "TopicBind.h"

using namespace v8;

Nan::Persistent<FunctionTemplate> TopicBind::constructor_template;

NAN_MODULE_INIT(TopicBind::Init) {
    Nan::HandleScope scope;

    Local<FunctionTemplate> t = Nan::New<FunctionTemplate>(New);
    t->InstanceTemplate()->SetInternalFieldCount(1);
    t->SetClassName(Nan::New("Topic").ToLocalChecked());

    Nan::SetPrototypeMethod(t, "name", Name);

    constructor_template.Reset(t);
    Nan::Set(target, Nan::New("Topic").ToLocalChecked(),
        Nan::GetFunction(t).ToLocalChecked());
}

NAN_METHOD(TopicBind::New) {
    if (!info.IsConstructCall()) {
        return Nan::ThrowError("Non-constructor invocation not supported");
    }

    TopicBind* obj = new TopicBind();
    obj->Wrap(info.This());

    info.GetReturnValue().Set(info.This());
}

TopicBind::TopicBind() {};
TopicBind::~TopicBind() {};


NAN_METHOD(TopicBind::Name) {
    std::string something("hello world");
    info.GetReturnValue().Set(Nan::New(something).ToLocalChecked());
}
