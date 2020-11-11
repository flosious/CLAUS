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

#include "export.hpp"

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__TOS_WIN__)
    #define PATH_DELIMITER '\\'
//     #define LINE_DELIMITER string("\r\n")
#elif defined(unix) || defined(__unix) || defined(__unix__) || defined(__APPLE__)
    #define PATH_DELIMITER '/'
//     #define LINE_DELIMITER string("\n")
#endif


#define LOT_DEFAULT "lot-unkown"
#define WAFER_DEFAULT ""
#define OLCDB_DEFAULT "olcdbid-unkown"
#define GROUP_DEFAULT "group-unkown"
#define repetition_DEFAULT ""
#define TOOL_NAME_DEFAULT "tool_name-unkown"
#define MONITOR_DEFAULT ""
#define CHIP_DEFAULT ""
#define LOT_SPLIT_DEFAULT ""
#define ENERGY_DEFAULT ""

/*
 *
 * 
 * EXPORT!!!2!!!_T
 *
 *
 */

/* VARIABLES */

string export2_t::data_column_delimiter="\t";
string export2_t::filename_config="";
string export2_t::directory_config="";
string export2_t::calc_location="";
int export2_t::smoothing_moving_window_mean_size;
double export2_t::depth_resolution;
vector<string> export2_t::export_column_names;
map<string,string> export2_t::replacements;

string origin_t::column_t::prefix_conf="";
string origin_t::column_t::suffix_conf="";

bool export2_t::export_MG_parameters=true;
bool export2_t::export_calc_history=true;
bool export2_t::export_calc_results=true;

/*************/

void export2_t::export_contents_to_file(string contents, string filename_p, measurement_group_t MG, string sub_directory)
{
	if (contents.length()==0) return;
	if (filename_p.length()==0) return;
	set<string> root_dirs;
	for (auto& measurement:MG.measurements)
	{
		export2_t exp(measurement);
		root_dirs.insert(exp.root_directory());
	}
	
	
// 	string output = tools::vec::combine_vec_to_string(contents,"\n");
	string dir="";
	for (auto& root_dir:root_dirs)
	{
		dir = tools::file::check_directory_string(root_dir)+sub_directory;
		dir = tools::file::check_directory_string(dir);
		tools::file::write_to_file(dir+filename_p,&contents,false);
	}
	return;
}

void export2_t::export_contents_to_file(vector<std::__cxx11::string> contents, string filename_p, measurement_group_t MG, string sub_directory)
{
	export_contents_to_file(tools::vec::combine_vec_to_string(contents,"\n"),filename_p,MG,sub_directory);
	return;
}


export2_t::export2_t(measurement_t* measurement_p)
{
	measurement = measurement_p;
}

std::__cxx11::string export2_t::tool_name()
{
	if (measurement->tool_name()!="") return measurement->tool_name();
	return TOOL_NAME_DEFAULT;
}

std::__cxx11::string export2_t::chip()
{
	string chip_p;
	if (measurement->filename_p->chip_x>-1) 
	{
		if (measurement->filename_p->chip_x>9) chip_p = "X"+to_string(measurement->filename_p->chip_x);
		else chip_p = "X0"+to_string(measurement->filename_p->chip_x);
		if (measurement->filename_p->chip_y>9) chip_p += "Y"+to_string(measurement->filename_p->chip_y);
		else chip_p += "Y0"+to_string(measurement->filename_p->chip_y);
		return chip_p;
	}
	return CHIP_DEFAULT;
}

std::__cxx11::string export2_t::group()
{
// 	if (measurement->filename_p->group>-1) return "g"+to_string(measurement->filename_p->group)+measurement->filename_p->repetition;
	if (measurement->filename_p->group>-1) return "g"+to_string(measurement->filename_p->group);
	return GROUP_DEFAULT;
}

std::__cxx11::string export2_t::repetition()
{
	if (measurement->filename_p->repetition!="") return measurement->filename_p->repetition;
	return repetition_DEFAULT;
}

std::__cxx11::string export2_t::lot()
{
	if (measurement->filename_p->lot!="") return measurement->filename_p->lot;
	return LOT_DEFAULT;
}

std::__cxx11::string export2_t::olcdb()
{
	if (measurement->filename_p->olcdb>-1) return to_string(measurement->filename_p->olcdb);
	return OLCDB_DEFAULT;
}

std::__cxx11::string export2_t::monitor()
{
	if (measurement->filename_p->monitor!="") return "m"+measurement->filename_p->monitor;
	return MONITOR_DEFAULT;
}

std::__cxx11::string export2_t::lot_split()
{
	if (measurement->filename_p->lot_split!="") return measurement->filename_p->lot_split;
	return LOT_SPLIT_DEFAULT;
}

