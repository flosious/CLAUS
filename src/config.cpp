// config.cpp
/* can load multiple config files (*.cfg, *.conf)
 * parses them
 * uses general expressions like:
 *  parameter_A = "abcdefg"
 *  parameter_B = 12352E2
 */

#include "config.hpp"


/// global variable
// config_t conf;

config_t::config_t() {
    return;
}

vector<string> config_t::filter_files(vector<string> * file_list) {
    return tools::file::filter_files_from_types(file_list, &file_endings,false);
}

int config_t::load_file(std::string filename_with_path) {
    vector<string> config_lines = tools::file::load_file_to_vector_string(filename_with_path);
    if (config_lines.size()==0) return 0;
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
		
		else if (key=="origin_suffix") 																origin_t::column_t::suffix_conf=value;
		else if (key=="origin_prefix") 																origin_t::column_t::prefix_conf=value;
		else if (key=="standard_reference_intensity_calculation_method") 							cluster_t::standard_reference_intensity_calculation_method = value;

		else if (key=="pse") 																		pse_t::file_locations.push_back(value);
		else if (key=="sql_db" || key=="db_location") 												database_t::file_location = value;
		else if (key=="ignore_file_type_endings") { if (value.find("1")!=string::npos) 				measurement_tools_t::ignore_file_type_endings=true; }
		
		else if (key=="print_errors") { if (value.find("1")!=string::npos) 													processor::print_errors=true; }
		else if (key=="use_jiang" || key=="jaing")  { if (value.find("0")!=string::npos) 									processor::use_jiang=false;}
		else if (key=="use_mass_interference_filter" || key=="mass_interference_filter") { if (value.find("1")!=string::npos) processor::use_mass_interference_filter=true;}
		else if (key=="force_RSF_to_foreign_matrix" || key=="force_RSF") { if (value.find("1")!=string::npos) 				processor::force_RSF_to_foreign_matrix=true;}
		
		else if (key=="use_directory_files_list") { if (value.find("1")!=string::npos) 				files_t::use_directory_files_list=true;}
        else if (key=="use_wildcards_in_filenames") { if (value.find("1")!=string::npos) 			files_t::use_wildcards_in_filenames=true; }
        else if (key=="search_sub_directories") { if (value.find("1")!=string::npos) 				files_t::search_sub_directories=true; }
        else if (key=="ignore_filename_substrings" || key=="ignore_filename") 						files_t::ignore_filename_substrings.push_back(value);
        else if (key=="plots_location") 															plot_t::plots_location=value;
		
		else if (key=="export_MG_parameters") { if (value.find("0")!=string::npos) 											export2_t::export_MG_parameters=false;}
		else if (key=="export_calculation_history" || key=="export_calc_history") { if (value.find("0")!=string::npos) 		export2_t::export_calc_history=false;}
		else if (key=="export_calculation_results" || key=="export_calc_results") { if (value.find("0")!=string::npos) 		export2_t::export_calc_results=false;}
        else if (key=="calc_location") 																export2_t::calc_location = value;
        else if (key=="export_location" || key=="export_directory") 								export2_t::directory_config=value;
		else if (key=="export_filename")															export2_t::filename_config=value;
		else if (key=="smoothing_moving_window_mean_size") 											export2_t::smoothing_moving_window_mean_size = tools::str::str_to_int(value);
		else if (key=="depth_resolution") 															export2_t::depth_resolution=tools::str::str_to_double(value);
        else if (key=="replace" || key=="replacements") 											export2_t::replacements [ string_parts[1] ] = string_parts[2];
		else if (key=="data_column_delimiter") 														export2_t::data_column_delimiter=value;
		
		else if (key == "dose_or_max_treshold")														 cluster_t::dose_or_Max__treshold = tools::str::str_to_int(value);
		else if (key == "sr_polynom_rank")														calc_models_t::jiang_t::SR_polynom_rank = tools::str::str_to_int(value);
		else if (key == "crel_polynom_rank")													calc_models_t::jiang_t::Crel_polynom_rank = tools::str::str_to_int(value);
		else if (key == "rsf_polynom_rank")														calc_models_t::jiang_t::RSF_polynom_rank = tools::str::str_to_int(value);
		else if (key=="measurement_group_definition") 												save_measurement_group_definition(value);
		else if (key=="measurement_definition") 													save_measurement_definition(value);
		else if (key=="export_columns" || key=="export_column_names")								save_export_column_names(value);

        else if (key=="use_impulse_filter_on_data") { if (value.find("0")!=string::npos) 			parser_methods::use_impulse_filter_on_data=false;}
        else 
		{
			cout << "config_t::\tCould not parse: " << config_lines[i] << endl;
			parsed_lines--;
		}
// 		cout << "KEY=" << key << "\tvalue=" << value << endl;
        
    }
    
    return parsed_lines;
	
}


void config_t::save_test(string value) 
{
    cout << "save_test(): '" << value << "'" << endl;
}


void config_t::save_export_column_names(string value)
{
	tools::str::remove_spaces(&value);
	export2_t::export_column_names = tools::str::get_strings_between_delimiter(value,"+");
	tools::str::remove_spaces(&export2_t::export_column_names);
}

void config_t::save_measurement_group_definition(string value)
{
	tools::str::remove_spaces(&value);
	vector<string> definitions = tools::str::get_strings_between_delimiter(value,"+");
	tools::str::remove_spaces(&definitions);
	
	measurement_group_t::defined_directory=false;
	measurement_group_t::defined_olcdbid=false;
	measurement_group_t::defined_groupid=false;
	measurement_group_t::defined_settings=false;
	measurement_group_t::defined_tool=false;
	
	for (auto& definition:definitions)
	{
		if (definition=="olcdbid"  || definition=="olcdb" ) measurement_group_t::defined_olcdbid=true;
		if (definition=="folder"  || definition=="directory" ) measurement_group_t::defined_directory=true;
		else if (definition=="groupid" || definition=="group") measurement_group_t::defined_groupid=true;
		else if (definition=="settings") measurement_group_t::defined_settings=true;
		else if (definition=="tool") measurement_group_t::defined_tool=true;
	}
}

void config_t::save_measurement_definition(string value)
{
	tools::str::remove_spaces(&value);
	vector<string> definitions = tools::str::get_strings_between_delimiter(value,"+");
	tools::str::remove_spaces(&definitions);
	
	measurement_t::defined_olcdbid				=false;
	measurement_t::defined_lot					=false;
	measurement_t::defined_lot_split			=false;
	measurement_t::defined_wafer				=false;
	measurement_t::defined_monitor				=false;
	measurement_t::defined_chip					=false;
	measurement_t::defined_groupid				=false;
	measurement_t::defined_repetition			=false;
	measurement_t::defined_polarity				=false;
	measurement_t::defined_sputter_element		=false;
	measurement_t::defined_sputter_energy		=false;
	
	for (auto& definition:definitions)
	{
		if (definition=="olcdbid" || definition=="olcdb") measurement_t::defined_olcdbid=true;
		else if (definition=="lot") measurement_t::defined_lot=true;
		else if (definition=="lot_split") measurement_t::defined_lot_split=true;
		else if (definition=="wafer") measurement_t::defined_wafer=true;
		else if (definition=="monitor") measurement_t::defined_monitor=true;
		else if (definition=="chip") measurement_t::defined_chip=true;
		else if (definition=="groupid" || definition=="group") measurement_t::defined_groupid=true;
		else if (definition=="repetition") measurement_t::defined_repetition=true;
		else if (definition=="sputter_energy") measurement_t::defined_sputter_energy=true;
		else if (definition=="polarity") measurement_t::defined_polarity=true;
		else if (definition=="sputter_element") measurement_t::defined_sputter_element=true;
	}
}
