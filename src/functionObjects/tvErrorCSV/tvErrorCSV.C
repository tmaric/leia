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


#include "tvErrorCSV.H"
#include "addToRunTimeSelectionTable.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
namespace functionObjects
{
    defineTypeNameAndDebug(tvErrorCSV, 0);
    addToRunTimeSelectionTable(functionObject, tvErrorCSV, dictionary);
}
}



// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::functionObjects::tvErrorCSV::tvErrorCSV
(
    const word& name,
    const Time& runTime,
    const dictionary& dict
)
:
    fvMeshFunctionObject (name, runTime, dict),
    field_(mesh_.lookupObject<volScalarField>(dict.get<word>("field"))),
    TV_initial_(0.0),
    csvFile_(fileName("tvError.csv"),  IOstreamOption(), IOstreamOption::appendType::APPEND)
{
    if ( !fileSize("tvError.csv") && Pstream::myProcNo() == 0)
    {
            // CSV Header 
            csvFile_ << "TIME,"
                << "TV,"
                << "E_TV,"
                << "E_TV_REL\n";
    }
    TV_initial_ = calcTV();
    Info << "TotalVariation initial value: " << TV_initial_ << nl << endl;

    write();
}

bool Foam::functionObjects::tvErrorCSV::write()
{
    const fvMesh& mesh = this->mesh_;
    const Time& runTime = mesh.time();

    scalar TV = calcTV();
    scalar E_TV = TV - TV_initial_; // - E_ABS
    scalar E_TV_REL = (TV - TV_initial_) / TV_initial_; // - E_REL 

    if (Pstream::myProcNo() == 0)
    {
        csvFile_ << runTime.timeOutputValue() << ","
            << TV << ","
            << E_TV << ","
            << E_TV_REL << "\n";
    }
    return true;
}

scalar Foam::functionObjects::tvErrorCSV::calcTV()
{
    const auto& own = mesh_.owner();  
    const auto& nei = mesh_.neighbour(); 
    scalar TV = 0.0;
    forAll(nei, faceI)
    {
        TV += mag(field_[nei[faceI]] - field_[own[faceI]]);
    }

    // TV across coupled process boundaries. 
    const auto& boundaryField = field_.boundaryField(); 
    const auto& patches = mesh_.boundary(); 
    const auto& faceOwner = mesh_.faceOwner();
    forAll(patches, patchI)
    {
        const fvPatch& patch = patches[patchI];
        if (isA<coupledFvPatch>(patch)) // coupled patch 
        {
            const auto& patchField = boundaryField[patchI]; 
            auto patchNeiFieldTmp = patchField.patchNeighbourField();
            const auto& patchNeiField = patchNeiFieldTmp();
            forAll(patchNeiField, faceI)
            {
                label faceJ = faceI + patch.start(); // Global face label.
                TV += mag(patchNeiField[faceJ] - field_[faceOwner[faceJ]]);
            }
        }
    }


    return TV;
}

// ************************************************************************* //
