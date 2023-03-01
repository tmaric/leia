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

Class
    Foam::narrowBand

Description

SourceFiles
    narrowBandI.H
    narrowBand.C
    narrowBandIO.C

\*---------------------------------------------------------------------------*/

#ifndef narrowBand_H
#define narrowBand_H

#include "typeInfo.H"
#include "autoPtr.H"
#include "runTimeSelectionTables.H"
#include "fvMesh.H"
#include "volFields.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{



/*---------------------------------------------------------------------------*\
                         Class narrowBand Declaration
\*---------------------------------------------------------------------------*/

class narrowBand
{
protected:
    const fvMesh& mesh_;

public:

    // TypeName("none");

    declareRunTimeSelectionTable
    (
        autoPtr,
        narrowBand, 
        Mesh, 
        (
            const fvMesh& mesh  
        ), 
        (mesh)
    );

    narrowBand(const fvMesh& mesh);

    static autoPtr<narrowBand> New(const fvMesh& mesh);

    virtual ~narrowBand() = default;


    // Member Functions
    virtual const volScalarField& getField() = 0;

};
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
