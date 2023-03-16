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

// Leve Set re-distancing model
autoPtr<redistancer> redist = redistancer::New(mesh);

// Level Set phase-indicator model
autoPtr<phaseIndicator> phaseInd = phaseIndicator::New(mesh); 

phaseInd->calcPhaseIndicator(alpha, psi);
alpha.write();

// Level Set interfaceBand model (optional)
autoPtr<interfaceBand> interfaceBand = interfaceBand::New(mesh, psi); 
interfaceBand->calc();
interfaceBand->write();

// SDPLS Source term
autoPtr<SDPLSSource> source = SDPLSSource::New(mesh); 
source->update(psi, U);
source->write();

// Advection velocity model
autoPtr<velocityModel> velocityModel = velocityModel::New(mesh); 

velocityModel->setVolumetricFlux(phi); 
if (args.found("fluxCorrection"))
    correctFlux(phi);
phi.write(); 

velocityModel->setVelocity(U);
U.write();

