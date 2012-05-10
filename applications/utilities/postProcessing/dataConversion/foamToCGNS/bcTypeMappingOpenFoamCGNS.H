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
  Conversion of CGNS files into Foam's mesh and fields:  Utility functions
  for the conversion of OpenFOAM boundary conditions names to CGNS enums

Authors 
  Martin Beaudoin, Hydro-Quebec - IREQ, 2005

\*---------------------------------------------------------------------------*/

#ifndef BCTYPEMAPPINGOPENFOAMCGNS_H
#define BCTYPEMAPPINGOPENFOAMCGNS_H

#include <string>
#include <map>
#include <algorithm>
//#include "CGNS_Boundary_Condition.h"

namespace Foam
{
// Note that these parameters can be wrtten in any uppercase or lowercase combination
// thus, VeloCiTy and velocity are the same
//
#define _OPENFOAM_INLET_STR_TOKEN     "inlet"            
#define _OPENFOAM_OUTLET_STR_TOKEN    "outlet"    
#define _OPENFOAM_WALL_STR_TOKEN      "wall"     
#define _OPENFOAM_WALLFUNC_STR_TOKEN  "wallFunctions"
#define _OPENFOAM_EMPTY_STR_TOKEN     "empty"     


    // Conversion from string to enum
	// I prefer this kind of conversion rather than a string-to-string conversion
	// ASCII syntax may change in time but the enum stays the same
#if 0
	typedef std::map<const std::string, const CGNSOO::BCType_t> m_OpenFoamToCGNS_BcTypeXlate_from_string;

	const m_OpenFoamToCGNS_BcTypeXlate_from_string::value_type OpenFoamBcTypeXlate_map_init[] = {
		m_OpenFoamToCGNS_BcTypeXlate_from_string::value_type( _OPENFOAM_INLET_STR_TOKEN,         CGNSOO::BCInflow ),
		m_OpenFoamToCGNS_BcTypeXlate_from_string::value_type( _OPENFOAM_OUTLET_STR_TOKEN,        CGNSOO::BCOutflow),
		m_OpenFoamToCGNS_BcTypeXlate_from_string::value_type( _OPENFOAM_WALL_STR_TOKEN,          CGNSOO::BCWall   ),
		m_OpenFoamToCGNS_BcTypeXlate_from_string::value_type( _OPENFOAM_WALLFUNCTIONS_STR_TOKEN, CGNSOO::BCWall   ),   
		m_OpenFoamToCGNS_BcTypeXlate_from_string::value_type( _OPENFOAM_EMPTY_STR_TOKEN,         CGNSOO::BCTypeNull)
	};
#else	
	typedef std::pair<const std::string,const CGNSOO::BCType_t> table_entry;
	const table_entry table[] = {   
		table_entry(_OPENFOAM_INLET_STR_TOKEN,    CGNSOO::BCInflow  ),
		table_entry(_OPENFOAM_OUTLET_STR_TOKEN,   CGNSOO::BCOutflow ),
		table_entry(_OPENFOAM_WALL_STR_TOKEN,     CGNSOO::BCWall    ),
		table_entry(_OPENFOAM_WALLFUNC_STR_TOKEN, CGNSOO::BCWall    ),   
		table_entry(_OPENFOAM_EMPTY_STR_TOKEN,    CGNSOO::BCTypeNull) 
	};
	const int n_table_entries = sizeof(table)/sizeof(table_entry);
#endif
					
	// Utility function to translate BC type strings into the corresponding CGNS enum
	static CGNSOO::BCType_t string_OpenFoam_to_CGNSBcType( std::string& s_bcType )
	{
		// Default value
		CGNSOO::BCType_t retValue = CGNSOO::BCTypeNull;
#if 0
		// Build a map
		static m_OpenFoamToCGNS_BcTypeXlate_from_string m_string_to_OpenFoamBcType(OpenFoamBcTypeXlate_map_init,
											   OpenFoamBcTypeXlate_map_init +
											   sizeof(OpenFoamBcTypeXlate_map_init) /
											   sizeof(OpenFoamBcTypeXlate_map_init[0]));
		m_OpenFoamToCGNS_BcTypeXlate_from_string::iterator p_m_string_to_OpenFoamBcType;

		p_m_string_to_OpenFoamBcType = m_string_to_OpenFoamBcType.find( s_bcType );

		if ( p_m_string_to_OpenFoamBcType != m_string_to_OpenFoamBcType.end() )
			retValue = p_m_string_to_OpenFoamBcType->second;
#else
		for ( int i=0 ; i<n_table_entries ; i++ )
		{
			if ( table[i].first == s_bcType )
			{
				retValue = table[i].second;
				break;
			}
		}
#endif
		return retValue;
	}


} // End namespace Foam

#endif
