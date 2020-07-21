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

#include "database_t.hpp"

#define TABLENAME_everything "everything"
#define TABLENAME_samples "samples"
#define TABLENAME_sample_implants "sample_implants"
#define TABLENAME_sample_layers "sample_layers"
#define TABLENAME_reference_measurement_isotopes "reference_measurement_isotopes"
#define TABLENAME_measurements "measurements"
#define TABLENAME_measurement_settings "measurement_settings"
#define TABLENAME_measurement_statistics "measurement_statistics"


/// global variable
database_t db;


database_t::database_t() 
{ 
	return;
} 

bool database_t::open() {
    int exit = 0; 

    exit = sqlite3_open(conf.db_location.c_str(), &this->DB);
  
    if (exit)      return false;  
	
	if (!create_tables()) return false;
	execute_sql("PRAGMA secure_delete = true");
    return true; 
}

bool database_t::create_tables() 
{
   
	if (!create_table_everything()) error_messages.push_back("database_t::database_t: !create_table_everything()");
	if (!create_table_samples()) error_messages.push_back("database_t::database_t: !create_table_samples()");
// 		if (!create_table_sample_implants()) error_messages.push_back("database_t::database_t: !create_table_sample_implants()");
// 		if (!create_table_sample_layers()) error_messages.push_back("database_t::database_t: !create_table_sample_layers()");
	if (!create_table_measurements()) error_messages.push_back("database_t::database_t: !create_table_measurements()");
// 		if (!create_table_measurement_settings()) error_messages.push_back("database_t::database_t: !create_table_measurement_settings()");
// 		if (!create_table_reference_measurement_isotopes()) error_messages.push_back("database_t::database_t: !create_table_reference_measurement_isotopes()");
// 		if (!create_table_measurement_statistics()) error_messages.push_back("database_t::database_t: !create_table_measurement_statistics()");

		
	return true;
}

/***********	TABLE DEFINITIONS	******************/
//TODO INT UND TEXT eins von beiden muss noch in '' gesetzt werden
bool database_t::create_table_everything()
{
    std::string sql;
    sql = "CREATE TABLE IF NOT EXISTS " + string(TABLENAME_everything) + "(" \
         "id INTEGER PRIMARY KEY, " \
         "lot                   	 TEXT, " \
         "wafer                  	 INT, " \
         "lot_split            	 TEXT, " \
         "chip_x                  	 INT, " \
         "chip_y                  	 INT, " \
         "monitor                	 TEXT, " \
         
         "matrix_elements      	 TEXT, " \
         // 31P1
         "implant_isotope          	 TEXT, " \
         // 2E15 in at/scm
         "dose	           	 INT, " \
         "maximum_concentration                 INT, " \
         "depth_at_maximum_concentration	INT, " \
         
         "tool_name                	 TEXT, " \
         
         /*O2+, Cs+*/
         "sputter_element            TEXT, " \
         "sputter_energy           	 INT, " \
         /* +/- */
		 "polarity         			 TEXT, " \
		 "olcdb						 INT, " \
		 "reference_calculation_method					 TEXT, " \
         "filename_with_path         TEXT, " \
		 "comments         			 TEXT);";
    return execute_sql(sql);
}

bool database_t::create_table_samples()
{
    std::string sql;
    sql = "CREATE TABLE IF NOT EXISTS " + string(TABLENAME_samples) + "(" \
         "sample_id INTEGER PRIMARY KEY, " \
         "lot                   	 TEXT, " \
         "wafer                  	 INT, " \
         "lot_split            	     TEXT, " \
         "chip_x                  	 INT, " \
         "chip_y                  	 INT, " \
         "monitor                	 TEXT, " \
		 "substrat                   TEXT, " \
		 "susbtrat_thickness      	 INT, " \
         "isotope                  	 TEXT, " \
         // 2E15 in at/scm
         "dose	                	 INT);";
    return execute_sql(sql);
}

