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

Info<< "Reading field " << alphaName << "\n" << endl;
volScalarField alpha
(
    IOobject
    (
        alphaName,
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

//- RTS of the phase indicator model.
autoPtr<phaseIndicator> phaseInd = phaseIndicator::New(mesh);

