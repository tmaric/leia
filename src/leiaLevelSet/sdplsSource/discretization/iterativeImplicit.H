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
    Foam::iterativeImplicit

Description
    The class iterativeImplicit implements an implicit discretization of
    the source term in the most simplest way. The linearization constants are
    calculated as 
    \f{equation}{ \begin{aligned}
        S_{iterativeImplicit,\,c} &= 0\\
        S_{iterativeImplicit,\,p} &= f_{nl}(\psi^n),
    \end{aligned}
    \f}
    resulting in the iterativeImplicit source
    \f{equation}{
        S_{iterativeImplicit}(\psi^{n+1}) = f_{nl}(\psi^n)\psi^{n+1}_c.
    \f}
    This is the most accurate linearization as the approximation fits the tangent of
    the relation \f$S_{SDPLS}(\psi^{n+1}) \sim \psi^{n+1}\f$ 
    \cite patankar_numerical_1980, see the sdplsSource equation .


SourceFiles
    iterativeImplicit.C

\*---------------------------------------------------------------------------*/

#ifndef iterativeImplicit_H
#define iterativeImplicit_H

#include "simpleLinearImplicit.H"


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                Class iterativeImplicit Declaration
\*---------------------------------------------------------------------------*/

// template<class Type>
class iterativeImplicit
: 
    public simpleLinearImplicit
{
    int niterations_;

public:

    // Static Data Members
    
    TypeName("iterativeImplicit");

    // Constructors

        //- Default Construct
        iterativeImplicit();

    //- Destructor
    virtual ~iterativeImplicit() = default;

    // Member Functions

    virtual bool iterative();


};

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

#endif

// ************************************************************************* //
