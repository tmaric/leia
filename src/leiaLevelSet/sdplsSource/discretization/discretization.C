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

#include "discretization.H"
#include "addToRunTimeSelectionTable.H"
#include "sdplsSource.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
    defineTypeNameAndDebug(discretization, false);
    defineRunTimeSelectionTable(discretization, Dictionary);
    addToRunTimeSelectionTable(discretization, discretization, Dictionary);
}

// * * * * * * * * * * * * * * * * Selectors * * * * * * * * * * * * * * * * //


Foam::autoPtr<discretization> Foam::discretization::New(const word type)
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
    Info << "Selecting SDPLS Source discretization: " << type << nl << endl;
    return autoPtr<discretization>(ctorPtr());
}

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::discretization::discretization()
{}

// * * * * * * * * * * * * * *  Member functions  * * * * * * * * * * * * * * //

Foam::tmp<fvScalarMatrix> 
Foam::discretization::
discretize(const volScalarField& nonLinearPart, const volScalarField& psi) const
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

    fvm.source()    = mesh.V().field() * Sc(nonLinearPart, psi);
    fvm.diag()      = mesh.V().field() * Sp(nonLinearPart);
    return tfvm;
}

Foam::tmp<scalarField> 
Foam::discretization::
Sc(const volScalarField& nonLinearPart, const volScalarField& psi) const
{
    return tmp<scalarField>(new scalarField(nonLinearPart.size(), 0.0));
}

Foam::tmp<scalarField> 
Foam::discretization::Sp(const volScalarField& nonLinearPart) const
{
    return tmp<scalarField>(new scalarField(nonLinearPart.size(), 0.0));
}


bool Foam::discretization::iterative()
{
    return false;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //
