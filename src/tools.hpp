
#ifndef TOOLS_HPP
#define TOOLS_HPP

#include <stdio.h>
#include <cmath>
#include <iomanip>      // std::get_time
#include <ctime>        // struct std::tm
#include <time.h>
#include <algorithm>
#include <iostream>
#include <stdlib.h>
#include <vector>
#include <string>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <dirent.h>
#include <string.h>
#include <numeric> 
#include <sys/stat.h>
#include <map>
#include "print.hpp"
#include <set>
#include <sstream>
#include <iostream>
#include <time.h>
#include <iomanip>
#include <sstream>
// #include <curl/curl.h> // libcurl4-gnutls-dev
// #include <gsl/gsl_sys.h>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__TOS_WIN__)
#define path_del "\\"
#define line_del "\n"
#elif defined(unix) || defined(__unix) || defined(__unix__) || defined(__APPLE__)
#define path_del "/"
#define line_del "\n"
#endif

#ifdef __unix__ 
#include <unistd.h>
#include <sys/stat.h>
#else
#include <direct.h>
#include <windows.h>
#endif

#define FILETYPE_DELIMITER string(".")

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__TOS_WIN__)
    #include <conio.h>   //for getch(), needed in wait_for_key()
    #include <windows.h> //for Sleep()
    //  static void sleep(int i) { Sleep(i*1000); }
    #define PATH_DELIMITER string("\\")
    #define LINE_DELIMITER string("\n")
#elif defined(unix) || defined(__unix) || defined(__unix__) || defined(__APPLE__)
    #define PATH_DELIMITER string("/")
    #define LINE_DELIMITER string("\n")
#endif

using namespace std;


class tools{
private:  
public:
	class web {
	public:
		static string get_contents(string url);
	};
    class str {
    public:
		static string to_ascii(string mainstring);
		static string remove_NOTchars_from_string(string mainstring, string NOTchars);
		/// checks wheter the input contains just empty strings ("") --> true or not --> false
		static bool is_empty(string input);
		static bool is_empty(vector<string> input);
		static bool is_empty(vector<vector<string>> input);
        static string get_string_between_string_A_and_next_B(string *mainstring,string A, string B);
        static int is_number(std::string str);
        static double str_to_double(string mainstring);
        static int str_to_int(string mainstring);
        static vector<string> get_all_string_between_string_A_and_next_B(string *mainstring,string A, string B);
        static string get_string_between_string_A_and_last_B(string *mainstring,string A, string B);
        static vector<string> get_strings_between_delimiter(string mainstring, string delimiter, bool empty=false);
        static int levenshtein_distance(const std::string &s1, const std::string &s2);
        static void replace_NOTchars_from_string(string *mainstring, string replacethis_NOT, string replacewith);
        static void replace_chars(vector<vector<string>> *mat, string replacethis, string replacewith);
        static void replace_chars(string *mainstring, string replacethis, string replacewith);
        static void replace_chars(vector<string> *mainstring, string replacethis, string replacewith);
        static void remove_spaces_from_string_end(string *mainstring);
        static void remove_spaces_from_string_start(string *mainstring);
        static void remove_spaces(string* mainstring);
        static void remove_spaces(vector<string>* mainstring);
		static void remove_spaces(vector<vector<string>>* mainstring);
        static string remove_linebreakers_from_string(string mainstring); // remove_linebreakers_from_string
        static string remove_chars_from_string(string mainstring, string chars);
        static bool replace_umlaute_to_windows_compatible(string *mainstring);
        static vector<string> args_to_vec(int    argc, char **argv);
        static void remove_duplicates(vector<string>* vec);
        /// returns the KGV of strings, that means, the least common multiple (which may be of course a single letter)
        static vector<string> remove_similarities(vector<string>* vec);
        static void to_lower(string* upper);
        static void to_lower(vector<string>* upper);
        /// extract all number chars into one integer ("A2B342c34" = "234234")
        static int extract_int_from_str(string str);
        static vector<string> find_all_substrings(vector<string>* vec, string find_this);
		static vector<string> find_all_substrings(set<string>& search_these, string find_this);
		static void remove_substring_from_mainstring(string* mainstring, string substring);
    };
    class time {
    public:
		static string tm_struct_to_string_time(tm tm, string time_format="%Y-%m-%d %H:%M:%S");
		static tm string_time_to_tm_structure(string time,string time_format="%Y-%m-%d %H:%M:%S");
		static string time_t_to_string(time_t ttime, string time_format="%Y-%m-%d %H:%M:%S");
        static string seconds_to_human_readable(double secs);
        static time_t string_to_time_t(string time,string time_format="%Y-%m-%d %H:%M:%S");
        static string get_time_now(string time_format="%Y-%M-%d %H-%M-%S");
    };
    class file {
    public:
		static void remove_file_type_from_name(vector<string>* filenames, string filetye_delimiter=".");
		static void remove_file_type_from_name(string* filenames, string filetye_delimiter=".");
        static vector<vector<string>> load(string filename, string delimiter);
        static vector<string> filter_files_from_types(vector<string> * file_list, vector<string> * file_types, bool exclude=false);
        static void remove_files(vector<string> files);
        static vector<string> load_file_to_vector_string(string filename_with_path);
        static string load_file_to_string(string filename_with_path);
        static bool write_mat_to_file(string filename_with_path,vector<vector<double>> *matrix, bool append, string column_delimiter, string line_delimiter=LINE_DELIMITER);
        static bool write_mat_to_file(string filename_with_path,vector<vector<string>> *matrix, bool append, string column_delimiter, string line_delimiter=LINE_DELIMITER);
        static bool write_to_file(string filename_with_path,string *contents, bool append=true);
		static string extract_filetype_ending(string filename,string filetye_delimiter=".");
        static string extract_directory_from_filename(string filename);
        static string extract_filename(string filename,string filetye_delimiter=".");
        static vector<string> extract_filenames(vector<string> filenames,string filetye_delimiter=".");
        static string check_directory_string(string add_directory, string root_dir);
        static string check_directory_string(string add_directory);
        #ifdef __unix__ 
        static int do_mkdir(const char *path, mode_t mode);
        static int mkpath(string pfad, mode_t mode);
        #else
        static int mkpath(string pfad, int delme);  
        #endif
        static bool file_exists(string filename);
		static bool directory_exists(string directory);
        static vector<string> get_file_list_from_dir_name(string dir_name, bool prepend_directory);
		/// returns the creation date of the file (filename with path) in a timeinfo (tm) structure
		static time_t creation_date(string filename);
    };
	class vec 
	{
	public:
// 		template <typename T> static vector<T> add(vector<T> *start,/*vector*/<T> *ende);
		static void add(vector<string> *result_vec,vector<string> adder);
		static void add(vector<double> *result_vec,vector<double> adder);
		static void remove(vector<double> *result_vec,vector<double> subtractors);
		
