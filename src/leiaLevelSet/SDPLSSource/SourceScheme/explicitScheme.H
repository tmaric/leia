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
    Foam::explicitScheme

Description
    The class explicitScheme implements the explicit discretization of the source
    term. The linearization constants are calculated as 
    \f{equation}{
    \begin{aligned}
        S_{explicit,\,c} &= f_{nl}(\psi^n) \psi^n_c\\
        S_{explicit,\,p} &= 0
    \end{aligned}
    \f}
    where the constant \f$S_c\f$ explicitly evaluates the entire SDPLS source term.
    With this, the explicit source is written as 
    \f{equation}{
        S_{explicit}(\psi^{n+1}) = f_{nl}(\psi^n) \psi^n
    \f}
    Without any implicit part, this discretized source term only contributes to the
    r.h.s. of the resulting algebraic linear system.

SourceFiles
    explicitScheme.C

\*---------------------------------------------------------------------------*/

#ifndef explicitScheme_H
#define explicitScheme_H

#include "SourceScheme.H"


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
// namespace fv
// {
/*---------------------------------------------------------------------------*\
                         Class explicitScheme Declaration
\*---------------------------------------------------------------------------*/

// template<class Type>
class explicitScheme
: 
    public SourceScheme
{
public:

    // Static Data Members
    
    TypeName("explicit");

    // Constructors
    explicitScheme();

    //- Destructor
    virtual ~explicitScheme() = default;

    // Member Functions

protected:
    virtual void updateSc(const volScalarField& nonLinearPart, const volScalarField& psi);
    virtual void updateSp(const volScalarField& nonLinearPart);


};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //


// } // End namespace fv
} // End namespace Foam

#endif

// ************************************************************************* //
