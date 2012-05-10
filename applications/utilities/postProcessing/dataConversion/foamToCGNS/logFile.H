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
    Data structure to manipulate a log file produced by one of the OpenFOAM
    Solvers
    
Authors 
  Robert Magnan,   Hydro-Quebec - IREQ, 2007

\*---------------------------------------------------------------------------*/

#ifndef logFile_H
#define logFile_H

#include "Istream.H"
#include "UList.H"
#include <sys/types.h>
#include <regex.h>
#include <vector>
#include <map>

namespace Foam
{

class logFile
{
private:
	enum pattern_id_t
	{
		TIME_SEPAR,
		SOLUTION_SING,
		SOLVING_FOR,
		TIME_STEP,
		EXEC_TIME,
		N_EXPR__
	};

private:
	int iter_;
	int separator_;
	std::vector<double> time_;
	std::vector<double> contCumul_;
	std::vector<double> contGlobal_;
	std::vector<double> contLocal_;
	std::vector<double> executionTime_;
	std::map<std::string,std::vector<double> > initialRes_;
	std::map<std::string,std::vector<double> > finalRes_;
	std::map<std::string,std::vector<int> >    iters_;
	
	std::map<pattern_id_t,regex_t*> expressions;
	bool regcomp_err;
	
protected:
	// callbacks
	int set_time( const std::string& );
	int skip( const std::string& );
	//int extract_variables( const std::string& );
	//int set_separator( const std::string& );
	int solving_for( const std::string& );
	//int dummy( const std::string& );
	int continuity( const std::string& );
	int exec_time( const std::string& );

	// parsing
	//void reset_sub_counters();
	void add_pattern( pattern_id_t id, const std::string& pattern );
	void parse( std::istream& stream );
	
private:
	// copy constructor and assignment are private	
	logFile( const logFile& ) {}
	void operator=( const logFile& ) {}
	
public:	
	logFile( std::istream& );
	//logFile( std::istream& );
	virtual ~logFile();
	
	const UList<label> labelList() const;
	
	int getNSteps() const { return time_.size(); }
	void writeCSV( std::ostream& o ) const;
	void getFinalRes( const std::string& scalar_name, std::vector<double>& resarray ) const;
};

} // End namespace Foam

#endif

