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
 * Reads the point range associated with this BCDataSet_t
 * \param ptrange point range [Output]
 */
void BCDataSet_t::readPointRange( range& ptrange ) const
{
	if ( parent().isA( "BC_t" ) )
	{
		go_here();
		
		PointSetType_t psettype;
		int npts;
		int ier = cg_ptset_info( &psettype, &npts );
		check_error( "BCDataSet_t::readPoinRange", "cg_ptset_info", ier );
		
		if ( psettype != PointRange )
			throw cgns_mismatch( "BCDataSet_t::readPoinRange", "This BCDataSet_t is not defined with a PointRange" );

		int physdim = getPhysicalDimension();
		Array<int> pts(npts*physdim);
		ier = cg_ptset_read( pts );
		check_found( "BCDataSet_t::readPoinRange", "IndexRange_t", ier );
		check_error( "BCDataSet_t::readPoinRange", "cg_ptset_read", ier );
		
		ptrange = pts;
	}
	else if ( parent().isA( "FamilyBC_t" ) )
	{
		/* TODO */
	}
	throw std::logic_error( "BCDataSet_t::readBCData - immediate parent is not a BC_t nor a FamilyBC_t ??? !!!" );
}

/*!
 * Reads the point list associated with this BCDataSet_t
 * \param ptlist point list [Output]
 */
void BCDataSet_t::readPointList( vector<int>& ptlist ) const
{
	if ( parent().isA( "BC_t" ) )
	{
		go_here();
		
		PointSetType_t psettype;
		int npts;
		int ier = cg_ptset_info( &psettype, &npts );
		check_error( "BCDataSet_t::readPoinRange", "cg_ptset_info", ier );
		
		if ( psettype != PointList )
			throw cgns_mismatch( "BCDataSet_t::readPoinRange", "This BCDataSet_t is not defined with a PointList" );

		Array<int> pts(npts);
		ier = cg_ptset_read( pts );
		check_found( "BCDataSet_t::readPoinRange", "IndexRange_t", ier );
		check_error( "BCDataSet_t::readPoinRange", "cg_ptset_read", ier );
		
		ptlist = pts;
	}
	else if ( parent().isA( "FamilyBC_t" ) )
	{
		/* TODO */
	}
	throw std::logic_error( "BCDataSet_t::readBCData - immediate parent is not a BC_t nor a FamilyBC_t ??? !!!" );
}

/*!
 * Writes the point range associated with this BCDataSet_t
 * \param ptrange point range [Input]
 */
void BCDataSet_t::writePointRange( const range& ptrange ) const
{
	if ( parent().isA( "BC_t" ) )
	{
		go_here();
		
		Array<int> pts(ptrange);
		int ier = cg_ptset_write( PointRange, 2, pts );
		check_error( "BCDataSet_t::writePoinRange", "cg_ptset_write", ier );
	}
	else if ( parent().isA( "FamilyBC_t" ) )
	{
		/* TODO */
	}
	throw std::logic_error( "BCDataSet_t::writeBCData - immediate parent is not a BC_t nor a FamilyBC_t ??? !!!" );
}

/*!
 * Writes the point list associated with this BCDataSet_t
 * \param ptlist point list [Input]
 */
void BCDataSet_t::writePointList( const vector<int>& ptlist ) const
{
	if ( parent().isA( "BC_t" ) )
	{
		go_here();
		
		Array<int> pts(ptlist);
		int ier = cg_ptset_write( PointList, ptlist.size(), pts );
		check_error( "BCDataSet_t::writePoinRange", "cg_ptset_write", ier );
	}
	else if ( parent().isA( "FamilyBC_t" ) )
	{
		/* TODO */
	}
	throw std::logic_error( "BCDataSet_t::writeBCData - immediate parent is not a BC_t nor a FamilyBC_t ??? !!!" );
}

//-------------------------------------------------------------

/*!
 * Reads the BCData associated with this BCDataSet_t
 * \param type Type of BCData (either Dirichlet or Neumann) to read
 * \return Handle to the corresponding BCData_t  
 */
BCData_t BCDataSet_t::readBCData( BCDataType_t type ) const
{
	if ( parent().isA( "BC_t" ) )
	{
		// We probably should check that it exists and return cgns_notfound if not!
		return BCData_t( push( "BCData_t", type ) );  // usage of 'type' - see CgnsTalk 2003/10/03 and 2003/11/03 by Florent Cayre, Snecma
							      // see also the documentation of cg_goto
	}
	else if ( parent().isA( "FamilyBC_t" ) )
	{
		return BCData_t( push( "BCData_t", type ) );
	}
	throw std::logic_error( "BCDataSet_t::readBCData - immediate parent is not a BC_t nor a FamilyBC_t ??? !!!" );
}

/*!
 * Writes the BCData associated with this BCDataSet_t
 * \param dtype  Type of BCData (Dirichlet or Neumann)
 * \return Handle to the newly writen BCData_t  
 */
BCData_t BCDataSet_t::writeBCData( BCDataType_t dtype )
{
	if ( parent().isA( "BC_t" ) )
	{
		// we are in <Base/Zone/ZoneBC/BC/BCDataSet> - get indices of all nodes in the hierarchy
		int ids   = getID();
		int ibc   = parent().getID();
		int izone = parent().parent().parent().getID();
		int ibase = parent().parent().parent().parent().getID();
		int ier = cg_bcdata_write( getFileID(), ibase, izone, ibc, ids, dtype );
		check_error( "BCDataSet_t::writeBCData", "cg_bcdata_write", ier );
		return BCData_t( push( "BCData_t", dtype ) ); // see documentation of cg_goto for the peculiar usage of dtype here
	}
	else if ( parent().isA( "FamilyBC_t" ) )
	{
		// the BCType goes into the BCDataSet - not yet written!
		// write the BCDataSet (or overwrite it if it already exists)
		parent().go_here(); // point to FamilyBC_t
		// get dataset name and BCType from attribute info (see FamilyBC_t class)
		string dsn;
		BCType_t bct;
		get_attribute( "Name", dsn );
		get_attribute( "BCType", bct );
		int ier = cg_bcdataset_write( const_cast<char*>(dsn.c_str()), bct, dtype );
		check_error( "BCDataSet_t::writeBCData", "cg_bcdataset_write", ier );

		return BCData_t( push( "BCData_t", dtype ) );
	}
	throw std::logic_error( "BCDataSet_t::writeBCData - immediate parent is not a BC_t nor a FamilyBC_t ??? !!!" );
}

}
