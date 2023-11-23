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
    Foam::explicitDiscretization

Description
    The class explicitDiscretization implements the explicitDiscretization discretization of the source
    term. The linearization constants are calculated as 
    \f{equation}{
    \begin{aligned}
        S_{explicitDiscretization,\,c} &= f_{nl}(\psi^n) \psi^n_c\\
        S_{explicitDiscretization,\,p} &= 0
    \end{aligned}
    \f}
    where the constant \f$S_c\f$ explicitDiscretizationly evaluates the entire SDPLS source term.
    With this, the explicitDiscretization source is written as 
    \f{equation}{
        S_{explicitDiscretization}(\psi^{n+1}) = f_{nl}(\psi^n) \psi^n
    \f}
    Without any implicit part, this discretized source term only contributes to the
    r.h.s. of the resulting algebraic linear system.

SourceFiles
    explicitDiscretization.C

\*---------------------------------------------------------------------------*/

#ifndef explicitDiscretization_H
#define explicitDiscretization_H

#include "discretization.H"


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                    Class explicitDiscretization Declaration
\*---------------------------------------------------------------------------*/


class explicitDiscretization
: 
    public discretization
{
public:

    // Static Data Members
    
        TypeName("explicit");

    // Constructors
        explicitDiscretization();

    //- Destructor
    virtual ~explicitDiscretization() = default;


protected:

    // Protected Member Functions

        //- Calculates the constant of the source term linearization.
        virtual tmp<scalarField> Sc
            (
                const volScalarField& nonLinearPart, 
                const volScalarField& psi
            ) const;

        //- Calculates the proportional constant of the source term 
        //  linearization.
        virtual tmp<scalarField> Sp
            (
                const volScalarField& nonLinearPart
            ) const;
};

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

#endif

// ************************************************************************* //
