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

#include "rotatingSplitVelocityFvPatchVectorField.H"
#include "addToRunTimeSelectionTable.H"
#include "fvPatchFieldMapper.H"
#include "volFields.H"
#include "surfaceFields.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

rotatingSplitVelocityFvPatchVectorField::
rotatingSplitVelocityFvPatchVectorField
(
    const fvPatch& p,
    const DimensionedField<vector, volMesh>& iF
)
:
    fixedValueFvPatchVectorField(p, iF),
    absoluteValue_(vector::zero),
    minZ_(0.0),
    maxZ_(0.0),
    minR_(0.0),
    maxR_(0.0),
    omegaOne_(vector::zero),
    omegaTwo_(vector::zero)
{}


rotatingSplitVelocityFvPatchVectorField::
rotatingSplitVelocityFvPatchVectorField
(
    const rotatingSplitVelocityFvPatchVectorField& ptf,
    const fvPatch& p,
    const DimensionedField<vector, volMesh>& iF,
    const fvPatchFieldMapper& mapper
)
:
    fixedValueFvPatchVectorField(ptf, p, iF, mapper),
    absoluteValue_(ptf.absoluteValue_),
    minZ_(ptf.minZ_),
    maxZ_(ptf.maxZ_),
    minR_(ptf.minR_),
    maxR_(ptf.maxR_),
    omegaOne_(ptf.omegaOne_),
    omegaTwo_(ptf.omegaTwo_)
{}


rotatingSplitVelocityFvPatchVectorField::
rotatingSplitVelocityFvPatchVectorField
(
    const fvPatch& p,
    const DimensionedField<vector, volMesh>& iF,
    const dictionary& dict
)
:
    fixedValueFvPatchVectorField(p, iF),
    absoluteValue_(dict.lookup("absoluteValue")),
    minZ_(readScalar(dict.lookup("minZ"))),
    maxZ_(readScalar(dict.lookup("maxZ"))),
    minR_(readScalar(dict.lookup("minR"))),
    maxR_(readScalar(dict.lookup("maxR"))),
    omegaOne_(dict.lookup("omegaOne")),
    omegaTwo_(dict.lookup("omegaTwo"))
{
    fvPatchVectorField::operator=(vectorField("value", dict, p.size()));
}


rotatingSplitVelocityFvPatchVectorField::
rotatingSplitVelocityFvPatchVectorField
(
    const rotatingSplitVelocityFvPatchVectorField& pivpvf
)
:
    fixedValueFvPatchVectorField(pivpvf),
    absoluteValue_(pivpvf.absoluteValue_),
    minZ_(pivpvf.minZ_),
    maxZ_(pivpvf.maxZ_),
    minR_(pivpvf.minR_),
    maxR_(pivpvf.maxR_),
    omegaOne_(pivpvf.omegaOne_),
    omegaTwo_(pivpvf.omegaTwo_)
{}


rotatingSplitVelocityFvPatchVectorField::
rotatingSplitVelocityFvPatchVectorField
(
    const rotatingSplitVelocityFvPatchVectorField& pivpvf,
    const DimensionedField<vector, volMesh>& iF
)
:
    fixedValueFvPatchVectorField(pivpvf, iF),
    absoluteValue_(pivpvf.absoluteValue_),
    minZ_(pivpvf.minZ_),
    maxZ_(pivpvf.maxZ_),
    minR_(pivpvf.minR_),
    maxR_(pivpvf.maxR_),
    omegaOne_(pivpvf.omegaOne_),
    omegaTwo_(pivpvf.omegaTwo_)
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

void rotatingSplitVelocityFvPatchVectorField::updateCoeffs()
{
    if (updated())
    {
        return;
    }

    const vectorField& C = patch().Cf();

    vectorField rotationVelocity = absoluteValue_ - ( omegaOne_ ^ C );

    forAll(C, facei)
     {
       scalar r = sqrt(C[facei].y()*C[facei].y() + C[facei].x()*C[facei].x());

       if ( ( C[facei].z() > minZ_ ) and ( C[facei].z() < maxZ_ ) and ( r > minR_ ) and ( r < maxR_ ) )
       {
          rotationVelocity[facei] = absoluteValue_ - ( omegaTwo_ ^ C[facei] );
       }
     }

    operator==(rotationVelocity);

    fixedValueFvPatchVectorField::updateCoeffs();
}


void rotatingSplitVelocityFvPatchVectorField::write(Ostream& os) const
{
    fvPatchVectorField::write(os);
    os.writeKeyword("absoluteValue")<< absoluteValue_ << token::END_STATEMENT << nl;
    os.writeKeyword("minZ")<< minZ_ << token::END_STATEMENT << nl;
    os.writeKeyword("maxZ")<< maxZ_ << token::END_STATEMENT << nl;
    os.writeKeyword("minR")<< minR_ << token::END_STATEMENT << nl;
    os.writeKeyword("maxR")<< maxR_ << token::END_STATEMENT << nl;
    os.writeKeyword("omegaOne")<< omegaOne_ << token::END_STATEMENT << nl;
    os.writeKeyword("omegaTwo")<< omegaTwo_ << token::END_STATEMENT << nl;
    writeEntry("value", os);
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

makePatchTypeField
(
    fvPatchVectorField,
    rotatingSplitVelocityFvPatchVectorField
);


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// ************************************************************************* //
