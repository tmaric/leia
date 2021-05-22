void calcPhaseIndicator 
( 
    volScalarField& alpha, 
    const volScalarField& phi,
    const dimensionedScalar& epsilon
)
{
    forAll(alpha, cellID)
    {
        if (phi[cellID] < -epsilon.value())
            alpha[cellID] = 0;
        else if (phi[cellID] > epsilon.value())
            alpha[cellID] = 1;
        else
        {
            alpha[cellID] = 0.5 + 
                 + phi[cellID] / (2*epsilon.value())
                 + Foam::sin((M_PI * phi[cellID]) / epsilon.value()) / (2*M_PI);
        }
    }
    alpha = 1 - alpha;
}
