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

#include "emptyNarrowBand.H"
#include "addToRunTimeSelectionTable.H"

namespace Foam
{
defineTypeNameAndDebug(emptyNarrowBand, false);
addToRunTimeSelectionTable(narrowBand, emptyNarrowBand, Dictionary);

emptyNarrowBand::emptyNarrowBand(const dictionary& dict, const volScalarField& psi)
    :
        narrowBand(dict, psi),
        field_(
            IOobject(
                "NarrowBand",
                mesh().time().timeName(),
                mesh(),
                IOobject::NO_READ,
                IOobject::AUTO_WRITE
                ), 
            mesh(), 
            dimensioned(dimless, 0.)
        )
{}

    const volScalarField& emptyNarrowBand::field() const
    {
        return field_;
    }

    volScalarField& emptyNarrowBand::field()
    {
        return field_;
    }

    void emptyNarrowBand::write() const
    {
        field().write();
    }

} // End namespace Foam

// ************************************************************************* //
