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

#include "Mollifier1.H"
#include "addToRunTimeSelectionTable.H"
// #include "fvSolution.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
// namespace fv
// {

defineTypeNameAndDebug(Mollifier1, false);
addToRunTimeSelectionTable(Mollifier, Mollifier1, Dictionary);


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Mollifier1::Mollifier1(const dictionary& dict)
    :
        Mollifier(dict),
        w1_(dict.get<scalar>("w1")),
        w2_(dict.getOrDefault<scalar>("w2", 1e-3))
{}

// * * * * * * * * * * * * * *  Member functions  * * * * * * * * * * * * * * //

double Mollifier1::mollify(double x) const
{
    double w1 = w1_; 
    double w2 = w2_;
    // w1 is the width of the plateau
    // w2 is the width before the mollifier is close to 0 (here 10^-3).  
    double s=1.0; // help variable  
    s = log(1000.0)/pow((w2-w1),2);    
    if (x >= 0) {
        if (x < w1) {
            return 1.0;
        } 
        else {
            return exp(- s*pow(x-w1, 2));
        }
    } 
    else {
        return mollify(-x);
    }
}



// } // End namespace fv

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //
