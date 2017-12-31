#include "pch.h"

#include "cfile.h"
#include "engine.h"

static void help_command(Environment& e);

static void inner_command(Environment& e)
{
    auto stride2 = (int)e.stack.pop_double();
    auto stride1 = (int)e.stack.pop_double();
    auto extent = (int)e.stack.pop_double();
    auto m2 = e.stack.pop_matrix();
    auto m1 = e.stack.pop_matrix();
    e.stack.push(inner_product(m1, m2, extent, stride1, stride2));
    e.auto_display();
}

static void exit_command(Environment& e) { std::exit(0); }

static void stack_command(Environment& e) { e.stack.display(); }

static void pop_command(Environment& e)
{
    e.stack.pop();
    e.auto_display();
}

static void plus_command(Environment& e)
{
    auto a = e.stack.pop_double();
    auto b = e.stack.pop_double();
    e.stack.push(a + b);
    e.auto_display();
}

static void minus_command(Environment& e)
{
    auto a = e.stack.pop_double();
    auto b = e.stack.pop_double();
    e.stack.push(b - a);
    e.auto_display();
}
static void mult_command(Environment& e)
{
    auto a = e.stack.pop_double();
    auto b = e.stack.pop_double();
    e.stack.push(a * b);
    e.auto_display();
}
static void div_command(Environment& e)
{
    auto a = e.stack.pop_double();
    auto b = e.stack.pop_double();
    e.stack.push(b / a);
    e.auto_display();
}
static void matrix_command(Environment& e)
{
    auto extent = (int)e.stack.pop_double();
    MatrixData m;
    m.data.resize(extent, 0);
    for (int x = extent - 1; x >= 0; --x)
        m.data[x] = e.stack.pop_double();
    e.stack.push(std::move(m));
    e.auto_display();
}
static void ones_command(Environment& e)
{
    auto extent = (int)e.stack.pop_double();
    MatrixData m;
    m.data.resize(extent, 1.0);
    e.stack.push(std::move(m));
    e.auto_display();
}

static void store_command(Environment& e)
{
    auto sym = e.stack.pop_symbol();
    auto v = e.stack.pop();
    e.varmap.emplace(sym.c_str(), std::move(v));

    e.auto_display();
}
static void load_command(Environment& e)
{
    auto sym = e.stack.pop_symbol();
    e.stack.push(e.varmap.at(sym.c_str()).clone());

    e.auto_display();
}

static void mat_mul_command(Environment& e)
{
    auto d = e.stack.pop_double();
    auto m = e.stack.pop_matrix();
    for (auto&& x : m.data)
        x *= d;
    e.stack.push(std::move(m));

    e.auto_display();
}

static void mat_pow_command(Environment& e)
{
    auto d = e.stack.pop_double();
    auto m = e.stack.pop_matrix();
    for (auto&& x : m.data)
        x = pow(x, d);
    e.stack.push(std::move(m));

    e.auto_display();
}

static void mat_add_command(Environment& e)
{
    auto d = e.stack.pop_double();
    auto m = e.stack.pop_matrix();
    for (auto&& x : m.data)
        x += d;
    e.stack.push(std::move(m));

    e.auto_display();
}

static void mat_add_mat_command(Environment& e)
{
    auto m1 = e.stack.pop_matrix();
    auto m2 = e.stack.pop_matrix();
    if (m1.data.size() != m2.data.size()) throw std::runtime_error("matricies do not have equal extents");
    for (size_t i = 0; i < m1.data.size(); ++i)
    {
        m1.data[i] += m2.data[i];
    }

    e.stack.push(std::move(m1));

    e.auto_display();
}

static void size_command(Environment& e)
{
    auto m = e.stack.pop_matrix();
    auto s = m.data.size();
    e.stack.push(std::move(m));
    e.stack.push(s);

    e.auto_display();
}

