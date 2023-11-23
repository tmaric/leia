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
    Foam::sdplsRQuad

Description
    SDPLS R-quadratic source term
    \f{equation}{
        f_{nl,\,lin}(\psi^{n}) = R + \| \nabla \psi \|_2^{-2} - 1
    \f} 

SourceFiles
    sdplsRQuad.C

\*---------------------------------------------------------------------------*/

#ifndef sdplsRQuad_H
#define sdplsRQuad_H

#include "typeInfo.H"
#include "autoPtr.H"
#include "runTimeSelectionTables.H"
#include "fvCFD.H"
#include "sdplsSource.H"


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                         Class sdplsRQuad Declaration
\*---------------------------------------------------------------------------*/

// template<class Type>
class sdplsRQuad
: 
    public sdplsSource
{
public:

    // Static Data Members
    
    TypeName("Rquad");

    // Constructors
    
        //- Construct from fvSolution
        sdplsRQuad(const dictionary& dict, const fvMesh& mesh);

    //- Destructor
    virtual ~sdplsRQuad() = default;

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
