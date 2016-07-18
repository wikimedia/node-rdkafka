#ifndef KAFKA_CONSUMER_H
#define KAFKA_CONSUMER_H

#include <node.h>
#include <nan.h>
#include <uv.h>

#include "rdkafkacpp.h"

#include "Queue.h"

using namespace v8;

class ConsumeResult {
    public:
        ConsumeResult(Nan::Persistent<Function>* c, RdKafka::Message* m) : callback(c), message(m) {}
        ~ConsumeResult() {
            this->callback->Reset();
            delete this->callback;
            delete this->message;
        }
        Nan::Persistent<Function>* callback;
        RdKafka::Message* message;
};

class KafkaConsumerBind : public Nan::ObjectWrap {
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

        RdKafka::KafkaConsumer* impl;
    private:
        static NAN_METHOD(New);


        KafkaConsumerBind(RdKafka::Conf* conf);
        ~KafkaConsumerBind();
        RdKafka::ErrorCode doClose();

        static void ConsumerLoop(void* context);
        static void ConsumerCallback(uv_async_t* handle);
        uv_thread_t consumerThread;

        uv_async_t resultNotifier;

        Queue<Nan::Persistent<Function>>* consumeJobQueue;
        Queue<ConsumeResult>* consumeResultQueue;

        std::atomic<bool> running;
};

#endif
