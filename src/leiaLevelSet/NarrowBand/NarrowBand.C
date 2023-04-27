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

#include "NarrowBand.H"
#include "addToRunTimeSelectionTable.H"

namespace Foam
{
defineTypeNameAndDebug(NarrowBand, false);
defineRunTimeSelectionTable(NarrowBand, Dictionary);
addToRunTimeSelectionTable(NarrowBand, NarrowBand, Dictionary);

NarrowBand::NarrowBand(const dictionary& dict, const volScalarField& psi)
    :
        mesh_(psi.mesh()),
        psi_(psi)

{}

autoPtr<NarrowBand> NarrowBand::New(const fvMesh& mesh, const volScalarField& psi)
{
    const fvSolution& fvSolution (mesh);
    const dictionary& levelSetDict = fvSolution.subDict("levelSet");
    const dictionary& NarrowBandDict = levelSetDict.subOrEmptyDict("narrowBand");
    const word& type = NarrowBandDict.getOrDefault<word>("type","none");

    auto* ctorPtr = DictionaryConstructorTable(type);

    if (!ctorPtr)
    {
        FatalIOErrorInLookup
        (
            fvSolution,
            "NarrowBand",
            type,
            *DictionaryConstructorTablePtr_
        ) << exit(FatalIOError);
    }

    Info << "Selecting NarrowBand type: " << type << nl << endl;
    
    // Construct the model and return the autoPtr to the object. 
    return autoPtr<NarrowBand>(ctorPtr(NarrowBandDict, psi));
}

const fvMesh& NarrowBand::mesh() const
{
    return mesh_;
}

const volScalarField& NarrowBand::psi() const
{
    return psi_;
}

void NarrowBand::calc()
{}

void NarrowBand::write() const
{}

} // End namespace Foam

// ************************************************************************* //
