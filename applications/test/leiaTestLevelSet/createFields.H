// Signed distance
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

volScalarField psi0("psi0", psi);

// Phase-indicator  
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

// Runtime Type Selection 
const fvSolution& fvSolution (mesh);
const dictionary& levelSetDict = fvSolution.subDict("levelSet");

//- redistancer selection
autoPtr<redistancer> redist = 
    redistancer::New(levelSetDict.subDict("redistancer"));

//- phase-indicator selection
autoPtr<phaseIndicator> phaseInd = 
    phaseIndicator::New(levelSetDict.subDict("phaseIndicator"));
