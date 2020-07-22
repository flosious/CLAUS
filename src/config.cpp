// config.cpp
/* can load multiple config files (*.cfg, *.conf)
 * parses them
 * uses general expressions like:
 *  parameter_A = "abcdefg"
 *  parameter_B = 12352E2
 */

#include "config.hpp"

/// global variable
config_t conf;

config_t::config_t() {
    return;
}

vector<string> config_t::filter_files(vector<string> * file_list) {
    return tools::file::filter_files_from_types(file_list, &file_endings,false);
}

int config_t::load_file(std::string filename_with_path) {
    vector<string> config_lines = tools::file::load_file_to_vector_string(filename_with_path);
    if (config_lines.size()==0) return 0;
	cout << "config_lines.size(): " << config_lines.size() << endl;
    if (parse(config_lines)>0) return 1;
    return 0;
}
int config_t::load() {
    files.push_back(default_file);
    return load(files);
}
int config_t::load(std::string filename_with_path) {
    if (filename_with_path.length()==0) files.push_back(default_file);
    files.push_back(filename_with_path);
    return load(files);
}
int config_t::load(std::vector<std::string> filenames_with_path) {
    // get just the config files
    files = filter_files(&filenames_with_path);
    // addd default conf to list of config files
    if (files.size()==0) files.push_back(default_file);
    // clear double entries
    tools::str::remove_duplicates(&files);
    // load the configs to RAM
    for (int i=0; i<files.size();i++) {
        if (load_file(files[i]) == 1) {
            files_loaded.push_back(files[i]);
        }
    }
    return files_loaded.size();
}

int config_t::parse(vector<string> config_lines) {
	int parsed_lines=0;
    for (int i=0; i<config_lines.size();i++) {
        vector<string> string_parts = tools::str::get_strings_between_delimiter(config_lines[i],"=");
        if (string_parts.size()<2) continue; // no "=" in line, skip
        tools::str::remove_spaces_from_string_start(&string_parts[0]);
        tools::str::remove_spaces_from_string_end(&string_parts[0]);
        tools::str::remove_spaces_from_string_start(&string_parts[1]);
        tools::str::remove_spaces_from_string_end(&string_parts[1]);
        tools::str::replace_chars(&string_parts,"\"","");
        string key = string_parts[0];
        string value = string_parts[1];
		key = tools::str::remove_linebreakers_from_string(key);
		value = tools::str::remove_linebreakers_from_string(value);
        parsed_lines++;
// 		cout << "key=" << key << "|value=" << value << endl;
        if ((key.compare(0,1,"#")==0) || (key.compare(0,2,"//")==0) || (key.compare(0,1,";")==0) ) 
		{
			parsed_lines--;
			continue; // comments, skip
		}
        else if (key.compare("test")==0) save_test(value);
// 		standard_reference_intensity_calculation_method
		else if (key=="standard_reference_intensity_calculation_method") standard_reference_intensity_calculation_method = value;
		else if (key=="sputter_time_resolution") sputter_time_resolution=tools::str::str_to_double(value);
		else if (key=="depth_resolution") depth_resolution=tools::str::str_to_double(value);
		else if (key=="pse") save_pse_file_locations(value);
		else if (key=="sql_db" || key=="db_location") db_location = value;
		else if (key=="input_files_are_one_measurement_group") { if (value.find("1")!=string::npos) input_files_are_one_measurement_group=true; } // 
		else if (key=="ignore_file_type_endings") { if (value.find("1")!=string::npos) ignore_file_type_endings=true; }
		else if (key=="print_errors") { if (value.find("1")!=string::npos) print_errors=true; }
		else if (key=="use_jiang") { if (value.find("0")!=string::npos) use_jiang=false; }
        else if (key=="ignore_filename_substrings") save_ignore_filename_substrings(value);
		else if (key=="ignore_filename") save_ignore_filename_substrings(value);
		else if (key=="threads") save_threads(value);
//         else if (key=="filename_format") save_filename_format(value);
        else if (key=="references_location") save_references_location(value);
        else if (key=="plots_location") save_plots_location(value);
        else if (key=="results_location") save_results_location(value);
        else if (key=="calc_location") save_calc_location(value);
        else if (key=="export_location" || key=="export_directory") save_export_location(value);
		else if (key=="export_filename") export_filename=value;
//         else if (key=="matrix_isotopes") save_matrix_isotopes(value);
//         else if (key=="output_format") save_output_format(string_parts);
        else if (key=="replace" || key=="replacements") save_replacements(string_parts);
//         else if (key=="measurement_file_types") save_measurement_file_types(value);
//         else if (key=="measurement_tool") save_measurement_tool(value);
		else if (key=="measurement_group_definition") save_measurement_group_definition(value);
		else if (key=="measurement_definition") save_measurement_definition(value);
		else if (key=="export_columns" || key=="export_column_names") save_export_column_names(value);
        else if (key=="data_column_delimiter") data_column_delimiter=value;
        else if (key=="file_name_delimiter") file_name_delimiter=value;
        else if (key=="use_directory_files_list") { if (value.find("1")!=string::npos) use_directory_files_list=true; }
        else if (key=="use_wildcards_in_filenames") { if (value.find("1")!=string::npos) use_wildcards_in_filenames=true; }
        else parsed_lines--;
        
    }
    return parsed_lines;
}

