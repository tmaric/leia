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
    Foam::EmptyNarrowBand

Description
    Abstract base class which starts the model hierachy for marking cells which are near the interface aka the "narrowBand".
    Calculate a field with {0,1} which mark cells near the zero level-set interface.


SourceFiles
    EmptyNarrowBand.C

\*---------------------------------------------------------------------------*/

#ifndef EmptyNarrowBand_H
#define EmptyNarrowBand_H

#include "NarrowBand.H"


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                         Class EmptyNarrowBand Declaration
\*---------------------------------------------------------------------------*/

class EmptyNarrowBand
    : public NarrowBand
{
private:
    volScalarField field_;

public:
    TypeName("empty");
    EmptyNarrowBand(const dictionary& dict, const volScalarField& psi);
    virtual ~EmptyNarrowBand() = default;

    // Member Functions
    const volScalarField& field() const;
    volScalarField& field();
    virtual void write() const;

};
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
