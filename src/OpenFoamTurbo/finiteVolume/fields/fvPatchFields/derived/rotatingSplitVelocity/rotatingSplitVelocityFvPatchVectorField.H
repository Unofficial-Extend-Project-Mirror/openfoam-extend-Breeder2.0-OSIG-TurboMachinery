/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | Copyright held by original author
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
    Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

Class
     Foam::rotatingSplitVelocityFvPatchVectorField

Description
    Foam::rotatingSplitVelocityFvPatchVectorField

SourceFiles
    Based on a constant value for the absoulte velocity, the relative velocity
    is computed from the angular velocity vector in two different domains, inside
    one patch, by Urel = Uabs - rotatingVelocity.

    The bounding box for domain Two is in cylindrical coordinates by minimum and
    maximum r=sqrt(x^2+y^2) and z coordinates.

    Example of the boundary condition specification:
    @verbatim
    hub
    {
        type            rotatingSplitVelocity;
        absoluteValue   (0 0 0);   // absolute velocity vector of the whole patch
        minZ 0.05;                 // Minimum Z coordinate of bounding box for domain Two
        maxZ 0.15;                 // Maximum Z coordinate of bounding box for domain Two
        minR 0.01;                 // Minimum R coordinate of bounding box for domain Two
        maxR 0.05;                 // Maximum R coordinate of bounding box for domain Two
        omegaOne (0.0 0.0 1000.0); // angular velocity vector of domain One
        omegaTwo (0.0 0.0 0.0);    // angular velocity vector of domain Two
        value uniform (0 0 0);     // initial value for the relative velocity
    }
    @endverbatim

NOTE:
      - if absoluteValue is zero, just the rotating velocity in the two domains is determined
      - If you just have one domain, just put the limits of domain Two outside your computation domain.
        This should work, but you have one needless loop, which may notice for a large number of patch faces.

TODO:
      - At the moment it is just checked if the center of each patch face is inside the bounding box of
        domain Two. If this is true the relative velocity is computed with the angular velocity vector
        of domain Two. Maybe it is reasonable to introduce a blending / interpolation near the cutting line
        of the two domains.

Author
    1991-2008 OpenCFD Ltd.
    2009 Oliver Borm <oli.borm@web.de>

\*---------------------------------------------------------------------------*/

#ifndef rotatingSplitVelocityFvPatchVectorField_H
#define rotatingSplitVelocityFvPatchVectorField_H

#include "fvPatchFields.H"
#include "fixedValueFvPatchFields.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                 Class rotatingSplitVelocityFvPatch Declaration
\*---------------------------------------------------------------------------*/

class rotatingSplitVelocityFvPatchVectorField
:
    public fixedValueFvPatchVectorField
{
    // Private data

        //- Inlet value
        vector absoluteValue_;

        //- Minimum Z Position
        scalar minZ_;

        //- Maximum Z Position
        scalar maxZ_;

        //- Minimum R Position
        scalar minR_;

        //- Maximum R Position
        scalar maxR_;

        //- Angular velocity of One
        vector omegaOne_;

        //- Angular velocity of Two
        vector omegaTwo_;

public:

    //- Runtime type information
    TypeName("rotatingSplitVelocity");


    // Constructors

        //- Construct from patch and internal field
        rotatingSplitVelocityFvPatchVectorField
        (
            const fvPatch&,
            const DimensionedField<vector, volMesh>&
        );

        //- Construct from patch, internal field and dictionary
        rotatingSplitVelocityFvPatchVectorField
        (
            const fvPatch&,
            const DimensionedField<vector, volMesh>&,
            const dictionary&
        );

        //- Construct by mapping given
        //  rotatingSplitVelocityFvPatchVectorField
        //  onto a new patch
        rotatingSplitVelocityFvPatchVectorField
        (
            const rotatingSplitVelocityFvPatchVectorField&,
            const fvPatch&,
            const DimensionedField<vector, volMesh>&,
            const fvPatchFieldMapper&
        );

        //- Construct as copy
        rotatingSplitVelocityFvPatchVectorField
        (
            const rotatingSplitVelocityFvPatchVectorField&
        );

        //- Construct and return a clone
        virtual tmp<fvPatchVectorField> clone() const
        {
            return tmp<fvPatchVectorField>
            (
                new rotatingSplitVelocityFvPatchVectorField(*this)
            );
        }

        //- Construct as copy setting internal field reference
        rotatingSplitVelocityFvPatchVectorField
        (
            const rotatingSplitVelocityFvPatchVectorField&,
            const DimensionedField<vector, volMesh>&
        );

        //- Construct and return a clone setting internal field reference
        virtual tmp<fvPatchVectorField> clone
        (
            const DimensionedField<vector, volMesh>& iF
        ) const
        {
            return tmp<fvPatchVectorField>
            (
                new rotatingSplitVelocityFvPatchVectorField
                (
                    *this,
                    iF
                )
            );
        }



    // Member functions

        //- Update the coefficients associated with the patch field
        virtual void updateCoeffs();

        //- Write
        virtual void write(Ostream&) const;
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
