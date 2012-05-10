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
  Conversion of CGNS files into Foam's mesh and fields :    Write CGNS file

Authors 
  Martin Beaudoin, Hydro-Quebec - IREQ, 2005

\*---------------------------------------------------------------------------*/

{
#if defined _WRITE_CGNS_

const pointField& points = mesh.points();
const cellShapeList& cells = mesh.cellShapes();

if ( access(cgnsDataPath.c_str(),W_OK) != 0 )
{
	cerr << "Please make sure that the CGNS output directory " << cgnsDataPath << " is existent and writable." << endl;
}

const std::string cgns_filename = cgnsDataPath + "/" + Times[i].name()+".cgns" ;

Info << "CGNS output file : " << cgns_filename << endl;

// Create a CGNS file
CGNSOO::file cgnsFile( cgns_filename, CGNSOO::file::WRITE );
const int cellDim = 3;
const int physDim = 3;
CGNSOO::Base_t cgnsBase = cgnsFile.writeBase( "base", cellDim, physDim );

#ifdef TURBOMACHINERY_WORKGROUP
// Turbomachinery descriptor
cgnsBase.writeDescriptor( "Turbomachinery Workgroup" );
#endif

// We work in SI units (do we really?)
cgnsBase.writeDataClass( CGNSOO::Dimensional );
cgnsBase.writeSIUnits();

// Dimensions - not here
//CGNSOO:DimensionalExponents dimensions;
//dimensions[CGNSOO::DimensionalExponenents::Length] = 1;
//cgnsBase.writeDimensionalExponents( dimensions );

// ConvergenceHistory
#if 0
int nsteps = logFile.getNSteps();
CGNSOO::ConvergenceHistory_t convHistory = cgnsBase.writeConvergenceHistory( nsteps, "Final residuals for each scalar field" );
std::vector<int>    final_res_dims(1,nsteps);
std::vector<double> final_res_array;
logFile.getFinalResidual(final_res_array);
convHistory.writeDataArray( "FinalResidual", final_res_dims, final_res_array );
#endif

// FlowEquationSet
CGNSOO::GoverningEquationsType_t equationSet = CGNSOO::GoverningEquationsNull;
#if defined CGNSTOFOAM_EXTRACT_FlowEquationSet
if ( application == "simpleFoam" ) equationSet = CGNSOO::NSTurbulentIncompressible;
else if ( application == "icoFoam" ) equationSet = CGNSOO::NSLaminar;
#endif

CGNSOO::FlowEquationSet_t cgnsFlowEq = cgnsBase.writeFlowEquationSet(physDim);
switch( equationSet )
{
case CGNSOO::NSTurbulentIncompressible:
	cgnsFlowEq.writeGoverningEquations( equationSet );
	cgnsFlowEq.writeTurbulenceModel( CGNSOO::TwoEquation_JonesLaunder );
	break;
case CGNSOO::NSTurbulent:
	cgnsFlowEq.writeGoverningEquations( equationSet );
	cgnsFlowEq.writeTurbulenceModel( CGNSOO::TwoEquation_JonesLaunder );
	break;
case CGNSOO::NSLaminar:
	cgnsFlowEq.writeGoverningEquations( equationSet );
	break;
default:
	break;
}

// Steady-state or not
cgnsBase.writeSimulationType( (steadyState) ? CGNSOO::NonTimeAccurate : CGNSOO::TimeAccurate );

int zoneNum = 0;   // Everything goes into a single zone (for now)

// Write single zone information
std::ostringstream oss;
oss << "Zone_" << zoneNum;
std::string zoneName = oss.str();
CGNSOO::ZoneType_t zoneType = CGNSOO::Unstructured;
std::vector<int> vertexSize(1,mesh.nPoints());
std::vector<int> cellSize(1,mesh.nCells());
std::vector<int> bndrySize;
Info << "Processing the mesh: " << vertexSize[0] << " nodes, " << cellSize[0] << " cells" << endl;
CGNSOO::Zone_t cgnsZone = cgnsBase.writeZone( zoneName, 
					      vertexSize, cellSize, bndrySize, 
					      zoneType );

// Obtain and write grid coordinates
std::vector<double> xCoordinates(vertexSize[0]);
std::vector<double> yCoordinates(vertexSize[0]);
std::vector<double> zCoordinates(vertexSize[0]);

forAll(points, pointi)
{
	xCoordinates[pointi] = points[pointi].x();
	yCoordinates[pointi] = points[pointi].y();
	zCoordinates[pointi] = points[pointi].z();
}

CGNSOO::GridCoordinates_t cgnsGridCoord = cgnsZone.writeGridCoordinates();
cgnsGridCoord.writeCoordinatesData( CGNSOO::GridCoordinates_t::CARTESIAN, 
				    xCoordinates, yCoordinates, zCoordinates );

// Build connectivity information
Info << "Processing mesh connectivity: " << mesh.nCells() << " cells of type: ";

// Do we have a single type of cell or do we deal with mixed cells
CGNSOO::ElementType_t element_type = CGNSOO::ElementTypeNull;
std::string s_element_type("ElementTypeNull");

bool first_time = true;;
forAll(cells, celli)
{
	int nVerticesFirstCell = 0;
	if( first_time )
	{
		first_time = false;
		nVerticesFirstCell = cells[celli].size();  // Type of first cell
		switch( nVerticesFirstCell )
		{
		case 8: 
			element_type = CGNSOO::HEXA_8;
			s_element_type = "HEXA_8";
			break;
	
		case 6:
			element_type = CGNSOO::PENTA_6;
			s_element_type = "PENTA_6";
			break;
		
		case 5:
			element_type = CGNSOO::PYRA_5;
			s_element_type = "PYRA_5";
			break;
		
		case 4:
			element_type = CGNSOO::TETRA_4;
			s_element_type = "TETRA_4";
			break;
		
		default:
			FatalErrorIn(args.executable())
				<< "Wrong number of vertices in cell\n"
				<< "    expected 4,5,6, or 8, found " 
				<< nVerticesFirstCell
				<< abort(FatalError);
			break;
		}
	}
	else if(nVerticesFirstCell != cells[celli].size()) // Is the next one different
	{
		element_type = CGNSOO::MIXED;
		s_element_type = "MIXED";
		break;
	}
}

Info << s_element_type << endl;

// Initialize the connectivity table - CGNS indices start at 1
std::vector<int> connectivity;
forAll(cells, celli)
{
	const labelList& shapeLabels = cells[celli];
	const int nVertices = shapeLabels.size();
	
	switch( nVertices )
	{
	case 8: 
		CGNS_Hexa8ConnectivityBuilder( connectivity, element_type, shapeLabels );
		break;

	case 6:
		CGNS_Prism6ConnectivityBuilder( connectivity, element_type, shapeLabels );
		break;
	
	case 5:
		CGNS_Pyra5ConnectivityBuilder( connectivity, element_type, shapeLabels );
		break;
	
	case 4:
		CGNS_Tetra4ConnectivityBuilder( connectivity, element_type, shapeLabels );
		break;
	
	default:
		FatalErrorIn(args.executable())
			<< "Wrong number of vertices in cell\n"
			<< "    expected 4,5,6, or 8, found " 
			<< shapeLabels.size()
			<< abort(FatalError);
		break;
	}
}

CGNSOO::Elements_t cgnsElements = cgnsZone.writeElements( 
					zoneName + std::string("_Body"),
					element_type,
					1,
					cellSize[0],
					0,
					connectivity );


// Process the solutions fields

Info << "Processing fields" << endl;

HashSet<word> volScalarNames;
HashSet<word> volVectorNames;
HashSet<word> surfScalarNames;
HashSet<word> surfVectorNames;
HashSet<word> sprayScalarNames;
HashSet<word> sprayVectorNames;

#include "getFieldNames.H"

PtrList<volScalarField>     volScalarFields;
PtrList<volVectorField>     volVectorFields;
PtrList<surfaceScalarField> surfScalarFields;
PtrList<surfaceVectorField> surfVectorFields;
PtrList<surfaceScalarField> sprayScalarFields;
PtrList<surfaceVectorField> sprayVectorFields;

// Read the VolScalarFields (cell centered)
readFields<volScalarField, fvMesh> (mesh,
				    objects,
				    volScalarNames,
				    volScalarFields);

// Read the VolVectorFields (cell centered)
readFields<volVectorField, fvMesh> (mesh,
				    objects,
				    volVectorNames,
				    volVectorFields);

if ( volScalarFields.size() || volVectorFields.size() )
{
	// We standardize on the more common "solution at the nodes"
	// convention when writing the CGNS file
	CGNSOO::FlowSolution_t cgnsSolution = cgnsZone.writeFlowSolution("FlowSolution", CGNSOO::Vertex);
	
	forAll(volScalarFields, fieldI)
	{
		const volScalarField& vsf = volScalarFields[fieldI];

		// Convert field name to CGNS's Quantity
		CGNSOO::Quantity_t e_cgns_qty = string_OpenFoam_to_CGNSField(vsf.name());
		if (e_cgns_qty == CGNSOO::NULL_DATA && !allow_userdefined_fields )
		{
			Info << "Scalar field : " << vsf.name() << " is not supported yet" << endl;
			continue;
		}	
		std::cout << "Processing scalar field '" << vsf.name() << "'" << std::endl;		
		
		// Interpolate at node
		pointScalarField psf(pInterp.interpolate(vsf));

		// Special case for static pressure: scale using density 'rho'
		if (e_cgns_qty == CGNSOO::PRESSURE)
		{
			Info << "    Scaling pressure " << vsf.name() << " by rho = " << rho_ << endl;
			psf *= rho_;
		}


#if defined _WRITECGNS_DEBUG_INFO
		Info << "vsf.name(): "<< vsf.name() << endl;
		Info << "vsf.size(): "<< vsf.size() << endl;
		Info << "psf.size(): "<< psf.size() << endl;
#endif

		std::auto_ptr<std::vector<double> > p_data( new std::vector<double>(psf.size()) );	
		forAll(psf, i)
		{
			(*p_data)[i] = psf[i];
		}
			
		std::string qty_s = (e_cgns_qty == CGNSOO::NULL_DATA) 
					? std::string(vsf.name())
					: (CGNSOO::QuantityEnumToString(e_cgns_qty));
		CGNSOO::DataArray_t field = cgnsSolution.writeField( qty_s, *p_data );
		if ( e_cgns_qty == CGNSOO::NULL_DATA )
		{
			// User defined data - we should add the appropriate units information
			dimensionSet dims = vsf.dimensions();
			std::vector<double> units(5);
			units[0] = dims[Foam::dimensionSet::MASS];
			units[1] = dims[Foam::dimensionSet::LENGTH];
			units[2] = dims[Foam::dimensionSet::TIME];
			units[3] = dims[Foam::dimensionSet::TEMPERATURE];
			units[4] = 0; // angle
			field.writeDimensionalExponents( units );
		} 
	}

	forAll(volVectorFields, fieldI)
	{
		const volVectorField& vvf = volVectorFields[fieldI];
		
		// Interpolate at nodes
		pointVectorField pvf(pInterp.interpolate(vvf));

		const std::string& offieldname = vvf.name();
		
#if defined _WRITECGNS_DEBUG_INFO
		Info << "vvf.name(): "<< vvf.name() << endl;
		Info << "vvf.size(): "<< vvf.size() << endl;
		Info << "pvf.size(): "<< pvf.size() << endl;
#endif

		// Separate the vector field into components for CGNS
		std::auto_ptr< std::vector<double> > p_data_x( new std::vector<double>(pvf.size()) );
		std::auto_ptr< std::vector<double> > p_data_y( new std::vector<double>(pvf.size()) );
		std::auto_ptr< std::vector<double> > p_data_z( new std::vector<double>(pvf.size()) );
		
		forAll(pvf, i)
		{
			(*p_data_x)[i] = pvf[i].x();
			(*p_data_y)[i] = pvf[i].y();
			(*p_data_z)[i] = pvf[i].z();
		}
		
		// Transfer to CGNS
		CGNSOO::Quantity_t e_cgns_qty = string_OpenFoam_to_CGNSField(offieldname+"_X");
		if ( e_cgns_qty == CGNSOO::NULL_DATA && !allow_userdefined_fields )
		{
			Info << "Vector field '" << vvf.name() << "' is not supported yet" << endl;
			continue;
		}

		// Name conversions happen in two steps:
		// 1. convert from OF to a CGNS enum
		// 2. from CGNS enum to CGNS quantity name
		CGNSOO::Quantity_t e_cgns_qty_x = string_OpenFoam_to_CGNSField(offieldname+"_X");
		CGNSOO::Quantity_t e_cgns_qty_y = string_OpenFoam_to_CGNSField(offieldname+"_Y");
		CGNSOO::Quantity_t e_cgns_qty_z = string_OpenFoam_to_CGNSField(offieldname+"_Z");
		std::string qty_x_s = (e_cgns_qty_x == CGNSOO::NULL_DATA) 
					? vvf.name() + "_X"
					: CGNSOO::QuantityEnumToString(e_cgns_qty_x);
		std::string qty_y_s = (e_cgns_qty_y == CGNSOO::NULL_DATA) 
					? vvf.name() + "_Y"
					: CGNSOO::QuantityEnumToString(e_cgns_qty_y);
		std::string qty_z_s = (e_cgns_qty_z == CGNSOO::NULL_DATA) 
					? vvf.name() + "_Z"
					: CGNSOO::QuantityEnumToString(e_cgns_qty_z);
		
		CGNSOO::DataArray_t fx = cgnsSolution.writeField( qty_x_s, *p_data_x );
		CGNSOO::DataArray_t fy = cgnsSolution.writeField( qty_y_s, *p_data_y );
		CGNSOO::DataArray_t fz = cgnsSolution.writeField( qty_z_s, *p_data_z );
		
		if ( e_cgns_qty == CGNSOO::NULL_DATA )
		{
			// User defined data - we should add the appropriate units information
			dimensionSet dims = vvf.dimensions();
			std::vector<double> units(5);
			units[0] = dims[Foam::dimensionSet::MASS];
			units[1] = dims[Foam::dimensionSet::LENGTH];
			units[2] = dims[Foam::dimensionSet::TIME];
			units[3] = dims[Foam::dimensionSet::TEMPERATURE];
			units[4] = 0; // angle
			fx.writeDimensionalExponents( units );
			fy.writeDimensionalExponents( units );
			fz.writeDimensionalExponents( units );
		} 
	}

	// Should we do something with the following four fields types ?
#if 0	
	// Read the SurfaceScalarFields
	readFields<surfaceScalarField, fvMesh> (mesh,
						objects,
						surfScalarNames,
						surfScalarFields);
	forAll(surfScalarFields, fieldI)
	{
#if defined _WRITECGNS_DEBUG_INFO
		const surfaceScalarField& ssf = surfScalarFields[fieldI];
		
		Info << "ssf.name(): "<< ssf.name() << endl;
		Info << "ssf.size(): "<< ssf.size() << endl;
#endif
	}
    
	// Read the SurfaceVectorFields
	readFields<surfaceVectorField, fvMesh> (mesh,
						objects,
						surfVectorNames,
						surfVectorFields);
	forAll(surfVectorFields, fieldI)
	{
#if defined _WRITECGNS_DEBUG_INFO
		const surfaceVectorField& svf = surfVectorFields[fieldI];
		
		Info << "svf.name(): "<< svf.name() << endl;
		Info << "svf.size(): "<< svf.size() << endl;
#endif
	}

	// Read the SprayScalarFields
	// Read the SprayVectorFields
#endif
}


// Extract the boundary conditions

const polyBoundaryMesh& patches = mesh.boundaryMesh();

Info << "Processing the boundary conditions from " 
     << patches.size() << " patches" 
     << endl;

if ( patches.size() > 0 )
{
	CGNSOO::ZoneBC_t cgnsZoneBC = cgnsZone.writeZoneBC();
	forAll(patches, patchI)
	{
                int noPatch = patchI+1;
		const polyPatch& pp = patches[patchI];
#if defined _WRITECGNS_DEBUG_INFO
		Info << "------------------------" << endl;
		Info << "Patch name         : " << pp.name() << endl;
		//Info << "Patch            : " << pp << endl;
		Info << "Patch.nPoints      : " << pp.nPoints() << endl;
		Info << "Patch.type         : " << pp.type() << endl;
		Info << "Patch.physicalType : " << pp.physicalType() << endl;
		Info << "Patch is coupled   : " <<  (pp.coupled()?"True":"False") << endl;
		//	Info << "Patch.boundaryPoints : " << pp.boundaryPoints() << endl;
#endif

                if(pp.nPoints() == 0)
                {
                    Info << noPatch <<
                        " --> Warning: Patch name: " << pp.name()
                        << " : This patch has zero points. Skipping." << endl;
                }
                else
                {
#if defined EXPORT_CYCLIC_BOUNDARIES
		if ( pp.type()==cyclicPolyPatch::typeName )
		{
			size_t ncyclicpoints = pp.nPoints()/2;
			labelList patch_index_points = pp.meshPoints();
			//Info << "point list = { " << patch_index_points << " } " << endl;
			edgeList cyclic_edges = dynamicCast<const cyclicPolyPatch,const polyPatch>(pp).coupledPoints();
			//Info << "cyclic_edges = { " << cyclic_edges << " } " << endl;
			std::vector<int> bc_points_index(pp.nPoints()); 
			forAll(cyclic_edges, eI)
			{
				bc_points_index[eI]               = patch_index_points[cyclic_edges[eI].start()] + 1;  // Indices start at in CGNS
				bc_points_index[eI+ncyclicpoints] = patch_index_points[cyclic_edges[eI].end()  ] + 1;
			}
			
			if ( exportcyclics )
			{
			CGNSOO::ZoneGridConnectivity_t zgconnec = cgnsZone.writeZoneGridConnectivity();
			
			std::string  refname  = pp.name()+"_1";
			std::string  pername  = pp.name()+"_2";
			CGNSOO::GridLocation_t         loc      = CGNSOO::Vertex;
			CGNSOO::GridConnectivityType_t ctype    = CGNSOO::Abutting1to1;
			CGNSOO::PointSetType_t         psettype = CGNSOO::PointList;
			int indexdim = 1;
			std::vector<int> refpoints(ncyclicpoints);
			std::vector<int> perpoints(ncyclicpoints);
			for ( size_t i=0 ; i<ncyclicpoints ; i++ )
			{
				refpoints[i] = bc_points_index[i];
				perpoints[i] = bc_points_index[i+ncyclicpoints];
			}
			std::string  donorzonename = zoneName;
			CGNSOO::ZoneType_t donorzonetype = CGNSOO::Unstructured;
			CGNSOO::PointSetType_t donorpsettype = CGNSOO::PointListDonor;

                        Info << noPatch << " : Writing cyclic reference patch: " << refname << endl;
			CGNSOO::GridConnectivity_t gconnecref = zgconnec.writeGridConnectivity(
				refname,
				loc, 
				ctype, 
				psettype,
				indexdim, 
				refpoints, 
				donorzonename,
				donorzonetype, 
				donorpsettype,
				perpoints);

                        Info << noPatch << " : Writing cyclic periodic patch: " << pername << endl;
			CGNSOO::GridConnectivity_t gconnecper = zgconnec.writeGridConnectivity(
				pername,
				loc, 
				ctype, 
				psettype,
				indexdim, 
				perpoints, 
				donorzonename,
				donorzonetype, 
				donorpsettype,
				refpoints);
				
			CGNSOO::GridConnectivityProperty_t cgns_gcref_property = gconnecref.writeProperty();
			CGNSOO::GridConnectivityProperty_t cgns_gcper_property = gconnecper.writeProperty();
			
			std::vector<float> rotcenter(3,0.0F);
			std::vector<float> rotangle(3,0.0F);
			std::vector<float> translation(3,0.0F);
			float angle = 30.0F;
			rotangle[2] = angle;
			CGNSOO::Periodic_t perioref = cgns_gcref_property.writeGridConnectivityPeriodic( 
				rotcenter, 
				rotangle, 
				translation );
			rotangle[2] = -angle;
			CGNSOO::Periodic_t perioper = cgns_gcper_property.writeGridConnectivityPeriodic( 
				rotcenter, 
				rotangle, 
				translation );
			}
			else
			{
				// export as two separate patches
				word pt = pp.physicalType();
				std::vector<int> refpoints(ncyclicpoints);
				std::vector<int> perpoints(ncyclicpoints);
				for ( size_t i=0 ; i<ncyclicpoints ; i++ )
				{
					refpoints[i] = bc_points_index[i];
					perpoints[i] = bc_points_index[i+ncyclicpoints];
				}

                                Info << noPatch << " : Writing cyclic patch: " << pp.name()+"_1" << endl;
				cgnsZoneBC.writeBC( pp.name()+"_1", 
						string_OpenFoam_to_CGNSBcType(pt), 
						CGNSOO::PointList, 
						refpoints );

                                Info << noPatch << " : Writing cyclic patch: " << pp.name()+"_2" << endl;
				cgnsZoneBC.writeBC( pp.name()+"_2", 
						string_OpenFoam_to_CGNSBcType(pt), 
						CGNSOO::PointList, 
						perpoints );
			}
		}
		else
		{
#endif		
			// Transfer the point indices making up the patch
			labelList patch_index_points = pp.meshPoints();
			//Info << "point list = { " << patch_index_points << " } " << endl;
			std::vector<int> bc_points_index(pp.nPoints()); 
			forAll(patch_index_points, pipI)
			{
				bc_points_index[pipI] = patch_index_points[pipI] + 1;  // Indices start at in CGNS
			}
			word pt = pp.physicalType();

                        Info << noPatch << " : Writing patch: " << pp.name() << endl;
			cgnsZoneBC.writeBC( pp.name(), 
					string_OpenFoam_to_CGNSBcType(pt), 
					CGNSOO::PointList, 
					bc_points_index );
#if defined EXPORT_CYCLIC_BOUNDARIES
		}
#endif	
                }	
	}
}

#endif
}
