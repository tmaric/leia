/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2023 Tomislav Maric, TU Darmstadt
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
    Foam::constantCurvatureSurfaceTension

Description

    A constant-curvature surface-tension force for testing if the pressure-
    velocity coupling algorithm is force-balanced. 

SourceFiles
    constantCurvatureSurfaceTension.C

\*---------------------------------------------------------------------------*/

#ifndef constantCurvatureSurfaceTension_H
#define constantCurvatureSurfaceTension_H

#include "IOdictionary.H"
#include "surfaceTensionForce.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                         Class constantCurvatureSurfaceTension Declaration
\*---------------------------------------------------------------------------*/

class constantCurvatureSurfaceTension
:
    public surfaceTensionForce
{
    protected:
        
        const fvSolution& fvSolutionDict_;
        const dictionary& levelSetDict_; 
        const dictionary& surfTensionDict_;
        
        // Volume fraction
        const dimensionedScalar curvature_;
        const volScalarField& alpha_;

public:

    // Static Data Members
    TypeName ("constantCurvatureSurfaceTension");

    // Constructors
    constantCurvatureSurfaceTension(const fvMesh& mesh);

    //- Destructor
    virtual ~constantCurvatureSurfaceTension() = default;

    // Member Functions
    virtual tmp<surfaceScalarField> faceSurfaceTensionForce() const; 
};

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
