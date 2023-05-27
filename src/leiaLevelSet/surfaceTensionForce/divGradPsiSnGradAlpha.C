/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2021 AUTHOR,AFFILIATION
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

#include "divGradPsiSnGradAlpha.H"
#include "addToRunTimeSelectionTable.H"
#include "volFields.H"
#include "surfaceFields.H"
#include "fvcGrad.H"
#include "fvcSnGrad.H"
#include "fvcDiv.H"
#include "surfaceInterpolate.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam {
using Foam::mag;

defineTypeNameAndDebug(divGradPsiSnGradAlpha, false);
addToRunTimeSelectionTable(surfaceTensionForce, divGradPsiSnGradAlpha, Mesh);

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //
divGradPsiSnGradAlpha::divGradPsiSnGradAlpha(const fvMesh& mesh)
:
    surfaceTensionForce(mesh),
    fvSolutionDict_(mesh_),
    levelSetDict_(fvSolutionDict_.subDict("levelSet")),
    surfTensionDict_(levelSetDict_.subDict("surfaceTensionForce")),
    normals_(mesh_.lookupObject<volVectorField>(surfTensionDict_.getOrDefault<word>("normals", "nc"))),
    alpha_(mesh_.lookupObject<volScalarField>(surfTensionDict_.getOrDefault<word>("alpha", "alpha.dispersed"))),
    psi_(mesh_.lookupObject<volScalarField>(surfTensionDict_.getOrDefault<word>("levelSet", "psi")))
{}

// * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * * //

tmp<surfaceScalarField> divGradPsiSnGradAlpha::faceSurfaceTensionForce() const 
{
    // Compute interface-normals using the gradient of the level set field. 
    tmp<volVectorField> nPsiTmp = fvc::grad(psi_);
    nPsiTmp->rename("nPsi");
    volVectorField& nPsi = nPsiTmp.ref();
    nPsi = nPsi / mag(nPsi);
    
    // Face-centered curvature as a linear interpolation of the trace of the gradient 
    // of the interface-normal-field. 
    return sigma_*fvc::interpolate(fvc::div(nPsi))*fvc::snGrad(alpha_);
}

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

// ************************************************************************* //
