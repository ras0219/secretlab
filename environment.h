#pragma once

#include "cstring.h"
#include "matrix.h"

#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

// TODO: move to another file
std::string read_line();

enum class ValueType
{
    SCALAR,
    MATRIX,
    SYMBOL,
    STRING,
};

struct Value
{
    struct SymbolTag
    {
    };
    static constexpr SymbolTag symbol_tag = {};

    struct StringTag
    {
    };
    static constexpr StringTag string_tag = {};

    Value(double a) : type(ValueType::SCALAR), d(a) {}
    Value(std::string_view a, SymbolTag);
    Value(std::string_view a, StringTag);
    Value(MatrixData&& a) : type(ValueType::MATRIX), m(std::move(a)) {}

    Value clone() const;
    void display() const;

    ValueType type;
    double d;
    CString s;
    MatrixData m;
};

struct Stack
{
    template<class... T>
    void push(T&&... t)
    {
        m_stack.emplace_back(std::forward<T>(t)...);
    }

    Value pop();
    double pop_double();
    CString pop_symbol();
    CString pop_string();
    MatrixData pop_matrix();

    void clear() { m_stack.clear(); }
    const Value& at_from_top(int index) const;
    int size() const { return m_stack.size(); }

    void display_top() const;
    void display() const;

private:
    std::vector<Value> m_stack;
};

using VarMap = std::unordered_map<std::string, Value>;

struct Environment
{
    Stack stack;
    VarMap varmap;

    bool auto_display_flag = true;

    void auto_display() const;
};

struct Command
{
    using CommandFunction = void (*)(Environment&);

    std::string_view name;
    std::string_view signature;

    CommandFunction function;
};
