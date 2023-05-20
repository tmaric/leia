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

namespace Foam
{

defineTypeNameAndDebug(divGradPsiSnGradAlpha, false);
addToRunTimeSelectionTable(surfaceTensionForce, divGradPsiSnGradAlpha, Dictionary);

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

divGradPsiSnGradAlpha::divGradPsiSnGradAlpha() {}

divGradPsiSnGradAlpha::divGradPsiSnGradAlpha(const dictionary& dict) {}

// * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * * //

tmp<surfaceScalarField> divGradPsiSnGradAlpha::faceSurfaceTensionForce
(
    const volScalarField& alpha, 
    const volScalarField& psi 
) const 
{
    // Compute interface-normals from the normalized 
    // gradient of the level set function.
    tmp<volVectorField> nSigmaTmp = fvc::grad(psi);
    volVectorField& nSigma = nSigmaTmp.ref();
    nSigma /= mag(nSigma);

    return fvc::interpolate(fvc::div(nSigma)) * fvc::snGrad(alpha);
}

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

// ************************************************************************* //
