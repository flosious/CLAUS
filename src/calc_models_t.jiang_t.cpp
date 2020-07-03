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
#include "calc_models_t.jiang_t.hpp"
#include "globalvars.hpp"


// vector<string> calc_history; // calc_history.push_back("crater"+"\t" + sputter_depth_p.to_str());

calc_models_t::jiang_t::jiang_t(measurement_group_t& measurement_group_p)
{
// 	measurement_group_t MG = measurement_group_p;
	measurement_group_priv = measurement_group_p;
}

bool calc_models_t::jiang_t::is_error()
{
	return error_p;
}

bool calc_models_t::jiang_t::calc()
{
	if (!error_p) return true;
	// TODO: what if concentration or depth is already set for some clusters? --> Irel nicht immer berechenbar?
	cout << measurement_group_priv.name()  << ":\ttrying to apply Jiangs protocol...";
	calc_history.push_back("jiang_t\t" + measurement_group_priv.name());

	/*calc fit parameters*/
	map<double,double> data_XY;
	if (SRs_to_Crel().first.is_set() && SRs_to_Crel().second.is_set())
	{
		data_XY.clear();
		tools::vec::combine_vecs_to_map(SRs_to_Crel().second.data,SRs_to_Crel().first.data,&data_XY);
		if (data_XY.size()>1) SR_to_Crel_polyfit.fit(data_XY,1);
	}
	
	if (Crel_to_Irel().first.is_set() && Crel_to_Irel().second.is_set())
	{	
		data_XY.clear();
		tools::vec::combine_vecs_to_map(Crel_to_Irel().second.data,Crel_to_Irel().first.data,&data_XY);
		if (data_XY.size()>1) CRel_to_Irel_polyfit.fit(data_XY,1);
	}
	
	for (auto& RSFs_to_Crel:clustername_to_RSFs_to_Crel())
	{
		
		if (RSFs_to_Crel.second.first.is_set() && RSFs_to_Crel.second.second.is_set())
		{
			data_XY.clear();
			tools::vec::combine_vecs_to_map(RSFs_to_Crel.second.second.data,RSFs_to_Crel.second.first.data,&data_XY);
			if (data_XY.size()>1) 
			{
				clustername_to_RSFs_to_Crel_polyfit[RSFs_to_Crel.first].fit(data_XY,data_XY.size()-1);
			}
		}
	}
	
	if (!CRel_to_Irel_polyfit.fitted()) 
	{
		error_messages_p.push_back("calc_models_t::jiang_t::calc( measurement_group_t& measurement_group_p ) : !CRel_to_Irel_polyfit.fitted()");
		cout << "failed" << endl;
		return false;
	}
	
	/*apply fit parameters to all measurements*/
	for (auto& measurement:measurement_group_priv.measurements)
	{
		string counter_cluster_name = measurement_group_priv.cluster_name_from_isotope(counter_matrix_isotope());
		string denominator_cluster_name = measurement_group_priv.cluster_name_from_isotope(denominator_matrix_isotope());
		/*matrix concentrationS*/
		quantity_t Crel_p = Crel_to_Irel().first;
		if (Irel(measurement).is_set()) Crel_p.data=CRel_to_Irel_polyfit.fitted_y_data(Irel(measurement).data);
		else 
		{
			if (measurement->clusters[counter_cluster_name].concentration().is_set())
			{
				if (measurement->clusters[counter_cluster_name].concentration().unit=="at%") Crel_p = measurement->clusters[counter_cluster_name].concentration().median()/(measurement->clusters[counter_cluster_name].concentration().median()*(-1)+100);
				else continue;
			}
			else if (measurement->clusters[denominator_cluster_name].concentration().is_set())
			{
				if (measurement->clusters[denominator_cluster_name].concentration().unit=="at%") Crel_p = (measurement->clusters[denominator_cluster_name].concentration().median()*(-1)+100)/measurement->clusters[denominator_cluster_name].concentration().median();
				else continue;
			}
			else continue;
		}
		measurement_group_priv.cluster_name_from_isotope(counter_matrix_isotope());
		
		
		measurement->clusters[counter_cluster_name].concentration_p = Crel_p * 100/(Crel_p+1);
		measurement->clusters[counter_cluster_name].concentration_p.unit = "at%";
		measurement->clusters[counter_cluster_name].concentration_p.dimension = "relative";
		measurement->clusters[counter_cluster_name].concentration_p.name = "concentration";
		calc_history.push_back(measurement->filename_p->filename_with_path()+"\t"+measurement->clusters[counter_cluster_name].name()+"\t" + measurement->clusters[counter_cluster_name].concentration_p.to_str());
		measurement->clusters[counter_cluster_name].already_checked_concentration=false;
		measurement->clusters[counter_cluster_name].already_checked_RSF=false;
		measurement->clusters[counter_cluster_name].already_checked_SF=false;
		/*this is a fast hack to solve the export issue for relative elemental (no isotope) matrix concentration*/
		measurement->clusters[counter_cluster_name].isotopes.resize(1);
		measurement->clusters[counter_cluster_name].isotopes[0] = counter_matrix_isotope();
		measurement->clusters[counter_cluster_name].isotopes[0].atoms = 1;
		
		
		measurement->clusters[denominator_cluster_name].concentration_p = measurement->clusters[counter_cluster_name].concentration_p*-1+100;
		measurement->clusters[denominator_cluster_name].concentration_p.unit = "at%";
		measurement->clusters[denominator_cluster_name].concentration_p.dimension = "relative";
		measurement->clusters[denominator_cluster_name].concentration_p.name = "concentration";
		calc_history.push_back(measurement->filename_p->filename_with_path()+"\t"+measurement->clusters[denominator_cluster_name].name()+"\t" + measurement->clusters[denominator_cluster_name].concentration_p.to_str());
		measurement->clusters[denominator_cluster_name].already_checked_concentration=false;
		measurement->clusters[denominator_cluster_name].already_checked_RSF=false;
		measurement->clusters[denominator_cluster_name].already_checked_SF=false;
		/*this is a fast hack to solve the export issue for relative elemental (no isotope) matrix concentration*/
		measurement->clusters[denominator_cluster_name].isotopes.resize(1);
		measurement->clusters[denominator_cluster_name].isotopes[0] = denominator_matrix_isotope();
		measurement->clusters[denominator_cluster_name].isotopes[0].atoms=1;
		
		/*sputter_rate*/
		if (SR_to_Crel_polyfit.fitted())
		{
			quantity_t SR = SRs_to_Crel().first;
			SR.data = SR_to_Crel_polyfit.fitted_y_data(Crel_p.data);
			measurement->crater.sputter_rate_p = SR;
			calc_history.push_back(measurement->filename_p->filename_with_path()+"\t"+"crater\t" + measurement->crater.sputter_rate_p.to_str());
			measurement->crater.already_calculated_sputter_depth=false;
			measurement->crater.already_calculated_sputter_rate=false;
		}
		
		/*RSFs*/
		for (auto& cluster_name_RSF:clustername_to_RSFs_to_Crel())
		{
			if (!clustername_to_RSFs_to_Crel_polyfit[cluster_name_RSF.first].fitted()) continue;
			quantity_t RSF = clustername_to_RSFs_to_Crel()[cluster_name_RSF.first].first;
			RSF.data = clustername_to_RSFs_to_Crel_polyfit[cluster_name_RSF.first].fitted_y_data(Crel_p.data);
			measurement->clusters[cluster_name_RSF.first].RSF_p = RSF;
			measurement->clusters[cluster_name_RSF.first].already_checked_concentration=false;
			measurement->clusters[cluster_name_RSF.first].already_checked_RSF=false;
			measurement->clusters[cluster_name_RSF.first].already_checked_SF=false;
			calc_history.push_back(measurement->filename_p->filename_with_path()+"\t"+measurement->clusters[cluster_name_RSF.first].name()+"\t" + measurement->clusters[cluster_name_RSF.first].RSF_p.to_str());
		}
	}
	if (error_messages().size()>0)
	{
		
	}
	else cout << "SUCCESS!" << endl;
	calc_history.push_back("jiang_t\t" + measurement_group_priv.name() +"\tstop - SUCCESS");
	error_p = false;
	return true;
}

