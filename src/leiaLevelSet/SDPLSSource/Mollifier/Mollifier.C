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

#include "Mollifier.H"
#include "addToRunTimeSelectionTable.H"
// #include "fvSolution.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
// namespace fv
// {

defineTypeNameAndDebug(Mollifier, false);
defineRunTimeSelectionTable(Mollifier, Dictionary);
addToRunTimeSelectionTable(Mollifier, Mollifier, Dictionary);

// * * * * * * * * * * * * * * * * Selectors * * * * * * * * * * * * * * * * //


Foam::autoPtr<Mollifier>
Mollifier::New(const dictionary& dict)
{
    const word& type = dict.getOrDefault<word>("type", "none");

    auto* ctorPtr = DictionaryConstructorTable(type);

    if (!ctorPtr)
    {
        FatalIOErrorInLookup
        (
            dict,
            "Mollifier",
            type,
            *DictionaryConstructorTablePtr_
        ) << exit(FatalIOError);
    }

    Info << "Selecting SDPLS source mollifier: " << type << nl << endl;
    return autoPtr<Mollifier>(ctorPtr(dict));
}

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Mollifier::Mollifier(const dictionary& dict)
{}

// * * * * * * * * * * * * * *  Member functions  * * * * * * * * * * * * * * //

tmp<volScalarField> Mollifier::field(const volScalarField& psi) const
{
    tmp<volScalarField> tfield
    (
        new volScalarField
        (
            IOobject
            (
                word(),
                fileName(),
                psi.mesh(),
                IOobject::NO_READ,
                IOobject::NO_WRITE,
                false
            ),
            psi.mesh(),
            dimensionedScalar(dimless, 0.0)
        )
    );

    forAll(tfield.cref(), cellID)
    {
        tfield.ref()[cellID] = mollify(psi[cellID]);
    }

    return tfield;
}


double Mollifier::mollify(double x) const
{
    return 1.0;
}



// } // End namespace fv

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //
