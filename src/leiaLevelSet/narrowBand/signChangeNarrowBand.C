/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2022 Tomislav Maric & Julian Reitzel, TU Darmstadt
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

#include "signChangeNarrowBand.H"
#include "addToRunTimeSelectionTable.H"
#include "processorFvPatch.H"

namespace Foam
{
    defineTypeNameAndDebug(signChangeNarrowBand, false);
    addToRunTimeSelectionTable(narrowBand, signChangeNarrowBand, Dictionary);

    signChangeNarrowBand::signChangeNarrowBand(const dictionary& dict, const volScalarField& psi)
        :
            emptyNarrowBand(dict, psi)
    {}

    void signChangeNarrowBand::calc()
    {
        const auto& own = mesh().owner();  
        const auto& nei = mesh().neighbour(); 
        const volScalarField& psi = this->psi();
        volScalarField& NarrowBand = field();
        NarrowBand = dimensionedScalar(NarrowBand.dimensions(), 0.);

        // Select the cells in the narrow band using face-connectivity.
        forAll(own, faceI)
        {
            // FIXME: If a cell is clipped by the interface at one of its corners,
            // and the surrounding cell centers do not switch the iso-sign, it 
            // may not be picked up by this. May cause numerical inconsistency. TM. 
            if (psi[own[faceI]] * psi[nei[faceI]] <= 0)
            {
                NarrowBand[own[faceI]] = 1;
                NarrowBand[nei[faceI]] = 1;
            }
        }
        // Set narrow band values across coupled process boundaries. 
        const auto& psiBdryField = psi.boundaryField(); // needed for Nc LLSQ contribs
        const auto& patches = mesh().boundary(); // needed for Nc LLSQ contribs
        const auto& faceOwner = mesh().faceOwner();
        forAll(psiBdryField, patchI)
        {
            const fvPatch& patch = patches[patchI];
            if (isA<coupledFvPatch>(patch)) // coupled patch 
            {
                const auto& psiPatchField = psiBdryField[patchI]; 
                auto psiPatchNeiFieldTmp = 
                    psiPatchField.patchNeighbourField();
                const auto& psiPatchNeiField = psiPatchNeiFieldTmp();
                forAll(psiPatchNeiField, faceI)
                {
                    label faceJ = faceI + patch.start(); // Global face label.
                    if (psi[faceOwner[faceJ]] * psiPatchNeiField[faceI] <= 0)
                    {
                        NarrowBand[faceOwner[faceJ]] = 1;
                    }
                }
            }
        }
    }

} // End namespace Foam

// ************************************************************************* //
