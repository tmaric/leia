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
#include "levelSetImplicitSurfaces.H"
#include "phaseIndicator.H"
#include "profile.H"
#include "NarrowBand.H"
#include "processorFvPatchField.H"
#include <math.h>

// template<typename Surface> 
// scalar signedDistance_profile(Surface const& surf, vector point)
// {
//     return surf->value(point);
// }

// template<typename Surface> 
// scalar tanh_profile(Surface const& surf, vector point)
// {
//     scalar const eps = 1.0;
//     scalar const limit = 0.2; 
//     return limit * Foam::tanh(1/limit * signedDistance_profile(surf, point)/eps);
// }


// template<typename Surface>  
void setField
(
    volScalarField& psi, 
    // profile const& LSprofile
    autoPtr<profile> LSprofile_ptr
)
{
    const auto& mesh = psi.mesh();
    const auto& cellCenters = mesh.C();

    // const fvSolution& fvSolution (mesh);
    // const dictionary& levelSetDict = fvSolution.subDict("levelSet");
    // const word& profile_word = 
    //     levelSetDict.getOrDefault<word>("profile", "signedDistance");

    // scalar (*profile)(Surface const&, vector);

    // if (profile_word == "tanh")
    // {
    //     profile = &tanh_profile;
    //     Info << "Level-set profile tanh\n";
    // }
    // else //(profile_word == "signedDistance")
    // {
    //     profile = &signedDistance_profile;
    //         Info << "Level-set profile signed-distance\n";
    // }
    // // else
    // // {
    // //     InfoErr << "Wrong level-set profile selected.\n";
    // //     exit();
    // //             // FatalIOErrorInLookup
    // //     // (
    // //     //     levelSetDict,
    // //     //     "profile",
    // //     //     systemType,
    // //     //     *dictionaryConstructorTablePtr_
    // //     // ) << exit(FatalIOError);
    // // }



    forAll(psi, cellI)
        // psi[cellI] = LSprofile.value(cellCenters[cellI]);
        psi[cellI] = LSprofile_ptr->value(cellCenters[cellI]);
    
    const auto& Cf = mesh.Cf();
    const auto& CfBoundaryField = Cf.boundaryField(); 
    auto& psiBoundaryField = psi.boundaryFieldRef();  
    const auto& meshBoundary = mesh.boundary(); 

    // Prescribe values at coupled boundary patches.
    forAll(meshBoundary, patchI)
    {
        // const fvPatch& patch = meshBoundary[patchI]; 
        // if (isA<coupledFvPatch>(patch))
        // {
            const auto& CfPatchField = CfBoundaryField[patchI];
            auto& psiPatchField = psiBoundaryField[patchI]; 
            forAll(psiPatchField, faceI)
            {
                // psiPatchField[faceI] = LSprofile.value(CfPatchField[faceI]);
                psiPatchField[faceI] = LSprofile_ptr->value(CfPatchField[faceI]);
            }
        // }
    }
    
    psi.correctBoundaryConditions();
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

int main(int argc, char *argv[])
{
    argList::addOption("alphaName", "word", "Name of the volume fraction field.");
    
    #include "setRootCase.H"
    #include "createTime.H"
    #include "createMesh.H"
    
    const auto alphaName = args.getOrDefault<word>("alphaName", "alpha");

    #include "createFields.H"

    // setField(psi, LSprofile);
    setField(psi, LSprofile_ptr);
    phaseInd->calcPhaseIndicator(alpha, psi);

    psi.write();
    alpha.write();

    // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

    // Checking the marker field convergence.
    OFstream initFile("leiaSetFields.csv"); 
    // Discretization length
    scalar h = Foam::max(Foam::pow(mesh.deltaCoeffs(),-1)).value();
    // |\Omega^-(t=0)|
    scalar V0 = gSum((alpha * mesh.V())());
    initFile << "H,VOL_ALPHA_0\n";
    initFile << h << "," << V0 << "\n";

    Info<< nl;
    runTime.printExecutionTime(Info);

    Info<< "End\n" << endl;

    return 0;
}


// ************************************************************************* //