measurement_group_t calc_models_t::jiang_t::measurement_group()
{
	return measurement_group_priv;
}


vector<string> calc_models_t::jiang_t::error_messages()
{
	return error_messages_p;
}

map<string,pair<quantity_t,quantity_t>> calc_models_t::jiang_t::clustername_to_RSFs_to_Crel()
{
	if (clustername_to_RSFs_to_Crel_p.size()>0) return clustername_to_RSFs_to_Crel_p;

	for (auto& measurement:measurement_group_priv.measurements)
	{
		if (!Crel(measurement->sample_p->matrix).is_set()) continue;
		for (auto& cluster:measurement->clusters)
		{
			if (!cluster.second.RSF().is_set()) continue;
			// already set
			if (clustername_to_RSFs_to_Crel_p.find(cluster.second.name())!=clustername_to_RSFs_to_Crel_p.end())
			{
				clustername_to_RSFs_to_Crel_p[cluster.second.name()].first = clustername_to_RSFs_to_Crel_p[cluster.second.name()].first.add_to_data(cluster.second.RSF());
				clustername_to_RSFs_to_Crel_p[cluster.second.name()].second = clustername_to_RSFs_to_Crel_p[cluster.second.name()].second.add_to_data(Crel(measurement));
			}
			else clustername_to_RSFs_to_Crel_p[cluster.second.name()]={cluster.second.RSF(), Crel(measurement)};
		}
	}

	return clustername_to_RSFs_to_Crel_p;
}

