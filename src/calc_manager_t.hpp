
#ifndef CALC_MANAGER_HPP
#define CALC_MANAGER_HPP

#include "quantity.hpp"
#include "measurement_group_t.hpp"

class calc_manager_t
{
private:
	set<measurement_group_t*> measurement_groups;
// 	vector<quantity_t*> in_calculation_list_p;
public:
// 	vector<quantity_t*> in_calculation_list();
// 	void add_to_in_calculation_list(quantity_t* quantity);
// 	bool is_in_calculation(quantity_t& quantity);
	
// 	bool calculate(quantity_t& quantity);
	calc_manager_t(list<measurement_group_t>& measurement_groups_p);
};

#endif // CALC_MANAGER_HPP
