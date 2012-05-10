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

Periodic_t GridConnectivityProperty_t::readGridConnectivityPeriodic( vector<float>& rotcenter, vector<float>& rotangle, vector<float>& translation ) const
{
	structure_t gconnec = parent();
	int igc = gconnec.getID(); // index of GridConnectivity_t or GridConnectivity1to1_t
	Array<float> acenter(3), aangle(3), atransl(3);
	bool is1to1 = gconnec.isA( "GridConnectivity1to1_t" );
	if ( is1to1 )
	{
		int ier = cg_1to1_periodic_read( getFileID(), getBase().getID(), getZone().getID(), igc, acenter, aangle, atransl );
		check_error( "GridConnectivityProperty_t::readGridConnectivityPeriodic", "cg_1to1_periodic_read", ier );
	}
	else
	{
		int ier = cg_conn_periodic_read( getFileID(), getBase().getID(), getZone().getID(), igc, acenter, aangle, atransl );
		check_error( "GridConnectivityProperty_t::readGridConnectivityPeriodic", "cg_conn_periodic_read", ier );
	}
	rotcenter   = acenter;
	rotangle    = aangle;
	translation = atransl;
	return Periodic_t(push( "GridConnectivityPeriodic_t", 1 ));
}

Periodic_t GridConnectivityProperty_t::writeGridConnectivityPeriodic( const vector<float>& rotcenter, const vector<float>& rotangle, const vector<float>& translation )
{
	structure_t gconnec = parent();
	int igc = gconnec.getID(); // index of GridConnectivity_t or GridConnectivity1to1_t
	//std::cerr << "gconnec.top_label = \"" << gconnec.getLabel() << "\", " << gconnec.getPath() << std::endl;
	Array<float> acenter(rotcenter), aangle(rotangle), atransl(translation);
	bool is1to1 = gconnec.isA( "GridConnectivity1to1_t" );
	if ( is1to1 )
	{
		int ier = cg_1to1_periodic_write( getFileID(), getBase().getID(), getZone().getID(), igc, acenter, aangle, atransl );
		check_error( "GridConnectivityProperty_t::writeGridConnectivityPeriodic", "cg_1to1_periodic_write", ier );
	}
	else
	{
		int ier = cg_conn_periodic_write( getFileID(), getBase().getID(), getZone().getID(), igc, acenter, aangle, atransl );
		check_error( "GridConnectivityProperty_t::writeGridConnectivityPeriodic", "cg_conn_periodic_write", ier );
	}
	return Periodic_t(push( "GridConnectivityPeriodic_t", 1 ));
}

Average_t GridConnectivityProperty_t::readGridConnectivityAverage( AverageInterfaceType_t& avgtype ) const
{
	structure_t gconnec = parent();
	int igc = gconnec.getID(); // index of GridConnectivity_t or GridConnectivity1to1_t
	bool is1to1 = gconnec.isA( "GridConnectivity1to1_t" );
	if ( is1to1 )
	{
		int ier = cg_1to1_average_read( getFileID(), getBase().getID(), getZone().getID(), igc, &avgtype );
		check_error( "GridConnectivityProperty_t::readGridConnectivityPeriodic", "cg_1to1_average_read", ier );
	}
	else
	{
		int ier = cg_conn_average_read( getFileID(), getBase().getID(), getZone().getID(), igc, &avgtype );
		check_error( "GridConnectivityProperty_t::readGridConnectivityPeriodic", "cg_conn_average_read", ier );
	}
	return Average_t(push( "GridConnectivityAverage_t", 1 ));
}

Average_t GridConnectivityProperty_t::writeGridConnectivityAverage( AverageInterfaceType_t avgtype )
{
	structure_t gconnec = parent();
	int igc = gconnec.getID(); // index of GridConnectivity_t or GridConnectivity1to1_t
	bool is1to1 = gconnec.isA( "GridConnectivity1to1_t" );
	if ( is1to1 )
	{
		int ier = cg_1to1_average_write( getFileID(), getBase().getID(), getZone().getID(), igc, avgtype );
		check_error( "GridConnectivityProperty_t::writeGridConnectivityPeriodic", "cg_1to1_average_write", ier );
	}
	else
	{
		int ier = cg_conn_average_write( getFileID(), getBase().getID(), getZone().getID(), igc, avgtype );
		check_error( "GridConnectivityProperty_t::writeGridConnectivityPeriodic", "cg_conn_average_write", ier );
	}
	return Average_t(push( "GridConnectivityAverage_t", 1 ));
}

}
