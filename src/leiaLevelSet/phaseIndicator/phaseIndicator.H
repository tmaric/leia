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

Class
    Foam::phaseIndicator

Description

SourceFiles
    phaseIndicator.C

\*---------------------------------------------------------------------------*/

#ifndef phaseIndicator_H
#define phaseIndicator_H

#include "typeInfo.H"
#include "autoPtr.H"
#include "runTimeSelectionTables.H"
#include "fvSolution.H"
#include "volFields.H"
#include "fvMesh.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                         Class phaseIndicator Declaration
\*---------------------------------------------------------------------------*/

class phaseIndicator
{
protected:

    const fvSolution& fvSolution_;
    const dictionary& levelSetDict_;
    const dictionary& phaseIndDict_; 

public:

    // Static Data Members

    TypeName("none");

    declareRunTimeSelectionTable
    (
        autoPtr,
        phaseIndicator, 
        Mesh, 
        (
            const fvMesh& mesh  
        ), 
        (mesh)
    );

    // Constructors
    
        //- Construct from components
        phaseIndicator(const fvMesh& mesh);

    // Selectors

        //- Select default constructed
        static autoPtr<phaseIndicator> New(const fvMesh& mesh);

    //- Destructor
    virtual ~phaseIndicator() = default;

    // Member Functions

    virtual void calcPhaseIndicator
    (
        volScalarField& alpha, 
        const volScalarField& psi 
    )
    {
        WarningIn("phaseIndicator::calcPhaseIndicator") 
            << "Phase indicator not used. " << endl;
    } 
    
};

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
