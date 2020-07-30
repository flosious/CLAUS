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
// 	if (contents.size()==0) return;
// 	if (filename_p.length()==0) return;
// 	set<string> root_dirs;
// 	for (auto& measurement:MG.measurements)
// 	{
// 		export2_t exp(measurement);
// 		root_dirs.insert(exp.root_directory());
// 	}
// 	
// 	
// 	string output = tools::vec::combine_vec_to_string(contents,"\n");
// 	string dir="";
// 	for (auto& root_dir:root_dirs)
// 	{
// 		dir = tools::file::check_directory_string(root_dir)+sub_directory;
// 		dir = tools::file::check_directory_string(dir);
// 		tools::file::write_to_file(dir+filename_p,&output,false);
// 	}
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

void export2_t::check_replacements(std::__cxx11::string& check_this)
{
	for (auto& replacement:conf.replacements)
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
	}
}

std::__cxx11::string export2_t::root_directory(string directory)
{
// 	string directory;
	if (directory!="") 
	{
		check_placeholders(directory);
	}
	else directory = measurement->filename_p->directory();
	directory = tools::file::check_directory_string(directory);
// 	tools::file::mkpath(directory,0750);
	return directory;
}

std::__cxx11::string export2_t::filename(string filename, string file_ending)
{
// 	if (filename != "" && lot()!=LOT_DEFAULT && wafer()!=WAFER_DEFAULT) check_placeholders(filename);
	if (filename != "") check_placeholders(filename);
	else
	{
		filename = olcdb();
		if (lot()!=LOT_DEFAULT) filename += "_" + lot();
		if (wafer()!=WAFER_DEFAULT) filename += "_" + wafer();
		if (chip()!=CHIP_DEFAULT) filename += "_" + chip();
		if (monitor()!=MONITOR_DEFAULT) filename += "_" + monitor();
		if (group()!=GROUP_DEFAULT) filename += "_" + group();
		if (group()!=GROUP_DEFAULT &&repetition()!=repetition_DEFAULT) filename += repetition();
	
		if (lot()==LOT_DEFAULT||wafer()==WAFER_DEFAULT||group()==GROUP_DEFAULT) filename = measurement->filename_p->filename_without_crater_depths();
		if (measurement->tool_name()!=TOOL_NAME_DEFAULT && measurement->tool_name()!="NULL") filename+="."+measurement->tool_name();
		filename+=file_ending;
	}
	for (int i=0;i<4;i++)
		tools::str::replace_chars(&filename,"__","_");
	return filename;
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





// /*EXPORT_T*/
// export_t::export_t(measurement_t *measurement_p)
// {
// 	measurement = measurement_p;
// // 	measurement->clusters["31P"].concentration().to_screen();
// 	format_id();
// 	get_directory();
// 	get_filename();
// }
// 
// // export_t::export_t(measurement_group_t *MG)
// // {
// // 	if (MG->measurements.size()==0) return;
// // 	measurement = MG->measurements.front();
// // // 	measurement->clusters["31P"].concentration().to_screen();
// // 	format_id();
// // 	get_directory();
// // 	get_filename();
// // }
// 
// 
// 
// bool export_t::format_id()
// {
// 	if (measurement->filename_p->olcdb>-1) olcdb = to_string(measurement->filename_p->olcdb);
// 	if (measurement->filename_p->wafer>-1) wafer = "w0"+to_string(measurement->filename_p->wafer);
// 	if (measurement->filename_p->wafer>9) wafer = "w"+to_string(measurement->filename_p->wafer);
// 	if (measurement->filename_p->group>-1) group = "g"+to_string(measurement->filename_p->group)+measurement->filename_p->repetition;
// 	if (measurement->filename_p->lot!="") lot = measurement->filename_p->lot;
// 	if (measurement->filename_p->lot_split!="") lot_split = measurement->filename_p->lot_split;
// 	if (measurement->filename_p->chip_x>-1) 
// 	{
// 		if (measurement->filename_p->chip_x>9) chip = "X"+to_string(measurement->filename_p->chip_x);
// 		else chip = "X0"+to_string(measurement->filename_p->chip_x);
// 		if (measurement->filename_p->chip_y>9) chip += "Y"+to_string(measurement->filename_p->chip_y);
// 		else chip += "Y0"+to_string(measurement->filename_p->chip_y);
// 	}
// 	if (measurement->filename_p->monitor!="") monitor = "m"+measurement->filename_p->monitor;
// 	if (measurement->tool_name()!="") tool = measurement->tool_name();
// 	return true;
// }
// 
// bool export_t::check_replacements(string& check_this)
// {
// 	vector <string> placeholders = tools::str::get_all_string_between_string_A_and_next_B(&check_this,"{","}");
// 	string olcdbid="olcdbid-unknown";
// 	string lotid="lot-unknown";
// 	string lot_splitid="";
// 	string waferid="wafer-unknown";
// 	string groupid="group-unknown";
// 	string chipid="";
// 	string monitorid="";
// 	string toolid="tool-unknown";
// 	string energy="";
// 	if (olcdb!="") olcdbid=olcdb;
// 	if (lot!="") lotid=lot;
// 	if (lot_split!="") lot_splitid=lot_split;
// 	if (wafer!="") waferid=wafer;
// 	if (group!="") groupid=group;
// 	if (chip!="") chipid=chip;
// 	if (monitor!="") monitorid=monitor;
// 	if (tool!="") toolid=tool;
// 	if (measurement->settings.sputter_energy().is_set()) energy=to_string((int)(measurement->settings.sputter_energy().data[0]))+measurement->settings.sputter_energy().unit+measurement->settings.sputter_element();
// 	
// 	for (string placeholder:placeholders)
// 	{
// 		string replacethis = "{"+placeholder+"}";
// 		if (placeholder=="olcdbid" || placeholder=="olcdb") tools::str::replace_chars(&check_this,"{"+placeholder+"}",olcdbid);
// 		else if (placeholder=="waferid" || placeholder=="wafer") tools::str::replace_chars(&check_this,"{"+placeholder+"}",waferid);
// 		else if (placeholder=="groupid" || placeholder=="group") tools::str::replace_chars(&check_this,"{"+placeholder+"}",groupid);
// 		else if (placeholder=="lotid" || placeholder=="lot") tools::str::replace_chars(&check_this,"{"+placeholder+"}",lotid);
// 		else if (placeholder=="lotsplit" || placeholder=="lot_split") tools::str::replace_chars(&check_this,"{"+placeholder+"}",lot_splitid);
// 		else if (placeholder=="chipid" || placeholder=="chip") tools::str::replace_chars(&check_this,"{"+placeholder+"}",chipid);
// 		else if (placeholder=="monitorid" || placeholder=="monitor") tools::str::replace_chars(&check_this,"{"+placeholder+"}",monitorid);
// 		else if (placeholder=="toolid" || placeholder=="tool" || placeholder=="tool_name") tools::str::replace_chars(&check_this,"{"+placeholder+"}",toolid);
// 		else if (placeholder=="energy") tools::str::replace_chars(&check_this,"{"+placeholder+"}",energy);
// 	}
// 	return true;
// }
// 
// bool export_t::get_directory()
// {
// 	if (directory!="") return true;
// 	directory = get_directory(measurement);
// 	if (!check_replacements(directory)) return false;
// 	if (directory=="") return false;
// 	tools::file::mkpath(directory,0750);
// 	return true;
// }
// 
// string export_t::get_directory(measurement_t *measurement)
// {
// 	string directory;
// 	if (conf.export_location!="")
// 	{
// 		directory=conf.export_location;
// // 		directory = tools::file::check_directory_string(directory);
// 	}
// 	else directory = measurement->filename_p->directory();
// 	directory = tools::file::check_directory_string(directory);
// 	return directory;
// }
// 
// bool export_t::get_filename(string file_ending)
// {
// 	filename = conf.export_filename;
// 	if (filename != "" && lot!="" && wafer!="") check_replacements(filename);
// 	else
// 	{
// 		filename = olcdb;
// 		if (lot!="") filename += "_" + lot;
// 		if (wafer!="") filename += "_" + wafer;
// 		if (chip!="") filename += "_" + chip;
// 		if (monitor!="") filename += "_" + monitor;
// 		if (group!="") filename += "_" + group;
// 	
// 		if (lot==""||wafer==""||group=="") filename = measurement->filename_p->filename_without_crater_depths();
// 		if (measurement->tool_name()!="" && measurement->tool_name()!="NULL") filename+="."+measurement->tool_name();
// 		filename+=file_ending;
// 	}
// 	tools::str::replace_chars(&filename,"__","_");
// 	tools::str::replace_chars(&filename,"__","_");
// 	tools::str::replace_chars(&filename,"__","_");
// 	return true;
// }
// 
// 
// bool export_t::write_to_file()
// {
// // 	if (!get_directory()) return false;
// // 	if (!get_filename()) return false;
// 	if (directory=="") return false;
// 	if (filename=="") return false;
// 	tools::file::mkpath(directory,0750);
// 	if (!tools::file::write_to_file(directory+filename,&output,false)) return false;
// 	return true;
// }
// 
// bool export_t::samples_overview(list<measurement_t> *measurements)
// {
// 	if (measurements->size()==0) return false;
// 	vector<vector<string>> output_matrix;
// 	vector<string> header_line={"measurement name"};
// 	for (measurement_t measurement:*measurements)
// 		for (map<string,cluster_t>::iterator it=measurement.clusters.begin();it!=measurement.clusters.end();++it)
// 			header_line.push_back(it->second.name());
// 	tools::str::remove_duplicates(&header_line);
// 	for (measurement_t measurement:*measurements)
// 	{
// 			vector<string> line;
// 			
// 			line.push_back(measurement.filename_p->filename_without_crater_depths());
// 			for (int i=1;i<header_line.size();i++)
// 			{
// 				cluster_t* cluster = &(measurement.clusters[header_line[i]]);
// 				if (cluster!=nullptr)
// 				{
// // 					quantity_t* quantity = &cluster->intensity();
// // 					TODO
// // 					if (quantity!=nullptr) line.push_back(statistics::get_median_from_Y());
// 				}
// 			}
// 	}
// 	return true;
// }
// 
// bool export_t::Ipr_global(list<measurement_t> *measurements)
// {
// 	vector<string> global_time={"Ipr","global_sputter_time","s"};
// 	vector<string> global_Ipr={"Ipr","sputter_current","nA"};
// 	for (auto measurement : *measurements)
// 	{
// 		if (measurement.crater.sputter_current().data.size()==0) continue;
// 
// // 		tools::vec::add(&global_Ipr,tools::mat::double_vec_to_str_vec(measurement->cluster_by_name("Ipr")->quantity_by_name("intensity")->data));
// 		tools::vec::add(&global_time,tools::mat::double_vec_to_str_vec(measurement.crater.global_sputter_time().data));
// 		tools::vec::add(&global_Ipr,tools::mat::double_vec_to_str_vec(measurement.crater.sputter_current().data));
// 		
// 	}
// 		conf.export_location = tools::file::check_directory_string(conf.export_location);
// 		if (conf.export_location=="") return false;
// 		tools::file::mkpath(conf.export_location,0750);
// 		vector<vector<string>> out_mat = tools::mat::transpose_matrix({global_time,global_Ipr});
// 		string output = tools::mat::format_matrix_to_string(&out_mat);
// 		if (!tools::file::write_to_file(conf.export_location+"Ipr_global.txt",&output,false)) return false;
// 
// 	return true;
// }




/////////////////////////////////////////
////////////// ORIGIN ///////////////////
/////////////////////////////////////////

set<std::__cxx11::string> origin_t::root_directories(measurement_group_t& MG)
{
	set<string> root_directories;
	for (auto& measurement: MG.measurements)
	{
		origin_t origin (measurement);
		root_directories.insert(origin.root_directory(conf.export_location));
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


string origin_t::suffix()
{
	string suffix="";
	if (lot()!=LOT_DEFAULT) suffix += "_" + lot();
	if (wafer()!=WAFER_DEFAULT) suffix += "_" + wafer();
	if (chip()!=CHIP_DEFAULT) suffix += "_" + chip();
	if (monitor()!=MONITOR_DEFAULT) suffix += "_" + monitor();
	// special sample names (not IHP compliant) have no lot or wafer
	if (lot()==LOT_DEFAULT && wafer()==WAFER_DEFAULT) 
	{
		if (measurement->filename_p->not_parseable_filename_parts.size()==0) suffix = "_"+measurement->filename_p->filename_without_crater_depths();
		else suffix = "_"+tools::vec::combine_vec_to_string(measurement->filename_p->not_parseable_filename_parts,"_");
	}
// 	else if (measurement->filename_p->not_parseable_filename_parts.size()!=0 && (lot()==LOT_DEFAULT || wafer()==WAFER_DEFAULT)) 
// 		suffix += "_"+tools::vec::combine_vec_to_string(measurement->filename_p->not_parseable_filename_parts,"_");
	if (repetition()!=repetition_DEFAULT) suffix += "_" + repetition();
	
// 	if ((lot()==LOT_DEFAULT && wafer()==WAFER_DEFAULT)) 
// 	{
// 		if (measurement->filename_p->not_parseable_filename_parts.size()==0) suffix = "_"+measurement->filename_p->filename_without_crater_depths();
// 		else suffix = "_"+tools::vec::combine_vec_to_string(measurement->filename_p->not_parseable_filename_parts,"_");
// 	}
	suffix.erase(suffix.begin());
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

origin_t::column_t::column_t(quantity_t quantity, string suffix)
{
	if (!quantity.is_set()) return;

	longname = quantity.name;
	unit = quantity.unit;
	comment = suffix;
	data = tools::mat::double_vec_to_str_vec(quantity.data);
	populated=true;
	return;
}

origin_t::column_t::column_t(std::__cxx11::string longname, quantity_t quantity, std::__cxx11::string suffix)
{
	column_t col(quantity,suffix);
	col.longname = longname;
	*this = col;
}


origin_t::column_t::column_t(quantity_t* quantity, string suffix)
{
	if (quantity==nullptr) return;
	if (!quantity->is_set()) return;

	longname = quantity->name;
	unit = quantity->unit;
	comment = suffix;
	data = tools::mat::double_vec_to_str_vec(quantity->data);
	populated=true;
	return;
}

origin_t::column_t::column_t(cluster_t* cluster, quantity_t quantity, string suffix)
{
	if (cluster==nullptr) return;
	if (!quantity.is_set()) return;
// 	cout << "cluster.name()B:\t" << cluster->name() << endl;
	*this = column_t(cluster,&quantity,suffix);
}

origin_t::column_t::column_t(cluster_t* cluster, quantity_t* quantity, string suffix)
{
	if (quantity==nullptr) return;
	if (!quantity->is_set()) return;
// 	cout << "cluster.name()A:\t" << cluster->name() << endl;
	
	string name=cluster->name();
	vector<isotope_t>* isotopes = &cluster->isotopes;
	if (isotopes->size()>0)
	{
		name="";
		for (int ii=0; ii<isotopes->size();ii++)
		{
			isotope_t isotope=isotopes->at(ii);
			/*if cluster is matrix isotope/element, check if nucleons in set -> not possible, just have clusters ...*/
			if (isotope.nucleons>0) name+="\\+("+to_string(isotope.nucleons)+")";
			name+=isotope.symbol;
			if (isotope.atoms>1) name+=to_string(isotope.atoms);
// 			name+= isotope.name();
			if (ii!=isotopes->size()-1) name+=" ";
		}
	}
	if (quantity->unit=="at%") 
	{
		isotope_t element = cluster->measurement->measurement_group->isotope_from_cluster_name(cluster->name());
		longname = element.symbol +" "+quantity->name;
		comment = element.symbol+suffix;
	}
	else 
	{
		longname = name +" "+quantity->name;
		comment = name+suffix;
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

bool origin_t::write_to_file(vector<column_t> columns, string directory_p, string filename_p)
{
	vector<vector<string>> output_matrix = cols_to_matrix(columns);
	string output=tools::mat::format_matrix_to_string(&output_matrix,"\n",conf.data_column_delimiter);
// 	apply_origin_replacements_on_string(output);
	check_replacements(output);
	if (directory_p=="") return false;
	directory_p = tools::file::check_directory_string(directory_p);
	if (filename_p=="") return false;
	tools::file::mkpath(directory_p,0750);
// 	cout << directory_p << "\t" << filename_p << endl;
	if (!tools::file::write_to_file(directory_p+filename_p,&output,false)) return false;
	return true;
}

vector<origin_t::column_t> origin_t::format_measurement_cluster_columns()
{
	if (conf.export_column_names.size()>0) return format_measurement_cluster_columns_from_config();
	vector<column_t> columns;
	if (measurement->crater.sputter_depth().is_set()) columns.push_back(column_t(measurement->crater.sputter_depth(conf.depth_resolution),"crater_"+suffix()));
	else columns.push_back(column_t(measurement->crater.sputter_time(),"crater_"+suffix()));
// 	columns.push_back(column_t(measurement->crater.global_sputter_time(),"crater_"+suffix()));
	for (auto& cluster : measurement->clusters)
	{
		if (cluster.second.concentration().is_set()) 
			columns.push_back(column_t(&cluster.second,cluster.second.concentration(),"_"+suffix()));
		else 
			columns.push_back(column_t(&cluster.second,cluster.second.intensity(),"_"+suffix()));
	}
// 	columns.push_back(column_t(measurement->crater.sputter_current(),"crater_"+suffix()));
// 	columns.push_back(column_t(measurement->crater.total_sputter_time(),"crater_"+suffix()));
// 	columns.push_back(column_t(measurement->crater.total_sputter_depths(),"crater_"+suffix()));
	quantity_t depths = measurement->crater.total_sputter_depths_from_linescans();
	if (measurement->crater.total_sputter_depths_from_filename().is_set()) depths = depths.add_to_data(measurement->crater.total_sputter_depths_from_filename());
	columns.push_back(column_t(depths,"crater_"+suffix()));
	for (auto& linescan : measurement->crater.linescans)
	{
		columns.push_back(column_t(&linescan.xy,"crater_"+suffix()));
		columns.push_back(column_t(&linescan.z,"crater_"+suffix()));	
		columns.push_back(column_t(linescan.fit(),"crater_"+suffix()));
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
	for (auto& col_name : conf.export_column_names)
	{
		if (col_name=="sputter_depth")
		{
			if (measurement->crater.sputter_depth().is_set()) columns.push_back(column_t(measurement->crater.sputter_depth(conf.depth_resolution),"crater_"+suffix()));
			else if (col_name!="sputter_time" && measurement->crater.sputter_time().is_set()) columns.push_back(column_t(measurement->crater.sputter_time(),"crater_"+suffix()));
		}
		if (col_name=="sputter_time" &&measurement->crater.sputter_time().is_set()) columns.push_back(column_t(measurement->crater.sputter_time(),"crater_"+suffix()));
		if (col_name=="sputter_rate" &&measurement->crater.sputter_rate().is_set()) columns.push_back(column_t(measurement->crater.sputter_rate(),"crater_"+suffix()));
		if (col_name=="global_sputter_time" &&measurement->crater.global_sputter_time().is_set()) columns.push_back(column_t(measurement->crater.global_sputter_time(),"crater_"+suffix()));
		if ((col_name=="Ipr" || col_name=="ipr") &&measurement->crater.sputter_current().is_set()) columns.push_back(column_t(measurement->crater.sputter_current(),"crater_"+suffix()));
	
		if (col_name.find("concentration")!=string::npos && col_name.find("intensit")!=string::npos)
			for (auto& cluster : measurement->clusters)
			{
				if (cluster.second.concentration().is_set()) columns.push_back(column_t(&cluster.second,cluster.second.concentration(),"_"+suffix()));
				else columns.push_back(column_t(&cluster.second,cluster.second.intensity(),"_"+suffix()));
			}
		
		if (col_name=="concentrations" || col_name=="concentration")
			for (auto& cluster : measurement->clusters)
				if (cluster.second.concentration().is_set()) columns.push_back(column_t(&cluster.second,cluster.second.concentration(),"_"+suffix()));
				
		if (col_name=="intensity" || col_name=="intensities")
			for (auto& cluster : measurement->clusters)
				if (cluster.second.intensity().is_set()) 
				{
					columns.push_back(column_t(&cluster.second,cluster.second.intensity(),"_"+suffix()));
// 					filter_gaussian(5,4)
// 					columns.push_back(column_t(&cluster.second,cluster.second.intensity().filter_gaussian(5,4),"_"+suffix()));
				}
		
		if (col_name=="total_sputter_time")
		{
			if (measurement->crater.total_sputter_time().is_set()) columns.push_back(column_t(measurement->crater.total_sputter_time(),"crater_"+suffix()));
		}
		
		if (col_name=="total_sputter_depths" || col_name=="total_sputter_depth")
		{
			quantity_t depths = measurement->crater.total_sputter_depths_from_linescans();
			if (measurement->crater.total_sputter_depths_from_filename().is_set()) depths = depths.add_to_data(measurement->crater.total_sputter_depths_from_filename());
			columns.push_back(column_t(depths,"crater_"+suffix()));
		}
		
		if (col_name=="linescans" || col_name=="linescan")
		{
			for (auto& linescan : measurement->crater.linescans)
			{
				columns.push_back(column_t(&linescan.xy,"crater_"+suffix()));
				columns.push_back(column_t(&linescan.z,"crater_"+suffix()));	
				columns.push_back(column_t(linescan.fit(),"crater_"+suffix()));
			}
		}
		
		if (col_name=="RSF")
			for (auto& cluster : measurement->clusters)
				if (cluster.second.RSF().is_set()) columns.push_back(column_t(&cluster.second,cluster.second.RSF(),"_"+suffix()));
				
		if (col_name=="reference_intensity")
			for (auto& cluster : measurement->clusters)
				if (cluster.second.reference_intensity().is_set()) columns.push_back(column_t(&cluster.second,cluster.second.reference_intensity(),"_"+suffix()));
		
		if (col_name=="dose")
			for (auto& cluster : measurement->clusters)
// 				if (cluster.second.equilibrium(false).dose().is_set()) columns.push_back(column_t(&cluster.second,cluster.second.equilibrium(false).dose(),"_"+suffix()));
				if (cluster.second.dose().is_set()) columns.push_back(column_t(&cluster.second,cluster.second.dose(),"_"+suffix()));
		
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



void origin_t::export_settings_mass_calibration_to_file(measurement_group_t MG_p)
{
	vector<column_t> columns = format_settings_mass_calibration(MG_p);
	if (columns.size()==0) return;
// 	set<string> root_directories;
// 	for (auto& measurement: MG_p.measurements)
// 	{
// 		origin_t origin (measurement);
// 		root_directories.insert(origin.root_directory(conf.export_location));
// 	}
	for (auto& dir:root_directories(MG_p))
		write_to_file(columns,dir,MG_p.name()+"_settings_mass_calibration.txt");
}


void origin_t::export_to_files(measurement_group_t MG_p)
{
// 	set<string> root_directories;
	for (auto& measurement: MG_p.measurements)
	{
		cout << "\ttrying to export: " << measurement->filename_p->filename_without_crater_depths() << " ...";
		origin_t origin (measurement);
		if (origin.write_to_file(origin.format_measurement_cluster_columns(),origin.root_directory(conf.export_location),origin.filename(conf.export_filename)))
			cout << "SUCCESS!" << endl;
		else
			cout << "failed" << endl;
// 		root_directories.insert(origin.root_directory(conf.export_location));
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
		write_to_file(cols,dir+conf.calc_location,MG_p.name()+"_parameters.txt");
}

void origin_t::export_MG_parameters_to_file(measurement_group_t& MG_p)
{
// 	set<string> root_directories;
// 	for (auto& measurement: MG_p.measurements)
// 	{
// 		origin_t origin (measurement);
// 		root_directories.insert(origin.root_directory(conf.export_location));
// 	}
	
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
		write_to_file(cols,dir+conf.calc_location,MG_p.name()+"_parameters.txt");
}


void origin_t::export_jiang_parameters_to_file(calc_models_t::jiang_t& jiang)
{
	if (jiang.is_error()) return;
	if (conf.calc_location=="") return;
// 	set<string> root_directories;
// 	for (auto& measurement: jiang.measurement_group().measurements)
// 	{
// 		origin_t origin (measurement);
// // 		origin.write_to_file(origin.format_measurement_cluster_columns(),origin.root_directory(conf.export_location),origin.filename(conf.export_filename));
// 		root_directories.insert(origin.root_directory());
// 	}

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
		write_to_file(cols,dir+conf.calc_location,"jiang.txt");
	return;
}




// bool origin_t::write(vector<column_t>& cols, string directory,string filename)
// {
// 	vector<vector<string>> output_matrix;
// 	for (column_t column:cols)
// 	{
// 		if (column.populated) output_matrix.push_back(column.vec());
// 	}
// 	if (output_matrix.size()==0) return false;
// 
// 	output_matrix = tools::mat::transpose_matrix(output_matrix);
// 	string output=tools::mat::format_matrix_to_string(&output_matrix,"\n",conf.data_column_delimiter);
// 	if (directory=="") return false;
// 	if (filename=="") return false;
// 
// 	directory=tools::file::check_directory_string(directory);
// 	tools::file::mkpath(directory,0750);
// 
// 	if (!tools::file::write_to_file(directory+filename,&output,false)) return false;
// 
// 	return true;
// }

// void origin_t::export_to_files(list<measurement_t>* measurements)
// {
// 	for (auto& measurement:*measurements)
// 	{
// 		cout << "origin_t::origin_t(measurement_t* measurement) : export_t(measurement):\t" <<  measurement.filename_p->filename_with_path() << endl;
// 		origin_t origin(&measurement);
// 		if (!origin.error) origin.export_to_file();
// 	}
// 	return;
// }
// 
// void origin_t::export_to_files(list<measurement_t*> measurements)
// {
// 	for (auto& measurement:measurements)
// 	{
// 		cout << "origin_t::origin_t(measurement_t* measurement) : export_t(measurement):\t" <<  measurement->filename_p->filename_with_path() << endl;
// 		origin_t origin(measurement);
// 		if (!origin.error) origin.export_to_file();
// 	}
// 	return;
// }



// void origin_t::export_to_files(calc_models_t::jiang_t& jiang)
// {
// 	measurement_group_t MGj = jiang.calc();
// 	export_to_files(MGj.measurements);
// 	if (jiang.is_error()) return;
// // 	export_to_files(MGj.measurements);
// 	
// 	if (conf.calc_location=="") return;
// 	/*now export jiang parameters*/
// 	vector<column_t> cols;
// 	quantity_t fit;
// 	{
// 		cols.push_back(column_t(jiang.Crel_to_Irel().second));
// 		cols.push_back(column_t(jiang.Crel_to_Irel().first));
// 		fit=jiang.Crel_to_Irel().first;
// 		fit.data = jiang.CRel_to_Irel_polyfit.fitted_y_data(jiang.Crel_to_Irel().second.data);
// 		cols.push_back(column_t(fit));
// 	}
// 	
// 	{
// 		cols.push_back(column_t(jiang.SRs_to_Crel().second));
// 		cols.push_back(column_t(jiang.SRs_to_Crel().first));
// 		fit=jiang.SRs_to_Crel().first;
// 		fit.data = jiang.SR_to_Crel_polyfit.fitted_y_data(jiang.SRs_to_Crel().second.data);
// 		cols.push_back(column_t(fit));
// 	}
// 	
// 	for (auto& RSFs:jiang.clustername_to_RSFs_to_Crel())
// 	{
// 		cols.push_back(RSFs.second.second);
// 		cols.push_back(RSFs.second.first);
// 		fit=RSFs.second.first;
// 		fit.data = jiang.clustername_to_RSFs_to_Crel_polyfit[RSFs.first].fitted_y_data(RSFs.second.second.data);
// 		cols.push_back(column_t(fit));
// 	}
// 	
// 	write(cols,MGj.measurements.front()->filename_p->directory()+conf.calc_location,"calc_jiang.txt");
// 	return;
// }

// void origin_t::export_to_files(measurement_group_t MG)
// {
// 	export_to_files(MG.measurements);
// 
// 	return;
// }
// 
// void origin_t::export_to_files(list<measurement_group_t> MGs)
// {	
// 	for (auto& MG:MGs)
// 		export_to_files(MG);
// 	return;
// }

// void origin_t::export_overview_to_files(list<measurement_t>* measurements)
// {
// 	if (measurements==nullptr || measurements->size()==0) return;
// 	vector<vector<string>> cols;
// 	
// 	/// maps longname -> orogin_ts
// 	map<string, vector<origin_t::column_t>> lname_to_column;
// 	/// measurement_name -> longname ->origin_exports 
// // 	map<string, map<string, origin_t>> mmap;
// 	vector<string> measurement_names={"measurement_names"};
// 	for (list<measurement_t>::iterator it=measurements->begin();it!=measurements->end();++it)
// 	{
// 		measurement_t* measurement = &(*it);
// 		origin_t origin(measurement);
// 		for (origin_t::column_t col:origin.columns)
// 		{
// 			col.measurement_name=measurement->filename_p->filename_without_crater_depths();
// 			lname_to_column[col.longname].push_back(col);
// 		}
// 		measurement_names.push_back(measurement->filename_p->filename_without_crater_depths());
// 	}
// 	cols.push_back(measurement_names);
// 	for (map<string, vector<origin_t::column_t>>::iterator it=lname_to_column.begin();it!=lname_to_column.end();it++)
// 	{
// 		string lname = it->first;
// 		if (lname.find("median")==string::npos) continue;
// 		vector<string> col;
// 		col.push_back(lname);
// 		for (string measurement_name:measurement_names)
// 		{
// 			for (origin_t::column_t origin_col:it->second)
// 			{
// 				if (measurement_name==origin_col.measurement_name) 
// 				{
// 					col.push_back(origin_col.data[0]);
// 					break;
// 				}
// 			}
// 		}
// 		cols.push_back(col);
// 	}
// // 	print(cols);
// 	cols = tools::mat::transpose_matrix(cols);
// 	origin_t origin(&measurements->front());
// 	origin.get_filename("_OVERVIEW.txt");
// 	origin.output=tools::mat::format_matrix_to_string(&cols);
// 	origin.write_to_file();
// 	
// // 	output = 
// 	return;
// }


