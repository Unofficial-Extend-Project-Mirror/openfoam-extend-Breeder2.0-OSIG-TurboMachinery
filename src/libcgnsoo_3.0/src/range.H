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
#ifndef CGNSRANGE_H
#define CGNSRANGE_H

#include <stdexcept>
#include <iostream>
#include <vector>
#include <algorithm>
using std::vector;

namespace CGNSOO
{

/*! \class range
 *   \brief Utility class to represent a CGNS index range
 */
class range : public vector<int>
{
private:
	size_t nmax;

public:
	range( size_t n=0 ) : vector<int>(2*n,0), nmax(n) {}
	// resize the range to allow n dimensions - so 2*n values are allocated
	void resize( size_t n )
	{
		nmax=n;
		vector<int>::resize(2*n,0);
	}
	void set( size_t i, size_t low, size_t high )
	{
		if ( i<nmax )
		{
			vector<int>::operator[](i) = low;
			vector<int>::operator[](i+nmax) = high;
		}
	}
	range& operator=( const vector<int>& v )
	{
		resize( v.size()/2 );
		vector<int>::operator=( v );
		return *this;
	}
	int dim() const
	{
		int ntot = 1;
		for ( size_t i=0 ; i<nmax ; i++ )
			ntot *= (high(i) - low(i) + 1);
		return ntot;
	}
	friend std::ostream& operator<<( std::ostream& o, const range& r )
	{
		o << "Begin=[" << r[0];
		for ( size_t i=1 ; i<r.nmax ; i++ )
			o << ',' << r[i];
		o << "], End=[" << r[r.nmax];
		for ( size_t i=r.nmax+1 ; i<2*r.nmax ; i++ )
			o << "," << r[i];
		o << "]";
		return o;
	}
	size_t size() const { return nmax; }
	//const int* values() const { return &vector<int>::operator[](0); }
	//operator int*() const { return &vector<int>::operator[](0); } //!< allows passing as a C array
	//operator vector<int>&() { return *this; }
	int& low ( size_t i ) { return vector<int>::operator[](i); }
	int& high( size_t i ) { return vector<int>::operator[](i+nmax); }
	int  low ( size_t i ) const { return vector<int>::operator[](i); }
	int  high( size_t i ) const { return vector<int>::operator[](i+nmax); }
	int  delta(size_t i) const { return vector<int>::operator[](i+nmax)-vector<int>::operator[](i)+1; }
	void swap( const vector<int>& transform );
};
 
}

#endif
