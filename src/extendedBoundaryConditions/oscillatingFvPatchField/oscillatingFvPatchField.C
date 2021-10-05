/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | Copyright (C) 2011-2013 OpenFOAM Foundation
     \\/     M anipulation  |
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

#include "Time.H"

namespace Foam {

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

template<class Type>
oscillatingFvPatchField<Type>::oscillatingFvPatchField
(
    const fvPatch& p,
    const DimensionedField<Type, volMesh>& iF
)
:
    fvPatchField<Type>(p, iF), 
    wrappedTmp_(new fvPatchField<Type>(p, iF)), 
    wrappedInitialTmp_(new fvPatchField<Type>(p, iF))
{}


template<class Type>
oscillatingFvPatchField<Type>::oscillatingFvPatchField
(
    const fvPatch& p,
    const DimensionedField<Type, volMesh>& iF,
    const word& patchType
)
:
    fvPatchField<Type>(p, iF, patchType),
    wrappedTmp_(new fvPatchField<Type>(p, iF, patchType)), 
    wrappedInitialTmp_(new fvPatchField<Type>(p, iF, patchType))
{}


template<class Type>
oscillatingFvPatchField<Type>::oscillatingFvPatchField
(
    const fvPatch& p,
    const DimensionedField<Type, volMesh>& iF,
    const Field<Type>& f
)
:
    fvPatchField<Type>(p, iF, f),
    wrappedTmp_(new fvPatchField<Type>(p, iF, f)),
    wrappedInitialTmp_(new fvPatchField<Type>(p, iF, f))
{}


template<class Type>
oscillatingFvPatchField<Type>::oscillatingFvPatchField
(
    const oscillatingFvPatchField<Type>& ptf,
    const fvPatch& p,
    const DimensionedField<Type, volMesh>& iF,
    const fvPatchFieldMapper& mapper
)
:
    fvPatchField<Type>(ptf, p, iF, mapper),
    wrappedTmp_(new fvPatchField<Type>(ptf, p, iF, mapper)),
    wrappedInitialTmp_(new fvPatchField<Type>(p, iF))
{}

template<class Type>
oscillatingFvPatchField<Type>::oscillatingFvPatchField
(
    const fvPatch& p,
    const DimensionedField<Type, volMesh>& iF,
    const dictionary& dict,
    const bool valueRequired
)
:
    fvPatchField<Type>(p, iF, dict, valueRequired), 
    wrappedTmp_
    (
        fvPatchField<Type>::New
        (
            p, 
            iF, 
            dict.subDict("wrappedType")
        )
    ),
    wrappedInitialTmp_
    (
        fvPatchField<Type>::New
        (
            p, 
            iF, 
            dict.subDict("wrappedType")
        )
    )
{} 

template<class Type>
oscillatingFvPatchField<Type>::oscillatingFvPatchField
(
    const oscillatingFvPatchField<Type>& ptf
)
:
    fvPatchField<Type>(ptf), 
    wrappedTmp_(ptf.wrappedTmp_), 
    wrappedInitialTmp_(ptf.wrappedInitialTmp_)
{}

template<class Type>
oscillatingFvPatchField<Type>::oscillatingFvPatchField
(
    const oscillatingFvPatchField<Type>& ptf,
    const DimensionedField<Type, volMesh>& iF
)
:
    fvPatchField<Type>(ptf,iF), 
    wrappedTmp_(new fvPatchField<Type>(ptf, iF)), 
    wrappedInitialTmp_(new fvPatchField<Type>(ptf, iF))
{}

// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<class Type>
void oscillatingFvPatchField<Type>::write(Ostream& os) const
{
    wrappedTmp_->write(os);  
}

template<class Type>
void oscillatingFvPatchField<Type>::updateCoeffs()
{
    if (this->updated())
    {
        return;
    }

    // Update the wrapped BC values. 
    wrappedTmp_->updateCoeffs(); 

    Field<Type>& wrappedInitialField = wrappedInitialTmp_.ref(); 
    Field<Type>& wrappedField = wrappedTmp_.ref(); 

    const Time& runTime = this->db().time(); 
    if (runTime.timeIndex() == 1)
        wrappedInitialField = wrappedField;

    // Modify the values of the wrapped BC.
    wrappedField = wrappedInitialField * cos(runTime.timeOutputValue() * M_PI); 

    // Assign the new field values of the wrapped BC to the decorator BC.
    fvPatchField<Type>::operator==(wrappedField);
}

template<class Type>
tmp<Field<Type> > 
oscillatingFvPatchField<Type>::valueInternalCoeffs
(
    const tmp<scalarField>& tsf
) const
{
    return wrappedTmp_->valueInternalCoeffs(tsf);
}


template<class Type>
tmp<Field<Type> > 
oscillatingFvPatchField<Type>::valueBoundaryCoeffs
(
    const tmp<scalarField>& tsf
) const
{
    return wrappedTmp_->valueBoundaryCoeffs(tsf); 
}

template<class Type>
tmp<Field<Type> > 
oscillatingFvPatchField<Type>::gradientInternalCoeffs() const
{
    return wrappedTmp_->gradientInternalCoeffs(); 
}

template<class Type>
tmp<Field<Type> > 
oscillatingFvPatchField<Type>::gradientBoundaryCoeffs() const
{
    return wrappedTmp_->gradientBoundaryCoeffs(); 
}

// ************************************************************************* //

} // End namespace Foam 
