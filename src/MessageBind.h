#ifndef MESSAGE_H
#define MESSAGE_H

#include <node.h>
#include <nan.h>

#include "rdkafkacpp.h"

using namespace v8;

class MessageBind : public Nan::ObjectWrap {
    public:
        static Nan::Persistent<FunctionTemplate> constructor_template;
        static NAN_MODULE_INIT(Init);
    private:
        static NAN_METHOD(New);

        MessageBind();
        ~MessageBind();
};

#endif