/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2022 AUTHOR,AFFILIATION
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

#include "GradPsi.H"
#include "addToRunTimeSelectionTable.H"
// #include "fvSolution.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
// namespace fv
// {

defineTypeNameAndDebug(GradPsi, false);
defineRunTimeSelectionTable(GradPsi, Dictionary);
addToRunTimeSelectionTable(GradPsi, GradPsi, Dictionary);

// * * * * * * * * * * * * * * * * Selectors * * * * * * * * * * * * * * * * //


Foam::autoPtr<GradPsi>
GradPsi::New(const word type, const fvMesh& mesh)
{
    auto* ctorPtr = DictionaryConstructorTable(type);

    if (!ctorPtr)
    {
        FatalErrorInFunction
        << "Unknown GradPsi type \"" << type << "\"\n\n"
        << "Valid write types : "
        << flatOutput(DictionaryConstructorTablePtr_->sortedToc()) << nl
        << exit(FatalError);
    }
    Info << "Selecting SDPLS Source GradPsi type: " << type << nl << endl;
    return autoPtr<GradPsi>(ctorPtr(mesh));
}

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

GradPsi::GradPsi(const fvMesh& mesh)
{}

// * * * * * * * * * * * * * *  Member functions  * * * * * * * * * * * * * * //

tmp<volVectorField> GradPsi::grad(const volScalarField& psi) const
{
    return fvc::grad(psi);
}

// tmp<volVectorField> GradPsi::grad(const volScalarField& psi) const
// {
//     const volScalarField& narowBand = mesh_.lookupObject("narrowBand");
//     const volVectorField& nc = mesh_.lookupObject("nc");
//     tmp<volVectorField> tgradpsi = fvc::grad(psi);
//     forAll(narowBand, cellID)
//     {
//         if (narowBand[cellID] == 1)
//         {
//             tgradpsi.ref()[cellID] = nc[cellID];
//         }
//     }
//     return tgradpsi;
// }



// } // End namespace fv

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //
