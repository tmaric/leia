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

\*---------------------------------------------------------------------------*/

#include "SDPLSSourceScheme.H"
#include "addToRunTimeSelectionTable.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
namespace fv
{

defineTypeNameAndDebug(SDPLSSourceScheme, false);
defineRunTimeSelectionTable(SDPLSSourceScheme, Dictionary);
addToRunTimeSelectionTable(SDPLSSourceScheme, SDPLSSourceScheme, Dictionary);

// * * * * * * * * * * * * * * * * Selectors * * * * * * * * * * * * * * * * //


Foam::autoPtr<SDPLSSourceScheme>
SDPLSSourceScheme::New(const fvMesh& mesh)
{
    const fvSolution& fvSolution (mesh);
    const dictionary& levelSetDict = fvSolution.subDict("levelSet");
    const dictionary& sourceTermDict = levelSetDict.optionalSubDict("SDPLSSourceTerm");
    const word& discretization = 
        sourceTermDict.getOrDefault<word>("discretization", "noSource");

    auto* ctorPtr = DictionaryConstructorTable(discretization);

    if (!ctorPtr)
    {
        FatalIOErrorInLookup
        (
            fvSolution,
            "SDPLSSourceScheme",
            discretization,
            *DictionaryConstructorTablePtr_
        ) << exit(FatalIOError);
    }

    // Construct the model and return the autoPtr to the object. 
    return autoPtr<SDPLSSourceScheme>(ctorPtr(mesh));
}

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

SDPLSSourceScheme::SDPLSSourceScheme(const fvMesh& mesh)
    :
        fvSolution_(mesh),
        levelSetDict_(fvSolution_.subDict("levelSet")),
        sourceTermDict_(levelSetDict_.optionalSubDict("SDPLSSourceTerm"))
        // narrowBand_(sourceTermDict_.getOrDefault<Switch>("narrowBand", Switch("off"))),
        // nbptr_(narrowBand::New(mesh))
{}

// * * * * * * * * * * * * * *  Member functions  * * * * * * * * * * * * * * //

tmp<fvScalarMatrix> SDPLSSourceScheme::fvmSDPLSSource(const volScalarField& psi, const volVectorField& U)
{
    tmp<fvScalarMatrix> tfvm = doFvmSDPLSSource(psi, U);
    
    // fvScalarMatrix& fvm = tfvm.ref();
    // if (narrowBand_)
    // {
    //     Info << "NarrowBand ON, deactivating SDPLSSource in farfield\n";
    //     deactivateFarfield(fvm);
    // }

    // // Debug Start
    // tmp<volScalarField> tSourceCells
    // (
    //     new volScalarField 
    //     (
    //         IOobject
    //         (
    //             "SourceCells",
    //             psi.mesh().time().timeName(),
    //             psi.mesh(),
    //             IOobject::NO_READ,
    //             IOobject::NO_WRITE
    //         ),
    //         psi.mesh(),
    //         fvm.dimensions(),
    //         fvm.source()
    //     )
    // );
    // if (psi.mesh().time().writeTime())
    // {
    //     Info << "Write SourceCells \n";
    //     tSourceCells.cref().write();
    // }
    // // Debug End

    return tfvm;
}

// void SDPLSSourceScheme::deactivateFarfield(fvScalarMatrix& fvm)
// {
//     // const fvMesh& mesh = fvm.psi().mesh();
//     // if (! mesh.objectRegistry::found("narrowBand"))
//     // {
//     //     FatalErrorInFunction
//     //     << "Can not deactivate Farfield because written narrowBand can not be found " << exit(FatalError);
//     // }
//     // const volScalarField& narrowBand = mesh.lookupObject<volScalarField>("narrowBand");

//     const volScalarField& narrowBand = getNarrowBand();

//     const labelList& passiveCells = findIndices(narrowBand, 0);
//     scalarField& diag = fvm.diag();
//     scalarField& source = fvm.source();
//     forAll(passiveCells, index)
//     {
//         const label cellID = passiveCells[index];
//         diag[cellID] = 0;
//         source[cellID] = 0;
//         // No need to set offdiagonal coefficients, 
//         // because SDPLSSource discretizations does not have offdiagonal entries yet.
//     }
// }

tmp<fvScalarMatrix> SDPLSSourceScheme::doFvmSDPLSSource(const volScalarField& psi, const volVectorField& U)
{
        tmp<fvScalarMatrix> tfvm
        (
            new fvScalarMatrix
            (
                psi,
                psi.dimensions()*dimVol/dimTime
            )
        );
    
        return tfvm;
}

tmp<volScalarField> SDPLSSourceScheme::minusR(const volScalarField& psi, const volVectorField& U)
{
    const fvMesh& mesh = psi.mesh();
    const Time& runtime = mesh.time(); 
    
    volVectorField const grad_psi = fvc::grad(psi).cref();

    volTensorField const grad_U = fvc::grad(U).cref();
    
    dimensioned<scalar> const eps = dimensioned(grad_psi.dimensions(), SMALL);
    volVectorField const normal_interface = (grad_psi / (mag(grad_psi) + eps)).cref();

    tmp<volScalarField> tminusR
    (
        new volScalarField 
        (
            IOobject
            (
                "minusR",
                runtime.timeName(),
                mesh,
                IOobject::NO_READ,
                IOobject::NO_WRITE
            ),
            ((grad_U & normal_interface) & normal_interface)
        )
    );

    if (runtime.writeTime())
    {
        Info<< "Write minusR field\n" << endl;
        tminusR.cref().write();
    }

    return tminusR;
}

// const volScalarField& SDPLSSourceScheme::getNarrowBand()
// {
//     return nbptr_->getField();
// } 

} // End namespace fv

// * * * * * * * * * * * * * *  Global functions  * * * * * * * * * * * * * * //
namespace fvm
{
tmp<fvScalarMatrix> SDPLSSource(const volScalarField& psi, const volVectorField& U)
{
    return fv::SDPLSSourceScheme::New(psi.mesh()).ref().fvmSDPLSSource(psi, U); 
}

// ************************************************************************* //

} // End namespace fvm
} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //
