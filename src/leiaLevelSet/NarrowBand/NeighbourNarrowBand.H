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

Class
    Foam::NeighbourNarrowBand

Description
    Concrete class which calculates a field with {0,1} which mark cells near the zero level-set interface.

SourceFiles
    NeighbourNarrowBand.C


\*---------------------------------------------------------------------------*/

#ifndef NeighbourNarrowBand_H
#define NeighbourNarrowBand_H

#include "SignChangeNarrowBand.H"


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                         Class NeighbourNarrowBand Declaration
\*---------------------------------------------------------------------------*/

class NeighbourNarrowBand
    : public SignChangeNarrowBand
{
    int ntimes_;
    
public:

    TypeName("neighbours");
    NeighbourNarrowBand(const dictionary& dict, const volScalarField& psi);
    virtual ~NeighbourNarrowBand() = default;
    virtual void calc();
};
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
