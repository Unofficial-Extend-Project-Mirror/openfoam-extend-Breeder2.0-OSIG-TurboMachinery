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
#ifndef CGNS_STRUCTURE_H
#define CGNS_STRUCTURE_H

#include "node.H"

/*! \namespace CGNSOO
 *  \brief The CGNSOO namespace comprises all classes defined in libcgnsoo
 */ 
namespace CGNSOO
{

/*! \class structure_t 
 *  \brief Abstract base class for all cgns entities
 *
 * All CGNS nodes corresponding to the SIDS structures derive from this
 * base class. 
 * This class implements all the common access methods for the children of a 
 * CGNS entity.
 * However, this class is not meant to be used directly by the CGNSOO user. 
 * Consequently, most methods are protected. Derived classes expose the 
 * methods which are appropriate considering the derived class semantics.
 * Methods specific to a CGNS entity are implemented within the derived class.
 * Implementation of some methods in the derived classes require access to another
 * objects structure_t:: methods which explain the few classes which are declared
 * public here. This should be considered a design bug and must be reserved for
 * internal usage only.
 * We might consider merging the 'node' class within this one.
 */
class structure_t
{
private:
	node* _nodeptr; //!< dynamically allocated pointer

public:
	// constructors
	structure_t( const file& f, int b )  //!< constructor - only used for Base_t::Base_t()
	{
		_nodeptr = node::allocate(f,b);
		_nodeptr->ref(); 
	}
	structure_t( const structure_t& s ) : _nodeptr(NULL)   //!< normal constructor
	{
		if ( s.valid() )
		{
			// create of copy of the node and reference it
			_nodeptr = node::allocate( *(s._nodeptr) );
			_nodeptr->ref(); 
		}
	}
	structure_t( const node* n ) : _nodeptr( const_cast<node*>(n) ) 
	{ 
		// just reference the existing node - no copy
		if ( _nodeptr ) _nodeptr->ref(); 
	}

	virtual ~structure_t();
	
	//!< conversion of datatype (i.e. Integer, Double,... ) to a standard alphanumeric string
	static string datatype_to_name( DataType_t t );

	//!< assignment operator
	void operator=( const structure_t& rhs ) 
	{
		if ( _nodeptr ) _nodeptr->unref();
		_nodeptr = NULL;
		if ( rhs.valid() )
		{
			_nodeptr = node::allocate( *(rhs._nodeptr) );
			_nodeptr->ref();
		}
	}
	
public:	
	structure_t parent() const  { return structure_t( valid() ? nd()->get_parent() : NULL ); }  //!< get the immediate parent structure
	void        go_here() const { checkValid(); nd()->go_here(); }                             //!< move the cgns MLL pointer to this structure
	int         getID() const   { checkValid(); return nd()->get_index(); }                     //!< get the numeric id of this structure
	bool        isA( const string& typestring ) const { checkValid(); return nd()->is_a(typestring); }        //!< check if this structure is of the given type
	int         findDataArrayIndex( const string& name ) const { checkValid(); return nd()->get_dataarray_index(name); } //!< look under this node for a DataArray named 'name'
	template <class T>
	void get_attribute( const string& id, T& value ) const
	{
		checkValid();
		nd()->get_attribute( id, value );
	}
	template <class T>
	void set_attribute( const string& id, T& value )
	{
		checkValid();
		nd()->set_attribute( id, value );
	}
	
protected:	
	// read/write methods common to mutiple node types
	// macros allows for an easy definition in subclasses by calling equivalent (protected) method in structure_t
	void	readFamilyName( string& famname ) const ;
	void	writeFamilyName( const string& famname ) ;
#define ChildMethod_FamilyName \
	void	readFamilyName( string& famname ) const { structure_t::readFamilyName(famname); } \
	void	writeFamilyName( const string& famname )  { structure_t::writeFamilyName(famname); }

	void	readGridLocation( GridLocation_t& loc ) const;
	void	writeGridLocation( GridLocation_t loc ) ;
#define ChildMethod_GridLocation \
	void	readGridLocation( GridLocation_t& loc ) const { structure_t::readGridLocation(loc); } \
	void	writeGridLocation( GridLocation_t loc )  { structure_t::writeGridLocation(loc); }

	int	getNbDescriptor() const;	
	void	readDescriptor( int index, string& name, string& text ) const  ;
	void	writeDescriptor( const string& name, const string& text ) ;
#define ChildMethod_Descriptor \
	int	getNbDescriptor() const { return structure_t::getNbDescriptor(); } \
	void	readDescriptor( int index, string& name, string& text ) const { structure_t::readDescriptor(index,name,text); } \
	void	writeDescriptor( const string& name, const string& text )  { structure_t::writeDescriptor(name,text); }

