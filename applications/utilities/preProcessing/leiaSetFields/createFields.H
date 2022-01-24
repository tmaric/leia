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

surfaceScalarField phi 
(
    IOobject
    (
        "phi", 
        runTime.timeName(), 
        mesh, 
        IOobject::NO_READ, 
        IOobject::NO_WRITE
    ),
    mesh, 
    dimensionedScalar("phi", dimVelocity * dimArea, 0)
);

volVectorField U 
(
    IOobject
    (
        "U", 
        runTime.timeName(), 
        mesh, 
        IOobject::NO_READ, 
        IOobject::AUTO_WRITE
    ),
    mesh, 
    dimensionedVector("U", dimVelocity, vector(0,0,0))
);

// Heaviside model
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
const dictionary& phaseIndDict = levelSetDict.subDict("phaseIndicator");
const word phaseIndType = phaseIndDict.getOrDefault<word>("type", "geometric");
autoPtr<phaseIndicator> phaseInd = phaseIndicator::New
(
    phaseIndType,
    phaseIndDict
);

