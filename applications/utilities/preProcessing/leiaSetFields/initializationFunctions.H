vector velocityShear2D(const vector& x)
{
    return vector(
        Foam::sin(2*M_PI*x[1])*Foam::sqr(Foam::sin(M_PI * x[0])),  
        -Foam::sin(2*M_PI*x[0])*Foam::sqr(Foam::sin(M_PI * x[1])),  
        0
    );
}

void setPhi
(
    volScalarField& phi, 
    const vector& center, 
    const scalar radius
)
{

    const fvMesh& mesh = phi.mesh();
    const auto& C = mesh.C(); 
    forAll(phi, cellI)
    {
        phi[cellI] = mag(C[cellI] - center) - radius;
    }
}


void setVolumetricFlux(surfaceScalarField& F)
{
    const fvMesh& mesh = F.mesh();
    const surfaceVectorField& Sf = mesh.Sf();
    const surfaceVectorField& Cf = mesh.Cf();

    forAll (Cf, faceID)
    {
        F[faceID] = 
            velocityShear2D(Cf[faceID]) & Sf[faceID];
    }
}

void setVelocity(volVectorField& U)
{
    const fvMesh& mesh = U.mesh();
    const volVectorField& C = mesh.C();

    forAll (C, cellID)
    {
        U[cellID] = velocityShear2D(C[cellID]); 
    }
}
