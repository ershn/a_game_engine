#pragma once

#include <concepts>
#include <iostream>

#include "vector.hpp"

namespace Age::Math
{
struct Matrix4;

struct Matrix3
{
    static constexpr std::size_t ROW_COUNT{3};
    static constexpr std::size_t COLUMN_COUNT{3};

    Vector3 columns[3];

    constexpr Matrix3() = default;

    constexpr Matrix3(float scalar)
        : columns{{scalar, 0.0f, 0.0f}, {0.0f, scalar, 0.0f}, {0.0f, 0.0f, scalar}}
    {
    }

    constexpr Matrix3(const Vector3 &col1, const Vector3 &col2, const Vector3 &col3)
        : columns{col1, col2, col3}
    {
    }

    explicit constexpr Matrix3(const Matrix4 &matrix);

    explicit operator const float *() const;

    constexpr const Vector3 &operator[](std::size_t index) const
    {
        if (std::is_constant_evaluated() == false)
            VBAIL_ERROR_IF(index > 2, columns[0], "Matrix3 index must be in the range [0, 2]: {}", index);

        return columns[index];
    }

    constexpr Vector3 &operator[](std::size_t index)
    {
        if (std::is_constant_evaluated() == false)
            VBAIL_ERROR_IF(index > 2, columns[0], "Matrix3 index must be in the range [0, 2]: {}", index);

        return columns[index];
    }

    constexpr Vector3 row(std::size_t index) const
    {
        if (std::is_constant_evaluated() == false)
            VBAIL_ERROR_IF(index > 2, row(0), "Matrix3 index must be in the range [0, 2]: {}", index);

        return {columns[0][index], columns[1][index], columns[2][index]};
    }
};

std::ostream &operator<<(std::ostream &out, const Matrix3 &matrix);

struct Matrix4
{
    static constexpr std::size_t ROW_COUNT{4};
    static constexpr std::size_t COLUMN_COUNT{4};

    Vector4 columns[4];

    constexpr Matrix4() = default;

    constexpr Matrix4(float scalar)
        : columns{
              {scalar, 0.0f, 0.0f, 0.0f},
              {0.0f, scalar, 0.0f, 0.0f},
              {0.0f, 0.0f, scalar, 0.0f},
              {0.0f, 0.0f, 0.0f, scalar}
          }
    {
    }

    constexpr Matrix4(const Vector4 &col1, const Vector4 &col2, const Vector4 &col3, const Vector4 &col4)
        : columns{col1, col2, col3, col4}
    {
    }

    explicit constexpr Matrix4(const Matrix3 &matrix, const Vector3 &translation = Vector3{})
        : columns{{matrix[0], 0.0f}, {matrix[1], 0.0f}, {matrix[2], 0.0f}, {translation, 1.0f}}
    {
    }

    explicit operator const float *() const;

    constexpr const Vector4 &operator[](std::size_t index) const
    {
        if (std::is_constant_evaluated() == false)
            VBAIL_ERROR_IF(index > 3, columns[0], "Matrix4 index must be in the range [0, 3]: {}", index);

        return columns[index];
    }

    constexpr Vector4 &operator[](std::size_t index)
    {
        if (std::is_constant_evaluated() == false)
            VBAIL_ERROR_IF(index > 3, columns[0], "Matrix4 index must be in the range [0, 3]: {}", index);

        return columns[index];
    }

