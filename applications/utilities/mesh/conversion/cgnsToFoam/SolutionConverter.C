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

Authors 
  Robert Magnan,  Hydro-Quebec - IREQ, 2007

\*---------------------------------------------------------------------------*/
#include "SolutionConverter.H"

#include "ConnectivityMapper.H"
#include "FoamQuantities.H"
#include "FoamFieldBuilder.H"

SolutionConverter::SolutionConverter
( 
    const Foam::polyMesh& mesh, 
    const CGNSOO::Base_t& base,
    const CGNSQuantityConverter& qc
) : cellmesh(mesh), qConverter(qc)
{
    int nbZones = base.getNbZone();
    std::string solutionName("");
    availableSolutions = findSolutionSetInZone(base,0,solutionName);
    solutionsInZone[0] = availableSolutions;
	
    for( int indexZone=1 ; indexZone<nbZones ; indexZone++ )
    {
        solset_t s = findSolutionSetInZone(base,indexZone,solutionName);
        if ( s != availableSolutions )
        {
            Foam::Warning << "The set of solution fields available is not the same in all zones" << Foam::endl;
            /*
                set<solinfo_t> result;
                set_intersection< set<solinfo_t>::const_iterator,
                set<solinfo_t>::const_iterator,
                set<solinfo_t>::iterator >(
                availableSolutions.begin(), availableSolutions.end(),
                s.begin(), s.end(),
                result.begin() 
                );
                available_solutions = result;
                */
        }
        solutionsInZone[indexZone] = s;
    }		
}	
	
void SolutionConverter::buildAndWriteFoamFields
( 
    const ConnectivityMapper& mapper, 
    const CGNSOO::Base_t&     base, 
    Foam::scalar              rho, 
    const Foam::Time&         time,
    bool                      dryRun,
    bool                      mapUnknown
)
{
    FoamQuantities fq;
	
    // Lookup each of the scalar quantities found in the CGNS file
    solset_t solutionSet = availableSolutions;
    while( solutionSet.size()>0 )
    {
        solset_t::iterator soliter = solutionSet.begin();
        CGNSOO::Quantity_t q  = soliter->quantity();
        CGNSOO::GridLocation_t solutionlocation = soliter->location();
		
        // In Foam, static pressure must by adimensionned using 
        // the density. rho is specified on the command line and
        // defaults to 1.0
        Foam::scalar scale_factor = 1.0;
        if ( q == CGNSOO::PRESSURE ) scale_factor /= rho;

        // Find out what is the equivalent of this in Foam
        std::string foamname = fq.getFoamName( q );
        if ( foamname == "" ) // q is not a "known" Foam quantities
        {
            if ( q == CGNSOO::USER_DATA )
                Foam::Warning << "Skipping unknown CGNS scalar : " 
                    << soliter->name() 
                    << Foam::endl;
            else
                Foam::Warning << "Skipping CGNS scalar : " 
                    << CGNSOO::QuantityEnumToString( q ) 
                    << Foam::endl;

            solutionSet.erase( solutionSet.begin() );
            continue;
        }
		
        //Foam::Info << "Processing field named " << foamname << Foam::endl;
        std::vector<CGNSOO::Quantity_t> required_quantities = fq.getQuantitiesInGroup(q);
        Foam::dimensionSet foam_dimensions = fq.getFoamDimensions( q );

        // check that we have everything we need to go on			
        bool ok = validateAndRemove( required_quantities, solutionSet );
        if ( !ok ) continue;
		
        // Now we build a separate std::vector for each required component
        // These vectors hold the actual solution
        int ncomponents = required_quantities.size();
        std::vector<Foam::scalar>* globalfielddata = new std::vector<Foam::scalar>[ ncomponents ];
        int icomponent = 0;
		
        for( std::vector<CGNSOO::Quantity_t>::const_iterator i  = required_quantities.begin();
             i != required_quantities.end();
             i++, icomponent++ )
        {
            // allocate enough space for the whole grid
            switch ( solutionlocation )
            {
                case CGNSOO::Vertex:
                    globalfielddata[icomponent].resize( mapper.getTotalNodes() );
                    break;
                case CGNSOO::CellCenter:
                    globalfielddata[icomponent].resize( mapper.getTotalCells() );
                    break;
                default:
                    Foam::Warning << "Unsupported GridLocation_t found in solution" << Foam::endl;
                    break;
            }
			
            // assemble values for this scalar in each zone into a global array
            CGNSOO::Quantity_t q = *i;
            int nbZones = base.getNbZone();
            for ( int zid=0 ; zid<nbZones ; zid++ )
            {
                std::vector<double> localfielddata;
                solset_t solutions = solutionsInZone[zid];
                for ( solset_t::const_iterator  s  = solutions.begin();
                      s != solutions.end();
                      s++ )
                {
                    if ( s->quantity() == q )
                    {
                        s->readField( localfielddata );
                        break;
                    }
                }
									
                // localfielddata now contains the scalar values for this zone
                // now, map it into the global mesh
                switch ( solutionlocation )
                {
                    case CGNSOO::Vertex:
                        // convert to global using node mapping
                        {
                            for ( int inode=0 ; inode<mapper.getLocalNodes(zid) ; inode++ )
                                globalfielddata[icomponent][mapper.n_localToGlobal(zid,inode)] 
                                    = localfielddata[inode]*scale_factor;
                        }
                        break;
                    case CGNSOO::CellCenter:
                        Foam::Warning << "Output of solution at cell center ... untested code..." << Foam::endl;
                        // convert to global using cell mapping
                        {
                            for ( int icell=0 ; icell<mapper.getLocalCells(zid) ; icell++ )
                                globalfielddata[icomponent][mapper.c_localToGlobal(zid,icell)] 
                                    = localfielddata[icell]*scale_factor;
                        }
                        break;
                    default:
                        Foam::Warning << "Unsupported GridLocation_t found in solution\n";
                        break;
                }
            }
        }
		
        // Output data in scalar or vector form
        // The FoamFieldBuilder class is responsible for building 
        // the necessary Foam datastructure and interpolating
        // the solution from nodes to cell centers if required
        if ( fq.isScalarQuantity( q ) ) // && viq.size()==1
        {
            assert ( ncomponents==1 );
            Foam::Info << "Writing scalar " << foamname << Foam::endl;
            FoamFieldBuilder< Foam::pointScalarField, 
                Foam::volScalarField, 
                Foam::dimensionedScalar, 
                Foam::scalar, 
                Foam::fvPatchScalarField > 
                scalar_builder( foamname, 
                foam_dimensions, 
                globalfielddata[0], 
                solutionlocation==CGNSOO::Vertex, 
                time.timeName(), 
                cellmesh );
            if ( !dryRun ) scalar_builder.write();
        }
        else if ( fq.isVectorQuantity( q ) )
        {
            assert ( ncomponents==3 );
            Foam::Info << "Writing vector " << foamname << Foam::endl;
            int nvals = globalfielddata[0].size();
            std::vector<Foam::vector> globalvfielddata( nvals );
            for ( int i=0 ; i<nvals ; i++ )
                globalvfielddata[i] = Foam::vector( 
                    globalfielddata[0][i], 
                    globalfielddata[1][i],
                    globalfielddata[2][i] );
            FoamFieldBuilder< Foam::pointVectorField, 
                Foam::volVectorField, 
                Foam::dimensionedVector, 
                Foam::vector, 
                Foam::fvPatchVectorField > 
                vector_builder( foamname,
                foam_dimensions, 
                globalvfielddata, 
                solutionlocation==CGNSOO::Vertex, 
                time.timeName(), 
                cellmesh );
            if ( !dryRun ) vector_builder.write();
        }
        delete [] globalfielddata;
    }
}

