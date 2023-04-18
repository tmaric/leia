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

Class
    Foam::explicitGradientFvPatchField

Group
    grpGenericBoundaryConditions

Description

    Sets the BC gradient to the gradient of the internalField of the last time step.

    $\nabla \psi_b^{n+1} = \nabla \psi_c^{n} $

    where
    \vartable
        \nabla(x)| gradient (user-specified)
    \endvartable

Usage
    Example of the boundary condition specification:
    \verbatim
    <patchName>
    {
        type            explicitGradient;
    }
    \endverbatim

SourceFiles
    explicitGradientFvPatchField.C

\*---------------------------------------------------------------------------*/

#ifndef explicitGradientFvPatchField_H
#define explicitGradientFvPatchField_H

#include "fvPatchField.H"


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                 Class explicitGradientFvPatchField Declaration
\*---------------------------------------------------------------------------*/


class explicitGradientFvPatchField
:
    public fvPatchField<scalar>
{
    // Private data
        Field<scalar> snGrad_;

public:

    //- Runtime type information
    TypeName("explicitGradient");


    // Constructors

        //- Construct from patch and internal field
        explicitGradientFvPatchField
        (
            const fvPatch&,
            const DimensionedField<scalar, volMesh>&
        );

        //- Construct from patch, internal field and dictionary
        explicitGradientFvPatchField
        (
            const fvPatch&,
            const DimensionedField<scalar, volMesh>&,
            const dictionary&
        );
        
        // explicitGradientFvPatchField
        // (
        //     const fvPatch& p,
        //     const DimensionedField<scalar, volMesh>& iF,
        //     const dictionary& dict,
        //     const bool valueRequired
        // );

        //- Construct by mapping the given explicitGradientFvPatchField
        //  onto a new patch
        explicitGradientFvPatchField
        (
            const explicitGradientFvPatchField&,
            const fvPatch&,
            const DimensionedField<scalar, volMesh>&,
            const fvPatchFieldMapper&
        );

        //- Construct as copy
        explicitGradientFvPatchField
        (
            const explicitGradientFvPatchField&
        );

        //- Construct and return a clone
        virtual tmp<fvPatchField<scalar>> clone() const
        {
            return tmp<fvPatchField<scalar>>
            (
                new explicitGradientFvPatchField(*this)
            );
        }

        //- Construct as copy setting internal field reference
        explicitGradientFvPatchField
        (
            const explicitGradientFvPatchField&,
            const DimensionedField<scalar, volMesh>&
        );

        //- Construct and return a clone setting internal field reference
        virtual tmp<fvPatchField<scalar>> clone
        (
            const DimensionedField<scalar, volMesh>& iF
        ) const
        {
            return tmp<fvPatchField<scalar>>
            (
                new explicitGradientFvPatchField(*this, iF)
            );
        }


    // Member functions

        // Return defining fields

            // //- Return gradient at boundary
            // virtual Field<scalar>& gradient()
            // {
            //     return snGrad_;
            // }

            // virtual const Field<scalar>& gradient() const
            // {
            //     return snGrad_;
            // }

        // Evaluation functions

            //- Return gradient at boundary
            virtual tmp<Field<scalar>> snGrad() const;

            virtual tmp<Field<vector>> calcGradient();

            //- Return the matrix diagonal coefficients corresponding to the
            //  evaluation of the value of this patchField with given weights
            virtual tmp<Field<scalar>> valueInternalCoeffs
            (
                const tmp<scalarField>&
            ) const;

            //- Return the matrix source coefficients corresponding to the
            //  evaluation of the value of this patchField with given weights
            virtual tmp<Field<scalar>> valueBoundaryCoeffs
            (
                const tmp<scalarField>&
            ) const;

            //- Return the matrix diagonal coefficients corresponding to the
            //  evaluation of the gradient of this patchField
            virtual tmp<Field<scalar>> gradientInternalCoeffs() const;

            //- Return the matrix source coefficients corresponding to the
            //  evaluation of the gradient of this patchField
            virtual tmp<Field<scalar>> gradientBoundaryCoeffs() const;

            //- Evaluate the patch field
            virtual void evaluate
            (
                const Pstream::commsTypes commsType =
                    Pstream::commsTypes::blocking
            );

            virtual void write(Ostream& os) const;
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
