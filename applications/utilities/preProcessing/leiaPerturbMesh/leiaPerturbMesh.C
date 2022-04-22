/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | Copyright (C) Tomislav Maric and TU Darmstadt
     \\/     M anipulation  |
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
    Computes average unstructured finite volume discretization length `h`
    preturbs internal cell-corner-points by `\alpha h`, where `\alpha` is a
    user-defined factor (defaults to 0.1).

Author
    Tomislav Maric
    maric@mma.tu-darmstadt.de
    Mathematical Modeling and Analysis,
    Mathematics department, TU Darmstadt, Germany

\*---------------------------------------------------------------------------*/

#include "fvCFD.H"
#include "pointFields.H"

#include <omp.h>
#include <random>

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

// Main program:
int main(int argc, char* argv[])
{
    argList::addOption("alpha", "scalar < 0.5", "Amount of point pertubation.");

    #include "setRootCase.H"
    #include "createTime.H"

    Foam::fvMesh mesh
    (
        Foam::IOobject
        (
            "region0",
            "constant", 
            runTime,
            Foam::IOobject::MUST_READ
        )
    );

    const auto alpha = args.getOrDefault<scalar>("alpha", 0.1);

    const auto meshPoints = mesh.points();
    auto newPoints (meshPoints);

    scalar deltaCoeffAverage = Foam::average(
        mesh.deltaCoeffs().internalField()
    ).value();

    scalar deltaX = pow(deltaCoeffAverage,-1);

    // Perturb new points.
    std::random_device rd;  
    std::mt19937 gen(rd()); 
    std::uniform_real_distribution<> dis(-alpha, alpha);
    forAll(newPoints, pointI)
    {
        newPoints[pointI] += deltaX * vector(dis(gen), dis(gen), dis(gen));
    }

    Info << Foam::max(newPoints - meshPoints) << endl;

    pointMesh pMesh (mesh);
    const auto& pBoundaryMesh = pMesh.boundary(); 
    forAll(pBoundaryMesh, patchI)
    {
        const auto& pPatch = pBoundaryMesh[patchI];
        if (! pPatch.coupled())
        {
            const auto& pointLabels = pPatch.meshPoints(); 
            forAll(pointLabels, pI)
            {
                newPoints[pointLabels[pI]] = meshPoints[pointLabels[pI]];
            }
        }
    }

    mesh.movePoints(newPoints);
    mesh.write();

    Info << "\nEnd\n" << endl;

    return 0;
}

// ************************************************************************* //
