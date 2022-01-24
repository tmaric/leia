/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2020 2020 Tomislav Maric, TU Darmstadt
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

#include "implicitSurfaces.H"
#include "addToRunTimeSelectionTable.H"

namespace Foam {

// * * * * * * * * * * * * Class implicitSurface  * * * * * * * * * * * //

// * * * * * * * * * * * * * * Static Members * * * * * * * * * * * * * //

defineTypeNameAndDebug(implicitSurface, false);
defineRunTimeSelectionTable(implicitSurface, dictionary);

autoPtr<implicitSurface> implicitSurface::New 
(
    const word& modelType, 
    const dictionary& dict 
)
{
    auto* ctorPtr = dictionaryConstructorTable(modelType);

    if (!ctorPtr)
    {
        FatalIOErrorInLookup
        (
            dict,
            "implicitSurface",
            modelType,
            *dictionaryConstructorTablePtr_
        ) << exit(FatalIOError);
    }

    // Construct the model and return the autoPtr to the object. 
    return autoPtr<implicitSurface>(ctorPtr(dict));
}

// * * * * * * * * * * * * Class plane  * * * * * * * * * * * //

// * * * * * * * * * * * * * * Static Members * * * * * * * * * * * * * //

defineTypeNameAndDebug(plane, false);
addToRunTimeSelectionTable(implicitSurface, plane, dictionary);

// * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

plane::plane(vector position, vector normal)
: 
    position_(position), 
    normal_(normal)
{
    normal_ /= Foam::mag(normal_);
}

plane::plane(const dictionary& dict)
    :
        position_(dict.get<vector>("position")),
        normal_(dict.get<vector>("normal"))
{}

// * * * * * * * * * * * * * Member Functions * * * * * * * * * * * * * //

scalar plane::value(const vector& x) const
{
    return Foam::dot(x - position_, normal_);
}

vector plane::grad(const vector& x) const
{
    return normal_; 
}

vector plane::position() const
{
    return position_; 
}

vector plane::normal() const
{
    return normal_; 
}

scalar plane::curvature(const vector& x) const
{
    return 0;
}

// * * * * * * * * * * * * Class sphere * * * * * * * * * * * //

// * * * * * * * * * * * * * * Static Members * * * * * * * * * * * * * //

defineTypeNameAndDebug(sphere, false);
addToRunTimeSelectionTable(implicitSurface, sphere, dictionary);

// * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

sphere::sphere(vector center, scalar radius)
    : 
        center_(center), 
        radius_(radius)
{}

sphere::sphere(dictionary const& dict)
    : 
        center_(dict.get<vector>("center")),
        radius_(dict.get<scalar>("radius"))
{}

// * * * * * * * * * * * * * Member Functions * * * * * * * * * * * * * //

scalar sphere::value(const vector& x) const
{
    return Foam::mag(x - center_) - radius_; 
}

vector sphere::grad(const vector& x) const
{
    scalar x0c0 = x[0] - center_[0];
    scalar x1c1 = x[1] - center_[1];
    scalar x2c2 = x[2] - center_[2];

    return vector(x0c0, x1c1, x2c2) / 
        sqrt(x0c0*x0c0 + x1c1*x1c1 + x2c2*x2c2);
}

vector sphere::center() const
{
    return center_; 
}

scalar sphere::radius() const
{
    return radius_; 
}

scalar sphere::curvature(const vector& x) const
{
    return 1 / radius_;
}

// * * * * * * * * * * * * Class ellipsoid * * * * * * * * * * * //

// * * * * * * * * * * * * * * Static Members * * * * * * * * * * * * * //

defineTypeNameAndDebug(ellipsoid, false);
addToRunTimeSelectionTable(implicitSurface, ellipsoid, dictionary);

// * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

ellipsoid::ellipsoid(vector center, vector axes)
    : 
        center_(center), 
        axes_(axes)
{
    setAxesSqr(axes);
}

ellipsoid::ellipsoid(const dictionary& dict)
    :
        center_(dict.get<vector>("center")),
        axes_(dict.get<vector>("axes"))
{
    setAxesSqr(axes_);
}

// * * * * * * * * * * * * * Member Functions * * * * * * * * * * * * * //

void ellipsoid::setAxesSqr(const vector& axes)
{
    axesSqr_[0] = Foam::sqr(axes[0]);
    axesSqr_[1] = Foam::sqr(axes[1]);
    axesSqr_[2] = Foam::sqr(axes[2]);
}

scalar ellipsoid::value(const vector& x) const
{
    return Foam::sqr(x[0] - center_[0]) / axesSqr_[0] + 
           Foam::sqr(x[1] - center_[1]) / axesSqr_[1] + 
           Foam::sqr(x[2] - center_[2]) / axesSqr_[2] - 1;
}

vector ellipsoid::grad(const vector& x) const
{
    return 2*vector
    (
        (x[0] - center_[0])/axesSqr_[0], 
        (x[1] - center_[1])/axesSqr_[1], 
        (x[2] - center_[2])/axesSqr_[2]
    );
}

vector ellipsoid::center() const
{
    return center_; 
}

vector ellipsoid::axes() const
{
    return axes_; 
}

scalar ellipsoid::curvature(const vector& x) const
{
    const scalar& x0 = x[0];
    const scalar& x1 = x[1];
    const scalar& x2 = x[2];

    const scalar& O0 = center_[0];
    const scalar& O1 = center_[1];
    const scalar& O2 = center_[2];

    const scalar& a0 = axesSqr_[0];
    const scalar& a1 = axesSqr_[1];
    const scalar& a2 = axesSqr_[2];

    return 1.0/(pow(a2, 2)*sqrt(4*pow(-O2 + x2, 2)/pow(a2, 4) + 4*pow(-O1 + x1, 2)
           /pow(a1, 4) + 4*pow(-O0 + x0, 2)/pow(a0, 4))) - 2.0*(-2*O2 + 2*x2)*(-O2 + x2)
           /(pow(a2, 6)*pow(4*pow(-O2 + x2, 2)/pow(a2, 4) + 4*pow(-O1 + x1, 2)
           /pow(a1, 4) + 4*pow(-O0 + x0, 2)/pow(a0, 4), 3.0/2.0)) + 1.0/(pow(a1, 2)
           *sqrt(4*pow(-O2 + x2, 2)/pow(a2, 4) + 4*pow(-O1 + x1, 2)/pow(a1, 4)
           + 4*pow(-O0 + x0, 2)/pow(a0, 4))) - 2.0*(-2*O1 + 2*x1)*(-O1 + x1)/(pow(a1, 6)
           *pow(4*pow(-O2 + x2, 2)/pow(a2, 4) + 4*pow(-O1 + x1, 2)/pow(a1, 4)
           + 4*pow(-O0 + x0, 2)/pow(a0, 4), 3.0/2.0)) + 1.0/(pow(a0, 2)*sqrt(4
           *pow(-O2 + x2, 2)/pow(a2, 4) + 4*pow(-O1 + x1, 2)/pow(a1, 4) + 4
           *pow(-O0 + x0, 2)/pow(a0, 4))) - 2.0*(-2*O0 + 2*x0)*(-O0 + x0)/(pow(a0, 6)
           *pow(4*pow(-O2 + x2, 2)/pow(a2, 4) + 4*pow(-O1 + x1, 2)/pow(a1, 4)
           + 4*pow(-O0 + x0, 2)/pow(a0, 4), 3.0/2.0));
}

// * * * * * * * * * * * * Class sinc * * * * * * * * * * * //

// * * * * * * * * * * * * * * Static Members * * * * * * * * * * * * * //

defineTypeNameAndDebug(sinc, false);
addToRunTimeSelectionTable(implicitSurface, sinc, dictionary);

// * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

sinc::sinc(vector origin, scalar amplitude, scalar omega)
    : 
        origin_(origin), 
        amplitude_(amplitude), 
        omega_(omega)
{}

sinc::sinc(const dictionary& dict)
    :
        origin_(dict.get<vector>("origin")),
        amplitude_(dict.get<scalar>("amplitude")),
        omega_(dict.get<scalar>("omega"))
{}

// * * * * * * * * * * * * * Member Functions * * * * * * * * * * * * * //

scalar sinc::value(const vector& x) const
{
    double r = Foam::sqrt
    (
        Foam::sqr(x[0] - origin_[0]) + 
        Foam::sqr(x[1] - origin_[1]) 
    );

    if (r < std::numeric_limits<double>::min())
        return amplitude_; 
    else 
    {
        return x[2] - origin_[2] - amplitude_ * sin(omega_ * r) / (omega_ * r);
    }
}

vector sinc::grad(const vector& x) const
{
    const scalar& A = amplitude_; 
    const scalar& O0 = origin_[0];
    const scalar& O1 = origin_[1];

    const scalar& x0 = x[0];
    const scalar& x1 = x[1];

    return vector // Expression calculated in sympy.
    (
        A*(O0 - x0)*(omega_*pow(pow(O0 - x0, 2) + pow(O1 - x1, 2), 3.0/2.0)*cos(omega_*sqrt(pow(O0 - x0, 2) + pow(O1 - x1, 2))) - (pow(O0 - x0, 2) + pow(O1 - x1, 2))*sin(omega_*sqrt(pow(O0 - x0, 2) + pow(O1 - x1, 2))))/(omega_*pow(pow(O0 - x0, 2) + pow(O1 - x1, 2), 5.0/2.0)),
            A*(O1 - x1)*(omega_*pow(pow(O0 - x0, 2) + pow(O1 - x1, 2), 3.0/2.0)*cos(omega_*sqrt(pow(O0 - x0, 2) + pow(O1 - x1, 2))) - (pow(O0 - x0, 2) + pow(O1 - x1, 2))*sin(omega_*sqrt(pow(O0 - x0, 2) + pow(O1 - x1, 2))))/(omega_*pow(pow(O0 - x0, 2) + pow(O1 - x1, 2), 5.0/2.0)),
            1
        );
}

vector sinc::origin() const
{
    return origin_; 
}

scalar sinc::amplitude() const
{
    return amplitude_; 
}

scalar sinc::omega() const
{
    return omega_; 
}

scalar sinc::curvature(const vector& x) const
{
    const scalar& x0 = x[0];
    const scalar& x1 = x[1];

    const scalar& O0 = origin_[0];
    const scalar& O1 = origin_[1];

    const scalar& A = amplitude_;
    const scalar& w = omega_;

    // Taken from sympy
    return 0.5*A*(O0 - x0)*(5*O0 - 5*x0)*(w*pow(pow(O0 - x0, 2)
           + pow(O1 - x1, 2), 3.0/2.0)*cos(w*sqrt(pow(O0 - x0, 2)
           + pow(O1 - x1, 2))) - (pow(O0 - x0, 2) + pow(O1 - x1, 2))*sin(w
           *sqrt(pow(O0 - x0, 2) + pow(O1 - x1, 2))))/(w*pow(pow(O0 - x0, 2)
           + pow(O1 - x1, 2), 7.0/2.0)*sqrt(pow(A, 2)*pow(O0 - x0, 2)*pow(w
           *pow(pow(O0 - x0, 2) + pow(O1 - x1, 2), 3.0/2.0)*cos(w
           *sqrt(pow(O0 - x0, 2) + pow(O1 - x1, 2))) - (pow(O0 - x0, 2)
           + pow(O1 - x1, 2))*sin(w*sqrt(pow(O0 - x0, 2)
           + pow(O1 - x1, 2))), 2)/(pow(w, 2)*pow(pow(O0 - x0, 2)
           + pow(O1 - x1, 2), 5)) + pow(A, 2)*pow(O1 - x1, 2)*pow(w
           *pow(pow(O0 - x0, 2) + pow(O1 - x1, 2), 3.0/2.0)*cos(w*sqrt(pow(O0 - x0, 2) + pow(O1 - x1, 2)))
           - (pow(O0 - x0, 2) + pow(O1 - x1, 2))*sin(w*sqrt(pow(O0 - x0, 2) + pow(O1 - x1, 2))), 2)/(pow(w, 2)
           *pow(pow(O0 - x0, 2) + pow(O1 - x1, 2), 5)) + 1)) + 0.5*A*(O0 - x0)*(w*pow(pow(O0 - x0, 2)
           + pow(O1 - x1, 2), 3.0/2.0)*cos(w*sqrt(pow(O0 - x0, 2) + pow(O1 - x1, 2))) - (pow(O0 - x0, 2)
           + pow(O1 - x1, 2))*sin(w*sqrt(pow(O0 - x0, 2) + pow(O1 - x1, 2))))*(-1.0/2.0*pow(A, 2)*(-2*O0 + 2*x0)
           *pow(w*pow(pow(O0 - x0, 2) + pow(O1 - x1, 2), 3.0/2.0)*cos(w*sqrt(pow(O0 - x0, 2) + pow(O1 - x1, 2)))
           - (pow(O0 - x0, 2) + pow(O1 - x1, 2))*sin(w*sqrt(pow(O0 - x0, 2) + pow(O1 - x1, 2))), 2)/(pow(w, 2)
           *pow(pow(O0 - x0, 2) + pow(O1 - x1, 2), 5)) - 1.0/2.0*pow(A, 2)*pow(O0 - x0, 2)*(10*O0 - 10*x0)
           *pow(w*pow(pow(O0 - x0, 2) + pow(O1 - x1, 2), 3.0/2.0)*cos(w*sqrt(pow(O0 - x0, 2) + pow(O1 - x1, 2)))
           - (pow(O0 - x0, 2) + pow(O1 - x1, 2))*sin(w*sqrt(pow(O0 - x0, 2) + pow(O1 - x1, 2))), 2)/(pow(w, 2)
           *pow(pow(O0 - x0, 2) + pow(O1 - x1, 2), 6)) - 1.0/2.0*pow(A, 2)*pow(O0 - x0, 2)*(w*pow(pow(O0 - x0, 2)
           + pow(O1 - x1, 2), 3.0/2.0)*cos(w*sqrt(pow(O0 - x0, 2) + pow(O1 - x1, 2))) - (pow(O0 - x0, 2)
           + pow(O1 - x1, 2))*sin(w*sqrt(pow(O0 - x0, 2) + pow(O1 - x1, 2))))*(-2*pow(w, 2)*(-O0 + x0)*(pow(O0 - x0, 2)
           + pow(O1 - x1, 2))*sin(w*sqrt(pow(O0 - x0, 2) + pow(O1 - x1, 2))) + 2*w*(-3*O0 + 3*x0)*sqrt(pow(O0 - x0, 2)
           + pow(O1 - x1, 2))*cos(w*sqrt(pow(O0 - x0, 2) + pow(O1 - x1, 2))) + 2*w*(-O0 + x0)*(-pow(O0 - x0, 2)
           - pow(O1 - x1, 2))*cos(w*sqrt(pow(O0 - x0, 2) + pow(O1 - x1, 2)))/sqrt(pow(O0 - x0, 2) + pow(O1 - x1, 2))
           + 2*(2*O0 - 2*x0)*sin(w*sqrt(pow(O0 - x0, 2) + pow(O1 - x1, 2))))/(pow(w, 2)*pow(pow(O0 - x0, 2)
           + pow(O1 - x1, 2), 5)) - 1.0/2.0*pow(A, 2)*(10*O0 - 10*x0)*pow(O1 - x1, 2)*pow(w*pow(pow(O0 - x0, 2)
           + pow(O1 - x1, 2), 3.0/2.0)*cos(w*sqrt(pow(O0 - x0, 2) + pow(O1 - x1, 2))) - (pow(O0 - x0, 2)
           + pow(O1 - x1, 2))*sin(w*sqrt(pow(O0 - x0, 2) + pow(O1 - x1, 2))), 2)/(pow(w, 2)*pow(pow(O0 - x0, 2)
           + pow(O1 - x1, 2), 6)) - 1.0/2.0*pow(A, 2)*pow(O1 - x1, 2)*(w*pow(pow(O0 - x0, 2) + pow(O1 - x1, 2), 3.0/2.0)
           *cos(w*sqrt(pow(O0 - x0, 2) + pow(O1 - x1, 2))) - (pow(O0 - x0, 2) + pow(O1 - x1, 2))*sin(w
           *sqrt(pow(O0 - x0, 2) + pow(O1 - x1, 2))))*(-2*pow(w, 2)*(-O0 + x0)*(pow(O0 - x0, 2) + pow(O1 - x1, 2))
           *sin(w*sqrt(pow(O0 - x0, 2) + pow(O1 - x1, 2))) + 2*w*(-3*O0 + 3*x0)*sqrt(pow(O0 - x0, 2) + pow(O1 - x1, 2))
           *cos(w*sqrt(pow(O0 - x0, 2) + pow(O1 - x1, 2))) + 2*w*(-O0 + x0)*(-pow(O0 - x0, 2) - pow(O1 - x1, 2))*cos(w
           *sqrt(pow(O0 - x0, 2) + pow(O1 - x1, 2)))/sqrt(pow(O0 - x0, 2) + pow(O1 - x1, 2)) + 2*(2*O0 - 2*x0)*sin(w
           *sqrt(pow(O0 - x0, 2) + pow(O1 - x1, 2))))/(pow(w, 2)*pow(pow(O0 - x0, 2) + pow(O1 - x1, 2), 5)))/(w
           *pow(pow(O0 - x0, 2) + pow(O1 - x1, 2), 5.0/2.0)*pow(pow(A, 2)*pow(O0 - x0, 2)*pow(w*pow(pow(O0 - x0, 2)
           + pow(O1 - x1, 2), 3.0/2.0)*cos(w*sqrt(pow(O0 - x0, 2) + pow(O1 - x1, 2))) - (pow(O0 - x0, 2)
           + pow(O1 - x1, 2))*sin(w*sqrt(pow(O0 - x0, 2) + pow(O1 - x1, 2))), 2)/(pow(w, 2)*pow(pow(O0 - x0, 2)
           + pow(O1 - x1, 2), 5)) + pow(A, 2)*pow(O1 - x1, 2)*pow(w*pow(pow(O0 - x0, 2) + pow(O1 - x1, 2), 3.0/2.0)
           *cos(w*sqrt(pow(O0 - x0, 2) + pow(O1 - x1, 2))) - (pow(O0 - x0, 2) + pow(O1 - x1, 2))*sin(w
           *sqrt(pow(O0 - x0, 2) + pow(O1 - x1, 2))), 2)/(pow(w, 2)*pow(pow(O0 - x0, 2) + pow(O1 - x1, 2), 5))
           + 1, 3.0/2.0)) + 0.5*A*(O0 - x0)*(-pow(w, 2)*(-O0 + x0)*(pow(O0 - x0, 2) + pow(O1 - x1, 2))*sin(w
           *sqrt(pow(O0 - x0, 2) + pow(O1 - x1, 2))) + w*(-3*O0 + 3*x0)*sqrt(pow(O0 - x0, 2) + pow(O1 - x1, 2))
           *cos(w*sqrt(pow(O0 - x0, 2) + pow(O1 - x1, 2))) + w*(-O0 + x0)*(-pow(O0 - x0, 2) - pow(O1 - x1, 2))
           *cos(w*sqrt(pow(O0 - x0, 2) + pow(O1 - x1, 2)))/sqrt(pow(O0 - x0, 2) + pow(O1 - x1, 2)) + (2*O0 - 2*x0)
           *sin(w*sqrt(pow(O0 - x0, 2) + pow(O1 - x1, 2))))/(w*pow(pow(O0 - x0, 2) + pow(O1 - x1, 2), 5.0/2.0)
           *sqrt(pow(A, 2)*pow(O0 - x0, 2)*pow(w*pow(pow(O0 - x0, 2) + pow(O1 - x1, 2), 3.0/2.0)*cos(w
           *sqrt(pow(O0 - x0, 2) + pow(O1 - x1, 2))) - (pow(O0 - x0, 2) + pow(O1 - x1, 2))*sin(w*sqrt(pow(O0 - x0, 2)
           + pow(O1 - x1, 2))), 2)/(pow(w, 2)*pow(pow(O0 - x0, 2) + pow(O1 - x1, 2), 5)) + pow(A, 2)*pow(O1 - x1, 2)
           *pow(w*pow(pow(O0 - x0, 2) + pow(O1 - x1, 2), 3.0/2.0)*cos(w*sqrt(pow(O0 - x0, 2) + pow(O1 - x1, 2)))
           - (pow(O0 - x0, 2) + pow(O1 - x1, 2))*sin(w*sqrt(pow(O0 - x0, 2) + pow(O1 - x1, 2))), 2)/(pow(w, 2)
           *pow(pow(O0 - x0, 2) + pow(O1 - x1, 2), 5)) + 1)) + 0.5*A*(O1 - x1)*(5*O1 - 5*x1)*(w*pow(pow(O0 - x0, 2)
           + pow(O1 - x1, 2), 3.0/2.0)*cos(w*sqrt(pow(O0 - x0, 2) + pow(O1 - x1, 2))) - (pow(O0 - x0, 2)
           + pow(O1 - x1, 2))*sin(w*sqrt(pow(O0 - x0, 2) + pow(O1 - x1, 2))))/(w*pow(pow(O0 - x0, 2)
           + pow(O1 - x1, 2), 7.0/2.0)*sqrt(pow(A, 2)*pow(O0 - x0, 2)*pow(w*pow(pow(O0 - x0, 2)
           + pow(O1 - x1, 2), 3.0/2.0)*cos(w*sqrt(pow(O0 - x0, 2) + pow(O1 - x1, 2))) - (pow(O0 - x0, 2)
           + pow(O1 - x1, 2))*sin(w*sqrt(pow(O0 - x0, 2) + pow(O1 - x1, 2))), 2)/(pow(w, 2)*pow(pow(O0 - x0, 2)
           + pow(O1 - x1, 2), 5)) + pow(A, 2)*pow(O1 - x1, 2)*pow(w*pow(pow(O0 - x0, 2) + pow(O1 - x1, 2), 3.0/2.0)
           *cos(w*sqrt(pow(O0 - x0, 2) + pow(O1 - x1, 2))) - (pow(O0 - x0, 2) + pow(O1 - x1, 2))*sin(w
           *sqrt(pow(O0 - x0, 2) + pow(O1 - x1, 2))), 2)/(pow(w, 2)*pow(pow(O0 - x0, 2) + pow(O1 - x1, 2), 5)) + 1))
           + 0.5*A*(O1 - x1)*(w*pow(pow(O0 - x0, 2) + pow(O1 - x1, 2), 3.0/2.0)*cos(w*sqrt(pow(O0 - x0, 2)
           + pow(O1 - x1, 2))) - (pow(O0 - x0, 2) + pow(O1 - x1, 2))*sin(w*sqrt(pow(O0 - x0, 2) + pow(O1 - x1, 2))))
           *(-1.0/2.0*pow(A, 2)*pow(O0 - x0, 2)*(10*O1 - 10*x1)*pow(w*pow(pow(O0 - x0, 2) + pow(O1 - x1, 2), 3.0/2.0)
           *cos(w*sqrt(pow(O0 - x0, 2) + pow(O1 - x1, 2))) - (pow(O0 - x0, 2) + pow(O1 - x1, 2))*sin(w
           *sqrt(pow(O0 - x0, 2) + pow(O1 - x1, 2))), 2)/(pow(w, 2)*pow(pow(O0 - x0, 2) + pow(O1 - x1, 2), 6))
           - 1.0/2.0*pow(A, 2)*pow(O0 - x0, 2)*(w*pow(pow(O0 - x0, 2) + pow(O1 - x1, 2), 3.0/2.0)*cos(w
           *sqrt(pow(O0 - x0, 2) + pow(O1 - x1, 2))) - (pow(O0 - x0, 2) + pow(O1 - x1, 2))*sin(w*sqrt(pow(O0 - x0, 2)
           + pow(O1 - x1, 2))))*(-2*pow(w, 2)*(-O1 + x1)*(pow(O0 - x0, 2) + pow(O1 - x1, 2))*sin(w*sqrt(pow(O0 - x0, 2)
           + pow(O1 - x1, 2))) + 2*w*(-3*O1 + 3*x1)*sqrt(pow(O0 - x0, 2) + pow(O1 - x1, 2))*cos(w*sqrt(pow(O0 - x0, 2)
           + pow(O1 - x1, 2))) + 2*w*(-O1 + x1)*(-pow(O0 - x0, 2) - pow(O1 - x1, 2))*cos(w*sqrt(pow(O0 - x0, 2)
           + pow(O1 - x1, 2)))/sqrt(pow(O0 - x0, 2) + pow(O1 - x1, 2)) + 2*(2*O1 - 2*x1)*sin(w*sqrt(pow(O0 - x0, 2)
           + pow(O1 - x1, 2))))/(pow(w, 2)*pow(pow(O0 - x0, 2) + pow(O1 - x1, 2), 5)) - 1.0/2.0*pow(A, 2)*(-2*O1 + 2*x1)
           *pow(w*pow(pow(O0 - x0, 2) + pow(O1 - x1, 2), 3.0/2.0)*cos(w*sqrt(pow(O0 - x0, 2) + pow(O1 - x1, 2)))
           - (pow(O0 - x0, 2) + pow(O1 - x1, 2))*sin(w*sqrt(pow(O0 - x0, 2) + pow(O1 - x1, 2))), 2)/(pow(w, 2)
           *pow(pow(O0 - x0, 2) + pow(O1 - x1, 2), 5)) - 1.0/2.0*pow(A, 2)*pow(O1 - x1, 2)*(10*O1 - 10*x1)*pow(w
           *pow(pow(O0 - x0, 2) + pow(O1 - x1, 2), 3.0/2.0)*cos(w*sqrt(pow(O0 - x0, 2) + pow(O1 - x1, 2)))
           - (pow(O0 - x0, 2) + pow(O1 - x1, 2))*sin(w*sqrt(pow(O0 - x0, 2) + pow(O1 - x1, 2))), 2)/(pow(w, 2)
           *pow(pow(O0 - x0, 2) + pow(O1 - x1, 2), 6)) - 1.0/2.0*pow(A, 2)*pow(O1 - x1, 2)*(w*pow(pow(O0 - x0, 2)
           + pow(O1 - x1, 2), 3.0/2.0)*cos(w*sqrt(pow(O0 - x0, 2) + pow(O1 - x1, 2))) - (pow(O0 - x0, 2)
           + pow(O1 - x1, 2))*sin(w*sqrt(pow(O0 - x0, 2) + pow(O1 - x1, 2))))*(-2*pow(w, 2)*(-O1 + x1)*(pow(O0 - x0, 2)
           + pow(O1 - x1, 2))*sin(w*sqrt(pow(O0 - x0, 2) + pow(O1 - x1, 2))) + 2*w*(-3*O1 + 3*x1)*sqrt(pow(O0 - x0, 2)
           + pow(O1 - x1, 2))*cos(w*sqrt(pow(O0 - x0, 2) + pow(O1 - x1, 2))) + 2*w*(-O1 + x1)*(-pow(O0 - x0, 2)
           - pow(O1 - x1, 2))*cos(w*sqrt(pow(O0 - x0, 2) + pow(O1 - x1, 2)))/sqrt(pow(O0 - x0, 2) + pow(O1 - x1, 2))
           + 2*(2*O1 - 2*x1)*sin(w*sqrt(pow(O0 - x0, 2) + pow(O1 - x1, 2))))/(pow(w, 2)*pow(pow(O0 - x0, 2)
           + pow(O1 - x1, 2), 5)))/(w*pow(pow(O0 - x0, 2) + pow(O1 - x1, 2), 5.0/2.0)*pow(pow(A, 2)*pow(O0 - x0, 2)
           *pow(w*pow(pow(O0 - x0, 2) + pow(O1 - x1, 2), 3.0/2.0)*cos(w*sqrt(pow(O0 - x0, 2) + pow(O1 - x1, 2)))
           - (pow(O0 - x0, 2) + pow(O1 - x1, 2))*sin(w*sqrt(pow(O0 - x0, 2) + pow(O1 - x1, 2))), 2)/(pow(w, 2)
           *pow(pow(O0 - x0, 2) + pow(O1 - x1, 2), 5)) + pow(A, 2)*pow(O1 - x1, 2)*pow(w*pow(pow(O0 - x0, 2)
           + pow(O1 - x1, 2), 3.0/2.0)*cos(w*sqrt(pow(O0 - x0, 2) + pow(O1 - x1, 2))) - (pow(O0 - x0, 2)
           + pow(O1 - x1, 2))*sin(w*sqrt(pow(O0 - x0, 2) + pow(O1 - x1, 2))), 2)/(pow(w, 2)*pow(pow(O0 - x0, 2)
           + pow(O1 - x1, 2), 5)) + 1, 3.0/2.0)) + 0.5*A*(O1 - x1)*(-pow(w, 2)*(-O1 + x1)*(pow(O0 - x0, 2)
           + pow(O1 - x1, 2))*sin(w*sqrt(pow(O0 - x0, 2) + pow(O1 - x1, 2))) + w*(-3*O1 + 3*x1)*sqrt(pow(O0 - x0, 2)
           + pow(O1 - x1, 2))*cos(w*sqrt(pow(O0 - x0, 2) + pow(O1 - x1, 2))) + w*(-O1 + x1)*(-pow(O0 - x0, 2)
           - pow(O1 - x1, 2))*cos(w*sqrt(pow(O0 - x0, 2) + pow(O1 - x1, 2)))/sqrt(pow(O0 - x0, 2) + pow(O1 - x1, 2))
           + (2*O1 - 2*x1)*sin(w*sqrt(pow(O0 - x0, 2) + pow(O1 - x1, 2))))/(w*pow(pow(O0 - x0, 2)
           + pow(O1 - x1, 2), 5.0/2.0)*sqrt(pow(A, 2)*pow(O0 - x0, 2)*pow(w*pow(pow(O0 - x0, 2)
           + pow(O1 - x1, 2), 3.0/2.0)*cos(w*sqrt(pow(O0 - x0, 2) + pow(O1 - x1, 2))) - (pow(O0 - x0, 2)
           + pow(O1 - x1, 2))*sin(w*sqrt(pow(O0 - x0, 2) + pow(O1 - x1, 2))), 2)/(pow(w, 2)*pow(pow(O0 - x0, 2)
           + pow(O1 - x1, 2), 5)) + pow(A, 2)*pow(O1 - x1, 2)*pow(w*pow(pow(O0 - x0, 2) + pow(O1 - x1, 2), 3.0/2.0)
           *cos(w*sqrt(pow(O0 - x0, 2) + pow(O1 - x1, 2))) - (pow(O0 - x0, 2) + pow(O1 - x1, 2))*sin(w
           *sqrt(pow(O0 - x0, 2) + pow(O1 - x1, 2))), 2)/(pow(w, 2)*pow(pow(O0 - x0, 2) + pow(O1 - x1, 2), 5)) + 1))
           - 1.0*A*(w*pow(pow(O0 - x0, 2) + pow(O1 - x1, 2), 3.0/2.0)*cos(w*sqrt(pow(O0 - x0, 2) + pow(O1 - x1, 2)))
           - (pow(O0 - x0, 2) + pow(O1 - x1, 2))*sin(w*sqrt(pow(O0 - x0, 2) + pow(O1 - x1, 2))))/(w*pow(pow(O0 - x0, 2)
           + pow(O1 - x1, 2), 5.0/2.0)*sqrt(pow(A, 2)*pow(O0 - x0, 2)*pow(w*pow(pow(O0 - x0, 2) + pow(O1 - x1, 2), 3.0/2.0)
           *cos(w*sqrt(pow(O0 - x0, 2) + pow(O1 - x1, 2))) - (pow(O0 - x0, 2) + pow(O1 - x1, 2))*sin(w*sqrt(pow(O0 - x0, 2)
           + pow(O1 - x1, 2))), 2)/(pow(w, 2)*pow(pow(O0 - x0, 2) + pow(O1 - x1, 2), 5)) + pow(A, 2)*pow(O1 - x1, 2)
           *pow(w*pow(pow(O0 - x0, 2) + pow(O1 - x1, 2), 3.0/2.0)*cos(w*sqrt(pow(O0 - x0, 2) + pow(O1 - x1, 2)))
           - (pow(O0 - x0, 2) + pow(O1 - x1, 2))*sin(w*sqrt(pow(O0 - x0, 2) + pow(O1 - x1, 2))), 2)/(pow(w, 2)
           *pow(pow(O0 - x0, 2) + pow(O1 - x1, 2), 5)) + 1));
}

// ************************************************************************* //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //