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
  Mapping of CGNS' Flow Solutions to Foam's fields

Authors
  Martin Beaudoin, Hydro-Quebec - IREQ, 2005
  Robert Magnan,   Hydro-Quebec - IREQ, 2007

\*---------------------------------------------------------------------------*/
#ifndef SOLUTION_CONVERTER_H
#define SOLUTION_CONVERTER_H

#include "IFstream.H"
#include "fvMesh.H"
#include "libcgnsoo3/cgnsoo.H"

#include "CGNSQuantityConverter.H"

#include <set>
#include <vector>
#include <map>
#include <string>

class ConnectivityMapper;

//------------------------------------------------------------------------------
// This class deals with the conversion between solution in the CGNS file
// and the Foam Fields.
// Methods are provided to write these fields to a time directory
//------------------------------------------------------------------------------
class SolutionConverter
{
private:
    // solinfo_t stores of all the pertinent information about
    // a scalar solution that was fonud in the CGNS file
    // It is used to keep track of the various scalar fields
    // that are defined and to map the CGNS conventionnal names 
    // to the Foam terminology.
    class solinfo_t
    {
    private:
        CGNSOO::Quantity_t     q;
        CGNSOO::GridLocation_t l;
        CGNSOO::FlowSolution_t f;
        int            fieldIndex;
        std::string         n;
    public:
        solinfo_t( CGNSOO::Quantity_t     q, 
        CGNSOO::GridLocation_t l, 
        CGNSOO::FlowSolution_t f, 
        int            fi, 
        const std::string&  n) : 
            q(q), l(l),f(f),fieldIndex(fi),n(n) 
            {}
		
        CGNSOO::Quantity_t     quantity() const { return q; }
        CGNSOO::GridLocation_t location() const { return l; }
        CGNSOO::FlowSolution_t solution() const { return f; }
        std::string         name() const { return n; }
        std::string         readField( vector<double>& localsol ) const
            {
                CGNSOO::DataType_t datatype;
                std::string fieldname;
                CGNSOO::DataArray_t da = f.readField( fieldIndex, fieldname, datatype );
                da.readData( localsol );
                return fieldname;
            }
        bool less( const solinfo_t& rhs ) const 
            { 
                if ( q==rhs.q ) return (n<rhs.n);
                return (q<rhs.q); 
            }
        bool operator==( const solinfo_t& rhs ) const 
            { 
                return (q==rhs.q && l==rhs.l); 
            }
        bool operator<( const solinfo_t& rhs ) const 
            { 
                return q<rhs.q && l<rhs.l && fieldIndex<rhs.fieldIndex; 
            }
    };
    struct solinfo_lt
    {
        bool operator()( const solinfo_t& s1, const solinfo_t& s2 ) const
            {
                return s1.less(s2);
            }
    };

private:
    typedef std::set<solinfo_t,solinfo_lt> solset_t;
    solset_t                availableSolutions;
    map<int,solset_t >      solutionsInZone;
    Foam::fvMesh            cellmesh;
    const CGNSQuantityConverter&  qConverter;
	
private:
    solset_t findSolutionSetInZone( const CGNSOO::Base_t& base, 
    int indexZone, 
    const std::string& solutionName );
	
    // removes all Quantity_t entries in required_components from the set 'solutions'
    // return true if they were all found and removed correctly
    bool validateAndRemove( const std::vector<CGNSOO::Quantity_t>& required_components, 
    solset_t& solutions );
	
public:
    SolutionConverter( const Foam::polyMesh& mesh, 
    const CGNSOO::Base_t& base,
    const CGNSQuantityConverter& qc );
	
    void buildAndWriteFoamFields( 
        const ConnectivityMapper& mapper, 
        const CGNSOO::Base_t&     base, 
        Foam::scalar              rho, 
        const Foam::Time&         time,
        bool                      dryRun = false,
        bool                      mapUnknown = false);
};

#endif
