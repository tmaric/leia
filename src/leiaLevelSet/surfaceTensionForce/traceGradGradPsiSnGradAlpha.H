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
    Foam::traceGradGradPsiSnGradAlpha

Description

    Computes the face-centered surface tension force using the trace
    of the gradient of the geometrical normal field computed in the 
    narrow band by the geometrical phase indicator. The field is fetched
    from the mesh (registry). Outside of the narrow band, the normal field
    is computed as a normalized gradient of the level set field (psi). 

SourceFiles
    traceGradGradPsiSnGradAlpha.C

\*---------------------------------------------------------------------------*/

#ifndef traceGradGradPsiSnGradAlpha_H
#define traceGradGradPsiSnGradAlpha_H

#include "IOdictionary.H"
#include "surfaceTensionForce.H"
#include "ITstream.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                         Class traceGradGradPsiSnGradAlpha Declaration
\*---------------------------------------------------------------------------*/

class traceGradGradPsiSnGradAlpha
:
    public surfaceTensionForce
{
    protected:
        
        const fvSolution& fvSolutionDict_;
        const dictionary& levelSetDict_; 
        const dictionary& surfTensionDict_;
        
        // Interface normals from the geometrical phase indicator. 
        const volVectorField& normals_;
        // Volume fraction
        const volScalarField& alpha_;
        // Level set field 
        const volScalarField& psi_;

public:

    // Static Data Members
    TypeName ("traceGradGradPsiSnGradAlpha");

    // Constructors
    traceGradGradPsiSnGradAlpha(const fvMesh& mesh);

    //- Destructor
    virtual ~traceGradGradPsiSnGradAlpha() = default;

    // Member Functions
    virtual tmp<surfaceScalarField> faceSurfaceTensionForce() const; 
};

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
