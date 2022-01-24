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
        
        velocityModel(const dictionary&) {};

        virtual ~velocityModel() = default;

        virtual Foam::vector velocity(Foam::vector x) const
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

        shear2D(const dictionary& dict) {}; 

        virtual ~shear2D() = default;

        virtual vector velocity(vector x) const
        {
            return vector(
                Foam::sin(2*M_PI*x[1])*Foam::sqr(Foam::sin(M_PI * x[0])),  
                -Foam::sin(2*M_PI*x[0])*Foam::sqr(Foam::sin(M_PI * x[1])),  
                0
            );
        } 
};

class deformation2D 
:
    public velocityModel
{
    public: 

        //- Runtime type information
        TypeName("deformation3D");
        
        deformation2D() = default;

        deformation2D(const dictionary& dict) {}; 

        virtual ~deformation2D() = default;

        virtual vector velocity(vector p) const
        {
            const scalar x = p[0];
            const scalar y = p[1];
            const scalar z = p[2];
            return vector(
                2*sin(2*M_PI*y)*pow(sin(M_PI*x),2)*sin(2*M_PI*z),
                -sin(2*M_PI*x)*pow(sin(M_PI*y),2)*sin(2*M_PI*z),
                -sin(2*M_PI*x)*sin(2*M_PI*y)*pow(sin(M_PI*z),2)               
            );
        } 
};

defineTypeNameAndDebug(deformation2D, 0);
addToRunTimeSelectionTable(velocityModel, deformation2D, dictionary);

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

    forAll (Cf, faceID)
    {
        phi[faceID] = 
            velocityModelPtr->velocity(Cf[faceID]) & Sf[faceID];
    }

    const auto& CfBoundaryField = Cf.boundaryField(); 
    const auto& SfBoundaryField = Sf.boundaryField(); 
    auto& phiBoundaryField = phi.boundaryFieldRef();  
    const auto& meshBoundary = mesh.boundary(); 

    forAll(meshBoundary, patchI)
    {
        const fvPatch& patch = meshBoundary[patchI]; 
        if (isA<processorFvPatch>(patch))
        {
            const auto& CfPatchField = CfBoundaryField[patchI];
            const auto& SfPatchField = SfBoundaryField[patchI];
            auto& phiPatchField = phiBoundaryField[patchI]; 
            forAll(phiPatchField, faceI)
            {
                phiPatchField[faceI] = 
                    velocityModelPtr->velocity(CfPatchField[faceI]) & SfPatchField[faceI];
            }
        }
    }
}

void setVelocity(
    volVectorField& U,
    autoPtr<velocityModel>& velocityModelPtr
)
{
    const fvMesh& mesh = U.mesh();
    const volVectorField& C = mesh.C();

    forAll (C, cellID)
    {
        U[cellID] = velocityModelPtr->velocity(C[cellID]); 
    }
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
