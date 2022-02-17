/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2021 Tomislav Maric, TU Darmstadt
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

#include "geometricPhaseIndicator.H"
#include "addToRunTimeSelectionTable.H"
#include "processorFvPatch.H"
#include "volFields.H"
#include "surfaceFields.H"
#include "fvcGrad.H"
#include "levelSetImplicitSurfaces.H"
#include "foamGeometry.H"
#include "simpleMatrix.H"
#include "processorFvPatch.H"


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

defineTypeNameAndDebug(geometricPhaseIndicator, false);
addToRunTimeSelectionTable(phaseIndicator, geometricPhaseIndicator, Dictionary);

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

geometricPhaseIndicator::geometricPhaseIndicator()
{}

geometricPhaseIndicator::geometricPhaseIndicator(const dictionary& dict)
{}

// * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * * //

void geometricPhaseIndicator::calcPhaseIndicator
(
    volScalarField& alpha, 
    const volScalarField& psi 
) 
{
    alpha == dimensionedScalar("0", dimless, 0);
    const fvMesh& mesh = alpha.mesh();

    // Set phase-indicator to 1 and 0 in the bulk.
    forAll(alpha, cellID)
    {
        if (psi[cellID] < 0)
            alpha[cellID] = 1;
        else 
            alpha[cellID] = 0;
    }

    // Select the cells in the narrow band.
    const auto& own = mesh.owner();  
    const auto& nei = mesh.neighbour(); 
    volScalarField narrowBand("narrowBand", psi);  
    forAll(own, faceI)
    {
        // FIXME: If a cell is clipped by the interface at one of its corners,
        // and the surrounding cell centers do not switch the iso-sign, it 
        // may not be picked up by this. May cause numerical inconsistency. TM. 
        if (psi[own[faceI]] * psi[nei[faceI]] < 0)
        {
            narrowBand[own[faceI]] = 1;
            narrowBand[nei[faceI]] = 1;
        }
    }
    // Set narrow band values across MPI process boundaries. 
    const auto& psiBdryField = psi.boundaryField(); // needed for Nc LLSQ contribs
    const auto& patches = mesh.boundary(); // needed for Nc LLSQ contribs
    const auto& faceOwner = mesh.faceOwner();
    forAll(psiBdryField, patchI)
    {
        const fvPatch& patch = patches[patchI];
        if (isA<processorFvPatch>(patch)) // MPI patch 
        {
            const auto& psiPatchField = psiBdryField[patchI]; 
            auto psiPatchNeiFieldTmp = 
                psiPatchField.patchNeighbourField();
            const auto& psiPatchNeiField = psiPatchNeiFieldTmp();
            forAll(psiPatchNeiField, faceI)
            {
                label faceJ = faceI + patch.start(); // Global face label.
                if (psi[faceOwner[faceJ]] * psiPatchNeiField[faceI] < 0)
                {
                    narrowBand[faceOwner[faceJ]] = 1;
                }
            }
        }
    }
    //Debugging, remove
    narrowBand.write();

    // ** An alternative simple second-order accurate extrapolation-based linear 
    // approximation of psi.** 
    // - Using \nabla \psi for the normal approximation is succeptible to small 
    //   oscillations in \psi, this is only OK for the first estimate. TM. 
    // Debugging, comment out.
    volVectorField nc ("nc", fvc::grad(psi));
    // volScalarField dc ("dc", psi - (nc & mesh.C()));
    
    // A LLSQ approximation of psi 
    // FIXME: Parallel implementation of the LLSQ approximation. 
    // - If a cell-face belongs to the MPI process boundary, fetch the patch-
    //   neighbor internal values, and include them into the LLSQ. 
    // TODO: Include boundary conditions into the LLSQ approximation. 
    // - In boundary cells, use BCs to enforce the normal orientation, then use
    //   the extrapolation-based approximation outlined above for the cutPlane. 
    // TODO: Profile the code with HPCToolkit and isolate bottlenecks. 
    const auto& cellCells = mesh.cellCells(); 
    const auto& cellCenters = mesh.C();
    // Compute the phase indicator in the narrow band.
    forAll(narrowBand, cellI)
    {
        if (narrowBand[cellI] == 1)
        {
            // Assemble the LLSQ linear system
            // TODO: Extend for 2D simulations in OpenFOAM. 
            //SquareMatrix<scalar> LLSQ(4,0);
            simpleMatrix LLSQ
            (
                4  /* size 4x4 */, 
                0. /* init coeff value*/, 
                0. /* init source value*/ 
            );
            scalarList planeCoeffs(4,0);
            scalarList LLSQsource(4,0);
        
            // NOTE: Nc doesn't contain cellI in OpenFOAM: cellI contributions
            // are handled additionallly. 
            const auto& Nc = cellCells[cellI];
                
            // equations 0,1,2
            // \frac{\partial e^{lsq}_c} {\partial n_{c,cmpt}} = 0 
            for (char row = 0; row < 3; ++row)
            {
                // Contributions from cellI : not in Nc
                // - nc coefficient contrib from cellI
                for(char col = 0; col < 3; ++col)
                    LLSQ(row,col) += 
                        cellCenters[cellI][col]*cellCenters[cellI][row];
                // - dc coefficient contrib from cellI 
                LLSQ(row, 3) += cellCenters[cellI][row];
                // - source contrib from cellI
                LLSQsource[row] += psi[cellI]*cellCenters[cellI][row];

                // Contributions from Nc 
                forAll(Nc, cellK)
                {
                    // - nc coefficient contrib from cellK
                    for(char col = 0; col < 3; ++col)
                        LLSQ(row,col) += 
                            cellCenters[Nc[cellK]][col]*cellCenters[Nc[cellK]][row];
                    // - dc coefficient contrib from cellK
                    LLSQ(row, 3) += cellCenters[Nc[cellK]][row];
                    // - source contrib from cellK
                    LLSQsource[row] += 
                        psi[Nc[cellK]]*cellCenters[Nc[cellK]][row];
                }
            }
            // equation 3
            // \frac{\partial e^{lsq}_c}{\partial d_c} = 0 
            // - nc coefficient contrib from cellI
            for(char col = 0; col < 3; ++col)
                LLSQ(3,col) += cellCenters[cellI][col];
            // - source contrib for cellI
            LLSQsource[3] += psi[cellI];

            // Contributions from Nc 
            forAll(Nc, cellK)
            {
                // - nc coefficient contrib from cellK
                for(char col = 0; col < 3; ++col)
                    LLSQ(3,col) += cellCenters[Nc[cellK]][col];
                // source term
                LLSQsource[3] += psi[Nc[cellK]];
            }
            // - dc coefficient is |Nc|: in OpenFOAM |Nc| + 1 (for cellI)
            LLSQ(3,3) = Nc.size() + 1;
            
            // Contributions from face-adjacent cells from MPI boundaries 
            const auto& nBandCell = mesh.cells()[cellI];
            const auto& cellCentersBdryField = cellCenters.boundaryField();
            forAll(nBandCell, faceI)
            {
                const label faceJ = nBandCell[faceI];
                // If the face is not internal. 
                if (! mesh.isInternalFace(faceJ))
                {
                    // Find the patch faceJ belongs to.
                    forAll(patches, patchI)
                    {
                        const fvPatch& patch = patches[patchI];
                        if (isA<processorFvPatch>(patch) && // MPI patch 
                            (faceJ >= patch.start()) && 
                            (faceJ < patch.start() + patch.size())) // faceJ belongs to it
                        {
                            // The face belongs to the MPI process-boundary. 
                            // Fetch the cell center of the cell.  
                            const auto& cellCentersPatchField = cellCentersBdryField[patchI]; 
                            auto cellCentersPatchNeiFieldTmp = 
                                cellCentersPatchField.patchNeighbourField();
                            const auto& cellCentersPatchNeiField = cellCentersPatchNeiFieldTmp();
                            const auto& cellCenter = cellCentersPatchNeiField[faceJ - patch.start()];

                            // Fetch the psi value of the cell.  
                            const auto& psiPatchField = psiBdryField[patchI]; 
                            auto psiPatchNeiFieldTmp = 
                                psiPatchField.patchNeighbourField();
                            const auto& psiPatchNeiField = psiPatchNeiFieldTmp();
                            const auto& psiValue = psiPatchNeiField[faceJ - patch.start()];
                            
                            // equations 0,1,2 contrib from MPI faceJ-adjacent cell
                            // \frac{\partial e^{lsq}_c} {\partial n_{c,cmpt}} = 0 
                            for (char row = 0; row < 3; ++row)
                            {
                                // - nc coefficient contrib from cellI
                                for(char col = 0; col < 3; ++col)
                                    LLSQ(row,col) += 
                                        cellCenter[col]*cellCenter[row];
                                // - dc coefficient contrib from cellI 
                                LLSQ(row, 3) += cellCenter[row];
                                // - source contrib from cellI
                                LLSQsource[row] += psiValue*cellCenter[row];

                                // dc contributions from MPI faceJ-adjacent cell 
                            }
                            
                            // equation 3 contrib from MPI faceJ-adjacent cell
                            // \frac{\partial e^{lsq}_c}{\partial d_c} = 0 
                            // - nc coefficient contrib from cellI
                            for(char col = 0; col < 3; ++col)
                                LLSQ(3,col) += cellCenter[col];

                            // MPI faceJ-adjacent cell contrib to the dc coeff.
                            LLSQ(3,3) += 1;

                            // - source contrib for cellI
                            LLSQsource[3] += psiValue;
                        }
                    }
                }
            }

            //LUscalarMatrix LU (LLSQ); 
            // Crashes in parallel, uses PStream::parRun() even if the linear  
            // system solution is local to the MPI process. TM. 
            //LU.solve(planeCoeffs, LLSQsource);
            //Foam::LUsolve(LLSQ, LLSQsource);
            scalarField& source = LLSQ.source(); 
            source = LLSQsource;
            planeCoeffs = LLSQ.solve(); // TODO: Improve: Gauss substitution. TM.
            
            hesseNormalPlane cutPlane(
                vector(planeCoeffs[0], planeCoeffs[1], planeCoeffs[2]), 
                planeCoeffs[3]
            );
            
            // FIXME: Debugging, remove
            nc[cellI] = cutPlane.normal();

            // TODO: Use the cutPlane to test for intersection and do not
            //       intersect cells that do not require intersection. TM. 
            auto cellIntersection = intersectCell(cellI, mesh, cutPlane); 
            alpha[cellI] = cellIntersection.volume() / mesh.V()[cellI];
        }
    }
    alpha.correctBoundaryConditions();
    //FIXME: Debugging, remove
    nc.write();
}

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

// ************************************************************************* //
