/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2021 AUTHOR,AFFILIATION
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

#include "tanh.H"
#include "profile.H"
#include "addToRunTimeSelectionTable.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

defineTypeNameAndDebug(tanhProfile, false);
addToRunTimeSelectionTable(profile, tanhProfile, Mesh);


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

tanhProfile::tanhProfile(const fvMesh& mesh, const implicitSurface& surface)
    :
        signedDistance(mesh, surface),
        eps_(profileDict_.getOrDefault<scalar>("eps", 1)),
        limit_(profileDict_.getOrDefault<scalar>("limit", 1))
{}

scalar tanhProfile::value(const vector point) const
{
    return limit_ * Foam::tanh(1/limit_ * signedDistance::value(point)/eps_);
}

// ************************************************************************* //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //
