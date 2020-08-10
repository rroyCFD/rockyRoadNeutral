// Extrapolate fields using a Adam-Bashforth like scheme
{
	scalar extrapCoeff(0.5);

	// account for the variable time-step process
	if(adjustTimeStep)
	{
		extrapCoeff = 0.5 * (runTime.deltaTValue()/runTime.deltaT0Value());
		// bound between 0 and 1; (assuming dt(n+1) is no larger than twice of dt(n))
		extrapCoeff = max(min(extrapCoeff, 1.0), 0.0);
		Info << "Extrapolation coeffcient: " << extrapCoeff << endl;
	}

	// extrapolate fields to next-time step to obtain a better initial condition
	p_rgh = ((1 + extrapCoeff) * p_rgh.oldTime() - extrapCoeff * p_rgh.oldTime().oldTime());

	// // extrapolate fixedFluxPressure gradient
	// *** setSnGrad 2nd argument shall be a surfacePatch field
	// setSnGrad<fixedFluxPressureFvPatchScalarField>
	// (
	//     p_rgh.boundaryFieldRef(),
	//     (
	//         (1 + extrapCoeff) * p_rgh.oldTime().boundaryField()
	//       - extrapCoeff * p_rgh.oldTime().oldTime().boundaryField()
	//     )
	// );
	// p_rgh.correctBoundaryConditions();

	forAll(p_rgh.boundaryField(), patchi)
	{
		if(isType<fixedFluxPressureFvPatchScalarField>(p_rgh.boundaryField()[patchi]))
		{
			fixedGradientFvPatchScalarField& p_rghPatch =
	            refCast<fixedGradientFvPatchScalarField>(p_rgh.boundaryFieldRef()[patchi]);

	        fixedGradientFvPatchScalarField& p_rghPatch0 =
	            refCast<fixedGradientFvPatchScalarField>(p_rgh.oldTime().boundaryFieldRef()[patchi]);

	        fixedGradientFvPatchScalarField& p_rghPatch00 =
	            refCast<fixedGradientFvPatchScalarField>(p_rgh.oldTime().oldTime().boundaryFieldRef()[patchi]);

	        p_rghPatch.gradient() = (
	            (1 + extrapCoeff) * p_rghPatch0.gradient()
	            - extrapCoeff * p_rghPatch00.gradient()
	        );
		}
	}


	// Update phi first, and then U (in case U depends on phi)
	phi = ((1 + extrapCoeff) * phi.oldTime() - extrapCoeff * phi.oldTime().oldTime());

	U = ((1 + extrapCoeff) * U.oldTime() - extrapCoeff * U.oldTime().oldTime());
	// U.correctBoundaryConditions(); updated at UEqn matrix formation
}