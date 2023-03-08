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
    Foam::explicitMag1GradientFvPatchField

Group
    grpGenericBoundaryConditions

Description

    Sets the BC gradient to the normalized gradient of the internalField of the last time step.

    $\nabla \psi_b^{n+1} = \frac{\nabla \psi_c^{n}}{|\nabla \psi_c^{n}|} $

    where
    \vartable
        \nabla(x)| gradient (user-specified)
    \endvartable

Usage
    Example of the boundary condition specification:
    \verbatim
    <patchName>
    {
        type            explicitMag1Gradient;
    }
    \endverbatim

SourceFiles
    explicitMag1GradientFvPatchField.C

\*---------------------------------------------------------------------------*/

#ifndef explicitMag1GradientFvPatchField_H
#define explicitMag1GradientFvPatchField_H

#include "explicitGradientFvPatchField.H"


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                 Class explicitMag1GradientFvPatchField Declaration
\*---------------------------------------------------------------------------*/


class explicitMag1GradientFvPatchField
:
    public explicitGradientFvPatchField
{
    // Private data


public:

    //- Runtime type information
    TypeName("explicitMag1Gradient");


    // Constructors

        //- Construct from patch and internal field
        explicitMag1GradientFvPatchField
        (
            const fvPatch&,
            const DimensionedField<scalar, volMesh>&
        );

        //- Construct from patch, internal field and dictionary
        explicitMag1GradientFvPatchField
        (
            const fvPatch&,
            const DimensionedField<scalar, volMesh>&,
            const dictionary&
        );

        //- Construct by mapping the given explicitMag1GradientFvPatchField
        //  onto a new patch
        explicitMag1GradientFvPatchField
        (
            const explicitMag1GradientFvPatchField&,
            const fvPatch&,
            const DimensionedField<scalar, volMesh>&,
            const fvPatchFieldMapper&
        );

        //- Construct as copy
        explicitMag1GradientFvPatchField
        (
            const explicitMag1GradientFvPatchField&
        );

        //- Construct and return a clone
        virtual tmp<fvPatchField<scalar>> clone() const
        {
            return tmp<fvPatchField<scalar>>
            (
                new explicitMag1GradientFvPatchField(*this)
            );
        }

        //- Construct as copy setting internal field reference
        explicitMag1GradientFvPatchField
        (
            const explicitMag1GradientFvPatchField&,
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
                new explicitMag1GradientFvPatchField(*this, iF)
            );
        }


    // Member functions
     
        virtual tmp<Field<vector>> calcGradient();
     
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
