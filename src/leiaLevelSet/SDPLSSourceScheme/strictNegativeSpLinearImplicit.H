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
    Foam::strictNegativeSpLinearImplicit

Description
    Concrete class which calculates the SDPLS source term.
    Similiar to simpleLinearImplicit it trys to calculate the non-linear part explicitly and the linear part implicitly.
    But there is a switch. If the implicit part would produce positive coefficients contributions the the linear system, 
    then this part is treaded explicitly in order to preserve diagonal dominance.

    strictNegativeSpLinearImplicit $S=S_c +S_p \psi$
	- $S_c = max(-R(\psi^\*),0)\psi^\*$
	- $S_p = -max(R(\psi^\*),0)$

SourceFiles
    strictNegativeSpLinearImplicit.C

\*---------------------------------------------------------------------------*/

#ifndef strictNegativeSpLinearImplicit_H
#define strictNegativeSpLinearImplicit_H

#include "typeInfo.H"
#include "SDPLSSourceScheme.H"
#include "fvMatrix.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
namespace fv
{
/*---------------------------------------------------------------------------*\
                         Class strictNegativeSpLinearImplicit Declaration
\*---------------------------------------------------------------------------*/

// template<class Type>
class strictNegativeSpLinearImplicit
: 
    public SDPLSSourceScheme
{
public:

    // Static Data Members
    
    TypeName("strictNegativeSpLinearImplicit");

    // Constructors
    strictNegativeSpLinearImplicit(const fvMesh& mesh);

    //- Destructor
    virtual ~strictNegativeSpLinearImplicit() = default;

    // Member Functions

    virtual tmp<fvScalarMatrix> doFvmSDPLSSource(const volScalarField& psi, const volVectorField& U);

};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //


} // End namespace fv
} // End namespace Foam

#endif

// ************************************************************************* //
