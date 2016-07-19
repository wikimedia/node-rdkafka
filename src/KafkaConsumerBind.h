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
        ConsumeResult(Nan::Persistent<Function>* c,
            void* payload, size_t len,
            std::string topic, int32_t partition, int64_t offset,
            RdKafka::ErrorCode err, std::string errStr,
            const std::string* key) {

            this->callback = c;
            this->payload = (char*) malloc(len);
            memcpy(this->payload, payload, len);
            this->len = (uint32_t) len;

            this->topic = topic;
            this->partition = partition;
            this->offset = offset;
            this->key = key;

            this->err = err;
            this->errStr = errStr;
        }
        ~ConsumeResult() {
            this->callback->Reset();
            delete this->callback;
            if (this->key) {
                delete this->key;
            }
            // Don't delete the payload here - it's passed to the v8 Buffer
            // without making a copy, so the memory is handled by v8 GC.
        }
        Nan::Persistent<Function>* callback;
        char* payload;
        uint32_t len;

        std::string topic;
        int32_t partition;
        double offset;

        const std::string* key;
        RdKafka::ErrorCode err;
        std::string errStr;
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
        static void ResultNotifierClosed(uv_handle_t* handle);

        static void ConsumerLoop(void* context);
        static void ConsumerCallback(uv_async_t* handle);
        uv_thread_t consumerThread;

        uv_async_t resultNotifier;

        Queue<Nan::Persistent<Function>>* consumeJobQueue;
        Queue<ConsumeResult>* consumeResultQueue;

        std::atomic<bool> running;
};

#endif
