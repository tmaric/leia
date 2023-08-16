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

\*---------------------------------------------------------------------------*/

#include "SDPLSSource.H"
#include "addToRunTimeSelectionTable.H"
#include "fvSolution.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
    defineTypeNameAndDebug(SDPLSSource, false);
    defineRunTimeSelectionTable(SDPLSSource, Dictionary);
    addToRunTimeSelectionTable(SDPLSSource, SDPLSSource, Dictionary);
} 

// * * * * * * * * * * * * * * * * Selectors * * * * * * * * * * * * * * * * //


Foam::autoPtr<SDPLSSource>
Foam::SDPLSSource::New(const fvMesh& mesh)
{
    const fvSolution& fvSolution (mesh);
    const dictionary& levelSetDict = fvSolution.subDict("levelSet");
    const dictionary& sourceTermDict = 
        levelSetDict.optionalSubDict("SDPLSSource");
    const word& type = sourceTermDict.getOrDefault<word>("type", "noSource");

    auto* ctorPtr = DictionaryConstructorTable(type);

    if (!ctorPtr)
    {
        FatalIOErrorInLookup
        (
            fvSolution,
            "SDPLSSource",
            type,
            *DictionaryConstructorTablePtr_
        ) << exit(FatalIOError);
    }

    Info << "Selecting SDPLS source term type: " << type << nl << endl;
    return autoPtr<SDPLSSource>(ctorPtr(sourceTermDict, mesh));
}

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::SDPLSSource::SDPLSSource(const dictionary& dict, const fvMesh& mesh)
    :
        discretization_
            (
                SourceScheme::
                New(dict.getOrDefault<word>("discretization","none"))
            ),
        gradPsi_(GradPsi::New(dict.getOrDefault<word>("gradPsi","fvc"), mesh)),
        mollifier_(Mollifier::New(dict.subOrEmptyDict("mollifier"))),
        R_(
            IOobject(
                "R",
                mesh.time().timeName(),
                mesh,
                IOobject::NO_READ,
                IOobject::AUTO_WRITE
                ), 
            mesh, 
            dimensioned(dimless/dimTime, 0.0)
        ),
        nonLinearPart_(
            IOobject
            (
                "SDPLS_nonLinearPart",
                mesh.time().timeName(),
                mesh,
                IOobject::NO_READ,
                IOobject::AUTO_WRITE
            ),
            mesh,
            dimensionedScalar(dimless/dimTime, 0.0)
        )
{}

// * * * * * * * * * * * * * *  Member functions  * * * * * * * * * * * * * * //

Foam::tmp<fvScalarMatrix> 
Foam::SDPLSSource::
fvmSDPLSSource(const volScalarField& psi, const volVectorField& U)
{
    update(psi, U);
    tmp<fvScalarMatrix> tfvm;
    if(mollifier_->type() == "none")
    {
        tfvm = discretization_->discretize(nonLinearPart_, psi);
    }
    else
    {
        volScalarField mollified_nonLinearPart =
            nonLinearPart_*mollifier_->field(psi);
        tfvm = discretization_->discretize(mollified_nonLinearPart, psi);
    }

    return tfvm;
}

void
Foam::SDPLSSource::update(const volScalarField& psi, const volVectorField& U)
{
    R_ = R(psi, U);
    nonLinearPart_ = nonLinearPart(R_, psi, U);
}


Foam::tmp<volScalarField> 
Foam::SDPLSSource::R(const volScalarField& psi, const volVectorField& U) const
{
    volVectorField const grad_psi = gradPsi(psi);
    volTensorField const grad_U = fvc::grad(U).cref();
    dimensioned<scalar> const eps = dimensioned(grad_psi.dimensions(), SMALL);
    volVectorField const normal_interface = 
        (grad_psi/(mag(grad_psi) + eps)).cref();

    return (grad_U & normal_interface) & normal_interface;
}

Foam::tmp<volVectorField>
Foam::SDPLSSource::gradPsi(const volScalarField& psi) const
{
    return gradPsi_->grad(psi);
}

Foam::tmp<volScalarField> 
Foam::SDPLSSource::nonLinearPart
    (
        const volScalarField& R,
        const volScalarField& psi,
        const volVectorField& U
    ) const
{
    const fvMesh& mesh = psi.mesh();
    return tmp<volScalarField>
    (
        new volScalarField
        (
            IOobject
            (
                word(),
                fileName(),
                mesh,
                IOobject::NO_READ,
                IOobject::NO_WRITE,
                false
            ),
            mesh,
            dimensionedScalar(nonLinearPart_.dimensions(), 0.0)
        )
    );
}

void Foam::SDPLSSource::write() const
{
    R_.write();
    nonLinearPart_.write();
}

bool Foam::SDPLSSource::iterative()
{
    return discretization_->iterative();
}

// * * * * * * * * * * * * * *  Global functions  * * * * * * * * * * * * * * //

Foam::tmp<fvScalarMatrix> 
Foam::fvm::SDPLSSource(const volScalarField& psi, const volVectorField& U)
{
    return SDPLSSource::New(psi.mesh()).ref().fvmSDPLSSource(psi, U);
}

// ************************************************************************* //




// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //
