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

Description

    Volume fraction of the phase \Omega^-1 given by an implicit planeace 
    \psi(x): R^3 -> R,
    
    \Omega^- := \{x, x \psi(x) < 0 \},
    
    for a polyedral finite volume cell \Omega_c, using the divergence
    theorem in 3D (volume), then again in 2D (area). 

\*---------------------------------------------------------------------------*/

#ifndef volumeFraction_H
#define volumeFraction_H

#include "fvMesh.H"
#include "surfaceFields.H"
#include "volFields.H"
#include <vector>
#include <algorithm>
#include "sigFpe.H"

#ifdef DEBUG_FOAM_GEOMETRY
#include "vtkSurfaceWriter.H"
#include "DynamicList.H"
#include "DynamicField.H"
#include <sstream>
#include <iomanip>
#endif 

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

template<typename PointSequence> 
scalar normalArea 
(
    PointSequence const& points, 
    const vector& normal
)
{ 
    scalar area = 0; 
    using sizeType = decltype(points.size());
    const auto size = points.size(); 
    for (sizeType i = 0; i < size - 1; ++i)
    {
        const auto iNext = (i + 1) % size;
        const point& ei = points[i];
        const point& eiNext = points[iNext];
        const vector edge = ei - eiNext; 
        vector edgeNormal = edge ^ normal; 
        edgeNormal /= Foam::mag(edgeNormal);
        const vector edgeCenter = 0.5 * (ei + eiNext);
        area += (edgeCenter & edgeNormal) * Foam::mag(edge);
    }
    area *= 0.5;
    return Foam::mag(area);
}

template<typename PointSequence> 
point centroid 
(
    PointSequence const& points
)
{ 
    return std::accumulate(
        points.begin(), points.end(), 
        point(0,0,0)
    ) / points.size();
}

template<typename PointSequence> 
vector centroidAreaNormal
(
    PointSequence const& points, 
    const point& centroid 
)
{ 
    vector areaNormal {0,0,0};
    
    using sizeType = decltype(points.size());
    const auto size = points.size(); 
    for (sizeType i = 0; i < size; ++i)
    {
        const auto iNext = (i + 1) % size;
        areaNormal += 0.5 * ((points[i] - centroid) ^ (points[iNext] - centroid));
    }
    
    return areaNormal;
}

template<typename OrientedPointSequence> 
    vector orientedAreaNormal 
(
    OrientedPointSequence const& points
)
{ 
    vector areaNormal {0, 0, 0};
    using sizeType = decltype(points.size());
    const auto& xp0 = points[0];
    for (sizeType p1 = 1; p1 < points.size(); ++p1)
    {
        sizeType p2 = (p1 + 1) % points.size(); 
        areaNormal += 0.5*((points[p1] - xp0) ^ (points[p2] - xp0));
    }
    return areaNormal;
}

struct implicitPlane 
{
    vector p_; 
    vector n_; 
    
    implicitPlane (vector p, vector n)
        :
            p_(p), n_(n)
    {
        n_ /= Foam::mag(n_); 
    }
    
    scalar operator()(const point& x) 
    {
        return n_ & (x - p_);
    }

    const vector& normal() 
    {
        return n_;
    }
};


