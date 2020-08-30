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
#include "measurement_group_t.hpp"
#include "measurement.hpp"


bool measurement_group_t::defined_olcdbid=true;
bool measurement_group_t::defined_groupid=true;
bool measurement_group_t::defined_settings=true;
bool measurement_group_t::defined_tool=true;

measurement_group_t::measurement_group_t()
{
	
}

bool measurement_group_t::check_cluster_names()
{
	set<string> clusternames = cluster_names();
	for (auto& measurement:measurements)
	{
		for (auto& clustername:clusternames)
			if (measurement->clusters.find(clustername)==measurement->clusters.end()) return false;
	}
	return true;
}

bool measurement_group_t::check_cluster_names(measurement_group_t* measurement_group)
{
	set<string> clusternames = cluster_names();
	for (auto& measurement:measurement_group->measurements)
	{
		for (auto& clustername:clusternames)
			if (measurement->clusters.find(clustername)==measurement->clusters.end()) return false;
		for (auto& cluster:measurement->clusters)
			if (clusternames.find(cluster.second.name())==clusternames.end()) return false;
	}
	return true;
}

quantity_t measurement_group_t::RSFs(cluster_t& cluster)
{
	quantity_t RSFs_p;
	for (auto& M:measurements)
	{
		if (M->sample_p->matrix != cluster.measurement->sample_p->matrix) continue;
		for (auto& C:M->clusters)
		{
			if (C.second.name()!=cluster.name()) continue;
			if (!C.second.RSF().is_set()) continue;
			if (RSFs_p.is_set()) RSFs_p = RSFs_p.add_to_data(C.second.RSF());
			else RSFs_p = C.second.RSF();
			break;
		}
	}
	return RSFs_p;
}

quantity_t measurement_group_t::SRs(sample_t::matrix_t& matrix)
{
	quantity_t SRs_p;
	for (auto& M:measurements)
	{
		if (M->sample_p->matrix != matrix) continue;
		if (SRs_p.is_set()) SRs_p = SRs_p.add_to_data(M->crater.sputter_rate());
		else SRs_p = M->crater.sputter_rate();
	}
	return SRs_p;
}



measurement_group_t::measurement_group_t(measurement_t* measurement_p)
{
	tool_name_p = measurement_p->tool_name();
	group_p = measurement_p->filename_p->group;
// 	cout << "group_p = " << group_p <<endl;
	olcdb_p = measurement_p->filename_p->olcdb;
	settings_p = measurement_p->settings;
	measurements.push_back(measurement_p);
}

map<quantity_t, sample_t::matrix_t> measurement_group_t::SR_vs_matrix()
{
	map<quantity_t, sample_t::matrix_t> SR_vs_matrix_p;
	for (auto& measurement:measurements)
	{
		if (measurement->crater.sputter_rate().is_set() && measurement->sample_p->matrix.isotopes().size()>0)
			SR_vs_matrix_p.insert({measurement->crater.sputter_rate(),measurement->sample_p->matrix});
	}
	return SR_vs_matrix_p;
}

map<quantity_t, sample_t::matrix_t> measurement_group_t::RSF_vs_matrix(std::__cxx11::string cluster_name)
{
	map<quantity_t, sample_t::matrix_t> RSF_vs_matrix_p;
	
	
	for (auto& measurement:measurements)
	{
		if (measurement->clusters[cluster_name].RSF().is_set() && measurement->sample_p->matrix.isotopes().size()>0)
			RSF_vs_matrix_p.insert({measurement->clusters[cluster_name].RSF(),measurement->sample_p->matrix});
	}
	return RSF_vs_matrix_p;
}

map<sample_t::matrix_t, vector<cluster_t *> > measurement_group_t::matrix_vs_ref_clusters()
{
	map<sample_t::matrix_t, vector<cluster_t *> > matrix_vs_ref_clusters_p;
	
	for (auto& measurement:measurements)
	{
		if (measurement->sample_p->matrix.isotopes().size()>0)
			matrix_vs_ref_clusters_p.insert({measurement->sample_p->matrix,measurement->reference_clusters()});
	}
	
	return matrix_vs_ref_clusters_p;
}


bool measurement_group_t::add(measurement_t* measurement)
{
	measurement_group_t mgroup(measurement);
	return add(&mgroup);
}

bool measurement_group_t::add(measurement_group_t* measurement_group)
{
	
	if (*measurement_group==*this)
	{
		measurements.merge(measurement_group->measurements);
		return true;
	}
	return false;
}

list<measurement_group_t> measurement_group_t::measurement_groups(list<measurement_t>* measurements)
{
	list<measurement_group_t> measurement_groups;
	bool added;
	for (auto& measurement:*measurements)
	{
		added=false;
		measurement_group_t mgroup(&measurement);
		// is there an exisiting measurement_group, which matches?
		for (auto& measurement_group:measurement_groups)
		{
			if (measurement_group.add(&mgroup))
			{
				measurement.measurement_group=&measurement_group;
				added=true;
				break;
			}
		}
		if (!added)
		{
			measurement_groups.push_back(mgroup);
			measurement.measurement_group=&measurement_groups.back();
		}
	}
	return measurement_groups;
}

void measurement_group_t::to_screen(std::__cxx11::string prefix)
{
	cout << prefix << "tool_name = " << tool_name_p << endl;
	cout << prefix << "group = " << group_p << endl;
	cout << prefix << "olcdb = " << olcdb_p << endl;
	settings_p.to_screen(prefix);
	cout << prefix << "measurements.size() = " << measurements.size() << endl;
	for (auto& measurement:measurements)
	{
		cout << prefix << "\t" << measurement->filename_p->filename_with_path() << endl;
	}
	return;
}

int measurement_group_t::group()
{
	return group_p;
}

int measurement_group_t::olcdb()
{
	return olcdb_p;
}

string measurement_group_t::tool_name()
{
	return tool_name_p;
}

measurement_settings_t measurement_group_t::settings()
{
	return settings_p;
}

set<string> measurement_group_t::reference_matrix_cluster_names()
{
	if (reference_cluster_names_p.size()>0) return reference_cluster_names_p;
	for (auto& measurement:measurements)
	{
		if (!measurement->load_from_database()) continue;
		for (auto& cluster:measurement->clusters)
		{
			if (cluster.second.leftover_elements(measurement->sample_p->matrix.isotopes()).size()==0)
				reference_cluster_names_p.insert(cluster.second.name());
		}
	}
	return reference_cluster_names_p;
}


vector<std::__cxx11::string> measurement_group_t::error_messages()
{
	return error_messages_p;
}

vector<isotope_t> measurement_group_t::reference_matrix_isotopes_vec()
{
	set<isotope_t> isotopes_set = reference_matrix_isotopes();
	vector<isotope_t> isotopes(isotopes_set.begin(),isotopes_set.end());
	return isotopes;
}

set<isotope_t> measurement_group_t::reference_matrix_isotopes()
{
	if (reference_matrix_isotopes_p.size()>0) return reference_matrix_isotopes_p;
	vector<isotope_t> isotopes_;
	for (auto& measurement:measurements)
	{
		if (!measurement->load_from_database()) continue;
		isotopes_ = measurement->sample_p->matrix.isotopes();
		reference_matrix_isotopes_p.insert(isotopes_.begin(),isotopes_.end());
	}
	return reference_matrix_isotopes_p;
}

/*ATTENTION! "28Si 11B" and "30Si 11B" can not be distinguished! this function is bijective: isotope <=> cluster*/
isotope_t measurement_group_t::isotope_from_cluster_name(string cluster_name)
{
	if (cluster_name_to_isotope_p.size()>0 && cluster_name_to_isotope_p.find(cluster_name)!=cluster_name_to_isotope_p.end()) return cluster_name_to_isotope_p[cluster_name];
	
	for (auto& isotope:isotopes())
	{
		string cluster_name = cluster_name_from_isotope(isotope);
		if (cluster_name!="") cluster_name_to_isotope_p[cluster_name] = isotope;
	}
	
	if (cluster_name_to_isotope_p.find(cluster_name)!=cluster_name_to_isotope_p.end()) return cluster_name_to_isotope_p[cluster_name];
	return {};
}

// isotope_t measurement_group_t::isotope_from_cluster_name(string cluster_name)
// {	
// 	
// 	for (auto& reference_matrix_isotope:reference_matrix_isotopes())
// 	{
// 		
// 	}
// 	
// 	for (auto& measurement:measurements)
// 	{
// 		if (!measurement->load_from_database()) continue;
// 		for (auto& cluster:measurement->clusters)
// 		{
// 			if (cluster.second.leftover_elements(measurement->sample_p->matrix.isotopes()).size()==0)
// 				reference_cluster_names_p.insert(cluster.second.name());
// 		}
// 	}
// 	return {};
// }



