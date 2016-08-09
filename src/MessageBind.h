#ifndef MESSAGE_H
#define MESSAGE_H

#include <node.h>
#include <nan.h>

#include "rdkafkacpp.h"

#include "Result.h"

using namespace v8;

class MessageBind : public Nan::ObjectWrap {
    public:
        static Nan::Persistent<Function> constructor;
        static NAN_MODULE_INIT(Init);
        static NAN_METHOD(New);

        //string topicName;
        static NAN_GETTER(TopicName);
        // int partition;
        static NAN_GETTER(Partition);
        // Buffer payload;
        static NAN_GETTER(Payload);
        // String key;
        static NAN_GETTER(Key);
        // int offset;
        static NAN_GETTER(Offset);

        static Local<Object> FromImpl(MessageResult* impl);
    private:

        explicit MessageBind(MessageResult* impl);
        ~MessageBind();

        char* payload;
        uint32_t len;
        std::string topic;
        int32_t partition;
        double offset;
        const std::string* key;
        Persistent<Object>* buffer;
};

#endif