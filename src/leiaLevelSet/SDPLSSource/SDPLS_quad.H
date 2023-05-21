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
    Foam::SDPLS_quad

Description
    SDPLS quadratic source term
    \f{equation}{
        f_{nl,\,lin}(\psi^{n}) = \| \nabla \psi \|_2^{-2} - 1
    \f} 

SourceFiles
    SDPLS_quad.C

\*---------------------------------------------------------------------------*/

#ifndef SDPLS_quad_H
#define SDPLS_quad_H

#include "typeInfo.H"
#include "autoPtr.H"
#include "runTimeSelectionTables.H"
#include "fvCFD.H"
#include "SDPLSSource.H"


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                         Class SDPLS_quad Declaration
\*---------------------------------------------------------------------------*/

// template<class Type>
class SDPLS_quad
: 
    public SDPLSSource
{
public:

    // Static Data Members
    
    TypeName("quad");

    // Constructors
    SDPLS_quad(const dictionary& dict, const fvMesh& mesh);

    //- Destructor
    virtual ~SDPLS_quad() = default;

protected:

    // Protected Member Functions

        //- Calculate the non-linear function \f$f_{nl}(\psi^{n})\f$.
        //  The calculation is explicit, using the values from the old time 
        //  step. 
        virtual tmp<volScalarField> nonLinearPart
        (
            const volScalarField& R,
            const volScalarField& psi,
            const volVectorField& U
        ) const;

};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

#endif

// ************************************************************************* //
