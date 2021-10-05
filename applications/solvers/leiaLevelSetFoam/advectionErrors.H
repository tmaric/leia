template<typename Ostream>
void reportErrors
(
    Ostream& os,
    const volScalarField& psi,
    const volScalarField& psi0,
    const volScalarField& alpha, 
    const volScalarField& alpha0
)
{
    // Compute the level set advection error. 
    volScalarField ePhi = Foam::mag(psi - psi0);

    // Zero the level set advection error in the bulk.
    forAll(alpha, cellI)
    {
        // only if cellI is a bulk cell 
        if ((alpha[cellI] == 0) || 
            (alpha[cellI] == 1)) 
            
        {
            // and the sign of the signed distance didn't switch,
            // after the advection reversal, so cellI is a bulk
            // cell at the final simulation time. 
            if (Foam::sign(psi[cellI] == Foam::sign(psi0[cellI]))) 
                ePhi[cellI] = 0; // the error is not interesting.
            // This avoids the problem of maximal errors at domain
            // boundaries, and at the points equidistant from the 
            // fluid interface, e.g. the center of the sphere. TM.
        }
    }

    scalar lInfEpsi = Foam::max(ePhi).value();

    const fvMesh& mesh = alpha0.mesh(); 
    const auto& runTime = mesh.time();
    const auto& V = mesh.V();

    scalar alphaGeomErr = 
        gSum((V * Foam::mag(alpha - alpha0))());
    
    scalar alphaBoundErr = Foam::max
    (
        // Max overshoot
        // Max positive value over c for  
        // alpha_c - 1 > 0, otherwise 0.
        Foam::max(Foam::gMax((alpha - 1)()), 0),  
        // Max undershoot
        // Max positive value over c for  
        // -alpha_c > 0, otherwise 0.
        Foam::max(Foam::gMax((alpha - 1)()), 0)  
    );

    scalar alphaVolErr = Foam::mag
    (
        gSum((alpha * V)()) - 
        gSum((alpha0 * V)())
    );

    // Unstructured FVM length approximation
    scalar deltaX = Foam::max(Foam::pow(mesh.deltaCoeffs(),-1)).value();

    os << runTime.timeOutputValue() << ","
        << deltaX << "," 
        << lInfEpsi << "," 
        << alphaVolErr << ","
        << alphaGeomErr << ","
        << alphaBoundErr << "\n"; 
}
