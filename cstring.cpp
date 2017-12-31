#include "pch.h"

#include "cstring.h"

CString::CString(const char* cstr) : CString(std::string_view(cstr, strlen(cstr))) {}
CString::CString(const std::string& s) : CString(std::string_view(s)) {}
CString::CString(std::string_view sv)
{
    std::unique_ptr<char[]> d(new char[sv.size() + 1]);
    memcpy(d.get(), sv.data(), sv.size());
    d[sv.size()] = '\0';
    m_data = std::move(d);
}

std::string_view CString::to_string_view() const { return {m_data.get(), strlen(m_data.get())}; }
