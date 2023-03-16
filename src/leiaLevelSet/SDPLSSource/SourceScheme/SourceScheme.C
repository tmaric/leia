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

#include "SourceScheme.H"
#include "addToRunTimeSelectionTable.H"
#include "SDPLSSource.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
// namespace fv
// {

defineTypeNameAndDebug(SourceScheme, false);
defineRunTimeSelectionTable(SourceScheme, Dictionary);
addToRunTimeSelectionTable(SourceScheme, SourceScheme, Dictionary);

// * * * * * * * * * * * * * * * * Selectors * * * * * * * * * * * * * * * * //


Foam::autoPtr<SourceScheme>
SourceScheme::New(const word type)
{
    auto* ctorPtr = DictionaryConstructorTable(type);

    if (!ctorPtr)
    {
        FatalErrorInFunction
        << "Unknown discretization type \"" << type << "\"\n\n"
        << "Valid write types : "
        << flatOutput(DictionaryConstructorTablePtr_->sortedToc()) << nl
        << exit(FatalError);
    }
    Info << "Selecting Source discretization: " << type << nl << endl;
    return autoPtr<SourceScheme>(ctorPtr());
}

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

SourceScheme::SourceScheme()
    :
        Sc_(zeroField()),
        Sp_(zeroField())
{}

// * * * * * * * * * * * * * *  Member functions  * * * * * * * * * * * * * * //

tmp<fvScalarMatrix> SourceScheme::discretize(const volScalarField& nonLinearPart, const volScalarField& psi)
{
    tmp<fvScalarMatrix> tfvm
    (
        new fvScalarMatrix
        (
            psi,
            psi.dimensions()*dimVol/dimTime
        )
    );

    fvScalarMatrix& fvm = tfvm.ref();
    const fvMesh& mesh = psi.mesh();

    updateSc(nonLinearPart, psi);
    updateSp(nonLinearPart);

    fvm.source()    = mesh.V().field() * Sc_;
    fvm.diag()      = mesh.V().field() * Sp_;
    return tfvm;
}

void SourceScheme::updateSc(const volScalarField& nonLinearPart, const volScalarField& psi)
{
    Sc_ = scalarField(nonLinearPart.size(), 0.0);
}

void SourceScheme::updateSp(const volScalarField& nonLinearPart)
{
    Sp_ = scalarField(nonLinearPart.size(), 0.0);
}


// } // End namespace fv

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //
