/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | Copyright Hydro-Quebec - IREQ, 2008
     \\/     M anipulation  |
-------------------------------------------------------------------------------
  License
  This file is part of OpenFOAM.

  OpenFOAM is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version.

  OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
  for more details.

  You should have received a copy of the GNU General Public License
  along with OpenFOAM; if not, write to the Free Software Foundation,
  Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA

  Description
  Quantities known to CGNS

 Authors
  Martin Beaudoin, Hydro-Quebec - IREQ, 2005
  Robert Magnan,   Hydro-Quebec - IREQ, 2007

\*---------------------------------------------------------------------------*/

#include <string>

#include "libcgnsoo3/cgnsoo.H"

#include "dimensionSet.H"

class CGNSQuantity
{
	// Fonction utilitaire permettant de retourner un nom OpenFoam et les unites correspondentes pour un type de champs scalaire CGNS
	static bool getFoamName( CGNSOO::Quantity_t cgns_q, std::string& sf_name, Foam::dimensionSet& sf_dimensionSet )
	{
		const Foam::dimensionSet adim  ( 0,0,0 ,0,0,0,0 );
		const Foam::dimensionSet meter ( 0,1,0 ,0,0,0,0 );
		const Foam::dimensionSet kilo  ( 1,0,0 ,0,0,0,0 );
		const Foam::dimensionSet second( 0,0,1 ,0,0,0,0 );
		
		switch(cgns_q)
		{
		case CGNSOO::PRESSURE:
			sf_name         = "p" ;
			sf_dimensionSet = kilo/(meter*meter);
			break;
		case CGNSOO::TURBULENT_DISSIPATION:
			sf_name         = "epsilon" ;
			sf_dimensionSet = (meter*meter)/(second*second*second);
			//sf_dimensionSet.reset(dimensionSet(0, 2, -3, 0, 0, 0, 0));
			break;
		case CGNSOO::TURBULENT_ENERGY_KINETIC:
			sf_name         = "k" ;
			sf_dimensionSet = (meter*meter)/(second*second);
			//sf_dimensionSet.reset(dimensionSet(0, 2, -2, 0, 0, 0, 0));
			break;
		default:
			//std::cout << "  - cgnsToOpenFoamNames: type de solution non-supporte: " << cgns_sol << std::endl;
			sf_name         = "unsupported" ;
			sf_dimensionSet = adim;
			return false;
		}
		
		return true;
	}
};
