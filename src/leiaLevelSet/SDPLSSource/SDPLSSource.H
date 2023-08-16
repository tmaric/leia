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
    The base class SDPLSSource is the root for the model hierarchy of the Signed
    Distance Preserving Level Set method by Fricke et al. \cite fricke_locally_2022. 
    It is the only class with which the user of the SDPLS
    method should interact. The SDPLS method extends the standard LS advection
    equation with the SDPLS source term. This is done by extending the r.h.s. of the
    LS equation in the solver application by calling

    fvmSDPLSSource(const volScalarField& psi, const volVectorField& U)

    on the RTS selected model and passing a reference to the LS field psi and
    the velocity field U. It returns a tmp<fvScalarMatrix> object for
    integration into the LS equation. This class hierarchy is responsible for
    providing implementations for the formula of non-linear SDPLS source terms. It
    is assumed that all SDPLS source term implementation will have the following
    structure and evaluate the source in cell \f$\Omega_c\f$ as
    \f{equation}{
        \label{eq:SDPLS-source}
        S_{SDPLS}(\psi^{n+1}) = f_{nl}(\psi^{n}) \psi^{n+1}_c
    \f}
    with the known solution field \f$\psi^{n}\f$ at time \f$t^{n}\f$, some
    non-linear scalar function \f$f_{nl}(\psi^{n})\f$ and \f$\psi^{n+1}_c\f$ the
    unknown solution value in cell \f$\Omega_c\f$ at time \f$t^{n+1}\f$. All
    inherited classes are responsible for implementing the explicit evaluation
    of the non-linear function by overloading the member function
    nonLinearPart(). This base class implements the case of an inactive source term
    \f$f_{nl}=0\f$.
    The discretization is done separately by the class hierarchy SourceScheme, which
    is related via the strategy pattern. Furthermore, this class has a strategy for
    a Mollifier, which restricts the active source term to a narrow band around the
    interface.

Usage
    For selecting the \c noSource SDPLS source term you may provide the 
    sub-dictionary 
    \verbatim
    SDPLSSource
    {
        type noSource;
    }
    \endverbatim
    in \c system/fvSolution.levelSet, but \c noSource is selected by default.
    The Level Set equation in a solver application needs to call 
    \c fvmSDPLSSource() and may look like
    \verbatim
    fvScalarMatrix psiEqn
    (
        fvm::ddt(psi)
        + fvm::div(phi, psi)
        ==
        source->fvmSDPLSSource(psi, U)
    );
    \endverbatim
    with \c source being selected in \c createFields.H as
    \verbatim
    autoPtr<SDPLSSource> source = SDPLSSource::New(mesh); 
    \endverbatim

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


/*---------------------------------------------------------------------------*\
                         Class SDPLSSource Declaration
\*---------------------------------------------------------------------------*/

class SDPLSSource
{
private:

    // Private Data

        //- Strategy for the discretization of the SDPLS source term
        autoPtr<SourceScheme> discretization_;

        //- Strategy for the gradient evaluation of the LS field
        autoPtr<GradPsi> gradPsi_;

        //- Strategy for a mollifier limiting the active source term to a 
        //  narrow band.
        autoPtr<Mollifier> mollifier_;

        //- Registered field, corresponds to -R in the source term of 
        //  Fricke et al.
        //
        //  TODO
        //  - put R() into SDPLS_R
        //  - Don't store R_, nonLinearPart_ member variables
        //  - Currently they are member variables to be registered and 
        //      written
        //  - Currently R() is in SDPLS_SOURCE to see the R field even for 
        //      noSource 
        volScalarField R_;

        //- Registered field for visualizing the non-linear part 
        //  \f$f_{nl}(\psi^{n})\f$ of an SDPLS source term.
        volScalarField nonLinearPart_;


    // Private Member Functions

        //- Calculate the non-linear function R by Fricke et al.
        //  The calculation is explicit, using the values from the old time 
        //  step. The method is placed in the base class on purpose. The R_ 
        //  field is written also for 'noSource' for development purpose.
        //  \f{equation}{
        //      R = \left \langle (\nabla \U)_c \frac{(\nabla
        //      \psi)_c^n}{\|(\nabla\psi)_c^n\|_2},  \frac{(\nabla
        //      \psi)_c^n}{\|(\nabla\psi)_c^n\|_2} \right \rangle, 
        //  \f} 
        //  Note: This 'R' corresponds to '-R' of Fricke et al.
        //  
        //  @param psi is the Level Set field
        //  @param U is the velocity field
        tmp<volScalarField> 
        R(const volScalarField& psi, const volVectorField& U) const;


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

        //- Select from fvSolution
        static autoPtr<SDPLSSource> New(const fvMesh& mesh);

    //- Destructor
    virtual ~SDPLSSource() = default;


    // Member functions

        //- Returns the discretized SDPLS source term as matrix.
        //
        //  This is the main function to be called in an equation of a solver.
        //
        //  @param psi is the Level Set field
        //  @param U is the velocity field
        tmp<fvScalarMatrix> 
        fvmSDPLSSource(const volScalarField& psi, const volVectorField& U);


        //- Updates the member field R_ and nonLinearPart_.
        //
        //  This function is called from fvmSDPLSSource(). 
        //  The user usually does not need to call this function. 
        //  It is just public to be called from createFields.H to initialize 
        //  the fields different from 0 and write them at time 0.  
        //
        //  @param psi is the Level Set field
        //  @param U is the velocity field
        void update(const volScalarField& psi, const volVectorField& U);
        
        //- Write private data members
        void write() const;

        bool iterative();


protected:

    // Protected Member Functions

        //- Calculate the non-linear function \f$f_{nl}(\psi^{n})\f$.
        //  The calculation is explicit, using the values from the old time 
        //  step. This instance returns a zero field.
        virtual tmp<volScalarField> nonLinearPart
        (
            const volScalarField& R,
            const volScalarField& psi,
            const volVectorField& U
        ) const;

        //- Calculates the gradient field of psi with the selected strategy.
        //
        //  @param psi is the Level Set field
        tmp<volVectorField> gradPsi(const volScalarField& psi) const;
};


// * * * * * * * * * * * * * *  Global functions  * * * * * * * * * * * * * * //
namespace fvm
{

    //- Global function which returns the discretized SDPLS source term as
    //  matrix.
    //
    //  Mimics the the structure of other call hierarchies of PDE operator.
    tmp<fvScalarMatrix>
    SDPLSSource(const volScalarField& psi, const volVectorField& U);


} // End namespace fvm
} // End namespace Foam

#endif

// ************************************************************************* //
