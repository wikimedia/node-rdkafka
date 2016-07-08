#include <node.h>
#include <nan.h>

#include "MessageBind.h"
#include "TopicBind.h"

using namespace v8;

NAN_METHOD(CreateMessage) {
    info.GetReturnValue().Set(MessageBind::NewInstance(info[0]));
}

NAN_METHOD(CreateTopic) {
    info.GetReturnValue().Set(TopicBind::NewInstance(info[0]));
}

void InitAll(Local<Object> exports) {
  Nan::HandleScope scope;

  MessageBind::Init();
  TopicBind::Init();

  Nan::Set(exports, Nan::New("Message").ToLocalChecked(),
          Nan::GetFunction(Nan::New<FunctionTemplate>(CreateMessage)).ToLocalChecked());

  Nan::Set(exports, Nan::New("Topic").ToLocalChecked(),
          Nan::GetFunction(Nan::New<FunctionTemplate>(CreateTopic)).ToLocalChecked());
}

NODE_MODULE(bindings, InitAll)