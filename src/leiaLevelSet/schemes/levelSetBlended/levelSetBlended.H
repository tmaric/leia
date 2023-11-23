/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2022 Tomislav Maric, TU Darmstadt 
-------------------------------------------------------------------------------
License
    This file is part of OpenFOAM.

    OpenFOAM is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM.  If not, see <http://www.gnu.org/licenses/>.

Class
    Foam::levelSetBlended

Group
    grpFvSurfaceInterpolationSchemes

Description
    Two-scheme blending differencing scheme. 

    Uses a blending factor computed from the face-based narrow-band indicator
    of a zero level set of the transported field. For the Level Set method, 
    use upwind outside of the narrow band of the zero level set, because the 
    order of accuracy away from the zero level set is irrelevant. Use linear  
    within the narrow band to obtain second-order accuracy and limit the 
    non-monotonic behavior of the linear scheme to the narrow band.

    The weight applies to the first scheme and 1-weight to the second scheme.

Usage

    \verbatim
    divSchemes
    {
        .
        .
        // First scheme is used inside the narrow band, second scheme outside.
        div(phi,U)  Gauss levelSetBlended linear upwind;
        .
        .
    }
    \endverbatim

SourceFiles
    levelSetBlended.C

\*---------------------------------------------------------------------------*/

#ifndef levelSetBlended_H
#define levelSetBlended_H

#include "surfaceFieldsFwd.H"
#include "surfaceInterpolationScheme.H"
#include "blendedSchemeBase.H"
#include "surfaceInterpolate.H"
#include "fvcAverage.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                        Class levelSetBlended Declaration
\*---------------------------------------------------------------------------*/

