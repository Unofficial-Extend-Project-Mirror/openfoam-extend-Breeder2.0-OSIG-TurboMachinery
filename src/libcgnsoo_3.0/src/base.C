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


/*! Gets the number of Family_t under this Base
 * \return Number of Family_t under this Base
 */
int Base_t::getNbFamily() const
{
	int n;
	int ier = cg_nfamilies( getFileID(), getID(), &n );
	check_error( "Base_t::getNbFamily", "cg_nfamilies", ier );
	return n;
}

/*! Reads a Family_t located under this Base
 * \param index   Index of the family to read [Input]
 * \param famname Name of that family [Output]
 * \param fambc   Flag indicating if there is a FamilyBC_t associated with that Family_t [Output]
 * \param ngeoref Number of Geometry references associated with that Family_t [Output]
 * \return Handle to the Family_t structure
 * \throw cgns_notfound When the requested family index does not exist
 */
Family_t Base_t::readFamily( int index, string& famname, bool& fambc, int& ngeoref ) const
{
	cgnsstring fname;
	int nfbc, ngeo;
	int ier = cg_family_read( getFileID(), getID(), ++index, fname, &nfbc, &ngeo );
	check_found( "Base_t::readFamily", "Family_t", ier );
	check_error( "Base_t::readFamily", "cg_family_read", ier );
	famname = fname;
	fambc   = (nfbc!=0);
	ngeoref = ngeo;
	return Family_t(push( "Family_t", index ), fambc, ngeo );
}

/*! Writes a Family_t under this Base
 * \param famname Name of the family [Input]
 * \return Handle to the Family_t structure
 */
Family_t Base_t::writeFamily( const string& famname )
{
	int ifam;
	int ier = cg_family_write( getFileID(), getID(), famname.c_str(), &ifam );
	check_error( "Base_t::writeFamily", "cg_family_write", ier );
	return Family_t(push("Family_t", ifam),0,0);
}

void Base_t::readSimulationType( SimulationType_t& sptr ) const
{
	SimulationType_t s;
	int ier = cg_simulation_type_read( getFileID(), getID(), &s );
	check_error( "Base_t::readSimulationtype", "cg_simulation_type_read", ier );
	sptr = s;
}

void Base_t::writeSimulationType( SimulationType_t s )
{
	int ier = cg_simulation_type_write( getFileID(), getID(), s );
	check_error( "Base_t::writeSimulationtype", "cg_simulation_type_write", ier );
}

Axisymetry_t Base_t::readAxisymmetry( vector<float>& refpt, vector<float>& axis ) const
{
	Array<float> apt(3), ax(3);	
	int ier = cg_axisym_read( getFileID(), getID(), apt, ax );
	check_error( "Base_t::readAxiSymmetry", "cg_axisym_read", ier );
	refpt = apt;
	axis  = ax;
	return Axisymetry_t(push( "Axisymmetry_t", 1 ));
}

Axisymetry_t Base_t::writeAxisymmetry( const vector<float>& refpt, const vector<float>& axis )
{
	Array<float> apt(refpt), ax(axis);
	int ier = cg_axisym_write( getFileID(), getID(), apt, ax );
	check_error( "Base_t::writeAxiSymmetry", "cg_axisym_write", ier );
	return Axisymetry_t(push( "Axisymmetry_t", 1 ));
}

Gravity_t Base_t::writeGravity( const vector<float>& gvect )
{
	Array<float> ag(gvect);
	int ier = cg_gravity_write( getFileID(), getID(), ag );
	check_error( "Base_t::writeGravity", "cg_gravity_write", ier );
	return Gravity_t(push( "Gravity_t", 1 ));
}

Gravity_t Base_t::readGravity( vector<float>& gvect )  const
{
	Array<float> agvect(gvect);
	int ier = cg_gravity_read( getFileID(), getID(), agvect );
	check_error( "Base_t::readGravity", "cg_gravity_read", ier );
	gvect = agvect;
	return Gravity_t(push( "Gravity_t", 1 ));
}

