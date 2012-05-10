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
#include <vector>
#include <cassert>

#include "range.H"

namespace CGNSOO
{
//------------------------------------------------
// range
//------------------------------------------------

void range::swap( const std::vector<int>& transform )
{
	// computes the inverse transformation vector
	int n = transform.size();
	assert( n==nmax );

	std::vector<int> invert(n);
	for ( int i=0 ; i<n ; i++ )
	{
		int index = abs(transform[i]);
		int sign  = transform[i]/index;
		invert[index-1] = sign;
	}
    
	// flip min/max if required
	for ( int i=0 ; i<n ; i++ )
	{
		if ( invert[i] < 0 )
		{
			int tmp = low(i);
			low(i) = high(i);
			high(i) = tmp;
		}
	}
}

}
