Info<< "Reading field phi\n" << endl;

volScalarField phi
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


Info<< "Reading field F\n" << endl;

surfaceScalarField F
(
    IOobject
    (
        "F",
        runTime.timeName(),
        mesh,
        IOobject::MUST_READ,
        IOobject::AUTO_WRITE
    ),
    mesh
);

// For F = F0 * cos(pi t / T) 
surfaceScalarField F0 ("F0", F);

// Advection error analysis. 
volScalarField phi0("phi0", phi);

// Heaviside model

const dimensionedScalar epsilon  = 
    1.5 * Foam::max(Foam::pow(mesh.deltaCoeffs(), -1)); /* 1.5h */

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
calcPhaseIndicator(alpha, phi, epsilon);
alpha.write();

// ||grad phi| - 1| error analysis.
volScalarField eGradPhi(
    IOobject
    (
        "egradPhi",
        runTime.timeName(),
        mesh,
        IOobject::NO_READ,
        IOobject::AUTO_WRITE
    ),
    Foam::mag(Foam::mag(fvc::grad(phi)) - 1)
);
eGradPhi.write();
