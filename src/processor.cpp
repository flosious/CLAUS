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
#include "processor.hpp"


// pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
vector<string> calc_history;

processor::processor(vector<string> arg_list) 
{
	string intro;
	#ifdef __unix__
	intro = "\n\nCLAUS Copyright (C) 2020  Florian Bärwolf\n" \
			"This program comes with ABSOLUTELY NO WARRANTY;\n" \
			"This is free software, and you are welcome to redistribute it\n" \
			"under certain conditions; See GPLv3\n\n";
	#else
	intro = "\n\nCLAUS Copyright (C) 2020  Florian Baerwolf\n" \
			"This program comes with ABSOLUTELY NO WARRANTY;\n" \
			"This is free software, and you are welcome to redistribute it\n" \
			"under certain conditions; See GPLv3\n\n";
	#endif
	cout << intro;
// 	cout << endl<<"CLAUS Copyright (C) 2020  Florian Bärwolf" << endl;
// 	cout << "This program comes with ABSOLUTELY NO WARRANTY;" << endl;
// 	cout << "This is free software, and you are welcome to redistribute it" << endl;
// 	cout << "under certain conditions; See GPLv3" << endl << endl;
	
    // save args
    args = arg_list;
	cout << "input files:" << endl;
	print(args);
    // load configs
    if (conf.load(args)==0) cout << "processor::processor conf.load(args)==0" << endl;
	// PSE periodic table of elements (isotopes, masses and natural abundance)
	if (PSE.load()==0) cout << "processor::processor !PSE.load()" << endl;
	if (!db.open()) cout << "processor::processor !db.open()" << endl;
    // filter files 
    if (!files.get(args)) cout << "processor::processor !files.get(args)" << endl;
	/// parse the filenames
	filenames = filename_t::parse(files.vec(),"_");
	/// parse the measurements and samples
	measurements = measurement_tools.measurements(&filenames,&samples);
	/// measurement_groups
	measurement_groups = measurement_group_t::measurement_groups(&measurements);
	/// managing calculation
// 	calc_manager_t calc_manager(measurement_groups);
	
	
	
	for (auto& MG:measurement_groups)
	{
		calc_models_t::jiang_t jiang(MG);
		if (jiang.calc())
		{
			origin_t::export_to_files(jiang.measurement_group());
			origin_t::export_jiang_parameters_to_file(jiang);
		}
		else
		{
			origin_t::export_to_files(MG);
			origin_t::export_MG_parameters_to_file(MG);
		}
		export2_t::export_contents_to_file(calc_history,"detailed_calculation_history.txt",MG,conf.calc_location);
	}
	
// 	print("calculation history:");
// 	print(calc_history);

	
	
	if (conf.print_errors)
	{
		print("tools.get_error_messages():");
		if (measurement_tools.get_error_messages().size()!=0) print(measurement_tools.get_error_messages());
		else print("\tno errors");
	}
	
	#ifdef __unix__
// 	if (measurements.size()<50) plot_t::export_to_files(&measurements);
    #else
	system("pause");
	#endif
	
}

processor::~processor() 
{
    cout << "processor finished\n";
}