std::__cxx11::string export2_t::wafer()
{
	if (measurement->filename_p->wafer>9) return "w"+to_string(measurement->filename_p->wafer);
	if (measurement->filename_p->wafer>0) return "w0"+to_string(measurement->filename_p->wafer);
	return WAFER_DEFAULT;
}

std::__cxx11::string export2_t::energy()
{
	if (measurement->settings.sputter_energy().is_set()) 
		return to_string((int)(measurement->settings.sputter_energy().data[0]))+measurement->settings.sputter_energy().unit+measurement->settings.sputter_element()+measurement->settings.polarity();
	return ENERGY_DEFAULT;
}

std::__cxx11::string export2_t::others()
{
	if (measurement->filename_p->not_parseable_filename_parts.size()==0) return "";
	string others_str = tools::vec::combine_vec_to_string(measurement->filename_p->not_parseable_filename_parts,"_");
	return others_str;
}


void export2_t::check_replacements(std::__cxx11::string& check_this)
{
	for (auto& replacement:replacements)
	{
		tools::str::replace_chars(&check_this,replacement.first,replacement.second);
	}
}


void export2_t::check_placeholders(std::__cxx11::string& check_this)
{
	vector <string> placeholders = tools::str::get_all_string_between_string_A_and_next_B(&check_this,"{","}");
	for (string placeholder:placeholders)
	{
		string replacethis = "{"+placeholder+"}";
		if (placeholder=="olcdbid" || placeholder=="olcdb") tools::str::replace_chars(&check_this,"{"+placeholder+"}",olcdb());
		else if (placeholder=="waferid" || placeholder=="wafer") tools::str::replace_chars(&check_this,"{"+placeholder+"}",wafer());
		else if (placeholder=="groupid" || placeholder=="group") tools::str::replace_chars(&check_this,"{"+placeholder+"}",group());
		else if (placeholder=="lotid" || placeholder=="lot") tools::str::replace_chars(&check_this,"{"+placeholder+"}",lot());
		else if (placeholder=="lotsplit" || placeholder=="lot_split") tools::str::replace_chars(&check_this,"{"+placeholder+"}",lot_split());
		else if (placeholder=="chipid" || placeholder=="chip") tools::str::replace_chars(&check_this,"{"+placeholder+"}",chip());
		else if (placeholder=="monitorid" || placeholder=="monitor") tools::str::replace_chars(&check_this,"{"+placeholder+"}",monitor());
		else if (placeholder=="toolid" || placeholder=="tool" || placeholder=="tool_name") tools::str::replace_chars(&check_this,"{"+placeholder+"}",tool_name());
		else if (placeholder=="energy") tools::str::replace_chars(&check_this,"{"+placeholder+"}",energy());
		else if (placeholder=="repetition") tools::str::replace_chars(&check_this,"{"+placeholder+"}",repetition());
		else if (placeholder=="other"||placeholder=="others"||placeholder=="rest") tools::str::replace_chars(&check_this,"{"+placeholder+"}",others());
	}
	while (check_this.find("__")!=string::npos)
		tools::str::replace_chars(&check_this,"__","_");
	
	while (check_this.back() == '_')
		check_this.erase(check_this.end()-1);
}

std::__cxx11::string export2_t::root_directory()
{
	string directory_p = directory_config;
	if (directory_p!="") 
	{
		check_placeholders(directory_p);
	}
	else directory_p = measurement->filename_p->directory();
	
	if (directory_p.back() == PATH_DELIMITER)
		directory_p.erase(directory_p.end()-1);
	while (directory_p.back() == '_')
		directory_p.erase(directory_p.end()-1);
		
	directory_p = tools::file::check_directory_string(directory_p);
	
	return directory_p;
}

std::__cxx11::string export2_t::filename(string file_ending)
{
	string filename_p = filename_config;
	if (filename_p != "") check_placeholders(filename_p);
	else
	{
		filename_p = olcdb();
		if (lot()!=LOT_DEFAULT) filename_p += "_" + lot();
		if (wafer()!=WAFER_DEFAULT) filename_p += "_" + wafer();
		if (chip()!=CHIP_DEFAULT) filename_p += "_" + chip();
		if (monitor()!=MONITOR_DEFAULT) filename_p += "_" + monitor();
		if (group()!=GROUP_DEFAULT) filename_p += "_" + group();
		if (group()!=GROUP_DEFAULT &&repetition()!=repetition_DEFAULT) filename_p += repetition();
	
		if (lot()==LOT_DEFAULT||wafer()==WAFER_DEFAULT||group()==GROUP_DEFAULT) filename_p = measurement->filename_p->filename_without_crater_depths();
		if (measurement->tool_name()!=TOOL_NAME_DEFAULT && measurement->tool_name()!="NULL") filename_p+="."+measurement->tool_name();
		filename_p+=file_ending;
	}	
	while (filename_p.find("__")!=string::npos)
		tools::str::replace_chars(&filename_p,"__","_");
	
	while (filename_p.back() == '_')
		filename_p.erase(filename_p.end()-1);
	
	return filename_p;
}


