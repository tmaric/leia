// Initialize fields for error analysis 
surfaceScalarField phi0 ("phi0", phi);
volVectorField U0("U0", U);

// Use psiEnd if present as field, otherwise copy psi
volScalarField psi0
(
    IOobject
    (
        "psiEnd",
        runTime.timeName(),
        mesh,
        IOobject::READ_IF_PRESENT,
        IOobject::NO_WRITE
    ),
    psi
);

// Use alphaEnd if present as field, otherwise copy alpha
volScalarField alpha0
(
    IOobject
    (
        "alphaEnd",
        runTime.timeName(),
        mesh,
        IOobject::READ_IF_PRESENT,
        IOobject::NO_WRITE
    ),
    alpha
);
// For example psiEnd, alphaEnd get written in the 3Dtranslation test case by the Allrun scripts. 
// leiaSetFiels is called in a different context for creating psiEnd and alphaEnd. See 3Dtranslation/system/init_End/ for further info. 


// Open error file for output 
OFstream errorFile("leiaLevelSetFoam.csv"); 

// CSV Header 
errorFile << "TIME,"
    << "DELTA_X,"
    << "L_INF_E_PSI,"
    << "E_VOL_ALPHA,"
    << "E_VOL_ALPHA_REL,"
    << "E_GEOM_ALPHA,"
    << "E_GEOM_ALPHA_REL,"
    << "E_BOUND_ALPHA,"
    << "ELAPSED_CPU_TIME,"
    << "MAX_CFL,"
    << "MAX_ABS_SUM_VOL_FLUXES\n";
    
#include "CourantNo.H"

// Report initial errors 
reportErrors(errorFile, psi, psi0, alpha, alpha0, phi, CoNum);
