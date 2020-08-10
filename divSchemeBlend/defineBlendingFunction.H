// --A blending function to transition from bf1 at z1 height to bf2 at z2 height 
//   a cosine transition function used for smoothness
//   applied in the computeDivSchemeBlendingField header file
 
inline scalar getBlendingCoeff(const scalar& z, const scalar& z1, const scalar& z2,  const scalar& bf1, const scalar& bf2)
{
    // UBlendingFactor[faceI] = blendingFactorUz2 +
    //                          0.5 * (blendingFactorUz1 - blendingFactorUz2) *
    //                         (1.0 + Foam::cos(((z - zBlending1)/(zBlending2 - zBlending1))*Foam::constant::mathematical::pi));
    return ( bf2 + 0.5 * (bf1 - bf2) * (1.0 + Foam::cos(((z - z1)/(z2 - z1))*Foam::constant::mathematical::pi)));
}