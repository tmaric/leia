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

Application
    leiaSetFields

Description
    Initialization of signed-distance fields.

\*---------------------------------------------------------------------------*/

#include "fvCFD.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //


#include "initializationFunctions.H"

int main(int argc, char *argv[])
{
    argList::addOption
    (
        "center",
        "(double double double)",
        "Sphere center."
    );

    argList::addOption
    (
        "radius",
        "double",
        "Sphere center."
    );

    #include "setRootCase.H"
    #include "createTime.H"
    #include "createMesh.H"

    #include "createFields.H"


    if (!args.found("center")) 
    {
        FatalErrorInFunction
            << "Sphere center not provided, use -center option." 
            << abort(FatalError);
    }
    if (!args.found("radius")) 
    {
        FatalErrorInFunction
            << "Sphere radius not provided, use -radius option." 
            << abort(FatalError);
    }
    
    const auto center = args.get<vector>("center");
    const auto radius = args.get<scalar>("radius");

    setPhi(phi, center, radius);
    setVolumetricFlux(F);
    setVelocity(U);

    phi.write();
    F.write();
    U.write();

    // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

    Info<< nl;
    runTime.printExecutionTime(Info);

    Info<< "End\n" << endl;

    return 0;
}


// ************************************************************************* //
