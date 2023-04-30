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
    The base class SDPLSSource is the root for the model hierarchy of the SDPLS
    method by Fricke et. al. \cite fricke_locally_2022. 
    It is the only class with which the user of the SDPLS method should interact. 
    The SDPLS method extends the standard LS advection equation with the SDPLS source term. 
    This is done by extending the r.h.s. of the LS equation in the solver application by calling

    fvmSDPLSSource(const volScalarField& psi, const volVectorField& U)

    on the RTS selected model and passing a reference to the LS field psi and the
    velocity field U. It returns a tmp<fvScalarMatrix> object for integration into
    the LS equation. This class hierarchy is responsible for providing
    implementations for the formula of non-linear SDPLS source terms. It is assumed
    that all SDPLS source term variants have the following structure and evaluate
    the source in cell \f$\Omega_c\f$ as 
    \f{equation}{ \label{eq:SDPLS source}
        S_{SDPLS}(\psi^{n+1}) = f_{nl}(\psi^{n+1}) \psi^{n+1}_c
    \f}
    with the solution field \f$\psi^{n+1}\f$ at time \f$t^{n+1}\f$, some non-linear
    scalar function \f$f_{nl}(\psi^{n+1})\f$ and \f$\psi^{n+1}_c\f$ the solution
    value in cell \f$\Omega_c\f$. All inherited classes are responsible for
    implementing this non-linear function by overloading the member function
    nonLinearPart(). This base class implements the case of an inactive source term.
    The discretization is done separately by the class hierarchy SourceScheme, which
    is related via the strategy pattern. Furthermore, this class has a strategy for
    a Mollifier, which restricts the active source term to a narrow band around the
    interface.


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
#include "Mollifier.H"
#include "GradPsi.H"


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
    autoPtr<GradPsi> gradPsi_;
    autoPtr<Mollifier> mollifier_;

    // TODO
	// - put R() into SDPLS_R
	// - Don't store R_, nonLinearPart_ member variables
	// - Currently they are member variables to be registered and written
	// - Currently R() is in SDPLS_SOURCE to see the R field even for noSource 
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

    tmp<volVectorField> gradPsi(const volScalarField& psi) const;

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
