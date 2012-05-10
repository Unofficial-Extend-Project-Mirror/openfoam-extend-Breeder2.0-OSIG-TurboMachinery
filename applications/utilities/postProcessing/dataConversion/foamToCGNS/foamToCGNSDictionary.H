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
  Conversion of CGNS files into Foam's mesh and fields

Authors 
  Robert Magnan,   Hydro-Quebec - IREQ, 2007

\*---------------------------------------------------------------------------*/

#ifndef foamToCGNSOptions_H
#define foamToCGNSOptions_H

#include "IOdictionary.H"
#include "Switch.H"

namespace Foam
{
#define kwConversionDirectory     "ConversionDirectory"
#define kwSplitMixedCellTypes     "SplitMixedCellTypes"
#define kwWriteConvergenceHistory "WriteConvergenceHistory"
#define kwAllowUserDefinedFields  "AllowUserDefinedFields"

class foamToCGNSDictionary : public IOdictionary
{
private:
	string conversionDirectory_;
	bool   splitMixed_;
	bool   writeConvergenceHistory_;
	bool   allowUserDefinedFields_;
	
public:
	foamToCGNSDictionary( const IOobject& o ) : 
		IOdictionary(o), 
		conversionDirectory_("FileConversions"),
		splitMixed_(false),
		writeConvergenceHistory_(false),
		allowUserDefinedFields_(false)
	{
		if ( found(kwConversionDirectory) )
			lookup(kwConversionDirectory) >> conversionDirectory_;
		if ( found(kwSplitMixedCellTypes) )
			splitMixed_ = Switch(lookup(kwSplitMixedCellTypes));
		if ( found(kwWriteConvergenceHistory) )
			writeConvergenceHistory_ = Switch(lookup(kwWriteConvergenceHistory));
		if ( found(kwAllowUserDefinedFields) )
			allowUserDefinedFields_ = Switch(lookup(kwAllowUserDefinedFields));
	}
	bool splitMixed() const { return splitMixed_; }
	bool writeConvergenceHistory() const { return writeConvergenceHistory_; }
	bool allowUserDefinedFields() const { return allowUserDefinedFields_; }
	string conversionDirectory() const { return conversionDirectory_; }
};

} // end of namespace Foam

#endif
