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
    Foam::mollifier1

Description
    Implements the mollifier m1 proposed by Fricke et al. \cite fricke_locally_2022.
    \f{equation}{
        m1(x) = 
        \begin{cases}
        1 & \text{if} \quad 0 \le x \le w_1,\\
        \exp \left( - \ln(10^3) \frac{(x-w_1)^2}{(w_2-w_1)^2} \right) & \text{if} \quad w_1 < x,\\
        m1(-x) & \text{if} \quad x<0
        \end{cases}
    \f} 


SourceFiles
    mollifier1.C

\*---------------------------------------------------------------------------*/

#ifndef mollifier1_H
#define mollifier1_H

#include "mollifier.H"


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{


/*---------------------------------------------------------------------------*\
                         Class mollifier1 Declaration
\*---------------------------------------------------------------------------*/

class mollifier1
    :   public mollifier
{
private:

    // Private Data Members

        //- is the width of the plateau
        scalar w1_;
        
        //- is the width before the mollifier is close to 0 (Default: 10^-3).  
        scalar w2_; 
public:

    // Static Data Members
    
        TypeName("m1");

    // Constructors

        //- Construct from fvSolution
        mollifier1(const dictionary& dict);
    
    //- Destructor
    virtual ~mollifier1() = default;

protected:

    // Protected Member functions
        
        //- Mollifier formula for a cell
        //
        //  @param x is the Level Set value
        virtual double mollify(double x) const;


};

} // End namespace Foam

#endif

// ************************************************************************* //
