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
#include "sample_t.hpp"
#include "globalvars.hpp"
#define TABLENAME_everything "everything"
#define TABLENAME_samples "samples"
#define TABLENAME_sample_implants "sample_implants"
#define TABLENAME_sample_layers "sample_layers"
 
 
 std::vector< isotope_t > sample_t::matrix_t::isotopes() const
{
	return isotopes_p;
}

 
 int sample_t::matrix_t::get_atoms_from_isotope ( isotope_t isotope )
{
	for (auto& is:isotopes())
	{
		if (isotope.symbol==is.symbol && isotope.nucleons == is.nucleons)
			return is.atoms;
	}
	return 0;
}

 
 quantity_t sample_t::matrix_t::relative_concentration(const isotope_t& isotope) const
{
// 	cout << isotope.nucleons << isotope.symbol << isotope.atoms << endl;
	quantity_t Crel;
	Crel.name = "relative concentration";
	Crel.unit = "at%";
	Crel.dimension = "relative";
	double sum=0;
	for (auto is:isotopes())
	{
		sum = sum +is.atoms;
// 		is.to_screen("is\t");
	}
	if (sum==0) return quantity_t();

	double atoms=0;
	for (auto& is:isotopes())
		if (isotope.symbol==is.symbol && isotope.nucleons == is.nucleons)
		{
			atoms = is.atoms;
			break;
		}
	
	Crel.data.push_back(atoms/sum*100);
// 	Crel.to_screen();
	return Crel;
}

 
 
bool sample_t::matrix_t::set_isotopes_from_name(string name)
{
	name_p = name;
	vector<string> isotope_strings;
// 	if (name_p.find(",")!=string::npos) isotope_strings = tools::str::get_strings_between_delimiter(name_p,",");
	isotope_strings = tools::str::get_strings_between_delimiter(name_p," ");
	for (auto& isotope_string:isotope_strings)
	{
		isotopes_p.push_back(isotope_t(isotope_string));
	}
	if (isotopes().size()==0) return false;
	return true;
}


// vector<std::__cxx11::string> sample_t::matrix_t::element_symbols()
// {
// 	if (element_symbols_p.size()>0) return element_symbols_p;
// 	string rest = name_p;
// 	for (auto& element:PSE.get_all_elements())
// 	{
// 		if (rest.find(element)) 
// 		{
// 			element_symbols_p.push_back(element);
// 			tools::str::remove_substring_from_mainstring(&rest,element);
// 		}
// 	}
// 	/*check if there are other symbols, which are not in PSE -> return {}*/
// 	tools::str::remove_spaces(&rest);
// 	rest=tools::str::remove_linebreakers_from_string(rest);
// 	if (!tools::str::is_number(rest)) return {};
// 	/***********/
// 	return element_symbols_p;
// }


std::__cxx11::string sample_t::matrix_t::name()
{
	if (isotopes().size()==0) return "";
	string result;
	for(auto& isotope: isotopes())
	{
		if (isotope.nucleons!=-1) result+=isotope.nucleons;
		result+=isotope.symbol;
		if (isotope.atoms!=-1) result+=isotope.nucleons;
	}
	result.erase(result.end()-1,result.end());
	return result;
}



bool sample_t::matrix_t::operator==(const matrix_t& matrix2)
{
	if (isotopes().size()==0 || matrix2.isotopes().size()==0) return false;
	bool found=false;
	for (auto& matrix_isotope:isotopes())
	{
// 		matrix_isotope.to_screen("matrix_isotope\t");
		found = false;
		for (auto& matrix_isotope2:matrix2.isotopes())
		{
			
// 			matrix_isotope2.to_screen("matrix_isotope2\t");
			if (matrix_isotope == matrix_isotope2 && relative_concentration(matrix_isotope) == matrix2.relative_concentration(matrix_isotope2))
			{
				found = true;
				break;
			}
// 			///TODO: relative_concentration comparison and NOT absolute!
// 			if (matrix_isotope==matrix_isotope2 && matrix_isotope.atoms == matrix_isotope2.atoms)
// 			{
// 				found = true;
// 			} 
// 			else found = false;
		}
		if (!found) return false;
	}
	return true;
}

bool sample_t::matrix_t::operator!=(const matrix_t& matrix)
{
	return !operator==(matrix);
}

bool sample_t::matrix_t::operator<(const matrix_t& matrix) const
{
	for (auto& isotope:isotopes())
	{
		if (relative_concentration(isotope)<matrix.relative_concentration(isotope)) return true;
		if (relative_concentration(isotope)!=matrix.relative_concentration(isotope)) return false;
	}
	return false;
}

/*******************************/





