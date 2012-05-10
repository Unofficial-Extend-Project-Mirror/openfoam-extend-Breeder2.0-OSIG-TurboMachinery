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

Authors
  Robert Magnan,  Hydro-Quebec - IREQ, 2007

\*---------------------------------------------------------------------------*/
//------------------------------------------------------------------------------
//
// Local class encapsulating some conversion requests
// on a CGNS element type
//
//------------------------------------------------------------------------------
#include "libcgnsoo3/cgnsoo.H"

#include "cellModeller.H"
#include "cellModel.H"
#include "string.H"

class CGNSElementType
{
public:
    static int getNumberOfNodesPerElement( CGNSOO::ElementType_t etype )
	{
            switch( etype )
            {
		case CGNSOO::NODE:
                    return 1;
		case CGNSOO::BAR_2:
                    return 2;
		case CGNSOO::BAR_3:
                    return 3;
		case CGNSOO::TRI_3:
                    return 3;
		case CGNSOO::TRI_6:
                    return 6;
		case CGNSOO::QUAD_4:
                    return 4;
		case CGNSOO::QUAD_8:
                    return 8;
		case CGNSOO::QUAD_9:
                    return 9;
		case CGNSOO::TETRA_4:
                    return 4;
		case CGNSOO::TETRA_10:
                    return 10;
		case CGNSOO::PYRA_5:
                    return 5;
		case CGNSOO::PYRA_14:
                    return 14;
		case CGNSOO::PENTA_6:
                    return 6;
		case CGNSOO::PENTA_15:
                    return 15;
		case CGNSOO::PENTA_18:
                    return 18;
		case CGNSOO::HEXA_8:
                    return 8;
		case CGNSOO::HEXA_20:
                    return 20;
		case CGNSOO::HEXA_27:
                    return 27;
		case CGNSOO::ElementTypeNull:
		case CGNSOO::ElementTypeUserDefined:
		case CGNSOO::NGON_n:
		case CGNSOO::MIXED:
                    return -1;
            }
            return -1;
	}
    static std::string enumToString( CGNSOO::ElementType_t etype )
	{
            switch( etype )
            {
		case CGNSOO::NODE:
                    return "NODE";
		case CGNSOO::BAR_2:
                    return "BAR_2";
		case CGNSOO::BAR_3:
                    return "BAR_3";
		case CGNSOO::TRI_3:
                    return "TRI_3";
		case CGNSOO::TRI_6:
                    return "TRI_6";
		case CGNSOO::QUAD_4:
                    return "QUAD_4";
		case CGNSOO::QUAD_8:
                    return "QUAD_8";
		case CGNSOO::QUAD_9:
                    return "QUAD_9";
		case CGNSOO::TETRA_4:
                    return "TETRA_4";
		case CGNSOO::TETRA_10:
                    return "TETRA_10";
		case CGNSOO::PYRA_5:
                    return "PYRA_5";
		case CGNSOO::PYRA_14:
                    return "PYRA_14";
		case CGNSOO::PENTA_6:
                    return "PENTA_6";
		case CGNSOO::PENTA_15:
                    return "PENTA_15";
		case CGNSOO::PENTA_18:
                    return "PENTA_18";
		case CGNSOO::HEXA_8:
                    return "HEXA_8";
		case CGNSOO::HEXA_20:
                    return "HEXA_20";
		case CGNSOO::HEXA_27:
                    return "HEXA_27";
		case CGNSOO::ElementTypeNull:
                    return "Null";
		case CGNSOO::ElementTypeUserDefined:
                    return "UserDefined";
		case CGNSOO::NGON_n:
                    return "NGON_n";
		case CGNSOO::MIXED:
                    return "MIXED";
            }
            return "Error - Unknown";
	}
    static const Foam::cellModel& getFoamCellModel( CGNSOO::ElementType_t etype )
	{
            Foam::string dictEntry = "unknown";
            switch ( etype )
            {
        	case CGNSOO::TETRA_4:
                    dictEntry = "tet"; break;
        	case CGNSOO::PYRA_5:
                    dictEntry = "pyr"; break;
        	case CGNSOO::PENTA_6:
                    dictEntry = "prism"; break;
        	case CGNSOO::HEXA_8:
                    dictEntry = "hex"; break;
		default:
                    break;
            }
     
            return *(Foam::cellModeller::lookup(dictEntry));
	}
    static bool is2D( CGNSOO::ElementType_t etype )
	{
            switch ( etype )
            {
        	case CGNSOO::TETRA_4:
        	case CGNSOO::PYRA_5:
        	case CGNSOO::PENTA_6:
        	case CGNSOO::HEXA_8:
                    return false;
		default:
                    break;
            }
            return true;
 	}
};
