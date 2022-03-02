/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2021 Tomislav Maric, TU Darmstadt 
-------------------------------------------------------------------------------
License
    This file is part of OpenFOAM.

    OpenFOAM is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM.  If not, see <http://www.gnu.org/licenses/>.

Description
    Velocity models for validating advection 

\*---------------------------------------------------------------------------*/

#ifndef advectionVerification_H 
#define advectionVerification_H 

#include "processorFvsPatchField.H"
#include "volFields.H"
#include "surfaceFields.H"
#include "runTimeSelectionTables.H"
#include "addToRunTimeSelectionTable.H"
#include "error.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //    

namespace Foam    
{    

/*---------------------------------------------------------------------------*\    
                    Advection (prescribed) velocity models
\*---------------------------------------------------------------------------*/    

class velocityModel
{
    Switch isOscillating_; 

    public: 
        
        //- Runtime type information
        TypeName("none");

        // Declare run-time constructor selection table

            declareRunTimeSelectionTable
            (
                autoPtr,
                velocityModel,
                dictionary,
                (
                    const dictionary& dict
                ),
                (dict)
            );
            
            
        //- Return a reference to the selected viscosity model
        static autoPtr<velocityModel> New
        (
            const word& name, 
            const dictionary& dict
        );

        velocityModel() = default;
        
        velocityModel(const dictionary& dict)
        :
            isOscillating_(dict.getOrDefault<Switch>("oscillation", "on"))
        {} 

        Switch isOscillating() const
        {
            return isOscillating_;
        } 

        virtual ~velocityModel() = default;