pair<quantity_t, quantity_t> calc_models_t::jiang_t::RSFs_to_Crel ( std::__cxx11::string cluster_name )
{
	if (clustername_to_RSFs_to_Crel().find(cluster_name)!=clustername_to_RSFs_to_Crel().end()) return clustername_to_RSFs_to_Crel()[cluster_name];
	return {quantity_t(),quantity_t()};
}

pair<quantity_t,quantity_t> calc_models_t::jiang_t::SRs_to_Crel()
{
	if (SRs_to_Crel_p.first.is_set() && SRs_to_Crel_p.second.is_set()) return SRs_to_Crel_p;
	
	for (auto& measurement:measurement_group_priv.measurements)
	{
		if (!Crel(measurement->sample_p->matrix).is_set()) continue;
		if (!measurement->crater.sputter_rate().is_set()) continue;
		if (SRs_to_Crel_p.first.is_set())
		{
			SRs_to_Crel_p.second = SRs_to_Crel_p.second.add_to_data(Crel(measurement));
			SRs_to_Crel_p.first = SRs_to_Crel_p.first.add_to_data(measurement->crater.sputter_rate());
		}
		else SRs_to_Crel_p = {measurement->crater.sputter_rate(), Crel(measurement)};
	}
	if (!SRs_to_Crel_p.first.is_set()) return {};
	if (!SRs_to_Crel_p.second.is_set()) return {};
	
	return SRs_to_Crel_p;
}

pair<quantity_t,quantity_t> calc_models_t::jiang_t::Crel_to_Irel()
{
	if (Crel_to_Irel_p.first.is_set() && Crel_to_Irel_p.second.is_set()) return Crel_to_Irel_p;
	
	for (auto& measurement:measurement_group_priv.measurements)
	{
		if (!measurement->load_from_database()) continue;
		if (measurement->sample_p->matrix.isotopes.size()>2 || measurement->sample_p->matrix.isotopes.size()<1) continue;
		if (!Crel(measurement->sample_p->matrix).is_set()) continue;
		if (measurement->reference_clusters().size()!=2) continue;
		if (!Irel(measurement).is_set()) continue;
		if (Crel_to_Irel_p.first.is_set())
		{
			Crel_to_Irel_p.first = Crel_to_Irel_p.first.add_to_data(Crel(measurement));
			Crel_to_Irel_p.second = Crel_to_Irel_p.second.add_to_data(Irel(measurement).median());
		}
		else Crel_to_Irel_p = {Crel(measurement) , Irel(measurement).median()};
// 		calc_history.push_back(measurement->filename_p->filename_with_path()+"\tIrel\t" + irel_temp.to_str());
	}
	if (!Crel_to_Irel_p.first.is_set()) return {};
	if (!Crel_to_Irel_p.second.is_set()) return {};
	
	
	return Crel_to_Irel_p;
}

quantity_t calc_models_t::jiang_t::Crel(measurement_t* measurement)
{
	calc_history.push_back(measurement->filename_p->filename_with_path()+"\t"+counter_matrix_isotope().name()+"/"+denominator_matrix_isotope().name()+"\t" + Crel(measurement->sample_p->matrix).to_str());
	return Crel(measurement->sample_p->matrix);
}

quantity_t calc_models_t::jiang_t::Crel(sample_t::matrix_t& matrix)
{
	if (matrix.isotopes.size()>2) return {};
	if (!matrix.relative_concentration(counter_matrix_isotope()).is_set()) return {};
	if (!matrix.relative_concentration(denominator_matrix_isotope()).is_set()) return {};
	if (matrix.relative_concentration(denominator_matrix_isotope()).data[0]==0) return {}; // catch devision by 0, which should already never be possible to happen
	return matrix.relative_concentration(counter_matrix_isotope()) / matrix.relative_concentration(denominator_matrix_isotope());;
}

quantity_t calc_models_t::jiang_t::Irel(measurement_t* measurement)
{
	string counter_cluster_name = measurement_group_priv.cluster_name_from_isotope(counter_matrix_isotope());
	string denominator_cluster_name = measurement_group_priv.cluster_name_from_isotope(denominator_matrix_isotope());
	if (measurement->clusters.find(counter_cluster_name)==measurement->clusters.end()) return {};
	if (measurement->clusters.find(denominator_cluster_name)==measurement->clusters.end()) return {};
	if (!measurement->clusters[counter_cluster_name].intensity().is_set()) return {};
	if (!measurement->clusters[denominator_cluster_name].intensity().is_set()) return {};
	/*median is far more stable in SIMS compared to mean*/
// 	return measurement->clusters[counter_cluster_name].intensity().median()/measurement->clusters[denominator_cluster_name].intensity().median();
	quantity_t irel_temp = measurement->clusters[counter_cluster_name].intensity()/measurement->clusters[denominator_cluster_name].intensity();
	calc_history.push_back(measurement->filename_p->filename_with_path()+"\t"+counter_cluster_name+"/"+denominator_cluster_name+"\t" + irel_temp.median().to_str());
	return irel_temp;
}

isotope_t calc_models_t::jiang_t::counter_matrix_isotope()
{
	if (counter_matrix_isotope_p.is_set()) return counter_matrix_isotope_p;
	if (measurement_group_priv.reference_matrix_isotopes().size()!=2) return {};
	if (!denominator_matrix_isotope().is_set()) return {};
	set<isotope_t> matrix_isotopes = measurement_group_priv.reference_matrix_isotopes();
	vector<isotope_t> matrix_isotopes_vec (matrix_isotopes.begin(),matrix_isotopes.end());
	if (matrix_isotopes_vec[0]==denominator_matrix_isotope()) counter_matrix_isotope_p = matrix_isotopes_vec[1];
	else counter_matrix_isotope_p = matrix_isotopes_vec[0];
	counter_matrix_isotope_p.atoms=1;
	calc_history.push_back("jiang_t::counter_matrix_isotope()\t" + counter_matrix_isotope_p.name());
	return counter_matrix_isotope_p;
}

