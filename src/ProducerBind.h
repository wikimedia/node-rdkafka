#ifndef PRODUCER_H
#define PRODUCER_H

#include <node.h>
#include <nan.h>

#include "rdkafkacpp.h"

using namespace v8;

class ProducerBind : public Nan::ObjectWrap {
    public:
        static Nan::Persistent<Function> constructor;
        static NAN_MODULE_INIT(Init);

        // void produce(topic, payload)
        static NAN_METHOD(Produce);

        RdKafka::Producer* impl;
    private:
        static NAN_METHOD(New);

        ProducerBind(RdKafka::Conf* conf);
        ~ProducerBind();
};

#endif
