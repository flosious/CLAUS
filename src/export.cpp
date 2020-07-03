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
#include "export.hpp"
#include "gnuplot_i.hpp" //Gnuplot class handles POSIX-Pipe-communikation with Gnuplot

#define LOT_DEFAULT "lot-unkown"
#define WAFER_DEFAULT "wafer-unkown"
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
void export2_t::export_contents_to_file(vector<std::__cxx11::string> contents, string filename_p, measurement_group_t MG, string sub_directory)
{
	if (contents.size()==0) return;
	if (filename_p.length()==0) return;
	set<string> root_dirs;
	for (auto& measurement:MG.measurements)
	{
		export2_t exp(measurement);
		root_dirs.insert(exp.root_directory());
	}
	
	
	string output = tools::vec::combine_vec_to_string(contents,"\n");
	string dir="";
	for (auto& root_dir:root_dirs)
	{
		dir = tools::file::check_directory_string(root_dir)+sub_directory;
		dir = tools::file::check_directory_string(dir);
		tools::file::write_to_file(dir+filename_p,&output,false);
	}
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
	if (measurement->filename_p->wafer>-1) return "w0"+to_string(measurement->filename_p->wafer);
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
	if (filename != "" && lot()!=LOT_DEFAULT && wafer()!=WAFER_DEFAULT) check_placeholders(filename);
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





/*EXPORT_T*/
export_t::export_t(measurement_t *measurement_p)
{
	measurement = measurement_p;
// 	measurement->clusters["31P"].concentration().to_screen();
	format_id();
	get_directory();
	get_filename();
}

// export_t::export_t(measurement_group_t *MG)
// {
// 	if (MG->measurements.size()==0) return;
// 	measurement = MG->measurements.front();
// // 	measurement->clusters["31P"].concentration().to_screen();
// 	format_id();
// 	get_directory();
// 	get_filename();
// }



bool export_t::format_id()
{
	if (measurement->filename_p->olcdb>-1) olcdb = to_string(measurement->filename_p->olcdb);
	if (measurement->filename_p->wafer>-1) wafer = "w0"+to_string(measurement->filename_p->wafer);
	if (measurement->filename_p->wafer>9) wafer = "w"+to_string(measurement->filename_p->wafer);
	if (measurement->filename_p->group>-1) group = "g"+to_string(measurement->filename_p->group)+measurement->filename_p->repetition;
	if (measurement->filename_p->lot!="") lot = measurement->filename_p->lot;
	if (measurement->filename_p->lot_split!="") lot_split = measurement->filename_p->lot_split;
	if (measurement->filename_p->chip_x>-1) 
	{
		if (measurement->filename_p->chip_x>9) chip = "X"+to_string(measurement->filename_p->chip_x);
		else chip = "X0"+to_string(measurement->filename_p->chip_x);
		if (measurement->filename_p->chip_y>9) chip += "Y"+to_string(measurement->filename_p->chip_y);
		else chip += "Y0"+to_string(measurement->filename_p->chip_y);
	}
	if (measurement->filename_p->monitor!="") monitor = "m"+measurement->filename_p->monitor;
	if (measurement->tool_name()!="") tool = measurement->tool_name();
	return true;
}

bool export_t::check_replacements(string& check_this)
{
	vector <string> placeholders = tools::str::get_all_string_between_string_A_and_next_B(&check_this,"{","}");
	string olcdbid="olcdbid-unknown";
	string lotid="lot-unknown";
	string lot_splitid="";
	string waferid="wafer-unknown";
	string groupid="group-unknown";
	string chipid="";
	string monitorid="";
	string toolid="tool-unknown";
	string energy="";
	if (olcdb!="") olcdbid=olcdb;
	if (lot!="") lotid=lot;
	if (lot_split!="") lot_splitid=lot_split;
	if (wafer!="") waferid=wafer;
	if (group!="") groupid=group;
	if (chip!="") chipid=chip;
	if (monitor!="") monitorid=monitor;
	if (tool!="") toolid=tool;
	if (measurement->settings.sputter_energy().is_set()) energy=to_string((int)(measurement->settings.sputter_energy().data[0]))+measurement->settings.sputter_energy().unit+measurement->settings.sputter_element();
	
	for (string placeholder:placeholders)
	{
		string replacethis = "{"+placeholder+"}";
		if (placeholder=="olcdbid" || placeholder=="olcdb") tools::str::replace_chars(&check_this,"{"+placeholder+"}",olcdbid);
		else if (placeholder=="waferid" || placeholder=="wafer") tools::str::replace_chars(&check_this,"{"+placeholder+"}",waferid);
		else if (placeholder=="groupid" || placeholder=="group") tools::str::replace_chars(&check_this,"{"+placeholder+"}",groupid);
		else if (placeholder=="lotid" || placeholder=="lot") tools::str::replace_chars(&check_this,"{"+placeholder+"}",lotid);
		else if (placeholder=="lotsplit" || placeholder=="lot_split") tools::str::replace_chars(&check_this,"{"+placeholder+"}",lot_splitid);
		else if (placeholder=="chipid" || placeholder=="chip") tools::str::replace_chars(&check_this,"{"+placeholder+"}",chipid);
		else if (placeholder=="monitorid" || placeholder=="monitor") tools::str::replace_chars(&check_this,"{"+placeholder+"}",monitorid);
		else if (placeholder=="toolid" || placeholder=="tool" || placeholder=="tool_name") tools::str::replace_chars(&check_this,"{"+placeholder+"}",toolid);
		else if (placeholder=="energy") tools::str::replace_chars(&check_this,"{"+placeholder+"}",energy);
	}
	return true;
}

bool export_t::get_directory()
{
	if (directory!="") return true;
	directory = get_directory(measurement);
	if (!check_replacements(directory)) return false;
	if (directory=="") return false;
	tools::file::mkpath(directory,0750);
	return true;
}

string export_t::get_directory(measurement_t *measurement)
{
	string directory;
	if (conf.export_location!="")
	{
		directory=conf.export_location;
// 		directory = tools::file::check_directory_string(directory);
	}
	else directory = measurement->filename_p->directory();
	directory = tools::file::check_directory_string(directory);
	return directory;
}

bool export_t::get_filename(string file_ending)
{
	filename = conf.export_filename;
	if (filename != "" && lot!="" && wafer!="") check_replacements(filename);
	else
	{
		filename = olcdb;
		if (lot!="") filename += "_" + lot;
		if (wafer!="") filename += "_" + wafer;
		if (chip!="") filename += "_" + chip;
		if (monitor!="") filename += "_" + monitor;
		if (group!="") filename += "_" + group;
	
		if (lot==""||wafer==""||group=="") filename = measurement->filename_p->filename_without_crater_depths();
		if (measurement->tool_name()!="" && measurement->tool_name()!="NULL") filename+="."+measurement->tool_name();
		filename+=file_ending;
	}
	tools::str::replace_chars(&filename,"__","_");
	tools::str::replace_chars(&filename,"__","_");
	tools::str::replace_chars(&filename,"__","_");
	return true;
}


bool export_t::write_to_file()
{
// 	if (!get_directory()) return false;
// 	if (!get_filename()) return false;
	if (directory=="") return false;
	if (filename=="") return false;
	tools::file::mkpath(directory,0750);
	if (!tools::file::write_to_file(directory+filename,&output,false)) return false;
	return true;
}

bool export_t::samples_overview(list<measurement_t> *measurements)
{
	if (measurements->size()==0) return false;
	vector<vector<string>> output_matrix;
	vector<string> header_line={"measurement name"};
	for (measurement_t measurement:*measurements)
		for (map<string,cluster_t>::iterator it=measurement.clusters.begin();it!=measurement.clusters.end();++it)
			header_line.push_back(it->second.name());
	tools::str::remove_duplicates(&header_line);
	for (measurement_t measurement:*measurements)
	{
			vector<string> line;
			
			line.push_back(measurement.filename_p->filename_without_crater_depths());
			for (int i=1;i<header_line.size();i++)
			{
				cluster_t* cluster = &(measurement.clusters[header_line[i]]);
				if (cluster!=nullptr)
				{
// 					quantity_t* quantity = &cluster->intensity();
// 					TODO
// 					if (quantity!=nullptr) line.push_back(statistics::get_median_from_Y());
				}
			}
	}
	return true;
}

bool export_t::Ipr_global(list<measurement_t> *measurements)
{
	vector<string> global_time={"Ipr","global_sputter_time","s"};
	vector<string> global_Ipr={"Ipr","sputter_current","nA"};
	for (auto measurement : *measurements)
	{
		if (measurement.crater.sputter_current().data.size()==0) continue;

// 		tools::vec::add(&global_Ipr,tools::mat::double_vec_to_str_vec(measurement->cluster_by_name("Ipr")->quantity_by_name("intensity")->data));
		tools::vec::add(&global_time,tools::mat::double_vec_to_str_vec(measurement.crater.global_sputter_time().data));
		tools::vec::add(&global_Ipr,tools::mat::double_vec_to_str_vec(measurement.crater.sputter_current().data));
		
	}
		conf.export_location = tools::file::check_directory_string(conf.export_location);
		if (conf.export_location=="") return false;
		tools::file::mkpath(conf.export_location,0750);
		vector<vector<string>> out_mat = tools::mat::transpose_matrix({global_time,global_Ipr});
		string output = tools::mat::format_matrix_to_string(&out_mat);
		if (!tools::file::write_to_file(conf.export_location+"Ipr_global.txt",&output,false)) return false;

	return true;
}




/////////////////////////////////////////
////////////// ORIGIN ///////////////////
/////////////////////////////////////////

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
	else if (measurement->filename_p->not_parseable_filename_parts.size()!=0 && (lot()==LOT_DEFAULT || wafer()==WAFER_DEFAULT)) 
		suffix += "_"+tools::vec::combine_vec_to_string(measurement->filename_p->not_parseable_filename_parts,"_");
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
	longname = name +" "+quantity->name;
	unit = quantity->unit;
	comment = name+suffix;
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
	vector<column_t> columns;
	if (measurement->crater.sputter_depth().is_set()) columns.push_back(column_t(measurement->crater.sputter_depth(conf.depth_resolution),"crater_"+suffix()));
	else columns.push_back(column_t(measurement->crater.sputter_time(),"crater_"+suffix()));
// 	columns.push_back(column_t(measurement->crater.global_sputter_time(),"crater_"+suffix()));
	for (auto& cluster : measurement->clusters)
	{
		if (cluster.second.concentration().is_set()) columns.push_back(column_t(&cluster.second,cluster.second.concentration(),"_"+suffix()));
		else columns.push_back(column_t(&cluster.second,cluster.second.intensity(),"_"+suffix()));
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

void origin_t::export_to_files(measurement_group_t MG_p)
{
// 	set<string> root_directories;
	for (auto& measurement: MG_p.measurements)
	{
		origin_t origin (measurement);
		origin.write_to_file(origin.format_measurement_cluster_columns(),origin.root_directory(conf.export_location),origin.filename(conf.export_filename));
// 		root_directories.insert(origin.root_directory(conf.export_location));
	}
}

void origin_t::export_MG_parameters_to_file(measurement_group_t& MG_p)
{
	set<string> root_directories;
	for (auto& measurement: MG_p.measurements)
	{
		origin_t origin (measurement);
		root_directories.insert(origin.root_directory(conf.export_location));
	}
	
	vector<string> filenames;
	
	/*this can be wrong, when measurements are missing some values!*/
	map<string,quantity_t> clustername_to_RSF;
	quantity_t SRs;
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
		else SRs.data.push_back(-1);
		filenames.push_back(M->filename_p->filename_with_path());
	}
	
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
	
	if (!SRs.is_set() && clustername_to_RSF.size()==0) return;
	vector<column_t> cols;
	cols.push_back(column_t(filenames,"filenames","",""));
	
	vector<string> SRs_data(SRs.data.size());
	for (int i=0;i<SRs_data.size();i++)
	{
		if (SRs.data[i]<0) SRs_data[i]="";
		else SRs_data[i] = to_string(SRs.data[i]);
	}
	cols.push_back(column_t(SRs_data,SRs.name,SRs.unit,""));
	
	for (auto& RSFs:clustername_to_RSF)
	{
		vector<string> RSF_data(RSFs.second.data.size());
		for (int i=0;i<RSF_data.size();i++)
		{
			if (RSFs.second.data[i]<0) RSF_data[i]="";
			else RSF_data[i] = to_string(RSFs.second.data[i]);
		}
// 		cols.push_back(column_t("RSF "+RSFs.first,RSFs.second));
		cols.push_back(column_t(RSF_data,"RSF "+RSFs.first,RSFs.second.unit,""));
	}
	
	
	for (auto& dir:root_directories)
		write_to_file(cols,dir+conf.calc_location,"MG-parameters.txt");
}


void origin_t::export_jiang_parameters_to_file(calc_models_t::jiang_t& jiang)
{
	if (jiang.is_error()) return;
	if (conf.calc_location=="") return;
	set<string> root_directories;
	for (auto& measurement: jiang.measurement_group().measurements)
	{
		origin_t origin (measurement);
// 		origin.write_to_file(origin.format_measurement_cluster_columns(),origin.root_directory(conf.export_location),origin.filename(conf.export_filename));
		root_directories.insert(origin.root_directory());
	}

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
	for (auto& dir:root_directories)
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

/////////////////////////////////////////
////////////   PLOT   ///////////////////
/////////////////////////////////////////

void plot_t::fast_plot(quantity_t X, quantity_t Y, string outputfile) 
{
	bool logscale = false;
	
	
	Gnuplot g1;
// 	string outputfile = "/tmp/exports/"+ Y.name+ "_vs_" +X.name;
	g1.savetops(outputfile.c_str());
// 	g1.set_title(Y.name+ " vs " +X.name);
	g1.set_style("points");
	g1.set_pointsize(1);
	if (logscale) g1.set_ylogscale();
	g1.set_ylabel(Y.name + " [" + Y.unit + "]");
	g1.set_xlabel(X.name + " [" + X.unit + "]");
	g1.plot_xy(X.data ,Y.data, Y.name);
}


void plot_t::export_to_files(list<measurement_t>* measurements)
{	
	for (auto& measurement:*measurements)
	{
		plot_t plot(&measurement);
	}
	return;
}


plot_t::plot_t(measurement_t* measurement) : export_t (measurement)
{
	bool logscale = true;
	bool save=true;
	
	string title, outputfile;
    
    outputfile = directory+measurement->filename_p->filename()+"_PLOT";
// 	cout << "outputfile=" << outputfile<< endl;
    title=measurement->filename_p->filename();
    tools::str::replace_chars(&title,"_","\\\\_");
    Gnuplot g1;
    
    g1.set_title(title);
    if (logscale) g1.set_ylogscale();
	vector<double> Y,X ;
	quantity_t Xq;
	
	for (auto& cluster:measurement->clusters)
	{
		
//         bool out=false;
//         for (int i=0;i<element_names.size();i++) {
//             if (it->first == element_names[i]) { out = true; break;}
//         }
//         if (out || element_names.size()==0) {

            /*create the actual plot or its temporary files*/
            
			string plotname;
			
            /*raw_data*/
            if (save) g1.savetops(outputfile.c_str());
            g1.set_style("points");
            g1.set_pointsize(0.05);
			
			
			if (cluster.second.concentration().is_set())
			{
				Y = cluster.second.concentration().data;
				g1.set_ylabel(cluster.second.concentration().name + "["+ cluster.second.concentration().unit + "]");
				plotname=cluster.second.name()+"\\\\_concentration";
			}
			else
			{
				Y = cluster.second.intensity().data;
				g1.set_ylabel(cluster.second.intensity().name + "["+ cluster.second.intensity().unit + "]");
				plotname=cluster.second.name()+"\\\\_intensity";
			}
			if (cluster.second.sputter_depth().is_set() )
			{
				X = cluster.second.sputter_depth().data;
				g1.set_xlabel(cluster.second.sputter_depth().name + "["+ cluster.second.sputter_depth().unit + "]");
			}
			else
			{
				X = cluster.second.sputter_time().data;
				g1.set_xlabel(cluster.second.sputter_time().name + "["+ cluster.second.sputter_time().unit + "]");
			}
// 			cout << "cluster.name()=" << cluster.second.name() << endl;
// 			cout << "Y.size()=" << Y.size() << endl;
// 			cout << "X.size()=" << X.size() << endl;
/////	/// SKIP
// 			if (statistics::get_median_from_Y(Y)>1E6) continue;
			
            g1.plot_xy(X ,Y,plotname);
            
//             /*interp_data*/
//             if (save) g1.savetops(outputfile.c_str());
//             g1.set_style("points");
//             g1.set_pointsize(0.05);
//             /*vector<double> */Y = it->second.interp_data.data_XY_transposed[1];
//             for (int i=0;i<Y.size();i++) Y[i]=Y[i]/it->second.interp_data.stats.median; 
//             g1.plot_xy(it->second.interp_data.data_XY_transposed[0] ,Y,it->first +"\\\\_interp");
            
            /*polyfit_data*/
//             if (save) g1.savetops(outputfile.c_str());
//             g1.set_style("lines");
// 			if (cluster.second.sputter_depth().is_set()) Xq = cluster.second.sputter_depth();
// 			else Xq = cluster.second.sputter_time();
//             if (cluster.second.concentration().is_set()) Y = cluster.second.concentration().moving_window_sd(5).moving_window_sd(5).moving_window_median(10).data;
// 			else Y = cluster.second.intensity().moving_window_sd(5).moving_window_sd(5).moving_window_median(10).data;
//             g1.plot_xy(Xq.data ,Y,cluster.second.name()+"\\\\_sd_median");
			
			 /*equilibrium*/
            if (save) g1.savetops(outputfile.c_str());
            g1.set_style("lines");
            if (cluster.second.equilibrium().concentration().is_set()) Y = cluster.second.equilibrium().concentration().data;
			else Y = cluster.second.equilibrium().intensity().data;
			if (cluster.second.equilibrium().sputter_depth().is_set() ) X = cluster.second.equilibrium().sputter_depth().data;
			else X = cluster.second.equilibrium().sputter_time().data;
            g1.plot_xy(X ,Y,cluster.second.name()+"\\\\_equilibrium");
			
			
			
			/*moving window statistics*/
//             if (save) g1.savetops(outputfile.c_str());
//             g1.set_style("lines");
//             if (cluster.second.concentration().is_set()) Y = cluster.second.concentration().moving_window_sd(5).data;
// 			else Y = cluster.second.intensity().moving_window_sd(5).data;
// 			if (cluster.second.sputter_depth().is_set() ) X = cluster.second.sputter_depth().data;
// 			else X = cluster.second.sputter_time().data;
//             g1.plot_xy(X ,Y,cluster.second.name()+"\\\\_sd");
			
// 			if (save) g1.savetops(outputfile.c_str());
//             g1.set_style("lines");
//             Y = cluster.second.intensity().moving_window_mean(20).data;
//             g1.plot_xy(measurement->crater.sputter_time().data ,Y,cluster.second.name()+"\\\\_mean");
			
// 			/*filtering*/
// 			if (save) g1.savetops(outputfile.c_str());
//             g1.set_style("lines");
//             Y = cluster.second.intensity().filter_gaussian(cluster.second.intensity().data.size()*0.01,0.001).data;
//             g1.plot_xy(measurement->crater.sputter_time().data ,Y,cluster.second.name()+"\\\\_filtered-gaussian");
//         }

        
    }
    
	
//     if (save) g1.savetops(outputfile.c_str());
// 	g1.set_style("lines");
// 	Y = measurement->reference_clusters()[0]->reference_intensity().data;
// 	g1.plot_xy(measurement->reference_clusters()[0]->sputter_time().data ,Y,"reference-intensity");
    
    vector<string> files=g1.get_tmpfiles();
    files_to_delete.insert(files_to_delete.end(),files.begin(),files.end());
    return;
}

plot_t::~plot_t()
{
	tools::file::remove_files(files_to_delete);
	return;
}

void plot_t::XY(map<double,double> XY, string outputfile) 
{
	vector<double> X,Y;
	tools::vec::split_map_to_vecs(&XY,&X,&Y);
	plot_t::XY(X,Y,outputfile);
	return;
}

void plot_t::XY(vector<double> X, vector<double> Y,string outputfile) 
{
    Gnuplot g;
    g.set_title("TEST");
    g.set_ylogscale();
    g.savetops("/tmp/"+outputfile);
    g.set_style("lines");
    g.plot_xy(X ,Y);
    return;
}
