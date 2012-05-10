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
#ifndef CGNSOO_H
#define CGNSOO_H

#include "structure_t.H"

namespace CGNSOO
{

/*
 * This file contains the class definition for all the SIDS structures
 * They are all based upon the structure_t base class.
 * The ChildMethod_* macros are used to uniformly declare methods to 
 * access the children nodes which are common to many structures.
 *
 */
 
/*! \class DataArray_t
 *  \brief Pointer to a CGNS DataArray_t structure
 */
class DataArray_t : public structure_t
{
public:
	DataArray_t() : structure_t(NULL) {}
	DataArray_t( const structure_t& n ) : structure_t( n ) {}

	void  readData( vector<int>& data ) const;
	void  readData( vector<float>& data ) const;
	void  readData( vector<double>& data ) const;
	
	ChildMethod_Descriptor
	ChildMethod_DataClass
	ChildMethod_DimensionalUnits
	ChildMethod_DimensionalExponents
	ChildMethod_DataConversion
};

/*! \class UserDefinedData_t
 *  \brief Pointer to a CGNS UserDefinedData_t structure
 */
class UserDefinedData_t : public structure_t
{
public:
	UserDefinedData_t() : structure_t(NULL) {}
	UserDefinedData_t( const structure_t& n ) : structure_t(n) {}
	
	void writeRange( const range& r );

	ChildMethod_Descriptor
	ChildMethod_GridLocation
	ChildMethod_DataArray
	ChildMethod_DataClass
	ChildMethod_DimensionalUnits
	ChildMethod_UserDefinedData
};

/*! \class ReferenceState_t
 *  \brief Pointer to a CGNS ReferenceState_t structure
 */
class ReferenceState_t : public structure_t
{
public:
	ReferenceState_t() : structure_t(NULL) {}
	ReferenceState_t( const structure_t& n ) : structure_t(n) {}

	ChildMethod_Descriptor
	ChildMethod_DataArray
	ChildMethod_DataClass
	ChildMethod_DimensionalUnits
	ChildMethod_UserDefinedData
};

/*! \class ConvergenceHistory_t
 *  \brief Pointer to a CGNS ConvergenceHistory_t structure
 */
class ConvergenceHistory_t : public structure_t
{
public:
	ConvergenceHistory_t() : structure_t(NULL) {}
	ConvergenceHistory_t( const structure_t& n ) : structure_t(n) {}
	
	ChildMethod_Descriptor
	ChildMethod_DataArray
	ChildMethod_DataClass
	ChildMethod_DimensionalUnits
	ChildMethod_UserDefinedData
};

/*! \class RotatingCoordinates_t
 *  \brief Pointer to a CGNS RotatingCoordinates_t structure
 */
class RotatingCoordinates_t : public structure_t
{
public:
	RotatingCoordinates_t() : structure_t(NULL) {}
	RotatingCoordinates_t( const structure_t& n ) : structure_t(n) {}

	ChildMethod_Descriptor
	ChildMethod_DataClass
	ChildMethod_DimensionalUnits
	ChildMethod_UserDefinedData
};
   
// ------------------------- physical model descriptions ---------------------------

/*! \class model_t
 *  \brief Abstract base class for the various CGNS model data structure
 */
class model_t  : public structure_t
{
protected:
	model_t() : structure_t(NULL) {} //!< default constructor
	model_t( const structure_t& n ) : structure_t(n) {}

	ChildMethod_Descriptor
	ChildMethod_DataArray
	ChildMethod_DataClass
	ChildMethod_DimensionalUnits
	ChildMethod_UserDefinedData
};

/*! \class TurbulenceModel_t
 *  \brief pointer to a CGNS TurbulenceModel_t data structure
 */
class TurbulenceModel_t : public model_t
{
public:
	TurbulenceModel_t() : model_t() {} //!< default constructor
	TurbulenceModel_t( const structure_t& n ) : model_t(n) {}

