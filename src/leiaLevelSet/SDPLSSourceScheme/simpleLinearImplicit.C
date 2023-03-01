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

#include "simpleLinearImplicit.H"
#include "fvMesh.H"
#include "addToRunTimeSelectionTable.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
namespace fv
{


defineTypeNameAndDebug(simpleLinearImplicit, false);
addToRunTimeSelectionTable(SDPLSSourceScheme, simpleLinearImplicit, Dictionary);

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //
simpleLinearImplicit::simpleLinearImplicit(const fvMesh& mesh)
    :
        SDPLSSourceScheme(mesh)
{}

// * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * * //

tmp<fvScalarMatrix> simpleLinearImplicit::doFvmSDPLSSource(const volScalarField& psi, const volVectorField& U)
{
    tmp<fvScalarMatrix> tfvm
    (
        new fvScalarMatrix
        (
            psi,
            psi.dimensions()*dimVol/dimTime
        )
    );

    tfvm.ref().diag() += psi.mesh().V() * minusR(psi, U).cref();
    return tfvm;
}


// ************************************************************************* //

} // End namespace fv
} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //
