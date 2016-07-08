#ifndef TOPIC_PARTITION_H
#define TOPIC_PARTITION_H

#include <node.h>
#include <nan.h>

#include "rdkafkacpp.h"

using namespace v8;

class TopicPartitionBind : public Nan::ObjectWrap {
    public:
        static Nan::Persistent<FunctionTemplate> constructor_template;
        static NAN_MODULE_INIT(Init);

        TopicPartitionBind(std::string topic, int partition);
        TopicPartitionBind(RdKafka::TopicPartition* impl);

        // String topic();
        static NAN_METHOD(Topic);
        // int partition();
        static NAN_METHOD(Partition);
        // int offset();
        static NAN_METHOD(Offset);
        // void setOffset(int offset);
        static NAN_METHOD(SetOffset);
        // ErrorCode err();
        static NAN_METHOD(Err);

        RdKafka::TopicPartition* impl;
    private:
        static NAN_METHOD(New);

        ~TopicPartitionBind();
};

#endif