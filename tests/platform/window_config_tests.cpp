#include "brodo/platform/window.h"

#include <gtest/gtest.h>

#include <stdexcept>

namespace {

TEST(WindowConfig, UsesBrodoDefaults)
{
    const brodo::platform::WindowConfig config;

    EXPECT_EQ(config.title, L"Brodo");
    EXPECT_EQ(config.width, 1280);
    EXPECT_EQ(config.height, 720);
}

TEST(WindowConfig, AcceptsPositiveSize)
{
    const brodo::platform::WindowConfig config{L"Test Window", 640, 480};

    EXPECT_EQ(config.title, L"Test Window");
    EXPECT_EQ(config.width, 640);
    EXPECT_EQ(config.height, 480);
}

TEST(WindowConfig, RejectsNonPositiveWidth)
{
    EXPECT_THROW((brodo::platform::WindowConfig{L"Test Window", 0, 480}), std::runtime_error);
    EXPECT_THROW((brodo::platform::WindowConfig{L"Test Window", -1, 480}), std::runtime_error);
}

TEST(WindowConfig, RejectsNonPositiveHeight)
{
    EXPECT_THROW((brodo::platform::WindowConfig{L"Test Window", 640, 0}), std::runtime_error);
    EXPECT_THROW((brodo::platform::WindowConfig{L"Test Window", 640, -1}), std::runtime_error);
}

} // namespace
