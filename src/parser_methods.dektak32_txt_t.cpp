/*
	Copyright (C) 2020 Florian Bärwolf
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
*/
#include "parser_methods.dektak32_txt_t.hpp"

dektak32_txt_t::dektak32_txt_t()
{
	/// overwrites parser_methods::tool_name
	tool_name = "dektak32";
	/// overwrites parser_methods::file_type_endings
	file_type_endings = {"txt"};
	/// this is not a primary measurement
	set_secondary_measurement();
}

bool dektak32_txt_t::check() {
	if (file_contents->find("DEKTAK32")!=string::npos) return true;
	if (file_contents->find("Stylus Type")!=string::npos) return true;
	return false;
}

/// Order is important
bool dektak32_txt_t::parse(filename_t* filename_p, string* contents)
{
	filename = filename_p;
	file_contents = contents;
	if (!check()) 
	{
// 		error_messages.push_back("dektak32_txt_t::parse:!check()");
		return false;
	}
	if (!parse_linescan()) 
	{
		error_messages.push_back("dektak32_txt_t::parse:!parse_linescan()");
		return false;
	}
// 	copy_to_measurement_p();
	return true;
}


bool dektak32_txt_t::parse_linescan()
{
	
	if (!parse_data_and_header_tensors(",")) return false;
	if (raw_data_tensor.size()==0) return false;
	if (raw_data_tensor[0].size()==0) return false;
	if (raw_data_tensor[0][0].size()==0) return false;

	vector<vector<double>> data_cols_lines = tools::mat::transpose_matrix(tools::mat::str_matrix_to_double_matrix(raw_data_tensor[0]));
	
	linescan_t linescan;
	
	linescan.xy.name="linescan_xy";
	linescan.xy.dimension="length";
	linescan.xy.unit="um";
	linescan.xy.data=data_cols_lines[0];
	
	linescan.z.name="linescan_z";
	linescan.z.dimension="length";
	linescan.z.unit="nm";
	linescan.z.data=data_cols_lines[1];
	
	measurement_p.crater.linescans.push_back(linescan);
	
	return true;
}