template<typename ImplicitPlane> 
scalar cutVolume 
(
    label cellL, 
    const fvMesh& mesh, 
    ImplicitPlane plane 
)
{
    scalar cutVol = 0;

    // Unstructured mesh addressing
    const auto& meshCells = mesh.cells();
    const auto& meshFaces = mesh.faces(); 
    const auto& faceCenters = mesh.Cf();
    const auto& meshPoints = mesh.points();
    const auto& faceOwner = mesh.faceOwner();
    
#ifdef DEBUG_FOAM_GEOMETRY
    DynamicField<point> vtkPoints; 
    DynamicList<face>   vtkFaces;
    using Foam::surfaceWriters::vtkWriter;
#endif 

    const auto& meshCell = meshCells[cellL];
    std::vector<point> capPolygon; 
    capPolygon.reserve(4);
    forAll(meshCell, faceL)
    {
        const label faceG = meshCell[faceL];
        // Obtain the face centroid.
        point xf {0, 0, 0};
        if (mesh.isInternalFace(faceG))
        {
             xf = faceCenters[faceG];
        }
        else // Boundary face
        { 
            const auto& faceCentersB = faceCenters.boundaryField();
            const auto& patches = mesh.boundary();
            forAll(faceCentersB, patchI)
            {
                const fvPatch& patch = patches[patchI];
                if ((faceG >= patch.start()) && 
                    (faceG < patch.start() + patch.size()))
                {
                    xf = faceCentersB[patchI][faceG - patch.start()];   
                }
            }
        }
        
        const auto& cellFace = meshFaces[faceG];
        // OF-specific owner-neighbor face area-normal vector orientation
        scalar orient = 1; 
        if (faceOwner[faceG] != cellL)
            orient = -1;
        for(label pointL = 0; pointL < cellFace.size(); ++pointL)
        {
            label pointLnext = (pointL + 1) % cellFace.size();

            const point& xfp0 = meshPoints[cellFace[pointL]];
            const point& xfp1 = meshPoints[cellFace[pointLnext]];
            
            std::array<scalar,3> psiValues {0, 0, 0};
            psiValues[0] = plane(xf);
            psiValues[1] = plane(xfp0);
            psiValues[2] = plane(xfp1);
            
            bool triangleOutside = 
                (psiValues[0] > 0) && 
                (psiValues[1] > 0) &&  
                (psiValues[2] > 0); 
            
            if (!triangleOutside)
            {

                bool triangleInside = 
                    (psiValues[0] <= 0) && 
                    (psiValues[1] <= 0) &&  
                    (psiValues[2] <= 0); 
                
                if (triangleInside)
                {
                    cutVol += xf & 
                        (orient*0.5*((xfp0  - xf) ^ (xfp1 -  xf))); // Triangle area normal vector.
                    
                    #ifdef DEBUG_FOAM_GEOMETRY // Visualize the inside triangle.
                    vtkFaces.append(
                        face {
                            vtkPoints.size(), 
                            vtkPoints.size() + 1, 
                            vtkPoints.size() + 2 
                        }
                    );
                    vtkPoints.append(xf);
                    vtkPoints.append(xfp0);
                    vtkPoints.append(xfp1);
                    #endif 
                } else // The triangle is intersected
                {
                    std::vector<point> triangle {xf, xfp0, xfp1};
                    std::vector<point> triangleCut; 
                    triangleCut.reserve(4);

                    // Iterate over triangle points and intersect the triangle 
                    for(label tp0 = 0; tp0 < 3; ++tp0)
                    {
                        label tp1 = (tp0 + 1) % 3;

                        // Triangle point tP0 is inside \Omega^- but not a coplanar point
                        if (psiValues[tp0] < -SMALL)
                        {
                            triangleCut.push_back(triangle[tp0]);
                        }
                        // Triangle point tp0 is a coplanar and is an (intersection) point
                        if ((psiValues[tp0] > -SMALL) && (psiValues[tp0] < SMALL))
                        {
                            triangleCut.push_back(triangle[tp0]);
                            capPolygon.push_back(triangle[tp0]);
                        }
                        else // Intersect the edge 
                        { 
                            sigFpe::unset(); // Disable OpenFOAM's SIGFPE trapping 

                            scalar lambda = psiValues[tp0] / 
                                (psiValues[tp0] - psiValues[tp1]);
                            if (std::isnan(lambda)) // Both tp0 and tp1 are on the plane.
                            {
                               triangleCut.push_back(triangle[tp0]); 
                               capPolygon.push_back(triangle[tp0]);
                               // tp1 will be picked up in the next iteration.
                            }
                            else if ((lambda > SMALL) && (lambda < 1-SMALL))
                            {
                                point cutPoint = triangle[tp0] + 
                                    lambda * (triangle[tp1] - triangle[tp0]); 
                                triangleCut.push_back(cutPoint); 
                                capPolygon.push_back(cutPoint);
                            }
                            sigFpe::set(); // Enable OpenFOAM's SIGFPE trapping 
                        }
                        
                    }

                    // Add contribution from the triangle cut polygon.
                    if (triangleCut.size() > 2) 
                    {
                        cutVol += triangleCut[0] & orientedAreaNormal(triangleCut);

                        #ifdef DEBUG_FOAM_GEOMETRY // Visualize the triangle cut polygon.
                        DynamicList<label> triangleCutFace;
                        std::size_t oldVtkPointsSize = vtkPoints.size();
                        for (std::size_t i = 0; i < triangleCut.size(); ++i)
                        { 
                            triangleCutFace.append(oldVtkPointsSize + i);
                            vtkPoints.append(triangleCut[i]);
                        };
                        vtkFaces.append(face(triangleCutFace));
                        #endif 
                    }

                }
            }
        }
    }

    // Compute the area of the cap polygon.
    scalar capPolygonArea = 0;
    if (capPolygon.size() > 2) 
    {
        // Coordinate system local to the cut triangle polygon.
        // Using polygon normal as Z-axis, first polygon edge 
        // as Y-axis, and the edge normal in (XY plane) as the
        // X-axis.
        const vector& k0dash = plane.normal();
        vector edge01 = capPolygon[1] - capPolygon[0];    
        scalar edge01mag = mag(edge01);
        vector j0dash = edge01 / edge01mag; 
        vector i0dash = j0dash ^ k0dash;  // edge normal
        
        const auto& capPolygonSize = capPolygon.size();
        using sizeType = decltype(capPolygon.size()); 
        for(sizeType k = 0; k < capPolygonSize / 2; ++k)
        {
            sizeType c0 = 2*k;
            sizeType c1 = (c0 + 1) % capPolygonSize;

            // Compute xe in the global coordinate system.
            vector xeDash = 0.5*(capPolygon[c0] + capPolygon[c1]) - 
                capPolygon[0];
            // Project xe to xeDash (local coordinate system (ne, e0, np))
            xeDash = vector(
                xeDash & i0dash, 
                xeDash & j0dash, 
                xeDash & k0dash
            );

            // Compute ne in the global coordinate system.
            vector neDash = (capPolygon[c1] - capPolygon[c0]) ^ plane.normal(); 
            // Project ne to neDash (local coordinate system (ne, e0, np))
            neDash = vector(
                neDash & i0dash, 
                neDash & j0dash, 
                neDash & k0dash
            );
            
            capPolygonArea += (xeDash & neDash);
        }
        capPolygonArea *= -0.5; // Cap polygon is oriented into the cut volume.
        // Add the volume contribution from the cap polygon.
        cutVol += capPolygon[0] & (plane.normal() * capPolygonArea);

        #ifdef DEBUG_FOAM_GEOMETRY // Visualize the cap polygon. 
        DynamicList<label> capFace;
        std::size_t oldVtkPointsSize = vtkPoints.size();
        for (std::size_t i = 0; i < capPolygon.size(); ++i)
        {
            capFace.append(oldVtkPointsSize + i);
            vtkPoints.append(capPolygon[i]);
        }
        vtkFaces.append(face(capFace));
        std::stringstream ss; 
        ss << "cutCell-" << std::setw(5) << std::setfill('0') << cellL; 
        vtkWriter writer(vtkPoints, vtkFaces, ss.str());
        writer.write();
        #endif

    }

    return  cutVol / 3.; 
};

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
