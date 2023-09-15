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
        auto baseAddress = GetModuleBaseAddress(procID, L"FlightSimulator.exe", size);

        Proc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, procID);

        //offset_scatter1 = baseAddress + 0x1E00 + 0x74253D8;
        //offset_scatter2 = offset_scatter1 + 4;
        //offset_scatter3 = offset_scatter2 + 4;

        SIZE_T bytesRead = 0; 

        auto array = std::make_unique<uint64_t[]>(size / 8);

        ReadProcessMemory(Proc, (void*)(baseAddress), array.get(), (SIZE_T)size, &bytesRead);

        for (auto i = 0; i < size / 8; ++i)
        {
            if (array[i] == 0x3F8000003F47AE14)
            {
                offset_scatter1 = i * 8 + 88;
                break;
            }
        }

        offset_scatter1 += baseAddress;
        offset_scatter2 = offset_scatter1 + 4;
        offset_scatter3 = offset_scatter2 + 4;

        ReadProcessMemory(Proc, (void*)(offset_scatter1), &scatter1, (SIZE_T)sizeof(scatter1), &bytesRead);
        ReadProcessMemory(Proc, (void*)(offset_scatter2), &scatter2, (SIZE_T)sizeof(scatter2), &bytesRead);
        ReadProcessMemory(Proc, (void*)(offset_scatter3), &scatter3, (SIZE_T)sizeof(scatter3), &bytesRead);

        myButton().Content(box_value(L"Loaded!"));
        myButton().IsEnabled(false);

        slider1().Value(scatter1);
        slider2().Value(scatter2);
        slider3().Value(scatter3);
    }

    void MainWindow::button2_Click(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& args)
    {
        scatter1 = 0.000650f;
        scatter2 = 0.001881f;
        scatter3 = 0.000085f;

        slider1().Value(scatter1);
        slider2().Value(scatter2);
        slider3().Value(scatter3);
    }

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
}
