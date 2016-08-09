#ifndef MACROS_H
#define MACROS_H

#define REQUIRE_ARGUMENTS(n)                                                   \
    if (info.Length() < (n)) {                                                 \
        return Nan::ThrowTypeError("Expected " #n " arguments");               \
    }

#define REQUIRE_ARGUMENT_FUNCTION(i, var)                                      \
    if (info.Length() <= (i) || !info[i]->IsFunction()) {                      \
        return Nan::ThrowTypeError("Argument " #i " must be a function");      \
    }                                                                          \
    Local<Function> var = Local<Function>::Cast(info[i]);

#define REQUIRE_ARGUMENT_STRING(i, var)                                        \
    if (info.Length() <= (i) || !info[i]->IsString()) {                        \
        return Nan::ThrowTypeError("Argument " #i " must be a string");        \
    }                                                                          \
    std::string var(*Nan::Utf8String(info[i]));

#define REQUIRE_ARGUMENT_NUMBER(i, var)                                        \
    if (info.Length() <= (i) || !info[i]->IsNumber()) {                        \
        return Nan::ThrowTypeError("Argument " #i " must be a number");        \
    }                                                                          \
    double var = info[i]->NumberValue();

#define REQUIRE_ARGUMENT_EXTERNAL(i, var, TYPE)                                \
    if (info.Length() <= (i) || !info[i]->IsExternal()) {                      \
        return Nan::ThrowTypeError("Argument " #i " must be a number");        \
    }                                                                          \
    TYPE var = static_cast<TYPE>(External::Cast(*info[i])->Value());

#define REQUIRE_ARGUMENT_OBJECT(i, var)                                        \
    if (info.Length() <= (i) || !info[i]->IsObject()) {                        \
        return Nan::ThrowTypeError("Argument " #i " must be an object");       \
    }                                                                          \
    Local<Object> var = Local<Object>::Cast(info[i]);

#define CONF_SET_PROPERTY(conf, key, value)                                    \
    if (conf->set(key, value, errstr) != RdKafka::Conf::CONF_OK) {             \
        Nan::ThrowError(errstr.c_str());                                       \
    }

#endif