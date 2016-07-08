#include "TopicBind.h"

using namespace v8;

Nan::Persistent<Function> TopicBind::constructor;

void
TopicBind::Init() {
  Nan::HandleScope scope;

  Local<FunctionTemplate> tpl = Nan::New<FunctionTemplate>(New);
  tpl->SetClassName(Nan::New("Topic").ToLocalChecked());
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  // Register all prototype methods
  Nan::SetPrototypeMethod(tpl, "name", Name);

  constructor.Reset(tpl->GetFunction());
}

Local<Object>
TopicBind::NewInstance(Local<Value> arg) {
  Nan::EscapableHandleScope scope;

  const unsigned argc = 1;
  Local<Value> argv[argc] = { arg };
  Local<Function> cons = Nan::New<Function>(constructor);
  Local<Object> instance = Nan::NewInstance(cons, argc, argv).ToLocalChecked();
  return scope.Escape(instance);
}

NAN_METHOD(TopicBind::New) {
    if (!info.IsConstructCall()) {
        return Nan::ThrowError("Non-constructor invocation not supported");
    }

    TopicBind* obj = new TopicBind();
    obj->Wrap(info.This());

    info.GetReturnValue().Set(info.This());
}

TopicBind::TopicBind() {
    std::string name("hello world");
    this->rd_topic = RdKafka::Topic.create()
};

TopicBind::~TopicBind() {};


NAN_METHOD(TopicBind::Name) {
    std::string something("hello world");
    info.GetReturnValue().Set(Nan::New(something).ToLocalChecked());
}
