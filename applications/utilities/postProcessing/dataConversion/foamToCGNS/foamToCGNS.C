/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | Copyright Hydro-Quebec - IREQ, 2008
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
    Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA

Description
    Translates OpenFoam mesh and field data to CGNS format.

Authors 
  Martin Beaudoin, Hydro-Quebec - IREQ, 2005

\*---------------------------------------------------------------------------*/
// Activate CGNS code generation
# define _WRITE_CGNS_
#include <fstream>
#include <sstream>
#include <unistd.h>
#include "libcgnsoo3/cgnsoo.H"
#include "libcgnsoo3/file.H"

#include "argList.H"
#include "volPointInterpolation.H"
#include "IOobjectList.H"
#include "OFstream.H"
#include "typeInfo.H"
#include "vertexMappingOpenFoamCGNS.H"
#include "fieldsNameMappingOpenFoamCGNS.H"
#include "bcTypeMappingOpenFoamCGNS.H"
#include "readFields.H"
#include "cyclicPolyPatch.H"

#include "fvCFD.H"
#include "singlePhaseTransportModel.H"
//#include "RASModel.H"
//#include "kEpsilon.H"

#include "surfaceMesh.H"

#if OPENFOAM_VERSION>140
#include "scalarIOField.H"
#endif

#include "logFile.H"

#include "foamToCGNSDictionary.H"

#include <memory>


using namespace Foam;

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

