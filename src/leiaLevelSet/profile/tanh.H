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
    Foam::signedDistance

Description
    Concrete class which represents the model for a tanh level-set profile with some parameters for setting the incline and asymptotic values.

SourceFiles
    signedDistance.C

\*---------------------------------------------------------------------------*/

#ifndef tanh_H
#define tanh_H

#include "signedDistance.H"


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                         Class profile Declaration
\*---------------------------------------------------------------------------*/

class tanhProfile
: 
    public signedDistance
{

    scalar const eps_;
    scalar const limit_; 


public:

    // Static Data Members

    TypeName("tanh");

    // Constructors
    
        //- Construct from components
        tanhProfile(const fvMesh& mesh, const implicitSurface& surface);

    //- Destructor
    virtual ~tanhProfile() = default;

    // Member Functions

    virtual scalar value(const vector point) const;
    
};

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
