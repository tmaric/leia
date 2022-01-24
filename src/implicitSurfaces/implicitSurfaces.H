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

#ifndef implicitSurfaces_H
#define implicitSurfaces_H

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

    class plane : public implicitSurface
    {
        vector position_; 
        vector normal_; 

        public:

            TypeName ("plane");

            plane() = default;

            plane(vector position, vector normal);

            plane(const dictionary& dict);

            virtual ~plane() = default;

            virtual scalar value(const vector& x) const;

            virtual vector grad(const vector& x) const;

            virtual scalar curvature(const vector& x) const;

            vector position() const;

            vector normal() const;
    };

    class sphere : public implicitSurface
    {
        vector center_; 
        scalar radius_; 

        public:

            TypeName ("sphere");

            sphere() = default;

            sphere(vector center, scalar radius);

            sphere(const dictionary& dict);

            virtual ~sphere() = default;

            virtual scalar value(const vector& x) const;

            virtual vector grad(const vector& x) const;

            virtual scalar curvature(const vector& x) const;

            vector center() const;

            scalar radius() const;
    };

    class ellipsoid : public implicitSurface
    {
        vector center_; 
        vector axes_; 
        vector axesSqr_;

        void setAxesSqr(const vector& axes);

        public:

            TypeName ("ellipsoid");

            ellipsoid(vector center, vector axes);

            ellipsoid(const dictionary& dict);

            virtual ~ellipsoid() = default;

            virtual scalar value(const vector& x) const;

            virtual scalar operator()(const vector& x) const;
            
            virtual vector grad(const vector& x) const;

            virtual scalar curvature(const vector& x) const;

            vector center() const;

            vector axes() const;
    };

    class sinc : public implicitSurface
    {
        vector origin_; 
        scalar amplitude_; 
        scalar omega_;

        public:

            TypeName ("sinc");

            sinc(vector origin, scalar amplitude, scalar omega);

            sinc(const dictionary& dict);

            virtual ~sinc() = default;

            virtual scalar value(const vector& x) const;

            virtual scalar operator()(const vector& x) const;
            
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