	void readDiffusion( vector<bool>& diff_flags ) const;
	void writeDiffusion( const vector<bool>& diff_flags );
};

/*! \class TurbulenceClosure_t
 *  \brief pointer to a CGNS TurbulenceClosure_t data structure
 */
class TurbulenceClosure_t : public model_t
{
public:
	TurbulenceClosure_t() : model_t() {} //!< default constructor
	TurbulenceClosure_t( const structure_t& n ) : model_t(n) {}
};

/*! \class ViscosityModel_t
 *  \brief pointer to a CGNS ViscosityModel_t data structure
 */
class ViscosityModel_t : public model_t
{
public:
	ViscosityModel_t() : model_t() {} //!< default constructor
	ViscosityModel_t( const structure_t& n ) : model_t(n) {}
};

/*! \class GasModel_t
 *  \brief pointer to a CGNS GasModel_t data structure
 */
class GasModel_t : public model_t
{
public:
	GasModel_t() : model_t( NULL ) {} //!< default constructor
	GasModel_t( const structure_t& n ) : model_t(n) {}
};

/*! \class ThermalConductivityModel_t
 *  \brief pointer to a CGNS ThermalConductivityModel_t data structure
 */
class ThermalConductivityModel_t : public model_t
{
public:
	ThermalConductivityModel_t() : model_t( NULL ) {} //!< default constructor
	ThermalConductivityModel_t( const structure_t& n ) : model_t(n) {}
};

/*! \class ThermalRelaxationModel_t
 *  \brief pointer to a CGNS ThermalRelaxationModel_t data structure
 */
class ThermalRelaxationModel_t : public model_t
{
public:
	ThermalRelaxationModel_t() : model_t( NULL ) {} //!< default constructor
	ThermalRelaxationModel_t( const structure_t& n ) : model_t(n) {}
};

/*! \class ChemicalKineticsModel_t
 *  \brief pointer to a CGNS ChemicalKineticsModel_t data structure
 */
class ChemicalKineticsModel_t : public model_t
{
public:
	ChemicalKineticsModel_t() : model_t( NULL ) {} //!< default constructor
	ChemicalKineticsModel_t( const structure_t& n ) : model_t(n) {}
};

/*! \class GoverningEquations_t
 *  \brief Pointer to a CGNS GoverningEquations_t structure
 */
class GoverningEquations_t : public structure_t
{
public:
	GoverningEquations_t() : structure_t( NULL ) {} //!< default constructor
	GoverningEquations_t( const structure_t& n ) : structure_t(n) {}

	void readDiffusionModel( vector<int>& diff ) const;
	void writeDiffusionModel( vector<int> diff );

	ChildMethod_Descriptor
	ChildMethod_UserDefinedData
};

/*! \class FlowEquationSet_t
 *  \brief Pointer to a CGNS FlowEquationSet_t structure
 */
class FlowEquationSet_t : public structure_t
{
public:
	FlowEquationSet_t() : structure_t( NULL ) {} //!< default constructor
	FlowEquationSet_t( const structure_t& n ) : structure_t(n) {}

	GoverningEquations_t       writeGoverningEquations( GoverningEquationsType_t );
	TurbulenceModel_t          writeTurbulenceModel( ModelType_t );
	TurbulenceClosure_t        writeTurbulenceClosure( ModelType_t );
	ViscosityModel_t           writeViscosityModel( ModelType_t );
	GasModel_t                 writeGasModel( ModelType_t );
	ThermalConductivityModel_t writeThermalConductivityModel( ModelType_t );
	ThermalRelaxationModel_t   writeThermalRelaxationModel( ModelType_t );
	ChemicalKineticsModel_t    writeChemicalKineticsModel( ModelType_t );

	ChildMethod_Descriptor
	ChildMethod_DataClass
	ChildMethod_DimensionalUnits
	ChildMethod_UserDefinedData
};

//--------------------------------------------------

/*! \class Base_t
 *  \brief Pointer to a CGNS Base_t structure
 */
class Base_t : public structure_t
{
public:
	Base_t() : structure_t( NULL ) {} //!< default constructor
	Base_t( const file& f, int b, int pd ) : structure_t( f, b ) { set_attribute( "PhysicalDimension",pd); } //!< constructs a Base_t from a given file pointer and a bse number (>=1)
	Base_t( const structure_t& n ) : structure_t( n ) {}    //!< constructs a Base_t from an existing structure_t
		
	int	     getPhysicalDimension() const { int pd; nd()->get_attribute("PhysicalDimension",pd); return pd; /*return nd()->get_physical_dimension();*/ }
	int	     getCellDimension() const { return nd()->get_cell_dimension(); }

	int          getNbZone() const;
	Zone_t       readZone( int index, string& zonename, vector<int>& nodesize, vector<int>& cellsize, vector<int>& bndrysize, ZoneType_t& type ) const;
	Zone_t       writeZone( const string& zonename, const vector<int>& nodesize, const vector<int>& cellsize, const vector<int>& bndrysize, ZoneType_t type );
	//deprecated - Zone_t       readZone( int index, string& Zone_tname, vector<int>& size, ZoneType_t& type ) const;
	//deprecated - Zone_t       writeZone( const string& name, vector<int>& dimensions, ZoneType_t type = Structured );

	Axisymetry_t readAxisymmetry( vector<float>& refpt, vector<float>& axis ) const;
	Axisymetry_t writeAxisymmetry( const vector<float>& refpt, const vector<float>& axis );

