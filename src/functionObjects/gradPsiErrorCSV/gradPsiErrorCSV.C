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


#include "gradPsiErrorCSV.H"
#include "addToRunTimeSelectionTable.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
namespace functionObjects
{
    defineTypeNameAndDebug(gradPsiErrorCSV, 0);
    addToRunTimeSelectionTable(functionObject, gradPsiErrorCSV, dictionary);
}
}



// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::functionObjects::gradPsiErrorCSV::gradPsiErrorCSV
(
    const word& name,
    const Time& runTime,
    const dictionary& dict
)
:
    fvMeshFunctionObject (name, runTime, dict),
    field_(mesh_.lookupObject<volScalarField>(dict.get<word>("field"))),
    psi_(mesh_.lookupObject<volScalarField>("psi")),
    // gradPsi_(mesh_.lookupObject<volVectorField>("gradPsi")),// Does not work, but why?
    csvFile_(fileName("gradPsiError.csv"),  IOstreamOption(), IOstreamOption::appendType::APPEND)

{
    if ( !fileSize("gradPsiError.csv") && Pstream::myProcNo() == 0)
    {
            // CSV Header 
            csvFile_ << "TIME,"
                << "MAX_DELTA_X,"
                << "MEAN_DELTA_X,"
                << "E_MAX_GRAD_PSI,"
                << "E_MEAN_GRAD_PSI,"
                << "E_NARROW_MAX_GRAD_PSI,"
                << "E_NARROW_MEAN_GRAD_PSI,"
                << "MAX_MAG_GRAD_PSI,"
                << "MEAN_MAG_GRAD_PSI,"
                << "NARROW_MAX_MAG_GRAD_PSI,"
                << "NARROW_MEAN_MAG_GRAD_PSI\n";
    }
    write();
}

bool Foam::functionObjects::gradPsiErrorCSV::write()
{

    const fvMesh& mesh = this->mesh_;
    const Time& runTime = mesh.time();

    const auto deltaX = pow(mesh.deltaCoeffs(),-1).cref();
    const auto max_deltaX = gMax(deltaX);
    const auto mean_deltaX = gAverage(deltaX);
    const auto gradPsiError = field_;
    const auto max_gradPsiError = gMax(gradPsiError);
    const auto mean_gradPsiError = gAverage(gradPsiError);
    scalar max_narrowGradPsiError = 0.0;
    scalar mean_narrowGradPsiError = 0.0;

    const volScalarField magGradPsi = mag(fvc::grad(psi_));
    // const volScalarField magGradPsi = mag(gradPsi_);
    const auto max_magGradPsi = gMax(magGradPsi);
    const auto mean_magGradPsi = gAverage(magGradPsi);
    scalar max_narrowMagGradPsi = 0.0;
    scalar mean_narrowMagGradPsi = 0.0;

    if (mesh.objectRegistry::found("NarrowBand"))
    {
        const auto narrowBand = mesh.lookupObject<volScalarField>("NarrowBand");
        List<scalar> narrowGradPsiError = subset(narrowBand, gradPsiError);
        max_narrowGradPsiError = gMax(narrowGradPsiError);
        mean_narrowGradPsiError = gAverage(narrowGradPsiError);

        List<scalar> narrowMagGradPsi = subset(narrowBand, magGradPsi);
        max_narrowMagGradPsi = gMax(narrowMagGradPsi);
        mean_narrowMagGradPsi = gAverage(narrowMagGradPsi);
    }

    if (Pstream::myProcNo() == 0)
    {
        csvFile_ << runTime.timeOutputValue() << ","
            << max_deltaX << ","
            << mean_deltaX << ","
            << max_gradPsiError << ","
            << mean_gradPsiError << ","
            << max_narrowGradPsiError << ","
            << mean_narrowGradPsiError << ","
            << max_magGradPsi << ","
            << mean_magGradPsi << ","
            << max_narrowMagGradPsi << ","
            << mean_narrowMagGradPsi << "\n";
    }
    return true;
}



// ************************************************************************* //
