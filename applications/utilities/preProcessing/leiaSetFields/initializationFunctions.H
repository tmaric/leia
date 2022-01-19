#include "processorFvsPatchField.H"

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

    const auto& CfBoundaryField = Cf.boundaryField(); 
    const auto& SfBoundaryField = Sf.boundaryField(); 
    auto& phiBoundaryField = phi.boundaryFieldRef();  
    const auto& meshBoundary = mesh.boundary(); 

    forAll(meshBoundary, patchI)
    {
        const fvPatch& patch = meshBoundary[patchI]; 
        if (isA<processorFvPatch>(patch))
        {
            const auto& CfPatchField = CfBoundaryField[patchI];
            const auto& SfPatchField = SfBoundaryField[patchI];
            auto& phiPatchField = phiBoundaryField[patchI]; 
            forAll(phiPatchField, faceI)
            {
                phiPatchField[faceI] = 
                    velocityShear2D(CfPatchField[faceI]) & SfPatchField[faceI];
            }
        }
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
