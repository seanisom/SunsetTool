// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

#include "pch.h"
#include "MainWindow.xaml.h"
#if __has_include("MainWindow.g.cpp")
#include "MainWindow.g.cpp"
#endif

#include <winrt/Microsoft.UI.Xaml.h>
#include <winrt/Microsoft.UI.Windowing.h>

#include <cstdio>
#include <iostream>
#include <sstream>
#include <string>
#include <tlhelp32.h>

using namespace winrt;
using namespace Microsoft::UI::Xaml;


// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.


DWORD GetTargetThreadIDFromProcName(const wchar_t* ProcName)
{
	PROCESSENTRY32 pe;
	HANDLE thSnapShot;
	BOOL retval, ProcFound = false;
	thSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (thSnapShot == INVALID_HANDLE_VALUE)
	{

		printf("Error: Unable create toolhelp snapshot!");
		return false;
	}
	pe.dwSize = sizeof(PROCESSENTRY32);
	retval = Process32First(thSnapShot, &pe);
	while (retval)
	{
		if (wcsstr(pe.szExeFile, ProcName))
		{
			return pe.th32ProcessID;
		}
		retval = Process32Next(thSnapShot, &pe);
	}
	return 0;
}

uintptr_t GetModuleBaseAddress(DWORD procId, const wchar_t* modName, DWORD& size)
{
    uintptr_t modBaseAddr = 0;
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, procId);
    if (hSnap != INVALID_HANDLE_VALUE)
    {
        MODULEENTRY32 modEntry;
        modEntry.dwSize = sizeof(modEntry);
        if (Module32First(hSnap, &modEntry))
        {
            do
            {
                if (!_wcsicmp(modEntry.szModule, modName))
                {
                    modBaseAddr = (uintptr_t)modEntry.modBaseAddr;
                    size = modEntry.modBaseSize;
                    break;
                }
            } while (Module32Next(hSnap, &modEntry));
        }
    }
    CloseHandle(hSnap);
    return modBaseAddr;
}


// Yeah, clean this up
HANDLE Proc;
auto scatter1 = 0.0f;
uintptr_t offset_scatter1 = 0;
auto scatter2 = 0.0f;
uintptr_t offset_scatter2= 0;
auto scatter3 = 0.0f;
uintptr_t offset_scatter3 = 0;

auto param5 = 0.0f;
uintptr_t offset_param5 = 0;
auto param1 = 0.0f;
uintptr_t offset_param1 = 0;
auto param2 = 0.0f;
uintptr_t offset_param2 = 0;

uint32_t param0 = 0;
uintptr_t offset_param0 = 0;

uintptr_t baseAddress;

namespace winrt::SunsetTool::implementation
{
    MainWindow::MainWindow()
    {
        InitializeComponent();
        AppWindow().Resize({ 800, 600 });
        AppWindow().Title(L"Sunset Tool");
    }

    int32_t MainWindow::MyProperty()
    {
        throw hresult_not_implemented();
    }

    void MainWindow::MyProperty(int32_t /* value */)
    {
        throw hresult_not_implemented();
    }

    void MainWindow::myButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        DWORD size = 0;
        auto procID = GetTargetThreadIDFromProcName(L"FlightSimulator.exe");
        baseAddress = GetModuleBaseAddress(procID, L"FlightSimulator.exe", size);

        Proc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, procID);

        //offset_scatter1 = baseAddress + 0x1E00 + 0x74253D8;
        //offset_scatter2 = offset_scatter1 + 4;
        //offset_scatter3 = offset_scatter2 + 4;

        SIZE_T bytesRead = 0; 

        auto array = std::make_unique<uint64_t[]>(size / 8);

        ReadProcessMemory(Proc, (void*)(baseAddress), array.get(), (SIZE_T)size, &bytesRead);

        for (auto i = 0; i < size / 8; ++i)
        {
            //if (array[i] == 0x3F8000003F47AE14)
            if (array[i] == 0xC5C7100000000000)
            {
                baseAddress += i * 8 + 128;
                break;
            }
        }

        offset_scatter1 += baseAddress + 88;
        offset_scatter2 = offset_scatter1 + 4;
        offset_scatter3 = offset_scatter2 + 4;

        offset_param5 = baseAddress + 36;
        offset_param1 = baseAddress + 52;
        offset_param2 = baseAddress + 60;
        offset_param0 = baseAddress + 8;


        ReadProcessMemory(Proc, (void*)(offset_scatter1), &scatter1, (SIZE_T)sizeof(scatter1), &bytesRead);
        ReadProcessMemory(Proc, (void*)(offset_scatter2), &scatter2, (SIZE_T)sizeof(scatter2), &bytesRead);
        ReadProcessMemory(Proc, (void*)(offset_scatter3), &scatter3, (SIZE_T)sizeof(scatter3), &bytesRead);

        ReadProcessMemory(Proc, (void*)(offset_param5), &param5, (SIZE_T)sizeof(param5), &bytesRead);
        ReadProcessMemory(Proc, (void*)(offset_param1), &param1, (SIZE_T)sizeof(param1), &bytesRead);
        ReadProcessMemory(Proc, (void*)(offset_param2), &param2, (SIZE_T)sizeof(param2), &bytesRead);

        myButton().Content(box_value(L"Loaded!"));
        myButton().IsEnabled(false);

        slider1().Value(scatter1);
        slider2().Value(scatter2);
        slider3().Value(scatter3);
        slider4().Value(param5);
        slider5().Value(param1);
        slider6().Value(param2);

        std::wstringstream stream;
        stream << std::hex << param0;
        headerParam().Text(stream.str());


        headerOffset().Text(std::to_wstring(8));
    }

    void MainWindow::button2_Click(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& args)
    {
        scatter1 = 0.000650f;
        scatter2 = 0.001881f;
        scatter3 = 0.000085f;
        param5 = 0.1f;
        param1 = 1.0f;
        param2 = 1.0f;

        slider1().Value(scatter1);
        slider2().Value(scatter2);
        slider3().Value(scatter3);
        slider4().Value(param5);
        slider5().Value(param1);
        slider6().Value(param2);
    }

    uint32_t result = 0xFFFFFFFF;

    void MainWindow::slider1_ValueChanged(Windows::Foundation::IInspectable const& sender, 
        Microsoft::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs  const& args)
    {
        scatter1 = static_cast<float>(args.NewValue());

        header1().Text(std::to_wstring(scatter1));

        SIZE_T bytesRead = 0;
        WriteProcessMemory(Proc, (void*)(offset_scatter1), &scatter1, (SIZE_T)sizeof(scatter1), &bytesRead);
    }

    void MainWindow::slider2_ValueChanged(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs const& args)
    {
        scatter2 = static_cast<float>(args.NewValue());

        header2().Text(std::to_wstring(scatter2));

        SIZE_T bytesRead = 0;
        WriteProcessMemory(Proc, (void*)(offset_scatter2), &scatter2, (SIZE_T)sizeof(scatter2), &bytesRead);
    }

    void MainWindow::slider3_ValueChanged(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs const& args)
    {
        scatter3 = static_cast<float>(args.NewValue());

        header3().Text(std::to_wstring(scatter3));

        SIZE_T bytesRead = 0;
        WriteProcessMemory(Proc, (void*)(offset_scatter3), &scatter3, (SIZE_T)sizeof(scatter3), &bytesRead);
    }

    void MainWindow::slider4_ValueChanged(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs const& args)
    {
        param5 = static_cast<float>(args.NewValue());

        header4().Text(std::to_wstring(param5));

        SIZE_T bytesRead = 0;
        WriteProcessMemory(Proc, (void*)(offset_param5), &param5, (SIZE_T)sizeof(param5), &bytesRead);
    }

    void MainWindow::slider5_ValueChanged(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs const& args)
    {
        param1 = static_cast<float>(args.NewValue());

        header5().Text(std::to_wstring(param1));

        SIZE_T bytesRead = 0;
        WriteProcessMemory(Proc, (void*)(offset_param1), &param1, (SIZE_T)sizeof(param1), &bytesRead);
    }

    void MainWindow::slider6_ValueChanged(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs const& args)
    {
        param2 = static_cast<float>(args.NewValue());
        

        header6().Text(std::to_wstring(param2));

        SIZE_T bytesRead = 0;
        WriteProcessMemory(Proc, (void*)(offset_param2), &param2, (SIZE_T)sizeof(param2), &bytesRead);
    }

    void MainWindow::param_TextChanged(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::Controls::TextChangedEventArgs const& args)
    {
        std::stringstream stream;
        stream << std::hex << winrt::to_string(headerParam().Text());
        param0 = std::stoul(stream.str());

        SIZE_T bytesRead = 0;
        WriteProcessMemory(Proc, (void*)(offset_param0), &param0, (SIZE_T)sizeof(param0), &bytesRead);
    }

    void MainWindow::offset_TextChanged(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::Controls::TextChangedEventArgs const& args)
    {
        offset_param0 = baseAddress + std::stoul(winrt::to_string(headerOffset().Text()));

        SIZE_T bytesRead = 0;
        ReadProcessMemory(Proc, (void*)(offset_param0), &param0, (SIZE_T)sizeof(param0), &bytesRead);

        std::wstringstream stream;
        stream << std::hex << param0;
        headerParam().Text(stream.str());
    }
}
