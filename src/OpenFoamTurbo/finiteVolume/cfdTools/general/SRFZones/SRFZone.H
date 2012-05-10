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
    Foam::SRFZone

Description
    Computes the additional fictitious force in a rotating relative coordinate system.
    The Coriolis and Centrifugal force for the momentum equation and the Centrifugal force
    for the energy equation.

    It is checked if the name of the density field is applied.

SourceFiles
    SRFZone.C

Author
    1991-2008 OpenCFD Ltd.
    2009 Oliver Borm <oli.borm@web.de>

\*---------------------------------------------------------------------------*/

#ifndef SRFZone_H
#define SRFZone_H

#include "dictionary.H"
#include "wordList.H"
#include "labelList.H"
#include "dimensionedScalar.H"
#include "dimensionedVector.H"
#include "volFieldsFwd.H"
#include "surfaceFieldsFwd.H"
#include "fvMatricesFwd.H"
#include "fvMatrices.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

// Forward declaration of classes
class fvMesh;

/*---------------------------------------------------------------------------*\
                           Class SRFZone Declaration
\*---------------------------------------------------------------------------*/

class SRFZone
{
    // Private data

        const fvMesh& mesh_;

        const word name_;

        const dictionary dict_;

        label cellZoneID_;

        dimensionedVector axis_;
        const dimensionedScalar omega_;
        dimensionedVector Omega_;

        //- Name of the compressibility field
        word rhoName_;

        //- Name of the field of the relative velocity
        word WName_;

    // Private Member Functions

        //- Disallow default bitwise copy construct
        SRFZone(const SRFZone&);

        //- Disallow default bitwise assignment
        void operator=(const SRFZone&);


public:

    // Constructors

        //- Construct from fvMesh and Istream
        SRFZone(const fvMesh& mesh, Istream& is);

        //- Return clone
        autoPtr<SRFZone> clone() const
        {
            notImplemented("autoPtr<SRFZone> clone() const");
            return autoPtr<SRFZone>(NULL);
        }

        //- Return a pointer to a new SRFZone created on freestore
        //  from Istream
        class iNew
        {
            const fvMesh& mesh_;

        public:

            iNew(const fvMesh& mesh)
            :
                mesh_(mesh)
            {}

            autoPtr<SRFZone> operator()(Istream& is) const
            {
                return autoPtr<SRFZone>(new SRFZone(mesh_, is));
            }
        };


    // Member Functions

        //- Update the mesh corresponding to given map

        //- Add the Coriolis force contribution
        void addCoriolis(fvVectorMatrix& WEqn) const;

        //- Make the given absolute flux relative within the SRF region
        void addCentrifugal(fvScalarMatrix& hEqn) const;

        //- Source term component
        void addSu(fvVectorMatrix& WEqn) const;

    // Ostream Operator

        friend Ostream& operator<<(Ostream& os, const SRFZone&)
        {
            notImplemented("Ostream& operator<<(Ostream& os, const SRFZone&)");
            return os;
        }
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
