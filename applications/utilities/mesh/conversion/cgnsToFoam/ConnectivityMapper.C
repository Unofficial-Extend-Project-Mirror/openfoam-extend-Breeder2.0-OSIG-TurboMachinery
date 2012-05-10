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
  Martin Beaudoin,  Hydro-Quebec - IREQ, 2005
  Robert Magnan,    Hydro-Quebec - IREQ, 2007

\*---------------------------------------------------------------------------*/
#include "libcgnsoo3/cgnsoo.H"

#include "ConnectivityMapper.H"
#include "CGNSElementType.H"

#include "labelList.H"
#include "Time.H"
#include "cellModeller.H"
#include "mergePoints.H"

#include <vector>
#include <string>

// convert a single CGNS element section into an OpenFoam cellShape
Foam::cellShape ConnectivityMapper::buildOneCellFromCGNSConnectivity( 
    std::vector<int>::const_iterator  connectivity_ptr,
    int                          numberOfNodesPerElement,
    const Foam::cellModel&       cell_model,
    int                          zoneIndex )
{
    Foam::labelList cellPoints(numberOfNodesPerElement);
    for( int i=0 ; i<numberOfNodesPerElement ; i++ )
    {
        foamNodeIndex localindex = (*connectivity_ptr)-1; // CGNS indices start at 1
        cellPoints[i] = n_localToGlobal(zoneIndex,localindex); 
        connectivity_ptr++;
    }

    Foam::cellShape c(cell_model,cellPoints);
    /*
        c.collapse();
        if ( c.nPoints() != cell_model.nPoints() )
        {
        Warning << "unstructured mesh: detected a degenerate cell" << Foam::endl;
        }
        */
    return c;
}

// convert a mixed CGNS element section into a list of OpenFoam cells
void ConnectivityMapper::extractCellsFromMixedCgnsElements( 
    const std::vector<int>&      connectivity,
    int                     zoneIndex,
    int                     eSectionIndex,
    list<Foam::cellShape>&  l_cells )
{
    static bool unknown_warn = false;
    
    // case of MIXED elements: do a cell-by-cell conversion
    for ( std::vector<int>::const_iterator p  = connectivity.begin() ; 
          p != connectivity.end() ;
    )
    {
        // Extract type of next cell
        CGNSOO::ElementType_t cell_etype = CGNSOO::ElementType_t(*p);
	p++; // skip this one

        // Transfer connectivity for this cell
	int numberOfNodesPerElement = CGNSElementType::getNumberOfNodesPerElement( cell_etype );

	if ( numberOfNodesPerElement == -1 )
	{
            Foam::Warning << "Unsupported cell type "
                << CGNSOO::ElementTypeName[cell_etype] 
                << " (code " << int(cell_etype) << ")"
                << " found "
                << "in mixed connectivity of zone " 
                << zoneIndex 
                << ", section " << eSectionIndex
                << " - skipping" << Foam::endl;
        }
        else
        {
            const Foam::cellModel& cell_model = CGNSElementType::getFoamCellModel( cell_etype );
            if ( cell_model.name() == "unknown" )
            {
                if ( !unknown_warn )
                    Foam::Warning << "Cell type "
                        << CGNSOO::ElementTypeName[cell_etype] 
                        << " (code " << int(cell_etype) << ")"
                        << " found "
                        << "in mixed connectivity of zone " 
                        << zoneIndex 
                        << ", section " << eSectionIndex
                        << " has no OpenFOAM equivalence"
                        << " - skipping" << Foam::endl;
                unknown_warn = true;
            }
            else
            {
                Foam::cellShape c = buildOneCellFromCGNSConnectivity(  
                    p,
                    numberOfNodesPerElement,
                    cell_model,
                    zoneIndex );
                l_cells.push_back( c );
            }
            p += numberOfNodesPerElement;
        }
    }
}

