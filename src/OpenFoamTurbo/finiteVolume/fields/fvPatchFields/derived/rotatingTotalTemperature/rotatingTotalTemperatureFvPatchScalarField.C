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

Author
    1991-2008 OpenCFD Ltd.
    2009 Oliver Borm <oli.borm@web.de>

\*---------------------------------------------------------------------------*/

#include "rotatingTotalTemperatureFvPatchScalarField.H"
#include "addToRunTimeSelectionTable.H"
#include "fvPatchFieldMapper.H"
#include "volFields.H"
#include "surfaceFields.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

rotatingTotalTemperatureFvPatchScalarField::rotatingTotalTemperatureFvPatchScalarField
(
    const fvPatch& p,
    const DimensionedField<scalar, volMesh>& iF
)
:
    totalTemperatureFvPatchScalarField(p, iF),
    omega_(vector::zero)
{}


rotatingTotalTemperatureFvPatchScalarField::rotatingTotalTemperatureFvPatchScalarField
(
    const rotatingTotalTemperatureFvPatchScalarField& ptf,
    const fvPatch& p,
    const DimensionedField<scalar, volMesh>& iF,
    const fvPatchFieldMapper& mapper
)
:
    totalTemperatureFvPatchScalarField(ptf, p, iF, mapper),
    omega_(ptf.omega_)
{}


rotatingTotalTemperatureFvPatchScalarField::rotatingTotalTemperatureFvPatchScalarField
(
    const fvPatch& p,
    const DimensionedField<scalar, volMesh>& iF,
    const dictionary& dict
)
:
    totalTemperatureFvPatchScalarField(p, iF, dict),
    omega_(dict.lookup("omega"))
{}


rotatingTotalTemperatureFvPatchScalarField::rotatingTotalTemperatureFvPatchScalarField
(
    const rotatingTotalTemperatureFvPatchScalarField& tppsf
)
:
    totalTemperatureFvPatchScalarField(tppsf),
    omega_(tppsf.omega_)
{}


rotatingTotalTemperatureFvPatchScalarField::rotatingTotalTemperatureFvPatchScalarField
(
    const rotatingTotalTemperatureFvPatchScalarField& tppsf,
    const DimensionedField<scalar, volMesh>& iF
)
:
    totalTemperatureFvPatchScalarField(tppsf, iF),
    omega_(tppsf.omega_)
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

void rotatingTotalTemperatureFvPatchScalarField::updateCoeffs()
{
    if (updated())
    {
        return;
    }

    vectorField rotationVelocity = omega_ ^ patch().Cf();

    vectorField Up = patch().lookupPatchField<volVectorField, vector>(UName())
        + rotationVelocity;

    totalTemperatureFvPatchScalarField::updateCoeffs(Up);
}


void rotatingTotalTemperatureFvPatchScalarField::write(Ostream& os) const
{
    totalTemperatureFvPatchScalarField::write(os);
    os.writeKeyword("omega")<< omega_ << token::END_STATEMENT << nl;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

makePatchTypeField
(
    fvPatchScalarField,
    rotatingTotalTemperatureFvPatchScalarField
);

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// ************************************************************************* //
