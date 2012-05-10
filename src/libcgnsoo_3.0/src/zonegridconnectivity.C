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

//----------------- one-to-one connectivity ---------------------------

int ZoneGridConnectivity_t::getNbGridConnectivity1to1() const
{
	int n1to1;
	int ier = cg_n1to1( getFileID(), getBase().getID(), getZone().getID(), &n1to1 );
	check_error( "ZoneGridConnectivity_t::getNbGridConnectivity1to1", "cg_n1to1", ier );
	return n1to1;
}

GridConnectivity1to1_t ZoneGridConnectivity_t::readGridConnectivity1to1( int index, string& connecname, string& donorname,
					vector<int>& range, vector<int>& donorrange, vector<int>& transform )  const
{
	index++;
	
	cgnsstring cname;
	cgnsstring dname;
	Array<int> itr(3);
	Array<int> irange(6);
	Array<int> idrange(6);
	int ier = cg_1to1_read( getFileID(), getBase().getID(), getZone().getID(), index, cname, dname, irange, idrange, itr );
	check_error( "ZoneGridConnectivity_t::readGridConnectivity1to1", "cg_1to1_read", ier );

	connecname = cname;
	donorname  = dname;

	transform = itr;
	range = irange;
	donorrange = idrange;

	return GridConnectivity1to1_t(push( "GridConnectivity1to1_t", index ));
}

GridConnectivity1to1_t ZoneGridConnectivity_t::readGridConnectivity1to1( 
	int index, string& connecname, string& donorname,
	range& r, range& d, vector<int>& transform )  const
{
	index++;
	
	cgnsstring cname, dname;
	Array<int> irange(6);
	Array<int> idrange(6);
	Array<int> itr(3);
	int ier = cg_1to1_read( getFileID(), getBase().getID(), getZone().getID(), index, cname, dname, irange, idrange, itr );
	check_error( "ZoneGridConnectivity_t::readGridConnectivity1to1", "cg_1to1_read", ier );

	connecname = cname;
	donorname  = dname;

	transform = itr;
	r.resize(3);
	d.resize(3);
	r = irange;
	d = idrange;

	return GridConnectivity1to1_t(push( "GridConnectivity1to1_t", index ));
}

GridConnectivity1to1_t ZoneGridConnectivity_t::writeGridConnectivity1to1(
	const string& connecname, const string& donorname,
	const vector<int>& range, const vector<int>& donorrange,
	const vector<int>& transform )
{
	int gc11;
	Array<int> arange(range);
	Array<int> adrange(donorrange);
	Array<int> atransf(transform);
	int ier = cg_1to1_write( getFileID(), getBase().getID(), getZone().getID(),
				 connecname.c_str(), donorname.c_str(),
				 arange, adrange, atransf, &gc11 );
	check_error( "ZoneGridConnectivity_t::writeGridConnectivity1to1", "cg_1to1_write", ier );
	return GridConnectivity1to1_t(push( "GridConnectivity1to1_t", gc11 ));
}

GridConnectivity1to1_t ZoneGridConnectivity_t::writeGridConnectivity1to1( 
	const string& connecname, const string& donorname,
	const range& r, const range& donorrange, const vector<int>& transform )
{
	int gc11;
	Array<int> arange(r);
	Array<int> adrange(donorrange);
	Array<int> atransf(transform);
	int ier = cg_1to1_write( getFileID(), getBase().getID(), getZone().getID(),
				 connecname.c_str(), donorname.c_str(),
				 arange, adrange, atransf, &gc11 );
	check_error( "ZoneGridConnectivity_t::writeGridConnectivity1to1", "cg_1to1_write", ier );
	return GridConnectivity1to1_t(push( "GridConnectivity1to1_t", gc11 ));
}

//----------------- general grid connectivity ---------------------------

int ZoneGridConnectivity_t::getNbGridConnectivity() const
{
	int nconn;
	int ier = cg_nconns( getFileID(), getBase().getID(), getZone().getID(), &nconn );
	check_error( "ZoneGridConnectivity_t::getNbGridConnectivity", "cg_nconns", ier );
	return nconn;
}

GridConnectivity_t ZoneGridConnectivity_t::readGridConnectivity( int index, const vector<int>& points, vector<int>& donordata ) const
{
	//index++;
	return *this;
}
 
