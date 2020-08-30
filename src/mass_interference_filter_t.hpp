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
#ifndef MASS_INTERFERENCE_FILTER_HPP
#define MASS_INTERFERENCE_FILTER_HPP

#include "measurement.hpp"
#include "cluster_t.hpp"
#include "globalvars.hpp"

///
class mass_interference_filter_t
{
private:
public:
	static void substract_Ge_from_As(cluster_t* As_cluster);
};
#endif // MASS_INTERFERENCE_FILTER_HPP
