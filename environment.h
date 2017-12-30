#pragma once

#include "matrix.h"
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

enum class ValueType
{
    SCALAR,
    MATRIX,
    SYMBOL,
};

struct Value
{
    Value(double a) : type(ValueType::SCALAR), d(a) {}
    Value(std::string_view a);
    Value(MatrixData&& a) : type(ValueType::MATRIX), m(std::move(a)) {}

    Value clone() const;
    void display() const;

    ValueType type;
    double d;
    std::unique_ptr<char[]> s;
    MatrixData m;
};

struct Stack
{
    template<class T>
    void push(T&& i)
    {
        m_stack.emplace_back(std::forward<T>(i));
    }

    Value pop();
    double pop_double();
    std::unique_ptr<char[]> pop_symbol();
    MatrixData pop_matrix();

    const Value& at_from_top(int index) const;

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
};

struct Command
{
    using CommandFunction = void (*)(Environment&);

    std::string_view name;
    std::string_view signature;

    CommandFunction function;
};
