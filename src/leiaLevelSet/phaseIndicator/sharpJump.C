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

#include "sharpJump.H"
#include "addToRunTimeSelectionTable.H"
#include "volFields.H"
#include "surfaceFields.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

defineTypeNameAndDebug(sharpJump, false);
addToRunTimeSelectionTable(phaseIndicator, sharpJump, Dictionary);

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

sharpJump::sharpJump(const dictionary& dict)
{}

// * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * * //

void sharpJump::calcPhaseIndicator
(
    volScalarField& alpha, 
    const volScalarField& phi
) 
{
    forAll(alpha, cellID)
    {
        if (phi[cellID] < 0)
            alpha[cellID] = 1;
        else if (phi[cellID] > 0)
            alpha[cellID] = 0;
    }
}

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

// ************************************************************************* //
