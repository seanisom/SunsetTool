#pragma once

namespace SunsetTool
{

class Model;


class MSFSMemory
{
public:
	enum class EVersion
	{
		None = 0,
		Unknown,
		SU13,
		SU14
	};

	MSFSMemory() = default;
	~MSFSMemory();
	MSFSMemory(const MSFSMemory& other) = delete;
	MSFSMemory(MSFSMemory&& other) noexcept = delete;
	MSFSMemory& operator=(const MSFSMemory& other) = delete;
	MSFSMemory& operator=(MSFSMemory&& other) noexcept = delete;

	auto Load() -> EVersion;

	auto ReadMemory(Model& model) -> bool;
	auto WriteMemory(const Model& model) -> bool;

private:
	HANDLE m_proc = nullptr;
	uintptr_t m_base_address = 0;
	uintptr_t m_offset_absorb_r = 0;
	uintptr_t m_offset_absorb_g = 0;
	uintptr_t m_offset_absorb_b = 0;
	uintptr_t m_offset_aerosols = 0;
	uintptr_t m_offset_color = 0;
	uintptr_t m_offset_sun = 0;
};

} // namespace SunsetTool