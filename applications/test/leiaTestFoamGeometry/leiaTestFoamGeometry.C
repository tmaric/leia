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
#include <iomanip>

#ifdef VISUALIZE_intersectCell
#include "vtkSurfaceWriter.H"
#include "DynamicList.H"
#include "DynamicField.H"
#include <sstream>
#endif 

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
    Info << "testVolFraction_UnitDomainInsidePlaneHalfspace \n"  << endl;
    alpha = dimensionedScalar("0", dimless, 0);
    const fvMesh& mesh = alpha.mesh();

    point planePoint {10, 10, 10}; 
    vector planeNormal {1, 1, 1};  

    implicitPlane cutPlane(planePoint, planeNormal);

    #ifdef VISUALIZE_intersectCell 
    DynamicField<point> allPoints;
    DynamicList<face> allFaces;
    #endif

    forAll(mesh.cells(), cellL) 
    {
        auto cellIntersection = intersectCell(cellL, mesh, cutPlane);
        alpha[cellL] =  cellIntersection.volume() / mesh.V()[cellL];
        
        if ((alpha[cellL] > 1 + SMALL) || (alpha[cellL] < -SMALL))
        {
            std::cout << "alpha = " << std::setprecision(16) 
                << alpha[cellL] << std::endl;
            FatalErrorInFunction
                << "Volume fraction out of bounds: " 
                << alpha[cellL] << " for cell " << cellL << "\n" 
                << abort(FatalError);
        }
    }

    alpha.rename("alpha.testVolFraction_UnitDomainInsidePlaneHalfspace");
    alpha.write();

    scalar error = Foam::max(
        Foam::mag(
            alpha.internalField() - dimensionedScalar("1", dimless, 1)
        )
    ).value();

    if (error > SMALL) 
    {
        alpha.write();
        FatalErrorInFunction
            << "The whole domain should be within phase 1, so alpha = 1. n"  
            << "Error in volume fraction calculation: "  
            << "max(|\\alpha - 1|): " << error 
            << abort(FatalError);
    }
    Info << "testVolFraction_UnitDomainInsidePlaneHalfspace PASS \n"  << endl;
}

void testVolFraction_UnitDomainHalved(volScalarField& alpha) 
{
    Info << "testVolFraction_UnitDomainHalved \n"  << endl;
    const fvMesh& mesh = alpha.mesh();
    const auto& V = mesh.V();

    if (Foam::mag(gSum(V) - 1) > SMALL) 
    {
        FatalErrorInFunction
            << "This test works only on the unit domain." 
            << abort(FatalError);
    }

    auto doTest = [](
        auto& alpha, 
        const auto& planeNormal
    )
    {
        alpha = dimensionedScalar("0", dimless, 0);
        const fvMesh& mesh = alpha.mesh();

        // Unit box domain has a centroid at (0.5, 0.5, 0.5) which is 
        // used here to halve the domain using different normal orientations.
        point planePoint (0.5, 0.5, 0.5);
        implicitPlane cutPlane(planePoint, planeNormal);

        Info << "Testing intersection with a plane with plane point " 
            << planePoint << " and plane normal " << planeNormal << endl;

        forAll(mesh.cells(), cellL) 
        {
            auto cellIntersection = intersectCell(cellL, mesh, cutPlane);
            alpha[cellL] =  cellIntersection.volume() / mesh.V()[cellL];

            if ((alpha[cellL] > 1 + SMALL) || (alpha[cellL] < -SMALL))
            {
                std::cout << "alpha = " << std::setprecision(16) 
                    << alpha[cellL] << std::endl;
                FatalErrorInFunction
                    << "Volume fraction out of bounds: " 
                    << alpha[cellL] << " for cell " << cellL << "\n" 
                    << abort(FatalError);
            }
        }
        alpha.rename("alpha.testVolFraction_UnitDomainHalvedVertically");
        alpha.write();

        scalar error = Foam::mag(
            Foam::sum(alpha.internalField() * mesh.V()) - 
            dimensionedScalar("0.5", dimVolume, 0.5)
        ).value();

        if (error > SMALL) 
        {
            FatalErrorInFunction
                << "The volume of a halved unit domain should be 0.5.\n"  
                << "|0.5 - \\sum_c \\alpha_c |\\Omega_c|| = " << error << "\n"
                << "planePoint : " << planePoint << "\n"
                << "planeNormal : " << planeNormal << "\n"
                << abort(FatalError);
        }
        Info << "Plane normal " << planeNormal << " PASS" << endl;
    };

    // Cartesian product of [-1, 0, 1] without [0,0,0] for halving the unit box domain.
    // Generated in Python using 
    // from itertools import product
    // input = [-1,0,1]
    // list(product(input, repeat = 3))
    vectorField normals {{
        vector(-1, -1, -1),
        vector(-1, -1, 0),
        vector(-1, -1, 1),
        vector(-1, 0, -1),
        vector(-1, 0, 0),
        vector(-1, 0, 1),
        vector(-1, 1, -1),
        vector(-1, 1, 0),
        vector(-1, 1, 1),
        vector(0, -1, -1),
        vector(0, -1, 0),
        vector(0, -1, 1),
        vector(0, 0, -1),
        vector(0, 0, 1),
        vector(0, 1, -1),
        vector(0, 1, 0),
        vector(0, 1, 1),
        vector(1, -1, -1),
        vector(1, -1, 0),
        vector(1, -1, 1),
        vector(1, 0, -1),
        vector(1, 0, 0),
        vector(1, 0, 1),
        vector(1, 1, -1),
        vector(1, 1, 0),
        vector(1, 1, 1)
    }};
    normals /= Foam::mag(normals);

    for(const auto& planeNormal : normals)
        doTest(alpha, planeNormal); 
    Info << "testVolFraction_UnitDomainHalved PASS \n"  << endl;
}

void testVolFraction_UnitDomainTranslatingPlane(
    volScalarField& alpha, 
    Time& runTime
) 
{
    alpha.rename("alpha.UnitDomainTranslatingPlane");

    vector planePoint(0, 0, 0.5);
    vector planeNormal(1, 0, 0);

    const fvMesh& mesh = alpha.mesh();

    auto doTest = [&]()
    {
        planePoint = vector(runTime.timeOutputValue(), 0, 0);

        implicitPlane cutPlane(planePoint, planeNormal); 

        forAll(mesh.cells(), cellL) 
        {
            auto cellIntersection = intersectCell(cellL, mesh, cutPlane);
            alpha[cellL] =  cellIntersection.volume() / mesh.V()[cellL];

            if ((alpha[cellL] > 1+SMALL) || (alpha[cellL] < -SMALL))
            {
                std::cout << "alpha  =" << std::setprecision(16) 
                    << alpha[cellL] << std::endl;
                FatalErrorInFunction
                    << "Volume fraction out of bounds: "
                    << alpha[cellL] << " for cell " << cellL << "\n" 
                    << abort(FatalError);
            }
        }

        scalar error = Foam::mag(
            Foam::sum(alpha.internalField() * mesh.V()) - 
            dimensionedScalar("alpha", dimVolume, runTime.timeOutputValue())
        ).value();

        if (error > SMALL) 
        {
            FatalErrorInFunction
                << "The volume of the intersected solution domain should be "  
                << "equal to the simulation time. " 
                << "|" << runTime.timeOutputValue() 
                << " - \\sum_c \\alpha_c |\\Omega_c|| = " << error << "\n"
                << "planePoint : " << planePoint << "\n"
                << "planeNormal : " << planeNormal << "\n"
                << abort(FatalError);
        }
    };

    doTest();
    alpha.write();
    
    while(runTime.run())
    {
        ++runTime;

        doTest();

        runTime.write();
    }

    // Reset simulation time.
    runTime.setTime(0, 0); 
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
    
    // - Single cell mesh as input: test special cases for the intersection
    // that cause point and face duplication.
    // - Standard mesh as input: test volume fraction calculation involving
    // mesh boundary. 
    
    // testVolFraction_UnitDomainInsidePlaneHalfspace(alpha);

    testVolFraction_UnitDomainHalved(alpha);

    // Test volume fraction calculation with a plane (1,0,0), moving along
    // (1,0,0) with velocity (1,0,0). 
    testVolFraction_UnitDomainTranslatingPlane(alpha, runTime);

    // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

    Info<< "End\n" << endl;

    return 0;
}


// ************************************************************************* //
