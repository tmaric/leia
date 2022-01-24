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

// RTS 
const fvSolution& fvSolution (mesh);
const dictionary& levelSetDict = fvSolution.subDict("levelSet");

// Level Set Method
//- redistancer selection
const dictionary& redistDict = levelSetDict.subDict("redistancer");
const word& redistType = 
    redistDict.getOrDefault<word>("type", "noRedistancing");
autoPtr<redistancer> redist = redistancer::New(redistType, redistDict);

//- phase-indicator selection
const dictionary& phaseIndDict = levelSetDict.subDict("phaseIndicator");
const word& phaseIndType = 
    redistDict.getOrDefault<word>("type", "geometric");
autoPtr<phaseIndicator> phaseInd = 
    phaseIndicator::New(phaseIndType, phaseIndDict); 
