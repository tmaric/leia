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

#include "Mollifier1.H"
#include "addToRunTimeSelectionTable.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
    defineTypeNameAndDebug(Mollifier1, false);
    addToRunTimeSelectionTable(Mollifier, Mollifier1, Dictionary);
}

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::Mollifier1::Mollifier1(const dictionary& dict)
    :
        Mollifier(dict),
        w1_(dict.get<scalar>("w1")),
        w2_(dict.getOrDefault<scalar>("w2", 1e-3))
{}

// * * * * * * * * * * * * * *  Member functions  * * * * * * * * * * * * * * //

double Foam::Mollifier1::mollify(double x) const
{
    double s = log(1000.0)/pow((w2_-w1_),2); // help variable  
    if (x >= 0) {
        if (x < w1_) {
            return 1.0;
        } 
        else {
            return exp(- s*pow(x-w1_, 2));
        }
    } 
    else {
        return mollify(-x);
    }
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //
