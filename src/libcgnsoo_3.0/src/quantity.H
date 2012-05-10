/*------------------------------------------------------------------------------
Copyright (C) 2004-2007 Hydro-Quebec

This file is part of CGNSOO

CGNSOO is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 2, or (at your option) any
later version.

CGNSOO is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with CGNSOO  If not, write to the Free Software Foundation,
Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
------------------------------------------------------------------------------*/
#ifndef CGNSQuantity_H
#define CGNSQuantity_H

#include <string>

// Representation and conversion of CGNS quantities in enumeration and string variant.
namespace CGNSOO
{

enum Quantity_t {
        	NULL_DATA,
                // Coordinates
                COORDINATE_X,
                COORDINATE_Y,
                COORDINATE_Z,
                COORDINATE_R,
                COORDINATE_THETA,
                COORDINATE_PHI,
                COORDINATE_NORMAL,
                COORDINATE_TANGENTIAL,
                COORDINATE_XI,
                COORDINATE_ETA,
                COORDINATE_ZETA,
                // Coordinate related
                COORDINATE_TRANSFORM,
                INTERPOLANTS_DONOR,
                ELEMENT_CONNECTIVITY,
                PARENT_DATA,
                // Solution quantities
                POTENTIAL,
                STREAM_FUNCTION,
                DENSITY,
                PRESSURE,
                TEMPERATURE,
                ENERGY_INTERNAL,
                ENTHALPY, 
		ENTROPY,
                ENTROPY_APPROX,
                DENSITY_STAGNATION,
                PRESSURE_STAGNATION,
                TEMPERATURE_STAGNATION,
                ENERGY_STAGNATION,
                ENTHALPY_STAGNATION,
                ENERGY_STAGNATION_DENSITY,
                VELOCITY_X,
                VELOCITY_Y,
                VELOCITY_Z,
                VELOCITY_R,
                VELOCITY_THETA,
                VELOCITY_PHI,
                VELOCITY_MAGNITUDE,
                VELOCITY_NORMAL,
                VELOCITY_TANGENTIAL,
                VELOCITY_SOUND,
                VELOCITY_SOUND_STAGNATION,
                MOMENTUM_X,
                MOMENTUM_Y,
                MOMENTUM_Z,
                MOMENTUM_MAGNITUDE,
		ROTATING_VELOCITY_X,
		ROTATING_VELOCITY_Y,
		ROTATING_VELOCITY_Z,
		ROTATING_MOMENTUM_X,
		ROTATING_MOMENTUM_Y,
		ROTATING_MOMENTUM_Z,
                ROTATING_VELOCITY_MAGNITUDE,
                ROTATING_PRESSURE_STAGNATION,
                ROTATING_ENERGY_STAGNATION,
                ROTATING_ENERGY_STAGNATION_DENSITY,
                ROTATING_ENTHALPY_STAGNATION,
                ENERGY_KINETIC,
                PRESSURE_DYNAMIC,
		SOUND_INTENSITY_DB,
		SOUND_INTENSITY,
                VORTICITY_X,
                VORTICITY_Y,
                VORTICITY_Z,
                VORTICITY_MAGNITUDE,
                SKIN_FRICTION_X,
                SKIN_FRICTION_Y,
                SKIN_FRICTION_Z,
                SKIN_FRICTION_MAGNITUDE,
                VELOCITY_ANGLE_X,
                VELOCITY_ANGLE_Y,
                VELOCITY_ANGLE_Z,
                VELOCITY_UNIT_VECTOR_X,
                VELOCITY_UNIT_VECTOR_Y,
                VELOCITY_UNIT_VECTOR_Z,
                MASS_FLOW,
                VISCOSITY_KINEMATIC,
                VISCOSITY_MOLECULAR,
                VISCOSITY_EDDY_KINEMATIC,
                VISCOSITY_EDDY,
                THERMAL_CONDUCTIVITY,
                REYNOLDS_STRESS_XX,
                REYNOLDS_STRESS_XY,
                REYNOLDS_STRESS_XZ,
                REYNOLDS_STRESS_YY,
                REYNOLDS_STRESS_YZ,
                REYNOLDS_STRESS_ZZ,
                // Solution related
                POWER_LAW_EXPONENT,
                SUTHERLAND_LAW_CONSTANT,
                TEMPERATURE_REFERENCE,
                VISCOSITY_MOLECULAR_REFERENCE,
                THERMAL_CONDUCTIVITY_REFERENCE,
                IDEAL_GAS_CONSTANT,
                SPECIFIC_HEAT_PRESSURE,
                SPECIFIC_HEAT_VOLUME,
                LENGTH_REFERENCE,
                // Turbulence model quantities
                TURBULENT_DISTANCE,
                TURBULENT_ENERGY_KINETIC,
                TURBULENT_DISSIPATION,
                TURBULENT_DISSIPATION_RATE,
                TURBULENT_B_B_REYNOLDS,
                TURBULENT_S_A_NU_TILDE,
		// Electromag
		VOLTAGE,
		ELECTRIC_FIELD_X,
		ELECTRIC_FIELD_Y,
		ELECTRIC_FIELD_Z,
		MAGNETIC_FIELD_X,
		MAGNETIC_FIELD_Y,
		MAGNETIC_FIELD_Z,
		CURRENT_DENSITY_X,
		CURRENT_DENSITY_Y,
		CURRENT_DENSITY_Z,
		ELECTRIC_CONDUCTIVITY,
		LORENTZ_FORCE_X,
		LORENTZ_FORCE_Y,
		LORENTZ_FORCE_Z,
		JOULE_HEATING,
                // Nondimensional parameters
                MACH,
		MACH_VELOCITY,    
                MACH_VELOCITY_SOUND,
                REYNOLDS,
                REYNOLDS_VELOCITY,
                REYNOLDS_LENGTH,
                REYNOLDS_VISCOSITY_KINEMATIC,
                PRANDTL,
                PRANDTL_THERMAL_CONDUCTIVITY,
                PRANDTL_VISCOSITY_MOLECULAR,
                PRANDTL_SPECIFIC_HEAT_PRESSURE,
                PRANDTL_TURBULENT,
                SPECIFIC_HEAT_RATIO,
                SPECIFIC_HEAT_RATIO_PRESSURE,
                SPECIFIC_HEAT_RATIO_VOLUME,
                COEF_PRESSURE,
                COEF_SKIN_FRICTION_X,
                COEF_SKIN_FRICTION_Y,
                COEF_SKIN_FRICTION_Z,
                COEF_PRESSURE_DYNAMIC,
                COEF_PRESSURE_REFERENCE,
                // Characteristics and Riemann invariants
                RIEMANN_INVARIANT_PLUS,
                RIEMANN_INVARIANT_MINUS,
                CHARACTERISTIC_ENTROPY,
                CHARACTERISTIC_VORTICITY_1,
                CHARACTERISTIC_VORTICITY_2,
                CHARACTERISTIC_ACOUSTIC_PLUS,
                CHARACTERISTIC_ACOUSTIC_MINUS,