set<isotope_t> measurement_group_t::isotopes()
{
	set<isotope_t> isotopes_p;
	for (auto& measurement:measurements)
	{
		set<isotope_t> isotopes_in_m = measurement->isotopes();
		for (auto& isotope:isotopes_in_m)
		isotopes_p.insert(isotopes_in_m.begin(),isotopes_in_m.end());
	}
	return isotopes_p;
}

set<std::__cxx11::string> measurement_group_t::cluster_names()
{
	if (cluster_names_p.size()>0) return cluster_names_p;
	for (auto& measurement:measurements)
	{
		for (auto& cluster:measurement->clusters)
			cluster_names_p.insert(cluster.second.name());
	}
	return cluster_names_p;
}

/*ATTENTION! "28Si 11B" and "30Si 11B" can not be distinguished! this function is bijective: isotope <=> cluster*/
std::__cxx11::string measurement_group_t::cluster_name_from_isotope(isotope_t isotope)
{	
// 	vector<string> isotope_names_in_cluster = tools::str::get_strings_between_delimiter();
	// matrix isotope?	
	for (auto& matrix_cluster_name:reference_matrix_cluster_names())
	{
		if (isotope.nucleons>0) // isotop
		{
			if (matrix_cluster_name.find(to_string(isotope.nucleons)+isotope.symbol)!=string::npos) return matrix_cluster_name;
		}
		else // element
			if (matrix_cluster_name.find(isotope.symbol)!=string::npos) return matrix_cluster_name;
	}
	
	for (auto& cluster_name:cluster_names())
	{
		if (isotope.nucleons>0) // isotop
		{
			if (cluster_name.find(to_string(isotope.nucleons)+isotope.symbol)!=string::npos) return cluster_name;
		}
		else // element
			if (cluster_name.find(isotope.symbol)!=string::npos) return cluster_name;
	}
	
	return "";
}

std::__cxx11::string measurement_group_t::name()
{
	stringstream name_p;
	if (defined_olcdbid) name_p << olcdb_p << "_";
	if (defined_groupid) name_p << "g" << group_p << "_";
	if (defined_settings) 
	{
		if (settings_p.sputter_energy().is_set()) name_p << to_string((int)settings_p.sputter_energy().data[0])+settings_p.sputter_energy().unit;
		if (settings_p.sputter_element()!="") name_p << settings_p.sputter_element();
		if (settings_p.polarity()!="") name_p << settings_p.polarity();
		if (settings_p.sputter_energy().is_set() || settings_p.sputter_element()!="" || settings_p.polarity()!="") name_p << "_";
	}
	if (defined_tool)  name_p  << tool_name_p << "_";
	string result = name_p.str();
	result.erase(result.begin()+result.size()-1);
	return result;
}

std::__cxx11::string measurement_group_t::to_str(std::__cxx11::string prefix)
{
	stringstream ss;
	int i=0;
	ss << prefix << name() << endl;
	ss << prefix << "{" << endl;
	for (auto& M:measurements)
	{
		ss << "\t" << prefix << M->to_str(prefix+"M["+to_string(i)+"]\t");
		i++;
	}
	ss << prefix << "}" << endl;
	return ss.str();
}

map<std::__cxx11::string, quantity_t> measurement_group_t::clustername_to_concentrations_from_all_measurements()
{
	map<string,quantity_t> clustername_to_concentrations_from_all_measurements;
	for (auto& cluster_name:cluster_names())
	{
		for (auto& M:measurements)
		{
			/*cluster is not in this measurement*/
			if (M->clusters.find(cluster_name)==M->clusters.end())
			{
				clustername_to_concentrations_from_all_measurements[cluster_name].data.push_back(-1);
				continue;
			}
			/*cluster is in this measurement*/
			
			if (M->clusters[cluster_name].concentration().is_set())
			{
				clustername_to_concentrations_from_all_measurements[cluster_name].data.push_back(M->clusters[cluster_name].concentration().trimmed_mean().data[0]);
// 				cout << "measurement:\t" << M->filename_p->filename_with_path() << endl; 
// 				cout << "cluster_name:\t" << cluster_name << endl; 
// 				M->clusters[cluster_name].concentration().to_screen();
				if (!clustername_to_concentrations_from_all_measurements[cluster_name].is_set())
				{
					clustername_to_concentrations_from_all_measurements[cluster_name].dimension = M->clusters[cluster_name].concentration().dimension;
					clustername_to_concentrations_from_all_measurements[cluster_name].unit = M->clusters[cluster_name].concentration().unit;
				}
			}
			else 
				clustername_to_concentrations_from_all_measurements[cluster_name].data.push_back(-1);
		}
	}
	return clustername_to_concentrations_from_all_measurements;
}

