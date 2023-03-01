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

#include "strictNegativeSpLinearImplicit.H"
#include "fvMesh.H"
#include "addToRunTimeSelectionTable.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
namespace fv
{


defineTypeNameAndDebug(strictNegativeSpLinearImplicit, false);
addToRunTimeSelectionTable(SDPLSSourceScheme, strictNegativeSpLinearImplicit, Dictionary);

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //
strictNegativeSpLinearImplicit::strictNegativeSpLinearImplicit(const fvMesh& mesh)
    :
        SDPLSSourceScheme(mesh)
{}

// * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * * //

tmp<fvScalarMatrix> strictNegativeSpLinearImplicit::doFvmSDPLSSource(const volScalarField& psi, const volVectorField& U)
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

    const fvMesh&  mesh = psi.mesh();
    const Time&  runtime = psi.mesh().time();

    volScalarField Sc
    (
        IOobject
        (
            "Sc",
            runtime.timeName(),
            mesh,
            IOobject::NO_READ,
            IOobject::NO_WRITE
        ),
        mesh,
        psi.dimensions()/dimTime
    );
    volScalarField Sp
    (
        IOobject
        (
            "Sp",
            runtime.timeName(),
            mesh,
            IOobject::NO_READ,
            IOobject::NO_WRITE
        ),
        mesh,
        dimless/dimTime
    );

    tmp<volScalarField> tminusR = minusR(psi, U);
    const volScalarField& minusR = tminusR.cref();
    forAll(psi, cellID)
    {
        Sc[cellID] = Foam::max(minusR[cellID], 0) *psi[cellID];
        Sp[cellID] = -Foam::max(-minusR[cellID],0);
    }

    fvm.diag() = mesh.V() * Sp;
    fvm.source() = mesh.V() * Sc;
    return tfvm;
}


// ************************************************************************* //

} // End namespace fv
} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //
