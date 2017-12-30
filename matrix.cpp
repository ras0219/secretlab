#include "pch.h"

#include "matrix.h"

#include <fmt/printf.h>

#include <cstdio>
#include <cstdlib>

void display(const MatrixData& m, int columns)
{
    if (m.data.empty())
    {
        printf("[ ]\n");
        return;
    }
    fmt::printf("[ %2.3f", m.data[0]);
    for (size_t x = 1; x < m.data.size(); ++x)
    {
        if (x % columns == 0)
            fmt::printf("\n  %2.3f", m.data[x]);
        else
            fmt::printf(" %2.3f", m.data[x]);
    }
    fmt::printf(" ]\n");
}

MatrixData multiply(const MatrixData& m, const MatrixData& v)
{
    return by_element(m, v, [](double a, double b) { return a * b; });
}
MatrixData transpose(const MatrixData& v1, int extent)
{
    MatrixData ret;
    ret.data.resize(v1.data.size());
    int imax = extent;
    int jmax = v1.data.size() / imax;
    for (int i = 0; i < imax; ++i)
        for (int j = 0; j < jmax; ++j)
            ret.data[j + i * jmax] = v1.data[i + j * imax];
    return ret;
}
MatrixData transpose2(const MatrixData& v1, int extent1, int extent2)
{
    MatrixData ret;
    ret.data.resize(v1.data.size());
    int imax = extent1;
    int jmax = extent2;
    int kmax = v1.data.size() / imax / jmax;
    for (int k = 0; k < kmax; ++k)
        for (int j = 0; j < jmax; ++j)
            for (int i = 0; i < imax; ++i)
                ret.data[j + i * jmax + k * imax * jmax] = v1.data[i + j * imax + k * imax * jmax];
    return ret;
}

MatrixData divide(const MatrixData& m, const MatrixData& v)
{
    return by_element(m, v, [](double a, double b) { return a / b; });
}

MatrixData bayes_rule(const MatrixData& src, const MatrixData& mult, const MatrixData& div)
{
    return divide(transpose(multiply(src, mult), mult.data.size()), div);
}

MatrixData dot(const MatrixData& v1, const MatrixData& v2)
{
    MatrixData out;
    out.data.resize(v1.data.size() / v2.data.size(), 0);

    int k = 0;
    for (size_t i = 0; i < v1.data.size();)
    {
        double d = 0;
        for (size_t j = 0; j < v2.data.size(); ++j, ++i)
        {
            d += v1.data[i] * v2.data[j];
        }
        out.data[k++] = d;
    }

    return out;
}

MatrixData multiply_matrix(const MatrixData& left, const MatrixData& right, int extent)
{
    auto left_extent = left.data.size() / extent;
    auto right_extent = right.data.size() / extent;
    MatrixData ret;
    ret.data.resize(left_extent * right_extent, 0.0);
    for (size_t i = 0; i < left_extent; ++i)
    {
        for (size_t j = 0; j < right_extent; ++j)
        {
            for (int k = 0; k < extent; ++k)
            {
                ret.data[j + i * right_extent] += left.data[k + i * extent] * right.data[j + k * right_extent];
            }
        }
    }
    return ret;
}

MatrixData inner_product(const MatrixData& m1, const MatrixData& m2, int inner_extent, int stride1, int stride2)
{
    auto m1_d1 = stride1;
    auto m1_d2 = inner_extent;
    auto m1_d3 = m1.data.size() / m1_d1 / m1_d2;

    auto m2_d1 = stride2;
    auto m2_d2 = inner_extent;
    auto m2_d3 = m2.data.size() / m2_d1 / m2_d2;

    MatrixData ret;
    ret.data.resize(m1.data.size() / inner_extent * m2.data.size() / inner_extent);

    for (int i1 = 0; i1 < m1_d1; ++i1)
    {
        for (int i2 = 0; i2 < m2_d1; ++i2)
        {
            for (size_t k2 = 0; k2 < m2_d3; ++k2)
            {
                for (size_t k1 = 0; k1 < m1_d3; ++k1)
                {
                    double v = 0.0;
                    for (int j = 0; j < inner_extent; ++j)
                    {
                        v +=
                            m1.data[i1 + j * m1_d1 + k1 * m1_d1 * m1_d2] * m2.data[i2 + j * m2_d1 + k2 * m2_d1 * m2_d2];
                    }
                    ret.data[i1 + m1_d1 * (i2 + m2_d1 * (k2 + m2_d3 * k1))] = v;
                }
            }
        }
    }
    return ret;
}