	Gravity_t    readGravity( vector<float>& g ) const;
	Gravity_t    writeGravity( const vector<float>& g );

	void         readSimulationType( SimulationType_t& ) const;
	void         writeSimulationType( SimulationType_t );

	int          getNbFamily() const;
	Family_t     readFamily( int ifamily, string& familyname, bool& hasfamilybc, int& ngeoref ) const;
	Family_t     writeFamily( const string& famname );

	int            getNbIntegralData() const;
	IntegralData_t readIntegralData( int index, string& name ) const;
	IntegralData_t writeIntegralData( const string& name );

	BaseIterativeData_t readBaseIterativeData( string& name, int& nsteps ) const;
	BaseIterativeData_t writeBaseIterativeData( const string& name, int nsteps );

	ChildMethod_Descriptor
	ChildMethod_DataClass
	ChildMethod_FlowEquationSet
	ChildMethod_RotatingCoordinates
	ChildMethod_ConvergenceHistory
	ChildMethod_ReferenceState
	ChildMethod_DimensionalUnits
	ChildMethod_UserDefinedData
};

/*! \class Zone_t
 *  \brief Pointer to a CGNS Zone_t structure
 */
class Zone_t : public structure_t
{
protected:
	Base_t getBase() const { return Base_t(parent()); }
	
public:
	Zone_t() : structure_t( NULL ) {} //!< default constructor
	Zone_t( const structure_t& n ) : structure_t( n ) {}
	//Zone_t( const structure_t& n, ZoneType_t type ) : structure_t( n ) { set_attributes( "ZoneType", type ); }
	
	int		       getIndexDimension() const { return nd()->get_index_dimension(); }
	ZoneType_t             getZoneType() const { ZoneType_t t; nd()->get_attribute("ZoneType",t); return t; }

	ZoneBC_t               readZoneBC() const;
	ZoneBC_t               writeZoneBC();

	ZoneGridConnectivity_t readZoneGridConnectivity() const;
	ZoneGridConnectivity_t writeZoneGridConnectivity();

	int                    getNbGridCoordinates() const;
	GridCoordinates_t      readGridCoordinates( int index, string& coordname ) const;
	GridCoordinates_t      writeGridCoordinates( const string& gcname = "GridCoordinates" );

	int                    getNbFlowSolution() const;
	FlowSolution_t	       readFlowSolution( int isol, string& solname, GridLocation_t& type ) const;
	FlowSolution_t	       writeFlowSolution( const string& quantity, GridLocation_t );

	int                    getNbElements() const;
	Elements_t	       readElements( int index, string& sectionname, ElementType_t& type, int& start, int& end, int& nbndry, bool& parent ) const;
	Elements_t	       writeElements( const string& sectionname, ElementType_t, int start, int end, int bndry, const vector<int>& connectivity );

	int                    getNbDiscreteData() const;
	DiscreteData_t         readDiscreteData( int index, string& name ) const;
	DiscreteData_t         writeDiscreteData( const string& name );
	
	int                    getNbRigidGridMotion() const;
	RigidGridMotion_t      readRigidGridMotion( int index, string& name, RigidGridMotionType_t& rgmt ) const;
	RigidGridMotion_t      writeRigidGridMotion( const string& name, RigidGridMotionType_t rgmt );
	
	int                    getNbArbitraryGridMotion() const;
	ArbitraryGridMotion_t  readArbitraryGridMotion( int index, string& name, ArbitraryGridMotionType_t& rgmt ) const;
	ArbitraryGridMotion_t  writeArbitraryGridMotion( const string& name, ArbitraryGridMotionType_t rgmt );
	
	ZoneIterativeData_t    readZoneIterativeData( string& name ) const;
	ZoneIterativeData_t    writeZoneIterativeData( const string& name );

	ChildMethod_Descriptor
	ChildMethod_DataClass
	ChildMethod_FamilyName
	ChildMethod_FlowEquationSet
	ChildMethod_RotatingCoordinates
	ChildMethod_ConvergenceHistory
	ChildMethod_ReferenceState
	ChildMethod_DimensionalUnits
	ChildMethod_UserDefinedData
};

/*! \class ZoneGridConnectivity_t
 *  \brief Pointer to a CGNS ZoneGridConnectivity_t structure
 */
class ZoneGridConnectivity_t : public structure_t
{
protected:
	Zone_t getZone() const { return Zone_t(parent()); }
	Base_t getBase() const { return Base_t(getZone().parent()); }
	
public:
	ZoneGridConnectivity_t() : structure_t( NULL ) {} //!< default constructor
	ZoneGridConnectivity_t( const structure_t& n ) : structure_t( n ) {}
	
