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

const string FamilyBC_t::attrib_NbDataSet = "NbDataSet";

/*!
\param dsname Name of the DataSet to write
\param type   Type of Boundary Condition (possibly Compound)
\return Handle to the created BCDataSet_t

Write a new BCDataSet_t under the current FamilyBC_t.

It is expected the user will call FamilyBC_t::writeBCDataSet once for each pair of
(dataset name, BCType) and then subsequently call BCDataSet_t::writeBCData for each 
piece of data using the appropriate BCDataSet_t parent.
Typical usage is
\code 
Family_t famiy = base.writeFamily( "InletFamily" );
FamilyBC_t fbc_inlet = family.writeFamilyBC( "Inlet", BCInflow );
BCDataSet bcds_sub   = fbc_inlet.writeBCDataSet( "Subsonic", BCInflowSubsonic );
	BCData bcd_sub_dir = bcds_sub.writeBCData( Dirichlet );
		DataArray usub = bcd_sub_dir.WriteDataArray( "VelocityX" );
BCDataSet bcds_super = fbc_inlet.writeBCDataSet( "Supersonic", BCInflowSupersonic );
	BCData bcd_sup_dir = bcds_sub.writeBCData( Neuman );
		DataArray usub = bcd_sub_dir.WriteDataArray( "VelocityX" );

*/  
BCDataSet_t FamilyBC_t::writeBCDataSet( const string& dsname, BCType_t type /*, BCDataType datatype*/ )
{
	// The behaviour of cg_bcdataset_write is peculiar. 
	// It does not always create a new dataset; in some situation
	// it can simply add a new BCData under the existing BCDataset !
	// Here is what the MLL documentation says:
	/*
	------
	The first time cg_bcdataset_write is called with a particular DatasetName, BCType, and 
	BCDataType, a new BCDataSet_t node is created, with a child BCData_t node. Subsequent 
	calls with the same DatasetName and BCType may be made to add additional BCData_t nodes, 
	of type BCDataType, to the existing BCDataSet_t node.
	-------
	*/
	
	/*!
	It is expected the user will call FamilyBC_t::writeBCDataSet once for each pair of
	(dataset name, BCType) and then subsequently call BCDataSet_t::writeBCData for each 
	piece of data using the appropriate BCDataSet_t parent.
	Typical usage is
	\code 
	Family_t famiy = base.writeFamily( "InletFamily" );
	FamilyBC_t fbc_inlet = family.writeFamilyBC( "Inlet", BCInflow );
	BCDataSet bcds_sub   = fbc_inlet.writeBCDataSet( "Subsonic", BCInflowSubsonic );
		BCData bcd_sub_dir = bcds_sub.writeBCData( Dirichlet );
			DataArray usub = bcd_sub_dir.WriteDataArray( "VelocityX" );
	BCDataSet bcds_super = fbc_inlet.writeBCDataSet( "Supersonic", BCInflowSupersonic );
		BCData bcd_sup_dir = bcds_sub.writeBCData( Neuman );
			DataArray usub = bcd_sub_dir.WriteDataArray( "VelocityX" );
			
	\note Due to some peculiarities in the MLL, one must be careful to complete the writing of
	aall information pertaining to a BCDataSet before going on to the next BCDataSet.
	*/  

	// Check how many dataset we have so far. Use an attribute since we can't read from a file opened for writing.
	int nds;
	get_attribute( attrib_NbDataSet, nds );
	
	// Postpone actual writing of the bcdataset until a call to BCDataSet_t::writeBCData(...) is made
	// Temporarily save BCType and dataset name as attributes of the BCDataSet
	// The following assumes that a BCDataSet with the same name and type does not already exist!
	BCDataSet_t ds( push( "BCDataSet_t", nds+1 ), dsname, type ); 

	return ds;
}

/*!
\param idnex  Index of dataset to read (>=0) [Input]
\param dsname Name of the DataSet [Output]
\param type   Type of Boundary Condition (possibly Compound) [Output]
\param dirichlet Indicates the availability of Dirichlet data [Output]
\param neumann   Indicates the availability of Neumann data [Output]
\return Handle to the created BCDataSet_t

Write a new BCDataSet_t under the current FamilyBC_t.
*/
BCDataSet_t FamilyBC_t::readBCDataSet( int index, string& dsname, BCType_t& type, bool& dirichlet, bool& neumann ) const
{
	go_here();
	cgnsstring name;
	int d, n;
	int ier = cg_bcdataset_read( ++index, name, &type, &d, &n );
	check_error( "FamilyBC_t::readBCDataSet", "cg_bcdataset_read", ier );
	dirichlet = (d!=0);
	neumann   = (n!=0);
	dsname = name;
	return BCDataSet_t( push( "BCDataSet_t", index ), dsname, type );
}

}