std::__cxx11::string sample_t::name()
{
	std::stringstream name;
	name << lot() << lot_split() << "_w" << wafer() << "_c" << chip_x() <<"-"<< chip_y() << "_" <<  monitor();
	return name.str();
}

 
sample_t::sample_t(filename_t* filename_p)
{
	wafer_p = filename_p->wafer;
	lot_p = filename_p->lot;
	lot_split_p = filename_p->lot_split;
	monitor_p = filename_p->monitor;
	chip_x_p = filename_p->chip_x;
	chip_y_p = filename_p->chip_y;
}

list<sample_t> sample_t::samples(list<filename_t> *filenames)
{
	bool added;
	list<sample_t> samples;
	for (auto& filename:*filenames)
	{
		added=false;
		sample_t sample_p(&filename);
		for (auto& sample:samples)
		{
			if(sample == sample_p)
			{
				added=true;
				break;
			}
		}
		if (!added) samples.push_back(sample_p);
	}
	return samples;
}

void sample_t::to_screen(std::__cxx11::string prefix)
{
	cout << prefix << "lot = " << lot() << endl;
	cout << prefix << "lot_split = " << lot_split() << endl;
	cout << prefix << "wafer = " << wafer() << endl;
	cout << prefix << "monitor = " << monitor() << endl;
	cout << prefix << "chip_x = " << chip_x() << endl;
	cout << prefix << "chip_y = " << chip_y() << endl;
	cout << prefix << "substrate = " << matrix.name() << endl;
	for (auto& dose:doses_p)
	{
		cout << "\t" << dose.first.nucleons << dose.first.symbol << endl;
		dose.second.to_screen(prefix+"\t");
	}
}

std::__cxx11::string sample_t::lot()
{
	return lot_p;
}

std::__cxx11::string sample_t::lot_split()
{
	return lot_split_p;
}

int sample_t::chip_x()
{
	return chip_x_p;
}

int sample_t::chip_y()
{
	return chip_y_p;
}

std::__cxx11::string sample_t::monitor()
{
	return monitor_p;
}

int sample_t::wafer()
{
	return wafer_p;
}

/*DATABASE*/


bool sample_t::load_from_database ( )
{
	std::string sql;

	sql = "SELECT * FROM " \
			TABLENAME_everything \
			" WHERE " \
			"lot='" +lot()+ "' AND " \
			"wafer=" +to_string(wafer())+ " AND " \
			"chip_x=" +to_string(chip_x())+ " AND " \
			"chip_y=" +to_string(chip_y())+ " AND " \
			"monitor='" +monitor()+ "';";

	map<string,vector<string>> lines_map;
	if (!db.execute_sql(sql,database_t::callback_lines_map,&lines_map)) 
	{
		return false;
	}
	if (lines_map.size()==0) return false;
	quantity_t implanted_dose;
	implanted_dose.name="dose";
	implanted_dose.unit="at/cm^2";
	implanted_dose.dimension = "amount*(length)^(-2)";
	implanted_dose.data.resize(1);
	vector<string>* implant_doses=&lines_map["implant_dose"];
	vector<string>* implant_isotopes=&lines_map["implant_isotope"];
	if (!matrix.set_isotopes_from_name(lines_map["matrix_elements"][0]))
	{
		cout << "sample_t::load_from_database: !matrix.set(" << lines_map["matrix_elements"][0] << ")" << endl;
	}
	for (int i=0;i<implant_doses->size();i++)
	{
		isotope_t isotope(implant_isotopes->at(i));
		if (!isotope.is_set()) 
		{
			cout << "sample_t::load_from_database: !isotope.is_set(): " << implant_isotopes->at(i) << endl;
			continue;
		}
		implanted_dose.data[0]=tools::str::str_to_double(implant_doses->at(i));
		doses_p[isotope]=implanted_dose;
	}
    return true;
}

// bool sample_t::save_to_database ( )
// {
// 	 std::string sql;
//  
// 	sql = "INSERT INTO " \
// 			TABLENAME_everything \
// 			" (lot, lot_split, wafer, chip_x, chip_y, monitor) " \
// 			" VALUES " \
// 			"(" \
// 			"'" +lot()+ "', " \
// 			"'" +lot_split()+ "', " \
// 			"" +to_string(wafer())+ ", " \
// 			"" +to_string(chip_x())+ ", " \
// 			"" +to_string(chip_y())+ ", " \
// 			"'" +monitor()+ "');";
//     return db.execute_sql(sql);
// }


/*OPERATORS*/

bool sample_t::operator == (sample_t& sample)
{
	if (lot()!=sample.lot()) return false;
	if (wafer()!=sample.wafer()) return false;
	if (chip_x()!=sample.chip_x()) return false;
	if (chip_y()!=sample.chip_y()) return false;
	if (monitor()!=sample.monitor()) return false;
	return true;
}


bool sample_t::operator != (sample_t& sample)
{
	return !operator==(sample);
}

bool sample_t::operator < (sample_t& sample)
{
	if (lot()>sample.lot()) return false;
	if (lot()==sample.lot()) 
	{
		if (wafer()>sample.wafer()) return false;
	}
	return true;
}


