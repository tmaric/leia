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

// RTS of the level-set profile
const dictionary& profileDict = levelSetDict.subDict("profile");
const word profileType = profileDict.get<word>("type");
autoPtr<profile> LSprofile_ptr = profile::New
(
    mesh, 
    implSurf.ref() 
);
profile LSprofile = LSprofile_ptr.ref();

//- RTS of the phase indicator model.
autoPtr<phaseIndicator> phaseInd = phaseIndicator::New(mesh);

