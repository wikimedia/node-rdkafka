#include "MessageBind.h"
#include "macros.h"

using namespace v8;

Nan::Persistent<Function> MessageBind::constructor;

NAN_MODULE_INIT(MessageBind::Init) {
    Nan::HandleScope scope;

    Local<FunctionTemplate> t = Nan::New<FunctionTemplate>(New);
    t->InstanceTemplate()->SetInternalFieldCount(6);
    t->SetClassName(Nan::New("Message").ToLocalChecked());

    // Register getters for all the properties
    Nan::SetAccessor(t->InstanceTemplate(), Nan::New("topicName").ToLocalChecked(), TopicName);
    Nan::SetAccessor(t->InstanceTemplate(), Nan::New("partition").ToLocalChecked(), Partition);
    Nan::SetAccessor(t->InstanceTemplate(), Nan::New("payload").ToLocalChecked(), Payload);
    Nan::SetAccessor(t->InstanceTemplate(), Nan::New("key").ToLocalChecked(), Key);
    Nan::SetAccessor(t->InstanceTemplate(), Nan::New("offset").ToLocalChecked(), Offset);

    constructor.Reset(t->GetFunction());
}

NAN_METHOD(MessageBind::New) {
    if (!info.IsConstructCall()) {
        return Nan::ThrowError("Non-constructor invocation not supported");
    }

    REQUIRE_ARGUMENTS(1);
    REQUIRE_ARGUMENT_EXTERNAL(0, impl, MessageResult*);
    MessageBind* obj = new MessageBind(impl);
    obj->Wrap(info.This());

    info.GetReturnValue().Set(info.This());
}

Local<Object> MessageBind::FromImpl(MessageResult* impl) {
    int argc = 1;
    Local<Value> argv[] = { External::New(Isolate::GetCurrent(), impl) };
    return Nan::NewInstance(Nan::New(MessageBind::constructor), argc, argv).ToLocalChecked();
}

MessageBind::MessageBind(MessageResult* impl) :
    payload(impl->payload),
    len(impl->len),
    topic(impl->topic),
    partition(impl->partition),
    offset(impl->offset),
    key(impl->key),
    buffer(NULL) { };
MessageBind::~MessageBind() {
    if (this->buffer == NULL) {
        // We didn't transfer ownership to JS, delete payload
        free(this->payload);
    } else {
        // Don't delete the payload here - it's passed to the v8 Buffer
        // without making a copy, so the memory is handled by v8 GC.
        this->buffer->Reset();
        delete this->buffer;
    }
};

NAN_GETTER(MessageBind::TopicName) {
   MessageBind* obj = ObjectWrap::Unwrap<MessageBind>(info.Holder());
   info.GetReturnValue().Set(Nan::New(obj->topic).ToLocalChecked());
}

NAN_GETTER(MessageBind::Partition) {
   MessageBind* obj = ObjectWrap::Unwrap<MessageBind>(info.Holder());
   info.GetReturnValue().Set(Nan::New(obj->partition));
}

NAN_GETTER(MessageBind::Payload) {
    MessageBind* obj = ObjectWrap::Unwrap<MessageBind>(info.Holder());
    if (obj->buffer == NULL) {
        obj->buffer = new Nan::Persistent<Object>(Nan::NewBuffer((char*) obj->payload,
            (uint32_t) obj->len).ToLocalChecked());
    }
    info.GetReturnValue().Set(Nan::New(*obj->buffer));
}

NAN_GETTER(MessageBind::Key) {
   MessageBind* obj = ObjectWrap::Unwrap<MessageBind>(info.Holder());
   if (obj->key != NULL) {
        info.GetReturnValue().Set(Nan::New(*obj->key).ToLocalChecked());
   } else {
        info.GetReturnValue().Set(Nan::Undefined());
   }
}

NAN_GETTER(MessageBind::Offset) {
   MessageBind* obj = ObjectWrap::Unwrap<MessageBind>(info.Holder());
   info.GetReturnValue().Set(Nan::New((double) obj->offset));
}