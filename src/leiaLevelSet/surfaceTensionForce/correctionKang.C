/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2023 AUTHOR,AFFILIATION
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

\*---------------------------------------------------------------------------*/

#include "correctionKang.H"
#include "addToRunTimeSelectionTable.H"
#include "fvCFD.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
    defineTypeNameAndDebug(correctionKang, false);
    addToRunTimeSelectionTable(surfaceTensionForce, correctionKang, Mesh);
}


// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //

Foam::tmp<Foam::surfaceScalarField> Foam::correctionKang::interpolate
(
    const volScalarField& kappa, 
    const volScalarField& psi 
) const 
{
    const fvMesh& mesh = psi.mesh();
    const labelUList & own = mesh.owner();  
    const labelUList & nei = mesh.neighbour(); 

    tmp<surfaceScalarField> tkappaf
        (
            new surfaceScalarField
            (
                IOobject
                (
                    word(),
                    fileName(),
                    mesh,
                    IOobject::NO_READ,
                    IOobject::NO_WRITE,
                    false
                ),
                mesh,
                dimensionedScalar(psi.dimensions()/dimLength/dimLength, 0.0)
            )
        );
    surfaceScalarField kappaf = tkappaf.ref();

    forAll(nei, faceI)
    {
        const label o = own[faceI];
        const label n = nei[faceI];
        kappaf[faceI] = (kappa[o]*psi[n] + kappa[n]*psi[0])/(psi[n] + psi[o]);
    }

    return tkappaf;

}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::correctionKang::correctionKang(const fvMesh& mesh)
:
    surfaceTensionForce(mesh),
    fvSolutionDict_(mesh_),
    levelSetDict_(fvSolutionDict_.subDict("levelSet")),
    surfTensionDict_(levelSetDict_.subDict("surfaceTensionForce")),
    alpha_(mesh_.lookupObject<volScalarField>
        (
            surfTensionDict_.getOrDefault<word>("alpha", "alpha.dispersed")
        )),
    psi_(mesh_.lookupObject<volScalarField>
        (
            surfTensionDict_.getOrDefault<word>("levelSet", "psi")
        ))
{}


// * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * * //

Foam::tmp<surfaceScalarField> 
Foam::correctionKang::faceSurfaceTensionForce() const 
{
    // Compute interface-normals using the gradient of the level set field. 
    tmp<volVectorField> nPsiTmp = fvc::grad(psi_);
    nPsiTmp->rename("nPsi");
    volVectorField& nPsi = nPsiTmp.ref();
    nPsi = nPsi / mag(nPsi);
    Info << nPsi.name() << endl;
    
    tmp<volScalarField> tkappa = fvc::div(nPsi);
    const volScalarField kappa = tkappa.cref();

    tmp<surfaceScalarField> tkappaf = this->interpolate(kappa, psi_);
    const surfaceScalarField kappaf = tkappaf.cref(); 

    return sigma_*kappaf*fvc::snGrad(alpha_);
}



// ************************************************************************* //
