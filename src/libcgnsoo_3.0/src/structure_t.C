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

structure_t::~structure_t()
{
	if ( _nodeptr ) _nodeptr->unref(); // do not delete _nodeptr, it is reference counted
}

/*static*/ string structure_t::datatype_to_name( DataType_t t )
{
	switch( t )
	{
	case Integer    : return "Integer";
	case RealSingle : return "RealSingle";
	case RealDouble : return "RealDouble";
	case Character  : return "Character";
	case DataTypeUserDefined : return "UserDefined";
	default: return "Null";
	}
	return "Null";
}

void	structure_t::readFamilyName( string& famname ) const  { checkValid(); nd()->readFamilyName(famname); }
void	structure_t::writeFamilyName( const string& famname ) { checkValid(); nd()->writeFamilyName(famname); }

void	structure_t::readGridLocation( GridLocation_t& loc ) const { checkValid(); nd()->readGridLocation(loc); }
void	structure_t::writeGridLocation( GridLocation_t loc ) { checkValid(); nd()->writeGridLocation(loc); }

int	structure_t::getNbDescriptor() const { checkValid(); return nd()->getNbDescriptor(); }
void	structure_t::readDescriptor( int index, string& name, string& text ) const  { checkValid(); nd()->readDescriptor(index,name,text); }
void	structure_t::writeDescriptor( const string& name, const string& text ) { checkValid(); nd()->writeDescriptor(name,text); }

void	structure_t::readDataClass( DataClass_t& dclass ) const { checkValid(); nd()->readDataClass(dclass); }
void	structure_t::writeDataClass( DataClass_t dclass)  { checkValid(); nd()->writeDataClass(dclass); }

void	structure_t::readDataConversionFactors( double& scale, double& offset ) const { checkValid();  nd()->readDataConversionFactors(scale,offset); }
void	structure_t::writeDataConversionFactors( double scale, double offset )  { checkValid();  nd()->writeDataConversionFactors(scale,offset); }

void	structure_t::readDimensionalExponents( vector<double>& exponents ) const  { checkValid();  nd()->readDimensionalExponents(exponents); }
void	structure_t::writeDimensionalExponents( const vector<double>& exponents ) { checkValid();  nd()->writeDimensionalExponents(exponents); }
void	structure_t::readDimensionalExponents( DimensionalExponents& exponents ) const  { checkValid();  nd()->readDimensionalExponents(exponents); }
void	structure_t::writeDimensionalExponents( const DimensionalExponents& exponents ) { checkValid();  nd()->writeDimensionalExponents(exponents); }

void	structure_t::readDimensionalUnits( MassUnits_t& m, LengthUnits_t& l, TimeUnits_t& t, TemperatureUnits_t& temp, AngleUnits_t& a) const { checkValid();  nd()->readDimensionalUnits(m,l,t,temp,a); }
void	structure_t::writeDimensionalUnits( MassUnits_t m, LengthUnits_t l, TimeUnits_t t, TemperatureUnits_t temp, AngleUnits_t a)     { checkValid();  nd()->writeDimensionalUnits(m,l,t,temp,a); }
void	structure_t::writeSIUnits() { checkValid();  nd()->writeSIUnits(); }

DataArray_t	structure_t::readDataArrayInfo( int index, string& arrayname, DataType_t& data, vector<int>& dimensions ) const { checkValid();  return DataArray_t(nd()->readDataArrayInfo(index,arrayname,data,dimensions)); }
DataArray_t	structure_t::writeDataArray( const string& name, int value )    { checkValid();  return DataArray_t(nd()->writeDataArray(name,value)); }
DataArray_t	structure_t::writeDataArray( const string& name, float value )  { checkValid();  return DataArray_t(nd()->writeDataArray(name,value)); }
DataArray_t	structure_t::writeDataArray( const string& name, double value ) { checkValid();  return DataArray_t(nd()->writeDataArray(name,value)); }
DataArray_t	structure_t::writeDataArray( const string& name, const string& value ) { checkValid();  return DataArray_t(nd()->writeDataArray(name,value)); }
DataArray_t	structure_t::writeDataArray( const string& name, const vector<int>& dimensions, const vector<int>&    values ) { checkValid();  return DataArray_t(nd()->writeDataArray(name,dimensions,values)); }
DataArray_t	structure_t::writeDataArray( const string& name, const vector<int>& dimensions, const vector<float>&  values ) { checkValid();  return DataArray_t(nd()->writeDataArray(name,dimensions,values)); }
DataArray_t	structure_t::writeDataArray( const string& name, const vector<int>& dimensions, const vector<double>& values ) { checkValid();  return DataArray_t(nd()->writeDataArray(name,dimensions,values)); }
DataArray_t	structure_t::writeDataArray( const string& name, const vector<int>& dimensions, const vector<string>& values ) { checkValid();  return DataArray_t(nd()->writeDataArray(name,dimensions,values)); }

ReferenceState_t  structure_t::readReferenceState( string& description ) const  { checkValid();  return ReferenceState_t(nd()->readReferenceState(description)); }
ReferenceState_t  structure_t::writeReferenceState( const string& description ) { checkValid();  return ReferenceState_t(nd()->writeReferenceState(description)); }

UserDefinedData_t structure_t::readUserDefinedData( int index, string& name ) const { checkValid();  return UserDefinedData_t(nd()->readUserDefinedData(index,name)); }
UserDefinedData_t structure_t::writeUserDefinedData( const string& name )     { checkValid();  return UserDefinedData_t(nd()->writeUserDefinedData(name)); }

RotatingCoordinates_t structure_t::readRotatingCoordinates( vector<float>& ratevector, vector<float>& rotcenter ) const { checkValid();  return RotatingCoordinates_t(nd()->readRotatingCoordinates(ratevector,rotcenter)); }
RotatingCoordinates_t structure_t::writeRotatingCoordinates( const vector<float>& ratevector, const vector<float>& rotcenter )     { checkValid();  return RotatingCoordinates_t(nd()->writeRotatingCoordinates(ratevector,rotcenter)); }

ConvergenceHistory_t  structure_t::readConvergenceHistory( int& niter, string& normdef ) const { checkValid();  return ConvergenceHistory_t(nd()->readConvergenceHistory(niter,normdef)); }
ConvergenceHistory_t  structure_t::writeConvergenceHistory( int niter, const string& normdef ) { checkValid();  return ConvergenceHistory_t(nd()->writeConvergenceHistory(niter,normdef)); }

FlowEquationSet_t	structure_t::readFlowEquationSet( int& dim, bool& goveq, bool& gasm, bool& viscositym, bool& thermalcondm, bool& turBC_tlos, bool& turbm ) const 
	{ checkValid();  return FlowEquationSet_t(nd()->readFlowEquationSet(dim,goveq,gasm,viscositym,thermalcondm,turBC_tlos,turbm)); }
FlowEquationSet_t	structure_t::writeFlowEquationSet( int dim )     
	{ checkValid();  return FlowEquationSet_t(nd()->writeFlowEquationSet(dim)); }

}
