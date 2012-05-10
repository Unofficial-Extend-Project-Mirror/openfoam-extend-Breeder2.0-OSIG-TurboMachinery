/*---------------------------------------------------------------------------*\
 =========                   |
 \\      /   F ield          | OpenFOAM: The Open Source CFD Toolbox
  \\    /    O peration      |
   \\  /     A nd            | Copyright Hydro-Quebec - IREQ, 2008
    \\/      M anipulation   |
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
    Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

Class
    profile1DfixedValueFvPatchField

Description
    This class implements a cylindrical boundary condition field defined 
    by a 1D fixed value profile (radial ou vertical)

    This class rely on a helper class profile1DRawData for reading the profile
    values from an ASCII file

    It is possible to provide a scale factor for the field values.

    For instance, for the pressure field, this scale factor could be 1/rho, rho being the density.
    This scaling factor will be used to scale the pressure since it is p/rho that is solved by
    simpleFoam, icoFoam, etc


    Usage:
        Here is an example of a dictionary entry for the velocity values for an inlet BC of
	type profile1DfixedValue

        inlet
        {
            type             profile1DfixedValue;  // Type of the boundary condition
            fileName         "profileExample.csv"; // Name for the ASCII file containing the 1D profile
            fileFormat       "turboCSV";           // Format of the file. turboCSV is a simple CSV file format
            interpolateCoord "Z";                  // The interpolation coordinate: "R"=radial, "Z"=vertical
            fieldName        "Velocity";           // The field we want to apply the profile to :
	                                           // "Velocity", "K", "Epsilon", "Omega", "Pressure"
            fieldScaleFactor  0.001;               // Scale factor for the field. This entry is optional. 
                                                   // The default is 1.0 
        }

        NB: The 1D profile ASCII file must be located under the "constant" directory of the case.

        For each specific fieldName, we expect to find the following entry/entries in the turboCSV file header:

        Velocity : "Velocity Axial" and "Velocity Radial" and "Velocity Circumferential"
        K        : "Turbulence Kinetic Energy"
        Epsilon  : "Turbulence Eddy Dissipation"
        Omega    : "Turbulence Specific Dissipation Rate"
        Pressure : "Pressure"


        See the file profile1DRawData.C for more information about the turboCSV file format.


    Current limitations:
        - The rotation axis is forced to the Z axis 

	- The profile is interpolated, not extrapolated, so make sure to provide a data file that
          fully covers your geometry

	- For OpenFOAM 1.3, you need to define the following preprocessor constant at compile
          time: _OPENFOAM_1_3

Authors 
    Martin Beaudoin, Hydro-Quebec - IREQ
    Maryse Page,     Hydro-Quebec - IREQ
    Robert Magnan,   Hydro-Quebec - IREQ

\*---------------------------------------------------------------------------*/

#ifndef profile1DfixedValueFvPatchField_H
#define profile1DfixedValueFvPatchField_H

#include <map>
#include <vector>

#include "fixedValueFvPatchFields.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
    Class profile1DfixedValueFvPatch Declaration
    \*---------------------------------------------------------------------------*/

template<class Type>
class profile1DfixedValueFvPatchField : public fixedValueFvPatchField<Type>
{
    // Private data

    Field<Type> profile1DValue_;

    fileName    fileName_;         // Name of the csv data file. Must be ocated under /constant/
    string      fileFormat_;       // turboCSV
    string      interpolateCoord_; // R or Z
    string      fieldName_;        // Name of the field : "Velocity", "Pressure", "K", "Epsilon", "Omega"
    scalar      fieldScaleFactor_; // Scale factor. Default: 1.0

    std::vector<double> interpolateVector_; // radius for radial profile, z for meridian profile
                                           // and sorting key values for the profiles

