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

#include "IOobject.H"
#include "dimensionSets.H"
#include "dimensionedScalarFwd.H"
#include "fvcSurfaceIntegrate.H"
#include "processorFvsPatchField.H"
#include "surfaceFieldsFwd.H"
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
    protected:

        const fvSolution& fvSolution_;
        const dictionary& velocityDict_; 
        Switch isOscillating_;
        const scalar tau_;

    public: 

        //- Runtime type information
        TypeName("none");

        // Declare run-time constructor selection table

            declareRunTimeSelectionTable
            (
                autoPtr,
                velocityModel,
                Mesh,
                (
                    const fvMesh& mesh 
                ),
                (mesh)
            );
            
            
        //- Return a reference to the selected viscosity model
        static autoPtr<velocityModel> New(const fvMesh& mesh);

        velocityModel(const fvMesh& mesh)
        :
            fvSolution_(mesh),
            velocityDict_(fvSolution_.subDict("velocityModel")),
            isOscillating_
            (
                velocityDict_.getOrDefault<Switch>
                (
                    "oscillation", 
                    "on"
                )
            ),
            tau_
            (
                velocityDict_.getOrDefault<scalar>
                (
                    "tau",
                    mesh.time().endTime().value())
            )
                
        {}

        virtual ~velocityModel() = default;

        Switch isOscillating() const
        {
            return isOscillating_;
        } 

        scalar oscillationFactor(scalar const time, const scalar tau) const
        {
            return Foam::cos(M_PI * time / tau);
        }

        void oscillateVelocity(
                volVectorField& U, 
                const volVectorField& U0,
                surfaceScalarField& phi,
                const surfaceScalarField& phi0, 
                const Time& runTime
        ) const
        {
            if (isOscillating())
            {
                const scalar osFactor = oscillationFactor(runTime.timeOutputValue(), tau_);
                
                phi == phi0 * osFactor;
                U == U0 * osFactor;
            }
        }

        virtual Foam::vector velocity(const vector& x) const
        {
            notImplemented("Foam::velocityModel::velocity"); 
            return vector(0,0,0);
        } 

        virtual void setVelocity(volVectorField& U) const 
        {
            const fvMesh& mesh = U.mesh();
            const auto& cellCenters = mesh.C(); 
            
            forAll(U, cellI)
            {
                U[cellI] = velocity(cellCenters[cellI]);
            }
            
            const auto& Cf = mesh.Cf();
            auto& UboundaryField = U.boundaryFieldRef(); 
            const auto& CfBoundaryField = Cf.boundaryField(); 
            const auto& meshBoundary = mesh.boundary(); 

            forAll(meshBoundary, patchI)
            {
                const auto& CfPatchField = CfBoundaryField[patchI];
                auto& UpatchField = UboundaryField[patchI]; 
                forAll(UpatchField, faceI)
                {
                    UpatchField[faceI] = velocity(CfPatchField[faceI]); 
                }
            }
        }

        virtual void setVolumetricFlux(surfaceScalarField& phi) const 
        {
            const fvMesh& mesh = phi.mesh();
            const surfaceVectorField& Sf = mesh.Sf();
            const surfaceVectorField& Cf = mesh.Cf();

            forAll(Cf, faceID)
            {
                phi[faceID] = (velocity(Cf[faceID]) & Sf[faceID]); 
            }

            const auto& CfBoundaryField = Cf.boundaryField(); 
            const auto& SfBoundaryField = Sf.boundaryField(); 
            auto& phiBoundaryField = phi.boundaryFieldRef();  
            const auto& meshBoundary = mesh.boundary(); 

            forAll(meshBoundary, patchI)
            {
                const auto& CfPatchField = CfBoundaryField[patchI];
                const auto& SfPatchField = SfBoundaryField[patchI];
                auto& phiPatchField = phiBoundaryField[patchI]; 
                forAll(phiPatchField, faceI)
                {
                    phiPatchField[faceI] = 
                    (
                        velocity(CfPatchField[faceI]) & 
                        SfPatchField[faceI]
                    ); 
                }
            }
        }
};

    
defineTypeNameAndDebug(velocityModel, 0);
defineRunTimeSelectionTable(velocityModel, Mesh);

autoPtr<velocityModel> velocityModel::New(const fvMesh& mesh)
{
    const fvSolution& fvSolution (mesh);
    const dictionary& velocityDict = fvSolution.subDict("velocityModel");
    const word modelType = velocityDict.getOrDefault<word>("type", "none");
    Info<< "Selecting advection velocity " << modelType << endl;

    auto* ctorPtr = MeshConstructorTable(modelType);

    if (!ctorPtr)
    {
        FatalIOErrorInLookup
        (
            fvSolution,
            "viscosityModel",
            modelType,
            *MeshConstructorTablePtr_
        ) << exit(FatalIOError);
    }

    return autoPtr<velocityModel>(ctorPtr(mesh));
}

