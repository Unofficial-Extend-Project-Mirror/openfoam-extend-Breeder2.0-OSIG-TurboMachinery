/*---------------------------------------------------------------------------*\
 =========                   |
 \\      /   F ield          | OpenFOAM: The Open Source CFD Toolbox
  \\    /    O peration      |
   \\  /     A nd            | Copyright Hydro-Quebec - IREQ, 2009
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
            fileName         "profileExample.csv"; // Name for the ASCII file containing the 1D profile.
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

#include "profile1DfixedValueFvPatchField.H"

#include "profile1DRawData.H"
#include "transformFvPatchField.H"
#include "dimensionedScalar.H"

#include <iostream>
#include <iterator>
#include <algorithm>
#include <functional>

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

template<class Type>
profile1DfixedValueFvPatchField<Type>::profile1DfixedValueFvPatchField
(
    const fvPatch& p,
    const DimensionedField<Type, volMesh>& iF
)
    :
    fixedValueFvPatchField<Type>(p, iF),
    profile1DValue_(p.size()),
    fileName_(""),
    fileFormat_(""),
    interpolateCoord_(""),
    fieldName_(""),
    fieldScaleFactor_(1.0)
{}


template<class Type>
profile1DfixedValueFvPatchField<Type>::profile1DfixedValueFvPatchField
(
    const profile1DfixedValueFvPatchField<Type>& ptf,
    const fvPatch& p,
    const DimensionedField<Type, volMesh>& iF,
    const fvPatchFieldMapper& mapper
)
    :
    fixedValueFvPatchField<Type>(p, iF),
    profile1DValue_(ptf.profile1DValue_, mapper),
    fileName_(ptf.fileName_),
    fileFormat_(ptf.fileFormat_),
    interpolateCoord_(ptf.interpolateCoord_),
    fieldName_(ptf.fieldName_),
    fieldScaleFactor_(ptf.fieldScaleFactor_),
    //
    interpolateVector_(ptf.interpolateVector_),
    mappedV_axial_(ptf.mappedV_axial_),
    mappedV_radial_(ptf.mappedV_radial_),
    mappedV_circum_(ptf.mappedV_circum_),
    mappedPressure_(ptf.mappedPressure_),
    mappedTke_(ptf.mappedTke_),
    mappedEpsilon_(ptf.mappedEpsilon_),
    mappedOmega_(ptf.mappedOmega_)
{
    fvPatchField<Type>::operator==(profile1DValue_);
}


template<class Type>
profile1DfixedValueFvPatchField<Type>::profile1DfixedValueFvPatchField
(
    const fvPatch& p,
    const DimensionedField<Type, volMesh>& iF,
    const dictionary& dict
)
    :
    fixedValueFvPatchField<Type>(p, iF),
    profile1DValue_(p.size(), pTraits<Type>::zero),  
    fileName_(dict.lookup("fileName")),
    fileFormat_(dict.lookup("fileFormat")),
    interpolateCoord_(dict.lookup("interpolateCoord")),
    fieldName_(dict.lookup("fieldName")),
    fieldScaleFactor_(dict.found("fieldScaleFactor") ? readScalar(dict.lookup("fieldScaleFactor")) : 1.0) // Optional, default value = 1.0 
    
{
    // This constructor is called once by the solver, at the beginning.
    // This constructor is called by paraFoam at every time step.
    if( debug )
    {
        Pout << "profile1DfixedValueFvPatchField<Type>::profile1DfixedValueFvPatchField(const fvPatch& p, const DimensionedField<Type, volMesh>& iF, const dictionary& dict) " << endl;
        Pout << "profile1D Dict: " << dict << endl;
        Pout << "profile1D Field: " << fieldName_
             << " - interpolateCoord : " << interpolateCoord_ << " : scaling factor: " << fieldScaleFactor_ << " : time: " << this->db().time().timeName() << endl;
    }
	
    // Recover enum for interpolateCoord
    profile1DType type_profile = string_to_profile1DType(interpolateCoord_);

    if(type_profile == BAD_PROFILE)
    {
        FatalErrorIn
            (
                "profile1DfixedValueFvPatchField<Type>::profile1DfixedValueFvPatchField(const fvPatch& p, const DimensionedField<Type, volMesh>& iF, const dictionary& dict)"
            )   << "interpolateCoord not recognized: " << interpolateCoord_ << abort(FatalError);
    }

    // Recover enum for fieldName
    profile1DField type_field = string_to_profile1DField(fieldName_);
	
    if(type_field == BAD_FIELD)
    {
        FatalErrorIn
            (
                "profile1DfixedValueFvPatchField<Type>::profile1DfixedValueFvPatchField(const fvPatch& p, const DimensionedField<Type, volMesh>& iF, const dictionary& dict)"
            )   << "fieldName not recognized: " << fieldName_ << abort(FatalError);
    }

    string pathFilename;

    if(this->db().time().processorCase())
    {
        pathFilename = this->db().rootPath()/this->db().caseName()/".."/this->db().time().constant()/fileName_;
    }
    else
    {
        pathFilename = this->db().rootPath()/this->db().caseName()/this->db().time().constant()/fileName_;
    }

    // Read the turboCSV file under the "constant"directory
    profile1DRawData turboCSV_profile(
        pathFilename, 
        fileFormat_);

    if(!turboCSV_profile.is_valid())
    {
        FatalErrorIn
            (
                "profile1DfixedValueFvPatchField<Type>::profile1DfixedValueFvPatchField(const fvPatch& p, const DimensionedField<Type, volMesh>& iF, const dictionary& dict)"
            )   << "problem with file: " << fileName_ << " : format : " << fileFormat_ << abort(FatalError);
    }

    // Grab the interpolateVector_  with validation of the key value
    // Profile type: radial or meridian profile
    // Unsorted
    switch(type_profile)
    {
        case R:
            turboCSV_profile.get_r(interpolateVector_);
            break;	
        case Z:
            turboCSV_profile.get_z(interpolateVector_);
            break;
        default:
            break;
    }

    // Do just a quick validation in order to detect problems early
    if(interpolateVector_.size() < 2)
    {
        FatalErrorIn (
            "profile1DfixedValueFvPatchField<Type>::profile1DfixedValueFvPatchField(const fvPatch& p, const DimensionedField<Type, volMesh>& iF, const dictionary& dict)"
        )
            << "   not enough values to interpolate with. " << endl
		<< "    Please validate the data values in the turboCSV file: " << fileName_
		<< abort(FatalError);
    }

    // Next, grab the desired field
    switch(type_field)
    {
        case VELOCITY_X:  
        case VELOCITY_Y:  
        case VELOCITY_Z:  
        case VELOCITY  :
            {
                std::vector<double> v_axial;
                std::vector<double> v_radial;
                std::vector<double> v_circum;

                // Read unsorted values
                turboCSV_profile.get_velocityAxial(v_axial);
                turboCSV_profile.get_velocityRadial(v_radial);
                turboCSV_profile.get_velocityCircumferential(v_circum);

                // map values according to interpolateVector
                mapFieldValues(v_axial,  interpolateVector_, mappedV_axial_);
                mapFieldValues(v_radial, interpolateVector_, mappedV_radial_);
                mapFieldValues(v_circum, interpolateVector_, mappedV_circum_);
            }
            break;
		
        case PRESSURE:
            {
                std::vector<double> pressure;

                // Read unsorted values
                turboCSV_profile.get_pressure(pressure);

                // map values according to interpolateVector
                mapFieldValues(pressure,  interpolateVector_, mappedPressure_);
            }

            break;
		
        case K:
            {
                std::vector<double> tke;

                // Read unsorted values
                turboCSV_profile.get_tke(tke);

                // map values according to interpolateVector
                mapFieldValues(tke,  interpolateVector_, mappedTke_);
            }
            break;
		
        case EPSILON:
            {
                std::vector<double> epsilon;

                // Read unsorted values
                turboCSV_profile.get_epsilon(epsilon);

                // map values according to interpolateVector
                mapFieldValues(epsilon,  interpolateVector_, mappedEpsilon_);
            }
            break;
		
        case OMEGA:
            {
                std::vector<double> omega;

                // Read unsorted values
                turboCSV_profile.get_omega(omega);

                // map values according to interpolateVector
                mapFieldValues(omega,  interpolateVector_, mappedOmega_);
            }
            break;
		
        default:
            break;
    }
			
    // Now that the maps are initialized, we can sort the interpolateVector_ values in ascending order for interpolation;
    // We will use those values as keys in order to retrieve the profiles values from the different maps.
    std::sort(interpolateVector_.begin(), interpolateVector_.end());

    if(debug)
    {
        std::cout << "interpolation vector: " << (type_profile == R ? profile1DRawData::KEY_R : profile1DRawData::KEY_Z) << " : ";
        std::copy(interpolateVector_.begin(), interpolateVector_.end(), std::ostream_iterator<double>(std::cout, " "));
        std::cout << std::endl;
        std::cout << "interpolateVector_.begin(): " << *interpolateVector_.begin() << std::endl;
        std::cout << "interpolateVector_.end(): " <<   *(--interpolateVector_.end()) << std::endl;
        std::cout << std::endl;
    }

    // Update the profile value for the patch
    this->updateProfileValues();

    this->updateCoeffs();
}


template<class Type>
profile1DfixedValueFvPatchField<Type>::profile1DfixedValueFvPatchField
(
    const profile1DfixedValueFvPatchField<Type>& ptf,
    const DimensionedField<Type, volMesh>& iF
)   :
    fixedValueFvPatchField<Type>(ptf, iF),
    profile1DValue_(ptf.profile1DValue_),
    fileName_(ptf.fileName_),
    fileFormat_(ptf.fileFormat_),
    interpolateCoord_(ptf.interpolateCoord_),
    fieldName_(ptf.fieldName_),
    fieldScaleFactor_(ptf.fieldScaleFactor_),
    //
    interpolateVector_(ptf.interpolateVector_),
    mappedV_axial_(ptf.mappedV_axial_),
    mappedV_radial_(ptf.mappedV_radial_),
    mappedV_circum_(ptf.mappedV_circum_),
    mappedPressure_(ptf.mappedPressure_),
    mappedTke_(ptf.mappedTke_),
    mappedEpsilon_(ptf.mappedEpsilon_),
    mappedOmega_(ptf.mappedOmega_)
{
    if(debug)
        Pout << "profile1DfixedValueFvPatchField<Type>::profile1DfixedValueFvPatchField(const profile1DfixedValueFvPatchField<Type>& ptf, const DimensionedField<Type, volMesh>& iF)" << endl;
	
    fvPatchField<Type>::operator==(profile1DValue_);
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

// Map from self
template<class Type>
void profile1DfixedValueFvPatchField<Type>::autoMap
(
    const fvPatchFieldMapper& m
)
{
    Field<Type>::autoMap(m);
    profile1DValue_.autoMap(m);
}

template<class Type>
void profile1DfixedValueFvPatchField<Type>::rmap
(
    const fvPatchField<Type>& ptf,
    const labelList& addr
)
{
    fixedValueFvPatchField<Type>::rmap(ptf, addr);

    const profile1DfixedValueFvPatchField<Type>& tiptf =
        refCast<const profile1DfixedValueFvPatchField<Type> >(ptf);

    profile1DValue_.rmap(tiptf.profile1DValue_, addr);
}


// Update the coefficients associated with the patch field
template<class Type>
void profile1DfixedValueFvPatchField<Type>::updateCoeffs()
{
    if (this->updated())
    {
        return;
    }

    if(this->dimensionedInternalField().mesh().changing())
        updateProfileValues();

    fvPatchField<Type>::operator==(profile1DValue_);
    
    fixedValueFvPatchField<Type>::updateCoeffs();
}

// Update the profile values
template<class Type>
void profile1DfixedValueFvPatchField<Type>::updateProfileValues()
{
    // Recover enum for interpolateCoord
    profile1DType type_profile = string_to_profile1DType(interpolateCoord_);

    // Recover enum for fieldName
    profile1DField type_field = string_to_profile1DField(fieldName_);
	
    // Grab the min and max value of interpolateValue in order to do check the bounds before trying to interpolate
    scalar min_interpolateVector_ = *interpolateVector_.begin();
    scalar max_interpolateVector_ = *(--interpolateVector_.end());


    // Face centers
#if defined _OPENFOAM_1_3
    const vectorField::subField& faceCentres = fvPatchField<Type>::patch().Cf();
#else
    const vectorField& faceCentres = fvPatchField<Type>::patch().Cf();
#endif
	
    // Geometrical center of the path:
    // The rotation axis is Z
    // The radius is computed in the X-Y plane
    // 
    vector origCentre = pTraits<vector>::zero;
    vector vector_deltaCentre = pTraits<vector>::zero;

    typedef std::vector<double>::iterator double_iterator;
    forAll(fvPatchField<Type>::patch(), faceI)
    {
        // Trace
        if( debug > 5 )
            Pout << "profile1DfixedValueFvPatchField: Face: " << faceI << endl;

        // Compute the radius (distance between the center of the face and Z axis, in the X-Y plane
        vector_deltaCentre = faceCentres[faceI] - origCentre;
        vector_deltaCentre.z() = 0.0;
        scalar radius(mag(vector_deltaCentre));

        // The interpolated value depends on type of profile (interpolateCoord: R or Z)
        scalar interpolateValue = (type_profile == Z) ? 
            faceCentres[faceI].z()   // interpolateCoord: Z
            :
            radius;                  // interpolateCoord: R


        // Verify if the interpolation value is within the bounds of the interpolateVector_
        if(interpolateValue < min_interpolateVector_ || interpolateValue > max_interpolateVector_ )
        {
            // We don't support any kind of extrapolation, so this is a fatal error.
            FatalErrorIn (
                "profile1DfixedValueFvPatchField<Type>::profile1DfixedValueFvPatchField(const fvPatch& p, const DimensionedField<Type, volMesh>& iF, const dictionary& dict)"
            )
                << "interpolateValue outside of interpolateVector_ bounds: " << endl
                    << "      interpolateValue: " << interpolateValue
                    << " : min(interpolateVector_) : " << min_interpolateVector_
                    << " : max(interpolateVector_) : " << max_interpolateVector_
                    << abort(FatalError);
        }

        // Find the inferior and superior bounds for the interpolation interval
        std::vector<double>::iterator upperBound, lowerBound;
        upperBound = lowerBound = std::upper_bound(interpolateVector_.begin(), interpolateVector_.end(), interpolateValue);
        lowerBound--;

        if( debug > 5 )
        {
            Pout << "profile1DfixedValueFvPatchField: vector_deltaCentre: " << vector_deltaCentre;
            Pout << "    : magnitude : " << interpolateValue << endl; 
            Pout << "profile1DfixedValueFvPatchField: Bound search for : " << interpolateValue 
                << " in the interval [ " 
                << *lowerBound << " , "
                << *upperBound << " ]" << endl;
        }

        // ratio for linear interpolation
        double delta_interval = *upperBound - *lowerBound;

        // We check here for a division by zero. We prefer to abort the solver than to continue with the computation
        if(fabs(delta_interval) < VSMALL)
        {
            FatalErrorIn (
                "profile1DfixedValueFvPatchField<Type>::profile1DfixedValueFvPatchField(const fvPatch& p, const DimensionedField<Type, volMesh>& iF, const dictionary& dict)"
            )
                << "   Catching a division by zero attempt: " << endl
                    << "    Please validate the data values in the turboCSV file: " << fileName_
                    << abort(FatalError);
        }

        // From here, we can safely compute the ratio_interval factor for linear interplation
        double ratio_interval = (interpolateValue - *lowerBound)/ delta_interval;
		
        switch(type_field)
        {
            case VELOCITY_X:
            case VELOCITY_Y:  
            case VELOCITY_Z:
            case VELOCITY  :
                {
                    // linear interpolation: interpol_velocityI = v_I[b_inf] + (v_I[b_sup] - v_I[b_inf]) * ratio_interval;
                    double interpol_velocityAxial           = interpolateInterval( mappedV_axial_[*lowerBound],  mappedV_axial_[*upperBound],  ratio_interval );
                    double interpol_velocityRadial          = interpolateInterval( mappedV_radial_[*lowerBound], mappedV_radial_[*upperBound], ratio_interval );
                    double interpol_velocityCircumferential = interpolateInterval( mappedV_circum_[*lowerBound], mappedV_circum_[*upperBound], ratio_interval );
                    vector Vcyl( interpol_velocityRadial, interpol_velocityCircumferential, interpol_velocityAxial );
			
                    // Conversion from cylindrical velocity to cartesian velocity
                    //
                    // V_axial  =  Vz;
                    // V_radial =  Vx * cos_teta + Vy * sin_teta; 
                    // V_teta   = -Vx * sin_teta + Vy * cos_teta;
                    //
                    // cos_teta = x / R;
                    // sin_teta = y / R;
                    //
                    // Vx = V_radial * cos_teta - V_teta * sin_teta = V_radial * x/R - V_teta * y/R;
                    // Vy = V_radial * sin_teta + V_teta * cos_teta = V_radial * y/R + V_teta * x/R;
                    // Vz = V_axial;
                    //
                    //double cos_teta = vector_deltaCentre[0] / radius;  // x / R
                    //double sin_teta = vector_deltaCentre[1] / radius;  // y / R
                    vector X(1,0,0);
                    vector R = vector_deltaCentre; R[2] = 0;
                    R /= mag(R) + VSMALL;
                    tensor rot = rotationTensor( X, R );
                    if ( debug > 5 )
                    {
                        Pout << "profile1DfixedValueFvPatchField: radius   : " << radius << endl;
                        Pout << "profile1DfixedValueFvPatchField: cos_teta : " << R[0] << endl;
                        Pout << "profile1DfixedValueFvPatchField: sin_teta : " << R[1] << endl;
                    }
                    vector Vcart = transform( rot, Vcyl );

                    switch(type_field)
                    {
                        case VELOCITY_X:
                            profile1DValue_[faceI] = pTraits<Type>::one * Vcart[0];
                            break;
				
                        case VELOCITY_Y:
                            profile1DValue_[faceI] = pTraits<Type>::one * Vcart[1];
                            break;

                        case VELOCITY_Z:
                            profile1DValue_[faceI] = pTraits<Type>::one * Vcart[2];
                            break;

                        case VELOCITY:
                            assignVector(profile1DValue_[faceI], Vcart);
                            break;

                        default:
                            break;
                    }

                }
                break;
            case PRESSURE:
                {
                    scalar interpol_pressure = interpolateInterval(mappedPressure_[*lowerBound], mappedPressure_[*upperBound], ratio_interval);
                    profile1DValue_[faceI] = pTraits<Type>::one * interpol_pressure;
                }
                break;
            case K:
                {
                    scalar interpol_k = interpolateInterval(mappedTke_[*lowerBound], mappedTke_[*upperBound], ratio_interval);
                    profile1DValue_[faceI] = pTraits<Type>::one * interpol_k;
                }
                break;
            case EPSILON:
                {
                    scalar interpol_epsilon = interpolateInterval(mappedEpsilon_[*lowerBound], mappedEpsilon_[*upperBound], ratio_interval);
                    profile1DValue_[faceI] = pTraits<Type>::one * interpol_epsilon;
                }
                break;
            case OMEGA:
                {
                    scalar interpol_omega = interpolateInterval(mappedOmega_[*lowerBound], mappedOmega_[*upperBound], ratio_interval);
                    profile1DValue_[faceI] = pTraits<Type>::one * interpol_omega;
                }
                break;
            default  : 
                break;
        }

        // Scale if needed
        if(fieldScaleFactor_ != 1.0)
            profile1DValue_[faceI] *= fieldScaleFactor_;
    }
}


// Map a list of values using a list of keys
template<class Type>
void profile1DfixedValueFvPatchField<Type>::mapFieldValues(
    std::vector<double>&      unsortedVal,
    std::vector<double>&      keys,
    std::map<double, double>& mappedVal) const
{
    // map values with keys using a std::map
    int i = 0;
    forAllIter(std::vector<double> , keys, keysValue)
        mappedVal[*keysValue] = unsortedVal[i++];
}


// Write
template<class Type>
void profile1DfixedValueFvPatchField<Type>::write(Ostream& os) const
{
    fvPatchField<Type>::write(os);
 
    os.writeKeyword("fileName")
        << fileName_ << token::END_STATEMENT << nl;

    os.writeKeyword("fileFormat")
        << fileFormat_ << token::END_STATEMENT << nl;

    os.writeKeyword("interpolateCoord")
        << interpolateCoord_ << token::END_STATEMENT << nl;

    os.writeKeyword("fieldName")
        << fieldName_ << token::END_STATEMENT << nl;
		
    os.writeKeyword("fieldScaleFactor")
        << fieldScaleFactor_ << token::END_STATEMENT << nl;
		
    // To output the values at the patch face centers
    this->writeEntry("value",os);
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// ************************************************************************* //
