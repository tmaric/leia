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
    A level-set equation in conservative form   

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
        phi       | Volumetric Flux [m^3/s]
    \endplaintable

\*---------------------------------------------------------------------------*/

#include "fvCFD.H"
#include "simpleControl.H"
#include "advectionErrors.H"
#include "phaseIndicator.H"
#include "redistancer.H"
#include "NarrowBand.H"
#include "SDPLSSource.H"
#include "advectionVerification.H"

// tmp
#include "fileName.H"
#include "uncollatedFileOperation.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

scalar maxDeltaT(surfaceScalarField phi, const dictionary& dict)
{
    scalar maxCo;
    if (dict.found("CFL"))
    {
        maxCo = dict.get<scalar>("CFL");
    }
    else
    {
        maxCo = dict.get<scalar>("maxCo");
    }
    scalar maxGrowFactor = 1.2;
    const fvMesh& mesh = phi.mesh();
    scalar deltaT = mesh.time().deltaT().value();
    scalarField sumPhi
    (
        fvc::surfaceSum(mag(phi))().primitiveField()
    );

    scalar deltaT_suggestion = maxCo / (0.5*gMax(sumPhi/mesh.V().field()));

    deltaT = min(deltaT * maxGrowFactor, deltaT_suggestion);

    return deltaT;
}


int main(int argc, char *argv[])
{
    argList::addNote
    (
        "Level set equation solver."
    );
    
    argList::addBoolOption
    (
        "fluxCorrection", 
        "Use Helmholz decomposition to enforce a div-free volumetric flux."
    );

    #include "addCheckCaseOptions.H"
    #include "setRootCaseLists.H"
    #include "createTime.H"
    #include "createMesh.H"

    #include "createFields.H"

    // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

    // CFL based deltaT setting
    if (runTime.controlDict().getOrDefault<bool>("adjustTimeStep", false))
    {
        runTime.setDeltaT(maxDeltaT(phi, runTime.controlDict()), false);
    }

    Info<< "\nCalculating scalar transport\n" << endl;

    #include "errorCalculation.H"

    while (runTime.run())
    {
        #include "CourantNo.H"

        ++runTime;
        Info<< "Time = " << runTime.timeName() << endl;
        Info<< "deltaT = " << runTime.deltaT().value() << nl << endl;

        if (velocityModel->isOscillating())
        {
            velocityModel->oscillateVelocity(U, U0, phi, phi0, runTime);
        }

        if (!source->iterative())
        {
            fvScalarMatrix psiEqn
            (
                fvm::ddt(psi)
                + fvm::div(phi, psi)
            ==
                // fvm::SDPLSSource(psi, U)
                source->fvmSDPLSSource(psi, U)
            );

            psiEqn.solve();
        }
        else
        {
            scalarField sdpls0 = scalarField(psi.size());
            scalar sdplsChange = 9999;

            uint i = 0;
            while (!std::isnan(sdplsChange) && sdplsChange > 1e-6 && i < source->maxIterations())
            {
                fvScalarMatrix psiEqn
                (
                    fvm::ddt(psi)
                    + fvm::div(phi, psi)
                ==
                    // fvm::SDPLSSource(psi, U)
                    source->fvmSDPLSSource(psi, U)
                );

                psiEqn.solve();

                fvScalarMatrix sdplsEqn = source->fvmSDPLSSource(psi, U);
                sdplsEqn.lower() = scalarField(psi.size());
                sdplsEqn.upper() = scalarField(psi.size());
                sdplsEqn.psi() == psi;

                scalarField sdpls = sdplsEqn.residual();
                sdplsChange = gSum(mag(sdpls - sdpls0));
                sdpls0 = sdpls;
                ++i;

                Info    << "Evaluating SDPLS source iterative "
                        << ":  Iteration change = " << sdplsChange
                        << endl;
            }
            Info    << "Evaluating SDPLS source iterative "
                    << ":  Final iteration change = " << sdplsChange
                    << ", No Iterations " << i
                    << endl;

        }

        redist->redistance(psi);
        
        phaseInd->calcPhaseIndicator(alpha, psi);

        narrowBand->calc();

        reportErrors(
            errorFile, 
            psi, 
            psi0, 
            alpha, 
            alpha0, 
            phi,
            CoNum
        );

        runTime.write();
        runTime.printExecutionTime(Info);
        

        // CFL based deltaT setting
    if (runTime.controlDict().getOrDefault<bool>("adjustTimeStep", false))
    {
        runTime.setDeltaT(maxDeltaT(phi, runTime.controlDict()), false);
    }

        // if last timestep would overshoot endTime, set deltaT
        if ((runTime.endTime() - runTime) < runTime.deltaT())
        {
            runTime.setDeltaT((runTime.endTime() - runTime), false);
        }

    }

    psi.write();
    alpha.write();

    Info<< "End\n" << endl;

    return 0;
}


// ************************************************************************* //
