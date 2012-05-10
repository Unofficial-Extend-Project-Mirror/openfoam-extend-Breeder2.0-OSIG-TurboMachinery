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
#include <sstream>
#include <algorithm>
#include <cstdlib>
#include "Array.H"
#include "cgnsoo.H"

namespace CGNSOO
{

int GridCoordinates_t::get_dataarray_index( const string& coordname ) const
{
	// find the index of the DataArray_t whose name is 'coordname'
	go_here();
	int nda;
	int iera = cg_narrays( &nda );
	check_error( "GridCoordinates_t::get_dataarray_index", "cg_narrays", iera );
	int dataindex = -1;
	for ( int i=0 ; i<nda ; i++ )
	{
		cgnsstring aname;
		DataType_t dtype;
		int ndim;
		int dims[3];
		int ier = cg_array_info( i+1, aname, &dtype, &ndim, dims );
		check_error( "GridCoordinates_t::get_dataarray_index", "cg_array_info", ier );
		//std::cerr << "compare with array name = \"" << aname << "\"\n";
		if ( coordname == aname ) { dataindex=i; break; }
	}
	if ( dataindex<0 )
	{
		std::ostringstream s;
		s << "this coordinate name (" << coordname << ") does not exist in current GridCoordinates_t";
		std::cerr << s.str() << endl;
		abort();
		throw cgns_mismatch( "GridCoordinates_t::get_dataarray_index", s.str() );
	}
	return dataindex;
}

int GridCoordinates_t::getNbCoordinatesData() const
{
	int nda;
	int ier = cg_ncoords( getFileID(), getBase().getID(), getZone().getID(), &nda );
	check_error( "GridCoordinates_t::getNbCoordinatesData", "cg_ncoord", ier );
	return nda;
}

void GridCoordinates_t::getCoordinatesDataInfo( int index, string& dataname, DataType_t& type ) const
{
	cgnsstring name;
	int ier = cg_coord_info( getFileID(), getBase().getID(), getZone().getID(), ++index,
				 &type, name );
	check_error( "GridCoordinates_t::getCoordinatesDataInfo", "cg_coord_info", ier );
	dataname = name;
}

DataArray_t GridCoordinates_t::readCoordinatesData( const string& coordname, const range& r, vector<float>& coo ) const
{
	int ntot = r.dim();
	Array<float> acoo(ntot);
	Array<int>   arange(r);
	
	int ier = cg_coord_read( getFileID(), getBase().getID(), getZone().getID(), 
				 coordname.c_str(), RealSingle, arange, arange+3, acoo );
	check_error( "GridCoordinates_t::readGridCoordinatesData", "cg_coord_read", ier );
	coo = acoo;
	int dataindex = get_dataarray_index( coordname );
	return DataArray_t(push( "DataArray_t", dataindex ));
}

DataArray_t GridCoordinates_t::readCoordinatesData( const string& coordname, const range& r, vector<double>& coo ) const
{
	int ntot = r.dim();
	Array<double> acoo(ntot);
	Array<int>    arange(r);
	
	int ier = cg_coord_read( getFileID(), getBase().getID(), getZone().getID(), 
				 coordname.c_str(), RealDouble, arange, arange+3, acoo );
	check_error( "GridCoordinates_t::readGridCoordinatesData", "cg_coord_read", ier );
	coo = acoo;
	int dataindex = get_dataarray_index( coordname );
	return DataArray_t(push( "DataArray_t", dataindex ));
}

DataArray_t GridCoordinates_t::readCoordinatesData( const string& coordname, vector<float>& coo ) const
{
	// find out the number of vertices to read and adjust coo vector size accordingly
	cgnsstring zname;
	int nvertex[9];
	int ierz = cg_zone_read( getFileID(), getBase().getID(), getZone().getID(), zname, nvertex );
	check_error( "GridCoordinates_t::readCoordinatesData", "cg_zone_read", ierz );
	int ndim = getZone().getIndexDimension();
	int nv = 1;
	for ( int i=0 ; i<ndim ; i++ )
		nv *= nvertex[i];
	Array<float> acoo(nv);

	// select a range to read everything
	int rangemin[3] = {1,1,1};
	int rangemax[3];
	rangemax[0] = nvertex[0];
	rangemax[1] = nvertex[1];
	rangemax[2] = nvertex[2];

	// find out the index of datarray whose name is 'coordname'
	int dataindex = get_dataarray_index( coordname );

	int gcid = getID();
	if ( gcid==1 )
	{
		int ier = cg_coord_read( getFileID(), getBase().getID(), getZone().getID(), 
					 coordname.c_str(), RealSingle,
					 rangemin, rangemax, acoo );
		check_error( "GridCoordinates_t::readCoordinatesData", "cg_coord_read", ier );
	}
	else
	{
		go_here();
		int ier = cg_array_read_as( dataindex, RealSingle, acoo );
		check_error( "GridCoordinates_t::readCoordinatesData", "cg_array_read_as", ier );
	}
	coo = acoo;
	
	return DataArray_t(push( "DataArray_t", dataindex ));
}

DataArray_t GridCoordinates_t::readCoordinatesData( const string& coordname, vector<double>& coo ) const
{
	// find out the number of vertices to read and adjust coo vector size accordingly
	cgnsstring zname;
	int nvertex[9];
	int ierz = cg_zone_read( getFileID(), getBase().getID(), getZone().getID(), zname, nvertex );
	check_error( "GridCoordinates_t::readCoordinatesData", "cg_zone_read", ierz );
	int ndim = getZone().getIndexDimension();
	int nv = 1;
	for ( int i=0 ; i<ndim ; i++ )
		nv *= nvertex[i];
	Array<double> acoo(nv);

	// select a range to read everything
	int rangemin[3] = {1,1,1};
	int rangemax[3];
	rangemax[0] = nvertex[0];
	rangemax[1] = nvertex[1];
	rangemax[2] = nvertex[2];

	// find out the index of datarray whose name is 'coordname'
	int dataindex = get_dataarray_index( coordname );

	int gcid = getID();
	if ( gcid==1 )
	{
		int ier = cg_coord_read( getFileID(), getBase().getID(), getZone().getID(), coordname.c_str(), RealDouble,
					 rangemin, rangemax, acoo );
		check_error( "GridCoordinates_t::readCoordinatesData", "cg_coord_read", ier );
	}
	else
	{
		go_here();
		int ier = cg_array_read_as( dataindex, RealDouble, acoo );
		check_error( "GridCoordinates_t::readCoordinatesData", "cg_array_read_as", ier );
	}
	coo = acoo;
	
	return DataArray_t(push( "DataArray_t", dataindex ));
}

DataArray_t GridCoordinates_t::writeCoordinatesData( const string& coordname, const vector<float>& coo )
{
	int gcid = getID();
	int cooid;
	Array<float> acoo( coo );
	if ( gcid==1 )
	{
		int ier = cg_coord_write( getFileID(), getBase().getID(), getZone().getID(), RealSingle, coordname.c_str(), acoo, &cooid );
		check_error( "GridCoordinates_t::writeCoordinates", "cg_coord_write", ier );
	}
	else
	{
		go_here();
		int length = coo.size();
		int ier = cg_array_write( coordname.c_str(), RealSingle, 1, &length, acoo );
		check_error( "GridCoordinates_t::writeCoordinates", "cg_array_write", ier );
		cooid = 1;
	}
	return DataArray_t(push( "DataArray_t", cooid ));
}

DataArray_t GridCoordinates_t::writeCoordinatesData( const string& coordname, const vector<double>& coo )
{
	int gcid = getID();
	int cooid;
	Array<double> acoo( coo );
	if ( gcid==1 )
	{
		int ier = cg_coord_write( getFileID(), getBase().getID(), getZone().getID(), RealDouble, coordname.c_str(), acoo, &cooid );
		check_error( "GridCoordinates_t::writeCoordinates", "cg_coord_write", ier );
	}
	else
	{
		go_here();
		int length = coo.size();
		int ier = cg_array_write( coordname.c_str(), RealDouble, 1, &length, acoo );
		check_error( "GridCoordinates_t::writeCoordinates", "cg_array_write", ier );
		cooid = 1;
	}
	return DataArray_t(push( "DataArray_t", cooid ));
}

void GridCoordinates_t::writeCoordinatesData( coordinatesystem_t syscoo, const vector<double>& coo1, const vector<double>& coo2, const vector<double>& coo3 )
{
        if ( coo1.size() != coo2.size() || coo1.size() != coo3.size() )
        	throw cgns_mismatch( "GridCoordinates_t::writeCoordinatesData", "coordinates vectors passed in do not have same size" );
	
	const cgnsstring x_string = "CoordinateX";
	const cgnsstring y_string = "CoordinateY";
	const cgnsstring z_string = "CoordinateZ";
	const cgnsstring r_string = "CoordinateR";
	const cgnsstring t_string = "CoordinateTheta";
	const cgnsstring p_string = "CoordinatePhi";
	
	const char* dni[3];
	switch( syscoo )
	{
	case CARTESIAN:
		dni[0] = x_string;
		dni[1] = y_string;
		dni[2] = z_string;
		break;
	case CYLINDRICAL:
		dni[0] = r_string;
		dni[1] = t_string;
		dni[2] = z_string;
		break;
	case SPHERICAL:
		dni[0] = r_string;
		dni[1] = t_string;
		dni[2] = p_string;
		break;
	}
	Array<double> acoo1(coo1);
	Array<double> acoo2(coo2);
	Array<double> acoo3(coo3);
	int ier;
	if ( getID()==1 ) // first set of grid coordinates is handled through special functions
	{
		int cooid;
		ier = cg_coord_write( getFileID(), getBase().getID(), getZone().getID(), RealDouble, dni[0], acoo1, &cooid );
		check_error( "GridCoordinates_t::writeCoordinates", "cg_coord_write", ier );
		ier = cg_coord_write( getFileID(), getBase().getID(), getZone().getID(), RealDouble, dni[1], acoo2, &cooid );
		check_error( "GridCoordinates_t::writeCoordinates", "cg_coord_write", ier );
		ier = cg_coord_write( getFileID(), getBase().getID(), getZone().getID(), RealDouble, dni[2], acoo3, &cooid );
		check_error( "GridCoordinates_t::writeCoordinates", "cg_coord_write", ier );
	}
	else
	{
		go_here();
		int length = coo1.size();
		ier = cg_array_write( dni[0], RealDouble, 1, &length, acoo1 );
		check_error( "GridCoordinates_t::writeCoordinates", "cg_array_write", ier );
		ier = cg_array_write( dni[1], RealDouble, 1, &length, acoo2 );
		check_error( "GridCoordinates_t::writeCoordinates", "cg_array_write", ier );
		ier = cg_array_write( dni[2], RealDouble, 1, &length, acoo3 );
		check_error( "GridCoordinates_t::writeCoordinates", "cg_array_write", ier );
	}
}

	
void GridCoordinates_t::readRind( vector<int>& rinddata ) const
{
	go_here();
	Array<int> rd(6);
	int ier = cg_rind_read( rd );
	check_error( "GridCoordinates_t::readRind", "cg_rind_read", ier );
	rinddata = rd;
}

void GridCoordinates_t::writeRind( const vector<int>& rinddata )
{
        if ( rinddata.size() != 6 )
        	throw cgns_mismatch( "GridCoordinates_t::writeRind", "vector passed in must contain exactly 6 values" );
	go_here();
	Array<int> rd(rinddata);
	int ier = cg_rind_write( rd );
	check_error( "GridCoordinates_t::writeRind", "cg_rind_write", ier );
}

}