                // Forces and moments
                FORCE_X,
                FORCE_Y,
                FORCE_Z,
                FORCE_R,
                FORCE_THETA,
                FORCE_PHI,
                LIFT,
                DRAG,
                MOMENT_X,
                MOMENT_Y,
                MOMENT_Z,
                MOMENT_R,
                MOMENT_THETA,
                MOMENT_PHI,
                MOMENT_XI,
                MOMENT_ETA,
                MOMENT_ZETA,
                MOMENT_CENTER_X,
                MOMENT_CENTER_Y,
                MOMENT_CENTER_Z,

                COEF_LIFT,
                COEF_DRAG,
                COEF_MOMENT_X,
                COEF_MOMENT_Y,
                COEF_MOMENT_Z,
                COEF_MOMENT_R,
                COEF_MOMENT_THETA,
                COEF_MOMENT_PHI,
                COEF_MOMENT_XI,
                COEF_MOMENT_ETA,
                COEF_MOMENT_ZETA,
                //COEF_PRESSURE_DYNAMIC, // already under nondimensional parameters
                COEF_AREA,
                COEF_LENGTH,
                // Time-dependent flow
                TIME_VALUES,
                ITERATION_VALUES,
                NUMBER_OF_ZONES,
                NUMBER_OF_FAMILIES,
                ZONE_POINTERS,
                FAMILY_POINTERS,
                RIGID_GRID_MOTION_POINTERS,
                ARBITRARY_GRID_MOTION_POINTERS,
                GRID_COORDINATES_POINTERS,
                FLOW_SOLUTIONS_POINTERS,
                ORIGIN_LOCATION,
                RIGID_ROTATION_ANGLE,
                RIGID_VELOCITY,
                RIGID_ROTATION_RATE,
                GRID_VELOCITY_X,
                GRID_VELOCITY_Y,
                GRID_VELOCITY_Z,
                GRID_VELOCITY_R,
                GRID_VELOCITY_THETA,
                GRID_VELOCITY_PHI,
                GRID_VELOCITY_XI,
                GRID_VELOCITY_ETA,
                GRID_VELOCITY_ZETA,
                // from version 2.1 on
                FUEL_AIR_RATIO,
                REFERENCE_TEMPERATURE_HOF,
                ENTHALPY_ENERGY_RATIO,
                COMPRESSIBILITY_FACTOR,
                VIBRATIONAL_ELECTRON_ENERGY,
                HEAT_OF_FORMATION,
                VIBRATIONAL_ELECTRON_TEMPERATURE,
                // Done with all predefined quantities
                USER_DATA
};

Quantity_t  QuantityStringToEnum( const std::string& s );
std::string QuantityEnumToString( Quantity_t q );

/* awk script to convert enumeration into a conversion table
 *
 * WARNING: There must be only only item per line...
 *
 * usage : awk -f script.awk input > output
 * the input file is made up of the lines between the curly brackets 
 * in the above enum. Only one line per value is accepted and must be
 * comma-terminated. Comments (// type) are acceptable.

#!/usr/bin/awk
# usage: awk -f enum2code.awk <enum.dat >enum_table.h
function map(s) {
        nm=split(s,a,"_");
        w = "";
        for (iw in a ) {
                left =substr(a[iw],1,1);
                right=substr(a[iw],2);
                m=toupper(left) tolower(right);
                w = w m;
        }
        return w;
}
/\/\// { print; }
/[[:space:]]+(\w+)\,/ {
  name=substr($1,1,length($1)-1);
  print "\t\t{ " name ",\t\"" map(name) "\" },";
}

*/

};   // namespace CGNSOO

#endif