/*
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 */



/////////////////////////////////////////
////////////// ORIGIN ///////////////////
/////////////////////////////////////////

set<std::__cxx11::string> origin_t::root_directories(measurement_group_t& MG)
{
	set<string> root_directories;
	for (auto& measurement: MG.measurements)
	{
		origin_t origin (measurement);
		root_directories.insert(origin.root_directory());
	}
	return root_directories;
}

void origin_t::apply_origin_replacements_on_string(std::__cxx11::string& replace_this)
{
	smatch match;
	string placeholder;
	string replacement;
	while (regex_search(replace_this,match,regex("\\^([0-9])"))) 
	{
		placeholder = "^"+string(match[1]);
		replacement = "\\+("+string(match[1])+")";
		tools::str::replace_chars(&replace_this,placeholder,replacement);
	}
}

string origin_t::prefix()
{
	string prefix = column_t::prefix_conf;
	check_placeholders(prefix);
// 	if (prefix!="") prefix = prefix + "_";
	return prefix;
}

string origin_t::suffix()
{
	string suffix=column_t::suffix_conf;
	if (suffix!="")
	{
		check_placeholders(suffix);
	}
	else 
	{
		if (lot()!=LOT_DEFAULT) suffix += "_" + lot();
		if (wafer()!=WAFER_DEFAULT) suffix += "_" + wafer();
		if (chip()!=CHIP_DEFAULT) suffix += "_" + chip();
		if (monitor()!=MONITOR_DEFAULT) suffix += "_" + monitor();
		// special sample names (not IHP compliant) have no lot or wafer
		if (lot()==LOT_DEFAULT && wafer()==WAFER_DEFAULT) 
		{
			if (measurement->filename_p->not_parseable_filename_parts.size()==0) suffix = "_"+measurement->filename_p->filename_without_crater_depths();
			else suffix = "_"+others();
		}
	
		if (repetition()!=repetition_DEFAULT) suffix += "_" + repetition();
		suffix.erase(suffix.begin());
		if (suffix!="") suffix = "_"+suffix;
	}
	
	return suffix;
}

origin_t::column_t::column_t(vector<std::__cxx11::string> data_p, std::__cxx11::string longname_p, std::__cxx11::string unit_p, std::__cxx11::string comment_p)
{
	if (data_p.size()==0) return;
	longname=longname_p;
	unit = unit_p;
	comment = comment_p;
	data = data_p;
	populated = true;
	return;
}

origin_t::column_t::column_t(quantity_t quantity, string suffix, string prefix)
{
	if (!quantity.is_set()) return;

	longname = quantity.name;
	unit = quantity.unit;
	comment = prefix+suffix;
	data = tools::mat::double_vec_to_str_vec(quantity.data);
	populated=true;
	return;
}

origin_t::column_t::column_t(std::__cxx11::string longname, quantity_t quantity, std::__cxx11::string suffix, string prefix)
{
	column_t col(quantity,suffix,prefix);
	col.longname = longname;
	*this = col;
}


origin_t::column_t::column_t(quantity_t* quantity, string suffix, string prefix)
{
	if (quantity==nullptr) return;
	if (!quantity->is_set()) return;

	longname = quantity->name;
	unit = quantity->unit;
	comment = prefix+suffix;
	data = tools::mat::double_vec_to_str_vec(quantity->data);
	populated=true;
	return;
}

origin_t::column_t::column_t(cluster_t* cluster, quantity_t quantity, string suffix, string prefix)
{
	if (cluster==nullptr) return;
	if (!quantity.is_set()) return;
// 	cout << "cluster.name()B:\t" << cluster->name() << endl;
	*this = column_t(cluster,&quantity,suffix,prefix);
}

