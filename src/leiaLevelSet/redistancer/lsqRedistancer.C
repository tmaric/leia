/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2021 AUTHOR,AFFILIATION
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

#include "lsqRedistancer.H"
#include "addToRunTimeSelectionTable.H"
#include "volFields.H"
#include "surfaceFields.H"
#include "fvcGrad.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

defineTypeNameAndDebug(lsqRedistancer, false);
addToRunTimeSelectionTable(redistancer, lsqRedistancer, dictionary);

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

lsqRedistancer::lsqRedistancer()
{}

lsqRedistancer::lsqRedistancer(const dictionary& dict)
{}

// * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * * //

void lsqRedistancer::redistance
(
    volScalarField& psi
) 
{
    Info << "Linear Least Squares Redistancing, field: "  << psi.name() << endl;

    volVectorField nc = fvc::grad(psi);
    //nc /= Foam::mag(nc);

    volScalarField psiRedist ("psiRedist", psi); 

    psiRedist = Foam::sign(psiRedist) * Foam::mag(psiRedist * nc / (nc & nc));
    psiRedist.correctBoundaryConditions();

    // Redistance only in the narrow band around the interface. 
    // -Mesh addressing
    
    const fvMesh& mesh = psi.mesh();
    const auto& own = mesh.owner();
    const auto& nei = mesh.neighbour();

    forAll(own, faceI)
    {
        scalar ownPsi = psi[own[faceI]];
        scalar neiPsi = psi[nei[faceI]];
        if ((ownPsi * neiPsi) < 0) 
        {
            psi[own[faceI]] = min(psi[own[faceI]], psiRedist[own[faceI]]);
            psi[nei[faceI]] = min(psi[nei[faceI]], psiRedist[nei[faceI]]);
        }
    }


    /*

    // TODO: performance improvement, only apply re-distancing in the narrow band
    //      - Select iso-surface cells and immediate neighbors using phi_Of,phi_Nf 
    
    // Compute nc 
    // TODO: move to attributes 
    volVectorField nc = fvc::grad(psi);
    nc /= Foam::mag(nc);
    
    // Compute d_c 

    // TODO: move to attributes
    const auto& runTime = mesh.time();
    volScalarField dc 
    (
        IOobject
        (
            "dc",
            runTime.timeName(), 
            mesh,
            IOobject::NO_READ,
            IOobject::NO_WRITE
        ),
        mesh,
        dimensionedScalar ("dc", dimLength, 0)
    );

    const volVectorField& xc = mesh.C();
    scalarField Cc (mesh.nCells(), 0);
    forAll(psi, cellI)
    {
        dc[cellI] -= Foam::dot(nc[cellI], xc[cellI]) - psi[cellI];
        Cc[cellI] += 1.0;

    }
    //forAll(own, faceI)
    //{
        //dc[own[faceI]] -= Foam::dot(nc[nei[faceI]], xc[nei[faceI]]) - psi[nei[faceI]];
        //Cc[own[faceI]] += 1.0;

        //dc[nei[faceI]] -= Foam::dot(nc[own[faceI]], xc[own[faceI]]) - psi[own[faceI]];
        //Cc[nei[faceI]] += 1.0;
    //}
    scalarField& dcInternal (dc);
    dcInternal /= Cc;
    
    // Redistance to n_c \cdot x_c + d_c.
    psi == (nc & xc) + dc;
    //psi.correctBoundaryConditions();
    */
}

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

// ************************************************************************* //
