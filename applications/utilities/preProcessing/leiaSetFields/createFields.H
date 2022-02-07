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

const fvSolution& fvSolution (mesh);
const dictionary& levelSetDict = fvSolution.subDict("levelSet");

//- RTS of the implicit surface 
const dictionary& implSurfDict = levelSetDict.subDict("implicitSurface");
const word implSurfType = implSurfDict.get<word>("type"); 
autoPtr<implicitSurface> implSurf = implicitSurface::New
(
    implSurfType, 
    implSurfDict 
);

//- RTS of the phase indicator model.
const dictionary& phaseIndDict = levelSetDict.subDict("phaseIndicator");
const word phaseIndType = phaseIndDict.getOrDefault<word>("type", "geometric");
autoPtr<phaseIndicator> phaseInd = phaseIndicator::New
(
    phaseIndType,
    phaseIndDict
);