origin_t::column_t::column_t(cluster_t* cluster, quantity_t* quantity, string suffix, string prefix)
{
	if (quantity==nullptr) return;
	if (!quantity->is_set()) return;
	
	string name=cluster->name();
	
	vector<isotope_t> isotopes;
	isotopes = cluster->isotopes;
	if (quantity->name.find("concentration")!=string::npos && isotopes.size()>1)
	{
		// remove any matrix isotopes from the list of isotopes in this cluster
		set<isotope_t> s = cluster->measurement->measurement_group->reference_matrix_isotopes();
		vector<isotope_t> v(s.begin(),s.end());
		isotopes = cluster->leftover_elements(v);
	}
	
// 	cout << "isotopes.size()\t" << isotopes.size()  << endl;
// 	for (auto& isotope:isotopes)
// 		isotope.to_screen();
	
	if (isotopes.size()>0)
	{
		name="";
		for (int ii=0; ii<isotopes.size();ii++)
		{
			isotope_t isotope=isotopes.at(ii);
			/*if cluster is matrix isotope/element, check if nucleons in set -> not possible, just have clusters ...*/
			if (isotope.nucleons>0) name+="\\+("+to_string(isotope.nucleons)+")";
			name+=isotope.symbol;
			if (isotope.atoms>1) name+=to_string(isotope.atoms);
			if (ii!=isotopes.size()-1) name+=" ";
// 			cout << "isotope.name()="<<isotope.name()<< endl;
// 			cout << "name=" << name << endl;
		}
	}

	if (quantity->unit=="at%" && isotopes.size()==1) 
	{
// 		isotope_t element = cluster->measurement->measurement_group->isotope_from_cluster_name(cluster->name());
		isotope_t element = isotopes[0];
		element.nucleons=-1;
// 		element.to_screen();
		longname = element.symbol +" "+quantity->name;
		comment = prefix+element.symbol+suffix;
	}
	else 
	{
		longname = name +" "+quantity->name;
		comment = prefix+name+suffix;
	}
	unit = quantity->unit;
	
	data = tools::mat::double_vec_to_str_vec(quantity->data);
	populated=true;
	return;
}

vector<string> origin_t::column_t::vec()
{
	vector<string> vector;
	if (!populated) return vector;
	apply_origin_replacements_on_string(longname);
	apply_origin_replacements_on_string(unit);
	apply_origin_replacements_on_string(comment);
	vector={longname,unit,comment};
	tools::vec::add(&vector,data);
	return vector;
}

vector<vector<string>> origin_t::cols_to_matrix(vector<column_t>& columns)
{
	vector<vector<string>> mat;
	for (column_t column:columns)
	{
		if (column.populated) mat.push_back(column.vec());
	}
	if (mat.size()==0) return mat;
	mat = tools::mat::transpose_matrix(mat);
	return mat;
}

bool origin_t::write_to_file(vector<column_t> columns, string directory_s, string filename_p)
{
	vector<vector<string>> output_matrix = cols_to_matrix(columns);
	string output=tools::mat::format_matrix_to_string(&output_matrix,"\n",data_column_delimiter);
// 	apply_origin_replacements_on_string(output);
	check_replacements(output);
	if (directory_s=="") return false;
	directory_s = tools::file::check_directory_string(directory_s);
	if (filename_p=="") return false;
	tools::file::mkpath(directory_s,0750);
// 	cout << directory_s << "\t" << filename_p << endl;
	if (!tools::file::write_to_file(directory_s+filename_p,&output,false)) return false;
	return true;
}

vector<origin_t::column_t> origin_t::format_measurement_cluster_columns()
{
	if (export_column_names.size()>0) return format_measurement_cluster_columns_from_config();
	vector<column_t> columns;
	if (measurement->crater.sputter_depth().is_set()) columns.push_back(column_t(measurement->crater.sputter_depth(depth_resolution),prefix()+"crater"+suffix()));
	else columns.push_back(column_t(measurement->crater.sputter_time(),prefix()+"crater"+suffix()));
	for (auto& cluster : measurement->clusters)
	{
		if (cluster.second.concentration().is_set()) 
		{
			if (smoothing_moving_window_mean_size>1) columns.push_back(column_t(&cluster.second,cluster.second.concentration().moving_window_mean(smoothing_moving_window_mean_size),suffix()));
			else columns.push_back(column_t(&cluster.second,cluster.second.concentration(),suffix(),prefix()));
		}
		else 
		{
			if (smoothing_moving_window_mean_size>1) columns.push_back(column_t(&cluster.second,cluster.second.intensity().moving_window_mean(smoothing_moving_window_mean_size),suffix()));
			else columns.push_back(column_t(&cluster.second,cluster.second.intensity(),suffix(),prefix()));
		}
	}
// 	columns.push_back(column_t(measurement->crater.sputter_current(),"crater_"+suffix()));
// 	columns.push_back(column_t(measurement->crater.total_sputter_time(),"crater_"+suffix()));
// 	columns.push_back(column_t(measurement->crater.total_sputter_depths(),"crater_"+suffix()));
	quantity_t depths = measurement->crater.total_sputter_depths_from_linescans();
	if (measurement->crater.total_sputter_depths_from_filename().is_set()) depths = depths.add_to_data(measurement->crater.total_sputter_depths_from_filename());
	columns.push_back(column_t(depths,prefix()+"crater" +suffix()));
	for (auto& linescan : measurement->crater.linescans)
	{
		columns.push_back(column_t(&linescan.xy,prefix()+"crater"+suffix()));
		columns.push_back(column_t(&linescan.z,prefix()+"crater"+suffix()));	
		columns.push_back(column_t(linescan.fit(),prefix()+"crater"+suffix()));
	}
	return columns;
}