// Extracts the cells of a unstructured CGNS mesh 
// into a list of Foam cells
void ConnectivityMapper::extractCellsFromUnstructuredCgnsElements( 
    CGNSOO::ElementType_t   elementType,
    const std::vector<int>&      connectivity,
    int                     zoneIndex,
    int                     eSectionIndex,
    list<Foam::cellShape>&  l_cells )
{
    const Foam::cellModel& cell_model = CGNSElementType::getFoamCellModel( elementType );
    if ( cell_model.name() == "unknown" )
    {
        Foam::Warning << "Cell type " 
            << CGNSElementType::enumToString(elementType) 
            << " found in zone " << zoneIndex 
            << ", section " << eSectionIndex
            << " has no OpenFOAM equivalence"
            << " - skipping" << Foam::endl;
        return;
    }
    int numberOfNodesPerElement = CGNSElementType::getNumberOfNodesPerElement( elementType );
    Foam::labelList cellPoints(numberOfNodesPerElement); // hex blocks
	
    for ( std::vector<int>::const_iterator p  = connectivity.begin() ;
          p != connectivity.end() ; 
    )
    {
        Foam::cellShape c = buildOneCellFromCGNSConnectivity( 
            p, 
            numberOfNodesPerElement,
            cell_model,
            zoneIndex );
        p += numberOfNodesPerElement;   
        l_cells.push_back(c);
    }
}

// This method take a (i,j,k), i,j,k>=1 triplet representing a node in a structured CGNS zone
// and returns an unique index representing the ordering of the nodes in the points lists.
// That returned index is >=1
int  ConnectivityMapper::getLocalNodeIndexFromStructuredCGNSTriplet( int ni, int nj, int nk, int i, int j, int k )
{
    // CGNS nodes are stored in the following fortran-type loop convention:
    //    (((node(i,j,k), i=1,ni), j=1,nj), k=1,nk)
    // see SIDS sections 7.1 and 7.2
    const int dimIJ = ni * nj;
    const int dimI  = ni;
    return 1 + (k-1)*dimIJ + (j-1)*dimI + (i-1);
}

// Definition of a new Structured Zone
void ConnectivityMapper::addStructuredZone( int nnodei, int nnodej, int nnodek, const list<Foam::point>& plist )
{
    int ncells = (nnodei-1)*(nnodej-1)*(nnodek-1);
    size_t nnodes = nnodei*nnodej*nnodek;
	
    if ( plist.size() != nnodes )
    {
        Foam::FatalError << "Invalid point list for a structured zone" << Foam::endl
            << "Expecting " << (unsigned int)nnodes << " points"
            << " but got " << (unsigned int)plist.size() << Foam::endl
            << exit(Foam::FatalError);
    }
    int last_index = zdata.size();
    int offset = (last_index==0)?0:(zdata[last_index-1].offset+zdata[last_index-1].n_nodes);
    zone_data_t d = { true, nnodei, nnodej, nnodek, ncells, nnodes, offset, plist };
    zdata.push_back(d);
    g_n_cells += ncells;
    g_n_nodes += nnodes;
    addStructuredConnectivity( last_index, nnodei, nnodej, nnodek );
}

// Definition of a new Unstructured Zone
void ConnectivityMapper::addUnstructuredZone(
    int nn,
    int nc, 
    const std::list<Foam::point>&     plist,
    const std::vector<std::string>&        sectionnames, 
    const std::vector<CGNSOO::ElementType_t>& etypes, 
    const std::vector< std::vector<int> >& connectivities )
{
    int last_index = zdata.size();
    int offset = (last_index==0)?0:(zdata[last_index-1].offset+zdata[last_index-1].n_nodes);
    zone_data_t d = { false, 0,0,0, nc, nn, offset, plist };
    zdata.push_back(d);
    g_n_cells += nc;
    g_n_nodes += nn;
	
    addUnstructuredConnectivity( last_index, etypes, connectivities );
	
    // keep a copy of 2D element sections for possible use as BC regions
    for ( size_t i=0 ; i<etypes.size() ; i++ )
    {
        if ( CGNSElementType::is2D(etypes[i]) )
        {
            uns_z2d.push_back(
                zone2d_data_t(  last_index, 
                sectionnames[i], 
                connectivities[i],
                etypes[i] ) 
            );
        }
    }
}

// Defines the implicit connectivity of a structured zone
void ConnectivityMapper::addStructuredConnectivity( int indexZone, int nnodei, int nnodej, int nnodek )
{
    const Foam::cellModel& hex = *(Foam::cellModeller::lookup("hex"));
	
    const int ncelli = nnodei-1;
    const int ncellj = nnodej-1;
    const int ncellk = nnodek-1;
	
    // for each cell, create its CGNS connectivity
    // convert it to use Foam indices instead of CGNS indices
    // and use it create a new Foam cell.
    int cellCgnsIndex[8];
    for( int k=1 ; k<=ncellk ; k++ )
	for( int j=1 ; j<=ncellj ; j++ )
            for( int i=1 ; i<=ncelli ; i++ )
            {
		// build the explicit connectivity	
		cellCgnsIndex[0] = getLocalNodeIndexFromStructuredCGNSTriplet( nnodei,nnodej,nnodek, i  , j  , k   );
		cellCgnsIndex[1] = getLocalNodeIndexFromStructuredCGNSTriplet( nnodei,nnodej,nnodek, i+1, j  , k   );
		cellCgnsIndex[2] = getLocalNodeIndexFromStructuredCGNSTriplet( nnodei,nnodej,nnodek, i+1, j+1, k   );
		cellCgnsIndex[3] = getLocalNodeIndexFromStructuredCGNSTriplet( nnodei,nnodej,nnodek, i  , j+1, k   );
		cellCgnsIndex[4] = getLocalNodeIndexFromStructuredCGNSTriplet( nnodei,nnodej,nnodek, i  , j  , k+1 );
		cellCgnsIndex[5] = getLocalNodeIndexFromStructuredCGNSTriplet( nnodei,nnodej,nnodek, i+1, j  , k+1 );
		cellCgnsIndex[6] = getLocalNodeIndexFromStructuredCGNSTriplet( nnodei,nnodej,nnodek, i+1, j+1, k+1 );
		cellCgnsIndex[7] = getLocalNodeIndexFromStructuredCGNSTriplet( nnodei,nnodej,nnodek, i  , j+1, k+1 );
		
		Foam::labelList cellPoints(8);
		for( int m=0 ; m<8 ; m++ )  // hex
		{
                    foamNodeIndex localIndex = cellCgnsIndex[m]-1;
                    cellPoints[m] = n_localToGlobal(indexZone,localIndex);
		}
		
		// collapse degenerated nodes
		Foam::cellShape c(hex,cellPoints);
		//c.collapse();
		if ( c.nPoints() != hex.nPoints() )
		{
                    Foam::Warning << "Degenerated cell detected in structured mesh at (" 
                        << "i=" << nnodei+1 << ","
                        << "j=" << nnodej+1 << ","
                        << "k=" << nnodek+1 << ") "
                        << "in zone " << indexZone+1 << Foam::endl;
		}
		else
		{
                    tmpcells.push_back( c );
		}
            }
}

// Explicitly defines the connectivity of a unstructured zone
// This connectivity can be splitted in multiple 'sections' as in CGNS
void ConnectivityMapper::addUnstructuredConnectivity( 
    int zoneIndex,
    const std::vector<CGNSOO::ElementType_t>& etypes, 
    const std::vector<std::vector<int> >& connectivities )
{
    // build a list of OpenFOAM cells
    int nesections = etypes.size();
    for ( int i=0 ; i<nesections ; i++ )
    {
        if ( etypes[i] == CGNSOO::MIXED )
            extractCellsFromMixedCgnsElements( connectivities[i], 
            zoneIndex,
            i,
            tmpcells );
        else
            extractCellsFromUnstructuredCgnsElements( etypes[i], 
            connectivities[i], 
            zoneIndex,
            i,
            tmpcells );
    }
}

