template<typename Ostream>
void reportErrors
(
    Ostream& os,
    const volScalarField& psi,
    const volScalarField& psi0,
    const volScalarField& alpha, 
    const volScalarField& alpha0,
    scalar maxCoNum
)
{
    // Compute the level set advection error. 
    volScalarField ePhi = Foam::mag(psi - psi0);

    // TODO: Improve the error calculation - use the narrow band. 
    // Zero the level set advection error in the bulk.
    forAll(alpha, cellI)
    {
        // If cellI is a bulk cell 
        if ((alpha[cellI] == 0) || 
            (alpha[cellI] == 1)) 
        {
            // and the sign of the signed distance didn't switch,
            // after the advection reversal, so cellI is a bulk
            // cell at the final simulation time. 
            if (Foam::sign(psi[cellI] == Foam::sign(psi0[cellI]))) 
                ePhi[cellI] = 0; 
            // This avoids the problem of maximal errors reported
            // away from the interface.
        }
    }

    scalar lInfEpsi = Foam::gMax(ePhi);

    const fvMesh& mesh = alpha0.mesh(); 
    const auto& runTime = mesh.time();
    const auto& V = mesh.V();

    scalar alphaV0 = gSum((alpha0 * V)());

    scalar alphaVolErr = Foam::mag
    (
        gSum((alpha * V)()) - alphaV0
    );
    
    scalar alphaVolErrRel = alphaVolErr / alphaV0; 

    scalar alphaGeomErr = 
        gSum((V * Foam::mag(alpha - alpha0))());

    scalar alphaGeomErrRel = alphaGeomErr / alphaV0;
    
    scalar alphaBoundErr = Foam::max
    (
        // Max overshoot
        // Max positive value over c for  
        // alpha_c - 1 > 0, otherwise 0.
        Foam::max(Foam::gMax((alpha - 1)()), 0.),  
        // Max undershoot
        // Max positive value over c for  
        // -alpha_c > 0, otherwise 0.
        Foam::max(Foam::gMax((alpha - 1)()), 0.)  
    );


    // Unstructured FVM length approximation
    scalar deltaX = Foam::gMin(
        Foam::pow(
            mesh.deltaCoeffs().internalField(),
            -1
        )()
    );

    // The main MPI process writes into the error file.
    if (Pstream::myProcNo() == 0)
    {
        os << runTime.timeOutputValue() << ","
            << deltaX << "," 
            << lInfEpsi << "," 
            << alphaVolErr << ","
            << alphaVolErrRel << ","
            << alphaGeomErr << ","
            << alphaGeomErrRel << ","
            << alphaBoundErr << "," 
            << runTime.elapsedCpuTime() << "," 
            << maxCoNum << "\n";
    }
}
