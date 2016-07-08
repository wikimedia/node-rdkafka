#ifndef TOPIC_H
#define TOPIC_H

#include <node.h>
#include <nan.h>

#include "rdkafkacpp.h"

using namespace v8;

class TopicBind : public Nan::ObjectWrap {
    public:
        static Nan::Persistent<FunctionTemplate> constructor_template;
        static NAN_MODULE_INIT(Init);

        // String name();
        static NAN_METHOD(Name);

    private:
        static NAN_METHOD(New);

        TopicBind();
        ~TopicBind();
};

#endif