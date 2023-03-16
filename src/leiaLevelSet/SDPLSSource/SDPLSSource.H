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
    Foam::SDPLSSource

Description
    This is the absract baseclass for the source term in the level-set-equation for two-phase-flow simulation.
    The source term is defined in "A locally signed-distance preserving level set method (SDPLS) for moving interfaces" by M. Fricke et. al..
    https://arxiv.org/abs/2208.01269

SourceFiles
    SDPLSSource.C

\*---------------------------------------------------------------------------*/

#ifndef SDPLSSource_H
#define SDPLSSource_H

#include "typeInfo.H"
#include "autoPtr.H"
#include "runTimeSelectionTables.H"
#include "fvCFD.H"
#include "SourceScheme.H"


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
// namespace fv
// {

// class SourceScheme;

/*---------------------------------------------------------------------------*\
                         Class SDPLSSource Declaration
\*---------------------------------------------------------------------------*/

class SDPLSSource
{
private:

    autoPtr<SourceScheme> discretization_;
    const word mollifier_;

    volScalarField R_;

    volScalarField nonLinearPart_;

public:

    // Static Data Members
    
    TypeName("noSource");

    declareRunTimeSelectionTable
    (
        autoPtr,
        SDPLSSource, 
        Dictionary, 
        (
            const dictionary& dict, const fvMesh& mesh
        ), 
        (dict, mesh)
    );

    // Constructors

        //- Construct from components
        SDPLSSource(const dictionary& dict, const fvMesh& mesh);

    // Selectors

        //- Select default constructed
        static autoPtr<SDPLSSource> New(const fvMesh& mesh);

    //- Destructor
    virtual ~SDPLSSource() = default;

    // Member functions
    tmp<fvScalarMatrix> fvmSDPLSSource(const volScalarField& psi, const volVectorField& U);

    void update(const volScalarField& psi, const volVectorField& U);
    
    void write() const;

protected:
    virtual tmp<volScalarField> nonLinearPart(const volScalarField& R, const volScalarField& psi, const volVectorField& U) const;

private:
    // old name 'minusR', new name 'R', wich mean the same term. 
    tmp<volScalarField> R(const volScalarField& psi, const volVectorField& U) const;

};

// } // End namespace fv

// * * * * * * * * * * * * * *  Global functions  * * * * * * * * * * * * * * //
namespace fvm
{
tmp<fvScalarMatrix> SDPLSSource(const volScalarField& psi, const volVectorField& U);


} // End namespace fvm
} // End namespace Foam

#endif

// ************************************************************************* //