// This method "compiles" the data structure, that is it tries to
// merge nodes that are physically at the same loaction and computes
// internal tables that makes it possible to go from a node number in 
// a zone to a global node number in the whole mesh.
void ConnectivityMapper::merge( double mergeTolerance )
{
    // Compute total number of points from all zones without merge
    // We don't rely on g_n_nodes in case merge() has been called previously
    int nptstotal = 0;
    for ( std::vector<zone_data_t>::const_iterator   z  = zdata.begin() ;
          z != zdata.end() ;
          z++ )
    {
        nptstotal += (*z).n_nodes;
    }
	
    // Construct a single table containing all points from all zones
    Foam::pointField oldPoints( nptstotal );
    int k=0;
    for ( std::vector<zone_data_t>::const_iterator   z  = zdata.begin() ;
          z != zdata.end() ;
          z++ )
    {
        const list<Foam::point>& plist = (*z).pts;
        for( list<Foam::point>::const_iterator p  = plist.begin() ;
             p != plist.end() ; 
             p++ )
        {
            oldPoints[k++] = *p;
        }
    }
	
    // compute absolute tolerance
    Foam::scalar minEdgeLength = computeMinEdgeLength( oldPoints );
    mergeTolerance *= minEdgeLength;
	
    // Merge according to tolerance
    // generates a mapping table
    bool verbose = false;
    /*bool b_change =*/ mergePoints( oldPoints, 
    mergeTolerance, 
    verbose, 
    oldToNew, 
    newPoints );
	
    // Build a permanent cellShapeList from the temporary std::list of cellShape
    int ncells = tmpcells.size();
    cellShapes.setSize( ncells );
    k = 0;
    for ( list<Foam::cellShape>::const_iterator i=tmpcells.begin() ; i!=tmpcells.end() ; i++ )
        cellShapes[k++] = (*i);
	
    forAll ( cellShapes, icell )
    {
        Foam::cellShape& c = cellShapes[icell];
        Foam::label npoints = c.nPoints();
        for ( int i=0 ; i<npoints ; i++ )
            c[i] = oldToNew[ c[i] ];
    }
	
    g_n_nodes = newPoints.size();
	
    // keep note that merge was done
    merged = true;
}

Foam::cellShapeList& ConnectivityMapper::getCells()
{
    return cellShapes;
}

const Foam::cellShapeList& ConnectivityMapper::getCells() const
{
    return cellShapes;
}


// Compute the minimum edge length to normalize tolerance
double ConnectivityMapper::computeMinEdgeLength( const Foam::pointField& points ) const
{
    Foam::scalar minEdgeLength = Foam::GREAT;
    for( list<Foam::cellShape>::const_iterator icell=tmpcells.begin() ; icell!=tmpcells.end() ; icell++ )
    {
        Foam::edgeList eList = (*icell).edges();
	
        forAll( eList, iedge )
        {
            minEdgeLength = Foam::min(
                minEdgeLength,
                eList[iedge].mag(points)
            );
        }
    }
    return static_cast<double>(minEdgeLength);
}

foamNodeIndex ConnectivityMapper::n_localToGlobal( int zid, foamNodeIndex nid ) const
{
    const int zoneOffset = zdata[zid].offset; //(zid==0) ? 0 : zdata[zid-1].offset;
    const int basic_index = zoneOffset + nid;
    if ( merged )
    {
        // get index of node in a global table without merge
        // get corresponding index after merge
        return oldToNew[basic_index];
    }
	
    return basic_index;
}

int ConnectivityMapper::c_localToGlobal( int zid, int cid ) const
{
    // get cell number of first cell in zid
    int basic_index = 0;
    for ( int i=0 ; i<zid ; i++ )
        basic_index += zdata[i].n_cells;
    // add cell offset
    return basic_index + cid;
}