bool database_t::create_table_measurements()
{
    std::string sql;
    sql = "CREATE TABLE IF NOT EXISTS " + string(TABLENAME_measurements) + "(" \
         "measurement_id INTEGER PRIMARY KEY, " \
         "sample_id					 INT, " \
         "tool_name                  TEXT, " \
         /* +/- */
         "polarity           		TEXT, " \
         /*O2+, Cs+*/
         "sputter_element            TEXT, " \
         "sputter_energy           	 INT);";
         
         "filename_with_path         TEXT);";
    return execute_sql(sql);
}

/*********BELOW NOT IN USE***********/ 

// bool database_t::create_table_sample_implants()
// {
//     std::string sql;
//     sql = "CREATE TABLE IF NOT EXISTS " + string(TABLENAME_sample_implants) + "(" \
//          "implant_id INTEGER PRIMARY KEY, " \
//          "sample_id                  INT, " \
//          // 31P1
//          "isotope                  	 TEXT, " \
//          // 2E15 in at/scm
//          "dose	                	 INT);";
//     return execute_sql(sql);
// }
// 
// bool database_t::create_table_sample_layers()
// {
//     std::string sql;
//     sql = "CREATE TABLE IF NOT EXISTS " + string(TABLENAME_sample_layers) + "(" \
//          "layer_id INTEGER PRIMARY KEY, " \
//          "sample_id                  INT, " \
//          /// 0 is first layer (wafer substrat), 1 is on top of 0 and so on ...
//          "layer_index				 INT, " \
//          // Si or SiGe30
//          "element_or_isotope      	 TEXT, " \
//          // in nm
//          "thickness                	 FLOAT);";
//     return execute_sql(sql);
// }

// bool database_t::create_table_measurement_settings() 
// {
//     std::string sql;
//     sql = "CREATE TABLE IF NOT EXISTS " + string(TABLENAME_measurement_settings) + "(" \
//          "settings_id INTEGER PRIMARY KEY, " \
//          "tool_name                   	 TEXT, " \
//          /* +/- */
//          "polarity           TEXT, " \
//          /*O2+, Cs+*/
//          "sputter_element            TEXT, " \
//          "sputter_energy           	 INT);";
//     return execute_sql(sql);
// }

// bool database_t::create_table_measurements()
// {
//     std::string sql;
//     sql = "CREATE TABLE IF NOT EXISTS " + string(TABLENAME_measurements) + "(" \
//          "measurement_id INTEGER PRIMARY KEY, " \
//          "sample_id					 INT, " \
//          "settings_id				 INT, " \
//          "filename_with_path         TEXT);";
//     return execute_sql(sql);
// }

bool database_t::create_table_reference_measurement_isotopes()
{
    std::string sql;
    sql = "CREATE TABLE IF NOT EXISTS " + string(TABLENAME_reference_measurement_isotopes) + "(" \
         "isotope_id INTEGER PRIMARY KEY, " \
         "maximum_concentration                 	 INT, " \
         "depth_at_maximum_concentration                  	 INT, " \
         "isotope                	 TEXT);";
    return execute_sql(sql);
}

// bool database_t::create_table_measurement_statistics()
// {
//     std::string sql;
//     sql = "CREATE TABLE IF NOT EXISTS " + string(TABLENAME_measurement_statistics) + "(" \
//          "isotope_id INTEGER PRIMARY KEY, " \
//          "maximum_concentration                 	 INT, " \
//          "depth_at_maximum_concentration                  	 INT, " \
//          "isotope                	 TEXT);";
//     return execute_sql(sql);
// }

/***********	TABLE DEFINITIONS ENDE	******************/

vector<string> database_t::get_error_messages()
{
	return error_messages;
}

database_t::~database_t() 
{ 
    close();
//     cout << "DB closed\n";
} 



bool database_t::close() {
    sqlite3_close(this->DB); 
    return true; 
}

bool database_t::execute_sql(std::__cxx11::string sql, int (*func_ptr)(void*,int,char**,char**), void* func_arg)
{
    char *zErrMsg = 0;
	int rc = sqlite3_exec(this->DB, sql.c_str(), func_ptr, func_arg, &zErrMsg);
	if( rc != SQLITE_OK ){
		error_messages.push_back(string("sql-command: ") + sql);
		error_messages.push_back(string("database_t::execute_sql() SQL error: ") + zErrMsg);
		std::cout << "execute_sql() SQL error: "<< zErrMsg << std::endl;
		std::cout << "sql-command: " << sql << "\n";
		sqlite3_free(zErrMsg);
		return false;
	}
	return true;
}

// int database_t::callback_lines_cols(void *ptr, int argc, char **argv, char **azColName)
// {
//       vector<vector<string>> *lines = static_cast<vector<vector<string>>*>(ptr);
//       vector<string> columns;
// 	  if (lines->size()==0) 
// 	  {
// 		//header//
// 		for(int i=0; i<argc; i++)
// 		{
// 			columns.push_back(azColName[i]); 
// 		}
// 		lines->push_back(columns);
// 	  }
// 	  //data//
// 	  columns.clear();
//       for(int i=0; i<argc; i++)
// 	  {
// 		if (argv[i]) columns.push_back(argv[i]); 
// 		else columns.push_back("NULL");
//       }
//       lines->push_back(columns);
//       return 0;
// }

int database_t::callback_lines_map(void *ptr, int argc, char **argv, char **azColName)
{
      map<string,vector<string>> *lines_map = static_cast<map<string,vector<string>>*>(ptr);
      for(int i=0; i<argc; i++)
	  {
		if (argv[i]) (*lines_map)[azColName[i]].push_back(argv[i]); 
		else (*lines_map)[azColName[i]].push_back("NULL"); 
      }
      return 0;
}

// // integrated in create_table_measurement_polyfit_statistics
// // bool database::create_table_measurement_polyfit_parameters() {
// //     std::string sql;
// //     sql = "CREATE TABLE IF NOT EXISTS " + string(TABLENAME_measurement_polyfit_parameters) + "(" \
// //          "ID INTEGER PRIMARY KEY     AUTOINCREMENT, " \
// //          "file_name                	 TEXT, " \
// //          "isotope                  	 TEXT, " \
// //          "absolute_time_start        TIMESTAMP, " \
// //          "impact_energy           	 INT, " \
// //          "sputter_time           	 DOUBLE, " \
// //          "surface_time             	 DOUBLE, " \
// //          "crater_depth             	 DOUBLE, " \
// //          "a0                      	 DOUBLE, " \
// //          "a1                      	 DOUBLE, " \
// //          "a2                      	 DOUBLE, " \
// //          "a3                      	 DOUBLE, " \
// //          "a4                      	 DOUBLE, " \
// //          "a5                     	 DOUBLE, " \
// //          "a6                         DOUBLE, " \
// //          "a7                       	 DOUBLE, " \
// //          "a8                      	 DOUBLE, " \
// //          "a9                      	 DOUBLE, " \
// //          "a10                      	 DOUBLE, " \
// //          "a11                      	 DOUBLE, " \
// //          "a12                      	 DOUBLE, " \
// //          "a13                      	 DOUBLE, " \
// //          "a14                      	 DOUBLE);";
// //     execute_sql(sql);
// //     return true;
// // }
// 
// // bool database_t::create_table_measurement_polyfit_statistics() {
// //     std::string sql;
// //     sql = "CREATE TABLE IF NOT EXISTS " + string(TABLENAME_measurement_polyfit_statistics) + "(" \
// //          "ID INTEGER PRIMARY KEY     AUTOINCREMENT, " \
// //          "measurement_file_name      TEXT, " \
// //          "isotope                  	 TEXT, " \
// //          "absolute_time_start        TIMESTAMP, " \
// //          "impact_energy           	 INT, " \
// //          /*positive, negative*/
// //          "measurement_mode           TEXT, " \
// //          /*O2+, Cs+*/
// //          "sputter_element            TEXT, " \
// //          "sputter_time           	 DOUBLE, " \
// //          "surface_time           	 DOUBLE, " \
// //          "crater_depth             	 DOUBLE, " \
// //          "area                   	 DOUBLE, " \
// //          "max                      	 DOUBLE, " \
// //          "max_x                      DOUBLE, " \
// //          "min                      	 DOUBLE, " \
// //          "min_x                      DOUBLE, " \
// //          "mean                     	 DOUBLE, " \
// //          "mean_trimmed          	 DOUBLE, " \
// //          "auto_corr              	 DOUBLE, " \
// //          "sd                         DOUBLE, " \
// //          "median                     DOUBLE, " \
// //          "mad                      	 DOUBLE, " \
// //          "abs_dev                  	 DOUBLE, " \
// //          "quantile                   DOUBLE, " \
// //          "skew                       DOUBLE, " \
// //          "curtosis                   DOUBLE, " \
// //          /*fit parameters*/
// //          "chisq_abs                  DOUBLE, " \
// //          "chisq_rel                  DOUBLE, " \
// //          "a0                      	 DOUBLE, " \
// //          "a1                      	 DOUBLE, " \
// //          "a2                      	 DOUBLE, " \
// //          "a3                      	 DOUBLE, " \
// //          "a4                      	 DOUBLE, " \
// //          "a5                     	 DOUBLE, " \
// //          "a6                         DOUBLE, " \
// //          "a7                       	 DOUBLE, " \
// //          "a8                      	 DOUBLE, " \
// //          "a9                      	 DOUBLE, " \
// //          "a10                      	 DOUBLE, " \
// //          "a11                      	 DOUBLE, " \
// //          "a12                      	 DOUBLE, " \
// //          "a13                      	 DOUBLE, " \
// //          "a14                      	 DOUBLE);";
// //     execute_sql(sql);
// //     return true;
// // }
// 
// // bool database_t::create_table_measurement_interpolated_statistics() {
// //     std::string sql;
// //     sql = "CREATE TABLE IF NOT EXISTS " + string(TABLENAME_measurement_interpolated_statistics) + "(" \
// //          "ID INTEGER PRIMARY KEY     AUTOINCREMENT, " \
// //          "measurement_file_name    	 TEXT, " \
// //          "isotope                  	 TEXT, " \
// //          "absolute_time_start        TIMESTAMP, " \
// //          "impact_energy           	 INT, " \
// //          /*positive, negative*/
// //          "measurement_mode           TEXT, " \
// //          /*O2+, Cs+*/
// //          "sputter_element            TEXT, " \
// //          "sputter_time           	 DOUBLE, " \
// //          "surface_time           	 DOUBLE, " \
// //          "crater_depth             	 DOUBLE, " \
// //          "area                   	 DOUBLE, " \
// //          "max                      	 DOUBLE, " \
// //          "max_x                      DOUBLE, " \
// //          "min                      	 DOUBLE, " \
// //          "min_x                      DOUBLE, " \
// //          "mean                     	 DOUBLE, " \
// //          "mean_trimmed          	 DOUBLE, " \
// //          "auto_corr              	 DOUBLE, " \
// //          "sd                         DOUBLE, " \
// //          "median                     DOUBLE, " \
// //          "mad                      	 DOUBLE, " \
// //          "abs_dev                  	 DOUBLE, " \
// //          "quantile                   DOUBLE, " \
// //          "skew                       DOUBLE, " \
// //          "curtosis                   DOUBLE);";
// //     execute_sql(sql);
// //     return true;
// // }
// 

