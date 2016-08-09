#include "ProducerBind.h"
#include "ConfHelper.h"
#include "macros.h"

#define POLLING_TIMEOUT 100

using namespace v8;

// Running in a background thread in order to call impl->poll from time to time.
// That makes all the delivery callbacks fire, so we can track the delivery of individual messages
void Poller(void* context) {
     ProducerBind* producerBind = static_cast<ProducerBind*> (context);
     while(producerBind->running) {
        producerBind->impl->poll(POLLING_TIMEOUT);
     }
}

// Deliver report callback - indicates that there're some new messages in the delivered queue
// The uv_async_t is notified after each message consumption, but libuv might coerce the callback
// execution.
// Called on event loop thread.
void DeliverReportCallback(uv_async_t* handle) {
    Isolate* isolate = Isolate::GetCurrent();
    HandleScope handleScope(isolate);

    ProducerBind* producerBind = static_cast<ProducerBind*> (handle->data);
    std::vector<DeliveryReport*>* deliveredMessages = producerBind->deliverReportQueue->pull();

    for(std::vector<DeliveryReport*>::iterator it = deliveredMessages->begin();
            it != deliveredMessages->end(); ++it) {
        DeliveryReport* report = *it;

        Local<Function> jsCallback = Nan::New(*report->callback);
        if (report->err == RdKafka::ErrorCode::ERR_NO_ERROR) {
            Local<Value> argv[] = {  Nan::Null(), Nan::New((uint32_t) report->offset) };
            Nan::MakeCallback(Nan::GetCurrentContext()->Global(), jsCallback, 2, argv);
        } else {
            // Got at error, return it as the first callback arg
            Local<Object> error = Nan::Error(report->errStr.c_str()).As<Object>();
            error->Set(Nan::New("code").ToLocalChecked(), Nan::New(report->err));
            Local<Value> argv[] = { error.As<Value>(), Nan::Null() };
            Nan::MakeCallback(Nan::GetCurrentContext()->Global(), jsCallback, 2, argv);
        }
        delete report;
    }
    delete deliveredMessages;
};

Nan::Persistent<Function> ProducerBind::constructor;

NAN_MODULE_INIT(ProducerBind::Init) {
    Nan::HandleScope scope;

    Local<FunctionTemplate> t = Nan::New<FunctionTemplate>(New);
    t->InstanceTemplate()->SetInternalFieldCount(5);
    t->SetClassName(Nan::New("Producer").ToLocalChecked());

    // Register all prototype methods
    Nan::SetPrototypeMethod(t, "produce", Produce);
    Nan::SetPrototypeMethod(t, "close", Close);

    constructor.Reset(t->GetFunction());
    Nan::Set(target, Nan::New("Producer").ToLocalChecked(),
        Nan::GetFunction(t).ToLocalChecked());
}

NAN_METHOD(ProducerBind::New) {
    if (!info.IsConstructCall()) {
        return Nan::ThrowError("Non-constructor invocation not supported");
    }

    REQUIRE_ARGUMENTS(1);
    REQUIRE_ARGUMENT_OBJECT(0, jsConf);

    ProducerBind* obj = new ProducerBind(ConfHelper::CreateConfig(jsConf));
    obj->Wrap(info.This());

    info.GetReturnValue().Set(info.This());
}

ProducerBind::ProducerBind(RdKafka::Conf* conf) : running(true) {
    std::string errstr;
    CONF_SET_PROPERTY(conf, "dr_cb", this);

    this->impl = RdKafka::Producer::create(conf, errstr);
    delete conf;
    if (!this->impl) {
        Nan::ThrowError(errstr.c_str());
    }

    this->deliverReportQueue = new Queue<DeliveryReport>(Blocking::NON_BLOCKING);

    uv_async_init(uv_default_loop(), &this->deliveryNotifier, &DeliverReportCallback);
    this->deliveryNotifier.data = this;

    uv_thread_create(&this->pollingThread, Poller, this);
}

ProducerBind::~ProducerBind() {
    delete this->impl;
    delete this->deliverReportQueue;
}

NAN_METHOD(ProducerBind::Produce) {
    std::string errstr;
    std::string* key;

    REQUIRE_ARGUMENTS(5);
    REQUIRE_ARGUMENT_STRING(0, topic_name);
    REQUIRE_ARGUMENT_NUMBER(1, partition);
    REQUIRE_ARGUMENT_STRING(2, payload);

    // Key is optional, so might be undefined
    if (info.Length() <= 3 || (!info[3]->IsString() && !info[3]->IsUndefined())) {
        return Nan::ThrowTypeError("Argument 4 must be a string or undefined");
    }


    if (info[3]->IsString()) {
        key = new std::string(*Nan::Utf8String(info[3]));
    } else {
        key = NULL;
    }

    REQUIRE_ARGUMENT_FUNCTION(4, jsCallback);

    ProducerBind* obj = ObjectWrap::Unwrap<ProducerBind>(info.Holder());

    // TODO: We don't want to recreate it all the time, take it as argument.
    // However the API is better/cleaner like this.
    RdKafka::Topic *topic = RdKafka::Topic::create(obj->impl, topic_name, NULL, errstr);
    if (!topic) {
        Nan::ThrowError(errstr.c_str());
    }

    // We've set up a delivery callback here, it will be passed together with the msg_opaque
    // and the pointer to it will be passed to the dr_cb delivery callback.
    // Then it will be transferred to the event loop together with the result and called.
    Nan::Persistent<Function>* persistentCallback = new Nan::Persistent<Function>(jsCallback);

    RdKafka::ErrorCode resp = obj->impl->produce(topic, partition,
    			  RdKafka::Producer::RK_MSG_COPY,
    			  const_cast<char *>(payload.c_str()), payload.size(),
    			  key, (void*) persistentCallback);
    delete topic;
    if (key != NULL) {
        delete key;
    }

    if (resp != RdKafka::ErrorCode::ERR_NO_ERROR) {
        Nan::ThrowError(RdKafka::err2str(resp).c_str());
    }
    obj->impl->poll(0);

    info.GetReturnValue().Set(Nan::Undefined());
}

NAN_METHOD(ProducerBind::Close) {
    ProducerBind* obj = ObjectWrap::Unwrap<ProducerBind>(info.Holder());
    obj->running = false;
    uv_thread_join(&obj->pollingThread);
    uv_close((uv_handle_t*) &obj->deliveryNotifier, NULL);
}

// Callback from librdkafka that's executed when producing is done
// This is called NOT ON THE EVENT THREAD
void ProducerBind::dr_cb(RdKafka::Message &message) {
    this->deliverReportQueue->push(new DeliveryReport(
        static_cast<Nan::Persistent<Function>*>(message.msg_opaque()),
        // Delivery report gives us the offset that's larger by one. // TODO report bug
        message.offset() - 1,
        message.err(),
        message.errstr()));
    uv_async_send(&this->deliveryNotifier);
}