#include "pch.h"

#include "environment.h"

Value::Value(std::string_view a, Value::SymbolTag) : type(ValueType::SYMBOL), s(a) {}
Value::Value(std::string_view a, Value::StringTag) : type(ValueType::STRING), s(a) {}
Value Value::clone() const
{
    switch (type)
    {
        case ValueType::SCALAR: return d;
        case ValueType::MATRIX: return m.clone();
        case ValueType::SYMBOL: return {s.to_string_view(), Value::symbol_tag};
        case ValueType::STRING: return {s.to_string_view(), Value::string_tag};
        default: throw std::runtime_error("unknown value type");
    }
}

void Value::display() const
{
    switch (type)
    {
        case ValueType::SCALAR: fmt::printf("= %f\n", d); return;
        case ValueType::MATRIX:
            fmt::printf("= ");
            ::display(m);
            return;
        case ValueType::SYMBOL: fmt::printf("= $%s\n", s.c_str()); return;
        default: std::terminate();
    }
}

Value Stack::pop()
{
    if (m_stack.size() < 1) throw std::runtime_error("stack underflow");
    auto ret = std::move(m_stack.back());
    m_stack.pop_back();
    return ret;
}

double Stack::pop_double()
{
    if (m_stack.size() < 1) throw std::runtime_error("stack underflow");
    if (m_stack.back().type != ValueType::SCALAR) throw std::runtime_error("type error: expected number");
    auto r = m_stack.back().d;
    m_stack.pop_back();
    return r;
}

CString Stack::pop_symbol()
{
    if (m_stack.size() < 1) throw std::runtime_error("stack underflow");
    if (m_stack.back().type != ValueType::SYMBOL) throw std::runtime_error("type error: expected symbol");
    auto s = std::move(m_stack.back().s);
    m_stack.pop_back();
    return s;
}

CString Stack::pop_string()
{
    if (m_stack.size() < 1) throw std::runtime_error("stack underflow");
    if (m_stack.back().type != ValueType::STRING) throw std::runtime_error("type error: expected string");
    auto s = std::move(m_stack.back().s);
    m_stack.pop_back();
    return s;
}

MatrixData Stack::pop_matrix()
{
    if (m_stack.size() < 1) throw std::runtime_error("stack underflow");
    if (m_stack.back().type != ValueType::MATRIX) throw std::runtime_error("type error: expected matrix");

    auto r = std::move(m_stack.back().m);
    m_stack.pop_back();
    return r;
}

const Value& Stack::at_from_top(int index) const
{
    if (index < 0 || (size_t)index >= m_stack.size()) throw std::runtime_error("stack underflow");
    return m_stack[m_stack.size() - index - 1];
}

void Stack::display_top() const
{
    if (m_stack.empty())
        fmt::printf("Stack Empty\n");
    else
        m_stack.back().display();
}
void Stack::display() const
{
    if (m_stack.empty())
    {
        fmt::printf("Stack Empty\n");
        return;
    }
    int i = m_stack.size();
    for (auto&& k : m_stack)
    {
        fmt::printf("%d) ", --i);
        k.display();
    }
}
