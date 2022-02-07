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
    Implicit surfaces with exact normal and curvature. 

\*---------------------------------------------------------------------------*/

#ifndef levelSetImplicitSurfaces_H
#define levelSetImplicitSurfaces_H

#include "typeInfo.H"
#include "autoPtr.H"
#include "runTimeSelectionTables.H"
#include "vector.H"
#include "dictionary.H"

namespace Foam {

    class implicitSurface
    {
        public: 

            TypeName("implicitSurface");

            declareRunTimeSelectionTable
            (
                autoPtr,
                implicitSurface, 
                dictionary, 
                (
                    const dictionary& dict 
                ), 
                (dict)
            );

            static autoPtr<implicitSurface> New(
                const word& name, 
                const dictionary& dict 
            );

            implicitSurface() = default;

            implicitSurface(const dictionary& dict) {};

            virtual ~implicitSurface() = default;

            virtual scalar value(const vector&) const = 0;
            virtual vector grad(const vector&) const = 0;
            virtual scalar curvature(const vector&) const = 0;
    };

    class implicitPlane : public implicitSurface
    {
        vector position_; 
        vector normal_; 

        public:

            TypeName ("implicitPlane");

            implicitPlane() = default;

            implicitPlane(vector position, vector normal);

            implicitPlane(const dictionary& dict);

            virtual ~implicitPlane() = default;

            virtual scalar value(const vector& x) const;

            virtual vector grad(const vector& x) const;

            virtual scalar curvature(const vector& x) const;

            vector position() const;

            vector normal() const;
    };

    class hesseNormalPlane : public implicitSurface
    {
        vector n_; 
        scalar d_; 
        
        public:

            TypeName ("hesseNormalPlane");
        
            hesseNormalPlane(vector n, scalar d);

            hesseNormalPlane(const dictionary&);

            virtual scalar value(const vector& x) const;

            virtual vector grad(const vector& x) const;

            virtual scalar curvature(const vector& x) const;

            const vector& normal() const;
            
            scalar cutoff() const;
    };

    class implicitSphere : public implicitSurface
    {
        vector center_; 
        scalar radius_; 

        public:

            TypeName ("implicitSphere");

            implicitSphere() = default;

            implicitSphere(vector center, scalar radius);

            implicitSphere(const dictionary& dict);

            virtual ~implicitSphere() = default;

            virtual scalar value(const vector& x) const;

            virtual vector grad(const vector& x) const;

            virtual scalar curvature(const vector& x) const;

            vector center() const;

            scalar radius() const;
    };

    class implicitEllipsoid : public implicitSurface
    {
        vector center_; 
        vector axes_; 
        vector axesSqr_;

        void setAxesSqr(const vector& axes);

        public:

            TypeName ("implicitEllipsoid");

            implicitEllipsoid(vector center, vector axes);

            implicitEllipsoid(const dictionary& dict);

            virtual ~implicitEllipsoid() = default;

            virtual scalar value(const vector& x) const;
            
            virtual vector grad(const vector& x) const;

            virtual scalar curvature(const vector& x) const;

            vector center() const;

            vector axes() const;
    };

    class implicitSinc : public implicitSurface
    {
        vector origin_; 
        scalar amplitude_; 
        scalar omega_;

        public:

            TypeName ("implicitSinc");

            implicitSinc(vector origin, scalar amplitude, scalar omega);

            implicitSinc(const dictionary& dict);

            virtual ~implicitSinc() = default;

            virtual scalar value(const vector& x) const;
            
            virtual vector grad(const vector& x) const;

            virtual scalar curvature(const vector& x) const;

            vector origin() const;

            scalar amplitude() const;

            scalar omega() const;
    };

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
