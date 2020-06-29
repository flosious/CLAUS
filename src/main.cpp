/* 
 * 	CLever AUtomated Scientist
 * 	Version 2020-06-21_7	ALPHA
 * 	Florian Bärwolf 2017,2018,2019,2020
 *	
 * 
 *  Copyright (C) 2020 Florian Bärwolf
	baerwolf@ihp-microelectronics.com
	
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
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
                        *********************************************************************************************
                        *                                                                                           *
                        *                                       DEDICATION                                          *
                        *                                                                                           *
                        *   This program is dedicated to my uncle, Klaus Bärwolf, who never hesitated to help me.   *
                        *                                                                                           *
                        *********************************************************************************************
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 * Purpose:
 * 		This software mimics the solution of the daily tasks and routines of material scientists within a semiconductor fab: Using very few, but similar methods over and over again.
 * 
 * Motivation:
 * 		This software was created in the hope to be useful in automating calculation of raw data measurements (e.g. intensities) to characteristic material parameters (e.g. concentrations).
 * 		While doing that, it decreases operator errors, saves time and opens new opportunities in decreasing measurement uncertainties.
 * 
 * Functionality:
 * 		fast, modular and reliable
 * 
 * Input:
 * 		exported, raw measurement files of (different) measurement tools
 * 
 * Output:
 * 		evaluated measurement results as files
 *		graphs (needs gnuplot)
 * 
 * Necessities:
 * 		config file(s) (config.conf)
 * 		periodic table of elements (pse.csv)
 *		sqlite3 database filled with reference measurements
 */

#include "processor.hpp"

int main( int    argc, char **argv )
{ 
    processor Klaus(tools::str::args_to_vec(argc,argv));
    return 0;
}
