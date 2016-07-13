#ifndef CONF_H
#define CONF_H

#include <node.h>
#include <nan.h>

#include "rdkafkacpp.h"

#include "macros.h"

using namespace v8;

namespace ConfHelper {
    RdKafka::Conf* CreateTopicConfig(Local<Object> jsConfig);
    RdKafka::Conf* CreateConfig(Local<Object> jsConfig);
}

#endif