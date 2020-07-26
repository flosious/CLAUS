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
#ifndef CALC_MODELS_HPP
#define CALC_MODELS_HPP

#include "quantity.hpp"
#include "measurement.hpp"
#include "cluster_t.hpp"
#include "fit_functions.hpp"
#include "measurement_group_t.hpp"

/*
 * classes for advanced physical-mathematical models for more complex calculation
 */

using namespace std;

// class measurement_group_t;

class calc_models_t
{
public:
	/// jiangs protocol 
	class jiang_t;
	/// jiangs protocol for 2 matrix elements for all possible mixtures
	class jiang_full_t;
	/// fully relative model
	class fully_relative_t;
};


#endif // CALC_MODELS_HPP
