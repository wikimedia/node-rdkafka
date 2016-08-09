#ifndef RESULT_H
#define RESULT_H

#include <node.h>
#include <nan.h>

#include "rdkafkacpp.h"

using namespace v8;

enum ResultType { MESSAGE, EVENT };
class Result {
    public:
        explicit Result(ResultType type): resultType(type) {}
        ResultType resultType;
};

/**
 * A wrapper for the JS callback and a message that should be sent to it.
 * Copies the message payload on construction.
 */
class MessageResult : public Result {
    public:
        explicit MessageResult(Nan::Persistent<Function>* callback,
            RdKafka::Message* message);

        ~MessageResult();
        // Construct the message JS object
        Local<Object> toJSObject();
        // Construct the message error JS object
        Local<Object> toJSError();

        Nan::Persistent<Function>* callback;

        std::string topic;
        int32_t partition;
        double offset;

        char* payload;
        uint32_t len;
        const std::string* key;

        RdKafka::ErrorCode err;
        std::string errStr;
};

/**
 * A wrapper for RdKafka::Event class
 */
class EventResult : public Result {
    public:
        explicit EventResult(RdKafka::Event* event);

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

#endif