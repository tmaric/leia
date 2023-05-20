fvScalarMatrix YoungLaplaceEqn
(
    fvm::laplacian(p_rgh) == fvc::div(fSigma->faceSurfaceTensionForce()*mesh.magSf())
);
YoungLaplaceEqn.solve();