    constexpr Vector4 row(std::size_t index) const
    {
        if (std::is_constant_evaluated() == false)
            VBAIL_ERROR_IF(index > 3, row(0), "Matrix4 index must be in the range [0, 3]: {}", index);

        return {columns[0][index], columns[1][index], columns[2][index], columns[3][index]};
    }
};

std::ostream &operator<<(std::ostream &out, const Matrix4 &matrix);

constexpr Matrix3::Matrix3(const Matrix4 &matrix)
    : columns{xyz(matrix[0]), xyz(matrix[1]), xyz(matrix[2])}
{
}

template <typename T>
concept IsMatrix = std::same_as<T, Matrix3> || std::same_as<T, Matrix4>;

template <IsMatrix T>
constexpr T operator-(const T &matrix)
{
    T result{matrix};
    for (std::size_t column{}; column < T::COLUMN_COUNT; ++column)
        result[column] *= -1.0f;
    return result;
}

template <IsMatrix T>
constexpr T operator+(const T &lhs, const T &rhs)
{
    T result{lhs};
    result += rhs;
    return result;
}

template <IsMatrix T>
constexpr T &operator+=(T &lhs, const T &rhs)
{
    for (std::size_t column{}; column < T::COLUMN_COUNT; ++column)
        lhs[column] += rhs[column];
    return lhs;
}

template <IsMatrix T>
constexpr T operator-(const T &lhs, const T &rhs)
{
    T result{lhs};
    result -= rhs;
    return result;
}

template <IsMatrix T>
constexpr T &operator-=(T &lhs, const T &rhs)
{
    for (std::size_t column{}; column < T::COLUMN_COUNT; ++column)
        lhs[column] -= rhs[column];
    return lhs;
}

template <IsMatrix T>
constexpr T operator*(const T &matrix, float scalar)
{
    T result{matrix};
    result *= scalar;
    return result;
}

template <IsMatrix T>
constexpr T &operator*=(T &matrix, float scalar)
{
    for (std::size_t column{}; column < T::COLUMN_COUNT; ++column)
        matrix[column] *= scalar;
    return matrix;
}

constexpr Vector3 operator*(const Matrix3 &matrix, const Vector3 &vector)
{
    return {dot(matrix.row(0), vector), dot(matrix.row(1), vector), dot(matrix.row(2), vector)};
}

constexpr Vector4 operator*(const Matrix4 &matrix, const Vector4 &vector)
{
    return {
        dot(matrix.row(0), vector), dot(matrix.row(1), vector), dot(matrix.row(2), vector), dot(matrix.row(3), vector)
    };
}

template <IsMatrix T>
constexpr T operator*(const T &lhs, const T &rhs)
{
    T result{};
    for (std::size_t row{}; row < T::ROW_COUNT; ++row)
        for (std::size_t column{}; column < T::COLUMN_COUNT; ++column)
            result[column][row] = dot(lhs.row(row), rhs[column]);
    return result;
}

template <IsMatrix T>
constexpr T &operator*=(T &lhs, const T &rhs)
{
    lhs = lhs * rhs;
    return lhs;
}

template <IsMatrix T>
constexpr T scale(const T &lhs, const T &rhs)
{
    T result{};
    for (std::size_t column{}; column < T::COLUMN_COUNT; ++column)
        for (std::size_t row{}; row < T::ROW_COUNT; ++row)
            result[column][row] = lhs[column][row] * rhs[column][row];
    return result;
}

constexpr float determinant(const Vector2 &col1, const Vector2 &col2)
{
    return col1.x * col2.y - col2.x * col1.y;
}

constexpr float determinant(const Vector3 &col1, const Vector3 &col2, const Vector3 &col3)
{
    return col1.x * determinant(yz(col2), yz(col3)) - col2.x * determinant(yz(col1), yz(col3)) +
           col3.x * determinant(yz(col1), yz(col2));
}

constexpr float determinant(const Matrix3 &matrix)
{
    return determinant(matrix[0], matrix[1], matrix[2]);
}

constexpr float determinant(const Vector4 &col1, const Vector4 &col2, const Vector4 &col3, const Vector4 &col4)
{
    return col1.x * determinant(yzw(col2), yzw(col3), yzw(col4)) -
           col2.x * determinant(yzw(col1), yzw(col3), yzw(col4)) +
           col3.x * determinant(yzw(col1), yzw(col2), yzw(col4)) -
           col4.x * determinant(yzw(col1), yzw(col2), yzw(col3));
}

constexpr float determinant(const Matrix4 &matrix)
{
    return determinant(matrix[0], matrix[1], matrix[2], matrix[3]);
}

constexpr Matrix3 transpose(const Matrix3 &matrix)
{
    Matrix3 transpose{};
    transpose[0].x = matrix[0].x;
    transpose[0].y = matrix[1].x;
    transpose[0].z = matrix[2].x;
    transpose[1].x = matrix[0].y;
    transpose[1].y = matrix[1].y;
    transpose[1].z = matrix[2].y;
    transpose[2].x = matrix[0].z;
    transpose[2].y = matrix[1].z;
    transpose[2].z = matrix[2].z;
    return transpose;
}

constexpr Matrix4 transpose(const Matrix4 &matrix)
{
    Matrix4 transpose{};
    transpose[0].x = matrix[0].x;
    transpose[0].y = matrix[1].x;
    transpose[0].z = matrix[2].x;
    transpose[0].w = matrix[3].x;
    transpose[1].x = matrix[0].y;
    transpose[1].y = matrix[1].y;
    transpose[1].z = matrix[2].y;
    transpose[1].w = matrix[3].y;
    transpose[2].x = matrix[0].z;
    transpose[2].y = matrix[1].z;
    transpose[2].z = matrix[2].z;
    transpose[2].w = matrix[3].z;
    transpose[3].x = matrix[0].w;
    transpose[3].y = matrix[1].w;
    transpose[3].z = matrix[2].w;
    transpose[3].w = matrix[3].w;
    return transpose;
}

constexpr Matrix3 inverse(const Matrix3 &matrix)
{
    Matrix3 adjugate{};
    adjugate[0].x = determinant(yz(matrix[1]), yz(matrix[2]));
    adjugate[0].y = -determinant(yz(matrix[0]), yz(matrix[2]));
    adjugate[0].z = determinant(yz(matrix[0]), yz(matrix[1]));
    adjugate[1].x = -determinant(xz(matrix[1]), xz(matrix[2]));
    adjugate[1].y = determinant(xz(matrix[0]), xz(matrix[2]));
    adjugate[1].z = -determinant(xz(matrix[0]), xz(matrix[1]));
    adjugate[2].x = determinant(xy(matrix[1]), xy(matrix[2]));
    adjugate[2].y = -determinant(xy(matrix[0]), xy(matrix[2]));
    adjugate[2].z = determinant(xy(matrix[0]), xy(matrix[1]));

    float determinant{matrix[0].x * adjugate[0].x + matrix[1].x * adjugate[0].y + matrix[2].x * adjugate[0].z};
    adjugate *= 1.0f / determinant;
    return adjugate;
}

constexpr Matrix4 inverse(const Matrix4 &matrix)
{
    Matrix4 adjugate{};
    adjugate[0].x = determinant(yzw(matrix[1]), yzw(matrix[2]), yzw(matrix[3]));
    adjugate[0].y = -determinant(yzw(matrix[0]), yzw(matrix[2]), yzw(matrix[3]));
    adjugate[0].z = determinant(yzw(matrix[0]), yzw(matrix[1]), yzw(matrix[3]));
    adjugate[0].w = -determinant(yzw(matrix[0]), yzw(matrix[1]), yzw(matrix[2]));
    adjugate[1].x = -determinant(xzw(matrix[1]), xzw(matrix[2]), xzw(matrix[3]));
    adjugate[1].y = determinant(xzw(matrix[0]), xzw(matrix[2]), xzw(matrix[3]));
    adjugate[1].z = -determinant(xzw(matrix[0]), xzw(matrix[1]), xzw(matrix[3]));
    adjugate[1].w = determinant(xzw(matrix[0]), xzw(matrix[1]), xzw(matrix[2]));
    adjugate[2].x = determinant(xyw(matrix[1]), xyw(matrix[2]), xyw(matrix[3]));
    adjugate[2].y = -determinant(xyw(matrix[0]), xyw(matrix[2]), xyw(matrix[3]));
    adjugate[2].z = determinant(xyw(matrix[0]), xyw(matrix[1]), xyw(matrix[3]));
    adjugate[2].w = -determinant(xyw(matrix[0]), xyw(matrix[1]), xyw(matrix[2]));
    adjugate[3].x = -determinant(xyz(matrix[1]), xyz(matrix[2]), xyz(matrix[3]));
    adjugate[3].y = determinant(xyz(matrix[0]), xyz(matrix[2]), xyz(matrix[3]));
    adjugate[3].z = -determinant(xyz(matrix[0]), xyz(matrix[1]), xyz(matrix[3]));
    adjugate[3].w = determinant(xyz(matrix[0]), xyz(matrix[1]), xyz(matrix[2]));

    float determinant{
        matrix[0].x * adjugate[0].x + matrix[1].x * adjugate[0].y + matrix[2].x * adjugate[0].z +
        matrix[3].x * adjugate[0].w
    };
    adjugate *= 1.0f / determinant;
    return adjugate;
}
} // namespace Age::Math
