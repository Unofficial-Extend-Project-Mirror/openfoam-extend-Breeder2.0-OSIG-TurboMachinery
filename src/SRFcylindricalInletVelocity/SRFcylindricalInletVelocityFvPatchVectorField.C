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

\*---------------------------------------------------------------------------*/

#include "SRFcylindricalInletVelocityFvPatchVectorField.H"
#include "volFields.H"
#include "addToRunTimeSelectionTable.H"
#include "fvPatchFieldMapper.H"
#include "surfaceFields.H"
#include "mathematicalConstants.H"
#include "SRFModel.H"

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::
SRFcylindricalInletVelocityFvPatchVectorField::
SRFcylindricalInletVelocityFvPatchVectorField
(
    const fvPatch& p,
    const DimensionedField<vector, volMesh>& iF
)
:
    fixedValueFvPatchField<vector>(p, iF),
    relative_(0),
    axis_(pTraits<vector>::zero),
    centre_(pTraits<vector>::zero),
    radialVelocity_(0),
    tangentVelocity_(0),
    axialVelocity_(0)
{}


Foam::
SRFcylindricalInletVelocityFvPatchVectorField::
SRFcylindricalInletVelocityFvPatchVectorField
(
    const SRFcylindricalInletVelocityFvPatchVectorField& ptf,
    const fvPatch& p,
    const DimensionedField<vector, volMesh>& iF,
    const fvPatchFieldMapper& mapper
)
:
    fixedValueFvPatchField<vector>(ptf, p, iF, mapper),
    relative_(ptf.relative_),
    axis_(ptf.axis_),
    centre_(ptf.centre_),
    radialVelocity_(ptf.radialVelocity_),
    tangentVelocity_(ptf.tangentVelocity_),
    axialVelocity_(ptf.axialVelocity_)
{}


Foam::
SRFcylindricalInletVelocityFvPatchVectorField::
SRFcylindricalInletVelocityFvPatchVectorField
(
    const fvPatch& p,
    const DimensionedField<vector, volMesh>& iF,
    const dictionary& dict
)
:
    fixedValueFvPatchField<vector>(p, iF, dict),
    relative_(dict.lookup("relative")),
    axis_(dict.lookup("axis")),
    centre_(dict.lookup("centre")),
    radialVelocity_(readScalar(dict.lookup("radialVelocity"))),
    tangentVelocity_(readScalar(dict.lookup("tangentVelocity"))),
    axialVelocity_(readScalar(dict.lookup("axialVelocity")))
{}


Foam::
SRFcylindricalInletVelocityFvPatchVectorField::
SRFcylindricalInletVelocityFvPatchVectorField
(
    const SRFcylindricalInletVelocityFvPatchVectorField& ptf
)
:
    fixedValueFvPatchField<vector>(ptf),
    relative_(ptf.relative_),
    axis_(ptf.axis_),
    centre_(ptf.centre_),
    radialVelocity_(ptf.radialVelocity_),
    tangentVelocity_(ptf.tangentVelocity_),
    axialVelocity_(ptf.axialVelocity_)
{}


Foam::
SRFcylindricalInletVelocityFvPatchVectorField::
SRFcylindricalInletVelocityFvPatchVectorField
(
    const SRFcylindricalInletVelocityFvPatchVectorField& ptf,
    const DimensionedField<vector, volMesh>& iF
)
:
    fixedValueFvPatchField<vector>(ptf, iF),
    relative_(ptf.relative_),
    axis_(ptf.axis_),
    centre_(ptf.centre_),
    radialVelocity_(ptf.radialVelocity_),
    tangentVelocity_(ptf.tangentVelocity_),
    axialVelocity_(ptf.axialVelocity_)
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

void Foam::SRFcylindricalInletVelocityFvPatchVectorField::updateCoeffs()
{
    if (updated())
    {
        return;
    }

    vector hatAxis = axis_/mag(axis_); // unit vector of axis of rotation

    vectorField r = (patch().Cf() - centre_); // defining vector origin to face centre

    vectorField d =  r - (hatAxis & r)*hatAxis; // subtract out axial-component

    vectorField dhat = d/mag(d); // create unit-drection vector

    vectorField tangVelo =  (tangentVelocity_)*(hatAxis)^dhat; // tangentialVelocity * zhat X rhat

    // If relative, include the effect of the SRF
    if (relative_)
    {
        // Get reference to the SRF Model
	const SRF::SRFModel& srf =
	     db().lookupObject<SRF::SRFModel>("SRFProperties");

        // Determine patch velocity due to SRF
	const vectorField SRFVelocity = srf.velocity(patch().Cf());

	operator==(-SRFVelocity + tangVelo + hatAxis*axialVelocity_ + radialVelocity_*dhat); // combine components into vector form
    }
    // If absolute, simply supply the inlet value as a fixed value
    else
    {
        operator==(tangVelo + hatAxis*axialVelocity_ + radialVelocity_*dhat);
    }

    fixedValueFvPatchField<vector>::updateCoeffs();
}


void Foam::SRFcylindricalInletVelocityFvPatchVectorField::write(Ostream& os) const
{
    fvPatchField<vector>::write(os);
    os.writeKeyword("relative") << relative_ << token::END_STATEMENT << nl;
    os.writeKeyword("axis") << axis_ << token::END_STATEMENT << nl;
    os.writeKeyword("centre") << centre_ << token::END_STATEMENT << nl;
    os.writeKeyword("radialVelocity") << radialVelocity_ << token::END_STATEMENT << nl;
    os.writeKeyword("tangentVelocity") << tangentVelocity_ << token::END_STATEMENT << nl;
    os.writeKeyword("axialVelocity") << axialVelocity_ << token::END_STATEMENT << nl;
    writeEntry("value", os);
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
   makePatchTypeField
   (
       fvPatchVectorField,
       SRFcylindricalInletVelocityFvPatchVectorField
   );
}


// ************************************************************************* //
