#include "TopicPartitionBind.h"
#include "macros.h"

using namespace v8;

Nan::Persistent<Function> TopicPartitionBind::constructor;

NAN_MODULE_INIT(TopicPartitionBind::Init) {
    Nan::HandleScope scope;

    Local<FunctionTemplate> t = Nan::New<FunctionTemplate>(New);
    t->InstanceTemplate()->SetInternalFieldCount(1);
    t->SetClassName(Nan::New("TopicPartition").ToLocalChecked());

    // Register all getters and setters for the properties
    Nan::SetAccessor(t->InstanceTemplate(), Nan::New("topic").ToLocalChecked(), Topic);
    Nan::SetAccessor(t->InstanceTemplate(), Nan::New("partition").ToLocalChecked(), Partition);
    Nan::SetAccessor(t->InstanceTemplate(), Nan::New("offset").ToLocalChecked(), Offset, SetOffset);
    Nan::SetAccessor(t->InstanceTemplate(), Nan::New("err").ToLocalChecked(), Err);

    constructor.Reset(t->GetFunction());

    Nan::Set(target, Nan::New("TopicPartition").ToLocalChecked(),
        Nan::GetFunction(t).ToLocalChecked());
}

NAN_METHOD(TopicPartitionBind::New) {
    if (!info.IsConstructCall()) {
        return Nan::ThrowError("Non-constructor invocation not supported");
    }

    TopicPartitionBind* obj;
    if (info.Length() == 1) {
        REQUIRE_ARGUMENT_EXTERNAL(0, impl, RdKafka::TopicPartition*);
        obj = new TopicPartitionBind(impl);
    } else {
        REQUIRE_ARGUMENTS(2);
        REQUIRE_ARGUMENT_STRING(0, topic);
        REQUIRE_ARGUMENT_NUMBER(1, partition);
        obj = new TopicPartitionBind(topic, (int)partition);
    }

    obj->Wrap(info.This());
    info.GetReturnValue().Set(info.This());
}

Local<Object> TopicPartitionBind::FromImpl(RdKafka::TopicPartition* impl) {
    int argc = 1;
    Local<Value> argv[] = {
        External::New(Isolate::GetCurrent(), impl)
    };
    return Nan::NewInstance(Nan::New(TopicPartitionBind::constructor), argc, argv).ToLocalChecked();
}

TopicPartitionBind::TopicPartitionBind(std::string topic, int partition) {
    this->impl = RdKafka::TopicPartition::create(topic, partition);
}

TopicPartitionBind::TopicPartitionBind(RdKafka::TopicPartition* impl) {
    this->impl = impl;
}

TopicPartitionBind::~TopicPartitionBind() {
    delete this->impl;
};

NAN_GETTER(TopicPartitionBind::Topic) {
    TopicPartitionBind* obj = ObjectWrap::Unwrap<TopicPartitionBind>(info.Holder());
    info.GetReturnValue().Set(Nan::New(obj->impl->topic()).ToLocalChecked());
}

NAN_GETTER(TopicPartitionBind::Partition) {
    TopicPartitionBind* obj = ObjectWrap::Unwrap<TopicPartitionBind>(info.Holder());
    info.GetReturnValue().Set(Nan::New(obj->impl->partition()));
}

NAN_GETTER(TopicPartitionBind::Offset) {
    TopicPartitionBind* obj = ObjectWrap::Unwrap<TopicPartitionBind>(info.Holder());
    info.GetReturnValue().Set(Nan::New<Number,int64_t>(obj->impl->offset()));
}

NAN_SETTER(TopicPartitionBind::SetOffset) {
    TopicPartitionBind* obj = ObjectWrap::Unwrap<TopicPartitionBind>(info.Holder());
    obj->impl->set_offset(value->Int32Value());
}

NAN_GETTER(TopicPartitionBind::Err) {
    TopicPartitionBind* obj = ObjectWrap::Unwrap<TopicPartitionBind>(info.Holder());
    info.GetReturnValue().Set(Nan::New(obj->impl->err()));
}