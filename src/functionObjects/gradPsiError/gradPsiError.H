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
    Foam::functionObjects::gradPsiError

Group
    grpFieldFunctionObjects

Description
    Calculates the absolute signed distance error of a field \psi which represents signed distance to an interface. 

    <equation>
    \f[
        gradPsiError = ||\nabla \psi|-1|
    \f]

Usage
    Minimal example by using \c system/controlDict.functions:
    \verbatim
    gradPsiError1
    {
        // Mandatory entries (unmodifiable)
        type           gradPsiError;
        libs           (libleiaFunctionObject);
        field          psi;
        result         gradPsiError;
        writeControl   writeTime;

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
        postProcess -func gradPsiError
    \endverbatim

    <if \c postProcess is not applicable>
    Usage by the \c postProcess utility is not available.

See also
    - Foam::functionObject
    - Foam::functionObjects::fvMeshFunctionObject
    - ExtendedCodeGuide::functionObjects::field::gradPsiError
    ...

SourceFiles
    gradPsiError.C

\*---------------------------------------------------------------------------*/

#ifndef gradPsiError_H
#define gradPsiError_H

#include "fvCFD.H"
#include "fieldExpression.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
namespace functionObjects
{

/*---------------------------------------------------------------------------*\
                   Class gradPsiError Declaration
\*---------------------------------------------------------------------------*/

class gradPsiError
:
    public fieldExpression
{
    // Private Member Functions

        //- Calculate the gradPsiErrornitude of the field and register the result
        template<class Type>
        bool calcgradPsiError();

        //- Calculate the gradPsiErrornitude of the field and return true if successful
        virtual bool calc();


public:

    //- Runtime type information
    TypeName("gradPsiError");


    // Constructors

        //- Construct from Time and dictionary
        gradPsiError
        (
            const word& name,
            const Time& runTime,
            const dictionary& dict
        );

        //- No copy construct
        gradPsiError(const gradPsiError&) = delete;

        //- No copy assignment
        void operator=(const gradPsiError&) = delete;


    //- Destructor
    virtual ~gradPsiError() = default;
};



// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace functionObjects
} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