// 
// 
// /*contains all properties of the measurement*/
// // Wozu?
// // bool database_t::create_table_measurements() {
// //     std::string sql;
// //     sql = "CREATE TABLE IF NOT EXISTS " + string(TABLENAME_measurements) + "(" \
// //          "ID INTEGER PRIMARY KEY     AUTOINCREMENT, " \
// //          "measurement_file_name    	 TEXT, " \
// //          "olcdb                   	 INT, " \
// //          "lot                   	 TEXT, " \
// //          "wafer                  	 INT, " \
// //          "comment                	 TEXT, " \
// //          "tool                   	 TEXT, " \
// //          "date_start               	 DATETIME, " \
// //          "absolute_time_start        TIMESTAMP, " \
// //          /*cluster: 11B, 30Si, 28Si 75As, ... geordnet nach masse*/
// //          "clusters                 	 TEXT, " \
// //          "sputter_time           	 DOUBLE, " \
// //          "surface_time           	 DOUBLE, " \
// //          "sputtercurrent_at_start    DOUBLE, " \
// //          "sputtercurrent_at_stop   	 DOUBLE, " \
// //          "crater_depth           	 DOUBLE, " \
// //          "impact_energy           	 INT, " \
// //          /*positive, negative*/
// //          "measurement_mode           TEXT, " \
// //          /*O2+, Cs+*/
// //          "sputter_element            TEXT, " \
// //          "settings_name           	 TEXT);";
// //     execute_sql(sql);
// //     return true;
// // }
// 
// void database_t::add_measurements(vector<measurement_t>* measurements)
// {
// 	for (int i=0;i<measurements->size();i++)
// 		if (!add_measurement(&measurements->at(i))) error_messages.push_back("database_t::add_measurements : " + measurements->at(i).id_get().filename_wo_crater_depths);
// 	return;
// }
// 
// // map<string, vector<string>> database_t::get_line_map(string table, int rowid)
// // {
// // 	
// // }

