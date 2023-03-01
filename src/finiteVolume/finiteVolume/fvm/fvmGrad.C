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


Description

SourceFiles
    fvmGrad.C


\*---------------------------------------------------------------------------*/

#include "fvmGrad.H"
#include "gaussGrad.H"
#include "fvMesh.H"
#include "linear.H"

// #include "fvCFD.H"
// #include "tensor.H"
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

namespace fvm
{
// Code modified from [310c23]: / src / finiteVolume / finiteVolume / fvm / fvmGrad.C gaussGrad<scalar>::fvmGrad
tmp<fvMatrix<vector> > grad(const GeometricField<scalar, fvPatchField, volMesh>& vf)
{
    const fvMesh& mesh = vf.mesh();

    // Construct linear interpolation object
    tmp<surfaceInterpolationScheme<scalar>> tinterpScheme = linear<scalar>(mesh);

    tmp<surfaceScalarField> tweights = tinterpScheme().weights(vf);
    const scalarField& wIn = tweights().internalField();

    // Problem can't init VectorMatrix from ScalarField
    tmp<fvMatrix<vector>> tfvm
    (
        new fvMatrix<vector>
        (
            vf,
            vf.dimensions()/dimLength
        )
    );
    fvMatrix<vector>& fvm = tfvm.ref();
    vectorField& source = fm.source();

    // Grab ldu parts of block matrix as linear always
    Field<vector>& d = fm.diag();
    Field<vector>& u = fm.upper();
    Field<vector>& l = fm.lower();

    const vectorField& SfIn = mesh.Sf().internalField();

    l = -wIn*SfIn;
    u = l + SfIn;
    fm.negSumDiag();

    // Boundary contributions
    forAll (vf.boundaryField(), patchI)
    {
        const fvPatchScalarField& pf = vf.boundaryField()[patchI];
        const fvPatch& patch = pf.patch();
        const vectorField& pSf = patch.Sf();
        const fvsPatchScalarField& pw = tweights().boundaryField()[patchI];
        const labelList& fc = patch.faceCells();

        const scalarField internalCoeffs(pf.valueInternalCoeffs(pw));

        // Diag contribution
        forAll (pf, faceI)
        {
            d[fc[faceI]] += internalCoeffs[faceI]*pSf[faceI];
        }

        if (patch.coupled())
        {
            CoeffField<vector>::linearTypeField& pcoupleUpper =
                fm.coupleUpper()[patchI].asLinear();
            CoeffField<vector>::linearTypeField& pcoupleLower =
                fm.coupleLower()[patchI].asLinear();

            const vectorField pcl = -pw*pSf;
            const vectorField pcu = pcl + pSf;

            // Coupling  contributions
            pcoupleLower -= pcl;
            pcoupleUpper -= pcu;
        }
        else
        {
            const scalarField boundaryCoeffs(pf.valueBoundaryCoeffs(pw));

            // Boundary contribution
            forAll (pf, faceI)
            {
                source[fc[faceI]] -= boundaryCoeffs[faceI]*pSf[faceI];
            }
        }
    }

    // Interpolation schemes with corrections not accounted for

    return tfm;
}


/* Attempt 1. to implement fvm::grad()
Idea: grad(psi) = div(psi*1), where 1 is the unit tensor with dim=2
Problem:
    OF div schemes accept div(surfaceScalarField flux, GeometricField<Type, volMesh> vf) and returns fvMesh<Type>
    Our grad(psi) is of Type Vector, psi of Type Scalar, and 1 is of Type Tensor. 
    So we cann not call fvm::div provided by OF, because we can not get an appropriate return Type, 
    even with interpolation of some property to an surfaceField
// tmp<fvMatrix<Vector<double>>>
void
grad
(
    const volScalarField& vf
)
{
    volTensorField I //identity matrix field
    (
    IOobject
    (
         "I",
         vf.mesh().time().timeName(),
         vf.mesh(),
         IOobject::NO_READ,
         IOobject::NO_WRITE
    ),
    vf.mesh(),
    tensor::one
    );
    // I = tensor::one;
    
    // const surfaceScalarField sf = fvc::interpolate(vf, "linear").cref();
    fvVectorMatrix tmp = fvm::div(vf.mesh().Sf(), vf).ref();
}*/

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace fvm

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //



// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //


// ************************************************************************* //
