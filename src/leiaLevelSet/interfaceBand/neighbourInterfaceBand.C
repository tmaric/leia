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

#include "neighbourInterfaceBand.H"
#include "addToRunTimeSelectionTable.H"
#include "processorFvPatch.H"

namespace Foam
{
    defineTypeNameAndDebug(neighbourInterfaceBand, false);
    addToRunTimeSelectionTable(interfaceBand, neighbourInterfaceBand, Dictionary);

    neighbourInterfaceBand::neighbourInterfaceBand(const dictionary& dict, const volScalarField& psi)
        :
            signChangeInterfaceBand(dict, psi),
            nneighbour_(dict.get<int>("n"))
    {}

    void neighbourInterfaceBand::calc()
    {
        signChangeInterfaceBand::calc();
        if (interfaceBandField.empty())
        {
            FatalErrorInFunction
            << "Initial interfaceBandField is empty! Can not find neighbours." << exit(FatalError);   
        }

        const volScalarField& interfaceBandField = interfaceBandField_;
        const labelListList& neighbourCells = mesh().cellCells();

        // // Approach: Does not work. newInterfaceBandIndices is empty or full with all labels.
        // auto lambda = [&interfaceBandField](label idx){
        //     scalar cmp = 0.0;
        //     return interfaceBandField[idx] > cmp;
        // };
        // labelList newInterfaceBandIndices = ListOps::findIndices(interfaceBandField, lambda);
        // label ntimes = ListOps::count_if(interfaceBandField, lambda);
        // Info << ntimes << " : ntimes the lambda evaled to true." << endl;  

        // Alternative approach: Find indices with forAll loop
        labelList newInterfaceBandIndices;
        forAll(interfaceBandField, cellID)
        {
            if(interfaceBandField[cellID] > 0.0)
            {
                newInterfaceBandIndices.append(cellID);
            }
        }
        // Info << newInterfaceBandIndices.size() << " : newInterfaceBandIndices.size()" << endl;
        
        if (newInterfaceBandIndices.empty())
        {
            return;
        }

        for(int i=0; i < nneighbour_; ++i)
        {
            labelList tmplist;

            // iterate over all new interfaceBand cells
            forAll(newInterfaceBandIndices, idxIndex)
            {
                label idxCell = newInterfaceBandIndices[idxIndex];
                const labelList& neighbours = neighbourCells[idxCell];

                //iterate over all neighbour cells
                forAll(neighbours, idxNei)
                {
                    label neighbour = neighbours[idxNei];
                    if(interfaceBandField_[neighbour] == 0.0)
                    {
                        interfaceBandField_[neighbour] = 1.0;
                        tmplist.append(neighbour);
                    }
                }
            }
            newInterfaceBandIndices = tmplist;
        }
    }

} // End namespace Foam

// ************************************************************************* //
