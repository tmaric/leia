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
    leiaLevelSetFoam 

Description

    \heading Solver details
    The equation is given by:

    \f[
        \ddt{phi} + \div \left(\vec{v} \phi\right) = 0 
    \f]

    Where:
    \vartable
        phi       | Passive scalar
    \endvartable

    \heading Required fields
    \plaintable
        phi       | Passive scalar
        F       | Volumetric Flux [m^3/s]
    \endplaintable

\*---------------------------------------------------------------------------*/

#include "fvCFD.H"
#include "simpleControl.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

int main(int argc, char *argv[])
{
    argList::addNote
    (
        "Level set equation solver."
    );

    #include "addCheckCaseOptions.H"
    #include "setRootCaseLists.H"
    #include "createTime.H"
    #include "createMesh.H"

    simpleControl simple(mesh);

    #include "createFields.H"

    // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

    Info<< "\nCalculating scalar transport\n" << endl;

    surfaceScalarField F0 ("F0", F);
    volScalarField phi0("phi0", phi);

    while (simple.loop())
    {
        Info<< "Time = " << runTime.timeName() << nl << endl;

        dimensionedScalar cosFactor 
        (
            "tFactor", 
            dimless, 
            Foam::cos(M_PI * runTime.timeOutputValue() / 
                runTime.endTime().value())
        );

        F == F0 * cosFactor; 

        fvScalarMatrix phiEqn
        (
            fvm::ddt(phi)
          + fvm::div(F, phi)
        );
        phiEqn.solve();

        runTime.write();
    }

    // Output L_inf(phi) and h for convergence analysis.
    volScalarField ePhi ("ePhi", Foam::mag(phi - phi0));
    ePhi.write();
    scalar lInfEphi = Foam::max(ePhi).value();
    scalar h = Foam::max(Foam::pow(mesh.deltaCoeffs(),-1)).value();
    OFstream errorFile ("leiaLevelSetFoam.csv"); 
    errorFile << h << "," << lInfEphi << endl;

    Info<< "End\n" << endl;

    return 0;
}


// ************************************************************************* //
