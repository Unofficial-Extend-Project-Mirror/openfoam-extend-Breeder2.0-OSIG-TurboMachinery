/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | Copyright Hydro-Quebec - IREQ, 2008
     \\/     M anipulation  |
-------------------------------------------------------------------------------
  License
  This file is part of OpenFOAM.

  OpenFOAM is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version.

  OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
  for more details.

  You should have received a copy of the GNU General Public License
  along with OpenFOAM; if not, write to the Free Software Foundation,
  Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA

Description
  Conversion of CGNS files into Foam's mesh and fields: Correspondance
  table between field names from OpenFOAM to CGNS

Authors 
  Martin Beaudoin, Hydro-Quebec - IREQ, 2005

\*---------------------------------------------------------------------------*/


#ifndef fieldsNameMapping_H
#define fieldsNameMapping_H

#include <string>
#include <map>
#include "libcgnsoo3/quantity.H"

namespace Foam
{

// Specifie le type de champs
// A noter que la casse des noms de ces paramertes dans les fichiers de config de OpenFoam est libre
// Donc, Velocity et VeLoCiTy sont permis... 
//
#define _OPENFOAM_VELOCITY_X_STR_TOKEN "U_X"     // Composante X du champs vectoriel de vitesse
#define _OPENFOAM_VELOCITY_Y_STR_TOKEN "U_Y"     // Composante Y du champs vectoriel de vitesse
#define _OPENFOAM_VELOCITY_Z_STR_TOKEN "U_Z"     // Composante Z du champs vectoriel de vitesse
#define _OPENFOAM_PRESSURE_STR_TOKEN   "p"       // Champs scalaire
#define _OPENFOAM_K_STR_TOKEN          "k"       // Champs scalaire
#define _OPENFOAM_EPSILON_STR_TOKEN    "epsilon" // Champs scalaire
#define _OPENFOAM_NU_TILDA_STR_TOKEN   "nuTilda" // Champs scalaire

    // Conversion string to enum
	//
	// Je prefere ce type de conversion plutot qu'une conversion string to string.
	// Ainsi, la syntaxe ASCII peut changer avec le temps, mais le enum reste pareil...
	typedef std::map<const std::string, const CGNSOO::Quantity_t> m_OpenFoamToCGNSFieldXlate_from_string;

	const m_OpenFoamToCGNSFieldXlate_from_string::value_type OpenFoamFieldXlate_map_init[] = {
		m_OpenFoamToCGNSFieldXlate_from_string::value_type( _OPENFOAM_VELOCITY_X_STR_TOKEN, CGNSOO::VELOCITY_X),
		m_OpenFoamToCGNSFieldXlate_from_string::value_type( _OPENFOAM_VELOCITY_Y_STR_TOKEN, CGNSOO::VELOCITY_Y),
		m_OpenFoamToCGNSFieldXlate_from_string::value_type( _OPENFOAM_VELOCITY_Z_STR_TOKEN, CGNSOO::VELOCITY_Z),
		m_OpenFoamToCGNSFieldXlate_from_string::value_type( _OPENFOAM_PRESSURE_STR_TOKEN,   CGNSOO::PRESSURE),
		m_OpenFoamToCGNSFieldXlate_from_string::value_type( _OPENFOAM_K_STR_TOKEN,          CGNSOO::TURBULENT_ENERGY_KINETIC),
		m_OpenFoamToCGNSFieldXlate_from_string::value_type( _OPENFOAM_EPSILON_STR_TOKEN,    CGNSOO::TURBULENT_DISSIPATION),
		m_OpenFoamToCGNSFieldXlate_from_string::value_type( _OPENFOAM_NU_TILDA_STR_TOKEN,   CGNSOO::TURBULENT_S_A_NU_TILDE)
	};

	// fonction utilitaire pour conversion des chaines de type de champs en enum
	static CGNSOO::Quantity_t string_OpenFoam_to_CGNSField(std::string s_field)
	{
		// Valeur par defaut
		CGNSOO::Quantity_t retValue = CGNSOO::NULL_DATA;

		// Construit la map
		static m_OpenFoamToCGNSFieldXlate_from_string m_string_to_OpenFoamField(OpenFoamFieldXlate_map_init,
											OpenFoamFieldXlate_map_init +
											sizeof(OpenFoamFieldXlate_map_init) /
											sizeof(OpenFoamFieldXlate_map_init[0]));
		m_OpenFoamToCGNSFieldXlate_from_string::iterator p_m_string_to_OpenFoamField;

		p_m_string_to_OpenFoamField = m_string_to_OpenFoamField.find( s_field);

		if(p_m_string_to_OpenFoamField != m_string_to_OpenFoamField.end())
			retValue = p_m_string_to_OpenFoamField->second;

		return retValue;
	}


} // End namespace Foam

#endif
