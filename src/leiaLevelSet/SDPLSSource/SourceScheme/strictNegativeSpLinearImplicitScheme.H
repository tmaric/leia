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
    Foam::strictNegativeSpLinearImplicitScheme

Description
    The class strictNegativeSpLinearImplicitScheme implements an implicit
    discretization of the source term that ensures contributions to the diagonal
    dominance of the resulting algebraic linear system. The linearization constants
    are computed as 
    \f{equation}{ \begin{aligned}
        S_{strictNegativeSpLinearImplicit,\,c} &= \max(f_{nl}(\psi^n),0)\psi^n_c\\
        S_{strictNegativeSpLinearImplicit,\,p} &= \min(f_{nl}(\psi^n),0),
    \end{aligned}
    \f}
    which ensures that the constant \f$S_p \le 0\f$ for all cells 
    \cite patankar_numerical_1980. The resulting source term, written as 
    \f{equation}{
        S_{strictNegativeSpLinearImplicit}(\psi^{n+1}) =
        \max(f_{nl}(\psi^n),0)\psi^n_c
        + \min(f_{nl}(\psi^n),0)\psi^{n+1}_c,
    \f}
    thus switches between explicit and implicit discretization cell by cell
    depending on the sign of \f$f(\psi^n)\f$. This addresses the problem that
    unconditional implicit treatment can cause the solution to diverge. The reason
    is that a positive \f$S_p\f$ reduces the diagonal coefficient in the assembled
    linear system. This harms the diagonal dominance of the matrix, which then may
    lose its M-matrix and discrete stability property.

SourceFiles
    strictNegativeSpLinearImplicitScheme.C

\*---------------------------------------------------------------------------*/

#ifndef strictNegativeSpLinearImplicitScheme_H
#define strictNegativeSpLinearImplicitScheme_H

#include "SourceScheme.H"


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
            Class strictNegativeSpLinearImplicitScheme Declaration
\*---------------------------------------------------------------------------*/

// template<class Type>
class strictNegativeSpLinearImplicitScheme
: 
    public SourceScheme
{
public:

    // Static Data Members
    
    TypeName("strictNegativeSpLinearImplicit");

    // Constructors

        //- Default construct
        strictNegativeSpLinearImplicitScheme();

    //- Destructor
    virtual ~strictNegativeSpLinearImplicitScheme() = default;

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
        virtual tmp<scalarField> Sp(const volScalarField& nonLinearPart) const;
};

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

#endif

// ************************************************************************* //
