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
#include "measurement_tools_t.hpp"
#include "measurement.hpp"
#include "sample_t.hpp"

measurement_tools_t::measurement_tools_t()
{
	get_file_types_endings_list();
// 	cout << "ffile_types_endings_list: " << endl;
// 	for (auto& file_type_ending:file_types_endings_list)
// 		cout << file_type_ending << endl;
}

list<measurement_t> measurement_tools_t::measurements(list<filename_t>* filenames_p, list<sample_t> *samples_p)
{
	/// this could be multithreaded
	list<measurement_t> measurements;
	for (list<filename_t>::iterator filename = filenames_p->begin();filename!=filenames_p->end();++filename)
	{
		measurement_t measurement_p = measurement(&*filename);
		// just save measurements without any errors
		if (measurement_p.tool_name()!="NULL") 
		{
			add_sample(samples_p, &measurement_p);
			measurements.push_back(measurement_p);
			add_measurement_to_cluster(&measurements.back());
			measurements.back().crater.clusters=&measurements.back().clusters;
		}
	}
	parser_methods::copy_secondaries_and_delete_from_list(&measurements);
	return measurements;
}

/*new parsers have to be added here*/
measurement_t measurement_tools_t::measurement(filename_t* filename_p)
{
	measurement_t measurement_p;
	string file_contents="";
	//load file//

	if (!conf.ignore_file_type_endings && !is_correct_file_type(filename_p)) return measurement_p; // aboart if file_type_ending is not listed
	file_contents = tools::file::load_file_to_string(filename_p->filename_with_path());
	file_contents=tools::str::to_ascii(file_contents);
	tools::str::remove_substring_from_mainstring(&file_contents,"\r");

	
	cout << "[" << parsed_measurements_counter << "]trying to parse: " << filename_p->filename_with_path();
	
	
	/*parser methods*/
	// dsims_asc
	dsims_asc_t dsims_asc;
	cout << "... dsims_asc ... ";
	if ((conf.ignore_file_type_endings || is_correct_file_type(filename_p,dsims_asc.get_file_type_endings())) && dsims_asc.parse(filename_p,&file_contents)) 
	{
		parsed_measurements_counter++;
		tools::vec::add(&error_messages,dsims_asc.get_error_messages());
		cout << "SUCCESS! " << /*filename_p->filename_with_path() <<*/ endl;
		return dsims_asc.measurement();
	} 
	else tools::vec::add(&error_messages,dsims_asc.get_error_messages());

	
	// tofsims_txt
	tofsims_txt_t tofsims_txt;
	cout << "... tofsims_txt ... ";
	if ((conf.ignore_file_type_endings || is_correct_file_type(filename_p,tofsims_txt.get_file_type_endings())) && tofsims_txt.parse(filename_p,&file_contents)) 
	{
		parsed_measurements_counter++;
		tools::vec::add(&error_messages,tofsims_txt.get_error_messages());
		cout << "SUCCESS! " << /*filename_p->filename_with_path() << */endl;
		return tofsims_txt.measurement();
	} 
	else tools::vec::add(&error_messages,tofsims_txt.get_error_messages());


	// images
	images_t images;
	cout << "... images ... " ;
	if ((conf.ignore_file_type_endings || is_correct_file_type(filename_p,images.get_file_type_endings())) && images.parse(filename_p,&file_contents))  
	{
		parsed_measurements_counter++;
		tools::vec::add(&error_messages,images.get_error_messages());
		cout << "SUCCESS! " << /*filename_p->filename_with_path() << */endl;
		return images.measurement();
	}
	else tools::vec::add(&error_messages,images.get_error_messages());

	// dektak32_txt_t
	dektak32_txt_t dektak32_txt;
	cout << "... dektak32_txt ... " ;
	if ((conf.ignore_file_type_endings || is_correct_file_type(filename_p,dektak32_txt.get_file_type_endings())) && dektak32_txt.parse(filename_p,&file_contents)) 
	{
		parsed_measurements_counter++;
		tools::vec::add(&error_messages,dektak32_txt.get_error_messages());
		cout << "SUCCESS! " << /*filename_p->filename_with_path() << */endl;
		return dektak32_txt.measurement();
	}
	else tools::vec::add(&error_messages,dektak32_txt.get_error_messages());

	// xps_csv_t
	xps_csv_t xps_csv;
	cout << "... xps_csv ... " ;
	if ((conf.ignore_file_type_endings || is_correct_file_type(filename_p,xps_csv.get_file_type_endings())) && xps_csv.parse(filename_p,&file_contents)) 
	{
		parsed_measurements_counter++;
		tools::vec::add(&error_messages,xps_csv.get_error_messages());
		cout << "SUCCESS! " << /*filename_p->filename_with_path() << */endl;
		return xps_csv.measurement();
	}
	else tools::vec::add(&error_messages,xps_csv.get_error_messages());

	
	/****************/
	cout << "... failed! no matching parser available" << endl;
	tools::vec::add(&error_messages, {filename_p->filename_with_path() + ": no parser applicable"});
	
	return measurement_p;
}

/*new parsers have to be added here*/
bool measurement_tools_t::get_file_types_endings_list()
{
	file_types_endings_list.clear();
	
	dsims_asc_t dsims_asc;
	file_types_endings_list.insert(dsims_asc.get_file_type_endings().begin(),dsims_asc.get_file_type_endings().end());
	
	tofsims_txt_t tofsims_txt;
	file_types_endings_list.insert(tofsims_txt.get_file_type_endings().begin(),tofsims_txt.get_file_type_endings().end());
	
	images_t images;
	file_types_endings_list.insert(images.get_file_type_endings().begin(),images.get_file_type_endings().end());
	
	dektak32_txt_t dektak32_txt;
	file_types_endings_list.insert(dektak32_txt.get_file_type_endings().begin(),dektak32_txt.get_file_type_endings().end());
	
	xps_csv_t xps_csv;
	file_types_endings_list.insert(xps_csv.get_file_type_endings().begin(),xps_csv.get_file_type_endings().end());
	
	if (file_types_endings_list.find("NULL")!=file_types_endings_list.end())
	{
		cout << "measurement_tools_t::get_file_types_endings_list(): file_types_endings_list.find('NULL')" << endl;
		return false;
	}
	/****************/
	return true;
}

bool measurement_tools_t::is_correct_file_type(filename_t* filename_p, unordered_set<string> file_type_endings_list_p)
{
	if (file_type_endings_list_p.empty()) file_type_endings_list_p = file_types_endings_list;

	vector<string> file_types(file_type_endings_list_p.begin(),file_type_endings_list_p.end());
// 	print(file_types);
// 	if (file_types.find(filename_p->file_type_ending())!=file_types.end()) return true;
	
	if (tools::mat::is_in_vec(&file_types,filename_p->type_ending())>-1) return true;
	return false;
}

void measurement_tools_t::add_measurement_to_cluster(measurement_t *measurement)
{
	for (auto& cluster:measurement->clusters)
		cluster.second.measurement = measurement;
// 	measurement->crater.measurement=measurement;
}

void measurement_tools_t::add_sample(list<sample_t> *samples, measurement_t *measurement)
{
	bool added;
	added=false;
	sample_t sample_p(measurement->filename_p);
	for (auto& sample:*samples)
	{
		if(sample == sample_p)
		{
			measurement->sample_p=&sample;
			added=true;
			break;
		}
	}
	if (!added) 
	{
		samples->push_back(sample_p);
		measurement->sample_p=&samples->back();
	}
}

/*new parsers have to be added here*/
vector<string> measurement_tools_t::get_error_messages()
{
	return error_messages;
}

