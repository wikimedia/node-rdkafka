#ifndef KAFKA_CONSUMER_H
#define KAFKA_CONSUMER_H

#include <node.h>
#include <nan.h>

#include "rdkafkacpp.h"

using namespace v8;

class KafkaConsumerBind : public Nan::ObjectWrap {
    public:
        static Nan::Persistent<Function> constructor;
        static NAN_MODULE_INIT(Init);

        // Message consumer(timeout);
        static NAN_METHOD(Consume);
        // void subscribe(topics[]);
        static NAN_METHOD(Subscribe);

        RdKafka::KafkaConsumer* impl;
    private:
        static NAN_METHOD(New);

        // TODO: take Conf object as the input
        KafkaConsumerBind();
        ~KafkaConsumerBind();
};

#endif
