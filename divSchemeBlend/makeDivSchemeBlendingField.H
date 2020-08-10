{
    // Update the old value of these quantities for checking for updated files.
    profileTableOld = profileTable;

    zBlending1Old = zBlending1;
    zBlending2Old = zBlending2;

    blendingFactorUz1Old = blendingFactorUz1;
    blendingFactorTz1Old = blendingFactorTz1;
    blendingFactorUz2Old = blendingFactorUz2;
    blendingFactorTz2Old = blendingFactorTz2;

    useWallDistZOld = useWallDistZ;
    //------------------------------------------------------------------------//

    // Get distance from the wall
    vector up = vector::zero;
    up.z() = 1.0;
    surfaceScalarField dFace = mesh.Cf() & up;
    if (useWallDistZ)
    {
        Info << "Calculating wall distance" << endl;
        wallDist d(mesh);
        dFace = fvc::interpolate(d.y());
    }

    // Set the blending factor fields.
    // internal field
    forAll(UBlendingFactor, faceI)
    {
        scalar area = mesh.magSf()[faceI];

        UBlendingFactor[faceI] = max(min(interpolateXY(area,faceAreaList,UBlendingList),1.0),0.0);
        TBlendingFactor[faceI] = max(min(interpolateXY(area,faceAreaList,TBlendingList),1.0),0.0);
    
    
        scalar z = 0.0;
        if (useWallDistZ)
        {
            z = dFace[faceI];
        }
        else
        {    
            z = mesh.Cf()[faceI].z();
        }

        if ((z > zBlending1) && (z < zBlending2))
        {
            // UBlendingFactor[faceI] = blendingFactorUz2 +
            //                          0.5 * (blendingFactorUz1 - blendingFactorUz2) *
            //                         (1.0 + Foam::cos(((z - zBlending1)/(zBlending2 - zBlending1))*Foam::constant::mathematical::pi));
            // TBlendingFactor[faceI] = blendingFactorTz2 +
            //                          0.5 * (blendingFactorTz1 - blendingFactorTz2) *
            //                         (1.0 + Foam::cos(((z - zBlending1)/(zBlending2 - zBlending1))*Foam::constant::mathematical::pi));
            
            UBlendingFactor[faceI] = getBlendingCoeff(z, zBlending1, zBlending2,  blendingFactorUz1, blendingFactorUz2);
            TBlendingFactor[faceI] = getBlendingCoeff(z, zBlending1, zBlending2,  blendingFactorTz1, blendingFactorTz2);
        }
        else if ( z > zBlending2 )
        {
            UBlendingFactor[faceI] = blendingFactorUz2;
            TBlendingFactor[faceI] = blendingFactorTz2;
        }

    }

    // boundary field
    forAll(UBlendingFactor.boundaryFieldRef(), patchI)
    {
        fvsPatchField<scalar>& UBFpatch = UBlendingFactor.boundaryFieldRef()[patchI];
        fvsPatchField<scalar>& TBFpatch = TBlendingFactor.boundaryFieldRef()[patchI];

        forAll(UBFpatch, faceI)
        {
            scalar area = mesh.boundary()[patchI].magSf()[faceI];

            UBFpatch[faceI] = max(min(interpolateXY(area,faceAreaList,UBlendingList),1.0),0.0);
            TBFpatch[faceI] = max(min(interpolateXY(area,faceAreaList,TBlendingList),1.0),0.0);


            scalar z = 0.0;
            if (useWallDistZ)
            {
                z = dFace.boundaryField()[patchI][faceI];
            }
            else
            {
                z = mesh.boundary()[patchI].Cf()[faceI].z();
            }


            if ((z > zBlending1) && (z < zBlending2))
            {
                UBFpatch[faceI] = getBlendingCoeff(z, zBlending1, zBlending2,  blendingFactorUz1, blendingFactorUz2);
                TBFpatch[faceI] = getBlendingCoeff(z, zBlending1, zBlending2,  blendingFactorTz1, blendingFactorTz2);
            }
            else if ( z > zBlending2 )
            {
                UBFpatch[faceI] = blendingFactorUz2;
                TBFpatch[faceI] = blendingFactorTz2;
            }
        }
    }
}