SolutionConverter::solset_t SolutionConverter::findSolutionSetInZone
( 
    const CGNSOO::Base_t& base, 
    int indexZone, 
    const std::string& solutionName
)
{
    // Read available solutions
    std::string zonename;
    std::vector<int> nodesize, cellsize, bndrysize;
    CGNSOO::ZoneType_t zonetype;
    CGNSOO::Zone_t    zone = base.readZone( indexZone, 
    zonename, 
    nodesize, cellsize, bndrysize, 
    zonetype );
					
    solset_t solutions;
    int nbsolutions = zone.getNbFlowSolution();
    for ( int isol=0 ; isol<nbsolutions ; isol++ )
    {
        std::string solname;
        CGNSOO::GridLocation_t gridloc;
        CGNSOO::FlowSolution_t flowsolution = zone.readFlowSolution( isol, solname, gridloc );
        if ( solutionName=="" || solname==solutionName )
        {
            int nbfields = flowsolution.getNbFields();
            for ( int ifield=0 ; ifield<nbfields ; ifield++ )
            {
                CGNSOO::DataType_t  datatype;
                std::string      fieldname;
                CGNSOO::DataArray_t field = flowsolution.readField( ifield, fieldname, datatype );
                CGNSOO::Quantity_t  quantity_enum = qConverter.getEnum( fieldname );
				
                solinfo_t this_info( quantity_enum,
                gridloc,
                flowsolution,
                ifield,
                fieldname);
                std::pair<solset_t::iterator,bool> p = solutions.insert( this_info );
            }
            break;
        }
    }
    return solutions;
}
	
// Removes all CGNSOO::Quantity_t entries in required_components 
// from the set 'solutions'.
// Return true if they were all found and removed correctly
bool SolutionConverter::validateAndRemove
( 
    const std::vector<CGNSOO::Quantity_t>& required_components, 
    solset_t& solutions 
)
{
    // In Foam, this scalar may be part of a vectorial quantity.
    // We build a list of the required CGNS Quantities to define
    // the corresponding Foam quantity.
    // Store results in quantity_components
    bool failed = false;
    for ( std::vector<CGNSOO::Quantity_t>::const_iterator 
              qlookup  = required_components.begin() ;
          qlookup != required_components.end() ;
          qlookup++ )
    {
        solset_t::const_iterator iq  = solutions.begin();
        while( iq != solutions.end() )
        {
            if ( (*iq).quantity()==*qlookup )
            {
                solutions.erase( iq );
                break;
            }
            iq++;
        }
        if ( iq == solutions.end() )
        {
            //Foam::Info << "The field " << newname
            //	<< " requires CGNS quantity "
            //	<< QuantityEnumToString( qlookup )
            //	<< " to be available" << Foam::endl;
            failed = true;
        }
    }
    return !failed;
}
