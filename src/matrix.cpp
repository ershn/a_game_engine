#include "matrix.hpp"

namespace Age::Math
{
Matrix3::operator const float *() const
{
    return static_cast<const float *>(columns[0]);
}

std::ostream &operator<<(std::ostream &out, const Matrix3 &matrix)
{
    out << std::showpoint << '{';
    for (std::size_t row{}; row < 3; ++row)
    {
        for (std::size_t column{}; column < 3; ++column)
        {
            out << matrix[column][row];
            if (column < 2)
                out << ", ";
        }
        if (row < 2)
            out << ",\n ";
    }
    out << "}\n";
    return out;
}

Matrix4::operator const float *() const
{
    return static_cast<const float *>(columns[0]);
}

std::ostream &operator<<(std::ostream &out, const Matrix4 &matrix)
{
    out << std::showpoint << '{';
    for (std::size_t row{}; row < 4; ++row)
    {
        for (std::size_t column{}; column < 4; ++column)
        {
            out << matrix[column][row];
            if (column < 3)
                out << ", ";
        }
        if (row < 3)
            out << ",\n ";
    }
    out << "}\n";
    return out;
}
} // namespace Age::Math
