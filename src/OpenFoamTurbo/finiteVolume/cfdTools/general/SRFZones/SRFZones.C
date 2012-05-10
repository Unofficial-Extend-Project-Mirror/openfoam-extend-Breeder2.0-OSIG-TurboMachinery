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

#include "SRFZones.H"
#include "Time.H"
#include "fvMesh.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
    defineTemplateTypeNameAndDebug(IOPtrList<SRFZone>, 0);
}

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::SRFZones::SRFZones(const fvMesh& mesh)
:
    IOPtrList<SRFZone>
    (
        IOobject
        (
            "SRFZones",
            mesh.time().constant(),
            mesh,
            IOobject::MUST_READ,
            IOobject::NO_WRITE
        ),
        SRFZone::iNew(mesh)
    )
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

void Foam::SRFZones::addCoriolis(fvVectorMatrix& WEqn) const
{
    forAll(*this, i)
    {
        operator[](i).addCoriolis(WEqn);
    }
}


void Foam::SRFZones::addCentrifugal(fvScalarMatrix& hEqn) const
{
    forAll(*this, i)
    {
        operator[](i).addCentrifugal(hEqn);
    }
}

void Foam::SRFZones::addSu(fvVectorMatrix& WEqn) const
{
    forAll(*this, i)
    {
        operator[](i).addSu(WEqn);
    }
}


// ************************************************************************* //
