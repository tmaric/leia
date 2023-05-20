/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2023 Tomislav Maric, TU Darmstadt
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

\*---------------------------------------------------------------------------*/

#include "fvPatchFieldsFwd.H"
#include "primitiveFieldsFwd.H"
#include "surfaceTensionForce.H"
#include "constantCurvatureSurfaceTension.H"
#include "addToRunTimeSelectionTable.H"
#include "volFields.H"
#include "surfaceFields.H"
#include "fvcGrad.H"
#include "fvcSnGrad.H"
#include "surfaceInterpolate.H"
#include "volFieldsFwd.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

defineTypeNameAndDebug(constantCurvatureSurfaceTension, false);
addToRunTimeSelectionTable(surfaceTensionForce, constantCurvatureSurfaceTension, Mesh);

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //
constantCurvatureSurfaceTension::constantCurvatureSurfaceTension(const fvMesh& mesh)
:
    surfaceTensionForce(mesh),
    fvSolutionDict_(mesh_),
    levelSetDict_(fvSolutionDict_.subDict("levelSet")),
    surfTensionDict_(levelSetDict_.subDict("surfaceTensionForce")),
    curvature_
    (
        "curvature", 
        pow(dimLength, -1), 
        surfTensionDict_.get<scalar>("curvature")
    ),
    alpha_(mesh_.lookupObject<volScalarField>(surfTensionDict_.getOrDefault<word>("alpha", "alpha.dispersed")))
{}

// * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * * //

tmp<surfaceScalarField> constantCurvatureSurfaceTension::faceSurfaceTensionForce() const 
{
    return sigma_ * curvature_ * fvc::snGrad(alpha_);
}

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

// ************************************************************************* //
