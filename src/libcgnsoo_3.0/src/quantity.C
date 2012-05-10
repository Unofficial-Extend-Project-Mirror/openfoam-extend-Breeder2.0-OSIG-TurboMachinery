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
#include <string>
using std::string;

#include "quantity.H"

namespace CGNSOO
{

struct quantity_enum_conversion
{
	Quantity_t  q;
	const char* s;
};

static quantity_enum_conversion conversionTable[] =
{
#include "enum_table.H"
};

// Conversion string-->enum
Quantity_t QuantityStringToEnum( const string& s )
{
	const int n = sizeof(conversionTable)/sizeof(quantity_enum_conversion);
	for ( int i=0 ; i<n ; i++ )
		if ( s == conversionTable[i].s ) return Quantity_t(i);
	return USER_DATA;
}

// Conversion enum-->string
string QuantityEnumToString( Quantity_t q )
{
	const int n = sizeof(conversionTable)/sizeof(quantity_enum_conversion);
	for ( int i=0 ; i<n ; i++ )
		if ( conversionTable[i].q == q ) return string(conversionTable[i].s);
	return string(conversionTable[USER_DATA].s);
}

}
