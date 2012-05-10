/*------------------------------------------------------------------------------
Copyright (C) 2004-2007 Hydro-Quebec

This file is part of CGNSOO

CGNSOO is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 2, or (at your option) any
later version.

CGNSOO is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with CGNSOO  If not, write to the Free Software Foundation,
Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
------------------------------------------------------------------------------*/
#include "Array.H"
#include "cgnsoo.H"

namespace CGNSOO
{

bool Elements_t::hasParentData() const
{
	ElementType_t etype;
	int start, end, nbndry, parentflag;
	cgnsstring sname;
	int ier = cg_section_read( getFileID(), getBase().getID(), getZone().getID(), getID(), sname, &etype, &start, &end, &nbndry, &parentflag );
	check_error( "Elements_t::hasParentData", "cg_section_read", ier );
	return parentflag!=0;
}

int Elements_t::findDataArrayIndex( const string& name ) const
{
	// this is circumvent (what I call) a bug in cgnslib
	// The datay Arrays named ElementConnectivity and ParentData
	// under an Elements_t are not really considered DataArrays
	// by the MLL... so we can not use cg_narrays
	if ( name == "ElementConnectivity" ) return 1;
	if ( name == "ParentData" && hasParentData() ) return 2;
	return 0;
	//throw cgns_badargument( "Elements_t::findDataArrayIndex", "invalid data array name" );
}

DataArray_t Elements_t::readConnectivity() const
{
	std::cerr << "Method Elements_t::readConnectivity() not yet implemented" << std::endl;
	int index = findDataArrayIndex("ElementConnectivity");
	return push( "DataArray_t", index ); 
}

void Elements_t::readConnectivityAndParent( vector<int>* connectivity, vector<int>* parentdata ) const
{
	// get DataSize to dimension the connectivity vector
	int dataSize;
	int ier = cg_ElementDataSize( getFileID(), getBase().getID(), getZone().getID(), getID(), &dataSize );
	check_error( "Elements_t::readConnectivity", "cg_ElementDataSize", ier );

	// get ElementSize to dimension the parent_data vector
	ElementType_t etype;
	int start, end, nbndry, parentflag;
	cgnsstring sname;
	ier = cg_section_read( getFileID(), getBase().getID(), getZone().getID(), getID(), sname, &etype, &start, &end, &nbndry, &parentflag );
	check_error( "Elements_t::readConnectivity", "cg_section_read", ier );
	int elementSize = end-start+1;
	
	// read the data
	Array<int> aconnec( dataSize );
	Array<int> aparent( 2*4*elementSize );
	ier = cg_elements_read( getFileID(), getBase().getID(), getZone().getID(), getID(), aconnec, aparent );
	if ( connectivity ) *connectivity = aconnec;
	if ( parentdata   )
	{
		if ( parentflag )
		{
			*parentdata = aparent;
		}
		else
		{
			// parent data was requested but is not available
			parentdata->resize(0);
		}
	}
	check_error( "Elements_t::readConnectivity", "cg_elements_read", ier );
}

DataArray_t Elements_t::readConnectivity( vector<int>& connectivity ) const
{
	readConnectivityAndParent( &connectivity, NULL );

	int index = findDataArrayIndex("ElementConnectivity");
	return push( "DataArray_t", index );
}

DataArray_t Elements_t::readParentData( vector<int>& parent_data ) const
{
	if ( ! hasParentData() ) throw cgns_notfound( "Elements_t::readParentData", "ParentData" );
	readConnectivityAndParent( NULL, &parent_data );

	int index = findDataArrayIndex("ParentData");
	return push( "DataArray_t", index );
}

DataArray_t Elements_t::writeElementParents( const vector<int>& parentdata )
{
	Array<int> aparent( parentdata );
	int ier = cg_parent_data_write( getFileID(), getBase().getID(), getZone().getID(), getID(), aparent );
	check_error( "Elements_t::writeElementParents", "cg_parent_data_write", ier );
	return push( "DataArray_t", 2 );
}

}
