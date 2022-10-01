#include "application.h"
#include <iostream>

#define WIDTH 1920
#define HEIGHT 1080

struct MemoryMetrics
{
    uint32_t total_allocated = 0;
    uint32_t total_freed = 0;
    uint32_t current_usage() const
    {
        return total_allocated - total_freed;
    }
};

static MemoryMetrics metrics;

void* operator new(size_t size)
{
    std::cout << "Allocating " << size << "bytes\n";
    metrics.total_allocated += size;

    return malloc(size);
}

void operator delete(void* memory, size_t size)
{
    std::cout << "Freeing " << size << "bytes\n";
    metrics.total_freed += size;
    free(memory);
}

void operator delete(void* memory)
{
    free(memory);
}

int main(int /*argc*/, char** /*argv*/)
{
    std::unique_ptr<Application> app = std::make_unique<Application>(WIDTH, HEIGHT, "Constantia");
    std::cout << "Startup Usage " << metrics.current_usage() << " bytes\n";
    app->run();
    return 0;
}