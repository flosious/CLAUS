#include "calc_manager_t.hpp"

calc_manager_t::calc_manager_t(list<measurement_group_t>& measurement_groups_p)
{
	for (auto& measurement_group:measurement_groups_p)
	{
		measurement_groups.insert(&measurement_group);
	}
}
