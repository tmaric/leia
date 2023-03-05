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
    Foam::profile

Description

SourceFiles
    profile.C

\*---------------------------------------------------------------------------*/

#ifndef profile_H
#define profile_H

#include "typeInfo.H"
#include "autoPtr.H"
#include "runTimeSelectionTables.H"
#include "fvSolution.H"
#include "volFields.H"
#include "fvMesh.H"
#include "levelSetImplicitSurfaces.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                         Class profile Declaration
\*---------------------------------------------------------------------------*/

// template<typename Surface> 
class profile
{
protected:

    const fvSolution& fvSolution_;
    const dictionary& levelSetDict_;
    const dictionary& profileDict_; 
    const implicitSurface& surface_;

public:

    // Static Data Members

    TypeName("none");

    declareRunTimeSelectionTable
    (
        autoPtr,
        profile, 
        Mesh, 
        (
            const fvMesh& mesh, const implicitSurface& surface  
        ), 
        (mesh, surface)
    );

    // Constructors
    
        //- Construct from components
        profile(const fvMesh& mesh, const implicitSurface& surface);

    // Selectors

        //- Select default constructed
        static autoPtr<profile> New(const fvMesh& mesh, const implicitSurface& surface);

    //- Destructor
    virtual ~profile() = default;

    // Member Functions
    
    virtual scalar value(const vector point) const
    {
        WarningIn("profile::value") 
        << "Profile is an abstract class." << endl;
        return 0;
    }
    
};

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
