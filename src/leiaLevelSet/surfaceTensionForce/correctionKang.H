/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2023 AUTHOR,AFFILIATION
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
    Foam::correctionKang

Description
    Surface Tension force with corrected curvature interpolation by Kang et al.
    \cite kang_boundary_2000 .

    \f{equation}{
        \kappa_f = 
        \frac{\kappa_{\O} |\psi_{\N}| 
        + \kappa_{\N} |\psi_{\O}|}{|\psi_{\N}| + |\psi_{\O}|}
    \f}


SourceFiles
    correctionKang.C

\*---------------------------------------------------------------------------*/

#ifndef correctionKang_H
#define correctionKang_H

#include "surfaceTensionForce.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                         Class correctionKang Declaration
\*---------------------------------------------------------------------------*/

class correctionKang
:
    public surfaceTensionForce
{
    // Privata Data Member

        const fvSolution& fvSolutionDict_;
        const dictionary& levelSetDict_; 
        const dictionary& surfTensionDict_;

        // Volume fraction
        const volScalarField& alpha_;

        //- Level set field 
        const volScalarField& psi_;

    // Private Member Functions

        //- Curvature surface field corrected interpolation by Kang et al.
        //  \cite kang_boundary_2000 . 
        tmp<surfaceScalarField> 
        interpolate
            (
                const volScalarField& kappa, 
                const volScalarField& psi 
            ) const;

public:

    TypeName ("correctionKang");

    // Constructors
        correctionKang(const fvMesh& mesh);

    //- Destructor
    virtual ~correctionKang() = default;

    // Member Functions
    
        //- Face-centered surface tension force 
        virtual tmp<surfaceScalarField> faceSurfaceTensionForce() const; 
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
