/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | Copyright (C) 1991-2008 OpenCFD Ltd.
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

\*---------------------------------------------------------------------------*/

#include "forces.H"
#include "volFields.H"
#include "dictionary.H"
#include "Time.H"

#include "incompressible/singlePhaseTransportModel/singlePhaseTransportModel.H"
#include "incompressible/RAS/RASModel/RASModel.H"
#include "incompressible/LES/LESModel/LESModel.H"
#include "basicThermo.H"
#include "compressible/RAS/RASModel/RASModel.H"
#include "compressible/LES/LESModel/LESModel.H"


// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
    defineTypeNameAndDebug(forces, 0);
}

// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //

Foam::tmp<Foam::volSymmTensorField> Foam::forces::devRhoReff() const
{
    if (obr_.foundObject<compressible::RASModel>("RASProperties"))
    {
        const compressible::RASModel& ras
            = obr_.lookupObject<compressible::RASModel>("RASProperties");

        return ras.devRhoReff();
    }
    else if (obr_.foundObject<incompressible::RASModel>("RASProperties"))
    {
        const incompressible::RASModel& ras
            = obr_.lookupObject<incompressible::RASModel>("RASProperties");

        return rhoRef_*ras.devReff();
    }
    else if (obr_.foundObject<compressible::LESModel>("LESProperties"))
    {
        const compressible::LESModel& les =
        obr_.lookupObject<compressible::LESModel>("LESProperties");

        return les.devRhoBeff();
    }
    else if (obr_.foundObject<incompressible::LESModel>("LESProperties"))
    {
        const incompressible::LESModel& les
            = obr_.lookupObject<incompressible::LESModel>("LESProperties");

        return rhoRef_*les.devBeff();
    }
    else if (obr_.foundObject<basicThermo>("thermophysicalProperties"))
    {
        const basicThermo& thermo =
             obr_.lookupObject<basicThermo>("thermophysicalProperties");

        const volVectorField& U = obr_.lookupObject<volVectorField>(Uname_);

        return -thermo.mu()*dev(twoSymm(fvc::grad(U)));
    }
    else if
    (
        obr_.foundObject<singlePhaseTransportModel>("transportProperties")
    )
    {
        const singlePhaseTransportModel& laminarT =
            obr_.lookupObject<singlePhaseTransportModel>
            ("transportProperties");

        const volVectorField& U = obr_.lookupObject<volVectorField>(Uname_);

        return -rhoRef_*laminarT.nu()*dev(twoSymm(fvc::grad(U)));
    }
    else if (obr_.foundObject<dictionary>("transportProperties"))
    {
        const dictionary& transportProperties =
             obr_.lookupObject<dictionary>("transportProperties");

        dimensionedScalar nu(transportProperties.lookup("nu"));

        const volVectorField& U = obr_.lookupObject<volVectorField>(Uname_);

        return -rhoRef_*nu*dev(twoSymm(fvc::grad(U)));
    }
    else
    {
        FatalErrorIn("forces::devRhoReff()")
            << "No valid model for viscous stress calculation."
            << exit(FatalError);

        return volSymmTensorField::null();
    }
}