    std::map<double, double> mappedV_axial_;   // key: interpolateVector, value : v_axial
    std::map<double, double> mappedV_radial_;  // key: interpolateVector, value : v_radial
    std::map<double, double> mappedV_circum_;  // key: interpolateVector, value : v_circum
    std::map<double, double> mappedPressure_;  // key: interpolateVector, value : pressure
    std::map<double, double> mappedTke_;       // key: interpolateVector, value : tke
    std::map<double, double> mappedEpsilon_;   // key: interpolateVector, value : epsilon
    std::map<double, double> mappedOmega_;     // key: interpolateVector, value : omega
    
public:

    //- Runtime type information
    TypeName("profile1DfixedValue");

    // Constructors

    //- Construct from patch and internal field
    profile1DfixedValueFvPatchField
    (
        const fvPatch&,
        const DimensionedField<Type, volMesh>&
    );

    //- Construct from patch, internal field and dictionary
    profile1DfixedValueFvPatchField
    (
        const fvPatch&,
        const DimensionedField<Type, volMesh>&,
        const dictionary&
    );

    //- Construct by mapping given profile1DfixedValueFvPatchField
    //  onto a new patch
    profile1DfixedValueFvPatchField
    (
        const profile1DfixedValueFvPatchField<Type>&,
        const fvPatch&,
        const DimensionedField<Type, volMesh>&,
        const fvPatchFieldMapper&
    );

    //- Construct and return a clone
    virtual tmp<fvPatchField<Type> > clone() const
        {
            return tmp<fvPatchField<Type> >
                (
                    new profile1DfixedValueFvPatchField<Type>(*this)
                );
        }

    //- Construct as copy setting internal field reference
    profile1DfixedValueFvPatchField
    (
        const profile1DfixedValueFvPatchField<Type>&,
        const DimensionedField<Type, volMesh>&
    );

    //- Construct and return a clone setting internal field reference
    virtual tmp<fvPatchField<Type> > clone(const DimensionedField<Type, volMesh>& iF) const
        {
            return tmp<fvPatchField<Type> >
                (
                    new profile1DfixedValueFvPatchField<Type>(*this, iF)
                );
        }


    // Member functions
    inline void assignVector(vector          &v_dest,  const vector v) {v_dest  = v;}	                   
    inline void assignVector(scalar          &s_dest,  const vector v) {s_dest  = pTraits<Type>::zero;}  // Dummy function for velocity vector
    inline void assignVector(tensor          &t_dest,  const vector v) {t_dest  = pTraits<Type>::zero;}  // Dummy function for velocity vector
    inline void assignVector(sphericalTensor &st_dest, const vector v) {st_dest = pTraits<Type>::zero;}  // Dummy function for velocity vector
#if !defined  _OPENFOAM_1_3
    inline void assignVector(symmTensor &st_dest, const vector v) {st_dest = pTraits<Type>::zero;}  // Dummy function for velocity vector
#endif

    // Linear interpolation 
    inline scalar interpolateInterval(scalar &valueInf, scalar &valueSup, scalar &ratio_interval) { return valueInf + (valueSup - valueInf) * ratio_interval;}

    // Access

    //- Return the profile value
    const Field<Type>& profile1DValue() const
        {
            return profile1DValue_;
        }

    //- Return reference to the profile value
    Field<Type>& profile1DValue()
        {
            return profile1DValue_;
        }

    // Mapping functions

    //- Map (and resize as needed) from self given a mapping object
    virtual void autoMap
    (
        const fvPatchFieldMapper&
    );

    //- Reverse map the given fvPatchField onto this fvPatchField
    virtual void rmap
    (
        const fvPatchField<Type>&,
        const labelList&
    );

    //- Update the coefficients associated with the patch field
    virtual void updateCoeffs();

    //- Update the profile values
    virtual void updateProfileValues();

    //- Map a list of values using a list of keys
    void mapFieldValues(
        std::vector<double>&      unsortedVal,
        std::vector<double>&      keys,
        std::map<double, double>& mappedVal) const;

    //- Write
    virtual void write(Ostream&) const;
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#ifdef NoRepository
#   include "profile1DfixedValueFvPatchField.C"
#endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
