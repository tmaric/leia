vector velocityShear2D(const vector& x)
{
    return vector(
        Foam::sin(2*M_PI*x[1])*Foam::sqr(Foam::sin(M_PI * x[0])),  
        -Foam::sin(2*M_PI*x[0])*Foam::sqr(Foam::sin(M_PI * x[1])),  
        0
    );
}

void setPsi
(
    volScalarField& psi, 
    const vector& center, 
    const scalar radius
)
{

    const fvMesh& mesh = psi.mesh();
    const auto& C = mesh.C(); 
    forAll(psi, cellI)
    {
        psi[cellI] = mag(C[cellI] - center) - radius;
    }
}


void setVolumetricFlux(surfaceScalarField& phi)
{
    const fvMesh& mesh = phi.mesh();
    const surfaceVectorField& Sf = mesh.Sf();
    const surfaceVectorField& Cf = mesh.Cf();

    forAll (Cf, faceID)
    {
        phi[faceID] = 
            velocityShear2D(Cf[faceID]) & Sf[faceID];
    }
    // TODO: the boundaries for advection cases. 
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
