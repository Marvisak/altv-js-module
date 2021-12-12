#pragma once

#include "V8Helpers.h"
#include "V8ResourceImpl.h"

class V8Timer
{
public:
    V8Timer(v8::Isolate* _isolate, v8::Local<v8::Context> _context, int64_t curTime, v8::Local<v8::Function> _callback, uint32_t _interval, bool _once, V8Helpers::SourceLocation&& _location)
        : isolate(_isolate), context(_isolate, _context), lastRun(curTime), callback(_isolate, _callback), interval(_interval), once(_once), location(std::move(_location))
    {
        // Log::Debug << "Create timer: " << curTime << " " << interval << Log::Endl;
    }

    bool Update(int64_t curTime)
    {
        if(curTime - lastRun >= interval)
        {
            V8Helpers::TryCatch([&] {
                v8::MaybeLocal<v8::Value> result = callback.Get(isolate)->CallAsFunction(context.Get(isolate), v8::Undefined(isolate), 0, nullptr);
                return !result.IsEmpty();
            });

            lastRun = curTime;

            return !once;
        }

        return true;
    }

    const V8Helpers::SourceLocation& GetLocation() const
    {
        return location;
    }
    int64_t GetInterval()
    {
        return interval;
    }
    bool IsOnce()
    {
        return once;
    }

private:
    v8::Isolate* isolate;
    V8Helpers::CPersistent<v8::Context> context;
    V8Helpers::CPersistent<v8::Function> callback;
    int64_t interval;
    int64_t lastRun = 0;
    bool once;
    V8Helpers::SourceLocation location;
};
