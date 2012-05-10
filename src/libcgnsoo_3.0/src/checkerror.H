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
#ifndef CGNS_CHECK_ERROR_H
#define CGNS_CHECK_ERROR_H

#include <string>
#include <stdexcept>

using std::string;
using std::runtime_error;
using std::logic_error;

#include "cgnslib.h"

namespace CGNSOO
{

/*! \exception cgns_exception
 *  \brief This exception is thrown when a cgns error code is returned when calling an MLL function
 */
class cgns_exception : public std::runtime_error
{
public:
	cgns_exception( const string& method, const string& function ) : runtime_error( "CGNS error in " + method + " while calling " + function + "\n" + cg_get_error() )  {}
	virtual ~cgns_exception() throw();
};

/*! \exception cgns_uninitialized
 *  \brief This exception is thrown when a cgns error code is returned when calling an MLL function
 */
class cgns_uninitialized : public std::runtime_error
{
public:
	cgns_uninitialized() : std::runtime_error( "Trying to use an uninitialized CGNS structure (one which was declared with the default constructor)" )  {}
	virtual ~cgns_uninitialized() throw();
};

/*! \exception cgns_mismatch
 *  \brief This exception is thrown when a method is invoqued on a node which is not of the appropriate type
 *  This exception class is historic, it should now be considered an internal error
 */
class cgns_mismatch : public std::logic_error
{
public:
	cgns_mismatch( const string& method, const string& cause ) : logic_error( "CGNS error in " + method + "\n" + cause )  {}
	virtual ~cgns_mismatch() throw();
};

/*! \exception cgns_badargument
 *  \brief This exception is thrown when a method is invoqued with invalid argument values
 */
class cgns_badargument : public std::logic_error
{
public:
	cgns_badargument( const string& method, const string& cause ) : logic_error( "CGNS error in " + method + "\n" + cause )  {}
	virtual ~cgns_badargument() throw();
};

/*! \exception cgns_wrongnode
 *  \brief This exception is thrown when a method is invoqued on a node which is not of the appropriate type
 *  This is an internal error
 */
class cgns_wrongnode : public std::logic_error
{
public:
	cgns_wrongnode( const string& method, const string& expecting, const string& have ) : logic_error( "Error in " + method + "\nWe expect to be in a " + expecting + " but the current node is a " + have )  {}
	virtual ~cgns_wrongnode() throw();
};

/*! \exception cgns_notfound
 *  \brief This exception is thrown when trying to read an optional node which was not found
 */
class cgns_notfound : public std::logic_error
{
public:
	cgns_notfound( const string& method, const string& expecting ) : logic_error( "Error in " + method + "\nTrying to read a " + expecting + " but it was not found" )  {}
	virtual ~cgns_notfound() throw();
};

}

#define check_found(method,looking_for,errorcode) \
if ( errorcode==CG_NODE_NOT_FOUND ) throw cgns_notfound(method,looking_for);

#define check_error(method,fonction,errorcode) \
if ( errorcode!=CG_OK ) throw cgns_exception(method,fonction);

#endif
