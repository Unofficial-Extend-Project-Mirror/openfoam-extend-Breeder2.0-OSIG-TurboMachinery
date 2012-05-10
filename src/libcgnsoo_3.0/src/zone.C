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

int Zone_t::getNbGridCoordinates() const
{
	int ngrids;
	int ier = cg_ngrids( getFileID(), getBase().getID(), getID(), &ngrids );
	check_error( "Zone_t::getNbGrid", "cg_ngrids", ier );
	return ngrids;
}

int Zone_t::getNbFlowSolution() const
{
	int nsol;
	int ier = cg_nsols( getFileID(), getBase().getID(), getID(), &nsol );
	check_error( "Zone_t::getNbFlowSolution", "cg_nsol", ier );
	return nsol;
}

#if 0
ZoneType_t Zone_t::getZoneType() const
{
	ZoneType_t zonetype;
	int ier = cg_zone_type( getFileID(), getBase().getID(), getID(), &zonetype );
	check_error( "Zone_t::getZoneType", "cg_zone_type", ier );
	return zonetype;
}
#endif

GridCoordinates_t Zone_t::readGridCoordinates( int index, string& coordname )  const
{
	cgnsstring cname;
	int ier = cg_grid_read( getFileID(), getBase().getID(), getID(), ++index, cname );
	check_error( "Zone_t::readGridCoordinatesInfo", "cg_grid_read", ier );
	coordname = cname;
	return GridCoordinates_t(push( "GridCoordinates_t", index ));
}

GridCoordinates_t Zone_t::writeGridCoordinates( const string& gcname )
{
	int gcid;
	int ier = cg_grid_write( getFileID(), getBase().getID(), getID(), gcname.c_str(), &gcid );
	//std::cout << "Writing a gridCoordinates : gcname = '" << gcname << "', gcid = " << gcid << endl;
	check_error( "Zone_t::writeGridCoordinates", "cg_grid_write", ier );
	return GridCoordinates_t(push( "GridCoordinates_t", gcid ));
}

int Zone_t::getNbElements() const
{
	int nsec;
	int ier = cg_nsections( getFileID(), getBase().getID(), getID(), &nsec );
	check_error( "Zone_t::getNbElements", "cg_nsections", ier );
	return nsec;
}

Elements_t Zone_t::readElements( int index, string& sectionname, ElementType_t& etype, int& start, int& end, int& nbndry, bool& parent ) const
{
	int parentflag;
	cgnsstring sn;
	int ier = cg_section_read( getFileID(), getBase().getID(), getID(), ++index, sn, &etype, &start, &end, &nbndry, &parentflag );
	check_error( "Zone_t::readElements", "cg_section_read", ier );
	parent = (parentflag!=0);
	sectionname = sn;
	return Elements_t(push( "Elements_t", index ));	
}

Elements_t Zone_t::writeElements( const string& sectionname, ElementType_t etype, int start, int end, int nbndry, const vector<int>& connectivity )
{	
	int element_size = end-start+1;
	if ( etype != MIXED )
	{
		int nnpe;   // number of nodes per element
		int ier = cg_npe( etype, &nnpe );
		check_error( "Zone_t::writeElements", "cg_npe", ier );
		if ( connectivity.size() != element_size*nnpe )
			throw cgns_mismatch( "Zone_t::writeElements", "The connectivity vector length is incorrect" );
	}
	else if ( etype == MIXED )
	{
		// do we have something to watch for?
	}
	int isect;
	Array<int> aconnec(connectivity);
	int ier = cg_section_write( getFileID(), getBase().getID(), getID(), sectionname.c_str(), etype, start, end, nbndry, aconnec, &isect );
	check_error( "Zone_t::writeElements", "cg_section_write", ier );
	return Elements_t(push( "Elements_t", isect ));
}

ZoneBC_t Zone_t::writeZoneBC()
{
	return ZoneBC_t(push("ZoneBC_t",1));
}

ZoneBC_t Zone_t::readZoneBC() const
{
	return ZoneBC_t(push("ZoneBC_t",1));
}

FlowSolution_t Zone_t::readFlowSolution( int index, string& solname, GridLocation_t& type) const
{
	index++;
	
	cgnsstring s;	
	int ier = cg_sol_info( getFileID(), getBase().getID(), getID(), index, s, &type );
	check_error( "Zone_t::readFlowSolution", "cg_sol_info", ier );
	solname = s;
	return FlowSolution_t(push( "FlowSolution_t", index ));
}

FlowSolution_t Zone_t::writeFlowSolution( const string& quantity, GridLocation_t gloc )
{
	int fsid;
	int ier = cg_sol_write( getFileID(), getBase().getID(), getID(), quantity.c_str(), gloc, &fsid );
	check_error( "Zone_t::writeFlowSolution", "cg_sol_write", ier );
	return FlowSolution_t(push( "FlowSolution_t", fsid ));
}

