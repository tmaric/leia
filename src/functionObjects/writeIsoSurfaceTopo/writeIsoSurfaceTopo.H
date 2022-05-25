/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2019 OpenCFD Ltd.
    Copyright (C) YEAR AUTHOR, AFFILIATION
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

Description
    Template for use with dynamic code generation of a functionObject.

SourceFiles
    functionObjectTemplate.C

\*---------------------------------------------------------------------------*/

#ifndef writeIsoSurfaceTopo_H
#define writeIsoSurfaceTopo_H

#include "regionFunctionObject.H"
#include "dictionaryContent.H"

#include <sstream>
#include <cmath>
#include <iomanip>
#include "isoSurfaceTopo.H"
#include "volPointInterpolation.H"
#include "volFields.H"

namespace Foam 
{
    void writeIsoSurfaceTopo(const fvMesh& mesh, bool forceWrite)
    {
        const volScalarField& psi = mesh.lookupObject<volScalarField>("psi");

        volPointInterpolation vpInterp(mesh); 
        tmp<pointScalarField> psiPointTmp = vpInterp.interpolate(psi);
        pointScalarField& psiPoint = psiPointTmp.ref();
        psiPoint.rename(string("psi") + string(".point"));

        isoSurfaceTopo isoTopo(
            mesh, 
            psi, 
            psiPoint, 
            0.
        );

        const Time& runTime = mesh.time();
        std::stringstream filePathStringStream; 
        filePathStringStream << runTime.path() + "/psiZeroLevelSet-" 
            << std::setw(8) << std::setfill('0') << runTime.timeIndex() << ".vtk";


        if (forceWrite || runTime.writeTime())
            isoTopo.write(filePathStringStream.str());
    }
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

// Forward Declarations
class fvMesh;

/*---------------------------------------------------------------------------*\
                         A templated functionObject
\*---------------------------------------------------------------------------*/

class writeIsoSurfaceTopoFunctionObject
:
    public functionObjects::regionFunctionObject,
    public dictionaryContent
{

    // Private Member Functions

        //- Cast reference of objectRegistry to fvMesh
        const fvMesh& mesh() const;

        //- No copy construct
        writeIsoSurfaceTopoFunctionObject
        (
            const writeIsoSurfaceTopoFunctionObject&
        ) = delete;

        //- No copy assignment
        void operator=
        (
            const writeIsoSurfaceTopoFunctionObject&
        ) = delete;

public:

    //- Runtime type information
    TypeName("writeIsoSurfaceTopo");

    // Constructors

        //- Construct from Time and dictionary
        writeIsoSurfaceTopoFunctionObject
        (
            const word& name,
            const Time& runTime,
            const dictionary& dict
        );


    //- Destructor
    virtual ~writeIsoSurfaceTopoFunctionObject();

    // Member Functions

        //- Code context as a dictionary
        const dictionary& codeContext() const noexcept
        {
            return dictionaryContent::dict();
        }

        //- Read optional controls
        virtual bool read(const dictionary& dict);

        //- Execute (at time-step)
        virtual bool execute();

        //- Write (at write interval)
        virtual bool write();

        //- Executed at the final time-loop
        virtual bool end();
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //

