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

int FlowSolution_t::getNbFields() const
{
	int nfields;
	int ier = cg_nfields( getFileID(), getBase().getID(), getZone().getID(), getID(), &nfields );
	check_error( "FlowSolution::getNbFields", "cg_nfields", ier );
	return nfields;
}

DataArray_t FlowSolution_t::readField( int index, string& name, DataType_t& type ) const
{
	index++;
	
	cgnsstring s;
	int ier = cg_field_info( getFileID(), getBase().getID(), getZone().getID(), getID(), index, &type, s );
	check_error( "FlowSolution::readField", "cg_field_read", ier );
	name = s;
	return DataArray_t( push("DataArray_t",index) );
}

DataArray_t FlowSolution_t::writeField( const string& name, vector<float>& values )
{
	int ifield;
	Array<float> avalues( values );
	int ier = cg_field_write( getFileID(), getBase().getID(), getZone().getID(), getID(), RealSingle, name.c_str(), avalues, &ifield );
	check_error( "FlowSolution::writeField", "cg_field_write", ier );
	return DataArray_t( push("DataArray_t",ifield) );
}

DataArray_t FlowSolution_t::writeField( const string& name, vector<double>& values )
{
	int ifield;
	Array<double> avalues( values );
	int ier = cg_field_write( getFileID(), getBase().getID(), getZone().getID(), getID(), RealDouble, name.c_str(), avalues, &ifield );
	check_error( "FlowSolution::writeField", "cg_field_write", ier );
	return DataArray_t( push("DataArray_t",ifield) );
}

}