map<std::__cxx11::string, quantity_t> measurement_group_t::clustername_to_intensities_from_all_measurements()
{
	map<string,quantity_t> clustername_to_intensities_from_all_measurements;
	for (auto& cluster_name:cluster_names())
	{
		for (auto& M:measurements)
		{
			/*cluster is not in this measurement*/
			if (M->clusters.find(cluster_name)==M->clusters.end())
			{
				clustername_to_intensities_from_all_measurements[cluster_name].data.push_back(-1);
				continue;
			}
			/*cluster is in this measurement*/
			
			if (M->clusters[cluster_name].intensity().is_set())
			{
				clustername_to_intensities_from_all_measurements[cluster_name].data.push_back(M->clusters[cluster_name].intensity().trimmed_mean().data[0]);
				if (!clustername_to_intensities_from_all_measurements[cluster_name].is_set())
				{
					clustername_to_intensities_from_all_measurements[cluster_name].dimension = M->clusters[cluster_name].intensity().dimension;
					clustername_to_intensities_from_all_measurements[cluster_name].unit = M->clusters[cluster_name].intensity().unit;
				}
			}
			else 
				clustername_to_intensities_from_all_measurements[cluster_name].data.push_back(-1);
		}
	}
	return clustername_to_intensities_from_all_measurements;
}

map<std::__cxx11::string, quantity_t> measurement_group_t::clustername_to_RSFs_from_all_measurements()
{
	map<string,quantity_t> clustername_to_RSFs_from_all_measurements;
	for (auto& cluster_name:cluster_names())
	{
		for (auto& M:measurements)
		{
			/*cluster is not in this measurement*/
			if (M->clusters.find(cluster_name)==M->clusters.end())
			{
				clustername_to_RSFs_from_all_measurements[cluster_name].data.push_back(-1);
				continue;
			}
			/*cluster is in this measurement*/
			
			if (M->clusters[cluster_name].RSF().is_set())
			{
				if (M->clusters[cluster_name].RSF().is_skalar_data())
					clustername_to_RSFs_from_all_measurements[cluster_name].data.push_back(M->clusters[cluster_name].RSF().data[0]);
				else
					clustername_to_RSFs_from_all_measurements[cluster_name].data.push_back(M->clusters[cluster_name].RSF().median().data[0]);
				if (!clustername_to_RSFs_from_all_measurements[cluster_name].is_set())
				{
					clustername_to_RSFs_from_all_measurements[cluster_name].dimension = M->clusters[cluster_name].RSF().dimension;
					clustername_to_RSFs_from_all_measurements[cluster_name].unit = M->clusters[cluster_name].RSF().unit;
				}
			}
			else 
				clustername_to_RSFs_from_all_measurements[cluster_name].data.push_back(-1);
		}
	}
	return clustername_to_RSFs_from_all_measurements;
}

vector<std::__cxx11::string> measurement_group_t::filenames_with_path()
{
	vector<string> filenames;
	for (auto& M:measurements)
		filenames.push_back(M->filename_p->filename_with_path());
	return filenames;
}


/*relies on same unit and dimension in all measurements*/
quantity_t measurement_group_t::SRs_from_all_measurements()
{
	quantity_t SRs;
	for (auto& M:measurements)
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
	}
	return SRs;
}


/*OPERATORS*/

bool measurement_group_t::operator==(measurement_group_t& measurement_group)
{
	// only add measurements with exactly same cluster_names into one common group
// 	if (!check_cluster_names(&measurement_group)) return false;
	if (defined_tool && tool_name_p!=measurement_group.tool_name_p) return false;
	if (defined_groupid && group_p != measurement_group.group_p) return false;
	if (defined_olcdbid && olcdb_p != measurement_group.olcdb_p) return false;
	if (defined_settings && settings_p != measurement_group.settings_p) return false;
	return true;
}


bool measurement_group_t::operator!=(measurement_group_t& measurement_group)
{
	return !operator==(measurement_group);
}

