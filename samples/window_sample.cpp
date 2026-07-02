#include "brodo/platform/window.h"

#include <chrono>
#include <thread>

int main()
{
    brodo::platform::Window window;

    while (!window.shouldClose()) {
        window.pollEvents();
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }

    return 0;
}
