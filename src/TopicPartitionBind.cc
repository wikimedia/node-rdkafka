#include "TopicPartitionBind.h"
#include "macros.h"

using namespace v8;

Nan::Persistent<FunctionTemplate> TopicPartitionBind::constructor_template;

NAN_MODULE_INIT(TopicPartitionBind::Init) {
    Nan::HandleScope scope;

    Local<FunctionTemplate> t = Nan::New<FunctionTemplate>(New);
    t->InstanceTemplate()->SetInternalFieldCount(1);
    t->SetClassName(Nan::New("TopicPartition").ToLocalChecked());

    // Register all prototype methods
    Nan::SetPrototypeMethod(t, "topic", Topic);
    Nan::SetPrototypeMethod(t, "partition", Partition);
    Nan::SetPrototypeMethod(t, "offset", Offset);
    Nan::SetPrototypeMethod(t, "setOffset", SetOffset);
    Nan::SetPrototypeMethod(t, "err", Err);

    constructor_template.Reset(t);
    Nan::Set(target, Nan::New("TopicPartition").ToLocalChecked(),
        Nan::GetFunction(t).ToLocalChecked());
}

NAN_METHOD(TopicPartitionBind::New) {
    if (!info.IsConstructCall()) {
        return Nan::ThrowError("Non-constructor invocation not supported");
    }

    REQUIRE_ARGUMENTS(2);
    REQUIRE_ARGUMENT_STRING(0, topic);
    REQUIRE_ARGUMENT_NUMBER(1, partition);

    TopicPartitionBind* obj = new TopicPartitionBind(std::string(*topic), (int)partition);
    obj->Wrap(info.This());

    info.GetReturnValue().Set(info.This());
}

TopicPartitionBind::TopicPartitionBind(std::string topic, int partition) {
    this->impl = RdKafka::TopicPartition::create(topic, partition);
}

TopicPartitionBind::TopicPartitionBind(RdKafka::TopicPartition* impl) {
    this->impl = impl;
}

TopicPartitionBind::~TopicPartitionBind() {};

NAN_METHOD(TopicPartitionBind::Topic) {
    TopicPartitionBind* obj = ObjectWrap::Unwrap<TopicPartitionBind>(info.Holder());
    info.GetReturnValue().Set(Nan::New(obj->impl->topic()).ToLocalChecked());
}

NAN_METHOD(TopicPartitionBind::Partition) {
    TopicPartitionBind* obj = ObjectWrap::Unwrap<TopicPartitionBind>(info.Holder());
    info.GetReturnValue().Set(Nan::New(obj->impl->partition()));
}

NAN_METHOD(TopicPartitionBind::Offset) {
    TopicPartitionBind* obj = ObjectWrap::Unwrap<TopicPartitionBind>(info.Holder());
    info.GetReturnValue().Set(Nan::New<Number,int64_t>(obj->impl->offset()));
}

NAN_METHOD(TopicPartitionBind::SetOffset) {
    REQUIRE_ARGUMENTS(1);
    REQUIRE_ARGUMENT_NUMBER(0, offset);

    TopicPartitionBind* obj = ObjectWrap::Unwrap<TopicPartitionBind>(info.Holder());
    obj->impl->set_offset((int) offset);
}

NAN_METHOD(TopicPartitionBind::Err) {
    TopicPartitionBind* obj = ObjectWrap::Unwrap<TopicPartitionBind>(info.Holder());
    info.GetReturnValue().Set(Nan::New(obj->impl->err()));
}