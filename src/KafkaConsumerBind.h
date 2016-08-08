#ifndef KAFKA_CONSUMER_H
#define KAFKA_CONSUMER_H

#include <atomic>
#include <node.h>
#include <nan.h>
#include <uv.h>

#include "rdkafkacpp.h"

#include "Queue.h"
#include "Result.h"

using namespace v8;

class KafkaConsumerBind : public Nan::ObjectWrap, public RdKafka::EventCb {
    public:
        static Nan::Persistent<Function> constructor;
        static NAN_MODULE_INIT(Init);

        // Message consumer(callback);
        static NAN_METHOD(Consume);
        // void subscribe(topics[]);
        static NAN_METHOD(Subscribe);
        // void commit(TopicPartition[]);
        static NAN_METHOD(Commit);
        // void close();
        static NAN_METHOD(Close);

        void event_cb (RdKafka::Event &event);

        RdKafka::KafkaConsumer* impl;
    private:
        static NAN_METHOD(New);

        KafkaConsumerBind(RdKafka::Conf* conf, Nan::Persistent<Function>* jsEmitCb);
        ~KafkaConsumerBind();
        RdKafka::ErrorCode doClose();
        static void ResultNotifierClosed(uv_handle_t* handle);

        static void ConsumerLoop(void* context);
        static void ConsumerCallback(uv_async_t* handle);
        uv_thread_t consumerThread;

        uv_async_t resultNotifier;

        Queue<Nan::Persistent<Function>>* consumeJobQueue;
        Queue<Result>* resultQueue;

        std::atomic<bool> running;

        Persistent<Function>* jsEmitCb;
};

#endif
