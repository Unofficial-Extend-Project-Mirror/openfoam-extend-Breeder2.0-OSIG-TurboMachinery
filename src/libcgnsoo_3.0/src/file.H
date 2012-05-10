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
#ifndef CGNSFILE_H
#define CGNSFILE_H

#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
using std::ostream;
using std::endl;
using std::string;
using std::vector;

#include "cgnsmll.H"
#include "checkerror.H"

namespace CGNSOO
{
class node;
class Base_t;

/*! \class file
 *  \brief Handle to the actual file containing a CGNS database
 *
 */
class file
{
public:
	/*! \enum openmode_t
	 *  \brief Various access modes for a CGNS database
	 */
	enum openmode_t { READONLY, WRITE, READWRITE };

private:
	string        _filename;  //!< name of this file as it was opened
	openmode_t    _openmode;  //!< mode it was opened in
	//vector<Base_t*> _basenode;  //!< pointers to individual Base_t node
	int           _fileindex; //!< CGNS index of this file
        bool          _valid;     //!< indicates if this data is meaningful or not

private:
        void     close( const char* ); //!< closes the file and invalidates the data
	//node*    add_base( Base_t* n );  //!< adds a new Base_t node to the data structure (not to the CGNS database)

public:
	file(); //!< default constructor
	file( const string& fn, openmode_t mode ); //!< creates a file by opening it (bases are not read)
        ~file();

	int        getNbBase() const; //!< get the number of bases stored in this file
	Base_t     readBase( int ibase, string& name, int& celldim, int& physdim ) const; //!< reads a Base_t structure
	Base_t     writeBase( const string& basename, int celldim, int physdim ); //!< writes a Base_t structure

        void       open( const string& fname, openmode_t rw ); //!< opens a new filename using the current object
	double     getVersion() const;                         //!< gets the CGNS version stored in the database
	int        getFileId() const { return _fileindex; }    //!< gets the CGNS id associated with this file
	openmode_t getMode() const { return _openmode; }       //!< gets the mode in which this file was opened
	operator   bool() const { return _valid; }             //!< checks if this file is correctly opened
};

};

#endif
