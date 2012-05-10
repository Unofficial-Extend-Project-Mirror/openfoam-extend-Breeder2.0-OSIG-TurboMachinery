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

#include "SRFZone.H"
#include "fvMesh.H"
#include "volFields.H"
#include "surfaceFields.H"
#include "fvMatrices.H"
#include "syncTools.H"

// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::SRFZone::SRFZone(const fvMesh& mesh, Istream& is)
:
    mesh_(mesh),
    name_(is),
    dict_(is),
    cellZoneID_(mesh_.cellZones().findZoneID(name_)),
    axis_(dict_.lookup("axis")),
    omega_(dict_.lookup("omega")),
    Omega_("Omega", omega_*axis_),
    rhoName_(dict_.lookup("rho")),
    WName_(dict_.lookup("Wxyz"))
{
    axis_ = axis_/mag(axis_);
    Omega_ = omega_*axis_;

    bool cellZoneFound = (cellZoneID_ != -1);
    reduce(cellZoneFound, orOp<bool>());

    if (!cellZoneFound)
    {
        FatalErrorIn
        (
            "Foam::SRFZone::SRFZone(const fvMesh&, Istream&)"
        )   << "cannot find SRF cellZone " << name_
            << exit(FatalError);
    }

}

// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

// Coriolis force
void Foam::SRFZone::addCoriolis(fvVectorMatrix& WEqn) const
{
    if (cellZoneID_ == -1)
    {
        return;
    }

    const labelList& cells = mesh_.cellZones()[cellZoneID_];
    const scalarField& V = mesh_.V();
    vectorField& Wsource = WEqn.source();
    const vectorField& Wxyz = WEqn.psi();
    const vector& Omega = Omega_.value();

    if (rhoName_ == "none")
    {
        forAll(cells, i)
        {
            Wsource[cells[i]] -= 2.0 * V[cells[i]] * (Omega ^ Wxyz[cells[i]]);
        }
    }
    else
    {
        const volScalarField& rhop =
            mesh_.thisDb().lookupObject<volScalarField>(rhoName_);

        forAll(cells, i)
        {
            Wsource[cells[i]] -= 2.0 * V[cells[i]] * rhop[cells[i]] * (Omega ^ Wxyz[cells[i]]);
        }
    }
}


// Centrifugal force for the energy equation
void Foam::SRFZone::addCentrifugal(fvScalarMatrix& hEqn) const
{
    if (cellZoneID_ == -1)
    {
        return;
    }

    const labelList& cells = mesh_.cellZones()[cellZoneID_];
    const vectorField& C = mesh_.C();
    const scalarField& V = mesh_.V();
    scalarField& Hsource = hEqn.source();
    const vector& Omega = Omega_.value();

    const vectorField& Wxyz =
        mesh_.thisDb().objectRegistry::lookupObject<vectorField>(WName_);

    if (rhoName_ == "none")
    {
        forAll(cells, i)
        {
            Hsource[cells[i]] -= V[cells[i]]
                * ( ( Omega ^ ( Omega ^ C[cells[i]] ) ) & Wxyz[cells[i]] );
        }
    }
    else
    {
        const scalarField& rhop =
            mesh_.thisDb().objectRegistry::lookupObject<scalarField>(rhoName_);

        forAll(cells, i)
        {
            Hsource[cells[i]] -= V[cells[i]] * rhop[cells[i]]
                * ( ( Omega ^ ( Omega ^ C[cells[i]] ) ) & Wxyz[cells[i]] );
        }
    }
}

// Coriolis and Centrifugal force for the momentum equation
void Foam::SRFZone::addSu(fvVectorMatrix& WEqn) const
{
    if (cellZoneID_ == -1)
    {
        return;
    }

    const labelList& cells = mesh_.cellZones()[cellZoneID_];
    const scalarField& V = mesh_.V();
    const vectorField& C = mesh_.C();
    vectorField& Wsource = WEqn.source();
    const vectorField& Wxyz = WEqn.psi();
    const vector& Omega = Omega_.value();

//     const vectorField& W =
//         mesh_.db().lookupObject<vectorField>(WName_);

    if (rhoName_ == "none")
    {
        forAll(cells, i)
        {
            Wsource[cells[i]] -= V[cells[i]] * ( 2.0 * ( Omega ^ Wxyz[cells[i]] )
                            + ( Omega ^ ( Omega ^ C[cells[i]] ) ) );

//             Info << W[cells[i]] << "\t" << Wxyz[cells[i]] << endl;
        }
    }
    else
    {
        const scalarField& rhop =
            mesh_.thisDb().lookupObject<scalarField>(rhoName_);

        forAll(cells, i)
        {
            Wsource[cells[i]] -= V[cells[i]] * rhop[cells[i]] * ( 2.0 * ( Omega ^ Wxyz[cells[i]] )
                            + ( Omega ^ ( Omega ^ C[cells[i]] ) ) );
        }
    }
}


// ************************************************************************* //
