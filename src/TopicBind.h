#ifndef TOPIC_H
#define TOPIC_H

#include <node.h>
#include <nan.h>

#include "rdkafkacpp.h"

using namespace v8;

// TODO: Figure out if we need this one or not. Depends on the perf of the producer
// - whether we need to recreate stuff every time or not.
class TopicBind : public Nan::ObjectWrap {
    public:
        static Nan::Persistent<Function> constructor;
        static NAN_MODULE_INIT(Init);
        static NAN_METHOD(New);

        // string name();
        static NAN_METHOD(Name);

        RdKafka::Topic* impl;
    private:

        // TODO: take Conf object as the input
        TopicBind();
        ~TopicBind();
};

#endif