origin_t::origin_t(measurement_t* measurement) : export2_t(measurement)
{
// 	write_to_file(format_measurement_cluster_columns(),root_directory(),filename());
	return;
}

vector<origin_t::column_t> origin_t::format_measurement_cluster_columns_from_config()
{
	vector<column_t> columns;
	for (auto& col_name : export_column_names)
	{
		if (col_name=="sputter_depth")
		{
			if (measurement->crater.sputter_depth().is_set()) columns.push_back(column_t(measurement->crater.sputter_depth(depth_resolution),prefix()+"crater"+suffix()));
			else if (col_name!="sputter_time" && measurement->crater.sputter_time().is_set()) columns.push_back(column_t(measurement->crater.sputter_time(),prefix()+"crater"+suffix()));
		}
		if (col_name=="sputter_time" &&measurement->crater.sputter_time().is_set()) columns.push_back(column_t(measurement->crater.sputter_time(),prefix()+"crater"+suffix()));
		if (col_name=="sputter_rate" &&measurement->crater.sputter_rate().is_set()) columns.push_back(column_t(measurement->crater.sputter_rate(),prefix()+"crater"+suffix()));
		if (col_name=="global_sputter_time" &&measurement->crater.global_sputter_time().is_set()) columns.push_back(column_t(measurement->crater.global_sputter_time(),prefix()+"crater"+suffix()));
		if ((col_name=="Ipr" || col_name=="ipr") &&measurement->crater.sputter_current().is_set()) 
		{
			if (smoothing_moving_window_mean_size>1) columns.push_back(column_t(measurement->crater.sputter_current().moving_window_mean(smoothing_moving_window_mean_size),prefix()+"crater"+suffix())); 
			else columns.push_back(column_t(measurement->crater.sputter_current(),prefix()+"crater"+suffix()));
		}
	
		if (col_name.find("concentration")!=string::npos && col_name.find("intensit")!=string::npos)
			for (auto& cluster : measurement->clusters)
			{
				if (cluster.second.concentration().is_set()) 
				{
					if (smoothing_moving_window_mean_size>1) columns.push_back(column_t(&cluster.second,cluster.second.concentration().moving_window_mean(smoothing_moving_window_mean_size),suffix(),prefix()));
					else columns.push_back(column_t(&cluster.second,cluster.second.concentration(),suffix(),prefix()));
				}
				else 
				{
					if (smoothing_moving_window_mean_size>1) columns.push_back(column_t(&cluster.second,cluster.second.intensity().moving_window_mean(smoothing_moving_window_mean_size),suffix(),prefix()));
					else columns.push_back(column_t(&cluster.second,cluster.second.intensity(),suffix(),prefix()));
				}
			}
		
		if (col_name=="concentrations" || col_name=="concentration")
		{
			for (auto& cluster : measurement->clusters)
			{
				if (cluster.second.concentration().is_set()) 
				{
					if (smoothing_moving_window_mean_size>1) columns.push_back(column_t(&cluster.second,cluster.second.concentration().moving_window_mean(smoothing_moving_window_mean_size),suffix(),prefix()));
					else columns.push_back(column_t(&cluster.second,cluster.second.concentration(),suffix(),prefix()));
				} 
			}
		}		
		if (col_name=="intensity" || col_name=="intensities")
		{
			for (auto& cluster : measurement->clusters)
				if (cluster.second.intensity().is_set()) 
				{
					if (smoothing_moving_window_mean_size>1)  columns.push_back(column_t(&cluster.second,cluster.second.intensity().moving_window_mean(smoothing_moving_window_mean_size),suffix(),prefix()));
					else columns.push_back(column_t(&cluster.second,cluster.second.intensity(),suffix(),prefix()));
				} 
		}
		
		if (col_name=="total_sputter_time")
		{
			if (measurement->crater.total_sputter_time().is_set()) columns.push_back(column_t(measurement->crater.total_sputter_time(),prefix()+"crater"+suffix()));
		}
		
		if (col_name=="total_sputter_depths" || col_name=="total_sputter_depth")
		{
			quantity_t depths = measurement->crater.total_sputter_depths_from_linescans();
			if (measurement->crater.total_sputter_depths_from_filename().is_set()) depths = depths.add_to_data(measurement->crater.total_sputter_depths_from_filename());
			columns.push_back(column_t(depths,prefix()+"crater"+suffix()));
		}
		
		if (col_name=="linescans" || col_name=="linescan" || col_name=="lineprofile" || col_name=="lineprofiles")
		{
			for (auto& linescan : measurement->crater.linescans)
			{
				columns.push_back(column_t(&linescan.xy,prefix()+"crater"+suffix()));
				columns.push_back(column_t(&linescan.z,prefix()+"crater"+suffix()));
				columns.push_back(column_t(linescan.fit(),prefix()+"crater"+suffix()));
			}
		}
		
		if (col_name=="RSF")
			for (auto& cluster : measurement->clusters)
				if (cluster.second.RSF().is_set()) columns.push_back(column_t(&cluster.second,cluster.second.RSF(),suffix(),prefix()));
				
		if (col_name=="reference_intensity")
			for (auto& cluster : measurement->clusters)
				if (cluster.second.reference_intensity().is_set()) columns.push_back(column_t(&cluster.second,cluster.second.reference_intensity(),suffix(),prefix()));
		
		if (col_name=="dose")
			for (auto& cluster : measurement->clusters)
				if (cluster.second.dose().is_set()) columns.push_back(column_t(&cluster.second,cluster.second.equilibrium().dose(),suffix(),prefix()));
	}
	
	return columns;
}

