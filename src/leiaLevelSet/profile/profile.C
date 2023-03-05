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

#include "profile.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

defineTypeNameAndDebug(profile, false);
defineRunTimeSelectionTable(profile, Mesh);


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

profile::profile(const fvMesh& mesh, const implicitSurface& surface)
    :
        fvSolution_(mesh),
        levelSetDict_(fvSolution_.subDict("levelSet")),
        profileDict_(levelSetDict_.subDict("profile")),
        surface_(surface)
{}

// * * * * * * * * * * * * * * * * Selectors * * * * * * * * * * * * * * * * //

autoPtr<Foam::profile> profile::New(const fvMesh& mesh, const implicitSurface& surface)
{
    const fvSolution& fvSolution (mesh);
    const dictionary& levelSetDict = fvSolution.subDict("levelSet");
    const dictionary& profileDict = levelSetDict.subDict("profile");
    const word& modelType = profileDict.getOrDefault<word>("type", "signedDistance");
    auto* ctorPtr = MeshConstructorTable(modelType);

    if (!ctorPtr)
    {
        FatalIOErrorInLookup
        (
            fvSolution,
            "profile",
            modelType,
            *MeshConstructorTablePtr_
        ) << exit(FatalIOError);
    }
    Info << "Selecting level-set profile:" << modelType << nl << endl;
    // Construct the model and return the autoPtr to the object. 
    return autoPtr<profile>(ctorPtr(mesh, surface));
}

// ************************************************************************* //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //
