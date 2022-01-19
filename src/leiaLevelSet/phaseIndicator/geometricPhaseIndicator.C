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

\*---------------------------------------------------------------------------*/

#include "geometricPhaseIndicator.H"
#include "addToRunTimeSelectionTable.H"
#include "volFields.H"
#include "surfaceFields.H"
#include "fvcGrad.H"
#include "foamGeometry.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

defineTypeNameAndDebug(geometricPhaseIndicator, false);
addToRunTimeSelectionTable(phaseIndicator, geometricPhaseIndicator, Dictionary);

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

geometricPhaseIndicator::geometricPhaseIndicator()
{}

geometricPhaseIndicator::geometricPhaseIndicator(const dictionary& dict)
{}

// * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * * //

void geometricPhaseIndicator::calcPhaseIndicator
(
    volScalarField& alpha, 
    const volScalarField& psi 
) 
{
    alpha == dimensionedScalar("0", dimless, 0);

    const fvMesh& mesh = alpha.mesh();

    forAll(alpha, cellID)
    {
        if (psi[cellID] < 0)
            alpha[cellID] = 1;
        else 
            alpha[cellID] = 0;
    }

    volVectorField nc ("nc", fvc::grad(psi));
    volScalarField dc ("dc", psi - (nc & mesh.C()));

    const auto& own = mesh.owner();  
    const auto& nei = mesh.neighbour(); 

    volScalarField narrowBand("narrowBand", psi);  
    forAll(own, faceI)
    {
        if (psi[own[faceI]] * psi[nei[faceI]] < 0)
        {
            narrowBand[own[faceI]] = 1;
            narrowBand[nei[faceI]] = 1;
        }
    }

    forAll(narrowBand, cellI)
    {
        if (narrowBand[cellI] == 1)
        {
            hesseNormalPlane cutPlane(nc[cellI], dc[cellI]);
            auto cellIntersection = intersectCell(cellI, mesh, cutPlane); 
            alpha[cellI] = cellIntersection.volume() / mesh.V()[cellI];
        }
    }
}

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

// ************************************************************************* //
