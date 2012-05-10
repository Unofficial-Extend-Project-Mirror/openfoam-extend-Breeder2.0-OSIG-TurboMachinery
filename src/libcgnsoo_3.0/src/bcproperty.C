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
 * Reads the WallFunction associated with this BCProperty_t
 * \param wft Type of wall function (Null, Generic, UserDefined) [Output]
 * \return Handle to the WallFunction_t  
 */
WallFunction_t BCProperty_t::readWallFunction( WallFunctionType_t& wft ) const
{
	int ibc = parent().getID();  // parent is a BC_t
	int ier = cg_bc_wallfunction_read( getFileID(), getBase().getID(), getZone().getID(), ibc, &wft );
	check_found( "BCProperty_t::readWallFunction", "WallFunction_t", ier );
	check_error( "BCProperty_t::readWallFunction", "cg_bc_wallfunction_read", ier );
	return WallFunction_t( push( "WallFunction_t", 1 ) );
}

/*!
 * Writes a WallFunction associated with this BCProperty_t
 * \param wft Type of wall function (Null, Generic, UserDefined) [Input]
 * \return Handle to the created WallFunction_t  
 */
WallFunction_t BCProperty_t::writeWallFunction( WallFunctionType_t wf )
{
	int ibc = parent().getID();  // parent is a BC_t
	int ier = cg_bc_wallfunction_write( getFileID(), getBase().getID(), getZone().getID(), ibc, wf );
	check_error( "BCProperty_t::writeWallFunction", "cg_bc_wallfunction_write", ier );
	return WallFunction_t( push( "WallFunction_t", 1 ) );
}

/*!
 * Reads the Area structure associated with this BCProperty_t
 * \param atype   Type of Area definition (BleedArea, CapturArea, ...)
 * \param surface Surface area
 * \param region  Region name
 * \return Handle to the Area_t  
 */
Area_t BCProperty_t::readArea( AreaType_t& atype, double& surface, string& region ) const
{
	int ibc = parent().getID();
	float sa;
	cgnsstring rn;
	int ier = cg_bc_area_read( getFileID(), getBase().getID(), getZone().getID(), ibc, &atype, &sa, rn );
	check_found( "BCProperty_t::readArea", "Area_t", ier );
	check_error( "BCProperty_t::readArea", "cg_bc_area_read", ier );
	surface = sa;
	region  = rn;
	return Area_t( push( "Area_t", 1 ) );
}

/*!
 * Writes an Area structure associated with this BCProperty_t
 * \param atype   Type of Area definition (BleedArea, CapturArea, ...) [Output]
 * \param surface Surface area [Output]
 * \param region  Region name [Output]
 * \return Handle to the created Area_t  
 */
Area_t BCProperty_t::writeArea( AreaType_t a, double surface, const string& region )
{
	int ibc = parent().getID();
	int ier = cg_bc_area_write( getFileID(), getBase().getID(), getZone().getID(), ibc, a, surface, region.c_str() );
	check_error( "BCProperty_t::writeArea", "cg_bc_area_write", ier );
	return Area_t( push( "Area_t", 1 ) );
}

}
