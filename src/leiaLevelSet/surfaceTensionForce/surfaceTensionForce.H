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
    Foam::surfaceTensionForce

Description
    Abstract base class for surface tension force modeling.

SourceFiles
    surfaceTensionForce.C

\*---------------------------------------------------------------------------*/

#ifndef surfaceTensionForce_H
#define surfaceTensionForce_H

#include "messageStream.H"
#include "surfaceFieldsFwd.H"
#include "typeInfo.H"
#include "autoPtr.H"
#include "runTimeSelectionTables.H"
#include "fvSolution.H"
#include "volFields.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                         Class surfaceTensionForce Declaration
\*---------------------------------------------------------------------------*/

class surfaceTensionForce
{
    
protected:
    
    const fvMesh& mesh_;
    
    const Time& runTime_;
    
    // Surface tension coefficient.
    IOdictionary transportProperties_;
    dimensionedScalar sigma_; 

public:

    // Static Data Members

    TypeName("none");

    declareRunTimeSelectionTable
    (
        autoPtr,
        surfaceTensionForce, 
        Mesh, 
        (
            const fvMesh& mesh 
        ), 
        (mesh)
    );

    // Constructors
    
        //- Construct from components
        surfaceTensionForce(const fvMesh& mesh);

    // Selectors

        //- Select default constructed
        static autoPtr<surfaceTensionForce> New(const fvMesh& mesh);

    //- Destructor
    virtual ~surfaceTensionForce() = default;

    // Member Functions
    
    //- Face-centered surface tension force 
    virtual tmp<surfaceScalarField> faceSurfaceTensionForce() const = 0;

};

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
