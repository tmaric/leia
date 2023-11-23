/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2022 Julian Reitzel, TU Darmstadt
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

#include "strictNegativeSpLinearImplicit.H"
#include "addToRunTimeSelectionTable.H"
#include "sdplsSource.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
    defineTypeNameAndDebug(strictNegativeSpLinearImplicit, false);
    addToRunTimeSelectionTable
        (
            discretization,
            strictNegativeSpLinearImplicit,
            Dictionary
        );
}

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //
Foam::strictNegativeSpLinearImplicit::
strictNegativeSpLinearImplicit()
    :
        discretization()
{}

// * * * * * * * * * * * * * *  Member functions  * * * * * * * * * * * * * * //

Foam::tmp<scalarField> 
Foam::strictNegativeSpLinearImplicit::
Sc(const volScalarField& nonLinearPart, const volScalarField& psi) const
{
    tmp<scalarField> tSc(new scalarField(nonLinearPart.size(), 0.0));
    forAll(nonLinearPart, cellID)
    {
        tSc.ref()[cellID] = Foam::max(nonLinearPart[cellID], 0) *psi[cellID];
    }
    return tSc;
}

Foam::tmp<scalarField> 
Foam::strictNegativeSpLinearImplicit::
Sp(const volScalarField& nonLinearPart) const
{
    tmp<scalarField> tSp(new scalarField(nonLinearPart.size(), 0.0));
    forAll(nonLinearPart, cellID)
    {
        tSp.ref()[cellID] = Foam::min(nonLinearPart[cellID], 0);
    }
    return tSp;
}

// ************************************************************************* //


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //
