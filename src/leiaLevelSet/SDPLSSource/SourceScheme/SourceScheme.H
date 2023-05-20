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
    The base class SourceScheme forms the root for the discretization hierarchy of
    the SDPLS source term implemented in SDPLSSource. For the discretization, the
    non-linear SDPLS source term \f$S_{SDPLS}\f$ is linearized as 
    \f{equation}{
        S_{SDPLS}(\psi^{n+1}) \approx S_c + S_{p} \psi^{n+1}_c,
    \f}
    where \f$S_c\f$ is a constant and \f$S_{p}\f$ is the proportionality constant
    for the linear part. Initializing the discretization and returning the
    tmp<fvScalarMatrix> is done by the public method

    discretize(nonLinearPart, psi),

    which takes the nonLinearPart and the LS field psi from the source term
    implementing the class SDPLSSource. Inheriting classes are responsible for
    implementing updateSc() for \f$S_c\f$ and updateSp() for \f$S_p\f$, which are
    called by discretize(). This base class implements the case of no discretization
    with 
    \f{equation}{ 
        S_{noSource}(\psi^{n+1})=0. 
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
