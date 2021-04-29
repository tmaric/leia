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

surfaceScalarField F 
(
    IOobject
    (
        "F", 
        runTime.timeName(), 
        mesh, 
        IOobject::NO_READ, 
        IOobject::NO_WRITE
    ),
    mesh, 
    dimensionedScalar("F", dimVelocity * dimArea, 0)
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
