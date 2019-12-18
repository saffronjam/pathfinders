#include "Matrix.hpp"

Matrix::Matrix()
	: m_col(0),
	  m_row(0),
	  m_scale(1)
{
	m_coeff.clear();
}
Matrix::Matrix(int const &col, int const &row, int const &scale)
{
	m_col = col;
	m_row = row;
	Resize(m_col, m_row);
	m_scale = scale;
}
float Matrix::Get(int const &i, int const &j) const
{
	if (i > 0 && i < m_row && j > 0 && j < m_row)
		return m_coeff[i + (j - 1) * m_col];
	else
		return 0;
}
void Matrix::Set(int const &i, int const &j, float const &_coeff)
{
	if (i > 0 && i < m_row && j > 0 && j < m_row)
		m_coeff[i + gf::ConstrainLower(j - 1, 0) * m_col] = _coeff;
}
void Matrix::Resize(int const &col, int const &row)
{
	m_col = col;
	m_row = row;
	m_coeff.resize(m_col * m_row, 0);
}
int Matrix::GetRow() const
{
	return m_row;
}
int Matrix::GetCol() const
{
	return m_col;
}
void Matrix::SetScale(int const &scale)
{
	m_scale = scale;
}
int Matrix::GetScale() const
{
	return m_scale;
}

Matrix operator+(Matrix const &m1, Matrix const &m2)
{
	Matrix result(m1);
	for (int i = 0; i < m1.GetCol(); i++)
	{
		for (int j = 0; j < m1.GetCol(); j++)
		{
			result.Set(i, j, m1.Get(i, j) + m2.Get(i, j));
		}
	}
	return result;
}
Matrix operator-(Matrix const &m1, Matrix const &m2)
{
	Matrix result(m1);
	for (int i = 0; i < m1.GetCol(); i++)
	{
		for (int j = 0; j < m1.GetCol(); j++)
		{
			result.Set(i, j, m1.Get(i, j) - m2.Get(i, j));
		}
	}
	return result;
}
Matrix operator*(Matrix const &m, float k)
{
	Matrix result(m);
	for (int i = 0; i < m.GetCol(); i++)
	{
		for (int j = 0; j < m.GetCol(); j++)
		{
			result.Set(i, j, m.Get(i, j) * k);
		}
	}
	return result;
}
Matrix operator/(Matrix const &m, float k)
{
	Matrix result(m);
	for (int i = 0; i < m.GetCol(); i++)
	{
		for (int j = 0; j < m.GetCol(); j++)
		{
			result.Set(i, j, m.Get(i, j) / k);
		}
	}
	return result;
}
