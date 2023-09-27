#include "pch.h"

#include "Model.h"


using namespace SunsetTool;

//FROM
//https://stackoverflow.com/a/34571089/5155484

typedef unsigned char uchar;
static const std::string b = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";//=
static std::string base64_encode(const std::string& in) {
    std::string out;

    int val = 0, valb = -6;
    for (uchar c : in) {
        val = (val << 8) + c;
        valb += 8;
        while (valb >= 0) {
            out.push_back(b[(val >> valb) & 0x3F]);
            valb -= 6;
        }
    }
    if (valb > -6) out.push_back(b[((val << 8) >> (valb + 8)) & 0x3F]);
    while (out.size() % 4) out.push_back('=');
    return out;
}


static std::string base64_decode(const std::string& in) {

    std::string out;

    std::vector<int> T(256, -1);
    for (int i = 0; i < 64; i++) T[b[i]] = i;

    int val = 0, valb = -8;
    for (uchar c : in) {
        if (T[c] == -1) break;
        val = (val << 6) + T[c];
        valb += 6;
        if (valb >= 0) {
            out.push_back(char((val >> valb) & 0xFF));
            valb -= 8;
        }
    }
    return out;
}

std::vector<std::string> split(const std::string& s, char delim) {
    std::vector<std::string> result;
    std::stringstream ss(s);
    std::string item;

    while (getline(ss, item, delim)) {
        result.push_back(item);
    }

    return result;
}


Model::Model(const float r, const float g, const float b,
                         const float aerosols, const float color, const float sun, std::wstring name) :
	m_absorb_r(r),
    m_absorb_g(g),
    m_absorb_b(b),
	m_aerosols(aerosols),
	m_color(color),
	m_sun(sun),
	m_name(std::move(name)) { }


Model Model::Default()
{
	return Model
	{
        0.000650f,
        0.001881f,
        0.000085f,
        0.1f,
        1.0f,
        1.0f,
        L"Default"
	};
}


float Model::AbsorbR() const
{
    return m_absorb_r;
}


void Model::AbsorbR(const float in)
{
    m_dirty = true;
    m_absorb_r = in;
}


float Model::AbsorbG() const
{
    return m_absorb_g;
}


void Model::AbsorbG(const float in)
{
    m_dirty = true;
    m_absorb_g = in;
}


float Model::AbsorbB() const
{
    return m_absorb_b;
}


void Model::AbsorbB(const float in)
{
    m_dirty = true;
    m_absorb_b = in;
}


float Model::Aerosols() const
{
    return m_aerosols;
}


void Model::Aerosols(const float in)
{
    m_dirty = true;
    m_aerosols = in;
}


float Model::Color() const
{
    return m_color;
}


void Model::Color(const float in)
{
    m_dirty = true;
    m_color = in;
}


float Model::Sun() const
{
    return m_sun;
}


void Model::Sun(const float in)
{
    m_dirty = true;
    m_sun = in;
}


const std::wstring& Model::Name() const
{
    return m_name;
}


void Model::Name(std::wstring in)  // NOLINT(performance-unnecessary-value-param)
{
    m_dirty = true;
    m_name = std::move(in);
}


bool Model::IsDirty() const
{
    return m_dirty;
}


std::string Model::ToBase64() const
{
    // TODO - support wstring names
	#pragma warning(suppress: 4244)
    const std::string ustring = { m_name.cbegin(), m_name.cend() };

    std::stringstream str;
    str << "v1.0," << m_absorb_r << "," << m_absorb_g << "," << m_absorb_b << ","
        << m_aerosols << "," << m_color << "," << m_sun << "," << ustring;

    return base64_encode(str.str());
}


bool Model::FromBase64(const std::string& input)
{
	const auto decoded = base64_decode(input);

	const auto results = split(decoded, ',');

    if (results.size() != 8)
		return false;

    if (results[0] != "v1.0")
        return false;

    m_absorb_r = stof(results[1]);
    m_absorb_g = stof(results[2]);
    m_absorb_b = stof(results[3]);
    m_aerosols = stof(results[4]);
    m_color = stof(results[5]);
    m_sun = stof(results[6]);

    m_name = { results[7].cbegin(), results[7].cend() };

    return true;
}


bool Model::LoadFromINI(const std::wstring& file_name)
{
    return false;
}


bool Model::SaveToINI(const std::wstring& file_name)
{
    return false;
}