vector<origin_t::column_t> origin_t::format_settings_mass_calibration(measurement_group_t& MG_p)
{
	vector<column_t> columns;
	vector<string> measurement_start_date_times(MG_p.measurements.size());
	vector<string> measurement_names(MG_p.measurements.size());
	map<string,vector<string>> clustername_to_masses;
	map<string,vector<string>> clustername_to_fields;
	int i=0;
	for (auto& M:MG_p.measurements)
	{
		measurement_start_date_times[i]=M->date_time_start;
		measurement_names[i]=M->filename_p->filename_with_path();
		
		for (auto& S:M->settings.clustername_to_mass_calib)
		{
			if (clustername_to_masses.find(S.first)!=clustername_to_masses.end()) continue;
			for (auto& M2:MG_p.measurements)
			{
				if (M2->settings.clustername_to_mass_calib.find(S.first)==M2->settings.clustername_to_mass_calib.end())
				{
					clustername_to_masses[S.first].push_back("");
					clustername_to_fields[S.first].push_back("");
				}
				else
				{
					clustername_to_masses[S.first].push_back(S.second[0]);
					clustername_to_fields[S.first].push_back(S.second[1]);
				}
			}
		}
		i++;
	}
	/*nothing interesting to export*/
	if (clustername_to_masses.size()==0) return columns;
	
	columns.push_back(column_t(measurement_names,"filenames","",""));
	columns.push_back(column_t(measurement_start_date_times,"date_times","",""));
	for (auto& clustername:clustername_to_masses)
	{
		columns.push_back(column_t(clustername.second,clustername.first+" masses","","amu"));
		columns.push_back(column_t(clustername_to_fields[clustername.first],clustername.first+" fields","","bits?"));
	}
	return columns;
}



void origin_t::export_settings_mass_calibration_to_file(measurement_group_t& MG_p)
{
	vector<column_t> columns = format_settings_mass_calibration(MG_p);
	if (columns.size()==0) return;

	for (auto& dir:root_directories(MG_p))
		write_to_file(columns,dir,MG_p.name()+"_settings_mass_calibration.txt");
}


void origin_t::export_to_files(measurement_group_t& MG_p)
{
	
	for (auto& measurement: MG_p.measurements)
	{
		cout << "\ttrying to export: " << measurement->filename_p->filename_without_crater_depths() << " ...";
		origin_t origin (measurement);
		if (origin.write_to_file(origin.format_measurement_cluster_columns(),origin.root_directory(),origin.filename()))
			cout << "SUCCESS!\t" << origin.root_directory() << origin.filename() << endl;
		else
			cout << "failed" << endl;
	}
	
	if (export_MG_parameters)
	{
		cout << "\ttrying to export: MG parameters ...";
		origin_t::export_MG_parameters_to_file_V2(MG_p);
		cout << "SUCCESS!" << endl;
	}
	
	if (export_calc_results)
	{
		cout << "\ttrying to export: calculation_results  ...";
		export_contents_to_file(MG_p.to_str(),MG_p.name()+"_calculation_results.txt",MG_p,calc_location);
		cout << "SUCCESS!" << endl;
	}
	
	if (export_calc_history)
	{
		cout << "\ttrying to export: detailed_calculation_history ...";
		export_contents_to_file(calc_history,MG_p.name()+"_detailed_calculation_history.txt",MG_p,calc_location);
		cout << "SUCCESS!" << endl;
	}
}

vector<std::__cxx11::string> origin_t::quantity_data_to_string(quantity_t& q)
{
	if (q.data.size()==0) return {};
	
	vector<string> q_string(q.data.size());
	for (int i=0;i<q_string.size();i++)
	{
		if (q.data[i]<0) q_string[i]="";
		else q_string[i] = to_string(q.data[i]);
	}
	return q_string;
}


/*
 * this method is more robust and allows measurements in same group with different clusters
 */
void origin_t::export_MG_parameters_to_file_V2(measurement_group_t& MG_p)
{
	if (MG_p.filenames_with_path().size()==0) return;
	
	vector<column_t> cols;
	quantity_t SRs = MG_p.SRs_from_all_measurements();
	
	for (auto& clustername:MG_p.cluster_names())
	{
		quantity_t Cs = MG_p.clustername_to_concentrations_from_all_measurements()[clustername];
		cols.push_back(column_t(quantity_data_to_string(Cs),clustername+" concentration",Cs.unit,""));
		quantity_t Is = MG_p.clustername_to_intensities_from_all_measurements()[clustername];
		cols.push_back(column_t(quantity_data_to_string(Is),clustername+" intensity",Is.unit,""));
		quantity_t RSFs = MG_p.clustername_to_RSFs_from_all_measurements()[clustername];
		cols.push_back(column_t(quantity_data_to_string(RSFs),clustername + " RSF",RSFs.unit,""));
	}
	
	cols.push_back(column_t(quantity_data_to_string(SRs),"sputter_rates",SRs.unit,""));
	cols.push_back(column_t(MG_p.filenames_with_path(),"filenames","",""));
		
	for (auto& dir:root_directories(MG_p))
		write_to_file(cols,dir+calc_location,MG_p.name()+"_parameters.txt");
}

void origin_t::export_MG_parameters_to_file(measurement_group_t& MG_p)
{	
	vector<string> filenames;
	
	map<string,quantity_t> clustername_to_RSF;
	map<string,quantity_t> clustername_to_C;
	map<string,quantity_t> clustername_to_I;
	quantity_t SRs;
	/*SRs*/
	for (auto& M:MG_p.measurements)
	{
		if (!SRs.is_set() && M->crater.sputter_rate().is_set())
		{
			SRs.dimension=M->crater.sputter_rate().dimension;
			SRs.unit=M->crater.sputter_rate().unit;
			SRs.name=M->crater.sputter_rate().name;
			SRs.data.push_back(M->crater.sputter_rate().data[0]);
		}
		else if (M->crater.sputter_rate().is_set())
		{
			SRs.data.push_back(M->crater.sputter_rate().data[0]);
		}
		else SRs.data.push_back(-1); // empty
		filenames.push_back(M->filename_p->filename_with_path());
	}
	
	/*concentrations*/
	for (auto& M:MG_p.measurements)
	{
		for (auto& cluster:M->clusters)
		{
			if (clustername_to_C.find(cluster.second.name())!=clustername_to_C.end()) continue;
			if (cluster.second.concentration().is_set())
			{
				clustername_to_C[cluster.second.name()] = cluster.second.concentration();
				clustername_to_C[cluster.second.name()].data.clear();
				for (auto& M2:MG_p.measurements)
				{
					for (auto& C2:M2->clusters)
					{
						if (cluster.second.name() == C2.second.name())
						{
							if (C2.second.concentration().is_set())
								clustername_to_C[cluster.second.name()].data.push_back(C2.second.concentration().trimmed_mean().data[0]);
							else
								clustername_to_C[cluster.second.name()].data.push_back(-1);
						}		
					}
				}
			}
		}
	}
	
	/*intensities*/
	for (auto& M:MG_p.measurements)
	{
		for (auto& cluster:M->clusters)
		{
			if (clustername_to_I.find(cluster.second.name())!=clustername_to_I.end()) continue;
			if (cluster.second.intensity().is_set())
			{
				clustername_to_I[cluster.second.name()] = cluster.second.intensity();
				clustername_to_I[cluster.second.name()].data.clear();
				for (auto& M2:MG_p.measurements)
				{
					for (auto& C2:M2->clusters)
					{
						if (cluster.second.name() == C2.second.name())
						{
							if (C2.second.intensity().is_set())
								clustername_to_I[cluster.second.name()].data.push_back(C2.second.intensity().trimmed_mean().data[0]);
							else
								clustername_to_I[cluster.second.name()].data.push_back(-1);
						}		
					}
				}
			}
		}
	}
	
	/*RSFs*/
	for (auto& M:MG_p.measurements)
	{
		/*clusters*/
		for (auto& cluster:M->clusters)
		{
			if (clustername_to_RSF.find(cluster.second.name())!=clustername_to_RSF.end()) continue;
			if (cluster.second.RSF().is_set())
			{
				clustername_to_RSF[cluster.second.name()] = cluster.second.RSF();
				clustername_to_RSF[cluster.second.name()].data.clear();
				for (auto& M2:MG_p.measurements)
				{
					for (auto& C2:M2->clusters)
					{
						if (cluster.second.name() == C2.second.name())
						{
							if (C2.second.RSF().is_set())
								clustername_to_RSF[cluster.second.name()].data.push_back(C2.second.RSF().data[0]);
							else
								clustername_to_RSF[cluster.second.name()].data.push_back(-1);
						}		
					}
				}
			}
		}
	}
	
	if (!SRs.is_set() && clustername_to_RSF.size()==0 && clustername_to_C.size()==0 && clustername_to_I.size()==0) return;
	vector<column_t> cols;
	cols.push_back(column_t(filenames,"filenames","",""));
	
	vector<string> SRs_data(SRs.data.size());
	for (int i=0;i<SRs_data.size();i++)
	{
		if (SRs.data[i]<0) SRs_data[i]="";
		else SRs_data[i] = to_string(SRs.data[i]);
	}
	cols.push_back(column_t(SRs_data,SRs.name,SRs.unit,""));
	
	for (auto& Cs:clustername_to_C)
	{
		vector<string> RSF_data(Cs.second.data.size());
		for (int i=0;i<RSF_data.size();i++)
		{
			if (Cs.second.data[i]<0) RSF_data[i]="";
			else RSF_data[i] = to_string(Cs.second.data[i]);
		}
		cols.push_back(column_t(RSF_data,Cs.first+" tr_mean(concentration)",Cs.second.unit,""));
	}
	
	for (auto& Is:clustername_to_I)
	{
		vector<string> RSF_data(Is.second.data.size());
		for (int i=0;i<RSF_data.size();i++)
		{
			if (Is.second.data[i]<0) RSF_data[i]="";
			else RSF_data[i] = to_string(Is.second.data[i]);
		}
		cols.push_back(column_t(RSF_data,Is.first+" tr_mean(intensity)",Is.second.unit,""));
	}
	
	for (auto& RSFs:clustername_to_RSF)
	{
		vector<string> RSF_data(RSFs.second.data.size());
		for (int i=0;i<RSF_data.size();i++)
		{
			if (RSFs.second.data[i]<0) RSF_data[i]="";
			else RSF_data[i] = to_string(RSFs.second.data[i]);
		}
		cols.push_back(column_t(RSF_data,RSFs.first+" RSF",RSFs.second.unit,""));
	}
	
	
	for (auto& dir:root_directories(MG_p))
		write_to_file(cols,dir+calc_location,MG_p.name()+"_parameters.txt");
}


