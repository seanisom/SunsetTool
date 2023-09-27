// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

#pragma once

#include "MainWindow.g.h"

#include "MSFSMemory.h"
#include "Model.h"

namespace winrt::SunsetTool::implementation
{
    struct MainWindow : MainWindowT<MainWindow>
    {
        MainWindow();

        void myButton_Click(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& args);

        void button2_Click(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& args);

        void button3_Click(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& args);

        void button5_Click(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& args);

        void slider1_ValueChanged(Windows::Foundation::IInspectable const& sender, 
            Microsoft::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs const& args);

        void header1_TextChanged(Windows::Foundation::IInspectable const& sender,
            Microsoft::UI::Xaml::Controls::TextChangedEventArgs const& args);

        void slider2_ValueChanged(Windows::Foundation::IInspectable const& sender,
            Microsoft::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs const& args);

        void header2_TextChanged(Windows::Foundation::IInspectable const& sender,
            Microsoft::UI::Xaml::Controls::TextChangedEventArgs const& args);

        void slider3_ValueChanged(Windows::Foundation::IInspectable const& sender,
            Microsoft::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs const& args);

        void header3_TextChanged(Windows::Foundation::IInspectable const& sender,
            Microsoft::UI::Xaml::Controls::TextChangedEventArgs const& args);

        void slider4_ValueChanged(Windows::Foundation::IInspectable const& sender,
            Microsoft::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs const& args);

        void header4_TextChanged(Windows::Foundation::IInspectable const& sender,
            Microsoft::UI::Xaml::Controls::TextChangedEventArgs const& args);

        void slider5_ValueChanged(Windows::Foundation::IInspectable const& sender,
            Microsoft::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs const& args);

        void header5_TextChanged(Windows::Foundation::IInspectable const& sender,
            Microsoft::UI::Xaml::Controls::TextChangedEventArgs const& args);

        void slider6_ValueChanged(Windows::Foundation::IInspectable const& sender,
            Microsoft::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs const& args);

        void header6_TextChanged(Windows::Foundation::IInspectable const& sender,
            Microsoft::UI::Xaml::Controls::TextChangedEventArgs const& args);

    private:
        void ReloadModel();

        ::SunsetTool::Model m_model;
        ::SunsetTool::MSFSMemory m_memory;
    };
}

namespace winrt::SunsetTool::factory_implementation
{
    struct MainWindow : MainWindowT<MainWindow, implementation::MainWindow>
    {
    };
}
