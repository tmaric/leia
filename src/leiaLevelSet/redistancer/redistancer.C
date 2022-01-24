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

#include "redistancer.H"
#include "addToRunTimeSelectionTable.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

defineTypeNameAndDebug(redistancer, false);
defineRunTimeSelectionTable(redistancer, Dictionary);
addToRunTimeSelectionTable(redistancer, redistancer, Dictionary);

// * * * * * * * * * * * * * * * * Selectors * * * * * * * * * * * * * * * * //

Foam::autoPtr<Foam::redistancer>
Foam::redistancer::New(const dictionary& dict)
{
    const word type = dict.getOrDefault<word>("type", "noRedistancing");

    // Find the constructor pointer for the model in the constructor table.
    auto* ctorPtr = DictionaryConstructorTable(type);

    // If the constructor pointer is not found in the table.
    if (!ctorPtr) 
    {
        FatalIOErrorInLookup
        (
            dict,
            "redistancer",
            type,
            *DictionaryConstructorTablePtr_
        ) << exit(FatalIOError);
    }

    return autoPtr<redistancer>(ctorPtr(dict));
}

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

redistancer::redistancer(const dictionary& dict)
{}

// ************************************************************************* //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //
