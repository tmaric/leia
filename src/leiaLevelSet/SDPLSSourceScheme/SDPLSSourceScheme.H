/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2022 AUTHOR,AFFILIATION
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
    Foam::SDPLSSourceScheme

Description
    This is the absract baseclass for the source term in the level-set-equation for two-phase-flow simulation.
    The source term is defined in "A locally signed-distance preserving level set method (SDPLS) for moving interfaces" by M. Fricke et. al..
    https://arxiv.org/abs/2208.01269

SourceFiles
    SDPLSSourceSchemeI.H
    SDPLSSourceScheme.C
    SDPLSSourceSchemeIO.C

\*---------------------------------------------------------------------------*/

#ifndef SDPLSSourceScheme_H
#define SDPLSSourceScheme_H

#include "typeInfo.H"
#include "autoPtr.H"
#include "runTimeSelectionTables.H"
#include "fvSolution.H"
#include "fvMesh.H"
#include "fvCFD.H"
// #include "narrowBand.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
namespace fv
{
/*---------------------------------------------------------------------------*\
                         Class SDPLSSourceScheme Declaration
\*---------------------------------------------------------------------------*/

// template<class Type>
class SDPLSSourceScheme
{
protected:

    const fvSolution& fvSolution_;
    const dictionary& levelSetDict_;
    const dictionary& sourceTermDict_;
    // const Switch narrowBand_;
    // autoPtr<narrowBand> nbptr_;

public:

    // Static Data Members
    
    TypeName("noSource");

    declareRunTimeSelectionTable
    (
        autoPtr,
        SDPLSSourceScheme, 
        Dictionary, 
        (
            const fvMesh& mesh 
        ), 
        (mesh)
    );

    // Constructors

        //- Construct from components
        SDPLSSourceScheme(const fvMesh& mesh);

    // Selectors

        //- Select default constructed
        static autoPtr<SDPLSSourceScheme> New(const fvMesh& mesh);

    //- Destructor
    virtual ~SDPLSSourceScheme() = default;

    // Member functions

    tmp<fvScalarMatrix> fvmSDPLSSource(const volScalarField& psi, const volVectorField& U);

    

protected:

    // Member functions
    virtual tmp<fvScalarMatrix> doFvmSDPLSSource(const volScalarField& psi, const volVectorField& U);

    tmp<volScalarField> minusR(const volScalarField& psi, const volVectorField& U);

private:

    void deactivateFarfield(fvScalarMatrix& fvm);

    // const volScalarField& getNarrowBand();

};

} // End namespace fv

// * * * * * * * * * * * * * *  Global functions  * * * * * * * * * * * * * * //
namespace fvm
{
tmp<fvScalarMatrix> SDPLSSource(const volScalarField& psi, const volVectorField& U);


} // End namespace fvm
} // End namespace Foam

#endif

// ************************************************************************* //
