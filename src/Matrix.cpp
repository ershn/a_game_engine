#include <sstream>

#include "Matrix.hpp"

namespace Age::Math
{
Matrix3::Matrix3()
    : _columns{}
{
}

Matrix3::Matrix3(float scalar)
    : _columns{{scalar, 0.0f, 0.0f}, {0.0f, scalar, 0.0f}, {0.0f, 0.0f, scalar}}
{
}

Matrix3::Matrix3(const Vector3 &col1, const Vector3 &col2, const Vector3 &col3)
    : _columns{col1, col2, col3}
{
}

Matrix3::operator const float *() const
{
    return static_cast<const float *>(_columns[0]);
}

const Vector3 &Matrix3::operator[](std::size_t index) const
{
    return _columns[index];
}

Vector3 &Matrix3::operator[](std::size_t index)
{
    return _columns[index];
}

Vector3 Matrix3::row(std::size_t index) const
{
    return {_columns[0][index], _columns[1][index], _columns[2][index]};
}

Matrix3 Matrix3::operator-() const
{
    Matrix3 result{*this};
    result[0] *= -1.0f;
    result[1] *= -1.0f;
    result[2] *= -1.0f;
    return result;
}

Matrix3 Matrix3::operator*(float scalar) const
{
    Matrix3 result{*this};
    result *= scalar;
    return result;
}

Matrix3 &Matrix3::operator*=(float scalar)
{
    _columns[0] *= scalar;
    _columns[1] *= scalar;
    _columns[2] *= scalar;
    return *this;
}

Vector3 Matrix3::operator*(const Vector3 &rhs) const
{
    Vector3 result{};
    result[0] = row(0).dot(rhs);
    result[1] = row(1).dot(rhs);
    result[2] = row(2).dot(rhs);
    return result;
}

Matrix3 Matrix3::operator+(const Matrix3 &rhs) const
{
    Matrix3 result{*this};
    result += rhs;
    return result;
}

Matrix3 &Matrix3::operator+=(const Matrix3 &rhs)
{
    _columns[0] += rhs[0];
    _columns[1] += rhs[1];
    _columns[2] += rhs[2];
    return *this;
}

Matrix3 Matrix3::operator-(const Matrix3 &rhs) const
{
    Matrix3 result{*this};
    result -= rhs;
    return result;
}

Matrix3 &Matrix3::operator-=(const Matrix3 &rhs)
{
    _columns[0] -= rhs[0];
    _columns[1] -= rhs[1];
    _columns[2] -= rhs[2];
    return *this;
}

Matrix3 Matrix3::operator*(const Matrix3 &rhs) const
{
    Matrix3 result{};
    for (std::size_t i{}; i < 3; i++)
        for (std::size_t j{}; j < 3; j++)
            result[j][i] = row(i).dot(rhs[j]);
    return result;
}

Matrix3 &Matrix3::operator*=(const Matrix3 &rhs)
{
    *this = (*this) * rhs;
    return *this;
}

Matrix3 &Matrix3::transpose()
{
    Matrix3 copy{*this};
    _columns[0].y = copy[1].x;
    _columns[0].z = copy[2].x;
    _columns[1].x = copy[0].y;
    _columns[1].z = copy[2].y;
    _columns[2].x = copy[0].z;
    _columns[2].y = copy[1].z;
    return *this;
}

Matrix3 Matrix3::transposed() const
{
    Matrix3 result{*this};
    result.transpose();
    return result;
}

std::string Matrix3::to_string() const
{
    std::stringstream ss{};
    for (std::size_t i{}; i < 3; i++)
    {
        ss << '|';
        for (std::size_t j{}; j < 3; j++)
        {
            if (j > 0)
                ss << ", ";
            ss << _columns[j][i];
        }
        ss << "|\n";
    }
    return ss.str();
}

Matrix4::Matrix4()
    : _columns{}
{
}

Matrix4::Matrix4(float scalar)
    : _columns{{scalar, 0.0f, 0.0f, 0.0f},
               {0.0f, scalar, 0.0f, 0.0f},
               {0.0f, 0.0f, scalar, 0.0f},
               {0.0f, 0.0f, 0.0f, scalar}}
{
}

Matrix4::Matrix4(const Vector4 &col1, const Vector4 &col2, const Vector4 &col3, const Vector4 &col4)
    : _columns{col1, col2, col3, col4}
{
}

Matrix4::operator const float *() const
{
    return static_cast<const float *>(_columns[0]);
}

const Vector4 &Matrix4::operator[](std::size_t index) const
{
    return _columns[index];
}

Vector4 &Matrix4::operator[](std::size_t index)
{
    return _columns[index];
}

Matrix3 Matrix4::to_matrix3() const
{
    return Matrix3{_columns[0].xyz(), _columns[1].xyz(), _columns[2].xyz()};
}

Vector4 Matrix4::row(std::size_t index) const
{
    return {_columns[0][index], _columns[1][index], _columns[2][index], _columns[3][index]};
}

Matrix4 Matrix4::operator-() const
{
    Matrix4 result{*this};
    result[0] *= -1.0f;
    result[1] *= -1.0f;
    result[2] *= -1.0f;
    result[3] *= -1.0f;
    return result;
}

Matrix4 Matrix4::operator*(float scalar) const
{
    Matrix4 result{*this};
    result *= scalar;
    return result;
}

Matrix4 &Matrix4::operator*=(float scalar)
{
    _columns[0] *= scalar;
    _columns[1] *= scalar;
    _columns[2] *= scalar;
    _columns[3] *= scalar;
    return *this;
}

Vector4 Matrix4::operator*(const Vector4 &rhs) const
{
    Vector4 result{};
    result[0] = row(0).dot(rhs);
    result[1] = row(1).dot(rhs);
    result[2] = row(2).dot(rhs);
    result[3] = row(3).dot(rhs);
    return result;
}

Matrix4 Matrix4::operator+(const Matrix4 &rhs) const
{
    Matrix4 result{*this};
    result += rhs;
    return result;
}

Matrix4 &Matrix4::operator+=(const Matrix4 &rhs)
{
    _columns[0] += rhs[0];
    _columns[1] += rhs[1];
    _columns[2] += rhs[2];
    _columns[3] += rhs[3];
    return *this;
}

Matrix4 Matrix4::operator-(const Matrix4 &rhs) const
{
    Matrix4 result{*this};
    result -= rhs;
    return result;
}

Matrix4 &Matrix4::operator-=(const Matrix4 &rhs)
{
    _columns[0] -= rhs[0];
    _columns[1] -= rhs[1];
    _columns[2] -= rhs[2];
    _columns[3] -= rhs[3];
    return *this;
}

Matrix4 Matrix4::operator*(const Matrix4 &rhs) const
{
    Matrix4 result{};
    for (std::size_t i{}; i < 4; i++)
        for (std::size_t j{}; j < 4; j++)
            result[j][i] = row(i).dot(rhs[j]);
    return result;
}

Matrix4 &Matrix4::operator*=(const Matrix4 &rhs)
{
    *this = (*this) * rhs;
    return *this;
}

Matrix4 &Matrix4::transpose()
{
    Matrix4 copy{*this};
    _columns[0].y = copy[1].x;
    _columns[0].z = copy[2].x;
    _columns[0].w = copy[3].x;
    _columns[1].x = copy[0].y;
    _columns[1].z = copy[2].y;
    _columns[1].w = copy[3].y;
    _columns[2].x = copy[0].z;
    _columns[2].y = copy[1].z;
    _columns[2].w = copy[3].z;
    _columns[3].x = copy[0].w;
    _columns[3].y = copy[1].w;
    _columns[3].z = copy[2].w;
    return *this;
}

Matrix4 Matrix4::transposed() const
{
    Matrix4 result{*this};
    result.transpose();
    return result;
}

std::string Matrix4::to_string() const
{
    std::stringstream ss{};
    for (std::size_t i{}; i < 4; i++)
    {
        ss << '|';
        for (std::size_t j{}; j < 4; j++)
        {
            if (j > 0)
                ss << ", ";
            ss << _columns[j][i];
        }
        ss << "|\n";
    }
    return ss.str();
}
} // namespace Age::Math