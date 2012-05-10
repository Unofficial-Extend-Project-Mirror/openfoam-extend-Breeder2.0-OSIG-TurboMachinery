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

GoverningEquations_t FlowEquationSet_t::writeGoverningEquations( GoverningEquationsType_t t )
{
	go_here();
	int ier = cg_governing_write( t );
	check_error( "FlowEquationSet_t::writeGoverningEquations", "cg_governing_write", ier );
	return GoverningEquations_t(push( "GoverningEquations_t", 1 ));
}

TurbulenceModel_t FlowEquationSet_t::writeTurbulenceModel( ModelType_t t )
{
	go_here();
	int ier = cg_model_write( "TurbulenceModel_t", t );
	check_error( "FlowEquationSet_t::writeTurbulenceModel", "cg_model_write", ier );
	return TurbulenceModel_t(push( "TurbulenceModel_t", 1 ));
}

TurbulenceClosure_t FlowEquationSet_t::writeTurbulenceClosure( ModelType_t t )
{
	go_here();
	int ier = cg_model_write( "TurbulenceClosure_t", t );
	check_error( "FlowEquationSet_t::writeTurbulenceClosure", "cg_model_write", ier );
	return TurbulenceClosure_t(push( "TurbulenceClosure_t", 1 ));
}

ThermalConductivityModel_t FlowEquationSet_t::writeThermalConductivityModel( ModelType_t t )
{
	go_here();
	int ier = cg_model_write( "ThermalConductivityModel_t", t );
	check_error( "FlowEquationSet_t::writeThermalConductivityModel", "cg_model_write", ier );
	return ThermalConductivityModel_t(push( "ThermalConductivityModel_t", 1 ));
}

ThermalRelaxationModel_t FlowEquationSet_t::writeThermalRelaxationModel( ModelType_t t )
{
	go_here();
	int ier = cg_model_write( "ThermalRelaxation_t", t );
	check_error( "FlowEquationSet_t::writeThermalRelaxationModel", "cg_model_write", ier );
	return ThermalRelaxationModel_t(push( "ThermalRelaxationModel_t", 1 ));
}

ViscosityModel_t FlowEquationSet_t::writeViscosityModel( ModelType_t t )
{
	go_here();
	int ier = cg_model_write( "ViscosityModel_t", t );
	check_error( "FlowEquationSet_t::writeViscosityModel", "cg_model_write", ier );
	return ViscosityModel_t(push( "ViscosityModel_t", 1 ));
}

GasModel_t FlowEquationSet_t::writeGasModel( ModelType_t t )
{
	go_here();
	int ier = cg_model_write( "GasModel_t", t );
	check_error( "FlowEquationSet_t::writeGasModel", "cg_model_write", ier );
	return GasModel_t(push( "GasModel_t", 1 ));
}

void TurbulenceModel_t::readDiffusion( vector<bool>& diff_flags ) const
{
	go_here();
	int d[6];
	int ier = cg_diffusion_read( d );
	check_error( "model::readDiffusion", "cg_diffusion_read", ier );
	diff_flags.resize( 6 );
	vector<bool>::iterator p=diff_flags.begin();
	for ( int i=0 ; i<6 ; i++, p++ )
		*p = (d[i]!=0);
}

void TurbulenceModel_t::writeDiffusion( const vector<bool>& diff_flags )
{
	go_here();
	int d[6];
	vector<bool>::const_iterator p=diff_flags.begin();
	for ( int i=0 ; i<6 ; i++, p++ )
		d[i] = (*p) ? 1 : 0;
	int ier = cg_diffusion_write( d );
	check_error( "model::writeDiffusion", "cg_diffusion_write", ier );
}

}
