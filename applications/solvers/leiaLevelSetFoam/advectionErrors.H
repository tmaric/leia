scalar calcEvolAlpha 
(
    const volScalarField& alpha,
    const volScalarField& alpha0 
)
{
    const fvMesh& mesh = alpha0.mesh(); 
    const auto& V = mesh.V();

    auto V0 = gSum((alpha0 * V)());
    return Foam::mag(V0 - gSum((alpha * V)())) / V0;
}

scalar calcEgeomAlpha 
(
    const volScalarField& alpha,
    const volScalarField& alpha0 
)
{
    const fvMesh& mesh = alpha0.mesh(); 
    const auto& V = mesh.V();
    return gSum((V * Foam::mag(alpha - alpha0))());
}

scalar calcEboundAlpha(const volScalarField& alpha)
{
    return Foam::max
    (
        // Max overshoot
        // Max positive value over c for  
        // alpha_c - 1 > 0, otherwise 0.
        Foam::max(Foam::max(alpha - 1).value(), 0),  
        // Max undershoot
        // Max positive value over c for  
        // -alpha_c > 0, otherwise 0.
        Foam::max(Foam::max(alpha - 1).value(), 0)  
    );
}