/*save_functions*/
void config_t::save_ignore_filename_substrings(string value)
{
	ignore_filename_substrings.push_back(value);
}

// void config_t::save_output_format(vector<string> values) 
// {
//     output_format={};
//     for (int i=1;i<values.size();i++) 
//         output_format.push_back(values[i]);
// }
void config_t::save_test(string value) 
{
    cout << "save_test(): '" << value << "'" << endl;
}
void config_t::save_pse_file_locations(string value) 
{
    pse_file_locations.push_back(value);
}
void config_t::save_threads(string value) 
{
    threads_max=tools::str::str_to_int(value);
}
void config_t::save_references_location(string value) 
{
    references_location = value;
}
void config_t::save_plots_location(string value) 
{
    plots_location = value;
}
void config_t::save_results_location(string value) 
{
    results_location = value;
}
void config_t::save_calc_location(string value) 
{
    calc_location = value;
}
void config_t::save_export_location(string value) 
{
    export_location = value;
}
// void config_t::save_matrix_isotopes(string value) 
// {
//     matrix_isotopes = tools::str::get_strings_between_delimiter(value,",");
// }
void config_t::save_replacements(vector<string> values) 
{
    // replace [ "THIS" ] = "WITH";
    replacements [ values[1] ] = values[2];
}
// void config_t::save_measurement_file_types(string value) 
// {
//     tools::str::remove_spaces(&value);
//     measurement_file_types = tools::str::get_strings_between_delimiter(value,",");
// }
// void config_t::save_measurement_tool(string value) 
// {
//     measurement_tool = value;
// }

void config_t::save_export_column_names(std::__cxx11::string value)
{
	tools::str::remove_spaces(&value);
// 	export_column_names.clear();
	export_column_names = tools::str::get_strings_between_delimiter(value,"+");
	tools::str::remove_spaces(&export_column_names);
// 	print(export_column_names);
}

void config_t::save_measurement_group_definition(std::__cxx11::string value)
{
	tools::str::remove_spaces(&value);
	vector<string> definitions = tools::str::get_strings_between_delimiter(value,"+");
	tools::str::remove_spaces(&definitions);
	
	measurement_group_definition_olcdbid=false;
	measurement_group_definition_groupid=false;
	measurement_group_definition_settings=false;
	measurement_group_definition_tool=false;
	
	for (auto& definition:definitions)
	{
		if (definition=="olcdbid"  || definition=="olcdb" ) measurement_group_definition_olcdbid=true;
		else if (definition=="groupid" || definition=="group") measurement_group_definition_groupid=true;
		else if (definition=="settings") measurement_group_definition_settings=true;
		else if (definition=="tool") measurement_group_definition_tool=true;
	}
}

void config_t::save_measurement_definition(string value)
{
	tools::str::remove_spaces(&value);
	vector<string> definitions = tools::str::get_strings_between_delimiter(value,"+");
	tools::str::remove_spaces(&definitions);
	
	measurement_definition_olcdbid=false;
	measurement_definition_lot=false;
	measurement_definition_lot_split=false;
	measurement_definition_wafer=false;
	measurement_definition_monitor=false;
	measurement_definition_chip=false;
	measurement_definition_groupid=false;
	measurement_definition_repetition=false;
	
	measurement_definition_polarity=false;
	measurement_definition_sputter_element=false;
	measurement_definition_sputter_energy=false;
	
	for (auto& definition:definitions)
	{
		if (definition=="olcdbid" || definition=="olcdb") measurement_definition_olcdbid=true;
		else if (definition=="lot") measurement_definition_lot=true;
		else if (definition=="lot_split") measurement_definition_lot_split=true;
		else if (definition=="wafer") measurement_definition_wafer=true;
		else if (definition=="monitor") measurement_definition_monitor=true;
		else if (definition=="chip") measurement_definition_chip=true;
		else if (definition=="groupid" || definition=="group") measurement_definition_groupid=true;
		else if (definition=="repetition") measurement_definition_repetition=true;
		else if (definition=="sputter_energy") measurement_definition_sputter_energy=true;
		else if (definition=="polarity") measurement_definition_polarity=true;
		else if (definition=="sputter_element") measurement_definition_sputter_element=true;
	}
}
