#include "MessageBind.h"

using namespace v8;

MessageBind::MessageBind() {};
MessageBind::~MessageBind() {};

Nan::Persistent<Function> MessageBind::constructor;

void
MessageBind::Init() {
  Nan::HandleScope scope;

  Local<FunctionTemplate> tpl = Nan::New<FunctionTemplate>(New);
  tpl->SetClassName(Nan::New("Message").ToLocalChecked());
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  // Register all prototype methods
  // Nan::SetPrototypeMethod(tpl, "start", WRAPPED_METHOD_NAME(Start));

  constructor.Reset(tpl->GetFunction());
}

Local<Object>
MessageBind::NewInstance(Local<Value> arg) {
  Nan::EscapableHandleScope scope;

  const unsigned argc = 1;
  Local<Value> argv[argc] = { arg };
  Local<Function> cons = Nan::New<Function>(constructor);
  Local<Object> instance = Nan::NewInstance(cons, argc, argv).ToLocalChecked();
  return scope.Escape(instance);
}

NAN_METHOD(MessageBind::New) {
    if (!info.IsConstructCall()) {
        return Nan::ThrowError("Non-constructor invocation not supported");
    }

    MessageBind* obj = new MessageBind();
    obj->Wrap(info.This());

    info.GetReturnValue().Set(info.This());
}
