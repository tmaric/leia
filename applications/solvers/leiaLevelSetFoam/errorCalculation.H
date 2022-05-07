// Initialize fields for error analysis 
surfaceScalarField phi0 ("phi0", phi);
volVectorField U0("U0", U);
volScalarField psi0 ("psi0", psi);
volScalarField alpha0 ("alpha0", alpha);

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
