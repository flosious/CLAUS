#ifndef FILES_T_HPP
#define FILES_T_HPP

#include <iostream>
#include <regex>
#include <map>

#include "tools.hpp"
#include "print.hpp"
#include <vector>
#include "globalvars.hpp"

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__TOS_WIN__)
    #define PATH_DELIMITER string("\\")
    #define LINE_DELIMITER string("\r\n")
#elif defined(unix) || defined(__unix) || defined(__unix__) || defined(__APPLE__)
    #define PATH_DELIMITER string("/")
    #define LINE_DELIMITER string("\n")
#endif


using namespace std;

// class file_t 
// {
// public:
//     /// the name of the measurement (without directory or file type ending)
//     string measurement_name;
//     /// the location of all files beloning to the measurement_name (jpg, txt, ...)
//     vector<string> locations;
//     /// the location of all measurement files beloning to the measurement_name, as specified in the user config
// //     vector<string> measurement_locations;
// };

class files_t 
{
	friend class config_t;
private:
	static vector<string> ignore_filename_substrings;
	static bool use_directory_files_list;
	static bool use_wildcards_in_filenames;
	
	/// from the input args
    vector<string> files_list;
	/// files that are ignored, defined in config_t
	vector<string> ignored_files;
	/// filter similiar entries from files_list
//     vector<string> files_list_wo_types;
	/// all the directories where the files from files_list are located in
    vector<string> directory_list;
	/// a list of all files located in all directories from directory_list
    vector<string> all_files_in_directories;

	/// removes files that are ignored, defined in config_t
	void filter_ignored_files(vector<string>* files);
    bool get_directory_list();
	bool get_all_files_in_directories();
	/// scans the input for wildcards (e.g. "*") and applies them
	bool wildcards();
	/// returns all files, asuming filename_regex is regulare expression
	vector<string> wildcards_regex_match(string filename_w_wildcard);
	vector<string> wildcards_substring_match(string filename_w_wildcard);
	/// returns all filenames, which contain a wildcard and removes them from the files_list
	vector<string> split_filenames_with_wildcard();
public:
	
    /// returns a list of locations (dir+filename), given a filename (or just a part of a name)
    vector<string> get_locations_from_filename(string name);

	/// returns a list of filenames (without type ending), which have the least count of common chars over all filenames
    vector<string> get_filenames_with_maximum_common_chars();
	
    /// populates private parameters
    bool get(vector<string> files_list_p);
	
// 	vector<identifier_t> get_identifiers();
	vector<string> vec();
};



#endif // FILE_T.HPP