void origin_t::export_jiang_parameters_to_file(calc_models_t::jiang_t& jiang)
{
	if (jiang.is_error()) return;
	if (calc_location=="") return;

	/*now export jiang parameters*/
	vector<column_t> cols;
	quantity_t fit;
	string longname, longnameC;
	
		longnameC = "C["+jiang.counter_matrix_isotope().symbol +"] / C[" + jiang.denominator_matrix_isotope().symbol+"]";
		
		longname = "I("+jiang.counter_matrix_isotope().symbol +") / I(" + jiang.denominator_matrix_isotope().symbol+")";
		cols.push_back(column_t(longname,jiang.Crel_to_Irel().second));
		cols.push_back(column_t(longnameC,jiang.Crel_to_Irel().first));
		fit=jiang.Crel_to_Irel().first;
		longname = "fitted C["+jiang.counter_matrix_isotope().symbol +"] / C[" + jiang.denominator_matrix_isotope().symbol+"]";
		fit.data = jiang.CRel_to_Irel_polyfit.fitted_y_data(jiang.Crel_to_Irel().second.data);
		cols.push_back(column_t(longname,fit));
	
	
	
		cols.push_back(column_t(longnameC,jiang.SRs_to_Crel().second));
		longname = "sputter_rate";
		cols.push_back(column_t(jiang.SRs_to_Crel().first));
		fit=jiang.SRs_to_Crel().first;
		fit.name = "fitted " + fit.name;
		fit.data = jiang.SR_to_Crel_polyfit.fitted_y_data(jiang.SRs_to_Crel().second.data);
		cols.push_back(column_t(fit));
	
	
	for (auto& RSFs:jiang.clustername_to_RSFs_to_Crel())
	{
		cols.push_back(column_t(longnameC, RSFs.second.second));
		cols.push_back(column_t("RSF "+RSFs.first, RSFs.second.first));
		fit=RSFs.second.first;
		fit.data = jiang.clustername_to_RSFs_to_Crel_polyfit[RSFs.first].fitted_y_data(RSFs.second.second.data);
		cols.push_back(column_t("fitted RSF "+RSFs.first,fit));
	}
	measurement_group_t MG_p = jiang.measurement_group();
	for (auto& dir:root_directories(MG_p))
		write_to_file(cols,dir+calc_location,"jiang.txt");
	return;
}

