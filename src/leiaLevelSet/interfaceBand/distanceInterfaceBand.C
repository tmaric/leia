/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2022 Tomislav Maric & Julian Reitzel, TU Darmstadt
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

#include "distanceInterfaceBand.H"
#include "addToRunTimeSelectionTable.H"
#include "fvCFD.H"

namespace Foam
{
    defineTypeNameAndDebug(distanceInterfaceBand, false);
    addToRunTimeSelectionTable(interfaceBand, distanceInterfaceBand, Dictionary);

    distanceInterfaceBand::distanceInterfaceBand(const dictionary& dict, const volScalarField& psi)
        :
            interfaceBand(dict, psi),
            ncells_(dict.getOrDefault<scalar>("ncells", 5))
    {
        // Info << "interfaceBand type: " << TypeNameString << " with ncells: " << ncells_ << nl << endl;
    }

    void distanceInterfaceBand::calc()
    {
        const volScalarField& psi = this->psi_;
        volScalarField& interfaceBand = interfaceBandField_;
        interfaceBand = dimensionedScalar(interfaceBand.dimensions(), 0.);

        const auto deltaX = pow(mesh().deltaCoeffs(),-1).cref();
        const auto deltaX_min = gMin(deltaX);

        forAll(interfaceBand, cellID)
        {
            if (psi[cellID]/deltaX_min < ncells_)
            {
                interfaceBand[cellID] = 1.0;
            }
        }
    }

} // End namespace Foam

// ************************************************************************* //
