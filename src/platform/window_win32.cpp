#include "brodo/platform/window.h"

#include <stdexcept>
#include <utility>

namespace brodo::platform {

struct WindowMessageAccess {
    static void requestClose(Window& window) noexcept
    {
        window.requestClose();
    }
};

namespace {

constexpr wchar_t kWindowClassName[] = L"BrodoPlatformWindow";

void validateWindowConfig(const WindowConfig& config)
{
    if (config.width <= 0) {
        throw std::runtime_error("Window width must be positive.");
    }

    if (config.height <= 0) {
        throw std::runtime_error("Window height must be positive.");
    }
}

LRESULT CALLBACK windowProcedure(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
    Window* window = nullptr;

    if (message == WM_NCCREATE) {
        const auto* create_struct = reinterpret_cast<CREATESTRUCTW*>(lparam);
        window = static_cast<Window*>(create_struct->lpCreateParams);
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(window));
    } else {
        window = reinterpret_cast<Window*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
    }

    if (window != nullptr && message == WM_CLOSE) {
        WindowMessageAccess::requestClose(*window);
        return 0;
    }

    return DefWindowProcW(hwnd, message, wparam, lparam);
}

void registerWindowClass()
{
    WNDCLASSEXW window_class{};
    window_class.cbSize = sizeof(WNDCLASSEXW);
    window_class.style = CS_HREDRAW | CS_VREDRAW;
    window_class.lpfnWndProc = windowProcedure;
    window_class.hInstance = GetModuleHandleW(nullptr);
    window_class.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    window_class.lpszClassName = kWindowClassName;

    if (RegisterClassExW(&window_class) == 0) {
        const DWORD error = GetLastError();
        if (error != ERROR_CLASS_ALREADY_EXISTS) {
            throw std::runtime_error("Failed to register Win32 window class.");
        }
    }
}

} // namespace

WindowConfig::WindowConfig() = default;

WindowConfig::WindowConfig(std::wstring title, int width, int height)
    : title(std::move(title))
    , width(width)
    , height(height)
{
    validateWindowConfig(*this);
}

Window::Window()
    : Window(WindowConfig{})
{
}

Window::Window(const WindowConfig& config)
{
    validateWindowConfig(config);
    registerWindowClass();

    const DWORD style = WS_OVERLAPPEDWINDOW;
    RECT window_rect{0, 0, config.width, config.height};
    if (AdjustWindowRectEx(&window_rect, style, FALSE, 0) == 0) {
        throw std::runtime_error("Failed to calculate Win32 window size.");
    }

    hwnd_ = CreateWindowExW(
        0,
        kWindowClassName,
        config.title.c_str(),
        style,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        window_rect.right - window_rect.left,
        window_rect.bottom - window_rect.top,
        nullptr,
        nullptr,
        GetModuleHandleW(nullptr),
        this);

    if (hwnd_ == nullptr) {
        throw std::runtime_error("Failed to create Win32 window.");
    }

    ShowWindow(hwnd_, SW_SHOW);
    UpdateWindow(hwnd_);
}

Window::~Window()
{
    if (hwnd_ != nullptr) {
        SetWindowLongPtrW(hwnd_, GWLP_USERDATA, 0);
        DestroyWindow(hwnd_);
    }
}

Window::Window(Window&& other) noexcept
    : hwnd_(std::exchange(other.hwnd_, nullptr))
    , should_close_(std::exchange(other.should_close_, false))
{
    if (hwnd_ != nullptr) {
        SetWindowLongPtrW(hwnd_, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
    }
}

Window& Window::operator=(Window&& other) noexcept
{
    if (this == &other) {
        return *this;
    }

    if (hwnd_ != nullptr) {
        SetWindowLongPtrW(hwnd_, GWLP_USERDATA, 0);
        DestroyWindow(hwnd_);
    }

    hwnd_ = std::exchange(other.hwnd_, nullptr);
    should_close_ = std::exchange(other.should_close_, false);

    if (hwnd_ != nullptr) {
        SetWindowLongPtrW(hwnd_, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
    }

    return *this;
}

void Window::pollEvents()
{
    MSG message{};
    while (PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE) != 0) {
        TranslateMessage(&message);
        DispatchMessageW(&message);
    }
}

bool Window::shouldClose() const noexcept
{
    return should_close_;
}

HWND Window::nativeHandle() const noexcept
{
    return hwnd_;
}

void Window::requestClose() noexcept
{
    should_close_ = true;
}

} // namespace brodo::platform