	// 1-to-1 connectivity
	int     	       getNbGridConnectivity1to1() const;
	GridConnectivity1to1_t readGridConnectivity1to1( int index, string& connecname, string& donorname, vector<int>& range, vector<int>& donorrange, vector<int>& transform ) const;
	GridConnectivity1to1_t writeGridConnectivity1to1( const string& connecname, const string& donorname,
							const vector<int>& range, const vector<int>& donorrange, const vector<int>& transform );
	// same method using ranges as arguments
	GridConnectivity1to1_t readGridConnectivity1to1( int index, string& connecname, string& donorname, range& r, range& donorrange, vector<int>& transform ) const;
	GridConnectivity1to1_t writeGridConnectivity1to1( const string& connecname, const string& donorname, const range& r, const range& donorrange, const vector<int>& transform );

	// general connectivity
	int     	       getNbGridConnectivity() const;
	GridConnectivity_t     readGridConnectivity( int index, const vector<int>& points, vector<int>& donordata ) const;
	GridConnectivity_t     writeGridConnectivity( const string& connecname,
						GridLocation_t, GridConnectivityType_t, PointSetType_t,
						int indexdim, const vector<int>& points, const string& donorname,
						ZoneType_t donorzonetype, PointSetType_t donorpsettype,
						const vector<int>& donordata );
	GridConnectivity_t     writeGridConnectivity( const string& connecname,
						GridLocation_t, GridConnectivityType_t, int indexdim,
						const range& rangepoints, const string& donorname,
						ZoneType_t donorZone_ttype, PointSetType_t donorpsettype,
						const vector<int>& donordata );

	// overset connectivity
	int     	       getNbOversetHoles() const;
	void                   getOversetHolesInfo( int index, string& holename, GridLocation_t& gloc, PointSetType_t& pstype, int& npointsets, int& npts ) const;
	OversetHoles_t         readOversetHoles( int index, vector<int>& points );
	OversetHoles_t         writeOversetHoles( const string& holename, GridLocation_t, const vector<int>& points );
	OversetHoles_t         writeOversetHoles( const string& holename, GridLocation_t, const range& rangepoints );

	ChildMethod_Descriptor
	ChildMethod_UserDefinedData
};

/*! \class Elements_t
 *  \brief Pointer to a CGNS Elements_t structure
 */
class Elements_t : public structure_t
{
protected:
	Zone_t getZone() const { return Zone_t(parent()); }
	Base_t getBase() const { return Base_t(getZone().parent()); }
	
	bool hasParentData() const;
	int findDataArrayIndex( const string& name ) const; // overrides definition in structure_t 
	
public:
	Elements_t() : structure_t( NULL ) {} //!< default constructor
	Elements_t( const structure_t& n ) : structure_t(n) {}

	DataArray_t readConnectivity() const; // provides access to the connectivity as a DataArray_t
	/*
	 * Reading/Writing an Element section with CGNS is most peculiar! 
	 */
	//!< This one reads both connectivity and parent data but you can't access the DataArray itself, NULL is accepted as either argument
	void        readConnectivityAndParent( vector<int>* connectivity, vector<int>* parentdata ) const;
	//!< This one reads both connectivity and parent data, discards the parentdata and return a DataArray_t corresponding to the connectivity table
	DataArray_t readConnectivity( vector<int>& connectivity ) const;
	//!< This one reads both connectivity and parent data, discards the connectivity and return a DataArray_t corresponding to the parent data table
	DataArray_t readParentData( vector<int>& parentdata ) const;
	//!< Adds the optional parent data under an Element_t
	DataArray_t writeElementParents( const vector<int>& parentdata );

	ChildMethod_Descriptor
	ChildMethod_UserDefinedData
};

/*! \class GridCoordinates_t
 *  \brief Pointer to a CGNS GridCoordinates_t structure
 */
class GridCoordinates_t : public structure_t
{
protected:
	Zone_t getZone() const { return Zone_t(parent()); }
	Base_t getBase() const { return Base_t(getZone().parent()); }

private:
	int get_dataarray_index( const string& cooname ) const;

public:
	/*! \enum coordinatesystem_t
	 * used in writeCoordinatesData below
	 * either (X,Y,Z) , (R,T,Z)  ,  (R,T,Phi) -- other combinations are not recognized
	 */
	enum coordinatesystem_t { CARTESIAN, CYLINDRICAL, SPHERICAL };  //!< accepted coordinate systems

public:
	GridCoordinates_t() : structure_t( NULL ) {} //!< default constructor
	GridCoordinates_t( const structure_t& n ) : structure_t(n) {}

	int         getNbCoordinatesData() const;
	void        getCoordinatesDataInfo( int index, string& dataname, DataType_t& type ) const;
	
	DataArray_t readCoordinatesData( const string& coordname, const range& r, vector<float>& coo ) const;
	DataArray_t readCoordinatesData( const string& coordname, const range& r, vector<double>& coo ) const;
	DataArray_t readCoordinatesData( const string& coordname, vector<float>& coo ) const;
	DataArray_t readCoordinatesData( const string& coordname, vector<double>& coo ) const;

	DataArray_t writeCoordinatesData( const string& coordname, const vector<float>& coo );
	DataArray_t writeCoordinatesData( const string& coordname, const vector<double>& coo );
	void        writeCoordinatesData( coordinatesystem_t coosys, const vector<double>& coo1, const vector<double>& coo2, const vector<double>& coo3 );
	
	void	    readRind( vector<int>& rinddata ) const;
	void        writeRind( const vector<int>& rinddata );	

	ChildMethod_Descriptor
	ChildMethod_DataArray
	ChildMethod_DataClass
	ChildMethod_DimensionalUnits
	ChildMethod_UserDefinedData
};

/*! \class GridConnectivity1to1_t
 *  \brief Pointer to a CGNS GridConnectivity1to1_t structure
 */
class GridConnectivity1to1_t : public structure_t
{
public:
	GridConnectivity1to1_t() : structure_t( NULL ) {} //!< default constructor
	GridConnectivity1to1_t( const structure_t& n ) : structure_t(n) {}

	GridConnectivityProperty_t readProperty() const;
	GridConnectivityProperty_t writeProperty();
	
	ChildMethod_Descriptor
	ChildMethod_UserDefinedData
};

/*! \class GridConnectivity_t
 *  \brief Pointer to a CGNS GridConnectivity_t structure
 */
class GridConnectivity_t : public structure_t
{
public:
	GridConnectivity_t() : structure_t( NULL ) {} //!< default constructor
	GridConnectivity_t( const structure_t& n ) : structure_t(n) {}

	GridConnectivityProperty_t readProperty() const;
	GridConnectivityProperty_t writeProperty();
	
	ChildMethod_Descriptor
	ChildMethod_GridLocation
	ChildMethod_DataArray
	ChildMethod_UserDefinedData
};

/*! \class OversetHoles_t
 *  \brief Pointer to a CGNS OversetHoles_t structure
 */
class OversetHoles_t : public structure_t
{
public:
	OversetHoles_t() : structure_t( NULL ) {} //!< default constructor
	OversetHoles_t( const structure_t& n ) : structure_t(n) {}

	ChildMethod_Descriptor
	ChildMethod_GridLocation
	ChildMethod_UserDefinedData
};

/*! \class GridConnectivityProperty_t
 *  \brief Pointer to a CGNS GridConnectivityProperty_t structure
 */
class GridConnectivityProperty_t : public structure_t
{
protected:
	ZoneGridConnectivity_t getZoneGridConnectivity() const { return ZoneGridConnectivity_t(parent().parent()); }
	Zone_t getZone() const { return Zone_t(getZoneGridConnectivity().parent()); }
	Base_t getBase() const { return Base_t(getZone().parent()); }

public:
	GridConnectivityProperty_t() : structure_t( NULL ) {} //!< default constructor
	GridConnectivityProperty_t( const structure_t& n ) : structure_t(n) {}

	Periodic_t readGridConnectivityPeriodic( vector<float>& rotcenter, vector<float>& rotangle, vector<float>& translation ) const;
	Periodic_t writeGridConnectivityPeriodic( const vector<float>& rotcenter, const vector<float>& rotangle, const vector<float>& translation );

	Average_t  readGridConnectivityAverage( AverageInterfaceType_t& ) const;
	Average_t  writeGridConnectivityAverage( AverageInterfaceType_t );

	ChildMethod_Descriptor
	ChildMethod_UserDefinedData
};

/*! \class RigidGridMotion_t
 *  \brief Pointer to a CGNS RigidGridMotion_t structure
 */
class RigidGridMotion_t : public structure_t
{
protected:
	Zone_t getZone() const { return Zone_t(parent()); }
	Base_t getBase() const { return Base_t(getZone().parent()); }
	
public:
	RigidGridMotion_t() : structure_t( NULL ) {} //!< default constructor
	RigidGridMotion_t( const structure_t& n ) : structure_t(n) {}

	ChildMethod_Descriptor
	ChildMethod_DataArray
	ChildMethod_DataClass
	ChildMethod_DimensionalUnits
	ChildMethod_UserDefinedData
};

/*! \class ArbitraryGridMotion_t
 *  \brief Pointer to a CGNS ArbitraryGridMotion_t structure
 */
class ArbitraryGridMotion_t : public structure_t
{
protected:
	Zone_t getZone() const { return Zone_t(parent()); }
	Base_t getBase() const { return Base_t(getZone().parent()); }
	
public:
	ArbitraryGridMotion_t() : structure_t( NULL ) {} //!< default constructor
	ArbitraryGridMotion_t( const structure_t& n ) : structure_t(n) {}

	ChildMethod_Descriptor
	ChildMethod_GridLocation
	ChildMethod_DataArray
	ChildMethod_DataClass
	ChildMethod_DimensionalUnits
	ChildMethod_UserDefinedData
};

/*! \class ZoneBC_t
 *  \brief Pointer to a CGNS ZoneBC_t structure
 */
class ZoneBC_t : public structure_t
{
protected:
	Zone_t getZone() const { return Zone_t(parent()); }
	Base_t getBase() const { return Base_t(getZone().parent()); }
	
public:
	ZoneBC_t() : structure_t( NULL ) {} //!< default constructor
	ZoneBC_t( const structure_t& n ) : structure_t(n) {}

	int getNbBoundaryConditions() const;

	BC_t readBC( int index, string& bcname, BCType_t& bctype, PointSetType_t& psettype ) const;
	BC_t writeBC( const string& bcname, BCType_t, PointSetType_t, const vector<int>& points );
	BC_t writeBC( const string& bcname, BCType_t, PointSetType_t, const range& points );
	//BC_t writeBC( const string& bcname, BCType_t, const range& points );

	ChildMethod_Descriptor
	ChildMethod_ReferenceState
	ChildMethod_DataClass
	ChildMethod_DimensionalUnits
	ChildMethod_UserDefinedData
};

/*! \class BC_t
 *  \brief Pointer to a CGNS BC_t structure
 */
class BC_t : public structure_t
{
protected:
	ZoneBC_t getZoneBC() const { return ZoneBC_t(parent()); }
	Zone_t   getZone() const   { return Zone_t(getZoneBC().parent()); }
	Base_t   getBase() const   { return Base_t(getZone().parent()); }
	void     getInfo( const char* mname, PointSetType_t& psettype, int& ) const;
	
public:
	BC_t() : structure_t( NULL ) {} //!< default constructor
	BC_t( const structure_t& n ) : structure_t(n) {}

	//int   getListLength() const;
	
	void  readPointList( vector<int>& bcpointlist ) const;
	void  readPointRange( range& bcpointrange ) const;
	void  readElementList( vector<int>& bcelemlist ) const;
	void  readElementRange( range& bcelemrange ) const;
        // writing of Points/Elements List/Range occurs in ZoneBC
	
	void  readNormalIndex( int normals[3] ) const;
	void  readNormal( vector<float> normals ) const;	
	void  writeNormalIndex( const int normalindex[3] );
	void  writeNormal( const vector<float>& normal );
	void  writeNormal( const vector<double>& normal );

	int          getNbDataSet() const;
	BCDataSet_t  readDataSet( int index, string& dsname, BCType_t& type, bool& dirichlet, bool& neumann ) const;
	BCDataSet_t  writeDataSet( const string& dsname, BCType_t type );

	BCProperty_t readBCProperty() const;
	BCProperty_t writeBCProperty() const;

	ChildMethod_Descriptor
	ChildMethod_FamilyName
	ChildMethod_GridLocation
	ChildMethod_ReferenceState
	ChildMethod_DataClass
	ChildMethod_DimensionalUnits
	ChildMethod_UserDefinedData
};

/*! \class BCDataSet_t
 *  \brief Pointer to a CGNS BCDataSet_t structure
 */
class BCDataSet_t : public structure_t
{
public:
	BCDataSet_t() : structure_t(NULL) {}
	//BCDataSet_t( const structure_t& n ) : structure_t(n) {}
	BCDataSet_t( const structure_t& n, const string& dsname, BCType_t t ) : structure_t(n) 
	{
		set_attribute( "Name", dsname );
		set_attribute( "BCType", t );
	}

	void     readPointRange( range& ptlist ) const;
	void     readPointList( vector<int>& ptlist ) const;
	void     writePointRange( const range& ptlist ) const;
	void     writePointList( const vector<int>& ptlist ) const;

	BCData_t readBCData( BCDataType_t type ) const;
	BCData_t writeBCData( BCDataType_t type );

