/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2021 Tomislav Maric, TU Darmstadt
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
    Foam::geometricPhaseIndicator

Description

    Computes the phase indicator as a volume fraction from a field \psi_c. 
    
    In each cell \Omega_c, a piecewise-planer approximation \psi^l_c is
    computed using linear least squares. This implicit plane is then used to
    truncate the cell and compute the phase-specific volume \Omega_c^-,
    resulting in the volume fraction 

    \alpha_c = |\Omega_c^-| / |\Omega_c| 

SourceFiles
    geometricPhaseIndicator.C

\*---------------------------------------------------------------------------*/

#ifndef geometricPhaseIndicator_H
#define geometricPhaseIndicator_H

#include "phaseIndicator.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                         Class geometricPhaseIndicator Declaration
\*---------------------------------------------------------------------------*/

class geometricPhaseIndicator
:
    public phaseIndicator
{

public:

    // Static Data Members

    TypeName ("geometric");

    // Constructors

    geometricPhaseIndicator();

    geometricPhaseIndicator(const dictionary& dict);

    //- Destructor
    virtual ~geometricPhaseIndicator() = default;

    // Member Functions
    virtual void calcPhaseIndicator
    (
        volScalarField& alpha, 
        const volScalarField& psi 
    ); 
};

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