Foam::scalar Foam::forces::rho(const volScalarField& p) const
{
    if (p.dimensions() == dimPressure)
    {
        return 1.0;
    }
    else
    {
        return rhoRef_;
    }
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::forces::forces
(
    const word& name,
    const objectRegistry& obr,
    const dictionary& dict,
    const bool loadFromFiles
)
:
    name_(name),
    obr_(obr),
    active_(true),
    log_(false),
    patchSet_(),
    pName_(""),
    Uname_(""),
    rhoRef_(0),
    CofR_(vector::zero),
    forcesFilePtr_(NULL)
{
    // Check if the available mesh is an fvMesh otherise deactivate
    if (!isA<fvMesh>(obr_))
    {
        active_ = false;
        WarningIn
        (
            "forces::forces(const objectRegistry& obr, const dictionary& dict)"
        )   << "No fvMesh available, deactivating."
            << endl;
    }

    read(dict);
}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

Foam::forces::~forces()
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

void Foam::forces::read(const dictionary& dict)
{
    if (active_)
    {
        log_ = dict.lookupOrDefault<Switch>("log", false);

        const fvMesh& mesh = refCast<const fvMesh>(obr_);

        patchSet_ =
            mesh.boundaryMesh().patchSet(wordReList(dict.lookup("patches")));

        // Optional entries U and p
        pName_ = dict.lookupOrDefault<word>("pName", "p");
        Uname_ = dict.lookupOrDefault<word>("Uname", "U");

        // Check whether Uname and pName exists, if not deactivate forces
        if
        (
            !obr_.foundObject<volVectorField>(Uname_)
         || !obr_.foundObject<volScalarField>(pName_)
        )
        {
            active_ = false;
            WarningIn("void forces::read(const dictionary& dict)")
                << "Could not find " << Uname_ << " or "
                << pName_ << " in database." << nl
                << "    De-activating forces."
                << endl;
        }

        // Reference density needed for incompressible calculations
        rhoRef_ = readScalar(dict.lookup("rhoInf"));

        // Centre of rotation for moment calculations
        CofR_ = dict.lookup("CofR");
    }
}


void Foam::forces::makeFile()
{
    // Create the forces file if not already created
    if (!forcesFilePtr_.valid())
    {
        if (debug)
        {
            Info<< "Creating forces file." << endl;
        }

        // File update
        if (Pstream::master())
        {
            fileName forcesDir;
            if (Pstream::parRun())
            {
                // Put in undecomposed case (Note: gives problems for
                // distributed data running)
                forcesDir =
                obr_.time().path()/".."/name_/obr_.time().timeName();
            }
            else
            {
                forcesDir = obr_.time().path()/name_/obr_.time().timeName();
            }

            // Create directory if does not exist.
            mkDir(forcesDir);

            // Open new file at start up
            forcesFilePtr_.reset(new OFstream(forcesDir/(type() + ".dat")));

            // Add headers to output data
            writeFileHeader();
        }
    }
}


void Foam::forces::writeFileHeader()
{
    if (forcesFilePtr_.valid())
    {
        forcesFilePtr_()
            << "# Time" << tab
            << "forces(pressure, viscous) moment(pressure, viscous)"
            << endl;
    }
}

void Foam::forces::execute()
{
    // Do nothing - only valid on write
}

void Foam::forces::end()
{
    // Do nothing - only valid on write
}

void Foam::forces::write()
{
    if (active_)
    {
        // Create the forces file if not already created
        makeFile();

        forcesMoments fm = calcForcesMoment();

        if (Pstream::master())
        {
            forcesFilePtr_() << obr_.time().value() << tab << fm << endl;

            if (log_)
            {
                Info<< "forces output:" << nl
                    << "    forces(pressure, viscous)" << fm.first() << nl
                    << "    moment(pressure, viscous)" << fm.second() << nl
                    << endl;
            }
        }
    }
}


Foam::forces::forcesMoments Foam::forces::calcForcesMoment() const
{
    const volVectorField& U = obr_.lookupObject<volVectorField>(Uname_);
    const volScalarField& p = obr_.lookupObject<volScalarField>(pName_);

    const fvMesh& mesh = U.mesh();

    forcesMoments fm
    (
        pressureViscous(vector::zero, vector::zero),
        pressureViscous(vector::zero, vector::zero)
    );

    const surfaceVectorField::GeometricBoundaryField& Sfb =
        mesh.Sf().boundaryField();

    tmp<volSymmTensorField> tdevRhoReff = devRhoReff();
    const volSymmTensorField::GeometricBoundaryField& devRhoReffb
        = tdevRhoReff().boundaryField();

    forAllConstIter(labelHashSet, patchSet_, iter)
    {
        label patchi = iter.key();

        vectorField Md = mesh.C().boundaryField()[patchi] - CofR_;

        vectorField pf =
            mesh.Sf().boundaryField()[patchi]*p.boundaryField()[patchi];

        fm.first().first() += rho(p)*sum(pf);
        fm.second().first() += rho(p)*sum(Md ^ pf);

        vectorField vf = Sfb[patchi] & devRhoReffb[patchi];

        fm.first().second() += sum(vf);
        fm.second().second() += sum(Md ^ vf);
    }

    reduce(fm, sumOp());

    return fm;
}


// ************************************************************************* //
