#pragma once

namespace SunsetTool
{

class Model
{
public:
    Model(float r, float g, float b,
        float aerosols, float color, float sun, std::wstring name);

    static Model Default();

    [[nodiscard]] auto AbsorbR() const -> float;
    auto AbsorbR(float in) -> void;
    [[nodiscard]] auto AbsorbG() const -> float;
    auto AbsorbG(float in) -> void;
    [[nodiscard]] auto AbsorbB() const -> float;
    auto AbsorbB(float in) -> void;
    [[nodiscard]] auto Aerosols() const -> float;
    auto Aerosols(float in) -> void;
    [[nodiscard]] auto Color() const -> float;
    auto Color(float in) -> void;
    [[nodiscard]] auto Sun() const -> float;
    auto Sun(float in) -> void;
    [[nodiscard]] auto Name() const -> const std::wstring&;
    auto Name(std::wstring in) -> void;

    [[nodiscard]] auto IsDirty() const -> bool;

    auto LoadFromINI(const std::wstring& file_name) -> bool;
    auto SaveToINI(const std::wstring& file_name) -> bool;

    auto ToBase64() const -> std::string;
    auto FromBase64(const std::string& input) -> bool;

private:
    float m_absorb_r = 0.0f;
    float m_absorb_g = 0.0f;
    float m_absorb_b = 0.0f;
    float m_aerosols = 0.0f;
    float m_color = 0.0f;
    float m_sun = 0.0f;
    std::wstring m_name;

    bool m_dirty = false;
};

} // namespace SunsetTool
