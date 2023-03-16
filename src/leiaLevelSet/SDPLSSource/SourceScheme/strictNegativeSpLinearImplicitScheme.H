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
    Concrete class which calculates the SDPLS source term strictNegativeSpLinearImplicitly.

SourceFiles
    strictNegativeSpLinearImplicitScheme.C

\*---------------------------------------------------------------------------*/

#ifndef strictNegativeSpLinearImplicitScheme_H
#define strictNegativeSpLinearImplicitScheme_H

#include "SourceScheme.H"


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
// namespace fv
// {
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
    strictNegativeSpLinearImplicitScheme();

    //- Destructor
    virtual ~strictNegativeSpLinearImplicitScheme() = default;

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
