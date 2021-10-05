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
        \ddt{psi} + \div \left(\vec{v} \psi\right) = 0 
    \f]

    Where:
    \vartable
        psi       | Passive scalar
    \endvartable

    \heading Required fields
    \plaintable
        psi       | Passive scalar
        F       | Volumetric Flux [m^3/s]
    \endplaintable

\*---------------------------------------------------------------------------*/

#include "fvCFD.H"
#include "simpleControl.H"
#include "advectionErrors.H"
#include "phaseIndicator.H"

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

    OFstream errorFile("leiaLevelSetFoam.csv"); 
    // CSV Header 
    errorFile << "TIME,"
        << "DELTA_X,"
        << "L_INF_E_PSI,"
        << "E_VOL_ALPHA,"
        << "E_GEOM_ALPHA,"
        << "E_BOUND_ALPHA\n";

    reportErrors(errorFile, psi, psi0, alpha, alpha0);

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

        phi == phi0 * cosFactor; 

        fvScalarMatrix psiEqn
        (
            fvm::ddt(psi)
          + fvm::div(phi, psi)
        );

        psiEqn.solve();
        phaseInd->calcPhaseIndicator(alpha, psi);

        reportErrors(errorFile, psi, psi0, alpha, alpha0);

        runTime.write();
    }

    // FIXME: they aren't picked up by the loop in the last time step. 
    psi.write();
    alpha.write();

    Info<< "End\n" << endl;

    return 0;
}


// ************************************************************************* //
