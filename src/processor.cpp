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
#include "processor.hpp"
#include "config.hpp"

// pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
vector<string> calc_history;


bool processor::use_jiang=true;
bool processor::print_errors=false;
bool processor::use_mass_interference_filter=false;
bool processor::force_RSF_to_foreign_matrix=false;


processor::processor(vector<string> arg_list) 
{
	config_t conf;
	string intro;
	#ifdef __unix__
	intro = "\nCLAUS Copyright (C) 2020  Florian Bärwolf\n" \
			"This program comes with ABSOLUTELY NO WARRANTY;\n" \
			"This is free software, and you are welcome to redistribute it\n" \
			"under certain conditions; See GPLv3\n";
	#else
	intro = "\nCLAUS Copyright (C) 2020  Florian Baerwolf\n" \
			"This program comes with ABSOLUTELY NO WARRANTY;\n" \
			"This is free software, and you are welcome to redistribute it\n" \
			"under certain conditions; See GPLv3\n";
	#endif
	cout << intro << endl;
	cout << "Version 2020-08-02_9	BETA" << endl;
	cout << "see https://github.com/flosious" << endl << endl;
	cout << "use the same clusters in each measurement within one common measurement group" << endl;
	cout << "cluster names must be unique for each isotope ((e.g. just \"11B 28Si\") for \"11B\") (bijective transformation)" << endl;
	
    // save args
    args = arg_list;
// 	cout << "input files:" << endl;
// 	print(args);
    // load configs
    if (conf.load(args)==0) cout << "processor::processor conf.load(args)==0" << endl;
	// PSE periodic table of elements (isotopes, masses and natural abundance)
	if (PSE.load()==0) cout << "processor::processor !PSE.load()" << endl;
	if (!db.open()) cout << "processor::processor !db.open()" << endl;
    // filter files 
    if (!files.get(args)) cout << "processor::processor !files.get(args)" << endl;
	cout << "input files: " << files.vec().size() << endl;
	/// parse the filenames
	filenames = filename_t::parse(files.vec(),"_");
	/// parse the measurements and samples
	measurements = measurement_tools.measurements(&filenames,&samples);
	if (measurements.size()==0) 
	{
		cout << "!!!\tno measurement could be parsed\t!!!" << endl;
		if (print_errors)
		{
			print("tools.get_error_messages():");
			if (measurement_tools.get_error_messages().size()!=0) print(measurement_tools.get_error_messages());
			else print("\tno errors");
		}
		#ifdef __unix__
		#else
		system("pause");
		#endif
		return;
	}
	/// measurement_groups
	measurement_groups = measurement_group_t::measurement_groups(&measurements);

	/* shake it baby! */
	for (auto& MG:measurement_groups)
	{
		calc_models_t::jiang_t jiang(MG);
		cout <<  "measurement_group: " << MG.name() << endl;
		cout << "{" << endl;
		
		if (use_jiang)
		{
			cout << "\ttrying to apply Jiangs protocol ...";
			if (jiang.calc())
				cout << "SUCCESS!" <<endl;
			else
				cout << "failed" << endl;
		}
		
		if (force_RSF_to_foreign_matrix)
		{
			cout << endl;
			cout << "!!!\t***************!ATTENTION!****************" << endl;
			cout << "!!!\t**  enforcing RSFs to foreign matrices  **" << endl;
			cout << "!!!\t******************************************" << endl;
			cout << endl;
			for (auto& M:MG.measurements)
			{
				for (auto& C:M->clusters)
				{
					if (C.second.is_reference()) continue;
					if (C.second.RSF().is_set())
					{
						for (auto& M2:MG.measurements)
						{
							if (M==M2) continue;
							for (auto& C2:M2->clusters)
							{
								if (C.second.name()==C2.second.name() && !C2.second.RSF().is_set())
								{
									C2.second.RSF_p = C.second.RSF();
									C2.second.already_checked_concentration=false;
									break;
								}
							}
						}
					}
				}
			}
		}
		
		if (use_mass_interference_filter)
		{
			cout << "\ttrying to apply: mass interference filters" << endl;
			for (auto& M:MG.measurements)
				for (auto& C:M->clusters)
					mass_interference_filter_t::substract_Ge_from_As(&C.second);
		}
		
		origin_t::export_to_files(MG);
		
		if (!jiang.is_error())
		{
			cout << "\ttrying to export: jiang parameters ...";
			origin_t::export_jiang_parameters_to_file(jiang);
			cout << "SUCCESS!" << endl;
		}
		
		cout << "}" << endl;
	}
	
	if (print_errors)
	{
		print("tools.get_error_messages():");
		if (measurement_tools.get_error_messages().size()!=0) print(measurement_tools.get_error_messages());
		else print("\tno errors");
	}
	// TESTS
	for (auto& MG:measurement_groups)
		for (auto& M:MG.measurements)
			M->crater.global_sputter_time().to_screen();
	
	#ifdef __unix__
    #else
	system("pause");
	#endif
	
}


processor::~processor() 
{
    cout << "processor finished\n";
}
