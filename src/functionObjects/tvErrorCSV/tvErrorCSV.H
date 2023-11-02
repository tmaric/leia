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
    Foam::functionObjects::tvErrorCSV

Group
    grpFieldFunctionObjects

Description
    Calculates the TotalVariation of a field \psi and its absolute and relative error compared to the initial state.
    The calculates properties are written to a CSV file "tvError.csv".

    \f[
        TV = \sum_f |\psi_N - \psi_O|
    \f]

    Note: This formula is only valid in 1D.

Usage
    Minimal example by using \c system/controlDict.functions:
    \verbatim
    tvErrorCSV
    {
        type            tvErrorCSV;
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
        postProcess -func tvErrorCSV
    \endverbatim

    <if \c postProcess is not applicable>
    Usage by the \c postProcess utility is not available.

See also
    - Foam::functionObject
    - Foam::functionObjects::fvMeshFunctionObject
    - ExtendedCodeGuide::functionObjects::field::tvErrorCSV
    ...

SourceFiles
    tvErrorCSV.C
    tvErrorCSVTEMPLATES.C
    ...

\*---------------------------------------------------------------------------*/

#ifndef tvErrorCSV_H
#define tvErrorCSV_H

#include "fvCFD.H"
#include "fvMeshFunctionObject.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
namespace functionObjects
{

/*---------------------------------------------------------------------------*\
                   Class tvErrorCSV Declaration
\*---------------------------------------------------------------------------*/

class tvErrorCSV
:
    public fvMeshFunctionObject
{

    const volScalarField& field_;
    scalar TV_initial_;
    OFstream csvFile_;
    scalar calcTV();

public:

    //- Runtime type information
    TypeName("tvErrorCSV");


    // Constructors

        //- Construct from Time and dictionary
        tvErrorCSV
        (
            const word& name,
            const Time& runTime,
            const dictionary& dict
        );

        //- No copy construct
        tvErrorCSV(const tvErrorCSV&) = delete;

        //- No copy assignment
        void operator=(const tvErrorCSV&) = delete;

        virtual bool execute()
        {
            return true;
        }

        virtual bool write();

    //- Destructor
    virtual ~tvErrorCSV() = default;
};



// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace functionObjects
} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
