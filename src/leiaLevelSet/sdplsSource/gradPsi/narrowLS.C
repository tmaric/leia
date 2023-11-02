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

#include "narrowLS.H"
#include "addToRunTimeSelectionTable.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
    defineTypeNameAndDebug(narrowLS, false);
    addToRunTimeSelectionTable(gradPsi, narrowLS, Dictionary);
}

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::narrowLS::narrowLS(const fvMesh& mesh)
    :
        gradPsi(mesh),
        narrowBand_(mesh.lookupObject<volScalarField>("NarrowBand")),
        nc_(mesh.lookupObject<volVectorField>("nc"))
{}

// * * * * * * * * * * * * * *  Member functions  * * * * * * * * * * * * * * //

Foam::tmp<volVectorField> Foam::narrowLS::grad(const volScalarField& psi) const
{
    tmp<volVectorField> tgradpsi = fvc::grad(psi);
    forAll(narrowBand_, cellID)
    {
        if (narrowBand_[cellID] == 1)
        {
            tgradpsi.ref()[cellID] = nc_[cellID];
        }
    }
    return tgradpsi;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //
