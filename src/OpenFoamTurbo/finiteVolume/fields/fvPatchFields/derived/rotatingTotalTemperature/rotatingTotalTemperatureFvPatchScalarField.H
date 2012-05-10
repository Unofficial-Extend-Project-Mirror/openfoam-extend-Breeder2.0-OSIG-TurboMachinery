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
    Foam::rotatingTotalTemperatureFvPatchScalarField

Description
    Computes from the totaltemperature in the absolute frame of reference
    and the relative velocity field in the relative frame of reference with
    the angular velocity vector, the static temperatur.

    Example of the boundary condition specification:
    @verbatim
    inlet
    {
        type      rotatingTotalTemperature;
        U         Urel;             // Name of the relative velocity field
        phi       phi;              // Name of the phi field
        psi       psi;              // Name of the psi field
        T0        uniform 288.15;   // absolute totaltemperature value
        gamma     1.4;              // heat capacity ratio
        omega (0 0 500.0);          // angular velocity vector
    }
    @endverbatim

SourceFiles
    rotatingTotalTemperatureFvPatchScalarField.C

Author
    1991-2008 OpenCFD Ltd.
    2009 Oliver Borm <oli.borm@web.de>

\*---------------------------------------------------------------------------*/

#ifndef rotatingTotalTemperatureFvPatchScalarField_H
#define rotatingTotalTemperatureFvPatchScalarField_H

#include "totalTemperatureFvPatchScalarField.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class rotatingTotalTemperatureFvPatch Declaration
\*---------------------------------------------------------------------------*/

class rotatingTotalTemperatureFvPatchScalarField
:
    public totalTemperatureFvPatchScalarField
{
    // Private data

        //- Angular velocity of the frame
        vector omega_;


public:

    //- Runtime type information
    TypeName("rotatingTotalTemperature");


    // Constructors

        //- Construct from patch and internal field
        rotatingTotalTemperatureFvPatchScalarField
        (
            const fvPatch&,
            const DimensionedField<scalar, volMesh>&
        );

        //- Construct from patch, internal field and dictionary
        rotatingTotalTemperatureFvPatchScalarField
        (
            const fvPatch&,
            const DimensionedField<scalar, volMesh>&,
            const dictionary&
        );

        //- Construct by mapping given rotatingTotalTemperatureFvPatchScalarField
        //  onto a new patch
        rotatingTotalTemperatureFvPatchScalarField
        (
            const rotatingTotalTemperatureFvPatchScalarField&,
            const fvPatch&,
            const DimensionedField<scalar, volMesh>&,
            const fvPatchFieldMapper&
        );

        //- Construct as copy
        rotatingTotalTemperatureFvPatchScalarField
        (
            const rotatingTotalTemperatureFvPatchScalarField&
        );

        //- Construct and return a clone
        virtual tmp<fvPatchScalarField> clone() const
        {
            return tmp<fvPatchScalarField>
            (
                new rotatingTotalTemperatureFvPatchScalarField(*this)
            );
        }

        //- Construct as copy setting internal field reference
        rotatingTotalTemperatureFvPatchScalarField
        (
            const rotatingTotalTemperatureFvPatchScalarField&,
            const DimensionedField<scalar, volMesh>&
        );

        //- Construct and return a clone setting internal field reference
        virtual tmp<fvPatchScalarField> clone
        (
            const DimensionedField<scalar, volMesh>& iF
        ) const
        {
            return tmp<fvPatchScalarField>
            (
                new rotatingTotalTemperatureFvPatchScalarField(*this, iF)
            );
        }


    // Member functions

        // Access

            //- Return the angular velocity of rotation
            const vector& omega() const
            {
                return omega_;
            }

            //- Return the angular velocity of rotation
            vector& omega()
            {
                return omega_;
            }


        // Evaluation functions

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
