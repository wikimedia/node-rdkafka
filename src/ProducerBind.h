#ifndef PRODUCER_H
#define PRODUCER_H

#include <atomic>
#include <node.h>
#include <nan.h>

#include "Queue.h"

#include "rdkafkacpp.h"

using namespace v8;

class DeliveryReport {
    public:
        DeliveryReport(Nan::Persistent<Function>* c,
            int64_t off,
            RdKafka::ErrorCode er,
            std::string erStr) : callback(c), offset(off), err(er), errStr(erStr) {}
        ~DeliveryReport() {
            this->callback->Reset();
            delete this->callback;
        }
        Nan::Persistent<Function>* callback;
        int64_t offset;
        RdKafka::ErrorCode err;
        std::string errStr;
};

class ProducerBind : public Nan::ObjectWrap, public RdKafka::DeliveryReportCb {
    public:
        static Nan::Persistent<Function> constructor;
        static NAN_MODULE_INIT(Init);

        // void produce(topic, payload)
        static NAN_METHOD(Produce);
        // void close()
        static NAN_METHOD(Close);

        void dr_cb(RdKafka::Message &message);

        RdKafka::Producer* impl;
    private:
        static NAN_METHOD(New);

        ProducerBind(RdKafka::Conf* conf);
        ~ProducerBind();

        Queue<DeliveryReport>* deliverReportQueue;
        uv_async_t deliveryNotifier;
        uv_thread_t pollingThread;

        std::atomic<bool> running;

        static void Poller(void* context);
        static void DeliverReportCallback(uv_async_t* handle);
};

#endif
