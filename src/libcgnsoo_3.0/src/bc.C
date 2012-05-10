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
 * Internal method used to get some info about this BC_t
 * \param method_name Name of the calling method - usde for issuing appropriate traceback in case of error. 
 * \param psettype    The PointSetType_t of this BC_t
 * \param nptsa       the number of points defining the region of this BC_t
 */
void BC_t::getInfo( const char* method_name, PointSetType_t& psettype, int& npnts ) const
{
	BCType_t     bctype;
	int          nlistflag, ndataset;
	cgnsstring   boconame;
	DataType_t   normaltype;
	Array<int>   normalindices(3);
	int ier = cg_boco_info( getFileID(), 
				getBase().getID(), 
				getZone().getID(), 
				getID(), 
				boconame, &bctype, 
				&psettype, &npnts, 
				normalindices, &nlistflag, &normaltype, 
				&ndataset );
	check_error( method_name, "cg_boco_info", ier );
}

/*!
 * Reads the point range defining a rectangular BC patch
 * \param bcpointrange range of points defining this BC region. 
 */
void BC_t::readPointRange( range& bcpointrange ) const
{
	// find out how many we are dealing with (this is stored in the BC_t node)
	static char method_name[] = "BC_t::readPointRange";
	PointSetType_t psettype;
	int npts;
	getInfo(method_name,psettype,npts);	
	if ( psettype != PointRange ) 
		throw cgns_mismatch( method_name, "This BC is not defined by a PointRange" );
	
	// according to the MLL, for a PointRange, npts is always 2
	// multiply by phys_dim to get total number of points...
	int dim = getBase().getPhysicalDimension();
	npts *= dim;
	
	// don't care about the normalList
	// the MLL does not copy the normals if the given array is NULL
	Array<int> v(npts);
	int ier = cg_boco_read( getFileID(), 
				getBase().getID(), 
				getZone().getID(), 
				getID(), 
				v, NULL );
	check_error( method_name, "cg_boco_read", ier );
	
	bcpointrange = v;
}

/*!
 * Reads the list of point indices defining an arbitrary BC patch
 * \param bcpointlist vector of point indices defining this BC region. 
 */
void BC_t::readPointList( vector<int>& bcpointlist ) const
{
	static char mname[] = "BC_t::readPointlist";
	
	PointSetType_t psettype;
	int npts;
	getInfo(mname,psettype,npts);	
	if ( psettype != PointList ) 
		throw cgns_mismatch( mname, "This BC is not defined by a PointList" );
	
	// don't care about the normalList.... 
	// the MLL does not copy the normals if the given array is NULL
	Array<int> v(npts);
	int ier = cg_boco_read( getFileID(), 
				getBase().getID(), 
				getZone().getID(), 
				getID(), 
				v, NULL );
	check_error( mname, "cg_boco_read", ier );

	bcpointlist = v;
}	

/*!
 * Reads the element range defining a rectangular BC patch
 * \param bcelemrange range of elements defining this BC region. 
 */
void BC_t::readElementRange( range& bcelemrange ) const
{
	static char mname[] = "BC_t::readElementRange";
	
	PointSetType_t psettype;
	int nelem;
	getInfo(mname,psettype,nelem);	
	if ( psettype != ElementRange ) 
		throw cgns_mismatch( mname, "This BC is not defined by an ElementRange" );
	
	// according to the MLL, nelem == 2..... 
	// multiply by phys_dim to get total number of points...
	int dim = getBase().getPhysicalDimension();
	nelem *= dim;
	
	// don't care about the normalList.... 
	// the MLL does not copy the normals if the given array is NULL
	Array<int> v(nelem);
	int ier = cg_boco_read( getFileID(), 
				getBase().getID(), 
				getZone().getID(), 
				getID(), 
				v, NULL );
	check_error( mname, "cg_boco_read", ier );
	
	bcelemrange = v;
}

/*!
 * Reads the list of element indices defining an arbitrary BC patch
 * \param bcelemlist vector of element indices defining this BC region. 
 */
void BC_t::readElementList( vector<int>& bcelemlist ) const
{
	static char mname[] = "BC_t::readElementList";
	
	PointSetType_t psettype;
	int nelem;
	getInfo(mname,psettype,nelem);	
	if ( psettype != ElementList ) 
		throw cgns_mismatch( mname, "This BC is not defined by an ElementList" );
	
	// don't care about the normalList.... 
	// the MLL does not copy the normals if the given array is NULL
	Array<int> v(nelem);
	int ier = cg_boco_read( getFileID(), 
				getBase().getID(), 
				getZone().getID(), 
				getID(), 
				v, NULL );
	check_error( mname, "cg_boco_read", ier );

	bcelemlist = v;
}

//--------------------------------- read/write information about normals -------------------------

/*!
 * Read the normals indices associated with a BC patch (in a structured mesh)
 * \param normals Three indices (0,+1 or -1) defining the face on which this BC patch lies.
 *
 * \note there is no way to know for sure whether the normalindex are actually 
 *       stored in the BC_t structure. If they are not there, the MLL just skip
 *       their assignment to normals[] which thus keeps its original values.
 */
void BC_t::readNormalIndex( int normals[3] ) const
{
	cgnsstring boconame;
	BCType_t bctype;
	PointSetType_t psettype;
	DataType_t normaldtype;
	int npts, nlistflag, ndataset;
	int ier = cg_boco_info( getFileID(), 
				getBase().getID(), 
				getZone().getID(), 
				getID(),
				boconame, &bctype, 
				&psettype, &npts, 
				normals, &nlistflag, &normaldtype, 
				&ndataset );
	check_error( "BC_t::readNormalIndex", "cg_boco_info", ier );
}

/*!
 * Read normals associated with a BC patch
 * \param normals vector of normals indices defining this BC region
 *
 * \note If the normals are not stored in the BC_t structure, an empty vector is returned.
 */
void BC_t::readNormal( vector<float> normals ) const
{
	// find out if we have the normals and what is their datatype
	cgnsstring boconame;
	BCType_t bctype;
	PointSetType_t psettype;
	DataType_t normaldtype;
	int normali[3];
	int npts, nlistflag, ndataset;
	int ier = cg_boco_info( getFileID(), 
				getBase().getID(), 
				getZone().getID(), 
				getID(),
				boconame, &bctype, 
				&psettype, &npts, 
				normali, &nlistflag, &normaldtype, 
				&ndataset );
	check_error( "BC_t::readBC", "cg_boco_info", ier );

	// do we have normals?	
	if ( nlistflag==0 )
	{
		// we could also return a cgns_notfound exception but returning 
		// an empty vector seems more in line with the MLL
		normals.resize(0);
		return;
	}

	// read the normal list and convert values to float if required	
	Array<int> dummypts(npts);
	int physdim = getBase().getPhysicalDimension();
	switch( normaldtype )
	{
	case RealSingle:
		{
		Array<float> v(npts*physdim);
        	int ier = cg_boco_read( getFileID(), 
					getBase().getID(), 
					getZone().getID(), 
					getID(), 
					dummypts, v );
		check_error( "BC_t::readDataArray", "cg_array_info", ier );
		normals = v;
		}
		break;
	case RealDouble:
		{
		double* v = new double[npts*physdim];
		int ier = cg_boco_read( getFileID(), 
					getBase().getID(), 
					getZone().getID(), 
					getID(), 
					dummypts, v );
		check_error( "BC_t::readDataArray", "cg_array_info", ier );
		normals.resize(npts*physdim);
		std::copy( v, v+npts*physdim, normals.begin() );
		delete [] v;
		}
		break;
	default:
		break;
	}
}

/*!
 * Write normal information associated with a BC patch
 * \param normalindices normals indices defining this BC region
 */
void BC_t::writeNormalIndex( const int normalindices[3] )
{
	if ( getZone().getZoneType() == Unstructured )
	{
		// normal indices only make sense in a structured zone
		throw cgns_mismatch( "BC_t::writeNormalIndex", "Current zone is not structured" );
	}
	int ier = cg_boco_normal_write( getFileID(), 
					getBase().getID(), 
					getZone().getID(), 
					getID(),
					normalindices, 0, 
					RealSingle, 
					NULL );
	check_error( "BC_t::writeBCNormal", "cg_boco_normal_write", ier );
}

/*!
 * Write normal information associated with a BC patch
 * \param normals vector of normals defining this BC region
 */
void BC_t::writeNormal( const vector<float>& normals )
{
	int nindices[3];
	Array<float> anormal( normals );
	int ier = cg_boco_normal_write( getFileID(), 
					getBase().getID(), 
					getZone().getID(), 
					getID(),
					nindices, 1, 
					RealSingle, 
					anormal );
	check_error( "BC_t::writeBCNormal", "cg_boco_normal_write", ier );
}

/*!
 * Write normal information associated with a BC patch
 * \param normals vector of normals defining this BC region
 */
void BC_t::writeNormal( const vector<double>& normals )
{
	int nindices[3];
	Array<double> anormal( normals );
	int ier = cg_boco_normal_write( getFileID(), 
					getBase().getID(), 
					getZone().getID(), 
					getID(),
					nindices, 1, 
					RealDouble, 
					anormal );
	check_error( "BC_t::writeBCNormal", "cg_boco_normal_write", ier );
}