        virtual Foam::vector velocity(const vector& x) const
        {
            notImplemented("Foam::velocityModel::velocity"); 
            return vector(0,0,0);
        } 
};
    
defineTypeNameAndDebug(velocityModel, 0);
defineRunTimeSelectionTable(velocityModel, dictionary);

autoPtr<velocityModel> velocityModel::New
(
    const word& modelType, 
    const dictionary& dict
)
{
    Info<< "Selecting advection velocity " << modelType << endl;

    auto* ctorPtr = dictionaryConstructorTable(modelType);

    if (!ctorPtr)
    {
        FatalIOErrorInLookup
        (
            dict,
            "viscosityModel",
            modelType,
            *dictionaryConstructorTablePtr_
        ) << exit(FatalIOError);
    }

    return autoPtr<velocityModel>(ctorPtr(dict));
}

class shear2D 
:
    public velocityModel
{
    public: 

        //- Runtime type information
        TypeName("shear2D");
        
        shear2D() = default;

        shear2D(const dictionary& dict) : velocityModel(dict) {}; 

        virtual ~shear2D() = default;

        virtual vector velocity(const vector& p) const
        {
            const scalar& x = p[0];
            const scalar& y = p[1];

            return vector(
                Foam::sin(2*M_PI*y)*Foam::sqr(Foam::sin(M_PI * x)),  
                -Foam::sin(2*M_PI*x)*Foam::sqr(Foam::sin(M_PI * y)),  
                0
            );
        } 
};

class deformation3D 
:
    public velocityModel
{
    public: 

        //- Runtime type information
        TypeName("deformation3D");
        
        deformation3D() = default;

        deformation3D(const dictionary& dict) : velocityModel(dict) {}; 

        virtual ~deformation3D() = default;

        virtual vector velocity(const vector& p) const
        {
            const scalar& x = p[0];
            const scalar& y = p[1];
            const scalar& z = p[2];
            return vector(
                2*sin(2*M_PI*y)*pow(sin(M_PI*x),2)*sin(2*M_PI*z),
                -sin(2*M_PI*x)*pow(sin(M_PI*y),2)*sin(2*M_PI*z),
                -sin(2*M_PI*x)*sin(2*M_PI*y)*pow(sin(M_PI*z),2)               
            );
        } 
};

defineTypeNameAndDebug(deformation3D, 0);
addToRunTimeSelectionTable(velocityModel, deformation3D, dictionary);

class shear3D 
:
    public velocityModel
{
    //Liovic, P., Rudman, M., Liow, J. L., Lakehal, D., & Kothe, D. (2006). A 3D
    //unsplit-advection volume tracking algorithm with planarity-preserving
    //interface reconstruction. Computers and Fluids, 35(10), 1011–1032.
    //https://doi.org/10.1016/j.compfluid.2005.09.003

    scalar R, Umax, x0, y0; 

    public: 

        //- Runtime type information
        TypeName("shear3D");
        
        shear3D() = default;

        shear3D(const dictionary& dict) 
        : 
            velocityModel(dict), 
            R(dict.getOrDefault<scalar>("R", 0.5)), 
            Umax(dict.getOrDefault<scalar>("Umax", 1.)), 
            x0(dict.getOrDefault<scalar>("x0", 0.5)),
            y0(dict.getOrDefault<scalar>("y0", 0.5))
        {}; 

        virtual ~shear3D() = default;

        virtual vector velocity(const vector& p) const
        {
            const scalar& x = p[0];
            const scalar& y = p[1];

            const scalar r = Foam::sqrt(
                Foam::sqr(x - x0) + 
                Foam::sqr(y - y0) 
            );

            return vector(
                Foam::sin(2*M_PI*y)*Foam::sqr(Foam::sin(M_PI * x)),  
                -Foam::sin(2*M_PI*x)*Foam::sqr(Foam::sin(M_PI * y)),  
                Umax * Foam::sqr(1 - (r / R))
            );
        } 
};

defineTypeNameAndDebug(shear3D, 0);
addToRunTimeSelectionTable(velocityModel, shear3D, dictionary);

class rotation 
:
    public velocityModel
{
    vector point_; 
    vector omega_;

    public: 

        //- Runtime type information
        TypeName("rotation");
        
        rotation() = default;

        rotation(const dictionary& dict) 
        : 
            velocityModel(dict),
            point_(dict.get<vector>("point")),
            omega_(dict.get<vector>("omega"))
        {}; 

        virtual ~rotation() = default;

        virtual vector velocity(const vector& p) const
        {
            return omega_ ^ (p - point_);
        } 
};

defineTypeNameAndDebug(rotation, 0);
addToRunTimeSelectionTable(velocityModel, rotation, dictionary);

/*---------------------------------------------------------------------------*\    
                    Function templates for advection 
\*---------------------------------------------------------------------------*/    

void setVolumetricFlux(
    surfaceScalarField& phi,
    const autoPtr<velocityModel>& velocityModelPtr
)
{
    const fvMesh& mesh = phi.mesh();
    const surfaceVectorField& Sf = mesh.Sf();
    const surfaceVectorField& Cf = mesh.Cf();

    const auto& runTime = mesh.time(); 

    scalar cosFactor = 1;
    if (velocityModelPtr->isOscillating())
    {
        cosFactor = Foam::cos(M_PI * runTime.timeOutputValue() / 
            runTime.endTime().value()
        );
    }

    forAll (Cf, faceID)
    {
        phi[faceID] = 
            (velocityModelPtr->velocity(Cf[faceID]) & Sf[faceID]) * cosFactor;
    }

    const auto& CfBoundaryField = Cf.boundaryField(); 
    const auto& SfBoundaryField = Sf.boundaryField(); 
    auto& phiBoundaryField = phi.boundaryFieldRef();  
    const auto& meshBoundary = mesh.boundary(); 

    forAll(meshBoundary, patchI)
    {
        // TODO: Check, overrides all patch values currently. TM
        /*const fvPatch& patch = meshBoundary[patchI]; */
        /*if (isA<processorFvPatch>(patch))*/
        /*{*/
            const auto& CfPatchField = CfBoundaryField[patchI];
            const auto& SfPatchField = SfBoundaryField[patchI];
            auto& phiPatchField = phiBoundaryField[patchI]; 
            forAll(phiPatchField, faceI)
            {
                phiPatchField[faceI] = 
                (
                    velocityModelPtr->velocity(CfPatchField[faceI]) & 
                    SfPatchField[faceI]
                ) * cosFactor;
            }
        /*}*/
    }
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