	void	readDataClass( DataClass_t& dclass ) const ;
	void	writeDataClass( DataClass_t dclass)  ;
#define ChildMethod_DataClass \
	void	readDataClass( DataClass_t& dclass ) const { structure_t::readDataClass(dclass); } \
	void	writeDataClass( DataClass_t dclass )  { structure_t::writeDataClass(dclass); }
                                                                
	void	readDataConversionFactors( double& scale, double& offset ) const ;
	void	writeDataConversionFactors( double scale, double offset )  ;
#define ChildMethod_DataConversion \
	void	readDataConversionFactors( double& scale, double& offset ) const { structure_t::readDataConversionFactors(scale,offset); } \
	void	writeDataConversionFactors( double& scale, double& offset )  { structure_t::writeDataConversionFactors(scale,offset); }

	void	readDimensionalExponents( vector<double>& exponents ) const  ;
	void	writeDimensionalExponents( const vector<double>& exponents ) ;
	void	readDimensionalExponents( DimensionalExponents& exponents ) const  ;
	void	writeDimensionalExponents( const DimensionalExponents& exponents ) ;
#define ChildMethod_DimensionalExponents \
	void	readDimensionalExponents( vector<double>& exponents ) const { structure_t::readDimensionalExponents(exponents); } \
	void	writeDimensionalExponents( const vector<double>& exponents )  { structure_t::writeDimensionalExponents(exponents); } \
	void	readDimensionalExponents( DimensionalExponents& exponents ) const { structure_t::readDimensionalExponents(exponents); } \
	void	writeDimensionalExponents( const DimensionalExponents& exponents )  { structure_t::writeDimensionalExponents(exponents); }

	void	readDimensionalUnits( MassUnits_t& m, LengthUnits_t& l, TimeUnits_t& t, TemperatureUnits_t& temp, AngleUnits_t& a) const;
	void	writeDimensionalUnits( MassUnits_t m, LengthUnits_t l, TimeUnits_t t, TemperatureUnits_t temp, AngleUnits_t a)     ;
	void	writeSIUnits() ;
#define ChildMethod_DimensionalUnits \
	void	readDimensionalUnits( MassUnits_t& m, LengthUnits_t& l, TimeUnits_t& t, TemperatureUnits_t& temp, AngleUnits_t& a ) const \
	        { structure_t::readDimensionalUnits(m,l,t,temp,a); } \
	void	writeDimensionalUnits( MassUnits_t m, LengthUnits_t l, TimeUnits_t t, TemperatureUnits_t temp, AngleUnits_t a ) \
	        { structure_t::writeDimensionalUnits(m,l,t,temp,a); } \
	void    writeSIUnits() { structure_t::writeSIUnits(); }

	int		getNbDataArray() const;
	DataArray_t	readDataArrayInfo( int index, string& arrayname, DataType_t& data, vector<int>& dimensions ) const;
	DataArray_t	writeDataArray( const string& name, int value );
	DataArray_t	writeDataArray( const string& name, float value );
	DataArray_t	writeDataArray( const string& name, double value );
	DataArray_t	writeDataArray( const string& name, const string& value );
	DataArray_t	writeDataArray( const string& name, const vector<int>& dimensions, const vector<int>&    values );
	DataArray_t	writeDataArray( const string& name, const vector<int>& dimensions, const vector<float>&  values );
	DataArray_t	writeDataArray( const string& name, const vector<int>& dimensions, const vector<double>& values );
	DataArray_t	writeDataArray( const string& name, const vector<int>& dimensions, const vector<string>& values );
#define ChildMethod_DataArray \
	int       getNbDataArray() const { return structure_t::getNbDataArray(); } \
	DataArray_t readDataArrayInfo( int index, string& arrayname, DataType_t& data, vector<int>& dimensions ) const \
		{ return structure_t::readDataArrayInfo(index,arrayname,data,dimensions); } \
	DataArray_t writeDataArray( const string& name, int value )     { return structure_t::writeDataArray(name,value); } \
	DataArray_t writeDataArray( const string& name, float value )   { return structure_t::writeDataArray(name,value); } \
	DataArray_t writeDataArray( const string& name, double value )  { return structure_t::writeDataArray(name,value); } \
	DataArray_t writeDataArray( const string& name, const string& value )  { return structure_t::writeDataArray(name,value); } \
	DataArray_t writeDataArray( const string& name, const vector<int>& dimensions, const vector<int>& values ) \
		{ return structure_t::writeDataArray(name,dimensions,values); } \
	DataArray_t writeDataArray( const string& name, const vector<int>& dimensions, const vector<float>& values ) \
		{ return structure_t::writeDataArray(name,dimensions,values); } \
	DataArray_t writeDataArray( const string& name, const vector<int>& dimensions, const vector<double>& values ) \
		{ return structure_t::writeDataArray(name,dimensions,values); } \
	DataArray_t writeDataArray( const string& name, const vector<int>& dimensions, const vector<string>& values ) \
		{ return structure_t::writeDataArray(name,dimensions,values); }

	ReferenceState_t	readReferenceState( string& description ) const;
	ReferenceState_t	writeReferenceState( const string& description );
#define ChildMethod_ReferenceState \
	ReferenceState_t	readReferenceState( string& description ) const { return structure_t::readReferenceState(description); } \
	ReferenceState_t	writeReferenceState( const string& description )  { return structure_t::writeReferenceState(description); }

	FlowEquationSet_t	readFlowEquationSet( int& dim, bool& goveq, bool& gasm, bool& viscositym, bool& thermalcondm, bool& turBC_tlos, bool& turbm ) const;
	FlowEquationSet_t	writeFlowEquationSet( int dim );
#define ChildMethod_FlowEquationSet \
	FlowEquationSet_t	readFlowEquationSet( int& dim, bool& goveq, bool& gasm, bool& viscositym, bool& thermalcondm, bool& turBC_tlos, bool& turbm ) const \
		{ return structure_t::readFlowEquationSet(dim,goveq,gasm,viscositym,thermalcondm,turBC_tlos,turbm); } \
	FlowEquationSet_t	writeFlowEquationSet( int dim )  { return structure_t::writeFlowEquationSet(dim); }

	int		  getNbUserDefinedData() const;
	UserDefinedData_t readUserDefinedData( int index, string& name ) const;
	UserDefinedData_t writeUserDefinedData( const string& name );
#define ChildMethod_UserDefinedData \
	int			getNbUserDefinedData() const { return structure_t::getNbUserDefinedData(); } \
	UserDefinedData_t	readUserDefinedData( int index, string& name ) const { return structure_t::readUserDefinedData(index,name); } \
	UserDefinedData_t	writeUserDefinedData( const string& name )  { return structure_t::writeUserDefinedData(name); }

	RotatingCoordinates_t   readRotatingCoordinates( vector<float>& ratevector, vector<float>& rotcenter ) const;
	RotatingCoordinates_t   writeRotatingCoordinates( const vector<float>& ratevector, const vector<float>& rotcenter );
#define ChildMethod_RotatingCoordinates \
	RotatingCoordinates_t 	readRotatingCoordinates( vector<float>& ratevector, vector<float>& rotcenter ) const { return structure_t::readRotatingCoordinates(ratevector,rotcenter); } \
	RotatingCoordinates_t 	writeRotatingCoordinates( const vector<float>& ratevector, const vector<float>& rotcenter )  { return structure_t::writeRotatingCoordinates(ratevector,rotcenter); }
	
	ConvergenceHistory_t    readConvergenceHistory( int& niter, string& normdef ) const;
	ConvergenceHistory_t    writeConvergenceHistory( int niter, const string& normdef );
#define ChildMethod_ConvergenceHistory \
	ConvergenceHistory_t 	readConvergenceHistory( int& niter, string& normdef ) const { return structure_t::readConvergenceHistory(niter,normdef); } \
	ConvergenceHistory_t 	writeConvergenceHistory( int niter, const string& normdef ) { return structure_t::writeConvergenceHistory(niter,normdef); }

protected:	
	node*       nd() const { return _nodeptr; }
	//!< Checks if this structure points to valid data. Invalid data can only happen when a structure was initialized with a NULL pointer.
	bool        valid() const { return _nodeptr!=NULL; }
	//!< Checks if this structure points to valid data. Invalid data can only happen when a structure was initialized with a NULL pointer.
	bool        checkValid() const throw(cgns_uninitialized) { if ( valid() ) return true; throw cgns_uninitialized(); }
	//!< Return a new structure_t as a child index 'i' of type 'stype' under this node.
	structure_t push( const char* stype, int i ) const { checkValid(); return structure_t( _nodeptr->push( stype, i ) );}
	//!< Return the CGNS file number of this node
	int         getFileID() const { checkValid(); return nd()->getFileID(); }
	//!< Return the Cell Dimension of the Base_t this structure_t is in.
	int         getCellDimension() const { checkValid(); return nd()->get_cell_dimension(); }
	//!< Return the Physical Dimension of the Base_t this structure_t is in.
	int         getPhysicalDimension() const { checkValid(); return nd()->get_physical_dimension(); }
};

} // namespace

#endif
