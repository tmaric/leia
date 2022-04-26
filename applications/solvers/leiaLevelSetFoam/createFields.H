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

// Phase-indicator  
// FIXME: Use phase indicator name from transport properties. TM. 
Info<< "Reading field alpha\n" << endl;
volScalarField alpha
(
    IOobject
    (
        "alpha",
        runTime.timeName(),
        mesh,
        IOobject::MUST_READ,
        IOobject::AUTO_WRITE
    ),
    mesh
);

// Volumetric flux
Info<< "Reading field phi\n" << endl;
surfaceScalarField phi 
(
    IOobject
    (
        "phi",
        runTime.timeName(),
        mesh,
        IOobject::NO_READ,
        IOobject::AUTO_WRITE
    ),
    mesh,
    dimensionedScalar ("phi", dimVolume / dimTime, 0)
);

// Velocity 
Info<< "Initializing field U (visualization)\n" << endl;
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


// Runtime Type Selection 
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
    phaseIndDict.getOrDefault<word>("type", "geometric");
autoPtr<phaseIndicator> phaseInd = 
    phaseIndicator::New(phaseIndType, phaseIndDict); 

//- compute and write initial phase-indicator.
phaseInd->calcPhaseIndicator(alpha, psi);
alpha.write();

// Advection Velocity 
const dictionary& velocityDict = fvSolution.subDict("velocityModel");
const word velocityType = velocityDict.getOrDefault<word>("type", "none");
autoPtr<velocityModel> velocityModel = 
    velocityModel::New(velocityType, velocityDict); 

// Set initial volumetric flux and velocity
setVolumetricFlux(phi, velocityModel); 
phi.write(); 

// phi is used for transport, U for visualization 
setVelocity(U, velocityModel);
U.write();