int main(int argc, char *argv[])
{
	bool allow_userdefined_fields = false;
	
	argList::noParallel();
	
	argList::validOptions.insert("rho", "value");
	argList::validOptions.insert("allowuserdefinedfields", "");
#if defined EXPORT_CYCLIC_BOUNDARIES	
	argList::validOptions.insert("exportcyclics", "");
#endif	
        //	argList::validOptions.insert("help", "");
#if 0	
	const word fieldTypes[] =
	{
		volScalarField::typeName,
		volVectorField::typeName
	};
#endif

#   include "addTimeOptions.H"
#   include "setRootCase.H"

	if(args.options().found("help"))
	{
		args.printUsage();
		exit(0);
	}
#if defined EXPORT_CYCLIC_BOUNDARIES	
	bool exportcyclics = false;
	if(args.options().found("exportcyclics"))
	{
		exportcyclics=true;
	}
#endif

#   include "createTime.H"
#   include "createMesh.H"

	// Dictionnary for foamToCGNS options
#if 0	
	IOdictionary optionsDict
	(
		IOobject( "foamToCGNSDict", 
			  mesh.time().constant(), 
			  mesh,
			  IOobject::READ_IF_PRESENT, 
			  IOobject::NO_WRITE )
	);
	if ( optionsDict.headerOk() )
	{
		if ( optionsDict.found("SplitMixedCellTypes") )
			Info << "Options: Splitting multiple cell into different zones" << endl;
		if ( optionsDict.found("WriteConvergenceHistory") )
			Info << "Options: Writing convergence history found in 'log' file" << endl;
		if ( optionsDict.found("ConversionPath") )
		{
			const entry& e = optionsDict.lookupEntry( "ConversionPath" );
			Info << "Options: Conversion Path is " << e << endl;
		}
	}
#else
	foamToCGNSDictionary optionsDict
	(
		IOobject( "foamToCGNSDict", 
			  mesh.time().constant(), 
			  mesh,
			  IOobject::READ_IF_PRESENT, 
			  IOobject::NO_WRITE )
	);
	if ( optionsDict.headerOk() )
	{
		Info << "foamToCGNSOptions: " << endl;
		Info << "\tSplitting multiple cell into different zones : " << optionsDict.splitMixed() << endl;
		Info << "\tWriting convergence history found in 'log' file : " << optionsDict.writeConvergenceHistory() << endl;
		Info << "\tConversion Path is " << optionsDict.conversionDirectory() << endl;
		Info << "\tAllow User Defined Fields is " << optionsDict.allowUserDefinedFields() << endl;
	}
#endif
	
	if (args.options().found("allowuserdefinedfields") || optionsDict.allowUserDefinedFields() )
	{
		allow_userdefined_fields = true;
		Info << "User defined fields are allowed" << endl;
	}
	
	// Construct interpolation on the raw mesh
	volPointInterpolation pInterp(mesh);
	
	fileName cgnsDataPath(runTime.path()/"ConversionCGNS");
	mkDir(cgnsDataPath);
	
	// Get times list
	instantList Times = runTime.times();
	
	// set startTime and endTime depending on -time and -latestTime options
#   include "checkTimeOptions.H"

	// Get a value for the density 'rho' for purpose pour la mise a l'echelle du champs de pression
	//
	// First default value: 1.0
	//
	// Then, we check if rho is specified in the dictionary turboMathDict; if it is, we override the default value
	//
	// Finally, we allow the user to override the value of rho if the option -rho is provided on the commande line
	// or if the dictionnary is not present for this specific case.

	scalar rho_ = 1.0;  // Default value;
	Info << "Default value for rho: " << rho_ << endl;

#if 0	
	// Next we check for the dictionnary turboMathDict
	IOdictionary turboMathDict(
		IOobject
		(
			"turboMathDict",
			mesh.time().constant(),
			mesh, 
			IOobject::READ_IF_PRESENT,
			IOobject::NO_WRITE
		)
	);

	if(turboMathDict.headerOk())
	{
		dimensionedScalar dimensionedRho(turboMathDict.lookup("rho"));

		// Read the density value from the turboDictionnary
		rho_ = dimensionedRho.value();

		Info << "\nDictionary turboMathDict: new rho value : " << rho_ << endl;
	}
	else
	{
		Info << "Warning : no Dictionary turboMathDict" << endl;
	}
#endif
	// Finally, we allow this ultimate override from the command line
	if(args.options().found("rho"))
	{
		rho_ = readScalar(IStringStream(args.options()["rho"])());
		Info << "\nUser specified value for rho: new rho value : " << rho_ << endl;
	}
	Info << endl;
	
	// What kind of simulation is this?
	bool steadyState = false;

#if defined CGNSTOFOAM_EXTRACT_FlowEquationSet
	word application( runTime.controlDict().lookup( "application" ) );
	if ( application=="simpleFoam" )
	{
		Info << "Solution computed with simpleFoam" << endl;
		word default_ddtScheme = mesh.ddtScheme("default");
		Info << "\tddtScheme = " << default_ddtScheme << endl;
		steadyState = ( default_ddtScheme == "steadyState" );
		
		// what is the turbulence model ?

#include "incompressible/simpleFoam/createFields.H"
	}
	else if ( application=="icoFoam" )
	{
		Info << "Solution computed with icoFoam" << endl;
	}
	else
	{
		Info << "Warning : unknown application : " << application << endl;
		Info << "Warning : will not include FlowEquationSet in the CGNS file" << endl;
	}
#endif //CGNSTOFOAM_EXTRACT_FlowEquationSet

	
#if 1
	std::string logfilename = args.caseName() + "/log";
	std::ifstream ifslog( logfilename.c_str() );
	if ( !ifslog )
	{
		Info << "Warning : log file not found. No convergence history will be included." << endl;	
	}
	else
	{
		logFile logf( ifslog );
		int nsteps = logf.getNSteps();
		Info << nsteps << " Steps found in convergence history" << endl;
	}
#endif
	
#if 1
	for (label i=startTime; i<endTime; i++)
	{
		runTime.setTime(Times[i], i);
	
		mesh.readUpdate();
	
		IOobjectList objects(mesh, runTime.timeName());
	
		Info << "----> Timestep: " << Times[i].name() << endl;
	
#include "writeCGNS.H"    // write a CGNS file for this time step
	
		Info << endl;
	}

#endif

	Info << "Done" << endl;
	
	return 0;
}


// ************************************************************************* //
