#include "pch.h"

#include "cfile.h"

void CFile::CFileDeleter::operator()(FILE* f) { fclose(f); }

int CFileView::scan_string(char* buf, int size) { return fscanf_s(m_ptr, "%s", buf, size); }

CFile CFile::open_wb(const std::experimental::filesystem::path& filename)
{
    FILE* out = nullptr;
    if (_wfopen_s(&out, filename.native().c_str(), L"wb")) throw std::runtime_error("Could not open file for writing");
    return CFile(out);
}
CFile CFile::open_rb(const std::experimental::filesystem::path& filename)
{
    FILE* out = nullptr;
    if (_wfopen_s(&out, filename.native().c_str(), L"rb")) throw std::runtime_error("Could not open file for reading");
    return CFile(out);
}
