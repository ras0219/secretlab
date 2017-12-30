#include "pch.h"

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
    e.stack.display_top();
}

static void exit_command(Environment& e) { std::exit(0); }

static void stack_command(Environment& e) { e.stack.display(); }

static void pop_command(Environment& e)
{
    e.stack.pop();
    e.stack.display_top();
}

static void plus_command(Environment& e)
{
    auto a = e.stack.pop_double();
    auto b = e.stack.pop_double();
    e.stack.push(a + b);
    e.stack.display_top();
}

static void minus_command(Environment& e)
{
    auto a = e.stack.pop_double();
    auto b = e.stack.pop_double();
    e.stack.push(b - a);
    e.stack.display_top();
}
static void mult_command(Environment& e)
{
    auto a = e.stack.pop_double();
    auto b = e.stack.pop_double();
    e.stack.push(a * b);
    e.stack.display_top();
}
static void div_command(Environment& e)
{
    auto a = e.stack.pop_double();
    auto b = e.stack.pop_double();
    e.stack.push(b / a);
    e.stack.display_top();
}
static void matrix_command(Environment& e)
{
    auto extent = (int)e.stack.pop_double();
    MatrixData m;
    m.data.resize(extent, 0);
    for (int x = extent - 1; x >= 0; --x)
        m.data[x] = e.stack.pop_double();
    e.stack.push(std::move(m));
    e.stack.display_top();
}
static void ones_command(Environment& e)
{
    auto extent = (int)e.stack.pop_double();
    MatrixData m;
    m.data.resize(extent, 1.0);
    e.stack.push(std::move(m));
    e.stack.display_top();
}

static void store_command(Environment& e)
{
    auto sym = e.stack.pop_symbol();
    auto v = e.stack.pop();
    e.varmap.emplace(sym.get(), std::move(v));

    e.stack.display_top();
}
static void load_command(Environment& e)
{
    auto sym = e.stack.pop_symbol();
    e.stack.push(e.varmap.at(sym.get()).clone());

    e.stack.display_top();
}

static void mat_mul_command(Environment& e)
{
    auto d = e.stack.pop_double();
    auto m = e.stack.pop_matrix();
    for (auto&& x : m.data)
        x *= d;
    e.stack.push(std::move(m));

    e.stack.display_top();
}

static void mat_pow_command(Environment& e)
{
    auto d = e.stack.pop_double();
    auto m = e.stack.pop_matrix();
    for (auto&& x : m.data)
        x = pow(x, d);
    e.stack.push(std::move(m));

    e.stack.display_top();
}

static void mat_add_command(Environment& e)
{
    auto d = e.stack.pop_double();
    auto m = e.stack.pop_matrix();
    for (auto&& x : m.data)
        x += d;
    e.stack.push(std::move(m));

    e.stack.display_top();
}

using namespace std::string_view_literals;

static constexpr Command commands[] = {
    {"exit"sv, "exit"sv, &exit_command},
    {"help"sv, "help"sv, &help_command},
    {"inner"sv, "inner :: m1 m2 dExtent dStride1 dStride2 -> m"sv, &inner_command},
    {"load"sv, "load :: y -> *"sv, &load_command},
    {"matrix"sv, "matrix :: d... dLen -> m"sv, &matrix_command},
    {"ones"sv, "ones :: dLen -> m"sv, &ones_command},
    {"m+"sv, "m+ :: m d -> m"sv, &mat_add_command},
    {"m*"sv, "m* :: m d -> m"sv, &mat_mul_command},
    {"m**"sv, "m** :: m d -> m"sv, &mat_pow_command},
    {"pop"sv, "pop :: * ->"sv, &pop_command},
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