int database_t::get_last_autoID_from_table(string table)
{
	int id=-1;
	string sql = "SELECT seq FROM sqlite_sequence WHERE name='" +table+ "'";
	/*
	vector<vector<string>> lines_cols;
	execute_sql(sql,callback_lines_cols,&lines_cols);
	if (lines_cols.size()!=2) return -1;
	id = tools::str::str_to_int(lines_cols[1][0]);
	*/
	map<string,vector<string>> lines_map;
	execute_sql(sql,callback_lines_map,&lines_map);
	if (lines_map["seq"].size()!=1) return -1;
	id = tools::str::str_to_int(lines_map["seq"][0]);
	return id;
}

// 
// 
// vector<measurement_t> database_t::get_measurements()
// {
// 	
// 	map<string, vector<string>> lines_map;
// 	string sql = "SELECT * FROM " \
//             TABLENAME_measurements \
//             "";
// 	
// 	execute_sql(sql,callback_lines_map,&lines_map);
// 	print(lines_map);
// 	
// 	/*transform map to measurement_t*/
// // 	map<string, vector<string>>::iterator it=lines_map.find("SAMPLE_ID");
// 	if (lines_map.empty()) return {};
// 	int measurements_count = lines_map["sample_id"].size();
// 	if (measurements_count==0) return {};
// 	vector<measurement_t> measurements(measurements_count);
// 	for (int i=0;i<measurements_count;i++)
// 	{
// 		measurement_t *measurement=&measurements[i];
// 		int sample_id = tools::str::str_to_int(lines_map["sample_id"][i]);
// 		measurement->id.olcdb = tools::str::str_to_int(lines_map["olcdb"][i]);
// 		measurement->id.filename_wo_crater_depths = (lines_map["filename_wo_crater_depths"][i]);
// 		measurement->id.directory = (lines_map["directory"][i]);
// 		measurement->id.repition = (lines_map["repition"][i]);
// 		measurement->id.tool = (lines_map["tool"][i]);
// 		measurement->id.group = tools::str::str_to_int(lines_map["mgroup"][i]);
// 		measurement->id.others = tools::str::get_strings_between_delimiter((lines_map["others"][i]),"_");
// 		measurement->id.date_time_start = (lines_map["date_time_start"][i]);
// 		measurement->id.date_time_stop = (lines_map["date_time_stop"][i]);
// 	}
// 	return measurements;
// }
// 
// int database_t::get_measurement_ID(measurement_t* measurement)
// {
// 	int db_id=0;
// 	identifier_t id = measurement->id_get();
// 	string sql = "SELECT * FROM " \
// 			TABLENAME_measurements \
// 			" WHERE " \
// 			"olcdb=" +to_string(id.olcdb)+ " AND " \
// 			"repition='" +id.repition+ "' AND " \
// 			"filename_wo_crater_depths='" +id.filename_wo_crater_depths+ "' AND " \
// 			"mgroup=" +to_string(id.group)+ " AND " \
// 			"tool='" +id.tool+ "';";
// 	map<string,vector<string>> lines_map;
// 	execute_sql(sql,callback_lines_map,&lines_map);
// 	print(lines_map);
// 	return db_id;
// }
// 
// bool database_t::add_measurement(measurement_t* measurement)
// {
// // 	bool error_l;
// 	identifier_t id = measurement->id_get();
// 	string sql = "INSERT INTO " \
//             TABLENAME_samples \
//             " (lot, lot_slpits, wafer, chip_x, chip_y, monitor)" \
//             " VALUES " \
//             "(" \
//             "'"+id.lot+"', " \
//             "'"+id.lot_splits+"', " \
//             ""+to_string(id.wafer)+", " \
//             ""+to_string(id.chip_x)+", " \
//             ""+to_string(id.chip_y)+", " \
//             "'"+id.monitor+"');" ;
// 	if (!execute_sql(sql)) return false;
// 	
// // 	int sample_ID = (get_last_autoID_from_table(TABLENAME_samples));
// 	int sample_ID = sqlite3_last_insert_rowid(this->DB);
// 	sql = "INSERT INTO " \
//             TABLENAME_measurements \
//             " (sample_id, olcdb, filename_wo_crater_depths, directory, repition, tool, " \
//             "mgroup, others, date_time_start, date_time_stop)" \
//             " VALUES " \
//             "(" \
//             ""+to_string(sample_ID)+", " \
// 			""+to_string(id.olcdb)+", " \
//             "'"+id.filename_wo_crater_depths+"', " \
// 			"'"+id.directory+"', " \
// 			"'"+id.repition+"', " \
// 			"'"+id.tool+"', " \
//             ""+to_string(id.group)+", " \
//             "'"+id.others_to_string()+"', " \
// 			"'"+id.date_time_start+"', " \
//             "'"+id.date_time_stop+"');" ;
// 	execute_sql(sql);
// 	return true;
// }
// 
// /*table for all samples*/
// bool database_t::create_table_crater_depths() 
// {
//     std::string sql;
//     sql = "CREATE TABLE IF NOT EXISTS " + string(TABLENAME_samples) + "(" \
// //          "ID INTEGER PRIMARY KEY     AUTOINCREMENT, " 
//          "measurement_id           	 INT, " \
//          "crater_depth             	 DOUBLE);";
//     return execute_sql(sql);
// }


