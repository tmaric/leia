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

Application
    
    leiaTestLevelSet

Description

    TODO

\*---------------------------------------------------------------------------*/

#include "fvCFD.H"
#include "phaseIndicator.H"
#include "redistancer.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

void redistancedPlaneIsUnchanged(autoPtr<redistancer>& redist, const fvMesh& mesh)
{
    // TEST: Redistancing a signed distance from a plane has no 
    //       effect if the redistancing algorithm is second-order
    //       accurate. Boundary values are calculated exactly. 
    
    // Set psi to a signed distance from a plane.
    const Time& runTime = mesh.time();
    const volVectorField& xc = mesh.C();

    dimensionedVector pTest = Foam::average(xc);  

    // TODO: extend the test with a parameterized normal orientation 

    // Zero the plane-normal component orthogonal to the 2D solution plane.
    // In the case of a 3D solution, all directions are 1.
    vector nVec (1,2.1,3);
    const auto& solutionDimensions = mesh.solutionD();
    for (int i = 0; i < 3; ++i)
    {
        if (solutionDimensions[i] < 0)
            nVec[i] = 0; 
    }
    // Normalize to obtain signed-distance
    nVec /= Foam::mag(nVec);
    dimensionedVector nTest ("nTest", dimless, nVec); 
    
    // Ignore boundary conditions for this test: signed-distances
    // at the domain boundary are prescribed!
    volScalarField psi 
    (
        IOobject
        (
            "psi", 
            runTime.timeName(), 
            mesh, 
            IOobject::NO_READ, 
            IOobject::NO_WRITE
        ),
        mesh,
        dimensionedScalar("0", dimLength, 0),
        "calculated"
    );
    volScalarField psi0("psi0", psi);

    // Initialize signed distance to a plane
    psi == (nTest & (xc - pTest)); 

    // Save the non-redistanced field for error evaluation
    psi0 == psi;

    // |\nabla \psi0|
    volScalarField magGradPsi0("magGradPsi0", Foam::mag(fvc::grad(psi0)));
    Info << "mean(|\\nabla \\psi0|) : " << 
        Foam::average(magGradPsi0).value() << endl;

    // ||\nabla \psi0| - 1|
    volScalarField eGradPsi0 = Foam::mag(
        Foam::mag(magGradPsi0) - 
        dimensionedScalar("1", dimless, 1)
    );
    eGradPsi0.rename("eGradPsi0");

    // Redistance the signed distance to a plane (nothing should happen) 
    redist->redistance(psi);

    // |\nabla \psi| 
    volScalarField magGradPsi("magGradPsi", Foam::mag(fvc::grad(psi)));
    Info << "mean(|\\nabla \\psi|) : " << 
        Foam::average(magGradPsi).value() << endl;

    volScalarField ePsi = Foam::mag(psi - psi0); 
    ePsi.rename("ePsi");

    // Valid errors available only at cell centers (internal field)
    scalar ePsiLinf = Foam::max(ePsi).value();
    if (ePsiLinf > 1e-13)
    {
        FatalErrorInFunction
            << " redistancing a planar signed distance fails: \n"
            << "Foam::max(Foam::mag(psi - psi0)) > 16*SMALL :" 
            << ePsiLinf 
            << abort(FatalError);
    }
    
    // Valid errors available only at cell centers (internal field)
    volScalarField eGradPsi = Foam::mag(
        magGradPsi - 
        dimensionedScalar("1", dimless, 1) 
    );
    eGradPsi.rename("eGradPsi");
    eGradPsi.write();

    scalar eGradPsiLinf = Foam::max(eGradPsi).value();
    Info << "eGradPsiLinf: " << eGradPsiLinf << endl;
    if (eGradPsiLinf > 1e-11)
    {
        FatalErrorInFunction
            << " redistancing a planar signed distance fails: \n"
            << "Foam::max(Foam::mag(Foam::mag(fvc::grad(psi)) - 1) > 16*SMALL) :" 
            << eGradPsiLinf 
            << abort(FatalError);
    }
    
    // TODO: Only if test fails.
    Info << "Writing fields" << endl;

    psi0.write();
    magGradPsi0.write();

    psi.write();
    magGradPsi.write();

    ePsi.write();
    eGradPsi0.write();
    eGradPsi.write();

    Info << "Done." << endl;
};

void phaseIndicatorIsBounded(autoPtr<phaseIndicator>& phaseInd, const fvMesh& mesh)
{
    const Time& runTime = mesh.time();

    // TEST: The phase-indicator is bounded [0,1] 
    volScalarField psi 
    (
        IOobject
        (
            "psi", 
            runTime.timeName(), 
            mesh, 
            IOobject::NO_READ, 
            IOobject::NO_WRITE
        ),
        mesh,
        dimensionedScalar("0", dimLength, 0),
        "calculated"
    );

    volScalarField alpha 
    (
        IOobject
        (
            "alpha", 
            runTime.timeName(), 
            mesh, 
            IOobject::NO_READ, 
            IOobject::NO_WRITE
        ),
        mesh,
        dimensionedScalar("0", dimless, 0),
        "calculated"
    );

    phaseInd->calcPhaseIndicator(alpha, psi);
}

int main(int argc, char *argv[])
{
    argList::addNote
    (
        "Level set equation solver."
    );

    #include "addCheckCaseOptions.H"
    #include "setRootCaseLists.H"
    #include "createTime.H"
    #include "createMesh.H"
    #include "createFields.H"

    redistancedPlaneIsUnchanged(redist, mesh);

    phaseIndicatorIsBounded(phaseInd, mesh);
    
    return 0;
}


// ************************************************************************* //
