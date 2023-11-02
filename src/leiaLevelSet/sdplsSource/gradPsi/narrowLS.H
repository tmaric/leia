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
    Foam::narrowLS

Description
    Strategy class for evaluating gradient psi.

    This instance uses the normals of the LLSQ fit in the
    narrowBand and fvc::grad(psi) in the far-field.

SourceFiles
    narrowLS.C

\*---------------------------------------------------------------------------*/

#ifndef narrowLS_H
#define narrowLS_H

#include "gradPsi.H"


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{


/*---------------------------------------------------------------------------*\
                         Class narrowLS1 Declaration
\*---------------------------------------------------------------------------*/

class narrowLS
    :   public gradPsi
{
private:

    //- Reference to a narrowBand around the interface
    const volScalarField& narrowBand_;

    //- Reference to the LLSQ normals of the geometricalPhaseIndicator
    const volVectorField& nc_;

public:

    // Static Data Members
    
    TypeName("narrowLS");

    // Constructors

        //- Construct from fvSolution

        narrowLS(const fvMesh& mesh);
    //- Destructor
    virtual ~narrowLS() = default;

    // Member functions

        //- Strategy for evaluating gradient of psi.
        //  This instance uses the normals of the LLSQ fit in the
        //  narrowBand and fvc::grad(psi) in the far-field.
        virtual tmp<volVectorField> grad(const volScalarField& psi) const;

};


} // End namespace Foam

#endif

// ************************************************************************* //
