#ifndef MESSAGE_H
#define MESSAGE_H

#include <node.h>
#include <nan.h>

#include "rdkafkacpp.h"

using namespace v8;

class MessageBind : public Nan::ObjectWrap {
    public:
        static void Init();
        static Local<Object> NewInstance(Local<Value> arg);
    private:
        static NAN_METHOD(New);
        static Nan::Persistent<Function> constructor;

        MessageBind();
        ~MessageBind();
};

#endif