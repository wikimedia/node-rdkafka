#include "MessageBind.h"
#include "macros.h"

using namespace v8;

Nan::Persistent<Function> MessageBind::constructor;

NAN_MODULE_INIT(MessageBind::Init) {
    Nan::HandleScope scope;

    Local<FunctionTemplate> t = Nan::New<FunctionTemplate>(New);
    t->InstanceTemplate()->SetInternalFieldCount(1);
    t->SetClassName(Nan::New("Message").ToLocalChecked());

    // Register getters for all the properties
    Nan::SetAccessor(t->InstanceTemplate(), Nan::New("errStr").ToLocalChecked(), ErrStr);
    Nan::SetAccessor(t->InstanceTemplate(), Nan::New("err").ToLocalChecked(), Err);
    Nan::SetAccessor(t->InstanceTemplate(), Nan::New("topicName").ToLocalChecked(), TopicName);
    Nan::SetAccessor(t->InstanceTemplate(), Nan::New("partition").ToLocalChecked(), Partition);
    Nan::SetAccessor(t->InstanceTemplate(), Nan::New("payload").ToLocalChecked(), Payload);
    Nan::SetAccessor(t->InstanceTemplate(), Nan::New("key").ToLocalChecked(), Key);
    Nan::SetAccessor(t->InstanceTemplate(), Nan::New("offset").ToLocalChecked(), Offset);
    Nan::SetAccessor(t->InstanceTemplate(), Nan::New("timestamp").ToLocalChecked(), Timestamp);

    constructor.Reset(t->GetFunction());
    Nan::Set(target, Nan::New("Message").ToLocalChecked(),
        Nan::GetFunction(t).ToLocalChecked());
}

NAN_METHOD(MessageBind::New) {
    if (!info.IsConstructCall()) {
        return Nan::ThrowError("Non-constructor invocation not supported");
    }

    // TODO: Prohibit direct instantiation
    REQUIRE_ARGUMENTS(1);
    REQUIRE_ARGUMENT_EXTERNAL(0, impl, RdKafka::Message*);
    MessageBind* obj = new MessageBind(impl);
    obj->Wrap(info.This());

    info.GetReturnValue().Set(info.This());
}

Local<Object> MessageBind::FromImpl(RdKafka::Message* impl) {
    int argc = 1;
    Local<Value> argv[] = { External::New(Isolate::GetCurrent(), impl) };
    return Nan::NewInstance(Nan::New(MessageBind::constructor), argc, argv).ToLocalChecked();
}

MessageBind::MessageBind(RdKafka::Message* impl) {
    this->impl = impl;
};
MessageBind::~MessageBind() {
    delete this->impl;
};

NAN_GETTER(MessageBind::ErrStr) {
   MessageBind* obj = ObjectWrap::Unwrap<MessageBind>(info.Holder());
   info.GetReturnValue().Set(Nan::New(obj->impl->errstr()).ToLocalChecked());
}

NAN_GETTER(MessageBind::Err) {
    MessageBind* obj = ObjectWrap::Unwrap<MessageBind>(info.Holder());
    info.GetReturnValue().Set(Nan::New(obj->impl->err()));
}

NAN_GETTER(MessageBind::TopicName) {
   MessageBind* obj = ObjectWrap::Unwrap<MessageBind>(info.Holder());
   info.GetReturnValue().Set(Nan::New(obj->impl->topic_name()).ToLocalChecked());
}

NAN_GETTER(MessageBind::Partition) {
   MessageBind* obj = ObjectWrap::Unwrap<MessageBind>(info.Holder());
   info.GetReturnValue().Set(Nan::New(obj->impl->partition()));
}

NAN_GETTER(MessageBind::Payload) {
    MessageBind* obj = ObjectWrap::Unwrap<MessageBind>(info.Holder());
    info.GetReturnValue().Set(Nan::CopyBuffer((char*) obj->impl->payload(), (uint32_t) obj->impl->len()).ToLocalChecked());
}

NAN_GETTER(MessageBind::Key) {
   MessageBind* obj = ObjectWrap::Unwrap<MessageBind>(info.Holder());
   const std::string* key = obj->impl->key();
   if (key != NULL) {
        info.GetReturnValue().Set(Nan::New(*key).ToLocalChecked());
   } else {
        info.GetReturnValue().Set(Nan::Undefined());
   }
}

NAN_GETTER(MessageBind::Offset) {
   MessageBind* obj = ObjectWrap::Unwrap<MessageBind>(info.Holder());
   info.GetReturnValue().Set(Nan::New((double) obj->impl->offset()));
}


NAN_GETTER(MessageBind::Timestamp) {
   MessageBind* obj = ObjectWrap::Unwrap<MessageBind>(info.Holder());
   RdKafka::MessageTimestamp timestamp = obj->impl->timestamp();
   if (timestamp.type == RdKafka::MessageTimestamp::MSG_TIMESTAMP_NOT_AVAILABLE) {
        info.GetReturnValue().Set(Nan::Undefined());
   } else {
        Local<Object> obj = Nan::New<Object>();
        obj->Set(Nan::New("type").ToLocalChecked(), Nan::New(timestamp.type));
        obj->Set(Nan::New("value").ToLocalChecked(), Nan::New((double) timestamp.timestamp));
        info.GetReturnValue().Set(obj);
   }
}