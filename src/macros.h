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
    Nan::Utf8String var(info[i]);

#define REQUIRE_ARGUMENT_NUMBER(i, var)                                        \
    if (info.Length() <= (i) || !info[i]->IsNumber()) {                        \
        return Nan::ThrowTypeError("Argument " #i " must be a number");        \
    }                                                                          \
    double var = info[i]->NumberValue();

#endif