	ChildMethod_Descriptor
	ChildMethod_GridLocation
	ChildMethod_ReferenceState
	ChildMethod_DataClass
	ChildMethod_DimensionalUnits
	ChildMethod_UserDefinedData
};

/*! \class BCData_t
 *  \brief Pointer to a CGNS BCData_t structure
 */
class BCData_t : public structure_t
{
public:
	BCData_t() : structure_t(NULL) {}
	BCData_t( const structure_t& n ) : structure_t(n) {}

	//int getListLength() const;

	ChildMethod_Descriptor
	ChildMethod_DataArray
	ChildMethod_DataClass
	ChildMethod_DimensionalUnits
	ChildMethod_UserDefinedData
};

/*! \class BCProperty_t
 *  \brief Pointer to a CGNS BCProperty_t structure
 */
class BCProperty_t : public structure_t
{
protected:
	BC_t     getBC() const     { return BC_t(parent()); }
	ZoneBC_t getZoneBC() const { return ZoneBC_t(getBC().parent()); }
	Zone_t   getZone() const   { return Zone_t(getZoneBC().parent()); }
	Base_t   getBase() const   { return Base_t(getZone().parent()); }
	
public:
	BCProperty_t() : structure_t(NULL) {}
	BCProperty_t( const structure_t& n ) : structure_t(n) {}

	WallFunction_t readWallFunction( WallFunctionType_t& wf ) const;
	WallFunction_t writeWallFunction( WallFunctionType_t wf );
	Area_t         readArea( AreaType_t& Area_t, double& surface, string& region ) const;
	Area_t         writeArea( AreaType_t Area_t, double surface, const string& region );

	ChildMethod_Descriptor
	ChildMethod_UserDefinedData
};

/*! \class Area_t
 *  \brief Pointer to a CGNS Area_t structure
 */
class Area_t : public structure_t
{
public:
	Area_t() : structure_t(NULL) {}
	Area_t( const structure_t& n ) : structure_t(n) {}

	ChildMethod_Descriptor
	ChildMethod_UserDefinedData
};

/*! \class WallFunction_t
 *  \brief Pointer to a CGNS WallFunction_t structure
 */
class WallFunction_t : public structure_t
{
public:
	WallFunction_t() : structure_t(NULL) {}
	WallFunction_t( const structure_t& n ) : structure_t(n) {}

	ChildMethod_Descriptor
	ChildMethod_UserDefinedData
};

/*! \class Gravity_t
 *  \brief Pointer to a CGNS Gravity_t structure
 */
class Gravity_t : public structure_t
{
public:
	Gravity_t() : structure_t(NULL) {}
	Gravity_t( const structure_t& n ) : structure_t(n) {}
	
	ChildMethod_Descriptor
	ChildMethod_DataClass
	ChildMethod_DimensionalUnits
	ChildMethod_UserDefinedData
};

/*! \class Periodic_t
 *  \brief Pointer to a CGNS Periodic_t structure
 */
class Periodic_t : public structure_t
{
public:
	Periodic_t() : structure_t(NULL) {}
	Periodic_t( const structure_t& n ) : structure_t(n) {}

	ChildMethod_Descriptor
	ChildMethod_DataClass
	ChildMethod_DimensionalUnits
	ChildMethod_UserDefinedData
};

/*! \class Average_t
 *  \brief Pointer to a CGNS Average_t structure
 */
class Average_t : public structure_t
{
public:
	Average_t() : structure_t(NULL) {}
	Average_t( const structure_t& n ) : structure_t(n) {}

	ChildMethod_Descriptor
	ChildMethod_UserDefinedData
};

/*! \class BaseIterativeData
 *  \brief Pointer to a CGNS BaseIterativeData_t structure
 */
class BaseIterativeData_t : public structure_t
{
public:
	BaseIterativeData_t() : structure_t(NULL) {}
	BaseIterativeData_t( const structure_t& n ) : structure_t(n) {}

	ChildMethod_Descriptor
	ChildMethod_DataArray
	ChildMethod_DataClass
	ChildMethod_DimensionalUnits
	ChildMethod_UserDefinedData
};

/*! \class ZoneIterativedata_t
 *  \brief Pointer to a CGNS ZoneIterativeData_t structure
 */
class ZoneIterativeData_t : public structure_t
{
public:
	ZoneIterativeData_t() : structure_t(NULL) {}
	ZoneIterativeData_t( const structure_t& n ) : structure_t(n) {}

