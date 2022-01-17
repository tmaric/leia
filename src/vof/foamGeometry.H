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
#include <set>
#include <algorithm>
#include "sigFpe.H"

#ifdef VISUALIZE_intersectCell
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
        /*n_ /= Foam::mag(n_); */
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

struct hesseNormalPlane 
{
    vector n_; 
    scalar d_; 
    
    hesseNormalPlane(vector n, scalar d)
        :
            n_(n), d_(d)
    {}
    
    scalar operator()(const point& x) 
    {
        return (n_ & x) + d_; 
    }

    const vector& normal() 
    {
        return n_;
    }
};

struct volumeArea 
{
    double volume_;
    double area_;

    volumeArea() 
        : 
            volume_(0), 
            area_(0)
    {}

    double volume() 
    {
        return volume_;
    }

    double area()  
    {
        return area_;
    }
};

template<typename Intersection=volumeArea, typename ImplicitPlane> 
Intersection intersectCell 
(
    label cellL, 
    const fvMesh& mesh, 
    ImplicitPlane plane
)
{
    Intersection cellIntersection;  

    // Unstructured mesh addressing
    const auto& meshCells = mesh.cells();
    const auto& meshFaces = mesh.faces(); 
    const auto& faceCenters = mesh.Cf();
    const auto& meshPoints = mesh.points();
    const auto& faceOwner = mesh.faceOwner();
    
#ifdef VISUALIZE_intersectCell
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
            // TODO: Remove
            forAll(faceCentersB, patchI)
            {
                const fvPatch& patch = patches[patchI];
                if (patch.type() == "empty") // 2D calculations on the "empty" patch 
                {
                    const face& emptyFace = mesh.faces()[faceG]; 
                    xf = point{0,0,0};
                    forAll(emptyFace, pointI)
                    {
                        xf += mesh.points()[emptyFace[pointI]];
                    }
                    xf /= emptyFace.size();
                }
                else if ((faceG >= patch.start()) && 
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
                (psiValues[0] > 2*SMALL) && 
                (psiValues[1] > 2*SMALL) &&  
                (psiValues[2] > 2*SMALL); 
            
            if (!triangleOutside)
            {

                bool triangleInside = 
                    (psiValues[0] < -2*SMALL) && 
                    (psiValues[1] < -2*SMALL) &&  
                    (psiValues[2] < -2*SMALL); 
                
                vector triAreaNormal = orient*0.5*((xfp0  - xf) ^ (xfp1 -  xf));

                if (triangleInside)
                {
                    cellIntersection.volume_ += (xf & triAreaNormal);
                    
                    #ifdef VISUALIZE_intersectCell  // Visualize the inside triangle.
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
                    // Local addressing simplifies intersection.
                    std::array<point,3> triangle {xf, xfp0, xfp1};
                    if (orient == -1)
                    {
                        // Ensure the orientation of the local triangle 
                        // generates outward-oriented area normal vector.
                        std::swap(triangle[1], triangle[2]);
                        std::swap(psiValues[1], psiValues[2]);
                    }
                    std::vector<point> triangleCut; 
                    //std::vector<point> cutPoints;
                    std::set<size_t> cutSet;
                    // Iterate over triangle points and intersect the triangle 
                    for(label tp0 = 0; tp0 < 3; ++tp0)
                    {
                        label tp1 = (tp0 + 1) % 3;

                        // Triangle point tP0 is inside \Omega^- but not a coplanar point
                        if (psiValues[tp0] < -2*SMALL)
                        {
                            triangleCut.push_back(triangle[tp0]);
                        }
                        // Triangle point tp0 is a coplanar and is an (intersection) point
                        if (Foam::mag(psiValues[tp0]) < 2*SMALL) 
                        {
                            triangleCut.push_back(triangle[tp0]);
                            cutSet.insert(triangleCut.size() - 1);
                            //cutPoints.push_back(triangle[tp0]);
                        }
                        else // Intersect the edge 
                        { 
                            sigFpe::unset(); // Disable OpenFOAM's SIGFPE trapping 

                            scalar lambda = psiValues[tp0] / 
                                (psiValues[tp0] - psiValues[tp1]);
                            if (std::isnan(lambda)) // Both tp0 and tp1 are on the plane.
                            {
                               triangleCut.push_back(triangle[tp0]); 
                               cutSet.insert(triangleCut.size() - 1);
                               //cutPoints.push_back(triangle[tp0]);
                            }
                            else if ((lambda > 2*SMALL) && (lambda < 1-2*SMALL))
                            {
                                point cutPoint = triangle[tp0] + 
                                    lambda * (triangle[tp1] - triangle[tp0]); 
                                triangleCut.push_back(cutPoint); 
                                cutSet.insert(triangleCut.size() - 1);
                                //cutPoints.push_back(cutPoint);
                            }
                            sigFpe::set(); // Enable OpenFOAM's SIGFPE trapping 
                        }
                        
                    }

                    // Add contribution from the triangle cut polygon.
                    if (triangleCut.size() > 2) 
                    {
                        cellIntersection.volume_ += triangleCut[0] & orientedAreaNormal(triangleCut);

                        // Add oriented cut points to the cap polygon.
                        for(std::size_t tc0 = 0; tc0 < triangleCut.size(); ++tc0)
                        {
                            const std::size_t tc1 = (tc0 + 1) % triangleCut.size();
                            const bool tc0Found = cutSet.find(tc0) != cutSet.end();
                            const bool tc1Found = cutSet.find(tc1) != cutSet.end();

                            if (tc0Found && tc1Found)
                            {
                                // Reverse cut point order to for outward orientation 
                                // of the cap polygon.
                                capPolygon.push_back(triangleCut[tc1]);
                                capPolygon.push_back(triangleCut[tc0]); 
                            }
                        }

                        #ifdef VISUALIZE_intersectCell // Visualize the triangle cut polygon.
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
        // E01 coordinate system: 
        // Z-axis (k0dash) : reversed polygon normal
        // Y-axis (j0dash) : first polygon edge
        // X-axis (i0dash) : j0dash x k0dash
        
        // Cap polygon is stored as a sequcence of points 
        // but actually stores disconnected edges 
        // (c0, c1), (c2, c3), ...
        // oriented counter-clockwise w.r.t. k0dash 
        vector k0dash = plane.normal();
        k0dash /= mag(k0dash);
        vector edge01 = capPolygon[1] - capPolygon[0]; 
        scalar edge01mag = mag(edge01);
        vector j0dash = edge01 / edge01mag; 
        vector i0dash = j0dash ^ k0dash;  

        const auto& capPolygonSize = capPolygon.size();
        using sizeType = decltype(capPolygon.size()); 
        for(sizeType k = 0; k < capPolygonSize / 2; ++k)
        {
            sizeType c0 = 2*k;
            sizeType c1 = (c0 + 1) % capPolygonSize;

            // Edge centroid in a global coordinate system. 
            vector xeDash = 0.5*(capPolygon[c0] + capPolygon[c1]) - capPolygon[0];

            // Edge centroid in the E01 coordinate system.
            xeDash = vector(
                xeDash & i0dash, 
                xeDash & j0dash, 
                xeDash & k0dash
            );

            // Edge normal in the global coordinate system.
            // Should be pointing outward from the cap polygon.
            vector neDash = (capPolygon[c1] - capPolygon[c0])^ k0dash;  

            // Edge normal in the E01 coordinate system.
            neDash = vector(
                neDash & i0dash, 
                neDash & j0dash, 
                neDash & k0dash
            );

            capPolygonArea += (xeDash & neDash);
        }
        capPolygonArea *= 0.5; // Cap polygon is oriented into the cut volume.
        // Add the volume contribution from the cap polygon.
        cellIntersection.volume_ += capPolygon[0] & (k0dash * capPolygonArea);

        #ifdef VISUALIZE_intersectCell // Visualize the cap polygon. 
        DynamicList<label> capFace;
        std::size_t oldVtkPointsSize = vtkPoints.size();
        for (std::size_t i = 0; i < capPolygon.size(); ++i)
        {
            capFace.append(oldVtkPointsSize + i);
            vtkPoints.append(capPolygon[i]);
        }
        vtkFaces.append(face(capFace));
        #endif
    }

    #ifdef VISUALIZE_intersectCell // Visualize the intersected cell 
    std::stringstream ss; 
    ss << "cutCell-" << std::setw(5) << std::setfill('0') << cellL; 
    vtkWriter writer(vtkPoints, vtkFaces, ss.str());
    writer.write();
    writer.close();
    #endif

    cellIntersection.volume_ /= 3.;

    return  cellIntersection; 
};

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
