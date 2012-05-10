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
const string GeometryReference_t::attrib_NbGeoParts( "NbGeoParts" );

void GeometryReference_t::writePart( const string& partname )
{
	int ipart;
	int ifam = parent().getID();
	int igeo = getID();
	int ier = cg_part_write( getFileID(), getBase().getID(), ifam, igeo, partname.c_str(), &ipart );
	check_error( "GeometryReference_t::writePart", "cg_part_write", ier );
	set_attribute( "NbGeoParts", ipart );
}

void GeometryReference_t::readPart( int ipart, string& partname ) const
{
	cgnsstring name;
	int ifam = parent().getID();
	int igeo = getID();
	int ier = cg_part_read( getFileID(), getBase().getID(), ifam, igeo, ipart, name );
	check_error( "GeometryReference_t::readPart", "cg_part_read", ier );
	partname = name;
}

}
