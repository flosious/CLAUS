/*
#include "parser_methods.hpp"


bool ref_sample_params_t::parse(string* filename_p, string* contents) 
{
// 	tools::str::
// 	int parsed_lines=0;
//     for (int i=0; i<config_lines.size();i++) {
//         vector<string> string_parts = tools::str::get_strings_between_delimiter(config_lines[i],"=");
//         if (string_parts.size()<2) continue; // no "=" in line, skip
//         tools::str::remove_spaces_from_string_start(&string_parts[0]);
//         tools::str::remove_spaces_from_string_end(&string_parts[0]);
//         tools::str::remove_spaces_from_string_start(&string_parts[1]);
//         tools::str::remove_spaces_from_string_end(&string_parts[1]);
//         tools::str::replace_chars(&string_parts,"\"","");
//         string key = string_parts[0];
//         string value = string_parts[1];
// 		key = tools::str::remove_linebreakers_from_string(key);
// 		value = tools::str::remove_linebreakers_from_string(value);
//         parsed_lines++;
// // 		cout << "key=" << key << "|value=" << value << endl;
//         if ((key.compare(0,1,"#")==0) || (key.compare(0,2,"//")==0) || (key.compare(0,1,";")==0) ) 
// 		{
// 			parsed_lines--;
// 			continue; // comments, skip
// 		}
//         else if (key.compare("test")==0) save_test(value);
// 		else if (key=="sputter_time_resolution") sputter_time_resolution=tools::str::str_to_double(value);
// 		else if (key=="depth_resolution") depth_resolution=tools::str::str_to_double(value);
// 		else if (key=="pse") save_pse_file_locations(value);
// 		else if (key=="input_files_are_one_measurement_group") { if (value.find("1")!=string::npos) input_files_are_one_measurement_group=true; } // 
// 		else if (key=="ignore_file_type_endings") { if (value.find("1")!=string::npos) ignore_file_type_endings=true; }
//         else if (key=="ignore_filename_substrings") save_ignore_filename_substrings(value);
// 		else if (key=="ignore_filename") save_ignore_filename_substrings(value);
// 		else if (key=="threads") save_threads(value);
//         else if (key=="filename_format") save_filename_format(value);
//         else if (key=="db_location") save_db_location(value);
//         else if (key=="references_location") save_references_location(value);
//         else if (key=="plots_location") save_plots_location(value);
//         else if (key=="results_location") save_results_location(value);
//         else if (key=="calc_location") save_calc_location(value);
//         else if (key=="export_location") save_export_location(value);
//         else if (key=="matrix_isotopes") save_matrix_isotopes(value);
//         else if (key=="output_format") save_output_format(string_parts);
//         else if (key=="replace" || key=="replacements") save_replacements(string_parts);
// //         else if (key=="measurement_file_types") save_measurement_file_types(value);
//         else if (key=="measurement_tool") save_measurement_tool(value);
//         else if (key=="data_column_delimiter") data_column_delimiter=value;
//         else if (key=="file_name_delimiter") file_name_delimiter=value;
//         else if (key=="use_directory_files_list") { if (value.find("1")!=string::npos) use_directory_files_list=true; }
//         else parsed_lines--;
//         
//     }
//     return parsed_lines;
	return true;
}*/