/*! \todo We need some method to specify both normalIndex and normalList
 *        in situations where both item are needed 
 *        (i.e. discontinuity line of a structured mesh for example).
 */
 
#if 0
/*!
 * Compute the number points in a BC patch
 * \return Number of points defining the patch 
 * \note Is this still useful ???
 */
int BC_t::getListLength() const
{
	int ibc = getID();
	
	cgnsstring     boconame;
	BCType_t       bocotype;
	PointSetType_t pstype;
	DataType_t     ndtype;
	int            npts, nindex, nlflg, nds;
	int ier = cg_boco_info( getFileID(), getBase().getID(), getZone().getID(), ibc,
				boconame, &bocotype, &pstype, &npts, &nindex, &nlflg,
				&ndtype, &nds );
	check_error( "BC_t::getListLength", "cg_boco_info", ier );
	int listlength;
	if ( pstype == PointRange )
	{
		// get pointRange
		int pnts[4];
		int physdim = getBase().getPhysicalDimension();
		void *nlist = ( ndtype == RealSingle ) ? (void*)new float[physdim*2] : (void*)new double[physdim*2];
		int ier = cg_boco_read( getFileID(), getBase().getID(), getZone().getID(), ibc, pnts, nlist );
		check_error( "BC_t::getListLength", "cg_boco_read", ier );
		if ( ndtype == RealSingle )
			delete [] (float*)nlist;
		else
			delete [] (double*)nlist;
		listlength = (pnts[2]-pnts[0]+1)*(pnts[3]-pnts[1]+1);
	}
	else
	{
		listlength = npts;
	}
	return listlength;
}
#endif

//------------------------ dataset read/write ---------------------------------

/*!
 * Returns the number of dataset under this BC_t
 * \return Number of dataset under this BC_t
 */
int BC_t::getNbDataSet() const
{
	cgnsstring     boconame;
	BCType_t       bctype;
	PointSetType_t psettype;
	DataType_t     normaldt;
	int            nlistflag, ndataset, ndata;
	int            normali[3];
	int ier = cg_boco_info( getFileID(), 
				getBase().getID(), 
				getZone().getID(), 
				getID(), 
				boconame, &bctype, 
				&psettype, &ndata, 
				normali, &nlistflag, &normaldt, 
				&ndataset );
	check_error( "BC_t::getNbDataSet", "cg_boco_info", ier );
	
	return ndataset;
}

/*!
 * Reads a BCDataSet definition
 * \param index     Index of the requested dataset [Input]
 * \param dsname    Corresponding name of the dataset [Output]
 * \param type      Type of boundary condition [Output]
 * \param dirichlet Indicates if this dataset has Dirichlet information [Output]
 * \param neumann   Indicates if this dataset has Neumann information [Output]
 * \return Handle to the requested BCDataSet_t  
 */
BCDataSet_t BC_t::readDataSet( int index, string& dsname, BCType_t& type, bool& dirichlet, bool& neumann ) const
{
	cgnsstring name;
	int dflg, nflg;
	int ier = cg_dataset_read( getFileID(), 
				   getBase().getID(), 
				   getZone().getID(), 
				   getID(), 
				   ++index, 
				   name, &type, &dflg, &nflg );
	check_found( "BC_t::readDataSet", "BCDataSet_t", ier );
	check_error( "BC_t::readDataSet", "cg_dataset_read", ier );
	
	dsname    = name;
	dirichlet = dflg;
	neumann   = nflg;
	
	return BCDataSet_t( push("BCDataSet_t",index), name, type );
}

/*!
 * Writes a BCDataSet definition
 * \param dsname Name of the dataset to write [Input]
 * \param type   Type of boundary condition [Input]
 * \return Handle to the newly created BCDataSet_t  
 */
BCDataSet_t BC_t::writeDataSet( const string& dsname, BCType_t type )
{
	int index;
	int ier = cg_dataset_write( getFileID(), 
				    getBase().getID(), 
				    getZone().getID(), 
				    getID(), 
				    dsname.c_str(), type, &index );
	check_error( "BC_t::writeDataSet", "cg_dataset_write", ier );

	return BCDataSet_t( push("BCDataSet_t",index), dsname, type );
}

//------------------------ BCProperty read/write ---------------------------------

/*!
 * Reads the BCProperty associated with the BC_t
 * \returns Handle to the BCProperty  
 */
BCProperty_t BC_t::readBCProperty() const
{
	return BCProperty_t( push("BCProperty_t",1) );
}

/*!
 * Writes a BCProperty under this BC_t
 * \returns Handle to the BCProperty  
 */
BCProperty_t BC_t::writeBCProperty() const
{
	return BCProperty_t( push("BCProperty_t",1) );
}

}
