/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2019-2021 OpenCFD Ltd.
    Copyright (C) YEAR AUTHOR, AFFILIATION
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

#include "writeIsoSurfaceTopo.H"
#define namespaceFoam  // Suppress <using namespace Foam;>
#include "fvCFD.H"
#include "unitConversion.H"
#include "addToRunTimeSelectionTable.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

defineTypeNameAndDebug(writeIsoSurfaceTopoFunctionObject, 0);

addRemovableToRunTimeSelectionTable
(
    functionObject,
    writeIsoSurfaceTopoFunctionObject,
    dictionary
);

} // End namespace Foam


// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //

const Foam::fvMesh&
Foam::writeIsoSurfaceTopoFunctionObject::mesh() const
{
    return refCast<const fvMesh>(obr_);
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::
writeIsoSurfaceTopoFunctionObject::
writeIsoSurfaceTopoFunctionObject
(
    const word& name,
    const Time& runTime,
    const dictionary& dict
)
:
    functionObjects::regionFunctionObject(name, runTime, dict)
{
    read(dict);
}

// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

Foam::writeIsoSurfaceTopoFunctionObject::~writeIsoSurfaceTopoFunctionObject() {}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

bool Foam::writeIsoSurfaceTopoFunctionObject::read(const dictionary& dict)
{
    writeIsoSurfaceTopo(this->mesh(), true);

    return true;
}


bool Foam::writeIsoSurfaceTopoFunctionObject::execute()
{
    writeIsoSurfaceTopo(this->mesh(), false);

    return true;
}


bool Foam::writeIsoSurfaceTopoFunctionObject::write()
{
    return true;
}


bool Foam::writeIsoSurfaceTopoFunctionObject::end()
{
    writeIsoSurfaceTopo(this->mesh(), true);

    return true;
}

// ************************************************************************* //

