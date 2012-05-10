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
  Robert Magnan, Hydro-Quebec - IREQ, 2007

\*---------------------------------------------------------------------------*/

#include "logFile.H"
#include <sstream>
#include <cassert>

namespace Foam
{

static std::string extract( const std::string& inLine, const std::string& token )
{
	size_t a = inLine.find( token );
	size_t b = token.length();
	std::string s = inLine.substr( a+b );
	std::string ret;
	for ( string::iterator i=s.begin() ; i!=s.end() ; i++ )
	{
		if ( (*i)!=',' && (*i)!=':' )
			ret.insert( ret.end(), 1, *i );
	}
	return ret;
}
/*
void logFile::reset_sub_counters()
{
	// this table should be member data of logFile
	const char* vars[] = {
		"Ux",
		"Uy",
		"Uz",
		"epsilon",
		"k",
		"p"
	};
	const int nvars = sizeof(vars)/sizeof(vars[0]);
	
	separator_ = 0;
	//time_ = 0;
	//contCumul_  = 0;
	//contGlobal_ = 0;
	//contLocal_  = 0;
	for ( int i=0 ; i<nvars ; i++ )
	{
		const string v( vars[i] );
		count_[v] = 0;
		finalRes_[v] = 0;
		iters_[v] = 0;
	}
}

int logFile::set_time( const std::string& s )
{
	iter_++;
	reset_sub_counters();
}
*/	
int logFile::set_time( const std::string& s )
{
	double rval;
	std::istringstream iss( extract( s, "Time = " ) );
	iss >> rval;
	time_.push_back( rval );
	return 0;
}

int logFile::skip( const std::string& s )
{
	return 1;
}

int logFile::solving_for( const std::string& s )
{
	int ival;
	double rval;
	
	std::string varname = extract( s, "Solving for " );
	
	std::istringstream iss1( extract( s, "Initial residual = " ) );
	iss1 >> rval;
	initialRes_[varname].push_back( rval );
	
	std::istringstream iss2( extract( s, "Final residual = " ) );
	iss2 >> rval;
	finalRes_[varname].push_back( rval );
	
	std::istringstream iss3( extract( s, "No iterations = " ) );
	iss3 >> ival;
	iters_[varname].push_back( ival );
	
	return 0;
}

int logFile::continuity( const std::string& s )
{
	double rval;
	
	std::istringstream iss1( extract( s, "cumulative=" ) );
	iss1 >> rval;
	contCumul_.push_back( rval );
	
	std::istringstream iss2( extract( s, "global=" ) );
	iss2 >> rval;
	contGlobal_.push_back( rval );
	
	std::istringstream iss3( extract( s, "local=" ) );
	iss3 >> rval;
	contLocal_.push_back( rval );
	
	return 0;
}

int logFile::exec_time( const std::string& s )
{
	double rval;
	std::istringstream iss1( extract( s, "ExecutionTime = " ) );
	iss1 >> rval;
	executionTime_.push_back( rval );
	
	return 0;
}

void logFile::add_pattern( pattern_id_t id, const std::string& pattern )
{
	// define a set of regex patterns that will be used to detect specific
	// structures in the logfile and invoque custom actions.
	const int comp_flags = REG_EXTENDED | REG_NOSUB;
	//const int nexpr = sizeof(expressions)/sizeof(expressions[0]);
	
	// loop to compile the patterns into a usable form by regexec
	regex_t* regbuf = new regex_t;
	regcomp_err = ::regcomp( regbuf, pattern.c_str(), comp_flags );
	if ( regcomp_err )
	{ 
		char errbuf[128];
		regerror( regcomp_err, 
			  regbuf, 
			  errbuf, sizeof(errbuf) );
		cerr << "Internal error while compiling pattern '" 
		     << pattern
		     << "' in logfile analysis\n";
		delete regbuf;
	}
	else
	{
		expressions[id] = regbuf;
	}
}

void logFile::parse( std::istream& stream )
{	
	// Static table containing the program to be executed for each line
	// Each entry defines a pattern to match and a corresponding action
	// to execute. Instructions are executed in sequence.
	const struct
	{
		pattern_id_t id;
		int (logFile::*action)( const std::string& );
	} instructions[] =
	{ 
		{ TIME_SEPAR   , &logFile::set_time },
		{ SOLUTION_SING, &logFile::skip },
		{ SOLVING_FOR  , &logFile::solving_for },
		//{ TIME_SEPAR   , &logFile::set_separator },
		//{ TIME_SEPAR   , &logFile::dummy },
		{ TIME_STEP    , &logFile::continuity },
		{ EXEC_TIME    , &logFile::exec_time }
	};
	const int ninstructions = sizeof(instructions)/sizeof(instructions[0]);
	
	// execute the program for each line
	std::string inLine;
	while( std::getline(stream,inLine) )
	{
		for ( int i=0 ; i<ninstructions ; i++ )
		{
			const int eflags = 0;
			//regmatch_t rm;
			regmatch_t pmatch[1];
			if ( ::regexec( expressions[instructions[i].id], 
				        inLine.c_str(), 
				        1, 
				        pmatch, 
				        eflags ) == 0 )
			{
				if ( (this->*instructions[i].action)(inLine) ) break;
			}
		}
	}
}

logFile::~logFile()
{	
	// cleanup memory allocated by regex compiler
	for ( std::map<pattern_id_t,regex_t*>::iterator i=expressions.begin() ; 
	      i!=expressions.end() ; 
	      i++ )
	{
		::regfree( (*i).second );
		delete (*i).second;
	}
}

logFile::logFile( std::istream& s ) : regcomp_err(false)
{
	add_pattern( TIME_SEPAR   , "^[ \t]*Time = " );
	add_pattern( SOLUTION_SING, "solution singularity" );
	add_pattern( SOLVING_FOR  , "solving for" );
	add_pattern( TIME_STEP    , "time step continuity errors :" );
	add_pattern( EXEC_TIME    , "ExecutionTime" );
	
	parse( s );
}
	
const UList<label> logFile::labelList() const
{
	return UList<label>();
}

void logFile::getFinalRes( const std::string& scalar_name, std::vector<double>& resarray ) const
{
	//resarray = finalRes_[scalar_name];
}

void logFile::writeCSV( std::ostream& o ) const
{
	const char separ = '\t';
	
	o << "Time" << separ
	  << "ExecutionTime" << separ
	  << "" << separ
	  << "" << separ
	  << "" << separ
	  << endl;

	int n = time_.size();
	for ( int i=0 ; i<n ; i++ )
	{
		o << time_[i] << separ
		  << executionTime_[i] << separ;
		for ( std::map<std::string,std::vector<double> >::const_iterator iir=initialRes_.begin() ;
		      iir!=initialRes_.end();
		      iir++ )
		{
			o << (*iir).second[i] << separ;
		}
		for ( std::map<std::string,std::vector<double> >::const_iterator iir=finalRes_.begin() ;
		      iir!=finalRes_.end();
		      iir++ )
		{
			o << (*iir).second[i] << separ;
		}
		for ( std::map<std::string,std::vector<int> >::const_iterator iir=iters_.begin() ;
		      iir!=iters_.end();
		      iir++ )
		{
			o << (*iir).second[i] << separ;
		}
		o << endl;
	}
}

} // End namespace Foam
