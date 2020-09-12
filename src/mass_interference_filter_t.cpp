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
#include "mass_interference_filter_t.hpp"
#include "globalvars.hpp"

void mass_interference_filter_t::substract_Ge_from_As(cluster_t* As_cluster)
{
	isotope_t As;
	As.symbol="As";
	As.nucleons=75;
	//no As cluster? -> do nothing
	if (!As_cluster->is_element_in_cluster(As)) return;
	vector<isotope_t> matrix_isotopes = As_cluster->measurement->measurement_group->reference_matrix_isotopes_vec();
	cluster_t* Ge_matrix_cluster=nullptr;
	for (isotope_t matrix_isotope:matrix_isotopes)
	{
		if (matrix_isotope.symbol=="Ge")
		{
			Ge_matrix_cluster = As_cluster->measurement->reference_cluster(matrix_isotope);
			break;
		}
	}
	// no Ge cluster
	if (Ge_matrix_cluster == nullptr) return;
	
	if (Ge_matrix_cluster->intensity().is_set() && As_cluster->concentration().is_set() && As_cluster->concentration_p.data.size()==Ge_matrix_cluster->intensity().data.size())
	{
// 		cout << "name=\t" << Ge_matrix_cluster->measurement->filename_p->filename_with_path() << endl;
		int Ge_max_pos = statistics::get_max_index_from_Y(Ge_matrix_cluster->intensity().data);
// 		cout << "Ge_max_pos="<<Ge_max_pos<<endl;
		double max_normalization_factor = As_cluster->concentration_p.data[Ge_max_pos] / Ge_matrix_cluster->intensity().data[Ge_max_pos];
// 		cout << "max_normalization_factor=" << max_normalization_factor << endl;
		// changing cluster data itself! private member! i think this is more efficient than copy and paste the cluster again in the aftermath
		quantity_t difference = Ge_matrix_cluster->intensity()*max_normalization_factor;
		difference.unit = As_cluster->concentration().unit;
		difference.dimension = As_cluster->concentration().dimension;
		difference.name = As_cluster->concentration().name;
// 		difference.to_screen();
// 		cout << "difference.data[Ge_max_pos]=" << difference.data[Ge_max_pos] << endl;
// 		cout << "As_cluster->concentration_p.size()="<<As_cluster->concentration_p.data.size()<<endl;
// 		cout << "difference.size()="<<difference.data.size()<<endl;
// 		cout << "As_cluster->concentration_p.data[Ge_max_pos]=" << As_cluster->concentration_p.data[Ge_max_pos] << endl;
		As_cluster->concentration_p=As_cluster->concentration_p-difference;
		
		for (auto& d : As_cluster->concentration_p.data)
			if (d<=0) d=1E-10;		
	}
}
