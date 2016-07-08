#ifndef TOPIC_H
#define TOPIC_H

#include <node.h>
#include <nan.h>

#include "rdkafkacpp.h"

using namespace v8;

class TopicBind : public Nan::ObjectWrap {
    public:
        static void Init();
        static Local<Object> NewInstance(Local<Value> arg);

        // String name();
        static NAN_METHOD(Name);


    private:
        static NAN_METHOD(New);
        static Nan::Persistent<Function> constructor;

        TopicBind();
        ~TopicBind();

        RdKafka rd_topic;
};

#endif