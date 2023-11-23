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
    Foam::sdplsR

Description
    The class sdplsR implements the concrete SDPLS source term formula proposed by
    Fricke et al. \cite fricke_locally_2022. As described in the base class
    sdplsSource, the explicit evaluation of the SDPLS source term instance is
    implemented in the member function nonLinearPart() as 
    \f{equation}{
        f_{nl,\,R}(\psi^{n}) = R = \left \langle (\nabla \U)_c \frac{(\nabla
        \psi)_c^n}{\|(\nabla\psi)_c^n\|_2},  \frac{(\nabla
        \psi)_c^n}{\|(\nabla\psi)_c^n\|_2} \right \rangle, 
    \f} 
    with the use of the
    gradient schemes specified in the fvSchemes dictionary.

Usage
    Provide
    \verbatim
    sdplsSource
    {
        type R;
        discretization <SourceScheme>;
    }
    \endverbatim
    with some type <SourceScheme> of the RTS in \c system/fvSolution.levelSet.

SourceFiles
    sdplsR.C

\*---------------------------------------------------------------------------*/

#ifndef sdplsR_H
#define sdplsR_H

#include "typeInfo.H"
#include "autoPtr.H"
#include "runTimeSelectionTables.H"
#include "fvCFD.H"
#include "sdplsSource.H"


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                         Class sdplsR Declaration
\*---------------------------------------------------------------------------*/

// template<class Type>
class sdplsR
: 
    public sdplsSource
{
public:

    // Static Data Members
    
    TypeName("R");

    // Constructors
    sdplsR(const dictionary& dict, const fvMesh& mesh);

    //- Destructor
    virtual ~sdplsR() = default;

protected:

    // Protected Member Functions

        //- Calculate the non-linear function \f$f_{nl}(\psi^{n})\f$.
        //  The calculation is explicit, using the values from the old time 
        //  step. This instance returns R.
        virtual tmp<volScalarField> nonLinearPart
        (
            const volScalarField& R,
            const volScalarField& psi,
            const volVectorField& U
        ) const;

};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //


// } // End namespace fv
} // End namespace Foam

#endif

// ************************************************************************* //
