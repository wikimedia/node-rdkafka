#ifndef MESSAGE_H
#define MESSAGE_H

#include <node.h>
#include <nan.h>

#include "rdkafkacpp.h"

using namespace v8;

class MessageBind : public Nan::ObjectWrap {
    public:
        static Nan::Persistent<Function> constructor;
        static NAN_MODULE_INIT(Init);

        // string errStr();
        static NAN_METHOD(ErrStr);
        // int err();
        static NAN_METHOD(Err);
        // Topic topic();
        // TODO: static NAN_METHOD(Topic);
        //string topicName();
        // TODO: static NAN_METHOD(TopicName);
        // int partition();
        // TODO: NAN_METHOD(Partition);
        // Buffer payload();
        static NAN_METHOD(Payload);

        static Local<Object> FromImpl(RdKafka::Message* impl);
        RdKafka::Message* impl;
    private:
        static NAN_METHOD(New);

        MessageBind(RdKafka::Message* impl);
        ~MessageBind();
};

#endif