isotope_t calc_models_t::jiang_t::denominator_matrix_isotope()
{
	if (denominator_matrix_isotope_p.is_set()) return denominator_matrix_isotope_p;
	if (measurement_group_priv.reference_matrix_isotopes().size()!=2) return {};

	if (single_element_matrix_isotopes().size()>1) return {}; // jiang not possible
	if (single_element_matrix_isotopes().size()==1) 
	{
		denominator_matrix_isotope_p = *single_element_matrix_isotopes().begin();
		return denominator_matrix_isotope_p;
	}
	/*no 100at% matrix in any known sample*/
	counter_matrix_isotope_p = matrix_isotopes_or_elements_with_LEAST_relative_concentration_in_ref_sample();
	set<isotope_t> matrix_isotopes = measurement_group_priv.reference_matrix_isotopes();
	vector<isotope_t> matrix_isotopes_vec (matrix_isotopes.begin(),matrix_isotopes.end());
	if (matrix_isotopes_vec[0]==counter_matrix_isotope_p) denominator_matrix_isotope_p = matrix_isotopes_vec[1];
	else denominator_matrix_isotope_p = matrix_isotopes_vec[0];
	denominator_matrix_isotope_p.atoms=1;
	calc_history.push_back("jiang_t::denominator_matrix_isotope()\t" + denominator_matrix_isotope_p.name());
	return denominator_matrix_isotope_p;
}


isotope_t calc_models_t::jiang_t::matrix_isotopes_or_elements_with_LEAST_relative_concentration_in_ref_sample()
{
// 	if (matrix_isotopes_or_elements_with_LEAST_relative_concentration_in_ref_sample_p.size()>0) return &matrix_isotopes_or_elements_with_LEAST_relative_concentration_in_ref_sample_p;
	isotope_t matrix_isotopes_or_elements_with_LEAST_relative_concentration_in_ref_sample_p;
	double least_relative_atoms;
	for (auto& measurement:measurement_group_priv.measurements)
	{
		if (!measurement->load_from_database()) continue;
// 		if (measurement->sample_p->matrix.isotopes.size()!=2 && measurement->sample_p->matrix.isotopes.size()!=1) continue;
		for (auto& matrix_isotope:measurement->sample_p->matrix.isotopes)
		{
			if (!measurement->sample_p->matrix.relative_concentration(matrix_isotope).is_set()) continue;
			if (!matrix_isotopes_or_elements_with_LEAST_relative_concentration_in_ref_sample_p.is_set() || least_relative_atoms > measurement->sample_p->matrix.relative_concentration(matrix_isotope).data[0] )
			{
				least_relative_atoms = measurement->sample_p->matrix.relative_concentration(matrix_isotope).data[0] ;
				matrix_isotopes_or_elements_with_LEAST_relative_concentration_in_ref_sample_p = matrix_isotope;
			}
		}
	}
	return matrix_isotopes_or_elements_with_LEAST_relative_concentration_in_ref_sample_p;
}

set<isotope_t> calc_models_t::jiang_t::single_element_matrix_isotopes()
{
	if (single_element_matrix_isotopes_p.size()>0) return single_element_matrix_isotopes_p;
	for (auto measurement:measurement_group_priv.measurements)
	{
		if (!measurement->load_from_database()) continue;
		if (measurement->sample_p->matrix.isotopes.size()==1) 
		{
			isotope_t is = measurement->sample_p->matrix.isotopes[0];
			is.atoms = 1; // will overwrite identical isotopes from other samples in other measurements within the MG
			single_element_matrix_isotopes_p.insert(is);
		}
	}
	return single_element_matrix_isotopes_p;
}














