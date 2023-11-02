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

#include "neighbourNarrowBand.H"
#include "addToRunTimeSelectionTable.H"

namespace Foam
{
    defineTypeNameAndDebug(neighbourNarrowBand, false);
    addToRunTimeSelectionTable(narrowBand, neighbourNarrowBand, Dictionary);

    neighbourNarrowBand::neighbourNarrowBand(const dictionary& dict, const volScalarField& psi)
        :
            signChangeNarrowBand(dict, psi),
            ntimes_(dict.get<int>("n"))
    {}

    void neighbourNarrowBand::calc()
    {
        signChangeNarrowBand::calc();
        if (field().empty())
        {
            FatalErrorInFunction
            << "Initial NarrowBandField is empty! Can not find neighbours." << exit(FatalError);   
        }

        const labelListList& neighbourCells = mesh().cellCells();

        // // Approach: Does not work. newNarrowBandIndices is empty or full with all labels.
        // const volScalarField& NarrowBandField = field();
        // auto lambda = [&NarrowBandField](label idx){
        //     scalar cmp = 0.0;
        //     return NarrowBandField[idx] > cmp;
        // };
        // labelList newNarrowBandIndices = ListOps::findIndices(field(), lambda);
        // label ntimes = ListOps::count_if(field(), lambda);
        // Info << ntimes << " : ntimes the lambda evaled to true." << endl;  

        // Alternative approach: Find indices with forAll loop
        labelList newNarrowBandIndices;
        forAll(field(), cellID)
        {
            if(field()[cellID] > 0.0)
            {
                newNarrowBandIndices.append(cellID);
            }
        }
        // Info << newNarrowBandIndices.size() << " : newNarrowBandIndices.size()" << endl;
        
        if (newNarrowBandIndices.empty())
        {
            return;
        }

        for(int i=0; i < ntimes_; ++i)
        {
            labelList tmplist;

            // iterate over all new NarrowBand cells
            forAll(newNarrowBandIndices, idxIndex)
            {
                label idxCell = newNarrowBandIndices[idxIndex];
                const labelList& neighbours = neighbourCells[idxCell];

                //iterate over all neighbour cells
                forAll(neighbours, idxNei)
                {
                    label neighbour = neighbours[idxNei];
                    if(field()[neighbour] == 0.0)
                    {
                        field()[neighbour] = 1.0;
                        tmplist.append(neighbour);
                    }
                }
            }
            newNarrowBandIndices = tmplist;
        }
    }

} // End namespace Foam

// ************************************************************************* //
