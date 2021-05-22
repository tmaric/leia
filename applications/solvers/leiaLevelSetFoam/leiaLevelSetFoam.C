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
#include "heaviside.H"
#include "advectionErrors.H"

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
        calcPhaseIndicator(alpha, phi, epsilon);

        // ||grad(phi)| - 1|
        eGradPhi = Foam::mag(Foam::mag(fvc::grad(phi)) - 1);

        runTime.write();
    }

    // Error evaluation
    
    // Discretization length
    scalar h = Foam::max(Foam::pow(mesh.deltaCoeffs(),-1)).value();

    // max_c(phi_0 - phi_end)
    ePhi = Foam::mag(phi - phi0);
    ePhi.write();
    scalar lInfEphi = Foam::max(ePhi).value();

    // max_c(||grad phi| - 1|)
    scalar lInfEgradPhi = Foam::max(eGradPhi).value();

    OFstream errorFile("leiaLevelSetFoam.csv"); 

    // CSV Header 
    errorFile << "H,"
        << "L_INF_E_PHI,L_INF_E_GRAD_PHI,"
        << "E_VOL_ALPHA,E_GEOM_ALPHA,"
        << "E_BOUND_ALPHA\n";

    errorFile << h << "," 
        << lInfEphi << "," << lInfEgradPhi << ","
        << calcEvolAlpha(alpha, alpha0) << "," 
        << calcEgeomAlpha(alpha, alpha0) << "," 
        << calcEboundAlpha(alpha) << "," 
        << endl;

    Info<< "End\n" << endl;

    return 0;
}


// ************************************************************************* //
