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
#include "cgnsoo.H"

namespace CGNSOO
{

const string Family_t::attrib_HasFamilyBC = "HasFamilyBC";
const string Family_t::attrib_NbGeoRef    = "NbGeoRef";

FamilyBC_t Family_t::readFamilyBC( /*int index,*/ string& fbcname, BCType_t& t ) const
{
	int index = 1; // there is only one FamilyBC_t under a family
	cgnsstring name;
	int ier = cg_fambc_read( getFileID(), getBase().getID(), getID(), index, name, &t );
	check_found( "Family_t::readFamilyBC", "FamilyBC_t", ier );
	check_error( "Family_t::readFamilyBC", "cg_fambc_read", ier );
	fbcname = name;
	FamilyBC_t temp_fbc( push( "FamilyBC_t", index ), 0 );
	// get the number of dataset under this familyBC
	temp_fbc.go_here();
	int nds;
	ier = cg_bcdataset_info( &nds );
	check_error( "Family_t::readFamilyBC", "cg_bcdataset_info", ier );
	// set the attribute accordingly
	return FamilyBC_t( push( "FamilyBC_t", index ), nds );
}

FamilyBC_t Family_t::writeFamilyBC( const string& fbcname, BCType_t t )
{
	int ifbc;
	int ier = cg_fambc_write( getFileID(), getBase().getID(), getID(), fbcname.c_str(), t, &ifbc );
	check_error( "Family_t::writeFamilyBC", "cg_fambc_write", ier );
	set_attribute( "NbFamilyBC", ifbc );
	return FamilyBC_t( push( "FamilyBC_t", ifbc ), 0 );
}

GeometryReference_t Family_t::readGeoRef( int index, string& geoname, string& filename, string& fileformat ) const
{
	cgnsstring gname;
	char*      fname;
	cgnsstring ffmt;
	int        nparts;

	int ier = cg_geo_read( getFileID(), getBase().getID(), getID(), ++index, gname, &fname, ffmt, &nparts );
	check_found( "Family_t::readGeoRef", "GeometryReference_t", ier );
	check_error( "Family_t::readGeoRef", "cg_geo_read", ier );
	geoname    = gname;
	filename   = fname;
	fileformat = ffmt;
	CGNSfree( ffmt );
	return GeometryReference_t( push( "GeometryReference_t", index ), nparts );
}

GeometryReference_t Family_t::writeGeoRef( const string& geoname, const string& filename, const string& fileformat )
{
	int igeo;
	int ier = cg_geo_write( getFileID(), getBase().getID(), getID(), geoname.c_str(), filename.c_str(), fileformat.c_str(), &igeo );
	check_error( "Family_t::writeGeoRef", "cg_geo_write", ier );
	set_attribute( "NbGeoRef", igeo );
	return GeometryReference_t( push( "GeometryReference_t", igeo ), 0 );
}

}
