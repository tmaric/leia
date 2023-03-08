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

#include "interfaceBand.H"
#include "addToRunTimeSelectionTable.H"

namespace Foam
{
defineTypeNameAndDebug(interfaceBand, false);
defineRunTimeSelectionTable(interfaceBand, Mesh);
addToRunTimeSelectionTable(interfaceBand, interfaceBand, Mesh);

interfaceBand::interfaceBand(const fvMesh& mesh, const volScalarField& psi)
    :
        mesh_(mesh),
        interfaceBandField_(
            IOobject(
                "interfaceBand",
                mesh.time().timeName(),
                mesh,
                IOobject::NO_READ,
                IOobject::AUTO_WRITE
                ), 
            mesh, 
            dimensioned(dimless, 0.)
        ),
        psi_(psi)
{}

autoPtr<interfaceBand> interfaceBand::New(const fvMesh& mesh, const volScalarField& psi)
{
    const fvSolution& fvSolution (mesh);
    const dictionary& levelSetDict = fvSolution.subDict("levelSet");
    const word& interfaceBandType = levelSetDict.getOrDefault<word>("interfaceBand", "none");

    auto* ctorPtr = MeshConstructorTable(interfaceBandType);

    if (!ctorPtr)
    {
        FatalIOErrorInLookup
        (
            fvSolution,
            "interfaceBand",
            interfaceBandType,
            *MeshConstructorTablePtr_
        ) << exit(FatalIOError);
    }

    // Construct the model and return the autoPtr to the object. 
    return autoPtr<interfaceBand>(ctorPtr(mesh, psi));
}

} // End namespace Foam

// ************************************************************************* //
