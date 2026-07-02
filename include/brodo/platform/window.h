#pragma once

#if !defined(_WIN32)
#error "brodo::platform::Window is currently only supported on Windows."
#endif

#include <Windows.h>

#include <string>

namespace brodo::platform {

struct WindowMessageAccess;

struct WindowConfig {
    std::wstring title = L"Brodo";
    int width = 1280;
    int height = 720;

    WindowConfig();
    WindowConfig(std::wstring title, int width, int height);
};

class Window {
public:
    Window();
    explicit Window(const WindowConfig& config);
    ~Window();

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    Window(Window&& other) noexcept;
    Window& operator=(Window&& other) noexcept;

    void pollEvents();
    bool shouldClose() const noexcept;
    HWND nativeHandle() const noexcept;

private:
    friend struct WindowMessageAccess;

    void requestClose() noexcept;

    HWND hwnd_ = nullptr;
    bool should_close_ = false;
};

} // namespace brodo::platform
