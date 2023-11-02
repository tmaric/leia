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
    Foam::simpleLinearImplicit

Description
    The class simpleLinearImplicit implements an implicit discretization of
    the source term in the most simplest way. The linearization constants are
    calculated as 
    \f{equation}{ \begin{aligned}
        S_{simpleLinearImplicit,\,c} &= 0\\
        S_{simpleLinearImplicit,\,p} &= f_{nl}(\psi^n),
    \end{aligned}
    \f}
    resulting in the simpleLinearImplicit source
    \f{equation}{
        S_{simpleLinearImplicit}(\psi^{n+1}) = f_{nl}(\psi^n)\psi^{n+1}_c.
    \f}
    This is the most accurate linearization as the approximation fits the tangent of
    the relation \f$S_{SDPLS}(\psi^{n+1}) \sim \psi^{n+1}\f$ 
    \cite patankar_numerical_1980, see the sdplsSource equation .


SourceFiles
    simpleLinearImplicit.C

\*---------------------------------------------------------------------------*/

#ifndef simpleLinearImplicit_H
#define simpleLinearImplicit_H

#include "discretization.H"


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                Class simpleLinearImplicit Declaration
\*---------------------------------------------------------------------------*/

// template<class Type>
class simpleLinearImplicit
: 
    public discretization
{
public:

    // Static Data Members
    
    TypeName("simpleLinearImplicit");

    // Constructors

        //- Default Construct
        simpleLinearImplicit();

    //- Destructor
    virtual ~simpleLinearImplicit() = default;

    // Member Functions

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
