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
    Foam::Mollifier1

Description


SourceFiles
    Mollifier1.C

\*---------------------------------------------------------------------------*/

#ifndef Mollifier1_H
#define Mollifier1_H

#include "typeInfo.H"
#include "autoPtr.H"
#include "runTimeSelectionTables.H"
#include "fvCFD.H"


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
// namespace fv
// {


/*---------------------------------------------------------------------------*\
                         Class Mollifier1 Declaration
\*---------------------------------------------------------------------------*/

class Mollifier
{
public:

    // Static Data Members
    
    TypeName("none");

    declareRunTimeSelectionTable
    (
        autoPtr,
        Mollifier, 
        Dictionary, 
        (
            const dictionary& dict
        ), 
        (dict)
    );

    // Constructors

        //- Construct from components
        Mollifier(const dictionary& dict);

    // Selectors

        //- Select default constructed
        static autoPtr<Mollifier> New(const dictionary& dict);

    //- Destructor
    virtual ~Mollifier() = default;

    // Member functions

    tmp<volScalarField> field(const volScalarField& psi) const;

protected:
    virtual double mollify(double x) const;


};

// } // End namespace fv

} // End namespace Foam

#endif

// ************************************************************************* //
