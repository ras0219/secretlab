#pragma once

#include <memory>
#include <string>
#include <string_view>

struct CString
{
    constexpr CString() = default;
    CString(const char* cstr);
    CString(const std::string& s);
    CString(std::string_view sv);

    const char* c_str() const { return m_data.get(); }
    const char* data() const { return m_data.get(); }
    char* data() { return m_data.get(); }

    std::string_view to_string_view() const;

private:
    std::unique_ptr<char[]> m_data;
};