// /*table for all reference samples*/
// bool database_t::create_table_ref_samples() 
// {
//     std::string sql;
//     sql = "CREATE TABLE IF NOT EXISTS " + string(TABLENAME_ref_samples) + "(" \
// //          "ID INTEGER PRIMARY KEY     AUTOINCREMENT, "
// 		 "ref_sample_id INTEGER PRIMARY KEY, " \
//          //identifier_t
//          "lot                   	 TEXT, " \
//          "lot_slpits            	 TEXT, " \
//          "wafer                  	 INT, " \
//          "chip_x                  	 INT, " \
//          "chip_y                  	 INT, " \
//          "monitor                	 TEXT, " \
//          // SiGe40 OR SiGe40Sn0 OR Si0.6Ge0.4 OR Si0.6Ge
//          "matrix_elements           	 TEXT, " \
//          // depth in nm
//          "matrix_depth           	 DOUBLE, " \
//          // like 30Si
//          "implant_isotope         	 TEXT, " \
//          // dose in at/scm
//          "implant_dose           	 DOUBLE, " \
//          // concentration in at/ccm
//          "implant_max_C           	 DOUBLE, " \
//          // depth in nm
//          "implant_D_at_max_C   	 DOUBLE);";
// 	return execute_sql(sql);
// }
// 
// /*table for all samples*/
// bool database_t::create_table_measurements() 
// {
//     std::string sql;
//     sql = "CREATE TABLE IF NOT EXISTS " + string(TABLENAME_measurements) + "(" \
// //          "ID INTEGER PRIMARY KEY     AUTOINCREMENT, "
// 		"measurement_id INTEGER PRIMARY KEY, " \
//          // SAMPLE_ID links to sample(identifier_t) so now just place other identifier_t parts here, which r unique for measurement
//          "sample_id              	 INT, " \
//          "olcdb		              	 INT, " \
//          "filename_wo_crater_depths	 TEXT, " \
//          "directory					 TEXT, " \
// 		 "repition					 TEXT, " \
// 		 "tool						 TEXT, " \
// 		 // can not use "group" in SQL ...
// 		 "mgroup                	 INT, " \
// 		 "others						 TEXT, " \
// 		 "date_time_start			 DATETIME, " \
// 		 "date_time_stop			 DATETIME);";
// 		 
//     return execute_sql(sql);
// }
// 
// 
// 
// // bool database_t::get_implanted_reference_samples(vector<implanted_reference_struct> *refs, implanted_reference_struct where) {
// //     string sql = "SELECT * FROM " + string(TABLENAME_implanted_reference_samples);
// //     string sqlm="";
// //     if (where.sample_name!="") sqlm+=" sample_name = "+string("'")+where.sample_name+"' AND";
// //     if (where.lot!="") sqlm+=" lot = "+string("'")+where.lot+"' AND";
// //     if (where.wafer!="") sqlm+=" wafer = "+string("'")+where.wafer+"' AND";
// //     if (where.comment!="") sqlm+=" comment = "+string("'")+where.comment+"' AND";
// //     if (where.matrix_element!="") sqlm+=" matrix_element = "+string("'")+where.matrix_element+"' AND";
// //     if (where.implant_element!="") sqlm+=" implant_element = "+string("'")+where.implant_element+"' AND";
// //     
// //     if (sqlm!="") {
// //         sqlm.erase(sqlm.end()-4,sqlm.end()); // remove the last " AND"
// //         sql+= " WHERE"+sqlm;
// //     }
// //     sql+=";";
// //     
// //     char *zErrMsg = 0;
// //     int rc = sqlite3_exec(database::DB, sql.c_str(), this->callback_implanted_reference_samples, refs, &zErrMsg);
// // 	if( rc != SQLITE_OK ){
// // 		std::cout << "sqlite3_exec() SQL error: "<< zErrMsg << std::endl;
// // 		std::cout << "sql-command: " << sql << "\n";
// // 		sqlite3_free(zErrMsg);
// // 		return false;
// // 	} else{
// //      #ifdef DEBUG 
// // 		std::cout << sql << "\n\t... executed" << std::endl;
// //       #endif
// //       
// //    }
// //     return true;
// // }
// 
// // bool database_t::set_implanted_reference_samples(vector<implanted_reference_struct> *refs, int ID) {
// //     for (int i=0;i<refs->size();i++) {
// //         string sql = "UPDATE " + string(TABLENAME_implanted_reference_samples) + " SET ";
// //             sql+="sample_name= '"+refs->at(i).sample_name+"', ";
// //             sql+="lot= '"+refs->at(i).lot+"', ";
// //             sql+="wafer= '"+refs->at(i).wafer+"', ";
// //             sql+="comment= '"+refs->at(i).comment+"', ";
// //             sql+="matrix_element= '"+refs->at(i).matrix_element+"', ";
// //             sql+="matrix_depth= "+ to_string(refs->at(i).matrix_depth)+", ";
// //             sql+="implant_element= '"+refs->at(i).implant_element+"', ";
// //             sql+="implant_dose= " +to_string(refs->at(i).implant_dose)+", ";
// //             sql+="implant_max_C= "+to_string(refs->at(i).implant_max_C)+", ";
// //             sql+="implant_max_D= "+to_string(refs->at(i).implant_max_D);
// //             if (ID>0) sql+=" WHERE ID = "+to_string(ID)+";";
// //             else {
// //                 if (refs->at(i).lot!="") sql+=" WHERE lot = '"+refs->at(i).lot+"'"; else return false;
// //                 if (refs->at(i).wafer!="") sql+=" AND wafer = '"+refs->at(i).wafer+"'"; else return false;
// //                 sql+=" AND comment = '"+refs->at(i).comment+"'";
// //                 if (refs->at(i).implant_element!="") sql+=" AND implant_element = '"+refs->at(i).implant_element+"'"; else return false;
// //             }
// //             execute_sql(sql);
// //     }
// //     return true;
// // }
