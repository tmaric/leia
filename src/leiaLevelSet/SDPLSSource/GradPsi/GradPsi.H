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
    Foam::GradPsi1

Description


SourceFiles
    GradPsi1.C

\*---------------------------------------------------------------------------*/

#ifndef GradPsi1_H
#define GradPsi1_H

#include "typeInfo.H"
#include "autoPtr.H"
#include "runTimeSelectionTables.H"
#include "fvCFD.H"


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
// namespace fv
// {


/*---------------------------------------------------------------------------*\
                         Class GradPsi1 Declaration
\*---------------------------------------------------------------------------*/

class GradPsi
{
public:

    // Static Data Members
    
    TypeName("fvc");

    declareRunTimeSelectionTable
    (
        autoPtr,
        GradPsi, 
        Dictionary, 
        (
            const fvMesh& mesh
        ), 
        (mesh)
    );

    // Constructors

        //- Construct from components
        GradPsi(const fvMesh& mesh);

    // Selectors

        //- Select default constructed
        static autoPtr<GradPsi> New(const word type, const fvMesh& mesh);

    //- Destructor
    virtual ~GradPsi() = default;

    // Member functions

    tmp<volVectorField> grad(const volScalarField& psi) const;


};

// } // End namespace fv

} // End namespace Foam

#endif

// ************************************************************************* //
