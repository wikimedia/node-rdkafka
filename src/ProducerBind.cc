#include "ProducerBind.h"
#include "ConfHelper.h"
#include "macros.h"

#define POLLING_TIMEOUT 100

using namespace v8;

Nan::Persistent<Function> ProducerBind::constructor;

NAN_MODULE_INIT(ProducerBind::Init) {
    Nan::HandleScope scope;

    Local<FunctionTemplate> t = Nan::New<FunctionTemplate>(New);
    t->InstanceTemplate()->SetInternalFieldCount(1);
    t->SetClassName(Nan::New("Producer").ToLocalChecked());

    // Register all prototype methods
    Nan::SetPrototypeMethod(t, "produce", Produce);

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

ProducerBind::ProducerBind(RdKafka::Conf* conf) {
    std::string errstr;
    CONF_SET_PROPERTY(conf, "dr_cb", this);

    this->impl = RdKafka::Producer::create(conf, errstr);
    delete conf;
    // TODO: Proper Error handling
    if (!this->impl) {
        Nan::ThrowError(errstr.c_str());
    }

    this->deliverReportQueue = new Queue<RdKafka::Message>(false);

    uv_async_init(uv_default_loop(), &this->deliveryNotifier, &ProducerBind::DeliverReportCallback);
    this->deliveryNotifier.data = this;

    uv_thread_create(&this->pollingThread, ProducerBind::Poller, this);
}

ProducerBind::~ProducerBind() {
    delete this->impl;
    delete this->deliverReportQueue;

    uv_close((uv_handle_t*) &this->deliveryNotifier, NULL);
}

NAN_METHOD(ProducerBind::Produce) {
    std::string errstr;

    REQUIRE_ARGUMENTS(2);
    REQUIRE_ARGUMENT_STRING(0, topic_name);
    REQUIRE_ARGUMENT_STRING(1, payload);

    ProducerBind* obj = ObjectWrap::Unwrap<ProducerBind>(info.Holder());

    // TODO: We don't want to recreate it all the time, take it as argument. However the API is better/cleaner like this.
    RdKafka::Topic *topic = RdKafka::Topic::create(obj->impl, topic_name, NULL, errstr);
    if (!topic) {
        Nan::ThrowError(errstr.c_str());
    }

    RdKafka::ErrorCode resp = obj->impl->produce(topic, 0,
    			  RdKafka::Producer::RK_MSG_COPY /* TODO: think about it Copy payload */,
    			  const_cast<char *>(payload.c_str()), payload.size(),
    			  NULL, NULL);

    if (resp != RdKafka::ErrorCode::ERR_NO_ERROR) {
        printf(" %d ", resp);
    }
    info.GetReturnValue().Set(Nan::Undefined());
}

// Callback from librdkafka that's executed when producing is done
// This is called NOT ON THE EVENT THREAD
void ProducerBind::dr_cb(RdKafka::Message &message) {
    // TODO: send back the pointers to the actual payload, not the complete message
    printf("RECEIVE %p\n", message.payload());
    this->deliverReportQueue->push(&message);
    uv_async_send(&this->deliveryNotifier);
    // TODO: set up a map of Message->JSCallback, set up a vector for finished messages,
    // set up a uv_async_t so that we could notify eventloop when something was delivered and call the JS callback.
}

// Running in a background thread in order to call impl->poll from time to time.
// That makes all the delivery callbacks fire, so we can track the delivery of individual messages
void ProducerBind::Poller(void* context) {
     ProducerBind* producerBind = static_cast<ProducerBind*> (context);
     // TODO: we need a way to stop this
     while(true) {
        producerBind->impl->poll(POLLING_TIMEOUT);
     }
}

// Deliver report callback - indicates that there're some new messages in the delivered queue
// The uv_async_t is notified after each message consumption, but libuv might coerce the callback
// execution.
// Called on event loop thread.
void ProducerBind::DeliverReportCallback(uv_async_t* handle) {
    Isolate* isolate = Isolate::GetCurrent();
    HandleScope handleScope(isolate);

    ProducerBind* producerBind = static_cast<ProducerBind*> (handle->data);
    std::vector<RdKafka::Message*>* deliveredMessages = producerBind->deliverReportQueue->pull();


    for(std::vector<RdKafka::Message*>::iterator it = deliveredMessages->begin(); it != deliveredMessages->end(); ++it) {
        RdKafka::Message* message = *it;
      //  printf("DELIVERED %d\n", message->err()); fflush(stdout);
        //printf("DELIVERED %d\n", (int) message->offset()); fflush(stdout);

        /*Local<Function> jsCallback = Nan::New(*result->callback);
        if (result->message->err() == RdKafka::ErrorCode::ERR_NO_ERROR) {
            Local<Value> argv[] = { Nan::Null(), MessageBind::FromImpl(result->message) };
            Nan::MakeCallback(Nan::GetCurrentContext()->Global(), jsCallback, 2, argv);
        } else {
            // Got at error, return it as the first callback arg
            Local<Object> error = Nan::Error(result->message->errstr().c_str()).As<Object>();
            error->Set(Nan::New("code").ToLocalChecked(), Nan::New(result->message->err()));
            Local<Value> argv[] = { error.As<Value>(), Nan::Null() };
            Nan::MakeCallback(Nan::GetCurrentContext()->Global(), jsCallback, 2, argv);
        }*/
        // Don't delete the Message since here we're not the owners of it, it will be handled by librdkafka
    }
    delete deliveredMessages;
};