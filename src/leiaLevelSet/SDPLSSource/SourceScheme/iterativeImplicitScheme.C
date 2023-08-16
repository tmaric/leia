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

#include "iterativeImplicitScheme.H"
#include "addToRunTimeSelectionTable.H"
#include "simpleLinearImplicitScheme.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
    defineTypeNameAndDebug(iterativeImplicitScheme, false);
    addToRunTimeSelectionTable
        (
            SourceScheme,
            iterativeImplicitScheme,
            Dictionary
        );
}

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //
Foam::iterativeImplicitScheme::iterativeImplicitScheme()
    :
        simpleLinearImplicitScheme(),
        niterations_(3)
{}

// * * * * * * * * * * * * * *  Member functions  * * * * * * * * * * * * * * //

bool Foam::iterativeImplicitScheme::iterative()
{

    // if (niterations_ > 0)
    // {
    //     return --niterations_;
    // }
    // else if (niterations_ == 0)
    // {
    //     niterations_ = 3;
    //     return 0;
    // }
    // else
    // {
    //     FatalErrorInFunction << "No valid choice" << exit(FatalError);
    //     return 0;
    // }
    return true;
}

// ************************************************************************* //


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //
