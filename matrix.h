#pragma once

#include <initializer_list>
#include <vector>

struct VectorData
{
    std::vector<double> data;
};

struct MatrixExtents
{
    std::vector<int> extents;
};

struct MatrixData
{
    MatrixData() = default;
    MatrixData(std::initializer_list<double> ilist) : data(ilist) {}

    MatrixData(MatrixData&&) = default;
    MatrixData(const MatrixData&) = delete;

    MatrixData& operator=(MatrixData&&) = default;
    MatrixData& operator=(const MatrixData&) = delete;

    MatrixData clone() const
    {
        MatrixData ret;
        ret.data = data;
        return ret;
    }

    std::vector<double> data;
};

struct MatrixView
{
    MatrixExtents& e;
    MatrixData& d;
};

MatrixData multiply_matrix(const MatrixData& left, const MatrixData& right, int extent);

MatrixData dot(const MatrixData& v1, const MatrixData& v2);
void display(const MatrixData& m, int columns = 4);

MatrixData bayes_rule(const MatrixData& src, const MatrixData& mult, const MatrixData& div);

template<class BinaryFunc>
MatrixData by_element(const MatrixData& m, const MatrixData& v, BinaryFunc func)
{
    MatrixData ret;
    ret.data.resize(m.data.size());
    for (size_t i_m = 0; i_m < m.data.size();)
    {
        for (size_t i_v = 0; i_v < v.data.size(); ++i_v, ++i_m)
        {
            ret.data[i_m] = func(m.data[i_m], v.data[i_v]);
        }
    }
    return ret;
}

MatrixData multiply(const MatrixData& m, const MatrixData& v);
MatrixData transpose(const MatrixData& v1, int extent);
MatrixData transpose2(const MatrixData& v1, int extent1, int extent2);
MatrixData divide(const MatrixData& m, const MatrixData& v);

MatrixData inner_product(const MatrixData& m1, const MatrixData& m2, int inner_extent, int stride1, int stride2);
