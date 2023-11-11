// ReSharper disable CppClangTidyPerformanceNoIntToPtr
#include "pch.h"

#include "MSFSMemory.h"

#include <tlhelp32.h>

#include "Model.h"

DWORD GetTargetThreadIDFromProcName(const wchar_t* ProcName)
{
	PROCESSENTRY32 pe;
	HANDLE thSnapShot;
	BOOL retval;
	thSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (thSnapShot == INVALID_HANDLE_VALUE)
	{

		printf("Error: Unable create toolhelp snapshot!");
		return 0;
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

uintptr_t GetModuleBaseAddress(DWORD procId, const wchar_t* modName, DWORD& size, std::wstring& path)
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
					path = modEntry.szExePath;
					break;
				}
			} while (Module32Next(hSnap, &modEntry));
		}
	}
	CloseHandle(hSnap);
	return modBaseAddr;
}


struct MSFSVersion
{
	int Major;
	int Minor;
	int Build;
	int Revision;
};

MSFSVersion GetMSFSVersion(const std::wstring& szVersionFile)
{
	DWORD  verHandle = 0;
	UINT   size = 0;
	LPBYTE lpBuffer = NULL;
	DWORD  verSize = GetFileVersionInfoSize(szVersionFile.c_str(), &verHandle);

	if (verSize != NULL)
	{
		LPSTR verData = new char[verSize];

		if (GetFileVersionInfo(szVersionFile.c_str(), verHandle, verSize, verData))
		{
			if (VerQueryValue(verData, L"\\", (VOID FAR * FAR*) & lpBuffer, &size))
			{
				if (size)
				{
					VS_FIXEDFILEINFO* verInfo = (VS_FIXEDFILEINFO*)lpBuffer;
					if (verInfo->dwSignature == 0xfeef04bd)
					{

						// Doesn't matter if you are on 32 bit or 64 bit,
						// DWORD is always 32 bits, so first two revision numbers
						// come from dwFileVersionMS, last two come from dwFileVersionLS
						return
						{
							(verInfo->dwFileVersionMS >> 16) & 0xffff,
							(verInfo->dwFileVersionMS >> 0) & 0xffff,
							(verInfo->dwFileVersionLS >> 16) & 0xffff,
							(verInfo->dwFileVersionLS >> 0) & 0xffff
						};
					}
				}
			}
		}
		delete[] verData;
	}
	return {};
}

SunsetTool::MSFSMemory::~MSFSMemory()
{
	if (m_proc)
		CloseHandle(m_proc);
}

auto SunsetTool::MSFSMemory::Load() -> EVersion
{
	DWORD size = 0;
	std::wstring path {};
	const auto procID = GetTargetThreadIDFromProcName(L"FlightSimulator.exe");

	if (procID == 0)
		return EVersion::None;

	m_base_address = GetModuleBaseAddress(procID, L"FlightSimulator.exe", size, path);

	if (m_base_address == 0)
		return EVersion::None;

	const auto version = GetMSFSVersion(path);

	m_proc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, procID);

	if (m_proc == nullptr)
		return EVersion::None;

	const auto out_mem = std::make_unique<uint64_t[]>(size / 8);

	SIZE_T bytes_read = 0;
	ReadProcessMemory(m_proc, reinterpret_cast<void*>(m_base_address), out_mem.get(), size, &bytes_read);

	for (auto i = 0u; i < size / 8; ++i)
	{
		if (out_mem[i] == 0xC5C7100000000000)
		{
			m_base_address += i * 8 + 128;
			break;
		}
	}

	EVersion v = EVersion::Unknown;

	if (m_base_address == 0)
		return v;
	if (version.Minor == 34)
		v = EVersion::SU13;
	else if (version.Minor == 35)
		v = EVersion::SU14;

	m_offset_absorb_r += m_base_address + 88;

	if (v == EVersion::SU14)
	{
		m_offset_absorb_r += 248;
	}

	m_offset_absorb_g = m_offset_absorb_r + 4;
	m_offset_absorb_b = m_offset_absorb_g + 4;

	m_offset_aerosols = m_base_address + 36;
	m_offset_color = m_base_address + 52;
	m_offset_sun = m_base_address + 60;

	return v;
}

bool SunsetTool::MSFSMemory::ReadMemory(Model& model)
{
	float absorb_r = 0.0f;
	float absorb_g = 0.0f;
	float absorb_b = 0.0f;
	float aerosols = 0.0f;
	float color = 0.0f;
	float sun = 0.0f;

	SIZE_T bytesRead = 0;
	ReadProcessMemory(m_proc, reinterpret_cast<void*>(m_offset_absorb_r), &absorb_r, sizeof(float), &bytesRead);
	ReadProcessMemory(m_proc, reinterpret_cast<void*>(m_offset_absorb_g), &absorb_g, sizeof(float), &bytesRead);
	ReadProcessMemory(m_proc, reinterpret_cast<void*>(m_offset_absorb_b), &absorb_b, sizeof(float), &bytesRead);
	ReadProcessMemory(m_proc, reinterpret_cast<void*>(m_offset_aerosols), &aerosols, sizeof(float), &bytesRead);
	ReadProcessMemory(m_proc, reinterpret_cast<void*>(m_offset_color), &color, sizeof(float), &bytesRead);
	ReadProcessMemory(m_proc, reinterpret_cast<void*>(m_offset_sun), &sun, sizeof(float), &bytesRead);

	model.AbsorbR(absorb_r);
	model.AbsorbG(absorb_g);
	model.AbsorbB(absorb_b);
	model.Aerosols(aerosols);
	model.Color(color);
	model.Sun(sun);

	return true;
}

bool SunsetTool::MSFSMemory::WriteMemory(const Model& model)
{
	if (!model.IsDirty())
		return true;

	const float absorb_r = model.AbsorbR();
	const float absorb_g = model.AbsorbG();
	const float absorb_b = model.AbsorbB();
	const float aerosols = model.Aerosols();
	const float color = model.Color();
	const float sun = model.Sun();

	SIZE_T bytesRead = 0;
	WriteProcessMemory(m_proc, reinterpret_cast<void*>(m_offset_absorb_r), &absorb_r, sizeof(float), &bytesRead);
	WriteProcessMemory(m_proc, reinterpret_cast<void*>(m_offset_absorb_g), &absorb_g, sizeof(float), &bytesRead);
	WriteProcessMemory(m_proc, reinterpret_cast<void*>(m_offset_absorb_b), &absorb_b, sizeof(float), &bytesRead);
	WriteProcessMemory(m_proc, reinterpret_cast<void*>(m_offset_aerosols), &aerosols, sizeof(float), &bytesRead);
	WriteProcessMemory(m_proc, reinterpret_cast<void*>(m_offset_color), &color, sizeof(float), &bytesRead);
	WriteProcessMemory(m_proc, reinterpret_cast<void*>(m_offset_sun), &sun, sizeof(float), &bytesRead);

	return true;
}