class shear2D 
:
    public velocityModel
{
    public: 

        //- Runtime type information
        TypeName("shear2D");

        shear2D(const fvMesh& mesh) : velocityModel(mesh) {}; 

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
        
        deformation3D(const fvMesh& mesh) : velocityModel(mesh) {}; 

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
addToRunTimeSelectionTable(velocityModel, deformation3D, Mesh);

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

        shear3D(const fvMesh& mesh) 
        : 
            velocityModel(mesh), 
            R(velocityDict_.getOrDefault<scalar>("R", 0.5)), 
            Umax(velocityDict_.getOrDefault<scalar>("Umax", 1.)), 
            x0(velocityDict_.getOrDefault<scalar>("x0", 0.5)),
            y0(velocityDict_.getOrDefault<scalar>("y0", 0.5))
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
addToRunTimeSelectionTable(velocityModel, shear3D, Mesh);

class translation 
:
    public velocityModel
{
    vector velocity_; 

    public: 

        //- Runtime type information
        TypeName("translation");
        
        translation() = default;

        translation(const fvMesh& mesh) 
        : 
            velocityModel(mesh),
            velocity_(velocityDict_.get<vector>("velocity"))
        {};

        virtual ~translation() = default;

        virtual vector velocity(const vector& p) const
        {
            return velocity_; 
        } 
};

defineTypeNameAndDebug(translation, 0);
addToRunTimeSelectionTable(velocityModel, translation, Mesh);

class rotation 
:
    public velocityModel
{
    vector point_; 
    vector omega_;

    public: 

        //- Runtime type information
        TypeName("rotation");

        rotation(const fvMesh& mesh) 
        : 
            velocityModel(mesh),
            point_(velocityDict_.get<vector>("point")),
            omega_(velocityDict_.get<vector>("omega"))
        {}; 

        virtual ~rotation() = default;

        virtual vector velocity(const vector& p) const
        {
            return omega_ ^ (p - point_);
        } 
};

defineTypeNameAndDebug(rotation, 0);
addToRunTimeSelectionTable(velocityModel, rotation, Mesh);


class vortex2D 
:
    public velocityModel
{
    scalar v0_; 

    public: 

        //- Runtime type information
        TypeName("vortex2D");

        vortex2D(const fvMesh& mesh) 
        : 
            velocityModel(mesh),
            v0_(velocityDict_.getOrDefault<scalar>("v0", -0.2))
        {}; 

        virtual ~vortex2D() = default;

        virtual vector velocity(const vector& p) const
        {
            return v0_ * Vector(-sin(M_PI*p[0])*cos(M_PI*p[1]), cos(M_PI*p[0])*sin(M_PI*p[1]), 0.0);
        } 
};

defineTypeNameAndDebug(vortex2D, 0);
addToRunTimeSelectionTable(velocityModel, vortex2D, Mesh);

class periodic2D 
:
    public velocityModel
{
    scalar v0_; 
    scalar c1_; 
    scalar c2_; 

    public: 

        //- Runtime type information
        TypeName("periodic2D");

        periodic2D(const fvMesh& mesh) 
        : 
            velocityModel(mesh),
            v0_(velocityDict_.getOrDefault<scalar>("v0", -0.2)),
						c1_(0.1),
						c2_(-2)
        {}; 

        virtual ~periodic2D() = default;

        virtual vector velocity(const vector& p) const
        {
            return  Vector(v0_ + c1_*p[0] + c2_*p[1], -c1_*p[1], 0.0);
        } 
};

defineTypeNameAndDebug(periodic2D, 0);
addToRunTimeSelectionTable(velocityModel, periodic2D, Mesh);


/*---------------------------------------------------------------------------*\    
                    Function templates for advection 
\*---------------------------------------------------------------------------*/    

void correctFlux(surfaceScalarField& phi)
{
    const fvMesh& mesh = phi.mesh();
    const Time& runTime = phi.time();
    
    volScalarField Evol = fvc::div(phi);
    
    Info << "gMax(fvc::div(phi)) = " << gMax(Evol) << endl; 
    Info << "gAverage(fvc::div(phi)) = " << gAverage(Evol) << endl; 

    // Projection method enforcement of div(phi) = 0.
    volScalarField p
    (
        IOobject
        (
            "p",
            runTime.timeName(), 
            mesh, 
            IOobject::NO_READ, 
            IOobject::AUTO_WRITE
        ),
        mesh,
        dimensionedScalar("p", dimArea / dimTime, 0),
        "zeroGradient"
    );

    // https://en.wikipedia.org/wiki/Projection_method_(fluid_dynamics)
    // v = v_sol + v_irrot  
    // v = v_sol + \grad p / div 
    // \laplace p = \div v 
    // v_sol = v - \grad p
    fvScalarMatrix pEqn
    (
        fvm::laplacian(p) == fvc::div(phi)
    );
    pEqn.solve(); 
    phi = phi - pEqn.flux();
    Evol = fvc::div(phi);

    Info << "Projection, gMax(fvc::div(phi)) = " << gMax(Evol) << endl; 
    Info << "Projection, gAverage(fvc::div(phi)) = " << gAverage(Evol) << endl; 

    if (runTime.writeTime())
        Evol.write();

}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
