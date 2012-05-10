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
    Foam::SRFZones

Description
    Container class for a set of SRFZones with the SRFZone member functions
    implemented to loop over the functions for each SRFZone.

SourceFiles
    SRFZones.C

Author
    1991-2008 OpenCFD Ltd.
    2009 Oliver Borm <oli.borm@web.de>

\*---------------------------------------------------------------------------*/

#ifndef SRFZones_H
#define SRFZones_H

#include "SRFZone.H"
#include "IOPtrList.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class SRFZones Declaration
\*---------------------------------------------------------------------------*/

class SRFZones
:
    public IOPtrList<SRFZone>
{

    // Private Member Functions

        //- Disallow default bitwise copy construct
        SRFZones(const SRFZones&);

        //- Disallow default bitwise assignment
        void operator=(const SRFZones&);


public:

    // Constructors

        //- Construct from fvMesh
        SRFZones(const fvMesh& mesh);

    // Member Functions

            //- Add the Coriolis force contribution to the momentum equation
            void addCoriolis(fvVectorMatrix& WEqn) const;

            //- Add the Centrifugal  force contribution to the energy equation
            void addCentrifugal(fvScalarMatrix& hEqn) const;

            //- Add the Coriolis and Centrifugal force contribution to the momentum equation
            void addSu(fvVectorMatrix& WEqn) const;
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
