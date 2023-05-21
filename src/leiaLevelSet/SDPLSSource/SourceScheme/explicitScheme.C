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

#include "explicitScheme.H"
#include "addToRunTimeSelectionTable.H"
#include "SDPLSSource.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
    defineTypeNameAndDebug(explicitScheme, false);
    addToRunTimeSelectionTable(SourceScheme, explicitScheme, Dictionary);
}

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //
Foam::explicitScheme::explicitScheme()
    :
        SourceScheme()
{}

// * * * * * * * * * * * * * *  Member functions  * * * * * * * * * * * * * * //

Foam::tmp<scalarField> 
Foam::explicitScheme::
Sc(const volScalarField& nonLinearPart, const volScalarField& psi) const
{
    return (nonLinearPart * psi)().field();
}

Foam::tmp<scalarField> 
Foam::explicitScheme::Sp(const volScalarField& nonLinearPart) const
{
    return tmp<scalarField>(new scalarField(nonLinearPart.size(), 0.0));
}

// ************************************************************************* //

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //
