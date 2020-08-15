// RhieChow flux correction for pressure gradient
if(RhieChow)
{
    // // delta direction approach with no correction for non-orthogonal cells

    // // this Rhie chow correction uses orthogonal gradient for snGrad and
    // // aligned gradient for face-interpolated-gradient
    // tmp<surfaceVectorField> gradpDiff_f
    // (
    //     (  (fvc::interpolate(fvc::grad(p_rgh)) & ed)
    //       - faceGradient.snGrad(p_rgh)
    //     ) * ed
    // );
    // phi += rAUf* (gradpDiff_f() & mesh.Sf());
    // gradpDiff_f.clear();

	// Regardless of magnitude of the face non-orthogonality,
	// gradients are estimated in the surface normal direction
    tmp<surfaceScalarField> gradpDiffFlux
    (
          (fvc::snGrad(p_rgh) * mesh.magSf())
        - (fvc::interpolate(fvc::grad(p_rgh)) & mesh.Sf())
    );

    // following the sign convection (snGrand - interpolatedGrad)
    phi -= rAUf * gradpDiffFlux();

    if(printInfo)
    {
        surfaceScalarField pressureCont = - (rAUf * gradpDiffFlux())/mesh.magSf();
        Info<< "\tPressure contrinution max: " << gMax(pressureCont)
            << "\tmin: " << gMin(pressureCont) << endl;
    }

    gradpDiffFlux.clear();
}

// RhieChow flux correction for time-steps
if(ddtCorr)
{
    phi += rAUf*fvc::ddtCorr(U, phi);

    if(printInfo)
    {
        surfaceScalarField timeStepCont =  (rAUf * fvc::ddtCorr(U, phi))/mesh.magSf();
        Info<< "\ttimeStep contrinution max: " << gMax(timeStepCont)
            << "\tmin: " << gMin(timeStepCont) << endl;
    }
}

// // Buoyancy force RhieChow correction
// if(buoyancyCorr)
// {
//     // Reconstructed and later interpolated buoyancy flux
//     tmp<surfaceScalarField> buoyancyTerm_RI_Flux
//     (
//         fvc::dotInterpolate(
//             mesh.Sf(),
//             fvc::reconstruct(Boussinesq.buoyancyTerm() * mesh.magSf() )
//         )
//     );

//     phi += rAUf * (Boussinesq.buoyancyTerm()* mesh.magSf() - buoyancyTerm_RI_Flux());

//     if(printInfo)
//     {
//         surfaceScalarField buoyancyCont =
//         (rAUf * (Boussinesq.buoyancyTerm() - buoyancyTerm_RI_Flux()/mesh.magSf()));
//         Info<< "\tbuoyancy contrinution max: " << gMax(buoyancyCont)
//             << "\tmin: " << gMin(buoyancyCont) << endl;
//     }

//     buoyancyTerm_RI_Flux.clear();
// }
