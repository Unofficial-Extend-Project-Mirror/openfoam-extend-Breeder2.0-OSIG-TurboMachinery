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

Authors
  Robert Magnan,   Hydro-Quebec - IREQ, 2007

\*---------------------------------------------------------------------------*/

#ifndef CGNSBoundaryConditions_H
#define CGNSBoundaryConditions_H

#include "Time.H"
#include "wallPolyPatch.H"
#include "cellShape.H"
#include "cellModeller.H"
#include "mergePoints.H"
#include "mathematicalConstants.H"
#include "transform.H"
#include "pointFields.H"
#include "vector.H"
#include "wordList.H"
#include "faceList.H"
#include "scalar.H"

#include <string>
#include <map>
#include <vector>
#include <set>

#include "libcgnsoo3/cgnsoo.H"

#include "ConnectivityMapper.H"

inline std::string toLower( const std::string& s )
{
	// explicit cast needed to resolve ambiguity
	std::string ret(s);
	std::transform( ret.begin(), ret.end(), ret.begin(),
		(int(*)(int)) std::tolower);
	return ret;
}

//------------------------------------------------------------------------------
// CGNSBoundaryCondition
//
// This class encapsulates all the work related to identifying 
// the boundary conditions patches from CGNS data
//
// Essentially this includes:
// - going from a patch defined by point list to one defined by a face list
// - computing cyclic matching patches
// - converting between CGNS BC types and Foam BC types
// - building a Foam::polyMesh from the accumulated information
//
//------------------------------------------------------------------------------
class CGNSBoundaryConditions
{	
private: // local types
	struct patchinfo
	{
		CGNSOO::BCType_t       bctype;
		std::vector<foamNodeIndex>  pointlist; // foam indices (>=0)
		int                    nfaces;
	public:
		patchinfo( CGNSOO::BCType_t t, const std::vector<foamNodeIndex>& pl, int nf = 0 ) : 
			bctype(t), pointlist(pl), nfaces(nf)
		{}
	};
	typedef std::multimap<std::string,patchinfo> mm_bcp;

private: // private data members
	const ConnectivityMapper& mapper;
	std::set<std::string> s_bcname;
	mm_bcp bcpatches;
	std::map<std::string,CGNSOO::BCType_t> familymap;
	Foam::DynamicList<Foam::face,100000> boundary_faces;
	bool debug_;
	
	// data setup by mergePatches()
	Foam::faceListList patchFaces;
	Foam::wordList     patchNames;
	Foam::wordList     patchTypes;
	Foam::wordList     patchPhysicalTypes;

private: // private methods
	static Foam::tensor rotationTransform( 
				Foam::vector rotAxis,
				Foam::scalar rotAngle // in degrees
			);
	void pointListToFaceList( std::set<foamNodeIndex>& pointlist, Foam::faceList& l_faces );
	Foam::faceList computeMatchingCyclicFaces( 
		const Foam::faceList&   cyclicFaceListRot,
		const Foam::faceList&   cyclicFaceListRef,
		const Foam::pointField& points,
		Foam::tensor            transformation,
		Foam::scalar            cyclicToleranceFactor );
	
	void identifyBoundaryFaces();
	void identifyBoundaryFaces2();
	
public:	// public interface
	CGNSBoundaryConditions( const ConnectivityMapper& m, 
				const std::map<std::string,CGNSOO::BCType_t>& fammap,
				bool debug = false );
	
	void addBoundaryPatch(
		CGNSOO::Base_t base,
		int    indexZone,
		CGNSOO::ZoneType_t zonetype,
		CGNSOO::BC_t bc, 
		std::string bcname, 
		CGNSOO::BCType_t bctype, 
		CGNSOO::PointSetType_t psettype );
	void addPatchesFromElements( CGNSOO::BCType_t bctype  );
	void buildPatches( bool separatePatches );
	int  computeCyclicBC( 
		const std::string&       patchName1, 
		const std::string&       patchName2,
		const Foam::vector& rotAxis,
		Foam::scalar        rotAngle,
		Foam::scalar        cyclicToleranceFactor );
	Foam::polyMesh* buildFoamMesh( const Foam::Time& t ) const;
	
public: // static methods
	static Foam::word getFoamTypeName( CGNSOO::BCType_t bctype, const std::string& bcname );
	static bool       validateUserDefinedBCName( const std::string& bcname );
};

#endif
