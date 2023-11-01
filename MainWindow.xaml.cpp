// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

#include "pch.h"
#include "MainWindow.xaml.h"
#if __has_include("MainWindow.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "MainWindow.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

#include <winrt/Microsoft.UI.Xaml.h>
#include <winrt/Microsoft.UI.Windowing.h>

#include <string>


using namespace winrt;
using namespace winrt::Windows::System::Threading;
using namespace Microsoft::UI::Xaml;
using namespace Windows::Foundation;

using namespace ::SunsetTool;


namespace winrt::SunsetTool::implementation
{
    MainWindow::MainWindow() : m_model(Model::Default())
    {
        InitializeComponent();
        AppWindow().Resize({ 700, 700 });
        const auto presenter = AppWindow().Presenter().as <
            Microsoft::UI::Windowing::OverlappedPresenter>();
        presenter.IsResizable(false);
        AppWindow().Title(L"Sunset Tool");
        Preset().SelectedIndex(0);
    }

    void MainWindow::myButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        m_memory.Load();

        m_memory.ReadMemory(m_model);

        myButton().Content(box_value(L"Loaded!"));
        myButton().IsEnabled(false);

        ReloadModel();
    }

    void MainWindow::button2_Click(Windows::Foundation::IInspectable const&, RoutedEventArgs const&)
    {
        m_model = Model::Default();

        ReloadModel();
    }

    void MainWindow::button3_Click(Windows::Foundation::IInspectable const&, RoutedEventArgs const&)
    {
        shareCode().Text(to_hstring(m_model.ToBase64()));
    }

    void MainWindow::button5_Click(Windows::Foundation::IInspectable const&, RoutedEventArgs const&)
    {
	    if (m_model.FromBase64(to_string(importCode().Text())))
        {
            importCode().Text(L"");
            flyout2().Hide();
            ReloadModel();
        }
    }

    void MainWindow::slider1_ValueChanged(Windows::Foundation::IInspectable const&, 
                                          Controls::Primitives::RangeBaseValueChangedEventArgs const& args)
    {
	    if (const auto new_val = static_cast<float>(args.NewValue()); m_model.AbsorbR() != new_val)
        {
            header1().Text(std::to_wstring(new_val));
        }
    }

    void MainWindow::header1_TextChanged(Windows::Foundation::IInspectable const&, Controls::TextChangedEventArgs const&)
    {
        try
        {
            m_model.AbsorbR(std::stof(to_string(header1().Text())));

            m_memory.WriteMemory(m_model);

            slider1().Value(m_model.AbsorbR());
        }
        catch (...) {};
    }

    void MainWindow::slider2_ValueChanged(Windows::Foundation::IInspectable const&, Controls::Primitives::RangeBaseValueChangedEventArgs const& args)
    {
	    if (const auto new_val = static_cast<float>(args.NewValue()); m_model.AbsorbG() != new_val)
        {
            header2().Text(std::to_wstring(new_val));
        }
    }

    void MainWindow::header2_TextChanged(Windows::Foundation::IInspectable const&, Controls::TextChangedEventArgs const&)
    {
        try
        {
            m_model.AbsorbG(std::stof(to_string(header2().Text())));

            m_memory.WriteMemory(m_model);

            slider2().Value(m_model.AbsorbG());
        }
        catch (...) {};
    }

    void MainWindow::slider3_ValueChanged(Windows::Foundation::IInspectable const&, Controls::Primitives::RangeBaseValueChangedEventArgs const& args)
    {
	    if (const auto new_val = static_cast<float>(args.NewValue()); m_model.AbsorbB() != new_val)
        {
            header3().Text(std::to_wstring(new_val));
        }
    }

    void MainWindow::header3_TextChanged(Windows::Foundation::IInspectable const&, Controls::TextChangedEventArgs const&)
    {
        try
        {
            m_model.AbsorbB(std::stof(to_string(header3().Text())));

            m_memory.WriteMemory(m_model);

            slider3().Value(m_model.AbsorbB());
        }
        catch (...) {};
    }

    void MainWindow::slider4_ValueChanged(Windows::Foundation::IInspectable const&, Controls::Primitives::RangeBaseValueChangedEventArgs const& args)
    {
	    if (const auto new_val = static_cast<float>(args.NewValue()); m_model.Aerosols() != new_val)
        {
            header4().Text(std::to_wstring(new_val));
        }
    }

    void MainWindow::header4_TextChanged(Windows::Foundation::IInspectable const&, Controls::TextChangedEventArgs const&)
    {
        try
        {
            m_model.Aerosols(std::stof(to_string(header4().Text())));

            m_memory.WriteMemory(m_model);

            slider4().Value(m_model.Aerosols());
        }
        catch (...) {};
    }

    void MainWindow::slider5_ValueChanged(Windows::Foundation::IInspectable const&, Controls::Primitives::RangeBaseValueChangedEventArgs const& args)
    {
	    if (const auto new_val = static_cast<float>(args.NewValue()); m_model.Color() != new_val)
        {
            header5().Text(std::to_wstring(new_val));
        }
    }

    void MainWindow::header5_TextChanged(Windows::Foundation::IInspectable const&, Controls::TextChangedEventArgs const&)
    {
        try
        {
            m_model.Color(std::stof(to_string(header5().Text())));

            m_memory.WriteMemory(m_model);

            slider5().Value(m_model.Color());
        }
        catch (...) {};
    }

    void MainWindow::slider6_ValueChanged(Windows::Foundation::IInspectable const&, Controls::Primitives::RangeBaseValueChangedEventArgs const& args)
    {
	    if (const auto new_val = static_cast<float>(args.NewValue()); m_model.Sun() != new_val)
        {
            header6().Text(std::to_wstring(new_val));
        }
    }

    void MainWindow::header6_TextChanged(Windows::Foundation::IInspectable const&, Controls::TextChangedEventArgs const&)
    {
        try
        {
            m_model.Sun(std::stof(to_string(header6().Text())));

            m_memory.WriteMemory(m_model);

            slider6().Value(m_model.Sun());
        }
        catch (...) {};
    }

    void MainWindow::Preset_SelectionChanged(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::Controls::SelectionChangedEventArgs const& args)
    {
        const auto index = Preset().SelectedIndex();
        if (index == -1)
        {
	        const auto item = Preset().SelectedItem();
            Preset().Items().Append(item);
            Preset().SelectedIndex(Preset().Items().Size() - 1);
        }
        else
        {
	        // Load the item into m_model
        }
    }

    fire_and_forget MainWindow::button6_Click(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& args)
    {
        const auto index = Preset().SelectedIndex();
        if (index == 0)
        {
            const Controls::ContentDialog dialog{};
            dialog.XamlRoot(rootPanel().XamlRoot());
            dialog.Title(box_value(L"Error"));
            dialog.Content(box_value(L"You cannot edit the Default preset!\n" \
									 L"Please type a new preset name in the box."));
            dialog.CloseButtonText(L"Ok");
            co_await dialog.ShowAsync();
        }

        // TODO - programmatic path
        m_model.SaveToINI(L"C:\\SunsetTool.ini");
    }

    fire_and_forget MainWindow::button7_Click(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& args)
    {
	    const auto index = Preset().SelectedIndex();
        if (index != 0)
        {
            Preset().SelectedIndex(0);
            Preset().Items().RemoveAt(index);
        }
        else
        {
            // https://stackoverflow.com/questions/68122223/how-can-you-co-await-in-a-c-winrt-lambda
            const Controls::ContentDialog dialog{};
            dialog.XamlRoot(rootPanel().XamlRoot());
            dialog.Title(box_value(L"Error"));
            dialog.Content(box_value(L"You cannot delete the Default sim values!"));
            dialog.CloseButtonText(L"Ok");
            co_await dialog.ShowAsync();
        }
    }

    void MainWindow::ReloadModel()
    {
        header1().Text(std::to_wstring(m_model.AbsorbR()));
        header2().Text(std::to_wstring(m_model.AbsorbG()));
        header3().Text(std::to_wstring(m_model.AbsorbB()));
        header4().Text(std::to_wstring(m_model.Aerosols()));
        header5().Text(std::to_wstring(m_model.Color()));
        header6().Text(std::to_wstring(m_model.Sun()));
    }
}
