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

        // void produce()
        static NAN_METHOD(Produce);
        // void subscribe(topics[]);
        static NAN_METHOD(Subscribe);

        RdKafka::Producer* impl;
    private:
        static NAN_METHOD(New);

        // TODO: take Conf object as the input
        ProducerBind();
        ~ProducerBind();
};

#endif
