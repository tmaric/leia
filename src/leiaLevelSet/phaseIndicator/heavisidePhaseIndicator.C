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

#include "heavisidePhaseIndicator.H"
#include "addToRunTimeSelectionTable.H"
#include "volFields.H"
#include "surfaceFields.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

defineTypeNameAndDebug(heavisidePhaseIndicator, false);
addToRunTimeSelectionTable(phaseIndicator, heavisidePhaseIndicator, Mesh);

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

heavisidePhaseIndicator::heavisidePhaseIndicator(const fvMesh& mesh)
    :
        phaseIndicator(mesh),
        nCells_(phaseIndDict_.getOrDefault<label>("nCells", 2))
{}

// * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * * //

void heavisidePhaseIndicator::calcPhaseIndicator
(
    volScalarField& alpha, 
    const volScalarField& psi 
) 
{
    const fvMesh& mesh = alpha.mesh();

    scalar epsilon = (nCells_ / 2.0) * 
        Foam::max(Foam::pow(mesh.deltaCoeffs(), -1)).value(); 

    forAll(alpha, cellID)
    {
        if (psi[cellID] < -epsilon)
            alpha[cellID] = 0;
        else if (psi[cellID] > epsilon)
            alpha[cellID] = 1;
        else
        {
            alpha[cellID] = 0.5 + 
                 + psi[cellID] / (2*epsilon)
                 + Foam::sin((M_PI * psi[cellID]) / epsilon) / 
                     (2*M_PI);
        }
    }
    alpha == 1 - alpha;
}

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

// ************************************************************************* //
