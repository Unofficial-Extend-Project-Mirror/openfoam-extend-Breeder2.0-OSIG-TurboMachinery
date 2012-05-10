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
#include "file.H"
#include "cgnsoo.H"

namespace CGNSOO
{

file::file() : _openmode(READONLY), _fileindex(-1), _valid(false)
{
}

file::file( const string& fname, openmode_t mode ) : _filename(fname), _openmode(mode), _valid(false)
{
	open( fname, mode );
}

file::~file()
{
	if ( _valid ) close( "file::~file" );
}

void file::close( const char* caller )
{
	int ier = cg_close( _fileindex );
	check_error( caller, "cg_close", ier );
	//_basenode.erase( _basenode.begin(), _basenode.end() );
	_fileindex = 0;
	_valid = false;
}

#if 0
node* file::add_base( Base_t* n )
{
	_basenode.push_back( n );
}
#endif

void file::open( const string& fname, openmode_t rw )
{
	if ( _valid ) close( "file::open" );

	_openmode = rw;
	int m;
	switch( _openmode )
	{
	case READONLY : m = MODE_READ;   break;
	case READWRITE: m = MODE_MODIFY; break;
	case WRITE    : m = MODE_WRITE;  break;
	}
	int ier = cg_open( const_cast<char*>(fname.c_str()), m, &_fileindex );
	check_error( "file::open", "cg_open", ier );
	_valid = (ier==0);
}

double file::getVersion() const
{
	if ( !_valid ) throw std::logic_error( "file::getVersion : file non initialise" );
	float v;
	int ier = cg_version( _fileindex, &v );
	check_error( "file::getVersion", "cg_version", ier );
	return double(v);
}

int file::getNbBase() const
{
	int nbases;
	int ier = cg_nbases( _fileindex, &nbases );
	check_error( "file::getNbBase", "cg_nbases", ier );
	return nbases;
}

Base_t file::readBase( int ibase, string& name, int& celldim, int& physdim ) const
{
	cgnsstring cname;
	int ier = cg_base_read( _fileindex, ++ibase, cname, &celldim, &physdim );
	check_error( "file::readBase", "cg_base_read", ier );
	name = cname;
	return Base_t( *this, ibase, physdim );
}

Base_t file::writeBase( const string& basename, int celldim, int physdim )
{
	int baseid;
	int ier = cg_base_write( _fileindex, basename.c_str(), celldim, physdim, &baseid );
	check_error( "file::writeBase", "cg_base_write", ier );
	return Base_t( *this, baseid, physdim );
}

};
