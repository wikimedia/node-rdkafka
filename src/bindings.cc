#include <node.h>
#include <nan.h>

#include "MessageBind.h"
#include "TopicBind.h"
#include "TopicPartitionBind.h"

using namespace v8;

NAN_MODULE_INIT(RegisterModule) {
    Nan::HandleScope scope;

    MessageBind::Init(target);
    TopicBind::Init(target);
    TopicPartitionBind::Init(target);
}

NODE_MODULE(bindings, RegisterModule)