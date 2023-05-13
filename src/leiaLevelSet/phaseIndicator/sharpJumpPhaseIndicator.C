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

#include "sharpJumpPhaseIndicator.H"
#include "addToRunTimeSelectionTable.H"
#include "volFields.H"
#include "surfaceFields.H"
#include "fvcAverage.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

defineTypeNameAndDebug(sharpJumpPhaseIndicator, false);
addToRunTimeSelectionTable(phaseIndicator, sharpJumpPhaseIndicator, Mesh);

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

sharpJumpPhaseIndicator::sharpJumpPhaseIndicator(const fvMesh& mesh)
:
        phaseIndicator(mesh),
        nAverages_(phaseIndDict_.getOrDefault<label>("nAverages", 0))
{}

// * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * * //

void sharpJumpPhaseIndicator::calcPhaseIndicator
(
    volScalarField& alpha, 
    const volScalarField& psi 
) 
{
    forAll(alpha, cellID)
    {
        if (psi[cellID] <= 0)
            alpha[cellID] = 1;
        else 
            alpha[cellID] = 0;
    }

    for (int i = 0; i < 1; ++i)
        alpha = fvc::average(alpha);
}

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

// ************************************************************************* //
