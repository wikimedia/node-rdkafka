#ifndef PRODUCER_H
#define PRODUCER_H

#include <node.h>
#include <nan.h>

#include "Queue.h"

#include "rdkafkacpp.h"

using namespace v8;

class ProducerBind : public Nan::ObjectWrap, public RdKafka::DeliveryReportCb {
    public:
        static Nan::Persistent<Function> constructor;
        static NAN_MODULE_INIT(Init);

        // void produce(topic, payload)
        static NAN_METHOD(Produce);

        void dr_cb(RdKafka::Message &message);

        RdKafka::Producer* impl;
    private:
        static NAN_METHOD(New);

        ProducerBind(RdKafka::Conf* conf);
        ~ProducerBind();

        Queue<RdKafka::Message>* deliverReportQueue;
        uv_async_t deliveryNotifier;
        uv_thread_t pollingThread;

        static void Poller(void* context);
        static void DeliverReportCallback(uv_async_t* handle);
};

#endif
