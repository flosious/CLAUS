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

#ifndef MEASUREMENT_GROUP_T_HPP
#define MEASUREMENT_GROUP_T_HPP




// #include "identifier.hpp"
#include <unordered_set>
#include <vector>
#include <list>
#include "filename_t.hpp"
#include "sample_t.hpp" // sample_t::matrix_t
#include "cluster_t.hpp"
#include <unordered_map>
#include "calc_models_t.hpp"

#include "measurement_settings_t.hpp"

class measurement_t;

using namespace std;

class measurement_group_t
{
private:
	/// advanced calculation models
// 	calc_models_t calc_models(&(measurements.front()->measurement_group));

	measurement_settings_t settings_p;
	string tool_name_p="NULL";
	int group_p=-1;
	int olcdb_p=-1;
	set<string> reference_cluster_names_p;
	vector<string> error_messages_p;
	/// map cluster_name to Irel --> Crel pair
	map<string, pair<quantity_t,quantity_t>> Irel_Crel_bases_p;
	/// all isotopes from all reference samples within the group
	set<isotope_t> reference_matrix_isotopes_p;
	set<string> checked_RSF_clusters_list_p;
	bool checked_SR=false;
	map<string,quantity_t> RSFs_from_custer_name;
	quantity_t SRs_p;
	
	map<string,isotope_t> cluster_name_to_isotope_p;
public:
	/// tries to generate some name for this group
	string name();
	vector<string> error_messages();
	/// true if each measurement has exactly same cluster_names
	bool check_cluster_names();
	bool check_cluster_names(measurement_group_t* measurement_group);
	set<string> reference_matrix_cluster_names();
	set<isotope_t> reference_matrix_isotopes();
	/// cluster_name to the belonging isotope or element
	string cluster_name_from_isotope (isotope_t isotope);
	/// cluster_names from all measurements and clusters within the group
	set<string> cluster_names();
	/// isotopes from all measurements and clusters within the grou
	set<isotope_t> isotopes();
	isotope_t isotope_from_cluster_name(string cluster_name);
	/// all matching RSFs from all measurements
	quantity_t RSFs(string cluster_name);
	/// all matching Sputter_Rates for a given matrix/reference for all measurements
	quantity_t SRs();
	
	/// returns the sample corresponding value for the specified matrix
// 	quantity_t RSF(cluster_t& searched_cluster,measurement_t& measurement);
// 	quantity_t SR(cluster_t& searched_cluster,measurement_t& measurement);
	
	/// Irel --> Crel for the given ref_cluster_name; concentrations from reference samples
// 	pair<quantity_t,quantity_t> Irel_Crel_bases(string ref_cluster_name);
	/// Irel --> Crel for all ref_cluster_name; concentrations from reference samples
// 	map<string, pair<quantity_t,quantity_t>> Irel_Crel_bases();
// 	map<string, map<string,quantity_t>> Irel_Crel_bases();

	/// from all measurements within the group, where sample_t::matrix_t is not empty
	map<quantity_t,sample_t::matrix_t> SR_vs_matrix();
	/// for one specific dopant cluster
	map<quantity_t,sample_t::matrix_t> RSF_vs_matrix(string cluster_name);
	/// matrices -> reference_clusters
	map<sample_t::matrix_t,vector<cluster_t*>> matrix_vs_ref_clusters();
	
	list<measurement_t*> measurements;
	int olcdb();
	int group();
	string tool_name();
	measurement_settings_t settings();
	
// 	static bool comparator(const measurement_group_t& A, const measurement_group_t& B);
	
	static list<measurement_group_t> measurement_groups(list<measurement_t>* measurements);
	
	
// 	measurement_group_t();
	measurement_group_t(measurement_t* measurement_p);
	measurement_group_t();
	/// adds a measurement to the group: TRUE if success; FALSE if failed (no change to group)
	bool add(measurement_t* measurement);
	/// adds another measurement_group to this group: TRUE if success; FALSE if failed (no change to group(s))
	bool add(measurement_group_t* measurement_group);
	
// 	void add_to_reference_cluster_names(string cluster_name);
// 	void set_reference_cluster_names(vector<string> cluster_names);
	
	void to_screen(string prefix="");
	
	/*OPERATORS*/
	bool operator == (measurement_group_t& measurement_group);
	bool operator != (measurement_group_t& measurement_group);
};


#endif // MEASUREMENT_GROUP_T_HPP
