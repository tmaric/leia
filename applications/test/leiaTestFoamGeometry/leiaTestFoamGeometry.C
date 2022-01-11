/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2022 Tomislav Maric, TU Darmstadt
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
    leiaTestVolFraction

Description

\*---------------------------------------------------------------------------*/

#include "IOobject.H"
#include "Identity.H"
#include "dimensionedScalarFwd.H"
#include "dimensionedType.H"
#include "fvCFD.H"
#include "foamGeometry.H"
#include "fvcAverage.H"
#include "plane.H"

// Area unit tests. 
void testTriangleArea()
{
    std::vector<point> triangle {
        vector{0, 0, 0}, 
        vector{1, 0, 0},
        vector{0, 1, 0}
    };
    
    scalar triAreaNormal = normalArea(triangle, vector{0,0,1});
    if (Foam::mag(triAreaNormal - 0.5) > SMALL) 
    {
        FatalErrorInFunction
            << "Error in area(triangle, normal): "  
            << triAreaNormal << ", should be 0.5. \n"
            << abort(FatalError);
    }
    
    scalar triAreaOriented = Foam::mag(orientedAreaNormal(triangle));
    if (Foam::mag(triAreaOriented - 0.5) > SMALL) 
    {
        FatalErrorInFunction
            << "Error in triAreaOriented(triangle): "  
            << triAreaOriented << ", should be 0.5. \n"
            << abort(FatalError);
    }
    
    vector triCentroid = centroid(triangle);
    scalar triCentroidArea = Foam::mag(centroidAreaNormal(triangle, triCentroid));
    if (Foam::mag(triCentroidArea - 0.5) > SMALL) 
    {
        FatalErrorInFunction
            << "Error in triAreaCentroid(triangle, centroid): "  
            << triCentroidArea << ", should be 0.5. \n"
            << abort(FatalError);
    }
}

void testOrientedSquareArea()
{
    std::vector<point> square {
        vector{0, 0, 0}, 
        vector{1, 0, 0},
        vector{1, 1, 0},
        vector{0, 1, 0}
    };
    
    scalar squareAreaNormal = normalArea(square, vector{0,0,1});
    if (Foam::mag(squareAreaNormal - 1) > SMALL) 
    {
        FatalErrorInFunction
            << "Error in area(square, normal): "  
            << squareAreaNormal << ", should be 1. \n"
            << abort(FatalError);
    }

    scalar squareAreaOriented = Foam::mag(orientedAreaNormal(square));
    if (Foam::mag(squareAreaOriented - 1) > SMALL) 
    {
        FatalErrorInFunction
            << "Error in areaOriented(square): "  
            << squareAreaOriented << ", should be 1. \n"
            << abort(FatalError);
    }

    vector squareCentroid = centroid(square);
    scalar squareCentroidArea = Foam::mag(centroidAreaNormal(square, squareCentroid));
    if (Foam::mag(squareCentroidArea - 1) > SMALL) 
    {
        FatalErrorInFunction
            << "Error in areaOriented(square): "  
            << squareAreaOriented << ", should be 1. \n"
            << abort(FatalError);
    }
}

void testUnorientedSquareArea()
{
    std::vector<point> square{
        vector{0, 0, 0}, 
        vector{1, 0, 0},
        vector{0, 1, 0},
        vector{0, 0, 0}, 
        vector{1, 1, 0},
        vector{0, 1, 0},
        vector{1, 0, 0},
        vector{1, 1, 0}
    };
    
    scalar unorientedSquareArea = normalArea(square, vector{0,0,1});
    if (Foam::mag(unorientedSquareArea - 1.) > SMALL) 
    {
        FatalErrorInFunction
            << "Error in normalArea(square, normal): "  
            << unorientedSquareArea << ", should be 1. \n"
            << abort(FatalError);
    }

    vector squareCentroid = centroid(square);
    scalar squareCentroidArea = Foam::mag(centroidAreaNormal(square, squareCentroid));
    if (Foam::mag(squareCentroidArea - 1) > SMALL) 
    {
        FatalErrorInFunction
            << "Error in centroidAreaNormal(square, centroid): "  
            << squareCentroidArea << ", should be 1. \n"
            << abort(FatalError);
    }
}

void testVolFraction_UnitDomainInsidePlaneHalfspace(volScalarField& alpha) 
{
    const fvMesh& mesh = alpha.mesh();

    point planePoint {10, 10, 10}; 
    vector planeNormal {1, 1, 1};  

    implicitPlane cutPlane(planePoint, planeNormal);

    forAll(mesh.cells(), cellL) 
    {
        alpha[cellL] = cutVolume(cellL, mesh, cutPlane) / mesh.V()[cellL];
    }
    alpha.correctBoundaryConditions();

    scalar error = Foam::max(
        Foam::mag(
            alpha.internalField() - dimensionedScalar("1", dimless, 1)
        )
    ).value();

    alpha.rename("alpha.testVolFraction_UnitDomainInsidePlaneHalfspace");
    alpha.write();

    if (error > SMALL) 
    {
        alpha.write();
        FatalErrorInFunction
            << "The whole domain should be within phase 1, so alpha = 1. n"  
            << "Error in volume fraction calculation: "  
            << "max(|\\alpha - 1|): " << error 
            << abort(FatalError);
    }
}

void testVolFraction_UnitDomainHalvedVertically(volScalarField& alpha) 
{
    alpha = dimensionedScalar("0", dimless, 0);
    const fvMesh& mesh = alpha.mesh();

    point planePoint {0.5, 0.5, 0.5}; 
    vector planeNormal {0, 0, 1};  

    implicitPlane cutPlane(planePoint, planeNormal);

    forAll(mesh.cells(), cellL) 
    {
        alpha[cellL] = cutVolume(cellL, mesh, cutPlane) / mesh.V()[cellL];
    }
    alpha.correctBoundaryConditions();

    scalar error = Foam::mag(
        Foam::sum(alpha.internalField() * mesh.V()) - 
        dimensionedScalar("0.5", dimVolume, 0.5)
    ).value();

    alpha.rename("alpha.testVolFraction_UnitDomainHalvedVertically");
    alpha.write();

    if (error > SMALL) 
    {
        FatalErrorInFunction
            << "The volume of a halved unit domain should be 0.5.\n"  
            << "|0.5 - \\sum_c \\alpha_c |\\Omega_c|| = " << error
            << abort(FatalError);
    }
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

int main(int argc, char *argv[])
{
    #include "setRootCase.H"
    #include "createTime.H"
    #include "createMesh.H"
    
    // Area unit tests

    testTriangleArea();

    testOrientedSquareArea();

    testUnorientedSquareArea();
    
    // Volume fraction tests
    volScalarField alpha
    (
        IOobject
        (
            "alpha",
            runTime.timeName(), 
            mesh, 
            IOobject::NO_READ,
            IOobject::AUTO_WRITE
        ),
        mesh,
        dimensionedScalar("0", dimless, 0)
    );
    
    testVolFraction_UnitDomainInsidePlaneHalfspace(alpha);

    testVolFraction_UnitDomainHalvedVertically(alpha);

    // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

    Info<< "End\n" << endl;

    return 0;
}


// ************************************************************************* //
