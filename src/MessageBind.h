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

        // string errStr;
        static NAN_GETTER(ErrStr);
        // int err;
        static NAN_GETTER(Err);
        //string topicName;
        static NAN_GETTER(TopicName);
        // int partition;
        static NAN_GETTER(Partition);
        // Buffer payload;
        static NAN_GETTER(Payload);
        // String key;
        static NAN_GETTER(Key);
        // int offset;
        static NAN_GETTER(Offset);
        // Timestamp { type, value };
        static NAN_GETTER(Timestamp);

        static Local<Object> FromImpl(RdKafka::Message* impl);
        RdKafka::Message* impl;
    private:
        static NAN_METHOD(New);

        MessageBind(RdKafka::Message* impl);
        ~MessageBind();
};

#endif