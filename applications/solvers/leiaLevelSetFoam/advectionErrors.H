class volumeError
{
    private: 
        scalar volume0_; 
        scalar eVolAlpha_; 

    public: 

        volumeError
        (
            const volScalarField& alpha,
            const volScalarField& alpha0 
        )
        : 
            volume0_(0), 
            eVolAlpha_(0)
        {
            calcEvolAlpha(alpha, alpha0);
        }

        void calcEvolAlpha 
        (
            const volScalarField& alpha,
            const volScalarField& alpha0 
        )
        {
            const fvMesh& mesh = alpha0.mesh(); 
            const auto& V = mesh.V();

            volume0_ = gSum((alpha0 * V)());
            eVolAlpha_ = Foam::mag(volume0_ - gSum((alpha * V)())) 
                / volume0_;
        }

        scalar volume0() const
        {
            return volume0_; 
        }

        scalar eVolAlpha() const
        {
            return eVolAlpha_;
        }
};


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