		static void split_map_to_vecs(map<double,double>* XY_mat, vector<double> *X, vector<double>* Y);
		static void combine_vecs_to_map(vector<double> *X, vector<double>* Y, map<double,double>* XY_mat);
		static void combine_vecs_to_map(vector<double> *X, vector<double> Y, map<double,double>* XY_mat);
		static void combine_vecs_to_map(vector<double> X, vector<double> *Y, map<double,double>* XY_mat);
		static void combine_vecs_to_map(vector<double> X, vector<double> Y, map<double,double>* XY_mat);
		static string combine_vec_to_string(vector<string>& vec, string delimiter="");
		
	};
    class mat {
    public:
		static int is_in_vec(vector<string>* vec, string find_this);
		static int find_in_vec(vector<string>* vec, string find_this);
		static void cut_matrix(vector<vector<string>>* mat);
        static bool find_str_in_mat(vector<vector<string>>* matrix, string find_this);
		static bool find_str_in_mat(vector<string>* vec, string find_this);
		/// returns -1 if not found, otherwise returns the column
        static int find_in_matrix_xy_return_y(vector<vector<string>>* matrix, string find_this);
        static vector<vector<vector<double>>> matrix_XYXY_to_tensor(vector<vector<double>> matrix_XYXY);
        static vector<vector<vector<double>>> matrix_XYY_to_tensor(vector<vector<double>> matrix_XYY);
        static vector<vector<double>> cut_mat_double_rectengular(vector<vector<double>> mat_transposed);
		static vector<vector<string>> cut_mat_rectengular(vector<vector<string>> mat_transposed);
        static vector<vector<double>> str_matrix_to_double_matrix(vector<vector<string>> mat_string);
		static vector<double> str_vec_to_double_vec(vector<string> vec_string);
        static vector<string> double_vec_to_str_vec(vector<double> vec_double);
        static vector<vector<string>> double_matrix_to_str_matrix(vector<vector<double>> mat_double);
        static vector<vector<double> > transpose_matrix_double(vector<vector<double> > matrix);
        static vector<int> get_empty_columns_from_matrix(std::vector<std::vector<std::string>> *mat);
        static vector<vector<string> > unify_2_vectors_to_matrix(vector<string > vector1, vector<string > vector2);
        static vector<vector<string> > transform_tensor_unifying_lines_to_matrix(vector<vector<vector<string> > > *tensor);
        static string create_overview_string_from_matrix(vector< vector<string> > *overview);
        static vector<vector<string> > transform_tensor_unifying_columns_to_matrix(vector < vector < vector < string > > > *tensor);
        static vector<vector<double> > transform_tensor_unifying_columns_to_matrix(vector < vector < vector < double > > > *tensor);
        static void remove_lines_or_columns_from_matrix(vector<vector<string> > *matrix,vector<int> *lines=nullptr, vector<int> *columns=nullptr);  
        static void remove_empty_cols_from_matrix(std::vector<std::vector<std::string>> *mat);
        static void remove_empty_lines_from_matrix(std::vector<std::vector<std::string>> *mat);
        static vector<vector<string> > transpose_matrix(vector<vector<string> > matrix);
        static vector<vector<double> > transpose_matrix(vector<vector<double> > matrix);
        static void fillup_matrix(vector<vector<string> > *matrix);
        static vector<vector <string> > format_string_to_matrix(string *mainstring, string line_delimiter=LINE_DELIMITER, string column_delimiter="\t");  
        static string format_matrix_to_string(vector<vector<string> > *matrix, string line_delimiter=LINE_DELIMITER, string column_delimiter="\t");
        static vector<vector<string> > add_vector_to_matrix(vector<vector<string > > matrix_summand, vector<string > vec);
    };
	class math 
	{
	public:
		/*=values * factor*/
		static vector<double> multiply(vector<double>* values, double factor);
		/*=values + summand*/
		static vector<double> add(vector<double>* values, double summand);
	};


// static bool create_directory(string directory,string root_dir, string path_delimiter);

static void wait_for_key();
 
static bool transform_string_to_xml(string *content, string *header,string *result);
static bool transform_xml_to_string(string *content, string *header,string *xml,int iter=0);
static bool transform_xml_to_vector_string(vector<string> *content, vector<string> *header,string *xml);
  



};


#endif // TOOLS_HPP
