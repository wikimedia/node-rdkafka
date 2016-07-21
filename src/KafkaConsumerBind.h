#ifndef KAFKA_CONSUMER_H
#define KAFKA_CONSUMER_H

#include <atomic>
#include <node.h>
#include <nan.h>
#include <uv.h>

#include "rdkafkacpp.h"

#include "Queue.h"

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
        enum ResultType { MESSAGE, EVENT };
        class Result {
            public:
                Result(ResultType type): type(type) {}
                ResultType type;
        };
        /**
         * A wrapper for the JS callback and a message that should be sent to it.
         * Copies the message payload on construction.
         */
        class MessageResult : public Result {
            public:
                MessageResult(Nan::Persistent<Function>* c, RdKafka::Message* message);
                ~MessageResult();
                // Construct the message JS object
                Local<Object> toJSObject();
                // Construct the message error JS object
                Local<Object> toJSError();

                Nan::Persistent<Function>* callback;
                RdKafka::ErrorCode err;
            private:
                char* payload;
                uint32_t len;
                std::string topic;
                int32_t partition;
                double offset;
                const std::string* key;
                std::string errStr;
        };
        /**
         * A wrapper for RdKafka::Event class
         */
        class EventResult : public Result {
            public:
                EventResult(RdKafka::Event* event);

                Local<Object> toJSError();
                Local<Object> toJSLog();
                Local<Object> toJSThrottle();

                RdKafka::Event::Type type;
                RdKafka::ErrorCode err;
                RdKafka::Event::Severity severity;
                std::string fac;
                std::string str;
                int throttleTime;
                std::string brokerName;
                int brokerId;
        };


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
