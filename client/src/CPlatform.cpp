#include "CPlatform.h"
#include "Log.h"
#include "CV8ScriptRuntime.h"
#include "cpp-sdk/ICore.h"

#include <chrono>

CPlatform::CPlatform() : defaultPlatform(v8::platform::NewDefaultPlatform()) {}

int CPlatform::NumberOfWorkerThreads()
{
    return defaultPlatform->NumberOfWorkerThreads();
}

std::shared_ptr<v8::TaskRunner> CPlatform::GetForegroundTaskRunner(v8::Isolate* isolate)
{
    return defaultPlatform->GetForegroundTaskRunner(isolate);
}

void CPlatform::CallOnWorkerThread(std::unique_ptr<v8::Task> task)
{
    defaultPlatform->CallOnWorkerThread(std::move(task));
}

void CPlatform::CallDelayedOnWorkerThread(std::unique_ptr<v8::Task> task, double delay_in_seconds)
{
    defaultPlatform->CallDelayedOnWorkerThread(std::move(task), delay_in_seconds);
}

bool CPlatform::IdleTasksEnabled(v8::Isolate* isolate)
{
    return defaultPlatform->IdleTasksEnabled(isolate);
}

std::unique_ptr<v8::JobHandle> CPlatform::PostJob(v8::TaskPriority priority, std::unique_ptr<v8::JobTask> job_state)
{
    return defaultPlatform->PostJob(priority, std::move(job_state));
}

double CPlatform::MonotonicallyIncreasingTime()
{
    return defaultPlatform->MonotonicallyIncreasingTime();
}

double CPlatform::CurrentClockTimeMillis()
{
    return defaultPlatform->CurrentClockTimeMillis();
}

v8::TracingController* CPlatform::GetTracingController()
{
    return defaultPlatform->GetTracingController();
}

v8::Platform::StackTracePrinter CPlatform::GetStackTracePrinter()
{
    return defaultPlatform->GetStackTracePrinter();
}

v8::PageAllocator* CPlatform::GetPageAllocator()
{
    return defaultPlatform->GetPageAllocator();
}

// * Custom overwrites

v8::ZoneBackingAllocator* CPlatform::GetZoneBackingAllocator()
{
    return CZoneBackingAllocator::Instance();
}

void CPlatform::DumpWithoutCrashing()
{
    Log::Error << "[V8] V8 crashed, printing current stack trace: " << Log::Endl;
    GetStackTracePrinter()();
}

void* debugMalloc(size_t size)
{
    void* ptr = malloc(size);
    CZoneBackingAllocator::Instance()->PushAllocation(ptr, size);
    return ptr;
}

void debugFree(void* ptr)
{
    CZoneBackingAllocator::Instance()->PushDeallocation(ptr);
    std::free(ptr);
}

CZoneBackingAllocator::MallocFn CZoneBackingAllocator::GetMallocFn() const
{
    if(isDebug) return debugMalloc;
    else
        return std::malloc;
}

CZoneBackingAllocator::FreeFn CZoneBackingAllocator::GetFreeFn() const
{
    if(isDebug) return debugFree;
    else
        return std::free;
}

void CZoneBackingAllocator::PushAllocation(void* ptr, size_t size)
{
    allocations.push_back({ size, ptr });
}

void CZoneBackingAllocator::PushDeallocation(void* ptr)
{
    deallocations.push_back({ ptr });
}

static std::string CurrentDateTime()
{
    time_t now = time(0);
    tm tstruct;
    char buf[80];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);

    return buf;
}
// todo: write this in a proper format
void CZoneBackingAllocator::WriteDebugInfoToFile()
{
    std::fstream stream((alt::ICore::Instance().GetClientPath() + "/v8_debug_info_" + CurrentDateTime() + ".out").CStr());
    if(!stream.good())
    {
        Log::Error << "[V8] Failed to write debug info to file" << Log::Endl;
        return;
    }

    stream << "Allocations: \n";
    for(auto& allocation : allocations)
    {
        stream << std::dec << allocation.size << " | " << std::hex << allocation.ptr << "\n";
    }
    stream << "Deallocations: \n";
    for(auto& deallocation : deallocations)
    {
        stream << std::hex << deallocation.ptr << "\n";
    }

    stream.close();
}

CZoneBackingAllocator* CZoneBackingAllocator::Instance()
{
    static CZoneBackingAllocator zoneBackingAllocator(alt::ICore::Instance().IsDebug());
    return &zoneBackingAllocator;
}