template<class Type>
class levelSetBlended
:
    public surfaceInterpolationScheme<Type>,
    public blendedSchemeBase<Type>
{
    // Private data

        //- Scheme 1
        tmp<surfaceInterpolationScheme<Type>> tScheme1_;

        //- Scheme 2
        tmp<surfaceInterpolationScheme<Type>> tScheme2_;

    // Private Member Functions

        //- No copy construct
        levelSetBlended(const levelSetBlended&) = delete;

        //- No copy assignment
        void operator=(const levelSetBlended&) = delete;


public:

    //- Runtime type information
    TypeName("levelSetBlended");

    // Constructors

        //- Construct from mesh and Istream.
        //  The name of the flux field is read from the Istream and looked-up
        //  from the mesh objectRegistry
        levelSetBlended
        (
            const fvMesh& mesh,
            Istream& is
        )
        :
            surfaceInterpolationScheme<Type>(mesh),
            tScheme1_
            (
                surfaceInterpolationScheme<Type>::New(mesh, is)
            ),
            tScheme2_
            (
                surfaceInterpolationScheme<Type>::New(mesh, is)
            )
        {}


        //- Construct from mesh, faceFlux and Istream
        levelSetBlended
        (
            const fvMesh& mesh,
            const surfaceScalarField& faceFlux,
            Istream& is
        )
        :
            surfaceInterpolationScheme<Type>(mesh),
            tScheme1_
            (
                surfaceInterpolationScheme<Type>::New(mesh, faceFlux, is)
            ),
            tScheme2_
            (
                surfaceInterpolationScheme<Type>::New(mesh, faceFlux, is)
            )
        {}


    // Member Functions

        //- Return the face-based blending factor
        virtual tmp<surfaceScalarField> blendingFactor
        (
             const GeometricField<Type, fvPatchField, volMesh>& vf
        ) const
        {
            const fvMesh& mesh = this->mesh();

            const surfaceScalarField& phi = 
                mesh.lookupObject<surfaceScalarField>("phi");

            tmp<surfaceScalarField> bfTmp
            (
                new surfaceScalarField (vf.name() + "BlendingFactor", phi)
            );
            surfaceScalarField& bf = bfTmp.ref();
            dimensionSet& bfDimensions = bf.dimensions();
            bfDimensions.reset(dimless);
            bf == dimensionedScalar(bf.name(), dimless, 1);
            bf.setOriented(false);

            // Compute the cell-centered blending factor bfCell as a
            // narrow-band indicator for the zero level set of vf.

            // - Internal field calculation 
            volScalarField bfCell // cell-centered blending factor.
            (
                IOobject
                (
                    "bfCell",
                    mesh.time().timeName(),
                    mesh,
                    IOobject::NO_READ,
                    IOobject::NO_WRITE
                ),
                mesh,
                dimensionedScalar("bfCell", bf.dimensions(), 0.)
            );

            const auto& own = mesh.owner();  
            const auto& nei = mesh.neighbour(); 
            forAll(own, faceI)
            {
                if (vf[own[faceI]] * vf[nei[faceI]] <= 0)
                {
                    bfCell[own[faceI]] = 1;
                    bfCell[nei[faceI]] = 1;
                }
            }

            // - MPI process-boundary calculation 
            const auto& vfBdryField = vf.boundaryField(); // needed for Nc LLSQ contribs
            const auto& patches = mesh.boundary(); // needed for Nc LLSQ contribs
            const auto& faceOwner = mesh.faceOwner();
            forAll(vfBdryField, patchI)
            {
                const fvPatch& patch = patches[patchI];
                if (isA<coupledFvPatch>(patch)) // coupled patch 
                {
                    const auto& vfPatchField = vfBdryField[patchI]; 
                    auto vfPatchNeiFieldTmp = vfPatchField.patchNeighbourField();
                    const auto& vfPatchNeiField = vfPatchNeiFieldTmp();
                    forAll(vfPatchNeiField, faceI)
                    {
                        label faceJ = faceI + patch.start(); // Global face label.
                        if (vf[faceOwner[faceJ]] * vfPatchNeiField[faceI] <= 0)
                        {
                            bfCell[faceOwner[faceJ]] = 1;
                        }
                    }
                }
            }

            // Extend the narrow band marker algebraically. 
            volScalarField bfCellSmooth ("bfCellSmooth", bfCell); 

            for(char I = 0; I < 1; ++I) 
            {
                // Smooth the narrow-band marker algebraically.
                bfCellSmooth = fvc::average(bfCellSmooth);

                // Extend the sharp narrow-band marker  
                forAll(bfCell, cellI) 
                    if (bfCellSmooth[cellI] > 0) bfCell[cellI] = 1;
            }

            // Smooth the cell-centered blending factor 
            bfCellSmooth = bfCell;
            for(char I = 0; I < 3; ++I) 
            {
                bfCellSmooth = fvc::average(bfCellSmooth);
            }

            // Reset the smooth blending factor in the narrow band
            forAll(bfCell, cellI) 
                if (bfCell[cellI] == 1) bfCellSmooth[cellI] = 1;


            // TODO: Uncomment for debugging output.
            if (mesh.time().writeTime())
            {
                bfCellSmooth.write();
                bfCell.write();
            }

            // Interpolate the blending factor bfCellSmooth to faces to get the
            // blending factor for the blended scheme.
            bf == fvc::interpolate(bfCellSmooth);

            return bfTmp;
        }

        //- Return the interpolation weighting factors
        tmp<surfaceScalarField>
        weights
        (
            const GeometricField<Type, fvPatchField, volMesh>& vf
        ) const
        {
            surfaceScalarField bf(blendingFactor(vf));

            surfaceScalarField tScheme1weights = tScheme1_.ref().weights(vf);
            tScheme1weights.setOriented(false);
            surfaceScalarField tScheme2weights = tScheme2_.ref().weights(vf);
            tScheme2weights.setOriented(false);
            
            return bf*tScheme1weights + (scalar(1) - bf)*tScheme2weights;
        }


        //- Return the face-interpolate of the given cell field
        //  with explicit correction
        tmp<GeometricField<Type, fvsPatchField, surfaceMesh>>
        interpolate
        (
            const GeometricField<Type, fvPatchField, volMesh>& vf
        ) const
        {
            surfaceScalarField bf(blendingFactor(vf));

            return
                bf*tScheme1_().interpolate(vf)
              + (scalar(1) - bf)*tScheme2_().interpolate(vf);
        }


        //- Return true if this scheme uses an explicit correction
        virtual bool corrected() const
        {
            return tScheme1_().corrected() || tScheme2_().corrected();
        }


        //- Return the explicit correction to the face-interpolate
        //  for the given field
        virtual tmp<GeometricField<Type, fvsPatchField, surfaceMesh>>
        correction
        (
            const GeometricField<Type, fvPatchField, volMesh>& vf
        ) const
        {
            surfaceScalarField bf(blendingFactor(vf));

            if (tScheme1_().corrected())
            {
                if (tScheme2_().corrected())
                {
                    return
                    (
                        bf
                      * tScheme1_().correction(vf)
                      + (scalar(1) - bf)
                      * tScheme2_().correction(vf)
                    );
                }
                else
                {
                    return
                    (
                        bf
                      * tScheme1_().correction(vf)
                    );
                }
            }
            else if (tScheme2_().corrected())
            {
                return
                (
                    (scalar(1) - bf)
                  * tScheme2_().correction(vf)
                );
            }
            else
            {
                return tmp<GeometricField<Type, fvsPatchField, surfaceMesh>>
                (
                    nullptr
                );
            }
        }
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
