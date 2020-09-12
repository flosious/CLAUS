#include "constants.hpp"

quantity_t constants::c()
{
	quantity_t val;
	val.name = "lightspeed";
	val.unit="m/s";
	val.dimension="length/time";
	val.data.push_back(GSL_CONST_MKSA_SPEED_OF_LIGHT);
	return val;
}

quantity_t constants::e()
{
	quantity_t val;
	val.name = "electron_charge";
	val.unit="A*s";
	val.dimension="current*time";
	val.data.push_back(GSL_CONST_MKSA_ELECTRON_CHARGE);
	return val;
}

quantity_t constants::h()
{
	quantity_t val;
	val.name = "planck";
	val.unit="J*s";
	val.dimension="mass*length*length/time";
	val.data.push_back(GSL_CONST_MKSA_PLANCKS_CONSTANT_H);
	return val;
}

quantity_t constants::mu0()
{
	quantity_t val;
	val.name = "permeability";
	val.unit="kg*m/A^2/s^2";
	val.dimension="mass*length/time/time/current/current";
	val.data.push_back(GSL_CONST_MKSA_VACUUM_PERMEABILITY);
	return val;
}

quantity_t constants::Na()
{
	quantity_t val;
	val.name = "avogadro";
	val.unit="1/mol";
	val.dimension="1/amount";
	val.data.push_back(6.02214076e23);
	return val;
}

quantity_t constants::u()
{
	quantity_t val;
	val.name = "atomic_mass";
	val.unit="kg";
	val.dimension="mass";
	val.data.push_back(GSL_CONST_MKSA_UNIFIED_ATOMIC_MASS);
	return val;
}

quantity_t constants::amu()
{
	return u();
}
