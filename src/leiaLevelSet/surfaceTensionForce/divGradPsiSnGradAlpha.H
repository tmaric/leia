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
    Foam::divGradPsiSnGradAlpha

Description

    Computes the face-centered surface tension force using the trace
    of the gradient of the level set function, multiplied by the face-
    normal gradient of the phase indicator (CSF model)

    f_{\Sigma,f} = \sigma_f [trace(\nabla(\psi))]_f snGrad(alpha)

SourceFiles
    divGradPsiSnGradAlpha.C

\*---------------------------------------------------------------------------*/

#ifndef divGradPsiSnGradAlpha_H
#define divGradPsiSnGradAlpha_H

#include "surfaceTensionForce.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                         Class divGradPsiSnGradAlpha Declaration
\*---------------------------------------------------------------------------*/

class divGradPsiSnGradAlpha
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
    TypeName ("divGradPsiSnGradAlpha");

    // Constructors
    divGradPsiSnGradAlpha(const fvMesh& mesh);

    //- Destructor
    virtual ~divGradPsiSnGradAlpha() = default;

    // Member Functions
    virtual tmp<surfaceScalarField> faceSurfaceTensionForce() const; 
};

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
