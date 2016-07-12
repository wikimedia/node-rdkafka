#ifndef KAFKA_CONSUMER_H
#define KAFKA_CONSUMER_H

#include <node.h>
#include <nan.h>
#include <uv.h>

#include "rdkafkacpp.h"

#include "JobQueue.h"

using namespace v8;

// TODO: make it thread safe
class Consumption {
    public:
        Nan::Persistent<Function>* callback;
        uv_async_t finishSignal;
        RdKafka::Message* message;

        Consumption(Local<Function> callback, uv_async_cb finishCallback) {
            this->callback = new Nan::Persistent<Function>(callback);
            uv_async_init(uv_default_loop(), &this->finishSignal, finishCallback);
        }

        ~Consumption() {
            this->callback->Reset();
            delete this->callback;
            uv_close((uv_handle_t*) &this->finishSignal, NULL);
        }
};

class KafkaConsumerBind : public Nan::ObjectWrap {
    public:
        static Nan::Persistent<Function> constructor;
        static NAN_MODULE_INIT(Init);

        // Message consumer(callback);
        static NAN_METHOD(Consume);
        // void subscribe(topics[]);
        static NAN_METHOD(Subscribe);

        RdKafka::KafkaConsumer* impl;
    private:
        static NAN_METHOD(New);

        // TODO: take Conf object as the input
        KafkaConsumerBind();
        ~KafkaConsumerBind();

        static void ConsumerLoop(void* context);
        static void ConsumerCallback(uv_async_t* handle);
        uv_thread_t consumerThread;

        JobQueue<Consumption>* consumeRequestQueue;
};

#endif
