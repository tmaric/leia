{
    // Temporarily making U relative to mesh motion
    if (mesh.moving())
    {
        U -= fvc::reconstruct(mesh.phi());
    }

    // Updating alpha1 using the Level Set Method 
    fvScalarMatrix psiEqn
    (
        fvm::ddt(psi)
      + fvm::div(phi, psi)
    );

    psiEqn.solve();

    redist->redistance(psi);
    
    phaseInd->calcPhaseIndicator(alpha1, psi);

    narrowBand->calc();

    // Making U absolute again after advection step
    if (mesh.moving())
    {
        U += fvc::reconstruct(mesh.phi());
    }

    alpha2 = 1.0 - alpha1;
    mixture.correct();
}

scalar domainFraction = 0;
domainFraction = alpha1.weightedAverage(mesh.Vsc()).value();

Info<< "Phase-1 volume fraction = "
    << domainFraction
    << "  Min(" << alpha1.name() << ") = " << min(alpha1).value()
    << "  Max(" << alpha1.name() << ") = " << max(alpha1).value()
    << endl;
