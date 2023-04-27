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

Class
    Foam::Mollifier11

Description


SourceFiles
    Mollifier11.C

\*---------------------------------------------------------------------------*/

#ifndef Mollifier11_H
#define Mollifier11_H

#include "Mollifier.H"


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
// namespace fv
// {


/*---------------------------------------------------------------------------*\
                         Class Mollifier11 Declaration
\*---------------------------------------------------------------------------*/

class Mollifier1
    :   public Mollifier
{
private:
        scalar w1_;
        scalar w2_; 
public:

    // Static Data Members
    
    TypeName("m1");

    // Constructors

        //- Construct from components
        Mollifier1(const dictionary& dict);
    //- Destructor
    virtual ~Mollifier1() = default;

protected:
    virtual double mollify(double x) const;


};

// } // End namespace fv

} // End namespace Foam

#endif

// ************************************************************************* //