	ChildMethod_Descriptor
	ChildMethod_DataArray
	ChildMethod_DataClass
	ChildMethod_DimensionalUnits
	ChildMethod_UserDefinedData
};

/*! \class DiscreteData_t
 *  \brief Pointer to a CGNS DiscreteData_t structure
 */
class DiscreteData_t : public structure_t
{
public:
	DiscreteData_t() : structure_t(NULL) {}
	DiscreteData_t( const structure_t& n ) : structure_t(n) {}
};

/*! \class IntegralData_t
 *  \brief Pointer to a CGNS IntegralData_t structure
 */
class IntegralData_t : public structure_t
{
public:
	IntegralData_t() : structure_t(NULL) {}
	IntegralData_t( const structure_t& n ) : structure_t(n) {}
};

/*! \class Axisymetry_t
 *  \brief Pointer to a CGNS Axisymetry_t structure
 */
class Axisymetry_t : public structure_t
{
public:
	Axisymetry_t() : structure_t(NULL) {}
	Axisymetry_t( const structure_t& n ) : structure_t(n) {}

	ChildMethod_Descriptor
	ChildMethod_DataClass
	ChildMethod_DimensionalUnits
	ChildMethod_UserDefinedData
};

/*! \class Family_t
 *  \brief Pointer to a CGNS Family_t structure
 */
class Family_t : public structure_t
{
private:
	static const string attrib_HasFamilyBC;
	static const string attrib_NbGeoRef;
	
protected:
	Base_t getBase() const { return Base_t(parent()); }

public:
	Family_t( const structure_t& n ) : structure_t(n) {}
	Family_t( const structure_t& n, bool hasfbc, int ngeoref ) : structure_t(n) 
	{
		set_attribute( attrib_HasFamilyBC, hasfbc );
		set_attribute( attrib_NbGeoRef, ngeoref );
	}
 
	bool                hasFamilyBC() const { bool hasfbc; get_attribute( attrib_HasFamilyBC, hasfbc ); return hasfbc; }
	FamilyBC_t          writeFamilyBC( const string& fbcname, BCType_t t );
	FamilyBC_t          readFamilyBC( string& fbcname, BCType_t& t ) const;

	int                 getNbGeoRef() const { int ngeo; get_attribute( attrib_NbGeoRef, ngeo ); return ngeo; }
	GeometryReference_t writeGeoRef( const string& geoname, const string& filename, const string& fileformat );
	GeometryReference_t readGeoRef ( int index, string& geoname, string& filename, string& fileformat ) const;

	ChildMethod_Descriptor
	ChildMethod_UserDefinedData
};

/*! \class GeometryReference_t
 *  \brief Pointer to a CGNS GeometryReference_t structure
 */
class GeometryReference_t : public structure_t
{
private:
	static const string attrib_NbGeoParts;

protected:
	Family_t getFamily() const { return Family_t(parent()); }
	Base_t   getBase() const { return Base_t(getFamily()); }

public:
	GeometryReference_t() : structure_t(NULL) {}
	GeometryReference_t( const structure_t& n, int nparts ) : structure_t(n) { set_attribute( attrib_NbGeoParts, nparts ); }
	
	int  getNbPart() const { int np; get_attribute( attrib_NbGeoParts, np ); return np; }
	void writePart( const string& partname );
	void readPart ( int index, string& partname ) const;

	ChildMethod_Descriptor
	ChildMethod_UserDefinedData
};

/*! \class FamilyBC_t
 *  \brief Pointer to a CGNS FamilyBC_t structure
 */
class FamilyBC_t : public structure_t
{
private:
	static const string attrib_NbDataSet;

public:
	FamilyBC_t() : structure_t(NULL) {}
	FamilyBC_t( const structure_t& n, int nds ) : structure_t(n) { set_attribute( attrib_NbDataSet, nds ); }

	int         getNbDataSet() const { int nds; get_attribute( attrib_NbDataSet, nds ); return nds; }
	BCDataSet_t readBCDataSet( int index, string& dsname, BCType_t& t, bool& dirichlet, bool& neumann ) const;
	BCDataSet_t writeBCDataSet( const string& dsname, BCType_t t /*, BCDataType datatype*/ );
};

/*! \class FlowSolution_t
 *  \brief Pointer to a CGNS FlowSolution_t structure
 */
class FlowSolution_t : public structure_t
{
protected:
	Zone_t getZone() const { return Zone_t(parent()); }
	Base_t getBase() const { return Base_t(getZone().parent()); }

public:
	FlowSolution_t() : structure_t(NULL) {}
	FlowSolution_t( const structure_t& n ) : structure_t(n) {}

	int getNbFields() const;
	DataArray_t readField( int ifield, string& name, DataType_t& type ) const;
	DataArray_t writeField( const string& name, vector<float>& values );
	DataArray_t writeField( const string& name, vector<double>& values );

	ChildMethod_Descriptor
	ChildMethod_GridLocation
	ChildMethod_DataArray
	ChildMethod_DataClass
	ChildMethod_DimensionalUnits
	ChildMethod_UserDefinedData
};

} // namespace

#endif
