/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2011-2016 OpenFOAM Foundation
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

#include "explicitMag1GradientFvPatchField.H"
#include "fvCFD.H"
#include "addToRunTimeSelectionTable.H"
#include "fvPatchFieldMapper.H"

// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //


Foam::explicitMag1GradientFvPatchField::explicitMag1GradientFvPatchField
(
    const fvPatch& p,
    const DimensionedField<scalar, volMesh>& iF
)
:
    explicitGradientFvPatchField(p, iF)
{}



Foam::explicitMag1GradientFvPatchField::explicitMag1GradientFvPatchField
(
    const fvPatch& p,
    const DimensionedField<scalar, volMesh>& iF,
    const dictionary& dict
)
:
    explicitGradientFvPatchField(p, iF, dict)
{}



Foam::explicitMag1GradientFvPatchField::explicitMag1GradientFvPatchField
(
    const explicitMag1GradientFvPatchField& ptf,
    const fvPatch& p,
    const DimensionedField<scalar, volMesh>& iF,
    const fvPatchFieldMapper& mapper
)
:
    explicitGradientFvPatchField(ptf, p, iF, mapper)
{}



Foam::explicitMag1GradientFvPatchField::explicitMag1GradientFvPatchField
(
    const explicitMag1GradientFvPatchField& ptf
)
:
    explicitGradientFvPatchField(ptf)
{}



Foam::explicitMag1GradientFvPatchField::explicitMag1GradientFvPatchField
(
    const explicitMag1GradientFvPatchField& ptf,
    const DimensionedField<scalar, volMesh>& iF
)
:
    explicitGradientFvPatchField(ptf, iF)
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

tmp<Field<vector>> Foam::explicitMag1GradientFvPatchField::calcGradient()
{
    tmp<Field<vector>> tgrad = explicitGradientFvPatchField::calcGradient();
    tgrad.ref().normalise();
    return tgrad;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
    makePatchTypeField
    (
        fvPatchScalarField,
        explicitMag1GradientFvPatchField
    );
}

// ************************************************************************* //
