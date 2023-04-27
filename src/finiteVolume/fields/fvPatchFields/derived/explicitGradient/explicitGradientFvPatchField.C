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

#include "explicitGradientFvPatchField.H"
#include "fvCFD.H"
#include "addToRunTimeSelectionTable.H"
#include "fvPatchFieldMapper.H"

#define scalarNAN pTraits<scalar>::max

// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //


Foam::explicitGradientFvPatchField::explicitGradientFvPatchField
(
    const fvPatch& p,
    const DimensionedField<scalar, volMesh>& iF
)
:
    fvPatchField<scalar>(p, iF),
    snGrad_(p.size(), Zero)
{
}


// Foam::explicitGradientFvPatchField::explicitGradientFvPatchField
// (
//     const fvPatch& p,
//     const DimensionedField<scalar, volMesh>& iF,
//     const dictionary& dict,
//     const bool valueRequired
// )
// :
//     fvPatchField<scalar>(p, iF, dict, valueRequired),
//     snGrad_(p.size(), Zero)
// {}

Foam::explicitGradientFvPatchField::explicitGradientFvPatchField
(
    const fvPatch& p,
    const DimensionedField<scalar, volMesh>& iF,
    const dictionary& dict
)
:
    fvPatchField<scalar>(p, iF, dict, false),
    snGrad_(p.size(), Zero)
{
    Field<scalar>::operator=
    (
        this->patchInternalField() + snGrad_/this->patch().deltaCoeffs()
    );
}



Foam::explicitGradientFvPatchField::explicitGradientFvPatchField
(
    const explicitGradientFvPatchField& ptf,
    const fvPatch& p,
    const DimensionedField<scalar, volMesh>& iF,
    const fvPatchFieldMapper& mapper
)
:
    fvPatchField<scalar>(ptf, p, iF, mapper),
    snGrad_(p.size(), Zero)
{
    if (notNull(iF) && mapper.hasUnmapped())
    {
        WarningInFunction
            << "On field " << iF.name() << " patch " << p.name()
            << " patchField " << this->type()
            << " : mapper does not map all values." << nl
            << "    To avoid this warning fully specify the mapping in derived"
            << " patch fields." << endl;
    }
}



Foam::explicitGradientFvPatchField::explicitGradientFvPatchField
(
    const explicitGradientFvPatchField& ptf
)
:
    fvPatchField<scalar>(ptf),
    snGrad_(ptf.size(), Zero)
{
}



Foam::explicitGradientFvPatchField::explicitGradientFvPatchField
(
    const explicitGradientFvPatchField& ptf,
    const DimensionedField<scalar, volMesh>& iF
)
:
    fvPatchField<scalar>(ptf, iF),
    snGrad_(ptf.size(), Zero)
{
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //


tmp<Field<vector>> Foam::explicitGradientFvPatchField::calcGradient()
{
    tmp<volScalarField> tpsi = db().lookupObject<volScalarField>("psi");
    tmp<volVectorField> tgrad = fvc::grad(tpsi);
    return tgrad.cref().boundaryField()[patch().index()].patchInternalField();
}


Foam::tmp<Foam::Field<scalar>> Foam::explicitGradientFvPatchField::snGrad() const
{
    return snGrad_.clone();
}


void Foam::explicitGradientFvPatchField::evaluate(const Pstream::commsTypes)
{
    if (!this->updated())
    {
        this->updateCoeffs();
    }

    tmp<vectorField> tnf = patch().nf();
    snGrad_ = calcGradient() & tnf.cref();

    Field<scalar>::operator=
    (
        this->patchInternalField() + snGrad_/this->patch().deltaCoeffs()
    );

    fvPatchField<scalar>::evaluate();
}


Foam::tmp<Foam::Field<scalar>>
Foam::explicitGradientFvPatchField::valueInternalCoeffs
(
    const tmp<scalarField>&
) const
{
    return tmp<Field<scalar>>
    (
        new Field<scalar>(this->size(), pTraits<scalar>::zero)
    );
}



Foam::tmp<Foam::Field<scalar>>
Foam::explicitGradientFvPatchField::valueBoundaryCoeffs
(
    const tmp<scalarField>&
) const
{
    return *this;
}



Foam::tmp<Foam::Field<scalar>>
Foam::explicitGradientFvPatchField::gradientInternalCoeffs() const
{
    return tmp<Field<scalar>>
    (
        new Field<scalar>(this->size(), Zero)
    );
}



Foam::tmp<Foam::Field<scalar>>
Foam::explicitGradientFvPatchField::gradientBoundaryCoeffs() const
{
    return snGrad();
}

void Foam::explicitGradientFvPatchField::write(Ostream& os) const
{
    fvPatchField<scalar>::write(os);
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
    makePatchTypeField
    (
        fvPatchScalarField,
        explicitGradientFvPatchField
    );
}

// ************************************************************************* //
