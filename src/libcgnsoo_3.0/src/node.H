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
#ifndef CGNS_NODE_H
#define CGNS_NODE_H

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <stdexcept>
#include <list>
#include <map>
#include <utility>
#include <algorithm>
#include <cstdlib>

using std::ostream;
using std::endl;
using std::string;
using std::vector;
using std::list;
using std::pair;
using std::map;

#include "cgnsmll.H"
#include "quantity.H"
#include "file.H"
#include "range.H"
#include "checkerror.H"

namespace CGNSOO
{
// Forward declaration of all CGNS structures inside the namespace
class DimensionalExponents;

class ArbitraryGridMotion_t;
class Area_t;
class Average_t;
class Axisymetry_t;
class Base_t;
class BaseIterativeData_t;
class BC_t;
class BCDataSet_t;
class BCData_t;
class BCProperty_t;
class ChemicalKineticsModel_t;
class ConvergenceHistory_t;
class DataArray_t;
class DimensionalUnits_t;
class DiscreteData_t;
class Elements_t;
class Family_t;
class FamilyBC_t;
class FamilyName_t;
class file;
class FlowEquationSet_t;
class FlowSolution_t;
class GasModel_t;
class GeometryReference_t;
class GoverningEquation_t;
class Gravity_t;
class GridConnectivity1to1_t;
class GridConnectivity_t;
class GridConnectivityProperty_t;
class GridCoordinates_t;
class IntegralData_t;
class OversetHoles_t;
class Periodic_t;
class ReferenceState_t;
class RigidGridMotion_t;
class RotatingCoordinates_t;
class ThermalConductivityModel_t;
class ThermalRelaxationModel_t;
class TurbulenceClosure_t;
class TurbulenceModel_t;
class UserDefinedData_t;
class ViscosityModel_t;
class WallFunction_t;
class Zone_t;
class ZoneBC_t;
class ZoneGridConnectivity_t;
class ZoneIterativeData_t;
	
/*! \class refcounted
 *  \brief Implements a reference counted data structure
 *
 * Destructor is protected and deletion is carried when 
 * refcount goes to zero.
 *
 */
class refcounted
{
private:
	unsigned    _refcount;  //!< reference counter; auto delete when it goes to zero

protected:
	virtual ~refcounted() {};

public:
	refcounted() : _refcount(0) {}
	
	void     ref() { _refcount++; }
	void     unref() { if ( _refcount-- == 1 ) delete this; }
};

/*! \class attribute
 *  \brief Internal utility class allowing values to be stored within a CGNS node
 *
 * This is essentially a slightly intelligent union
 *
 */
class attribute
{
private:
	enum type_t { NONE, INT, REAL, BOOL, STRING, ZONETYPE, BCTYPE };
	
private:
	type_t type;
	void*  value;
	
public:
	attribute() : type(NONE), value(NULL) {}
	~attribute()
	{
		switch (type)
		{
		case INT     : delete static_cast<int*>(value); break;
		case REAL    : delete static_cast<double*>(value); break;
		case BOOL    : delete static_cast<bool*>(value); break;
		case STRING  : delete static_cast<string*>(value); break;
		case ZONETYPE: delete static_cast<ZoneType_t*>(value); break;
		case BCTYPE  : delete static_cast<BCType_t*>(value); break;
		default: break;
		}
	}
	void get( int& v ) const { if (type!=INT) throw logic_error( "attribute::get" ); v = *static_cast<int*>(value); }
	void get( double& v ) const { if (type!=REAL) throw logic_error( "attribute::get" ); v = *static_cast<double*>(value); }
	void get( bool& v ) const { if (type!=BOOL) throw logic_error( "attribute::get" ); v = *static_cast<bool*>(value); }
	void get( string& v ) const { if (type!=STRING) throw logic_error( "attribute::get" ); v = *static_cast<string*>(value); }
	void get( ZoneType_t& v ) const { if (type!=ZONETYPE) throw logic_error( "attribute::get" ); v = *static_cast<ZoneType_t*>(value); }
	void get( BCType_t& v ) const { if (type!=BCTYPE) throw logic_error( "attribute::get" ); v = *static_cast<BCType_t*>(value); }
	void operator=( int v ) 
	{ if (type==NONE) { value = new int; type=INT; }
	  if (type==INT) { *static_cast<int*>(value) = v; return; }
	  throw logic_error( "attribute::operator=(int)" );
	}
	void operator=( double v ) 
	{ if (type==NONE) { value = new double; type=REAL; }
	  if (type==REAL) { *static_cast<double*>(value) = v; return; }
	  throw logic_error( "attribute::operator=(double)" );
	}
	void operator=( bool v ) 
	{ if (type==NONE) { value = new double; type=BOOL; }
	  if (type==BOOL) { *static_cast<bool*>(value) = v; return; }
	  throw logic_error( "attribute::operator=(bool)" );
	}
	void operator=( const string& v ) 
	{ if (type==NONE) { value = new string; type=STRING; }
	  if (type==STRING) { *static_cast<string*>(value) = v; return; }
	  throw logic_error( "attribute::operator=(string&)" );
	}
	void operator=( ZoneType_t v ) 
	{ if (type==NONE) { value = new ZoneType_t; type=ZONETYPE; }
	  if (type==ZONETYPE) { *static_cast<ZoneType_t*>(value) = v; return; }
	  throw logic_error( "attribute::operator=(ZoneType_t)" );
	}
	void operator=( BCType_t v ) 
	{ if (type==NONE) { value = new BCType_t; type=BCTYPE; }
	  if (type==BCTYPE) { *static_cast<BCType_t*>(value) = v; return; }
	  throw logic_error( "attribute::operator=(BCType_t)" );
	}
	friend ostream& operator<<( ostream& o, const attribute& a )
	{
		switch( a.type )
		{
		case INT   : o << *static_cast<int*>(a.value); break;
		case REAL  : o << *static_cast<double*>(a.value); break;
		case BOOL  : o << *static_cast<bool*>(a.value); break;
		case STRING: o << *static_cast<string*>(a.value); break;
		case BCTYPE: o << *static_cast<BCType_t*>(a.value); break;
		case ZONETYPE: o << *static_cast<ZoneType_t*>(a.value); break;
		default: break;
		}
		return o;
	}
};

/*!
 * \class node
 * \brief Handle into a cgns dataBase_t
 *
 * Light data structure acting as a handle to access 
 * the corresponding structure in the CGNS database.
 * Operation on a node is not checked semantically.
 * This class is not meant to be used directly by users of libcgnsoo.
 */
class node : public refcounted
{
private:
	typedef vector< pair<string,int> > path_t;
	
	string      _type;	//!< the type of this node (Base_t,Zone_t,...)
	int         _index;	//!< the cgns index (>=1) of this node
	const file* _fptr;	//!< pointer to the file this node is in
	const node* _parent;	//!< immediate parent of this node, NULL for a Base_t
	list<node*> _children;	//!< pointers to children nodes
	int         _nbda;      //!< number of daatarray writen under this node
	map<string,attribute> _attributes; //!< optional attributes stored in the node to avoid requesting the value from the MLL
					   //!< this is useful when the file is opened for writing and we cannot read from the database.
					   
private:
	friend class structure_t;
	friend ostream& operator<<( ostream& o, const node& n ); //!< dumps the node data to output stream
	
	//!< constructors are private - use the public allocate(...) methods and ->ref()/unref()
	node( const file& f, int b );    //!< constructor - only used for creating a Base_t node
	node( const node& parentnode, const string& t, int i );  //!< normal constructor for all other child node
	~node();

private:
	static node* allocate( const node& );         //!< allocates a new node
	static node* allocate( const file&, int b );  //!< allocates a Base_t node
	
	path_t           build_path() const;          //!< builds a path to the current node from the Base_t
	node*            push( const string& nodetype, int nodenumber ); //!< create a children to the current node of given nodetype and nodenumber
	file::openmode_t get_file_mode() const { return _fptr->getMode(); } //!< get the file mode the current file is opened in
	int              get_base_id() const; //!< get the Base_t id under which this node is located
	int              get_zone_id() const; //!< get the Zone_t id under which this node is located (if applicable; throws cgns_wrongnode otherwise)
	
	void             set_index( int i ) { _index = i; }
	int              get_index() const { return _index; }
	int              get_dataarray_index( const string& ) const;
	int              add_dataarray() { return ++_nbda; }
	string           get_path() const;
	string           get_type() const  { return _type; }
	void             delete_child( const string& );

public:	
	void             go_here() const; //!< set the CGNS MLL pointer to this node (does a cg_goto)
	int              getFileID() const { return _fptr->getFileId(); } //!< get the file number of this node
	bool             is_a( const string& nodetype ) const; //!< checks whether this node is of the given type
	const node*      get_parent() const { return _parent; } //!< get the immediate parent to the current node
	int              get_cell_dimension() const; //!< get the cell dimension of the Base_t this node is in
	int              get_physical_dimension() const; //!< get the physical dimension of the Base_t this node is in
	int              get_index_dimension() const; //!< get the index dimension of the Zone_t this node is in (if applicable; throws cgns_wrongnode otherwise))
	template <class T>
	void get_attribute( const string& id, T& value ) const
	{
		map<string,attribute>::const_iterator i=_attributes.find(id);
		(*i).second.get( value );
	}
	template <class T>
	void set_attribute( const string& id, T& value )
	{
		_attributes[id] = value;
	}

	
private:
	//!< read/write methods common to mutiple node types     
	void	readFamilyName( string& famname );
	void	writeFamilyName( const string& famname );
	
	void	readGridLocation( GridLocation_t& loc );
	void	writeGridLocation( GridLocation_t loc );
	
	int     getNbDescriptor() const;
	void	readDescriptor( int index, string& name, string& text );
	void	writeDescriptor( const string& name, const string& text );

	void	readDataClass( DataClass_t& dclass );
	void	writeDataClass( DataClass_t );

	void	readDataConversionFactors( double& scale, double& offset );
	void	writeDataConversionFactors( double scale, double offset );

	void	readDimensionalExponents( vector<double>& exponents );
	void	writeDimensionalExponents( const vector<double>& exponents );
	void	readDimensionalExponents( DimensionalExponents& exponents );
	void	writeDimensionalExponents( const DimensionalExponents& exponents );

	void	readDimensionalUnits( MassUnits_t&, LengthUnits_t&, TimeUnits_t&, TemperatureUnits_t&, AngleUnits_t& );
	void	writeDimensionalUnits( MassUnits_t, LengthUnits_t, TimeUnits_t, TemperatureUnits_t, AngleUnits_t );
	void	writeSIUnits(); // convenience

	node*	readFlowEquationSet( int& dim, bool& goveq, bool& gasm, bool& viscositym, bool& thermalcondm, bool& turbclos, bool& turbm );
	node*	writeFlowEquationSet( int dim );

	node*	readConvergenceHistory( int& niter, string& normdef );
	node*	writeConvergenceHistory( int niter, const string& normDefinition );

	node*	readRotatingCoordinates( vector<float>& ratevector, vector<float>& rotcenter );
	node*	writeRotatingCoordinates( const vector<float>& ratevector, const vector<float>& rotcenter );

	node*	readReferenceState( string& description );
	node*	writeReferenceState( const string& description );

	int	getNbDataArray() const;
	node*	readDataArrayInfo( int index, string& arrayname, DataType_t& data, vector<int>& dimensions );
	node*	writeDataArray( const string& name, int value );
	node*	writeDataArray( const string& name, float value );
	node*	writeDataArray( const string& name, double value );
	node*	writeDataArray( const string& name, const string& value );
	node*	writeDataArray( const string& name, const vector<int>& dimensions, const vector<int>&    values );
	node*	writeDataArray( const string& name, const vector<int>& dimensions, const vector<float>&  values );
	node*	writeDataArray( const string& name, const vector<int>& dimensions, const vector<double>& values );
	node*	writeDataArray( const string& name, const vector<int>& dimensions, const vector<string>& values );

	int	getNbUserDefinedData() const;
	node*	readUserDefinedData( int index, string& name );
	node*	writeUserDefinedData( const string& name );
};

} // namespace

#endif
