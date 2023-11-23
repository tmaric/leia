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

\*---------------------------------------------------------------------------*/

#include "sdplsRLin.H"
#include "addToRunTimeSelectionTable.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
    defineTypeNameAndDebug(sdplsRLin, false);
    addToRunTimeSelectionTable(sdplsSource, sdplsRLin, Dictionary);
}

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //
Foam::sdplsRLin::sdplsRLin(const dictionary& dict, const fvMesh& mesh)
    :
        sdplsSource(dict, mesh)
{}

// * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * * //

Foam::tmp<volScalarField> 
Foam::sdplsRLin::
nonLinearPart
    (
        const volScalarField& R, 
        const volScalarField& psi, 
        const volVectorField& U
    ) const
{
    const fvMesh& mesh = psi.mesh();

    return tmp<volScalarField>
    (
        new volScalarField
        (
            IOobject
            (
                word(),
                fileName(),
                mesh,
                IOobject::NO_READ,
                IOobject::NO_WRITE,
                false
            ),
            R + dimensioned<scalar>(dimless/dimTime, 1.0)
            *(dimensioned<scalar>(1.0) - mag(grad(psi)))
        )
    );
}

// ************************************************************************* //



// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //
