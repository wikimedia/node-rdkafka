#ifndef TOPIC_PARTITION_H
#define TOPIC_PARTITION_H

#include <node.h>
#include <nan.h>

#include "rdkafkacpp.h"

using namespace v8;

class TopicPartitionBind : public Nan::ObjectWrap {
    public:
        static Nan::Persistent<Function> constructor;
        static NAN_MODULE_INIT(Init);

        // String topic;
        static NAN_GETTER(Topic);
        // int partition;
        static NAN_GETTER(Partition);
        // int offset;
        static NAN_GETTER(Offset);
        static NAN_SETTER(SetOffset);
        // ErrorCode err;
        static NAN_GETTER(Err);

        static Local<Object> FromImpl(RdKafka::TopicPartition* impl);

        RdKafka::TopicPartition* impl;
    private:
        static NAN_METHOD(New);

        TopicPartitionBind(std::string topic, int partition);
        TopicPartitionBind(RdKafka::TopicPartition* impl);
        ~TopicPartitionBind();
};

#endif