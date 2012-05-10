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
  Conversion of CGNS files into Foam's mesh and fields:   Correspondance
  table between cell vertex indices in OpenFOAM and CGNS

Authors 
  Martin Beaudoin, Hydro-Quebec - IREQ, 2005

\*---------------------------------------------------------------------------*/


#ifndef vertexMappingOpenFoam_H
#define vertexMappingOpenFoam_H

namespace Foam
{
template< int m_number_of_vertices, CGNSOO::ElementType_t m_element_type >
class CGNS_CellConnectivityBuilder
{
private:
	const int (*m_swap_table)[2];
public:
	CGNS_CellConnectivityBuilder( const int (*st)[2] ) : m_swap_table(st)
	{}
protected:	
	void init( std::vector<int>& connectivity, CGNSOO::ElementType_t& e_type, const labelList& shapeLabels )
	{
		if(e_type == CGNSOO::MIXED)
		{
			connectivity.push_back(m_element_type);
		}
		//else if(e_type != m_element_type)
		//{
		//	e_type = m_element_type;
		//}
		for ( int i=0 ; i<m_number_of_vertices ; i++ )
		{
			connectivity.push_back( shapeLabels[m_swap_table[i][1]]+1 );
		}
	}
};

// Starting index: easier to read if we take both original notation, 
// and adapt starting indices accordingly

const int CGNS_STARTING_INDEX       = 1;  // Starting index in CGNS
const int OPENFOAM_STARTING_INDEX   = 0;  // Starting index in OpenFoam

// CGNS to OpenFoam
const int table_CGNStoOpenFoam_swapIndicesTetrahedron[4][2] = { 
        {1 - CGNS_STARTING_INDEX, 0 - OPENFOAM_STARTING_INDEX},
        {2 - CGNS_STARTING_INDEX, 1 - OPENFOAM_STARTING_INDEX},
        {3 - CGNS_STARTING_INDEX, 2 - OPENFOAM_STARTING_INDEX},
        {4 - CGNS_STARTING_INDEX, 3 - OPENFOAM_STARTING_INDEX}
};

const int table_CGNStoOpenFoam_swapIndicesPyramid[5][2] = {
        {1 - CGNS_STARTING_INDEX, 0 - OPENFOAM_STARTING_INDEX},
        {2 - CGNS_STARTING_INDEX, 1 - OPENFOAM_STARTING_INDEX},
        {3 - CGNS_STARTING_INDEX, 2 - OPENFOAM_STARTING_INDEX},
        {4 - CGNS_STARTING_INDEX, 3 - OPENFOAM_STARTING_INDEX},
        {5 - CGNS_STARTING_INDEX, 4 - OPENFOAM_STARTING_INDEX}
};

const int table_CGNStoOpenFoam_swapIndicesPrism[6][2] = {
        {1 - CGNS_STARTING_INDEX, 0 - OPENFOAM_STARTING_INDEX},
        {2 - CGNS_STARTING_INDEX, 1 - OPENFOAM_STARTING_INDEX},
        {3 - CGNS_STARTING_INDEX, 2 - OPENFOAM_STARTING_INDEX},
        {4 - CGNS_STARTING_INDEX, 3 - OPENFOAM_STARTING_INDEX},
        {5 - CGNS_STARTING_INDEX, 4 - OPENFOAM_STARTING_INDEX},
        {6 - CGNS_STARTING_INDEX, 5 - OPENFOAM_STARTING_INDEX}
};

const int table_CGNStoOpenFoam_swapIndicesHexahedron[8][2] = { 
        {1 - CGNS_STARTING_INDEX, 0 - OPENFOAM_STARTING_INDEX},
        {2 - CGNS_STARTING_INDEX, 1 - OPENFOAM_STARTING_INDEX},
        {3 - CGNS_STARTING_INDEX, 2 - OPENFOAM_STARTING_INDEX},
        {4 - CGNS_STARTING_INDEX, 3 - OPENFOAM_STARTING_INDEX},
        {5 - CGNS_STARTING_INDEX, 4 - OPENFOAM_STARTING_INDEX},
        {6 - CGNS_STARTING_INDEX, 5 - OPENFOAM_STARTING_INDEX},
        {7 - CGNS_STARTING_INDEX, 6 - OPENFOAM_STARTING_INDEX},
        {8 - CGNS_STARTING_INDEX, 7 - OPENFOAM_STARTING_INDEX}
};


// OpenFoam to CGNS
const int table_OpenFoamtoCGNS_swapIndicesTetrahedron[4][2] = {  
        {0 - OPENFOAM_STARTING_INDEX, 1 - CGNS_STARTING_INDEX},
        {1 - OPENFOAM_STARTING_INDEX, 2 - CGNS_STARTING_INDEX},
        {2 - OPENFOAM_STARTING_INDEX, 3 - CGNS_STARTING_INDEX},
        {3 - OPENFOAM_STARTING_INDEX, 4 - CGNS_STARTING_INDEX}
};

const int table_OpenFoamtoCGNS_swapIndicesPyramid[5][2] = { 
        {0 - OPENFOAM_STARTING_INDEX, 1 - CGNS_STARTING_INDEX},
        {1 - OPENFOAM_STARTING_INDEX, 2 - CGNS_STARTING_INDEX},
        {2 - OPENFOAM_STARTING_INDEX, 3 - CGNS_STARTING_INDEX},
        {3 - OPENFOAM_STARTING_INDEX, 4 - CGNS_STARTING_INDEX},
        {4 - OPENFOAM_STARTING_INDEX, 5 - CGNS_STARTING_INDEX}
};

const int table_OpenFoamtoCGNS_swapIndicesPrism[6][2] = { 
        {0 - OPENFOAM_STARTING_INDEX, 1 - CGNS_STARTING_INDEX},
        {1 - OPENFOAM_STARTING_INDEX, 2 - CGNS_STARTING_INDEX},
        {2 - OPENFOAM_STARTING_INDEX, 3 - CGNS_STARTING_INDEX},
        {3 - OPENFOAM_STARTING_INDEX, 4 - CGNS_STARTING_INDEX},
        {4 - OPENFOAM_STARTING_INDEX, 5 - CGNS_STARTING_INDEX},
        {5 - OPENFOAM_STARTING_INDEX, 6 - CGNS_STARTING_INDEX}
};

const int table_OpenFoamtoCGNS_swapIndicesHexahedron[8][2] = { 
        {0 - OPENFOAM_STARTING_INDEX, 1 - CGNS_STARTING_INDEX},
        {1 - OPENFOAM_STARTING_INDEX, 2 - CGNS_STARTING_INDEX},
        {2 - OPENFOAM_STARTING_INDEX, 3 - CGNS_STARTING_INDEX},
        {3 - OPENFOAM_STARTING_INDEX, 4 - CGNS_STARTING_INDEX},
        {4 - OPENFOAM_STARTING_INDEX, 5 - CGNS_STARTING_INDEX},
        {5 - OPENFOAM_STARTING_INDEX, 6 - CGNS_STARTING_INDEX},
        {6 - OPENFOAM_STARTING_INDEX, 7 - CGNS_STARTING_INDEX},
        {7 - OPENFOAM_STARTING_INDEX, 8 - CGNS_STARTING_INDEX}
};

class CGNS_Hexa8ConnectivityBuilder : public CGNS_CellConnectivityBuilder<8,CGNSOO::HEXA_8>
{
public:
	CGNS_Hexa8ConnectivityBuilder( std::vector<int>& connectivity, CGNSOO::ElementType_t& e_type, const labelList& shapeLabels ) : 
		CGNS_CellConnectivityBuilder<8,CGNSOO::HEXA_8>( table_OpenFoamtoCGNS_swapIndicesHexahedron )
	{ init( connectivity, e_type, shapeLabels ); }
};
class CGNS_Prism6ConnectivityBuilder : public CGNS_CellConnectivityBuilder<6,CGNSOO::PENTA_6>
{
public:
	CGNS_Prism6ConnectivityBuilder( std::vector<int>& connectivity, CGNSOO::ElementType_t& e_type, const labelList& shapeLabels ) : 
		CGNS_CellConnectivityBuilder<6,CGNSOO::PENTA_6>( table_OpenFoamtoCGNS_swapIndicesPrism )
	{ init( connectivity, e_type, shapeLabels ); }
};
class CGNS_Pyra5ConnectivityBuilder : public CGNS_CellConnectivityBuilder<5,CGNSOO::PYRA_5>
{
public:
	CGNS_Pyra5ConnectivityBuilder( std::vector<int>& connectivity, CGNSOO::ElementType_t& e_type, const labelList& shapeLabels ) : 
		CGNS_CellConnectivityBuilder<5,CGNSOO::PYRA_5>( table_OpenFoamtoCGNS_swapIndicesPyramid )
	{ init( connectivity, e_type, shapeLabels ); }
};
class CGNS_Tetra4ConnectivityBuilder : public CGNS_CellConnectivityBuilder<4,CGNSOO::TETRA_4>
{
public:
	CGNS_Tetra4ConnectivityBuilder( std::vector<int>& connectivity, CGNSOO::ElementType_t& e_type, const labelList& shapeLabels ) : 
		CGNS_CellConnectivityBuilder<4,CGNSOO::TETRA_4>( table_OpenFoamtoCGNS_swapIndicesTetrahedron )
	{ init( connectivity, e_type, shapeLabels ); }
};

} // End namespace Foam

#endif
