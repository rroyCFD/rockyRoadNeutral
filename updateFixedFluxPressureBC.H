// Update the fixedFluxPressure BCs to ensure flux consistency
if(fixedFluxBC)
{
    setSnGrad<fixedFluxPressureFvPatchScalarField>
    (
        p_rgh.boundaryFieldRef(),
        (
            phi.boundaryField()
          - phiFixedFlux.boundaryField()
        )/(mesh.magSf().boundaryField()*rAUf.boundaryField())
    );
    p_rgh.correctBoundaryConditions();

    // Apply the same gradient to pp following the coresponding p_rgh BCs
    forAll(p_rgh.boundaryField(), patchi)
    {
        if(isType<fixedFluxPressureFvPatchScalarField> (p_rgh.boundaryField()[patchi]))
        {
            fixedFluxPressureFvPatchScalarField& p_rghGradPatch = refCast<fixedFluxPressureFvPatchScalarField>(p_rgh.boundaryFieldRef()[patchi]);
            fixedGradientFvPatchScalarField& ppGradPatch        = refCast<fixedGradientFvPatchScalarField>(pp.boundaryFieldRef()[patchi]);

            ppGradPatch.gradient() = p_rghGradPatch.gradient();
            Info << "maxGrad: " << gMax(p_rghGradPatch.gradient()) << endl;
        }
    }   
}