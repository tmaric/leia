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
    Foam::NarrowLS1

Description


SourceFiles
    NarrowLS1.C

\*---------------------------------------------------------------------------*/

#ifndef NarrowLS1_H
#define NarrowLS1_H

#include "GradPsi.H"


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
// namespace fv
// {


/*---------------------------------------------------------------------------*\
                         Class NarrowLS1 Declaration
\*---------------------------------------------------------------------------*/

class NarrowLS
    :   public GradPsi
{
private:

    const volScalarField& narrowBand_;
    const volVectorField& nc_;

public:

    // Static Data Members
    
    TypeName("narrowLS");

    // Constructors

        //- Construct from components

        NarrowLS(const fvMesh& mesh);
    //- Destructor
    virtual ~NarrowLS() = default;

    virtual tmp<volVectorField> grad(const volScalarField& psi) const;


};

// } // End namespace fv

} // End namespace Foam

#endif

// ************************************************************************* //
