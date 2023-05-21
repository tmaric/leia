/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2022 AUTHOR,AFFILIATION
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

#include "strictNegativeSpLinearImplicitScheme.H"
#include "addToRunTimeSelectionTable.H"
#include "SDPLSSource.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
// namespace fv
// {


defineTypeNameAndDebug(strictNegativeSpLinearImplicitScheme, false);
addToRunTimeSelectionTable(SourceScheme, strictNegativeSpLinearImplicitScheme, Dictionary);

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //
strictNegativeSpLinearImplicitScheme::strictNegativeSpLinearImplicitScheme()
    :
        SourceScheme()
{}

tmp<scalarField> strictNegativeSpLinearImplicitScheme::Sc(const volScalarField& nonLinearPart, const volScalarField& psi)
{
    tmp<scalarField> tSc(new scalarField(nonLinearPart.size(), 0.0));
    forAll(nonLinearPart, cellID)
    {
        tSc.ref()[cellID] = Foam::max(nonLinearPart[cellID], 0) *psi[cellID];
    }
    return tSc;
}

tmp<scalarField> strictNegativeSpLinearImplicitScheme::Sp(const volScalarField& nonLinearPart)
{
    tmp<scalarField> tSp(new scalarField(nonLinearPart.size(), 0.0));
    forAll(nonLinearPart, cellID)
    {
        tSp.ref()[cellID] = Foam::min(nonLinearPart[cellID], 0);
    }
    return tSp;
}

// ************************************************************************* //

// } // End namespace fv
} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //
