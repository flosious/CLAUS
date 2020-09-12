/*
	Copyright (C) 2020 Florian Bärwolf
	floribaer@gmx.de

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

#include "quantity.hpp"
#include "gsl/gsl_const_mksa.h"

class constants
{
public:
	///speed of light
	static quantity_t c();
	///planck
	static quantity_t h();
	///avogadro
	static quantity_t Na();
	///Faraday
	static quantity_t F();
	///Blotzmann
	static quantity_t k();
	///molar Gas
	static quantity_t R0();
	/// permeability vacuum
	static quantity_t mu0();
	/// permittivity vacuum
	static quantity_t epsilon0();
	///gravitation
	static quantity_t G();
	/// solar constants
	static quantity_t S();
	///proton mass
	static quantity_t mp();
	///electron mass
	static quantity_t me();
	/// electron charge
	static quantity_t e();
	/// atomic mass
	static quantity_t u();
	/// atomic mass
	static quantity_t amu();
	///bohr radius
	static quantity_t a0();
	///calorie
	static quantity_t cal();
};

#endif // CONSTANTS_HPP
