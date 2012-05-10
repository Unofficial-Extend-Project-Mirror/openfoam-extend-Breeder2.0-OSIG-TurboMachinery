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
#ifndef FOAM_FIELD_BULDER_HPP
#define FOAM_FIELD_BULDER_HPP

#include "fvMesh.H"
//#include "pointFields.H"
#include "pointVolInterpolation.H"

//------------------------------------------------------------------------------
// The FoamFieldBuilder template class is responsible for building 
// the necessary Foam datastructure to represent a scalar or vector field 
// on a fvMesh and interpolating the solution from nodes to cell centers 
// if required.
//
// Template parameters are
//     pfieldclass : point field class        - PointScalarField   or PointVectorField
//     vfieldclass : volume field class       - volScalarField     or volVectorField
//     dimclass    ; dimensioned object class - dimensionedScalar  or dimensionedVector
//     objclass    : type of quantity         - Foam::scalar       or Foam::vector
//     fvpatchfieldclass : patch field class  - fvPatchScalarField or fvPatchVectorField
// obviously, these cannot be mixed arbitrarily
//
// pfieldclass is only used in the process of converting a solution 
// at the nodes into a solution at the cell centers
//------------------------------------------------------------------------------
	
	
template < class pfieldclass, class vfieldclass, class dimclass, class objclass, class fvpatchfieldclass >
class FoamFieldBuilder : private vfieldclass
{
private:
    void assign( pfieldclass& pfield, const std::vector<objclass>& fielddata, int n )
	{
            for ( int i=0 ; i<n ; i++ )
                pfield[i] = fielddata[i];
	}
    void assign( vfieldclass& vfield, const std::vector<objclass>& fielddata, int n )
	{
            for ( int i=0 ; i<n ; i++ )
                vfield[i] = fielddata[i];
	}
	
public:
    FoamFieldBuilder( const std::string& name,
    const Foam::dimensionSet& dims,
    const std::vector<objclass>& fielddata, 
    bool          cgnsSolutionAtNodes,
    Foam::word    timename,
    Foam::fvMesh& c_mesh
    )
        : vfieldclass ( Foam::IOobject
        (
            name,
            timename,  
            c_mesh,
            Foam::IOobject::NO_READ,
            Foam::IOobject::AUTO_WRITE
        ),
        c_mesh,
        dimclass( name, dims, objclass() ),
        fvpatchfieldclass::calculatedType()
        )
	{
            if ( cgnsSolutionAtNodes )
            {
                Foam::pointMesh p_mesh(c_mesh);
                pfieldclass pfield (
                    Foam::IOobject
                    (
                        name + "_cgns",
                        timename,  
                        c_mesh,
                        Foam::IOobject::NO_READ,
                        Foam::IOobject::AUTO_WRITE
                    ),
                    p_mesh,
                    dimclass( name + "_cgns", dims, objclass() ),
                    fvpatchfieldclass::calculatedType()
                );
				
                int nnodes = fielddata.size();
                assign( pfield, fielddata, nnodes );
			
                // Interpolate from vertices to cell centers
                Foam::pointVolInterpolation pvInterpol(p_mesh, c_mesh);
                pvInterpol.interpolate( pfield, *this );
            }
            else
            {
                // Solution are already cell based, just copy them over
                int ncells = fielddata.size();
                assign( *this, fielddata, ncells );
            }
	}
    virtual bool write() const
	{
            return vfieldclass::write();
	}
};

#endif
