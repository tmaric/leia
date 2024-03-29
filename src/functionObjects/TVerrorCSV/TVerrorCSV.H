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
    Foam::functionObjects::TVerrorCSV

Group
    grpFieldFunctionObjects

Description
    Calculates the TotalVaration of a field \psi and its absolute and relative error compared to the initial state.
    The calculates properties are written to a CSV file "TVerror.csv".

    <equation>
    \f[
        TV = \sum_f \psi_N - \psi_O
    \f]

Usage
    Minimal example by using \c system/controlDict.functions:
    \verbatim
    TVerrorCSV
    {
        type            TVerrorCSV;
        libs            ( leiaFunctionObjects );
        writeControl    timeStep;
        field           psi;
    }

    \endverbatim

   The inherited entries are elaborated in:
     - \link functionObject.H \endlink
     - \link fieldExpression.H \endlink
     - \link fieldsExpression.H \endlink
     - \link writeFile.H \endlink
     ...

    <if \c postProcess is applicable>
    Minimal example by using the \c postProcess utility:
    \verbatim
        postProcess -func TVerrorCSV
    \endverbatim

    <if \c postProcess is not applicable>
    Usage by the \c postProcess utility is not available.

See also
    - Foam::functionObject
    - Foam::functionObjects::fvMeshFunctionObject
    - ExtendedCodeGuide::functionObjects::field::TVerrorCSV
    ...

SourceFiles
    TVerrorCSV.C
    TVerrorCSVTEMPLATES.C
    ...

\*---------------------------------------------------------------------------*/

#ifndef TVerrorCSV_H
#define TVerrorCSV_H

#include "fvCFD.H"
#include "fvMeshFunctionObject.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
namespace functionObjects
{

/*---------------------------------------------------------------------------*\
                   Class TVerrorCSV Declaration
\*---------------------------------------------------------------------------*/

class TVerrorCSV
:
    public fvMeshFunctionObject
{

    const volScalarField& field_;
    scalar TV_initial_;
    OFstream csvFile_;
    scalar calcTV();

public:

    //- Runtime type information
    TypeName("TVerrorCSV");


    // Constructors

        //- Construct from Time and dictionary
        TVerrorCSV
        (
            const word& name,
            const Time& runTime,
            const dictionary& dict
        );

        //- No copy construct
        TVerrorCSV(const TVerrorCSV&) = delete;

        //- No copy assignment
        void operator=(const TVerrorCSV&) = delete;

        virtual bool execute()
        {
            return true;
        }

        virtual bool write();

    //- Destructor
    virtual ~TVerrorCSV() = default;
};



// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace functionObjects
} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
