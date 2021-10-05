Info<< "Reading field psi\n" << endl;

volScalarField psi 
(
    IOobject
    (
        "psi",
        runTime.timeName(),
        mesh,
        IOobject::MUST_READ,
        IOobject::AUTO_WRITE
    ),
    mesh
);


Info<< "Reading field F\n" << endl;

surfaceScalarField phi 
(
    IOobject
    (
        "phi",
        runTime.timeName(),
        mesh,
        IOobject::MUST_READ,
        IOobject::AUTO_WRITE
    ),
    mesh
);

// For phi = phi0 * cos(pi t / T) 
surfaceScalarField phi0 ("phi0", phi);

// Advection error analysis. 
volScalarField psi0
(
    IOobject
    (
        "psi0",
        runTime.timeName(),
        mesh,
        IOobject::NO_READ,
        IOobject::AUTO_WRITE
    ),
    psi 
);
volScalarField epsi 
(
    IOobject
    (
        "ePsi",
        runTime.timeName(),
        mesh,
        IOobject::NO_READ,
        IOobject::AUTO_WRITE
    ),
    mesh, 
    dimensionedScalar("ePsi", dimLength, 0)
);

// Marker field 
volScalarField alpha 
(
    IOobject
    (
        "alpha",
        runTime.timeName(),
        mesh,
        IOobject::NO_READ,
        IOobject::AUTO_WRITE
    ),
    mesh, 
    dimensionedScalar("alpha", dimless, 0)
);

// RTS of the phase indicator model.
const fvSolution& fvSolution (mesh);
const dictionary& levelSetDict = fvSolution.subDict("levelSet");
autoPtr<phaseIndicator> phaseInd = 
    phaseIndicator::New(levelSetDict.subDict("phaseIndicator"));
phaseInd->calcPhaseIndicator(alpha, psi);
// Write initial indicator.
alpha.write();

// Alpha error analysis.
volScalarField alpha0 ("alpha0", alpha);
