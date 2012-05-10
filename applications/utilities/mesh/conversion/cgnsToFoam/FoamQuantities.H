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
  Mapping between CGNS' quantities and Foam's

Authors
  Martin Beaudoin, Hydro-Quebec - IREQ, 2005
  Robert Magnan,   Hydro-Quebec - IREQ, 2007

\*---------------------------------------------------------------------------*/
#ifndef FOAM_QUANTITIES_H
#define FOAM_QUANTITIES_H

#include "libcgnsoo3/cgnsoo.H"

#include <string>
#include <vector>
#include <cstdarg>
#include <cassert>

//------------------------------------------------------------------------------
// FoamQuantities
// Class to represent the correspondance between Foam's quantity names
// and CGNS's. 
// Some complexity comes from the fact that all CGNS quantities are scalars
// while Foam is aware that some quantities are vectorial.
//
// Mapping is accomplished through a lookup table 'vdata'. 
// Each known CGNS quantity is associated a (vgroup,groupindex) pair. 
// Vectorial components share the same 'vgroup' and 'newname'  values
// Pure scalars have a unique vgroup number and a 0 groupindex.
//------------------------------------------------------------------------------

class FoamQuantities
{
private:
    struct q_data { // entries of the lookup table
        CGNSOO::Quantity_t q;		// CGNS quantity
        std::string             newname;	// Corresponding name used by Foam
        Foam::dimensionSet dims;	// dimensions of the Foam quantity
        int                vgroup;	// group of scalars to which 'q' belongs
        int                groupindex;  // index of this scalar in the group
    };
private:
    std::vector<q_data> vdata;
    int last_index;

private:	
    void addVector( const std::string& name, int n, const Foam::dimensionSet& dims, ... )
	{
            va_list ap;
            va_start(ap,dims);
            for ( int k=0 ; k<n ; k++ )
            {
                CGNSOO::Quantity_t q = static_cast<CGNSOO::Quantity_t>(va_arg(ap,int));
                q_data d = {q,name,dims,last_index,k};
                //d.q = q;
                //d.newname = name;
                //d.vgroup = last_index;
                //d.groupindex = k;
                vdata.push_back( d );
            }
            va_end(ap);
            last_index++;
	}
    void addScalar( const std::string& name, const Foam::dimensionSet& dims, CGNSOO::Quantity_t q )
	{
            q_data d = {q,name,dims,last_index,0};
            //d.q = q;
            //d.newname = name;
            //d.vgroup = last_index;
            //d.groupindex = k;
            vdata.push_back( d );
            last_index++;
	}
    CGNSOO::Quantity_t lookup( int group, int index, std::vector<q_data>::const_iterator start ) const
	{
            for ( std::vector<q_data>::const_iterator i =start ; 
                  i!=vdata.end() ; 
                  i++ )
            {
                if ( (*i).vgroup == group && (*i).groupindex == index ) 
                    return (*i).q;
            }
            return CGNSOO::NULL_DATA;
	}
	
public:
    FoamQuantities() : last_index(0) 
	{
            // build a static table defining the components and units of each known quantity
            const Foam::dimensionSet adim   ( 0,0,0,  0,0,0,0 );
            const Foam::dimensionSet kg     ( 1,0,0,  0,0,0,0 );
            const Foam::dimensionSet meter  ( 0,1,0,  0,0,0,0 );
            const Foam::dimensionSet second ( 0,0,1,  0,0,0,0 );
            const Foam::dimensionSet kelvin ( 0,0,0,  1,0,0,0 );
            const Foam::dimensionSet force   = kg*meter/(second*second);
            const Foam::dimensionSet surface = meter*meter;
            const Foam::dimensionSet pressure= force/surface;
            const Foam::dimensionSet density = kg/(meter*meter*meter);
            const Foam::dimensionSet m2_s    = (meter*meter)/second;
            const Foam::dimensionSet m2_s2   = (meter*meter)/(second*second);
            const Foam::dimensionSet m2_s3   = (meter*meter)/(second*second*second);
		
            addVector("U", 3       , meter/second     , CGNSOO::VELOCITY_X ,CGNSOO::VELOCITY_Y ,CGNSOO::VELOCITY_Z );
            addVector("W", 3       , adim/second      , CGNSOO::VORTICITY_X,CGNSOO::VORTICITY_Y,CGNSOO::VORTICITY_Z);
            addScalar("p"          , pressure/density , CGNSOO::PRESSURE ); // p/rho
            addScalar("k"          , m2_s2            , CGNSOO::TURBULENT_ENERGY_KINETIC );
            addScalar("epsilon"    , m2_s3            , CGNSOO::TURBULENT_DISSIPATION );
            addScalar("nuTilda"    , m2_s3            , CGNSOO::TURBULENT_S_A_NU_TILDE );
            addScalar("omega"      , adim/second      , CGNSOO::TURBULENT_DISSIPATION_RATE );
            addScalar("temperature", kelvin           , CGNSOO::TEMPERATURE );
            addScalar("potential"  , m2_s             , CGNSOO::POTENTIAL );
            addScalar("density"    , density          , CGNSOO::DENSITY );
	}
    bool isVectorQuantity( CGNSOO::Quantity_t q ) const
	{
            int n=0;
            for ( std::vector<q_data>::const_iterator i =vdata.begin() ; 
                  i!=vdata.end() ; 
                  i++ )
            {
                if ( (*i).q == q ) 
                {
                    for ( std::vector<q_data>::const_iterator j =vdata.begin() ; 
                          j!=vdata.end() ; 
                          j++ )
                    {
                        if ( (*j).vgroup == (*i).vgroup ) n++;
                    }			
                    break;
                }
            }
            return (n==3);
	}
    bool isScalarQuantity( CGNSOO::Quantity_t q ) const
	{
            return !isVectorQuantity(q);
	}
    std::string getFoamName( CGNSOO::Quantity_t q ) const
	{
            for ( std::vector<q_data>::const_iterator i =vdata.begin() ; 
                  i!=vdata.end() ; 
                  i++ )
            {
                if ( (*i).q == q ) return (*i).newname;
            }
            return "";
	}
    CGNSOO::Quantity_t firstQuantityInGroup( CGNSOO::Quantity_t q ) const
	{
            for ( std::vector<q_data>::const_iterator i =vdata.begin() ; 
                  i!=vdata.end() ; 
                  i++ )
            {
                if ( (*i).q == q ) 
                    return lookup( (*i).vgroup, 0, i );
            }
            return q;
	}
    CGNSOO::Quantity_t nextQuantityInGroup( CGNSOO::Quantity_t q ) const
	{
            for ( std::vector<q_data>::const_iterator i =vdata.begin() ; 
                  i!=vdata.end() ; 
                  i++ )
            {
                if ( (*i).q == q ) 
                    return lookup( (*i).vgroup, (*i).groupindex+1, i );
            }
            return CGNSOO::NULL_DATA;
	}
    std::vector<CGNSOO::Quantity_t> getQuantitiesInGroup( CGNSOO::Quantity_t q )
	{
            std::vector<CGNSOO::Quantity_t> components;
            for ( CGNSOO::Quantity_t qlookup  = firstQuantityInGroup(q) ;
                  qlookup != CGNSOO::NULL_DATA ;
                  qlookup  = nextQuantityInGroup(qlookup) )
            {
                components.push_back( qlookup );
            }
            return components;
	}
    Foam::dimensionSet getFoamDimensions( CGNSOO::Quantity_t q ) const
	{
            for ( std::vector<q_data>::const_iterator i =vdata.begin() ; 
                  i!=vdata.end() ; 
                  i++ )
            {
                if ( (*i).q == q ) return (*i).dims;
            }
            return Foam::dimensionSet(0,0,0,0,0,0,0);
	}
};
	
#endif