/*! Returns the number of Zone_t under the Base_t
 * \return Number of Zone_t structures
 */
int Base_t::getNbZone() const
{
	int nzones;
	int ier = cg_nzones( getFileID(), getID(), &nzones );
	check_error( "Base_t::getNbZone", "cg_nzones", ier );
	return nzones;
}

/*! Reads information about a specified Zone_t
 * \param index     Index of zone to read
 * \param zonename  Name of that zone
 * \param nodesize  Number of nodes in each direction in structured zones, total nb. of nodes for unstructured
 * \param cellsize  Number of cells in each direction in structured zones, total nb. of cells for unstructured
 * \param bndrysize Number of boundary vertices in each direction in structured zone, total nb of boundary vertices for unstructured
 * \param type      Type of zone (Structured or Unstructured)
 * \throw cgns_notfound When the requested zone index was not found under the current base
 */
Zone_t Base_t::readZone( int index, string& zonename, vector<int>& nodesize, vector<int>& cellsize, vector<int>& bndrysize, ZoneType_t& type ) const
{
	cgnsstring zname;
	int sizedata[9]; // What is the max number of dimensions???
	int ier;
	ier = cg_zone_read( getFileID(), getID(), ++index, zname, sizedata );
	check_error( "Base_t::getZone", "cg_zone_read", ier );
	ier = cg_zone_type( getFileID(), getID(), index, &type );
	check_error( "Base_t::getZone", "cg_zone_type", ier );

	// copy to user parameters
	switch( type )
	{
	case Unstructured:
		nodesize.resize(1,sizedata[0]);
		cellsize.resize(1,sizedata[1]);
		bndrysize.resize(1,sizedata[2]);
		break;
	case Structured:
		{
		int nindexdim = getCellDimension();   // don't call get_index_dimension - 'this' is not a Zone_t!
		nodesize.resize(nindexdim);
		cellsize.resize(nindexdim);
		bndrysize.resize(0);
		for ( int i=0 ; i<nindexdim ; i++ )
			{
			nodesize[i] = sizedata[i];
			cellsize[i] = sizedata[nindexdim+i];
			}
		}
		break;
	}
	zonename = zname;
	
	structure_t s = push( "Zone_t", index );
	s.set_attribute( "ZoneType", type );
	return Zone_t(s);
	
	//return Zone_t(push("Zone_t",index));
}

#if 0
// ------------------ deprecated ------------------------
Zone_t Base_t::readZone( int index, string& zonename, vector<int>& zsize, ZoneType_t& type ) const
{
	cgnsstring zname;
	Array<int> size(9); // What is the max number of dimensions???
	int ier;
	ier = cg_zone_read( getFileID(), getID(), ++index, zname, size );
	check_error( "Base_t::getZone", "cg_zone_read", ier );
	ier = cg_zone_type( getFileID(), getID(), index, &type );
	check_error( "Base_t::getZone", "cg_zone_type", ier );

	// copy to user parameters
	zonename = zname;
	int n = (type==Unstructured) ? 1 : getCellDimension();   // don't call get_index_dimension - 'this' is not a Zone_t!
	zsize.resize(3*n);
	for ( int i=0 ; i<2*n ; i++ )
		zsize[i] = size[i];
	if ( type==Structured )
	{
		// in a structured Zone_t, the number of boundary vertices is always zero and is not transmitted by the MLL
		for ( int i=0 ; i<n ; i++ )
			zsize[2*n+i] = 0;
	}
	else
	{
		zsize[2] = size[2];
	}

	return Zone_t(push("Zone_t",index));
}
#endif

/*! Writes a new Zone_t
 * \param zonename  Name of the zone
 * \param nodesize  Number of nodes in each direction in structured zones, total nb. of nodes for unstructured
 * \param cellsize  Number of cells in each direction in structured zones, total nb. of cells for unstructured
 * \param bndrysize Number of boundary vertices in each direction in structured zone, total nb of boundary vertices for unstructured
 * \param type      Type of zone (Structured or Unstructured)
 * \throw cgns_badargument When the various size vector do not make sense for the given type of zone
 */
Zone_t Base_t::writeZone( const string& zonename, const vector<int>& nodesize, const vector<int>& cellsize, const vector<int>& bndrysize, ZoneType_t type )
{
	int physdim = getPhysicalDimension(); //// HERE!!! _-_- We can't call this here file not opened for reading!!!
	int index, ier;
	if ( type == Structured )
	{
		if ( nodesize.size() != physdim || cellsize.size() != physdim || bndrysize.size() > 0 )
			throw cgns_badargument( "Base_t::writeZone", "Invalid zone sizes" );
			
		Array<int> adims(physdim*3);
		for ( int i=0 ; i<physdim ; i++ )
		{
			adims[i] = nodesize[i];
			adims[i+physdim] = cellsize[i];
			adims[i+2*physdim] = 0;
		}
		ier = cg_zone_write( getFileID(), getID(), zonename.c_str(), adims, type, &index );
	}
	else
	{
		if ( nodesize.size() != 1 || cellsize.size() != 1 || bndrysize.size() > 1 )
			throw cgns_badargument( "Base_t::writeZone", "Invalid zone sizes" );
		
		Array<int> adims(3);
		adims[0] = nodesize[0];
		adims[1] = cellsize[0];
		adims[2] = (bndrysize.size()>0) ? bndrysize[0] : 0;
		ier = cg_zone_write( getFileID(), getID(), zonename.c_str(), adims, type, &index );
	}
	check_error( "Base_t::writeZone", "cg_zone_write", ier );
	
	structure_t s = push( "Zone_t", index );
	s.set_attribute( "ZoneType", type );
	return Zone_t(s);
	//return Zone_t(push( "Zone_t", index ));
}

#if 0
// ---------------------- deprecated --------------------
Zone_t Base_t::writeZone( const string& zonename, vector<int>& dims, ZoneType_t type )
{
	int index;
	Array<int> adims(dims);
	int ier = cg_zone_write( getFileID(), getID(), zonename.c_str(), adims, type, &index );
	check_error( "Base_t::writeZone", "cg_zone_write", ier );
	return Zone_t(push( "Zone_t", index ));
}
#endif

int Base_t::getNbIntegralData() const
{
	go_here();
	int nid;
	int ier = cg_nintegrals( &nid );
	check_error( "Base_t::getNbIntegralData", "cg_nintegrals", ier );
	return nid;
}

IntegralData_t Base_t::readIntegralData( int index, string& name ) const
{
	go_here();
	cgnsstring s;
	int ier = cg_integral_read( ++index, s );
	check_error( "Base_t::readIntegralData", "cg_integral_read", ier );
	name = s;
	return IntegralData_t(push("IntegralData_t",index));
}

IntegralData_t Base_t::writeIntegralData( const string& name )
{
	go_here();
	int ier = cg_integral_write( name.c_str() );
	check_error( "Base_t::writeIntegralData", "cg_integral_write", ier );
	int nid;
	ier = cg_nintegrals( &nid );
	check_error( "Base_t::writeIntegralData", "cg_nintegrals", ier );
	return IntegralData_t(push("IntegralData_t",nid));
}

BaseIterativeData_t Base_t::readBaseIterativeData( string& name, int& nsteps ) const
{
	cgnsstring s;
	int ier = cg_biter_read( getFileID(), getID(), s, &nsteps );
	check_error( "Zone_t::readBaseIterativeData", "cg_biter_read", ier );
	name = s;
	return BaseIterativeData_t(push("BaseIterativeData_t",1));
}

BaseIterativeData_t Base_t::writeBaseIterativeData( const string& name, int nsteps )
{
	int ier = cg_biter_write( getFileID(), getID(), name.c_str(), nsteps );
	check_error( "Zone_t::writeBaseIterativeData", "cg_biter_write", ier );
	return BaseIterativeData_t(push("ZoneIterativeData_t",1));
}

}
