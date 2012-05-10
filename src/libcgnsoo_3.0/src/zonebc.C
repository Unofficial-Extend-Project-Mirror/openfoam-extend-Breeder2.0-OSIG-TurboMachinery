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

/*!
 * Return the number of BC_t structures stored in the current ZoneBC_t
 * \return Number of BC_t structures
 */
int ZoneBC_t::getNbBoundaryConditions() const
{
	int nbocos;
	int ier = cg_nbocos( getFileID(), getBase().getID(), getZone().getID(), &nbocos );
	check_error( "Zone_t::getNbBoundaryConditions", "cg_cg_nbocos", ier );
	return nbocos;
}

/*!
 * Reads the information about a BC_t structure 
 * \param index       Index of the requested BC_t structure (>=0) [Input]
 * \param bcname      Name of that BC [Output]
 * \param bctype      Type of that BC (BCInflow, BCOutflow, ...) [Output]
 * \param psettype    Type of pointset usde to defined this BC region (PointRange|PointList|ElementRange|ElementList) [Output]
 * \return Handle to the requested BC_t structure
 * \throw cgns_notfound
 */
BC_t ZoneBC_t::readBC( int index, string& bcname, BCType_t& bctype, PointSetType_t& psettype ) const
{
	int npnts,   nlistflag, ndataset;
	cgnsstring   boconame;
	DataType_t   normaltype;
	Array<int>   normalindices(3);
	int ier = cg_boco_info( getFileID(), getBase().getID(), getZone().getID(), ++index,
				boconame, &bctype, &psettype, &npnts, 
				normalindices, &nlistflag, &normaltype, &ndataset );
	check_found( "ZoneBC_t::readBC", "BC_t", ier );
	check_error( "ZoneBC_t::readBC", "cg_boco_info", ier );
	bcname = boconame;

	return BC_t(push("BC_t",index));
}

/*!
 * Writes the information about a BC_t structure - valid for PointList and ElementList
 * \param bcname      Name of that BC
 * \param bctype      Type of that BC (BCInflow, BCOutflow, ...)
 * \param psettype    Type of pointset usde to defined this BC region (PointRange,PointList,ElementRange,ElementRlist)
 * \param points      vector of indices to the points or elements defining this BC region
 * \return Handle to the new BC_t structure
 */
BC_t ZoneBC_t::writeBC( const string& bcname, BCType_t bctype, PointSetType_t psettype, const vector<int>& points )
{
	if ( psettype != PointList && psettype != ElementList )
		throw cgns_badargument( "ZoneBC_t::writeBC", "PointSetType (argument 3) must be PointList or ElementList" );
	int ibc;
	Array<int> apts( points );
	int ier = cg_boco_write( getFileID(), getBase().getID(), getZone().getID(), bcname.c_str(),
				 bctype, psettype, apts.size(), apts, &ibc );
	check_error( "ZoneBC_t::writeBC", "cg_boco_write", ier );
	return BC_t(push( "BC_t", ibc ));
}

/*!
 * Writes the information about a BC_t structure - valid for PointRange and ElementRange
 * \param bcname      Name of that BC
 * \param bctype      Type of that BC (BCInflow, BCOutflow, ...)
 * \param psettype    Type of pointset usde to defined this BC region (PointRange,PointList,ElementRange,ElementRlist)
 * \param points      range of point indices or element indices defining this BC region
 * \return Handle to the new BC_t structure
 */
BC_t ZoneBC_t::writeBC( const string& bcname, BCType_t bctype, PointSetType_t psettype, const range& points )
{
	if ( psettype != PointRange && psettype != ElementRange )
		throw cgns_badargument( "ZoneBC_t::writeBC", "PointSetType (argument 3) must be PointRange or ElementRange" );
	int ibc;
	Array<int> apts( points );
	int ier = cg_boco_write( getFileID(), getBase().getID(), getZone().getID(), bcname.c_str(),
				 bctype, psettype, 2, apts, &ibc );
	check_error( "ZoneBC_t::writeBC", "cg_boco_write", ier );
	return BC_t(push( "BC_t", ibc ));
}


}
