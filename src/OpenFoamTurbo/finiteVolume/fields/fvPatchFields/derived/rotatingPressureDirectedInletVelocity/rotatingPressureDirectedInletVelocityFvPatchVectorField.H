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
     Foam::rotatingPressureDirectedInletVelocityFvPatchVectorField

Description
    Based on the direction of the absolute velocity vector and the
    angular velocity vector, the components of the relative velocity
    vector are determined. You can choose if you want to specify the
    inlet direction in cartesian or cylindrical coordinates.

    Example of the boundary condition specification:
    @verbatim
    inlet
    {
        type rotatingPressureDirectedInletVelocity;
        phi              phi; // Name of the flux field
        rho              rho; // Name of the density field
        inletDirection  (0 0 -1);           // direction of the absolute velocity vector C
        // in case of cartesian coordinates it is defined as:   ( Cx/|C| Cy/|C| Cz/|C| );
        // in case of cylindrical coordinates it is defined as: ( Cr/|C| Cu/|C| Cz/|C| );
        cylindricalCCS  yes;                        // Switch between cartesian and cylindrical coordinates
        omega           (0 0 500.0);                // angular velocity vector
        value           uniform (0 0 -10);          // Initial Value
    }
    @endverbatim

SourceFiles
    rotatingPressureDirectedInletVelocityFvPatchVectorField.C

Author
    1991-2008 OpenCFD Ltd.
    2009 Oliver Borm <oli.borm@web.de>

\*---------------------------------------------------------------------------*/

#ifndef rotatingPressureDirectedInletVelocityFvPatchVectorField_H
#define rotatingPressureDirectedInletVelocityFvPatchVectorField_H

#include "fvPatchFields.H"
#include "fixedValueFvPatchFields.H"
#include "Switch.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                 Class rotatingPressureDirectedInletVelocityFvPatch Declaration
\*---------------------------------------------------------------------------*/

class rotatingPressureDirectedInletVelocityFvPatchVectorField
:
    public fixedValueFvPatchVectorField
{
    // Private data
        //- Name of the flux field
        word phiName_;

        //- Name of the density field
        word rhoName_;

        //- Normalized velocity direction
        vector inletDir_;

        //- Is the supplied inlet value in cartesian or cylindrical coordinates?
        Switch cylindricalCCS_;

        //- Angular velocity of the frame
        vector omega_;

public:

    //- Runtime type information
    TypeName("rotatingPressureDirectedInletVelocity");


    // Constructors

        //- Construct from patch and internal field
        rotatingPressureDirectedInletVelocityFvPatchVectorField
        (
            const fvPatch&,
            const DimensionedField<vector, volMesh>&
        );

        //- Construct from patch, internal field and dictionary
        rotatingPressureDirectedInletVelocityFvPatchVectorField
        (
            const fvPatch&,
            const DimensionedField<vector, volMesh>&,
            const dictionary&
        );

        //- Construct by mapping given
        //  rotatingPressureDirectedInletVelocityFvPatchVectorField
        //  onto a new patch
        rotatingPressureDirectedInletVelocityFvPatchVectorField
        (
            const rotatingPressureDirectedInletVelocityFvPatchVectorField&,
            const fvPatch&,
            const DimensionedField<vector, volMesh>&,
            const fvPatchFieldMapper&
        );

        //- Construct as copy
        rotatingPressureDirectedInletVelocityFvPatchVectorField
        (
            const rotatingPressureDirectedInletVelocityFvPatchVectorField&
        );

        //- Construct and return a clone
        virtual tmp<fvPatchVectorField> clone() const
        {
            return tmp<fvPatchVectorField>
            (
                new rotatingPressureDirectedInletVelocityFvPatchVectorField(*this)
            );
        }

        //- Construct as copy setting internal field reference
        rotatingPressureDirectedInletVelocityFvPatchVectorField
        (
            const rotatingPressureDirectedInletVelocityFvPatchVectorField&,
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
                new rotatingPressureDirectedInletVelocityFvPatchVectorField
                (
                    *this,
                    iF
                )
            );
        }


    // Member functions

        // Access

            //- Return the name of rho
            const word& rhoName() const
            {
                return rhoName_;
            }

            //- Return reference to the name of rho to allow adjustment
            word& rhoName()
            {
                return rhoName_;
            }

            //- Return the name of phi
            const word& phiName() const
            {
                return phiName_;
            }

            //- Return reference to the name of phi to allow adjustment
            word& phiName()
            {
                return phiName_;
            }

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
