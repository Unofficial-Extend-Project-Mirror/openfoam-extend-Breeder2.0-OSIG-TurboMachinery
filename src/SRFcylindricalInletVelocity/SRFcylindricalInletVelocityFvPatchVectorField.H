/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | Copyright (C) 2010-2010 OpenCFD Ltd.
     \\/     M anipulation  |
-------------------------------------------------------------------------------
License
    This file is part of OpenFOAM.

    OpenFOAM is free software; you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by the
    Free Software Foundation; either version 3 of the License, or (at your
    option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM; if not, write to the Free Software Foundation,
    Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

Class
    Foam::cylindricalInletVelocityFvPatchVectorField

Description
    Describes an inlet vector boundary condition in cylindrical coordinates
    given a central axis, central point, rpm, axial and radial velocity.

    Example of the boundary condition specification:
    @verbatim
    inlet
    {
        type            SRFcylindricalInletVelocity;
	relative	yes;
	axis            (0 0 1);
        centre          (0 0 0);
        radialVelocity  -10;
	tangentVelocity 10;
	axialVelocity   30;
        value		(0 0 0);
    }
    @endverbatim


SourceFiles
    cylindricalInletVelocityFvPatchVectorField.C

Author
    Original code by OpenCFD
    Modified/corrected and extended by Alex Wouden, Penn State University

\*---------------------------------------------------------------------------*/

#ifndef SRFcylindricalInletVelocityFvPatchVectorField_H
#define SRFcylindricalInletVelocityFvPatchVectorField_H

#include "fixedValueFvPatchFields.H"
#include "Switch.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
/*---------------------------------------------------------------------------*\
         Class cylindricalInletVelocityFvPatchVectorField Declaration
\*---------------------------------------------------------------------------*/

class SRFcylindricalInletVelocityFvPatchVectorField
:
    public fixedValueFvPatchVectorField
{
    // Private data
   
        //- Is the supplied inlet value relative to the SRF
	Switch relative_;

	//- Axis
	const vector axis_;

        //- Central point
        const vector centre_;

	//- Radial velocity
	const scalar radialVelocity_;

        //- Tangent velocity
        const scalar tangentVelocity_;

        //- Axial velocity
        const scalar axialVelocity_;


public:

   //- Runtime type information
   TypeName("SRFcylindricalInletVelocity");


   // Constructors

        //- Construct from patch and internal field
        SRFcylindricalInletVelocityFvPatchVectorField
        (
            const fvPatch&,
            const DimensionedField<vector, volMesh>&
        );

        //- Construct from patch, internal field and dictionary
        SRFcylindricalInletVelocityFvPatchVectorField
        (
            const fvPatch&,
            const DimensionedField<vector, volMesh>&,
            const dictionary&
        );

        //- Construct by mapping given
        //  flowRateInletVelocityFvPatchVectorField
        //  onto a new patch
        SRFcylindricalInletVelocityFvPatchVectorField
        (
            const SRFcylindricalInletVelocityFvPatchVectorField&,
            const fvPatch&,
            const DimensionedField<vector, volMesh>&,
            const fvPatchFieldMapper&
        );

        //- Construct as copy
        SRFcylindricalInletVelocityFvPatchVectorField
        (
            const SRFcylindricalInletVelocityFvPatchVectorField&
        );

        //- Construct and return a clone
        virtual tmp<fvPatchVectorField> clone() const
        {
            return tmp<fvPatchVectorField>
            (
                new SRFcylindricalInletVelocityFvPatchVectorField(*this)
            );
        }

        //- Construct as copy setting internal field reference
        SRFcylindricalInletVelocityFvPatchVectorField
        (
            const SRFcylindricalInletVelocityFvPatchVectorField&,
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
                new SRFcylindricalInletVelocityFvPatchVectorField(*this, iF)
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
