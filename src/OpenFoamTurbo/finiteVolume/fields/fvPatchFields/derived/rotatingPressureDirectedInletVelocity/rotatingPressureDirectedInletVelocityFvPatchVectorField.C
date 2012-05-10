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

#include "rotatingPressureDirectedInletVelocityFvPatchVectorField.H"
#include "addToRunTimeSelectionTable.H"
#include "fvPatchFieldMapper.H"
#include "volFields.H"
#include "surfaceFields.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

rotatingPressureDirectedInletVelocityFvPatchVectorField::
rotatingPressureDirectedInletVelocityFvPatchVectorField
(
    const fvPatch& p,
    const DimensionedField<vector, volMesh>& iF
)
:
    fixedValueFvPatchVectorField(p, iF),
    phiName_("phi"),
    rhoName_("rho"),
    inletDir_(vector::zero),
    cylindricalCCS_(0),
    omega_(vector::zero)
{}


rotatingPressureDirectedInletVelocityFvPatchVectorField::
rotatingPressureDirectedInletVelocityFvPatchVectorField
(
    const rotatingPressureDirectedInletVelocityFvPatchVectorField& ptf,
    const fvPatch& p,
    const DimensionedField<vector, volMesh>& iF,
    const fvPatchFieldMapper& mapper
)
:
    fixedValueFvPatchVectorField(ptf, p, iF, mapper),
    phiName_(ptf.phiName_),
    rhoName_(ptf.rhoName_),
    inletDir_(ptf.inletDir_),
    cylindricalCCS_(ptf.cylindricalCCS_),
    omega_(ptf.omega_)
{}


rotatingPressureDirectedInletVelocityFvPatchVectorField::
rotatingPressureDirectedInletVelocityFvPatchVectorField
(
    const fvPatch& p,
    const DimensionedField<vector, volMesh>& iF,
    const dictionary& dict
)
:
    fixedValueFvPatchVectorField(p, iF),
    phiName_(dict.lookupOrDefault<word>("phi", "phi")),
    rhoName_(dict.lookupOrDefault<word>("rho", "rho")),
    inletDir_(dict.lookup("inletDirection")),
    cylindricalCCS_(dict.lookup("cylindricalCCS")),
    omega_(dict.lookup("omega"))
{
    fvPatchVectorField::operator=(vectorField("value", dict, p.size()));
}


rotatingPressureDirectedInletVelocityFvPatchVectorField::
rotatingPressureDirectedInletVelocityFvPatchVectorField
(
    const rotatingPressureDirectedInletVelocityFvPatchVectorField& pivpvf
)
:
    fixedValueFvPatchVectorField(pivpvf),
    phiName_(pivpvf.phiName_),
    rhoName_(pivpvf.rhoName_),
    inletDir_(pivpvf.inletDir_),
    cylindricalCCS_(pivpvf.cylindricalCCS_),
    omega_(pivpvf.omega_)
{}


rotatingPressureDirectedInletVelocityFvPatchVectorField::
rotatingPressureDirectedInletVelocityFvPatchVectorField
(
    const rotatingPressureDirectedInletVelocityFvPatchVectorField& pivpvf,
    const DimensionedField<vector, volMesh>& iF
)
:
    fixedValueFvPatchVectorField(pivpvf, iF),
    phiName_(pivpvf.phiName_),
    rhoName_(pivpvf.rhoName_),
    inletDir_(pivpvf.inletDir_),
    cylindricalCCS_(pivpvf.cylindricalCCS_),
    omega_(pivpvf.omega_)
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

void rotatingPressureDirectedInletVelocityFvPatchVectorField::updateCoeffs()
{
    if (updated())
    {
        return;
    }

    const vectorField& C = patch().Cf();
    vectorField rotationVelocity = omega_ ^ C;
    vector axisHat = inletDir_/mag(inletDir_);
    vectorField inletDirComputation_(patch().size(),axisHat);
    scalar radius, cx, cy, cz;

    const surfaceScalarField& phi =
        db().lookupObject<surfaceScalarField>(phiName_);

    const fvsPatchField<scalar>& phip =
        patch().patchField<surfaceScalarField, scalar>(phi);

    vectorField n = patch().nf();

    if (cylindricalCCS_)
    {
     forAll(C, facei)
     {
       radius = sqrt(C[facei].y()*C[facei].y() + C[facei].x()*C[facei].x());
       cz = axisHat.z();

       if (radius > 0.0)
       {
          cx = (C[facei].x()*axisHat.x() - C[facei].y()*axisHat.y())/radius;
          cy = (C[facei].y()*axisHat.x() + C[facei].x()*axisHat.y())/radius;

          inletDirComputation_[facei] = vector(cx,cy,cz);
       }
       else
       {
          inletDirComputation_[facei] = vector(0.0,0.0,cz);
       }
     }
    }

    scalarField ndmagS = (n & inletDirComputation_)*patch().magSf();

    if (phi.dimensions() == dimVelocity*dimArea)
    {
        operator==(inletDirComputation_*phip/ndmagS - rotationVelocity);
    }
    else if (phi.dimensions() == dimDensity*dimVelocity*dimArea)
    {
        const fvPatchField<scalar>& rhop =
            patch().lookupPatchField<volScalarField, scalar>(rhoName_);

        operator==(inletDirComputation_*phip/(rhop*ndmagS) - rotationVelocity);
    }
    else
    {
        FatalErrorIn
        (
            "rotatingPressureDirectedInletVelocityFvPatchVectorField::updateCoeffs()"
        )   << "dimensions of phi are not correct"
            << "\n    on patch " << this->patch().name()
            << " of field " << this->dimensionedInternalField().name()
            << " in file " << this->dimensionedInternalField().objectPath()
            << exit(FatalError);
    }

    fixedValueFvPatchVectorField::updateCoeffs();
}


void rotatingPressureDirectedInletVelocityFvPatchVectorField::write(Ostream& os) const
{
    fvPatchVectorField::write(os);
    if (phiName_ != "phi")
    {
        os.writeKeyword("phi") << phiName_ << token::END_STATEMENT << nl;
    }
    if (rhoName_ != "rho")
    {
        os.writeKeyword("rho") << rhoName_ << token::END_STATEMENT << nl;
    }
    os.writeKeyword("inletDirection") << inletDir_ << token::END_STATEMENT << nl;
    os.writeKeyword("cylindricalCCS") << cylindricalCCS_ << token::END_STATEMENT << nl;
    os.writeKeyword("omega")<< omega_ << token::END_STATEMENT << nl;
    writeEntry("value", os);
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

makePatchTypeField
(
    fvPatchVectorField,
    rotatingPressureDirectedInletVelocityFvPatchVectorField
);


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// ************************************************************************* //
