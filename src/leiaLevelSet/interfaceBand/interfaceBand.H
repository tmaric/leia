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
    Foam::interfaceBand

Description
    Abstract base class which starts the model hierachy for marking cells which are near the interface aka the "narrowBand".
    Calculate a field with {0,1} which mark cells near the zero level-set interface.


SourceFiles
    interfaceBand.C

\*---------------------------------------------------------------------------*/

#ifndef interfaceBand_H
#define interfaceBand_H

#include "typeInfo.H"
#include "autoPtr.H"
#include "runTimeSelectionTables.H"
#include "fvMesh.H"
#include "volFields.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{



/*---------------------------------------------------------------------------*\
                         Class interfaceBand Declaration
\*---------------------------------------------------------------------------*/

class interfaceBand
{
protected:
    const fvMesh& mesh_;
    volScalarField interfaceBandField_;
    const volScalarField& psi_;

public:

    TypeName("none");

    declareRunTimeSelectionTable
    (
        autoPtr,
        interfaceBand, 
        Mesh, 
        (
            const fvMesh& mesh, const volScalarField& psi
        ), 
        (mesh, psi)
    );

    interfaceBand(const fvMesh& mesh, const volScalarField& psi);

    static autoPtr<interfaceBand> New(const fvMesh& mesh, const volScalarField& psi);

    virtual ~interfaceBand() = default;


    // Member Functions

    const fvMesh& mesh()
    {
        return mesh_;
    }

    virtual void calc()
    {}

    void write()
    {
        interfaceBandField_.write();
    }
};
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