static std::string serialize_helper(MatrixData const& m)
{
    std::string ret;
    for (auto&& x : m.data)
        ret += fmt::sprintf("%.16f ", x);
    ret += fmt::sprintf("%d matrix", (int)m.data.size());
    return ret;
}
static std::string serialize_helper(double d) { return fmt::sprintf("%.16f", d); }
static std::string serialize_helper(const Value& v)
{
    switch (v.type)
    {
        case ValueType::MATRIX: return serialize_helper(v.m);
        case ValueType::SCALAR: return serialize_helper(v.d);
        case ValueType::SYMBOL: return fmt::sprintf("$$%s", v.s.c_str());
        case ValueType::STRING: return fmt::sprintf("\"%s\"", v.s.c_str());
        default: std::terminate();
    }
}

static void serialize_command(Environment& e) { fmt::printf("%s\n", serialize_helper(e.stack.at_from_top(0))); }

static void pwd_command(Environment&)
{
    std::array<wchar_t, 1024> cwd;
    cwd[0] = L'\0';
    auto hr = GetCurrentDirectoryW(cwd.size(), cwd.data());
    if (FAILED(hr))
    {
        throw std::runtime_error("GetCurrentDirectoryW failed");
    }
    std::array<char, 1024> utf8_cwd;
    utf8_cwd[0] = '\0';
    auto z = WideCharToMultiByte(CP_UTF8, 0, cwd.data(), -1, utf8_cwd.data(), utf8_cwd.size(), nullptr, nullptr);
    fmt::printf("%s\n", utf8_cwd.data());
}

static void fswrite(std::string_view sv, FILE* f) { fwrite(sv.data(), 1, sv.size(), f); }

static void dump_command(Environment& e)
{
    fmt::printf("Filename>");

    std::string filename = read_line();

    auto p = fs::absolute(filename);
    auto out_file_holder = CFile::open_wb(p);
    CFileView out_file = out_file_holder;

    for (auto&& p : e.varmap)
    {
        auto s = serialize_helper(p.second);
        out_file.printf("%s\n$$%s store\n", s, p.first);
    }

    for (int x = e.stack.size() - 1; x >= 0; --x)
    {
        out_file.printf("%s\n", serialize_helper(e.stack.at_from_top(x)));
    }

    fmt::printf("Wrote state to \"%s\".\n", p.u8string());
}

using namespace std::string_view_literals;

static constexpr Command commands[] = {
    {"dump"sv, "dump"sv, &dump_command},
    {"exit"sv, "exit"sv, &exit_command},
    {"help"sv, "help"sv, &help_command},
    {"pwd"sv, "pwd"sv, &pwd_command},
    {"inner"sv, "inner :: m1 m2 dExtent dStride1 dStride2 -> m"sv, &inner_command},
    {"load"sv, "load :: y -> *"sv, &load_command},
    {"matrix"sv, "matrix :: d... dLen -> m"sv, &matrix_command},
    {"serialize"sv, "serialize :: * -> *"sv, &serialize_command},
    {"ones"sv, "ones :: dLen -> m"sv, &ones_command},
    {"m+"sv, "m+ :: m d -> m"sv, &mat_add_command},
    {"m*"sv, "m* :: m d -> m"sv, &mat_mul_command},
    {"m**"sv, "m** :: m d -> m"sv, &mat_pow_command},
    {"m+m"sv, "m+m :: m m -> m"sv, &mat_add_mat_command},
    {"pop"sv, "pop :: * ->"sv, &pop_command},
    {"size"sv, "size :: m -> m d"sv, &size_command},
    {"stack"sv, "stack :: ->"sv, &stack_command},
    {"store"sv, "store :: * y ->"sv, &store_command},
    {"+"sv, "+ :: d d -> d"sv, &plus_command},
    {"-"sv, "- :: d d -> d"sv, &minus_command},
    {"*"sv, "* :: d d -> d"sv, &mult_command},
    {"/"sv, "/ :: d d -> d"sv, &div_command},
};

static void help_command(Environment& e)
{
    fmt::printf("Commands:\n");
    for (auto&& command : commands)
    {
        fmt::printf("  %s\n", command.signature);
    }
    fmt::printf("\nSpecial operations:\n"
                "  <N> - push literal number N\n"
                "  @<N> - push Nth stack element, from the top\n"
                "  $<name> - push value of variable <name>\n"
                "  $$<name> - push symbol for variable <name>\n");
}

extern "C" Commands __cdecl get_commands() { return {sizeof(commands) / sizeof(commands[0]), commands}; }