ZoneGridConnectivity_t Zone_t::writeZoneGridConnectivity()
{
	return ZoneGridConnectivity_t(push( "ZoneGridConnectivity_t", 1 ));
}

ZoneGridConnectivity_t Zone_t::readZoneGridConnectivity() const
{
	return ZoneGridConnectivity_t(push( "ZoneGridConnectivity_t", 1 ));
}

int Zone_t::getNbDiscreteData() const
{
	go_here();
	int ndd;
	int ier = cg_ndiscrete( getFileID(), getBase().getID(), getID(), &ndd );
	check_error( "Zone_t::getNbDiscreteData", "cg_ndiscrete", ier );
	return ndd;
}

DiscreteData_t Zone_t::readDiscreteData( int index, string& name ) const
{
	cgnsstring s;
	int ier = cg_discrete_read( getFileID(), getBase().getID(), getID(), ++index, s );
	check_error( "Zone_t::readDiscreteData", "cg_discrete_read", ier );
	name = s;
	return DiscreteData_t(push("DiscreteData_t",index));
}

DiscreteData_t Zone_t::writeDiscreteData( const string& name )
{
	int index;
	int ier = cg_discrete_write( getFileID(), getBase().getID(), getID(), name.c_str(), &index );
	check_error( "Zone_t::writeIntegralData", "cg_integral_write", ier );
	int nid;
	return DiscreteData_t(push("DiscreteData_t",index));
}

int Zone_t::getNbRigidGridMotion() const
{
	int nrgm;
	int ier = cg_n_rigid_motions( getFileID(), getBase().getID(), getID(), &nrgm );
	check_error( "Zone_t::getNbRigidGridMotion", "cg_n_rigid_motions", ier );
	return nrgm;
}

RigidGridMotion_t Zone_t::readRigidGridMotion( int index, string& name, RigidGridMotionType_t& rgmt ) const
{
	cgnsstring s;
	int ier = cg_rigid_motion_read( getFileID(), getBase().getID(), getID(), ++index, s, &rgmt );
	check_error( "Zone_t::readRigidGridMotion", "cg_rigid_motion_read", ier );
	name = s;
	return DiscreteData_t(push("DiscreteData_t",index));
}

RigidGridMotion_t Zone_t::writeRigidGridMotion( const string& name, RigidGridMotionType_t rgmt )
{
	int index;
	cgnsstring s;
	int ier = cg_rigid_motion_write( getFileID(), getBase().getID(), getID(), name.c_str(), rgmt, &index );
	check_error( "Zone_t::writeRigidGridMotion", "cg_rigid_motion_write", ier );
	return DiscreteData_t(push("DiscreteData_t",index));
}

int Zone_t::getNbArbitraryGridMotion() const
{
	int nrgm;
	int ier = cg_n_arbitrary_motions( getFileID(), getBase().getID(), getID(), &nrgm );
	check_error( "Zone_t::getNbArbitraryGridMotion", "cg_n_arbitrary_motions", ier );
	return nrgm;
}

ArbitraryGridMotion_t Zone_t::readArbitraryGridMotion( int index, string& name, ArbitraryGridMotionType_t& rgmt ) const
{
	cgnsstring s;
	int ier = cg_arbitrary_motion_read( getFileID(), getBase().getID(), getID(), ++index, s, &rgmt );
	check_error( "Zone_t::readArbitraryGridMotion", "cg_arbitrary_motion_read", ier );
	name = s;
	return DiscreteData_t(push("DiscreteData_t",index));
}

ArbitraryGridMotion_t Zone_t::writeArbitraryGridMotion( const string& name, ArbitraryGridMotionType_t rgmt )
{
	int index;
	cgnsstring s;
	int ier = cg_arbitrary_motion_write( getFileID(), getBase().getID(), getID(), name.c_str(), rgmt, &index );
	check_error( "Zone_t::writeArbitraryGridMotion", "cg_arbitrary_motion_write", ier );
	return DiscreteData_t(push("DiscreteData_t",index));
}

ZoneIterativeData_t Zone_t::readZoneIterativeData( string& name ) const
{
	cgnsstring s;
	int ier = cg_ziter_read( getFileID(), getBase().getID(), getID(), s );
	check_error( "Zone_t::readZoneIterativeData", "cg_ziter_read", ier );
	name = s;
	return ZoneIterativeData_t(push("ZoneIterativeData_t",1));
}

ZoneIterativeData_t Zone_t::writeZoneIterativeData( const string& name )
{
	int ier = cg_ziter_write( getFileID(), getBase().getID(), getID(), name.c_str() );
	check_error( "Zone_t::writeZoneIterativeData", "cg_ziter_write", ier );
	return ZoneIterativeData_t(push("ZoneIterativeData_t",1));
}

}
