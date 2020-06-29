
#ifndef PARSER_XPS_CSV_T_HPP
#define PARSER_XPS_CSV_T_HPP

#include "parser_methods.hpp"
#include "filename_t.hpp"

class xps_csv_t: public parser_methods
{
private:
	bool parse_elements();
	bool parse_clusters();
	class element_t
	{
	public:
		/// like O1s, Ge3d
		string name;
		quantity_t sputter_time;
		quantity_t concentration;
	};
	vector<element_t> elements;
	bool check();
public:
	xps_csv_t();
	bool parse(filename_t* filename_p, string* contents);
};

#endif // PARSER_XPS_CSV_T_HPP
