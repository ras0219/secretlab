#pragma once

#include <filesystem>
#include <fmt/printf.h>
#include <memory>

struct CFile;
struct CFileView;

struct CFile
{
    static CFile open_wb(const std::experimental::filesystem::path& filename);
    static CFile open_rb(const std::experimental::filesystem::path& filename);

    constexpr CFile() = default;
    explicit CFile(FILE* f) : m_ptr(f, {}) {}

    FILE* get() const { return m_ptr.get(); }

private:
    struct CFileDeleter
    {
        void operator()(FILE* f);
    };

    std::unique_ptr<FILE, CFileDeleter> m_ptr;
};

struct CFileView
{
    constexpr CFileView(FILE* ptr) : m_ptr(ptr) {}
    CFileView(const CFile& file) : m_ptr(file.get()) {}

    int scan_string(char* buf, int size);

    template<class... Ts>
    void printf(Ts&&... ts)
    {
        fmt::fprintf(m_ptr, std::forward<Ts>(ts)...);
    }

private:
    FILE* m_ptr;
};
