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
#include <string.h>
#include "node.H"
#include "dimensionalexponents.H"

namespace CGNSOO
{

//----------------------------
// static allocation methods
//----------------------------

node* node::allocate( const node& parent_node )
{
	node* n = new node( parent_node );
	return n;
}

node* node::allocate( const file& f, int bid )
{
	node* n = new node( f, bid );
	return n;
}

//------------------------------------------------
// constructors / destructor
//------------------------------------------------

node::node( const file& f, int b ) : _type("Base_t"), _index(b), _fptr(&f), _parent(NULL), _nbda(0)
{
	//std::cerr << "Created new Base_t." << b << " node\n";
}

node::node( const node& parentnode, const string& t, int i ) : _type(t), _index(i), _fptr(parentnode._fptr), _parent(&parentnode), _nbda(0)
{
	//std::cerr << "Created new " << t << "." << i << " node\n";
}

node::~node()
{
	// delete all attributes
	_attributes.erase( _attributes.begin(), _attributes.end() );
}

//----------------------------
// protected methods
//----------------------------

node* node::push( const string& t, int i )
{
	node *n = new node( *this, t, i );
	_children.push_back( n );
	return n;
}

node::path_t node::build_path() const
{
	// build the list of nodes leading to the current node
	list<const node*> nodes;
	const node* n = this;
	while ( n->_parent )
	{
		nodes.push_front( n );
		n = n->_parent;
	}
	nodes.push_front( n );

	// create a path out of this list
	path_t path;
	for ( list<const node*>::iterator i=nodes.begin() ; i!=nodes.end() ; i++ )
		path.push_back( pair<string,int>( (*i)->get_type(), (*i)->get_index() ) );
	return path;
}

void node::go_here() const
{
	int ier = 0;
	path_t path = build_path();
#if 0	
	std::cerr << "node::go_here : " << (*this);
	std::cerr << "build_path=";
	for ( path_t::const_iterator i=path.begin() ; i!=path.end() ; i++ )
		std::cerr << " [ " << (*i).first << '.' << (*i).second << " ] ";
	std::cerr << std::endl;
#endif	
	switch( path.size() )
	{
	case 1: // Base_t
		ier = cg_goto( getFileID(), path[0].second, "end" );
		break;
	case 2: 
		ier = cg_goto( getFileID(), path[0].second, 
					      path[1].first.c_str(), path[1].second, 
					      "end" );
		break;
	case 3: 
		ier = cg_goto( getFileID(), path[0].second, 
					      path[1].first.c_str(), path[1].second, 
					      path[2].first.c_str(), path[2].second, 
					      "end" );
		break;
	case 4: 
		ier = cg_goto( getFileID(), path[0].second, 
					      path[1].first.c_str(), path[1].second, 
					      path[2].first.c_str(), path[2].second, 
					      path[3].first.c_str(), path[3].second, 
					      "end" );
		break;
	case 5: 
		ier = cg_goto( getFileID(), path[0].second, 
					      path[1].first.c_str(), path[1].second, 
					      path[2].first.c_str(), path[2].second, 
					      path[3].first.c_str(), path[3].second, 
					      path[4].first.c_str(), path[4].second, 
					      "end" );
		break;
	case 6: 
		ier = cg_goto( getFileID(), path[0].second, 
					      path[1].first.c_str(), path[1].second, 
					      path[2].first.c_str(), path[2].second, 
					      path[3].first.c_str(), path[3].second, 
					      path[4].first.c_str(), path[4].second, 
					      path[5].first.c_str(), path[5].second, 
					      "end" );
		break;
	case 7: 
		ier = cg_goto( getFileID(), path[0].second, 
					      path[1].first.c_str(), path[1].second, 
					      path[2].first.c_str(), path[2].second, 
					      path[3].first.c_str(), path[3].second, 
					      path[4].first.c_str(), path[4].second, 
					      path[5].first.c_str(), path[5].second, 
					      path[6].first.c_str(), path[6].second, 
					      "end" );
		break;
	case 8: 
		ier = cg_goto( getFileID(), path[0].second, 
					      path[1].first.c_str(), path[1].second, 
					      path[2].first.c_str(), path[2].second, 
					      path[3].first.c_str(), path[3].second, 
					      path[4].first.c_str(), path[4].second, 
					      path[5].first.c_str(), path[5].second, 
					      path[6].first.c_str(), path[6].second, 
					      path[7].first.c_str(), path[7].second, 
					      "end" );
		break;
	default:
		std::cerr << "node::go_here with " << path.size() << " levels - not yet supported\n";
		break;
	}
	check_error( "node::go_here", "cg_goto", ier );
}

int node::get_cell_dimension() const
{
	cgnsstring basename;
	int celldim, physdim;
	int ier = cg_base_read( getFileID(), get_base_id(), basename, &celldim , &physdim );
	check_error( "node::get_cell_dimension", "cg_base_read", ier );
	return celldim;
}

int node::get_physical_dimension() const
{
	cgnsstring basename;
	int celldim, physdim;
	int ier = cg_base_read( getFileID(), get_base_id(), basename, &celldim , &physdim );
	check_error( "node::get_physical_dimension", "cg_base_read", ier );
	return physdim;
}

int node::get_index_dimension() const
{
	ZoneType_t zonetype;
	int ier = cg_zone_type( getFileID(), get_base_id(), get_zone_id(), &zonetype );
	check_error( "node::get_index_dimension", "cg_zone_type", ier );
	return (zonetype==Unstructured) ? 1 : get_cell_dimension();
}


/*! Search under this node for a DataArray named 'name' and return its index (>=1)
 * returns 0 if not found
 */
int node::get_dataarray_index( const string& name ) const
{
	go_here();
	int na;
	int ier = cg_narrays( &na );
	check_error( "node::get_dataarray_index", "cg_narrays", ier );
	for ( int i=0 ; i<na ; i++ )
	{
		cgnsstring array_name;
		DataType_t datatype;
		int ndim;
		int dimvect[32];
		int ier = cg_array_info( i+1, array_name, &datatype, &ndim, dimvect );
		check_error( "node::get_dataarray_index", "cg_array_info", ier );
		if ( name == array_name ) return i+1;
	}
	return 0;
}

string node::get_path() const
{
	path_t path = build_path();
	string spath( "/" );
	if ( path[0].second >= 0 )
	{
		spath += "Base_t/";
		for ( path_t::const_iterator i=path.begin() ; i!=path.end() ; i++ )
			spath += (*i).first + '/';
	}
	return spath;
}

int node::get_base_id() const
{
	// go up the node list to get the topmost parent - this has to be the Base_t
	const node* n = this;
	while (n->_parent)
		n = n->_parent;
	return n->_index;
}

int node::get_zone_id() const
{
	// go up the node list to get the topmost parent - this has to be a Base_t
	// the Zone_t was the node just under it
	const node* n = this;
	const node* p = NULL;
	while (n->_parent)
	{
		p = n;
		n = n->_parent;
	}
	if (!p) throw cgns_wrongnode( "node::get_zone_id", "Zone_t", get_type() );
	return p->_index;
}

bool node::is_a( const string& matchType ) const
{
	return get_type()==matchType;
}

void node::delete_child( const string& childname )
{
	go_here();
	int ier = cg_delete_node( const_cast<char*>(childname.c_str()) );
	check_error( "node::delete", "cg_delete_node", ier );
}

//-------------------------------------------------------------------------
// get_number_of
//-------------------------------------------------------------------------

int node::getNbDescriptor() const
{
	go_here();
	int ndescriptors;
	int ier = cg_ndescriptors( &ndescriptors );
	check_error( "node::getNbDescriptor", "cg_ndescriptors", ier );
	return ndescriptors;
}

int node::getNbDataArray() const
{
	go_here();
	int ndata;
	int ier = cg_narrays( &ndata );
	check_error( "node::getNbDataArrays", "cg_ndata", ier );
	return ndata;
}

int node::getNbUserDefinedData() const
{
	go_here();
	int ndata;
	int ier = cg_nuser_data( &ndata );
	check_error( "node::getNbUserDefinedData", "cg_nuser_data", ier );
	return ndata;
}

//-------------------------------------------------------------------------
// read & write
//-------------------------------------------------------------------------

void node::readDescriptor( int index, string& name, string& descriptor_text )
{
	go_here();
	cgnsstring cname;
	char* text;
	int ier = cg_descriptor_read( index+1, cname, &text );
	check_error( "node::readDescriptor", "cg_descriptor_read", ier );
	descriptor_text = text;
	CGNSfree( text );
	name  = cname;
}

void node::writeDescriptor( const string& name, const string& descriptor_text )
{
	go_here();
	int ier = cg_descriptor_write( name.c_str(), descriptor_text.c_str() );
	check_error( "node::writeDescriptor", "cg_descriptor_write", ier );
}

void node::readDataClass( DataClass_t& dclass )
{
	go_here();
	int ier = cg_dataclass_read( &dclass );
	check_error( "node::readDataClass", "cg_dataclass_read", ier );
}

void node::writeDataClass( DataClass_t c )
{
	go_here();
	int ier = cg_dataclass_write( c );
	check_error( "node::writeDataClass", "cg_dataclass_write", ier );
}

void node::readDataConversionFactors( double& scale, double& offset )
{
	go_here();
	DataType_t type;
	int ierinfo = cg_conversion_info( &type );
	check_error( "node::readDataConversionFactors", "cg_conversion_info", ierinfo );
	int ier;
	if ( type == RealSingle )
	{
		float fs_fo[2];
		ier = cg_conversion_read( fs_fo );
		scale  = fs_fo[0];
		offset = fs_fo[1];
	}
	else
	{
		double fs_fo[2];
		ier = cg_conversion_read( fs_fo );
		scale  = fs_fo[0];
		offset = fs_fo[1];
	}
	check_error( "node::readDataConversionFactors", "cg_conversion_read", ier );	
}

void node::writeDataConversionFactors( double scale, double offset )
{
	go_here();
	double fs_fo[2] = { scale, offset };
	int ier = cg_conversion_write( RealDouble, fs_fo );
	check_error( "node::writeDataConversionFactors", "cg_conversion_write", ier );
}

void node::readDimensionalExponents( vector<double>& exponents )
{
	go_here();
	DataType_t type;
	int ierinfo = cg_exponents_info( &type );
	check_error( "node::readDimensionalExponents", "cg_exponents_info", ierinfo );
	int ier;
	if ( type == RealSingle )
	{
		float e[5];
		ier = cg_exponents_read( &e );
		for ( int i=0 ; i<5 ; i++ )
			exponents[i] = e[i];
	}
	else
	{
		double e[5];
		ier = cg_exponents_read( &e );
		for ( int i=0 ; i<5 ; i++ )
			exponents[i] = e[i];
	}
	check_error( "node::readDimensionalExponents", "cg_exponents_read", ier );
	check_found( "node::readDimensionalExponents", "DimensionalExponents_t", ier );
}

void node::writeDimensionalExponents( const vector<double>& exponents )
{
	go_here();
	int ier = cg_exponents_write( RealDouble, &exponents[0] );
	check_error( "node::writeDimensionalExponents", "cg_exponents_write", ier );
}

void node::readDimensionalExponents( DimensionalExponents& exponents )
{
	go_here();
	DataType_t type;
	int ierinfo = cg_exponents_info( &type );
	check_error( "node::readDimensionalExponents", "cg_exponents_info", ierinfo );
	int ier;
	double e[5];
	if ( type == RealSingle )
	{
		float f[5];
		ier = cg_exponents_read( &f );
		std::copy( f, f+5, e );
	}
	else
	{
		ier = cg_exponents_read( &e );
	}
	check_error( "node::readDimensionalExponents", "cg_exponents_read", ier );
	check_found( "node::readDimensionalExponents", "DimensionalExponents_t", ier );
	for ( int i=0 ; i<5 ; i++ )
		exponents[static_cast<DimensionalExponents::exponentType_t>(i)] = e[i];
}

void node::writeDimensionalExponents( const DimensionalExponents& exponents )
{
	go_here();
	//Array<double> evector = exponents;
	//int ier = cg_exponents_write( RealDouble, evector );
	int ier = cg_exponents_write( RealDouble, static_cast<double*>(Array<double>(exponents)) );
	check_error( "node::writeDimensionalExponents", "cg_exponents_write", ier );
}

void node::readDimensionalUnits( MassUnits_t& mass, LengthUnits_t& length, TimeUnits_t& time, TemperatureUnits_t& temp, AngleUnits_t& angle )
{
	go_here();
	int ier = cg_units_read( &mass, &length, &time, &temp, &angle );
	check_error( "node::readUnits", "cg_units_read", ier );
}

void node::writeDimensionalUnits( MassUnits_t m, LengthUnits_t l, TimeUnits_t t, TemperatureUnits_t s, AngleUnits_t a )
{
	go_here();
	int ier = cg_units_write( m, l, t, s, a );
	check_error( "node::writeUnits", "cg_units_write", ier );
}

void node::writeSIUnits()
{
	MassUnits_t        mass   = Kilogram;
	LengthUnits_t      length = Meter;
	TimeUnits_t        time   = Second;
	TemperatureUnits_t temp   = Kelvin;
	AngleUnits_t       angle  = Degree;

	go_here();
	int ier = cg_units_write( mass, length, time, temp, angle );
	check_error( "node::writeSIUnits", "cg_units_write", ier );
}

node* node::readConvergenceHistory( int& niter, string& normdef )
{
	go_here();
	char* description;
	int ier = cg_convergence_read( &niter, &description );
	check_error( "node::readUnits", "cg_units_read", ier );
	normdef = description;
	return push( "Units_t", 1 );
}

node* node::writeConvergenceHistory( int niter, const string& normdef )
{
	go_here();
	int ier = cg_convergence_write( niter, normdef.c_str() );
	check_error( "node::writeConvergenceHistory", "cg_convergence_write", ier );
	return push( "ConvergenceHistory_t", 1 );
}

node* node::readReferenceState( string& description )
{
	go_here();
	char* d;
	int ier = cg_state_read( &d );
	check_error( "node::readReferenceState", "cg_state_read", ier );
	description = *d;
	return push("ReferenceState_t",1);
}

node* node::writeReferenceState( const string& description )
{
	go_here();
	int ier = cg_state_write( description.c_str() );
	check_error( "node::writeReferenceState", "cg_state_write", ier );
	return push( "ReferenceState_t", 1 );
}

void node::readGridLocation( GridLocation_t& loc )
{
	go_here();
	GridLocation_t loctype;
	int ier = cg_gridlocation_read( &loctype );
	check_error( "node::getGridLocation", "cg_gridlocation_read", ier );
	loc = loctype;
}

void node::writeGridLocation( GridLocation_t loc )
{
	go_here();
	int ier = cg_gridlocation_write( loc );
	check_error( "node::writeGridLocation", "cg_gridlocation_write", ier );
}

node* node::readFlowEquationSet( int& EquationDimension, bool& GoverningEquationsFlag,
          bool& GasModelFlag, bool& ViscosityModelFlag, bool& ThermalConductivityFlag,
          bool& TurbulenceClosureFlag, bool& TurbulenceModelFlag )
{
	int goveqf;
	int gasmf;
	int viscmf;
	int thermcf;
	int turBC_tf;
	int turbmf;
 	go_here();
	int ier = cg_equationset_read(  &EquationDimension, &goveqf, &gasmf, &viscmf, &thermcf, &turBC_tf, &turbmf );
	check_error( "node::readFlowEquationSet", "cg_equationset_read", ier );
	GoverningEquationsFlag = (goveqf !=0);
        GasModelFlag           = (gasmf  !=0);
        ViscosityModelFlag     = (viscmf !=0);
        ThermalConductivityFlag= (thermcf!=0);
        TurbulenceClosureFlag  = (turBC_tf !=0);
        TurbulenceModelFlag    = (turbmf !=0);
	return push( "FlowEquationSet_t", 1 );
}

node* node::writeFlowEquationSet( int eqdim )
{
	go_here();
	int ier = cg_equationset_write( eqdim );
	check_error( "node::writeFlowEquationSet", "cg_equationset_write", ier );
	return push( "FlowEquationSet_t", 1 );
}

node* node::readRotatingCoordinates( vector<float>& ratevector, vector<float>& rotcenter )
{
	go_here();
	float rate[3], center[3];
	int ier = cg_rotating_read( rate, center );
	check_error( "node::readRotatingCoordinates", "cg_rotating_read", ier );
	int ndim = get_physical_dimension();
	ratevector.resize(ndim);
	rotcenter.resize(ndim);
	for ( int i=0 ; i<ndim ; i++ )
	{
		ratevector[i] = rate[i];
		rotcenter[i]  = center[i];
	}
	return push( "RotatingCoordinates_t", 1 );
}

node* node::writeRotatingCoordinates( const vector<float>& ratevector, const vector<float>& rotcenter )
{
	go_here();
	int ier = cg_rotating_write( &ratevector[0], &rotcenter[0] );
	check_error( "node::writeRotatingCoordinates", "cg_rotating_write", ier );
	return push( "RotatingCoordinates_t", 1 );
}

void node::readFamilyName( string& famname )
{
	go_here();
	cgnsstring s;
	int ier = cg_famname_read( s );
	check_error( "node::writeFamilyName", "cg_famname_write", ier );
	famname = s;
}

void node::writeFamilyName( const string& famname )
{
	go_here();
	int ier = cg_famname_write( famname.c_str() );
	check_error( "node::writeFamilyName", "cg_famname_write", ier );
}

node* node::readUserDefinedData( int index, string& name )
{
	go_here();
	cgnsstring s;
	int ier = cg_user_data_read( ++index, s );
	check_error( "node::readUserDefinedData", "cg_user_data_read", ier );
	name = s;
	return push("UserDefinedData_t",index);
}

node* node::writeUserDefinedData( const string& name )
{
	go_here();
	int ier = cg_user_data_write( name.c_str() );
	check_error( "node::writeUserDefinedData", "cg_user_data_write", ier );
	int n;
	ier = cg_nuser_data( &n );
	check_error( "node::writeUserDefinedData", "cg_nuser_data", ier );
	return push("UserDefinedData_t",n);
}

//-------------------------------------------------------------------------
// data arrays
//-------------------------------------------------------------------------

/*
 * For data arrays we keep a variable named 'nbda' inside class 'node' to keep track
 * of the number of data arrays already under this parent.
 * In READONLY mode there is no problem since one can't write
 * In WRITE mode the initial value of 0 is acceptable
 * In READWRITE mode however, the DataArray_t we write might already exist! We must make sure
 * not to increment 'nbda' if the data array we are writing already exists
*/

node* node::readDataArrayInfo( int index, string& arrayname, DataType_t& datatype, vector<int>& dimensions )
{
	index++;
	
	go_here();
	cgnsstring array_name;
	int ndim;
	int dimvect[32];
	int ier = cg_array_info( index, array_name, &datatype, &ndim, dimvect );
	check_error( "node::readDataArrayInfo", "cg_array_info", ier );
	arrayname = array_name;
	dimensions.resize( ndim );
	for ( int i=0 ; i<ndim ; i++ )
		dimensions[i] = dimvect[i];
	return push( "DataArray_t", index );
}

node* node::writeDataArray( const string& name, const vector<int>& dimensions, const vector<double>& values )
{
	go_here();
	int index;
	if ( get_file_mode() == file::READWRITE )
	{
		index = get_dataarray_index( name );
		if ( index==0 ) index = getNbDataArray();
	}
	else
	{
		index = add_dataarray();
	}
	int ier = cg_array_write( name.c_str(), RealDouble, dimensions.size(), &dimensions[0], &values[0] );
	check_error( "node::writeDataArray", "cg_array_write", ier );
	return push( "DataArray_t", index );
}

node* node::writeDataArray( const string& name, const vector<int>& dimensions, const vector<int>& values )
{
	go_here();
	int index;
	if ( get_file_mode() == file::READWRITE )
	{
		index = get_dataarray_index( name );
		if ( index==0 ) index = getNbDataArray();
	}
	else
	{
		index = add_dataarray();
	}
	int ier = cg_array_write( name.c_str(), Integer, dimensions.size(), &dimensions[0], &values[0] );
	check_error( "node::writeDataArray", "cg_array_write", ier );
	return push( "DataArray_t", index );
}

node* node::writeDataArray( const string& name, const vector<int>& dimensions, const vector<float>& values )
{
	go_here();
	int index;
	if ( get_file_mode() == file::READWRITE )
	{
		index = get_dataarray_index( name );
		if ( index==0 ) index = getNbDataArray();
	}
	else
	{
		index = add_dataarray();
	}
	int ier = cg_array_write( name.c_str(), RealSingle, dimensions.size(), &dimensions[0], &values[0] );
	check_error( "node::writeDataArray", "cg_array_write", ier );
	return push( "DataArray_t", index );
}

node* node::writeDataArray( const string& name, const vector<int>& dimensions, const vector<string>& values )
{
	int ntot = 1;
	for ( vector<int>::const_iterator i=dimensions.begin() ; i!=dimensions.end() ; i++ )
		ntot *= (*i);
	char* s = new char[ntot];
	for ( int i=0 ; i<ntot ; i++ )
		s[i] = ' ';
	int i, stride;
	switch( dimensions.size() )
	{
	case 1:
		strncpy( s, values[0].c_str(), dimensions[0] );
		break;
	case 2:
		stride = dimensions[0];
		for ( i=0 ; i<dimensions[1] ; i++ )
			strncpy( &s[i*stride], values[i].c_str(), values[i].length() );
		break;
	default:
		throw std::range_error( "node::writeDataArray(const string&,const vector<int>&,const vector<string>& ) can only handle dimension 1 or 2" );
	}
	go_here();
	int index;
	if ( get_file_mode() == file::READWRITE )
	{
		index = get_dataarray_index( name );
		if ( index==0 ) index = getNbDataArray();
	}
	else
	{
		index = add_dataarray();
	}
	int ier = cg_array_write( name.c_str(), Character, dimensions.size(), &dimensions[0], s );
	check_error( "node::writeDataArray", "cg_array_write", ier );
	delete [] s;
	return push( "DataArray_t", index );
}

node* node::writeDataArray( const string& name, int value )
{
	go_here();
	int index;
	if ( get_file_mode() == file::READWRITE )
	{
		index = get_dataarray_index( name );
		if ( index==0 ) index = getNbDataArray();
	}
	else
	{
		index = add_dataarray();
	}
	static int dims[1] = {1};
	int ier = cg_array_write( name.c_str(), Integer, 1, dims, &value );
	check_error( "node::writeDataArray", "cg_array_write", ier );
	return push( "DataArray_t", index );	
}

node* node::writeDataArray( const string& name, float value )
{
	go_here();
	int index;
	if ( get_file_mode() == file::READWRITE )
	{
		index = get_dataarray_index( name );
		if ( index==0 ) index = getNbDataArray();
	}
	else
	{
		index = add_dataarray();
	}
	static int dims[1] = {1};
	int ier = cg_array_write( name.c_str(), RealSingle, 1, dims, &value );
	check_error( "node::writeDataArray", "cg_array_write", ier );
	return push( "DataArray_t", index );
}

node* node::writeDataArray( const string& name, double value )
{
	go_here();
	int index;
	if ( get_file_mode() == file::READWRITE )
	{
		index = get_dataarray_index( name );
		if ( index==0 ) index = getNbDataArray();
	}
	else
	{
		index = add_dataarray();
	}
	static int dims[1] = {1};
	int ier = cg_array_write( name.c_str(), RealDouble, 1, dims, &value );
	check_error( "node::writeDataArray", "cg_array_write", ier );
	return push( "DataArray_t", index );
}

node* node::writeDataArray( const string& name, const string& value )
{
	go_here();
	int index;
	if ( get_file_mode() == file::READWRITE )
	{
		index = get_dataarray_index( name );
		if ( index==0 ) index = getNbDataArray();
	}
	else
	{
		index = add_dataarray();
	}
	int dims[1] = {value.length()};
	int ier = cg_array_write( name.c_str(), Character, 1, dims, value.c_str() );
	check_error( "node::writeDataArray", "cg_array_write", ier );
	return push( "DataArray_t", index );
}

ostream& operator<<( ostream& o, const node& n )
{
	o << "node " << n._type << "." << n._index << " parent is ";
	if ( n._parent ) 
		o << n._parent->_type << "." << n._parent->_index;
	else
		o << "NULL";
	o << std::endl;
	if ( n._attributes.size() > 0 ) o << "Attributes\n";
	for ( map<string,attribute>::const_iterator i=n._attributes.begin() ; i!=n._attributes.end() ; i++ )
		o << "\t\"" << (*i).first << "\" = " << (*i).second << std::endl;		
	return o;
}

};
