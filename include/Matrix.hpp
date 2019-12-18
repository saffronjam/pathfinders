#pragma once

#include "Funclib.hpp"

class Matrix
{
public:
	Matrix();
	Matrix(int const &_col, int const &_row, int const &_scale);
	float Get(int const &i, int const &j) const;
	void Set(int const &i, int const &j, float const &_coeff);
	void Resize(int const &_col, int const &_row);
	int GetRow() const;
	int GetCol() const;
	void SetScale(int const &_scale);
	int GetScale() const;

private:
	std::vector<float> m_coeff;
	int m_col;
	int m_row;
	int m_scale;
};

Matrix operator+(Matrix const &m1, Matrix const &m2);
Matrix operator-(Matrix const &m1, Matrix const &m2);
Matrix operator*(Matrix const &m, float const &k);
Matrix operator/(Matrix const &m, float const &k);