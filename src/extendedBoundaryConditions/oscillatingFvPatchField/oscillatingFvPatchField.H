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

Class
    Foam::oscillatingFvPatchField

Description

SourceFiles
    oscillatingFvPatchField.C

\*---------------------------------------------------------------------------*/

#include "fvPatchField.H"
#include "tmp.H"

#ifndef oscillatingFvPatchField_H
#define oscillatingFvPatchField_H

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                        Class oscillatingFvPatchField Declaration
\*---------------------------------------------------------------------------*/

template<class Type>
class oscillatingFvPatchField
:
    public fvPatchField<Type>
{
    // Wrapped boundary condition.
    tmp<fvPatchField<Type> > wrappedTmp_; 

    // Initial wrapped boundary condition.
    tmp<fvPatchField<Type> > wrappedInitialTmp_; 

public:

    //- Runtime type information
    TypeName("oscillating");

    // Constructors

    //- Construct from patch and internal field
    oscillatingFvPatchField
    (
        const fvPatch&,
        const DimensionedField<Type, volMesh>&
    );

    //- Construct from patch and internal field and patch type
    oscillatingFvPatchField
    (
        const fvPatch&,
        const DimensionedField<Type, volMesh>&,
        const word& patchType
    );

    //- Construct from patch and internal field and patch field
    oscillatingFvPatchField
    (
        const fvPatch&,
        const DimensionedField<Type, volMesh>&,
        const Field<Type>&
    );

    //- Construct from patch, internal field and dictionary
    oscillatingFvPatchField
    (
        const fvPatch&,
        const DimensionedField<Type, volMesh>&,
        const dictionary&,
        const bool valueRequired=false
    );

    //- Construct by mapping the given oscillatingFvPatchField onto a new patch
    oscillatingFvPatchField
    (
        const oscillatingFvPatchField<Type>&,
        const fvPatch&,
        const DimensionedField<Type, volMesh>&,
        const fvPatchFieldMapper&
    );

    //- Construct as copy
    oscillatingFvPatchField(const oscillatingFvPatchField<Type>&);

    //- Construct as copy setting internal field reference
    oscillatingFvPatchField
    (
        const oscillatingFvPatchField<Type>&,
        const DimensionedField<Type, volMesh>&
    );

    //- Destructor
    virtual ~oscillatingFvPatchField<Type>() {}

    // Member functions
    virtual void write(Ostream&) const;
    
    //- Update the coefficients associated with the patch field
    virtual void updateCoeffs();

    //- Return the matrix diagonal coefficients corresponding to the
    //  evaluation of the value of this patchField with given weights
    virtual tmp<Field<Type> > valueInternalCoeffs
    (
        const tmp<scalarField>&
    ) const;

    //- Return the matrix source coefficients corresponding to the
    //  evaluation of the value of this patchField with given weights
    virtual tmp<Field<Type> > valueBoundaryCoeffs
    (
        const tmp<scalarField>&
    ) const;

    //- Return the matrix diagonal coefficients corresponding to the
    //  evaluation of the gradient of this patchField
    virtual tmp<Field<Type> > gradientInternalCoeffs() const;

    //- Return the matrix source coefficients corresponding to the
    //  evaluation of the gradient of this patchField
    virtual tmp<Field<Type> > gradientBoundaryCoeffs() const;

};

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#ifdef NoRepository
#   include "oscillatingFvPatchField.C"
#endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
