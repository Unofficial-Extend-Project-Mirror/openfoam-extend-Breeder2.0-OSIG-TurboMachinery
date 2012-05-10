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
#include <cassert>
#include <vector>
#include <algorithm>

using std::vector;
using std::copy;

/*!
 * \class Array
 * \brief Deals with conversion from std::vector to standard C arrays
 *
 * Many CGNS functions require the use of standard C arrays to specify
 * a list of values. However in C++, using a std::vector is more common. 
 * The methods in cgnsoo expect vectors as arguments and the Array
 * class is use to make the conversion to and from C arrays.
 */
#if 1
template <class T>
class Array
{
private:
	size_t nelem;
	T*     data;
	
public:
	Array() : nelem(0), data( NULL ) {}
	Array( int n ) : nelem(n), data( new T[nelem] ) {}
	Array( int n, T v ) : nelem(n), data( new T[nelem] ) 
	{
		for ( int i=0 ; i<nelem ; i++ )
			data[i] = v;
	}
	Array( const Array& a ) : nelem(a.nelem), data( new T[nelem] )
	{
		std::copy( a.data, a.data+nelem, data );
	}
	Array( const vector<T>& v ) : nelem(v.size()), data( new T[nelem] )
	{
		std::copy( v.begin(), v.end(), data );
	}
	~Array() { delete [] data; }
	
	Array& operator=( const Array& rhs )
	{
		if ( nelem != rhs.nelem ) 
		{ 
			delete [] data;
			nelem = rhs.nelem;
			data = (nelem) ? new T[nelem] : NULL;
		}
		std::copy( rhs.data, rhs.data+nelem, data );
		return *this;
	}
	Array& operator=( const vector<T>& rhs )  // also works with a 'range'
	{
		if ( nelem != rhs.size() ) 
		{ 
			delete [] data;
			nelem = rhs.size();
			data = (nelem) ? new T[nelem] : NULL;
		}
		std::copy( rhs.begin(), rhs.end(), data );
		return *this;
	}
	T& operator[]( int i )
	{
		assert( i>=0 && i<nelem );
		return data[i];
	}
	const T& operator[]( int i ) const
	{
		assert( i>=0 && i<nelem );
		return data[i];
	}
#if 1
	// With this method, assigning an Array to a vector causes a useless double-copy of the data
	operator vector<T>() const
	{
		vector<T> v(nelem);
		std::copy( data, data+nelem, v.begin() );
		return v;
	}
#else
	// It would be nice to have a 'friend operator=( vector<T>&, const Array<T>& )'
	// but C++ does not allow to have non-member (),[],=,-> operators
	// This is a workaround but requires extensive changes to the cgnsoo code
	// (everywhere an Array is assigned to a vector).
	void assignTo( vector<T>& v );
	{
		v.resize(nelem);
		std::copy( data, data+nelem, v.begin() );
	}
#endif	
	size_t size() const { return nelem; }
	operator const T*() const
	{
		return data;
	}
	operator T*()
	{
		return data;
	}
};
#else

// This version takes advantage of the fact that most STL implements a vector class as a C array
// Not quite as portable but slightly more efficient!

template <class T>
class Array : public vector<T>
{
public:
	Array() : vector<T>() {}
	Array( int n ) : vector<T>(n) {}
	Array( const Array& a ) : vector<T>( vector<T>(*this) ) {}
	Array( const vector<T>& v ) : vector<T>( v ) {}
	~Array() {}
	
	Array& operator=( const Array& rhs )     { vector<T>::operator=( vector<T>( rhs ) ); }
	Array& operator=( const vector<T>& rhs ) { vector<T>::operator=( rhs ); }
	T& operator[]( int i ) { return vector<T>::operator[](i); }
	const T& operator[]( int i ) const { return vector<T>::operator[](i); }
	operator vector<T>() const { return vector<T>(*this); }
	operator const T*() const { return &vector<T>::operator[](0); }
	operator T*() { return &vector<T>::operator[](0); }
};
#endif
