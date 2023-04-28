/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2022 Julian Reitzel, TU Darmstadt
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

Class
    Foam::SourceScheme

Description
    This is the absract baseclass for the source term in the level-set-equation for two-phase-flow simulation.
    The source term is defined in "A locally signed-distance preserving level set method (SDPLS) for moving interfaces" by M. Fricke et. al..
    https://arxiv.org/abs/2208.01269

      \f[ y = mx + b \f]
 
     \f[ y \approx mx + b \f]
 
     The distance between \f$(x_1,y_1)\f$ and \f$(x_2,y_2)\f$ is \f$\sqrt{(x_2-x_1)^2+(y_2-y_1)^2}\f$.
 
     \f[
       |I_2|=\left| \int_{0}^T \psi(t) 
                \left\{ 
                   u(a,t)-
                   \int_{\gamma(t)}^a 
                   \frac{d\theta}{k(\theta,t)}
                   \int_{a}^\theta c(\xi)u_t(\xi,t)\,d\xi
                \right\} dt
             \right|
     \f]
 
   \f[
    \hat{S}(\phi(\x,t)) \approx \hat{S}_c + \hat{S}_{p} \phi(\x,t)
   \f] 
       
   \f{equation}{
       \label{eq:linearization}
       \hat{S}(\phi(\x,t)) \approx \hat{S}_c + \hat{S}_{p} \phi(\x,t)
   \f}
  
   \f{equation}{
   \begin{aligned}
     a + b &= 1 \\
     b &= 456
   \end{aligned}
   \f}

SourceFiles
    SourceScheme.C

\*---------------------------------------------------------------------------*/

#ifndef SourceScheme_H
#define SourceScheme_H

#include "typeInfo.H"
#include "autoPtr.H"
#include "runTimeSelectionTables.H"
#include "fvCFD.H"


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
// namespace fv
// {

class SDPLSSource;


/*---------------------------------------------------------------------------*\
                         Class SourceScheme Declaration
\*---------------------------------------------------------------------------*/

// template<class Type>
class SourceScheme
{
protected:

    scalarField Sc_;
    scalarField Sp_;

public:

    // Static Data Members
    
    TypeName("none");

    declareRunTimeSelectionTable
    (
        autoPtr,
        SourceScheme, 
        Dictionary, 
        (), 
        ()
    );

    // Constructors

    //- Construct from components
    SourceScheme();

    // Selectors

    //- Select default constructed
    static autoPtr<SourceScheme> New(const word type);

    //- Destructor
    virtual ~SourceScheme() = default;

    // Member functions

    tmp<fvScalarMatrix> discretize(const volScalarField& nonLinearPart, const volScalarField& psi);

protected:
    virtual void updateSc(const volScalarField& nonLinearPart, const volScalarField& psi);
    virtual void updateSp(const volScalarField& nonLinearPart);


};

// } // End namespace fv


} // End namespace Foam

#endif

// ************************************************************************* //
