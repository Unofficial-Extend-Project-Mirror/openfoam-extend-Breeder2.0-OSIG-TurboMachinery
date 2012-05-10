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
#ifndef DIMENSIONAL_EXPONENTS_H
#define DIMENSIONAL_EXPONENTS_H

#include "Array.H"
#include <map>
using std::map;

namespace CGNSOO
{

/*!
 * \class DimensionalExponents
 * This is a utility class to simplify the specification of unit exponents
 */
class DimensionalExponents
{
public:
	enum exponentType_t { Mass=0, Length, Time, Temperature, Angle }; //!< The various unit types for which exponents can be specified
	
private:
	map<exponentType_t,double> exponents;
	
public:
	DimensionalExponents() {}
	
	double operator[]( exponentType_t e ) const 
	{ 
		map<exponentType_t,double>::const_iterator i=exponents.find(e);
		return (i==exponents.end()) ? 0 : (*i).second;
	}
	double& operator[]( exponentType_t e )
	{ 
		if (exponents.find(e)==exponents.end()) exponents[e]=0;
		return exponents[e];
	}
	
	//! Converts the DimensionalExponents into an array to be passed on to the MLL
	operator Array<double>() const
	{
		Array<double> e(5,0.0);
		map<exponentType_t,double>::const_iterator i;
		for ( int k=0 ; k<5 ; k++ )
			if ( (i=exponents.find(static_cast<exponentType_t>(k))) != exponents.end() ) e[k] = (*i).second;
		return e;
	}
};

}

#endif
