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
#ifndef GLOBALVARS_HPP
#define GLOBALVARS_HPP

// #if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__TOS_WIN__)
//     #define PATH_DELIMITER '\\'
//     #define LINE_DELIMITER string("\r\n")
// #elif defined(unix) || defined(__unix) || defined(__unix__) || defined(__APPLE__)
//     #define PATH_DELIMITER '/'
//     #define LINE_DELIMITER string("\n")
// #endif

#include <vector>
// #include "config.hpp"
// #include "database_t.hpp"

// singletons
// extern pse_t PSE;
// extern config_t conf;
// extern database_t db;
extern vector<string> calc_history;

#endif // GLOBALVARS
