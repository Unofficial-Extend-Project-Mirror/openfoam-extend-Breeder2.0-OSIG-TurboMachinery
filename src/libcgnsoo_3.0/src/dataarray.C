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

void DataArray_t::readData( vector<int>& data ) const
{
	parent().go_here();
	int index = getID();
	
	cgnsstring array_name;
	DataType_t datatype;
	int ndim;
	int dimvect[32];
	int ier = cg_array_info( index, array_name, &datatype, &ndim, dimvect );
	check_error( "DataArray_t::readData", "cg_array_info", ier );
	if ( datatype != Integer )
	{
		string type_name = datatype_to_name( datatype );

  		throw cgns_mismatch( "DataArray_t::readData", "cannot read a DataArray<" + type_name + "> into a vector<int>" );
	}

	int totsize = 1;
	for ( int i=0 ; i<ndim ; i++ )
		totsize *= dimvect[i];
	
	Array<int> adata(totsize);
	ier = cg_array_read( index, adata );
	data = adata;
	
	check_error( "DataArray_t::readData", "cg_array_read", ier );
}

void DataArray_t::readData( vector<float>& data ) const
{
	parent().go_here();
	int index = getID();
	
	cgnsstring array_name;
	DataType_t datatype;
	int ndim;
	int dimvect[32];
	int ier = cg_array_info( index, array_name, &datatype, &ndim, dimvect );
	check_error( "DataArray_t::readData", "cg_array_info", ier );

	int totsize = 1;
	for ( int i=0 ; i<ndim ; i++ )
		totsize *= dimvect[i];
	data.resize( totsize );
	
	Array<float> adata(totsize);
	ier = cg_array_read_as( index, RealSingle, adata );
	check_error( "DataArray_t::readData", "cg_array_read", ier );
	data = adata;
}

void DataArray_t::readData( vector<double>& data )  const
{
	parent().go_here();
	int index = getID();
	
	cgnsstring array_name;
	DataType_t datatype;
	int ndim;
	int dimvect[32];
	int ier = cg_array_info( index, array_name, &datatype, &ndim, dimvect );
	check_error( "DataArray_t::readData", "cg_array_info", ier );

	int totsize = 1;
	for ( int i=0 ; i<ndim ; i++ )
		totsize *= dimvect[i];
	data.resize( totsize );

	Array<double> adata(totsize);
	ier = cg_array_read_as( index, RealDouble, adata );
	check_error( "DataArray_t::readData", "cg_array_read", ier );
	data = adata;
}

}
