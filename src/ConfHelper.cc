#include "ConfHelper.h"

using namespace v8;

RdKafka::Conf* ConfHelper::CreateTopicConfig(Local<Object> jsConfig) {
    std::string errstr;
    RdKafka::Conf *conf = RdKafka::Conf::create(RdKafka::Conf::CONF_TOPIC);
    Local<Array> keys = Nan::GetOwnPropertyNames(jsConfig).ToLocalChecked();
    for (size_t i = 0; i < keys->Length(); i++) {
        Local<Value> key = Nan::Get(keys, i).ToLocalChecked();
        Local<Value> val = Nan::Get(jsConfig, key).ToLocalChecked();
        if (val == Nan::Undefined()) {
            continue;
        }
        CONF_SET_PROPERTY(conf, std::string(*Nan::Utf8String(key)), std::string(*Nan::Utf8String(val)));
    }
    return conf;
}

RdKafka::Conf* ConfHelper::CreateConfig(Local<Object> jsConfig) {
    std::string errstr;
    RdKafka::Conf *conf = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);
    Local<Array> keys = Nan::GetOwnPropertyNames(jsConfig).ToLocalChecked();

    for (size_t i = 0; i < keys->Length(); i++) {
        Local<Value> key = Nan::Get(keys, i).ToLocalChecked();
        Local<Value> val = Nan::Get(jsConfig, key).ToLocalChecked();
        if (val == Nan::Undefined()) {
            continue;
        }
        std::string keyName(*Nan::Utf8String(key));
        if (keyName.compare("default_topic_conf") == 0) {
            RdKafka::Conf* tconf = CreateTopicConfig(Local<Object>::Cast(val));
            CONF_SET_PROPERTY(conf, "default_topic_conf", tconf);
            delete tconf;
        } else {
            CONF_SET_PROPERTY(conf, keyName, std::string(*Nan::Utf8String(val)));
        }
        // TODO: handle other possibile non-trivial cases like callbacks etc
    }
    return conf;
}