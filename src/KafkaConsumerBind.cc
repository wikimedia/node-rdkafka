#include "KafkaConsumerBind.h"
#include "MessageBind.h"
#include "macros.h"

using namespace v8;

Nan::Persistent<Function> KafkaConsumerBind::constructor;

NAN_MODULE_INIT(KafkaConsumerBind::Init) {
    Nan::HandleScope scope;

    Local<FunctionTemplate> t = Nan::New<FunctionTemplate>(New);
    t->InstanceTemplate()->SetInternalFieldCount(1);
    t->SetClassName(Nan::New("KafkaConsumer").ToLocalChecked());

    // Register all prototype methods
    Nan::SetPrototypeMethod(t, "consume", Consume);
    Nan::SetPrototypeMethod(t, "subscribe", Subscribe);

    constructor.Reset(t->GetFunction());

    Nan::Set(target, Nan::New("KafkaConsumer").ToLocalChecked(),
        Nan::GetFunction(t).ToLocalChecked());
}

NAN_METHOD(KafkaConsumerBind::New) {
    if (!info.IsConstructCall()) {
        return Nan::ThrowError("Non-constructor invocation not supported");
    }

    KafkaConsumerBind* obj = new KafkaConsumerBind();

    obj->Wrap(info.This());
    info.GetReturnValue().Set(info.This());
}

KafkaConsumerBind::KafkaConsumerBind() {
    // TODO: Handle configuration and errors
    std::string errstr;
    RdKafka::Conf *conf = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);
    if (conf->set("group.id", "test_test_test1", errstr) != RdKafka::Conf::CONF_OK) {
        Nan::ThrowError(errstr.c_str());
    }
    if (conf->set("metadata.broker.list", "127.0.0.1:9092", errstr) != RdKafka::Conf::CONF_OK) {
        Nan::ThrowError(errstr.c_str());
    };

    this->impl = RdKafka::KafkaConsumer::create(conf, errstr);
    // TODO: Proper Error handling
    if (!this->impl) {
        Nan::ThrowError(errstr.c_str());
    }
}

KafkaConsumerBind::~KafkaConsumerBind() {};

NAN_METHOD(KafkaConsumerBind::Consume) {
    REQUIRE_ARGUMENTS(1);
    REQUIRE_ARGUMENT_NUMBER(0, timeout);

    KafkaConsumerBind* obj = ObjectWrap::Unwrap<KafkaConsumerBind>(info.Holder());
    RdKafka::Message* result = obj->impl->consume(timeout);
    info.GetReturnValue().Set(MessageBind::FromImpl(result));
}

NAN_METHOD(KafkaConsumerBind::Subscribe) {
    REQUIRE_ARGUMENTS(1);

    // TODO: Find a more efficient way to cas the JS array to std::vector
    Local<Array> jsArray = Local<Array>::Cast(info[0]);
    std::vector<std::string> topics(jsArray->Length());
    for (int i = 0; i < jsArray->Length(); i++) {
        topics[i] = std::string(*Nan::Utf8String(jsArray->Get(i)));
    }

    KafkaConsumerBind* obj = ObjectWrap::Unwrap<KafkaConsumerBind>(info.Holder());
    int result = obj->impl->subscribe( topics );
    printf(" %d ", result);
}