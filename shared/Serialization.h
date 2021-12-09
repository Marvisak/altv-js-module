#pragma once

#include "v8.h"
#include "cpp-sdk/ICore.h"

namespace V8Helpers
{
    alt::MValue V8ToMValue(v8::Local<v8::Value> val, bool allowFunction = true);
    v8::Local<v8::Value> MValueToV8(alt::MValueConst val);
    void MValueArgsToV8(alt::MValueArgs args, std::vector<v8::Local<v8::Value>>& v8Args);

    alt::MValueByteArray V8ToRawBytes(v8::Local<v8::Value> val);
    v8::MaybeLocal<v8::Value> RawBytesToV8(alt::MValueByteArrayConst bytes);

    class SerializerDelegate : public v8::ValueSerializer::Delegate
    {
    public:
        void ThrowDataCloneError(v8::Local<v8::String> message) override;
    };
}  // namespace V8Helpers
