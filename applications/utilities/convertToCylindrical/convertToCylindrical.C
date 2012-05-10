/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | Copyright (C) 1991-2010 OpenCFD Ltd.
     \\/     M anipulation  |
-------------------------------------------------------------------------------
License
    This file is part of OpenFOAM.

    OpenFOAM is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM.  If not, see <http://www.gnu.org/licenses/>.

Application
    convertToCylindrical

Description
    Converts a velocity field from Cartesian coordinates to cylindrical coordinates

Author
     Bryan Lewis, Penn State University
     Assembled from forum threads by Hrvoje Jasak and Hakkan Nilsson

Usage
     After the simulation has completed, run this application to convert the velocity field to 
     cylindrical coordinates (r,theta,z)

     Velocity field must be titled "U"

     The model must be oriented with the x-y plan at the r-theta plane
     and the z-axis must be the center axis of rotation

     If you need a different orientation, change the code at 75-76 and 170-172
 
\*---------------------------------------------------------------------------*/

#include "fvCFD.H"
#include "cylindricalCS.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

int main(int argc, char *argv[])
{
    timeSelector::addOptions();

    #include "setRootCase.H"
    #include "createTime.H"
    
    instantList timeDirs = timeSelector::select0(runTime, args);

    #include "createMesh.H"

    forAll(timeDirs, timeI)
    {
        runTime.setTime(timeDirs[timeI], timeI);
        Info<< "Time = " << runTime.timeName() << endl;
        mesh.readUpdate();
        // defining cylindrical coordinate system

        Info<< " creating cylindrical system (r, teta, z) from cartesian system with r = x and z = z" << endl;

        cylindricalCS cyl(
		"cylindricalCS", 
		point(0, 0, 0), 
		vector(0, 0, 1), 
		vector(1, 0, 0),
		false); //keyword false: Use radians since cos and sin work with radians

	//Read the vectors pointing from the origin of the mesh to the cell centers
	volVectorField cc
        (
            IOobject
            (
                "cc",
                runTime.timeName(),
                mesh,
                IOobject::NO_READ,
                IOobject::AUTO_WRITE
            ),
            mesh.C()
        );

	//The vector field cc will now be transformed into cylindrical coordinates
	volVectorField ccCyl
        (
               IOobject
               (
                  "ccCyl",
                  runTime.timeName(),
                  mesh,
                  IOobject::NO_READ,
                  IOobject::NO_WRITE
               ),
               mesh,
               vector (0,0,0)
        );
	ccCyl.internalField() = cyl.localVector(cc.internalField());
        forAll (ccCyl.boundaryField(), patchI)
        {
               ccCyl.boundaryField()[patchI] = cyl.localVector(cc.boundaryField()[patchI]);
        }
//	cc.write();
//	ccCyl.write();

	// Define theta
	volScalarField theta
            (
                IOobject
                (
                    "theta",
                    runTime.timeName(),
                    mesh,
                    IOobject::NO_READ
                ),
                ccCyl.component(vector::Y)
            );
//	theta.write();


	// Set up U
        IOobject Uheader
        (
            "U",
            runTime.timeName(),
            mesh,
            IOobject::MUST_READ 
        );


        if (Uheader.headerOk())   // Check that U exists
        {
            mesh.readUpdate();

            Info<< " Reading U" << endl;
            volVectorField U(Uheader, mesh);
	 
	    // Set up Ucyl 
	    volVectorField Ucyl
       	    (
          	IOobject
		(
		    "Ucyl",
                    runTime.timeName(),
                    mesh,
                    IOobject::NO_READ
//		    IOobject::AUTO_WRITE		    
               	),
		mesh,
		dimensionedVector
		(
		    "Ucyl",
		    dimensionSet(0,1,-1,0,0,0,0),
		    vector::zero
		)	
	    );

	    // transformation of velocity field U from cartesian -> cylindrical
	    Info<< " converting U" << endl;

	    Ucyl.replace(vector::X, (U.component(vector::X)*cos(theta))+(U.component(vector::Y)*sin(theta)));//Ur
	    Ucyl.replace(vector::Y, (-1*U.component(vector::X)*sin(theta))+(U.component(vector::Y)*cos(theta)));//Uteta
	    Ucyl.replace(vector::Z, U.component(vector::Z)); //Uz

	    Ucyl.write();         

	}
      	else
      	{
        	Info<< " No exisiting U field" << endl;
      	}
    }
    return 0;
}



// ************************************************************************* //