// ---- TODO ------
// General connectivity writing
//   There should be only two possible situations:
//     * PointRange + PointListDonor
//     * PointList  + CellListDonor
//   There should be one overloaded method for each case 
//   thus removing the need for PointSetType_t as argument
//   As a utility, the combination PointRange+CellListDonor could be 
//   accepted and internally generate a list of points for the receiver Zone_t
//------------------
GridConnectivity_t ZoneGridConnectivity_t::writeGridConnectivity( const string& connecname,
					GridLocation_t gloc, GridConnectivityType_t gcnc,
					PointSetType_t pset, int indexdim, const vector<int>& points,
					const string& donorname, ZoneType_t donorzonetype,
					PointSetType_t donorpsettype, const vector<int>& donordata )
{
	int icnc;
	Array<int> apts( points );
	Array<int> addata( donordata );
	int ier = cg_conn_write( getFileID(), getBase().getID(), getZone().getID(),
				 connecname.c_str(), gloc, gcnc, pset, points.size()/indexdim, apts,
				 donorname.c_str(), donorzonetype, donorpsettype,
				 Integer, donordata.size()/indexdim, addata, &icnc );
	check_error( "ZoneGridConnectivity_t::writeGridConnectivity", "cg_conn_write", ier );
	return GridConnectivity_t(push( "GridConnectivity_t", icnc ));
}

GridConnectivity_t ZoneGridConnectivity_t::writeGridConnectivity( const string& connecname,
					GridLocation_t gloc, GridConnectivityType_t gcnc, int indexdim,
					const range& rangepoints,
					const string& donorname, ZoneType_t donorzonetype,
					PointSetType_t donorpsettype, const vector<int>& donordata )
{
	int icnc;
	Array<int> arange( rangepoints );
	Array<int> addata( donordata );
	int ier = cg_conn_write( getFileID(), getBase().getID(), getZone().getID(),
				 connecname.c_str(), gloc, gcnc, PointRange, 2, arange,
				 donorname.c_str(), donorzonetype, donorpsettype,
				 Integer, donordata.size()/indexdim, addata, &icnc );
	check_error( "ZoneGridConnectivity_t::writeGridConnectivity", "cg_conn_write", ier );
	return GridConnectivity_t(push( "GridConnectivity_t", icnc ));
}

//----------------- overset holes ---------------------------

int ZoneGridConnectivity_t::getNbOversetHoles() const
{
	int nholes;
	int ier = cg_nholes( getFileID(), getBase().getID(), getZone().getID(), &nholes );
	check_error( "ZoneGridConnectivity_t::getNbOversetHoles", "cg_nholes", ier );
	return nholes;
}

void ZoneGridConnectivity_t::getOversetHolesInfo( 
	int index, string& holename, GridLocation_t& gloc, 
	PointSetType_t& pstype, int& nps, int& np ) const
{
	++index;
	
	cgnsstring name;
	GridLocation_t location;
	int ier = cg_hole_info( getFileID(), getBase().getID(), getZone().getID(),
				index, name, &location, &pstype, &nps, &np );
	check_error( "ZoneGridConnectivity_t::readOversetHoles", "cg_hole_info", ier );
	holename = name;
	gloc = location;
}

// writing a hole using a list of points
OversetHoles_t ZoneGridConnectivity_t::writeOversetHoles( const string& holename, GridLocation_t gloc, const vector<int>& points )
{
	int indexdim = getZone().getIndexDimension();
	int icnc;
	Array<int> apts(points);
	int ier = cg_hole_write( getFileID(), getBase().getID(), getZone().getID(),
				 holename.c_str(), gloc, PointList, 1, points.size()/indexdim, apts,
				 &icnc );
	check_error( "ZoneGridConnectivity_t::writeOversetHoles", "cg_hole_write", ier );
	return OversetHoles_t(push( "OversetHoles_t", icnc ));
}

// writing a hole using a range of points
OversetHoles_t ZoneGridConnectivity_t::writeOversetHoles( const string& holename, GridLocation_t gloc, const range& rangepoints )
{
	int icnc;
	Array<int> arange(rangepoints);
	int ier = cg_hole_write( getFileID(), getBase().getID(), getZone().getID(),
				 holename.c_str(), gloc, PointRange, 1, 2, arange,
				 &icnc );
	check_error( "ZoneGridConnectivity_t::writeOversetHoles", "cg_hole_write", ier );
	return OversetHoles_t(push( "OversetHoles_t", icnc ));
}

// reading a hole - user must call readOversetHoleInfo in order to know how to interpret "points"
OversetHoles_t ZoneGridConnectivity_t::readOversetHoles( int index, vector<int>& points )
{
	++index;
	
	cgnsstring holename;
	GridLocation_t location;
	PointSetType_t pstype;
	int nps, np;
	int ier = cg_hole_info( getFileID(), getBase().getID(), getZone().getID(),
				index, holename, &location, &pstype, &nps, &np );
	check_error( "ZoneGridConnectivity_t::readOversetHoles", "cg_hole_info", ier );
	
	int ntot = np*nps;
	Array<int> apts(ntot);
	ier = cg_hole_read( getFileID(), getBase().getID(), getZone().getID(), index, apts );
	check_error( "ZoneGridConnectivity_t::readOversetHoles", "cg_hole_read", ier );
	points = apts;
	
	return OversetHoles_t(push( "OversetHoles_t", index ));
}


}
