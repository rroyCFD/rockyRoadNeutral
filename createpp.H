//----------------------------------------------------------------------------//
Info << "\nCreating pressure-coorection field pp" << endl;
wordList ppbt = p_rgh.boundaryField().types();

// prepare the boundary condition types for pressure-correction field
// if pressure BC is a fixed value type, pp is fixedValue, uniform zero
// if preseser BC is a fixed Gradient type, pp is the same gradient

// check for fixedFluxPressure BC
bool fixedFluxBC (false);

forAll(ppbt, b)
{
    if(ppbt[b] == "timeVaryingMappedFixedValue")
    {
        ppbt[b] = "fixedValue";
    }

    // pp gradient is a copy of p_rgh gradient derived from fixedFluxPressure BC
    else if(ppbt[b] == "fixedFluxPressure")
    {
        ppbt[b] = "fixedGradient";
        fixedFluxBC = true;
    }
}

// need to create a corresponding mixed BC for pp;
// p fixedVale -> pp 0 | p fixedGrad -> pp fixedGrad

// if (isType<fixedFluxFvPatchScalarField>(p_rgh.boundaryField()[patchI]))
//     {
//         // gradient will follow the p_rgh gradient
//         ppbt[patchI] = "fixedGradient";
//     }
// }

volScalarField pp
(
    IOobject
    (
        "pp",
        runTime.timeName(),
        mesh,
        IOobject::READ_IF_PRESENT, // NO_READ,
        IOobject::AUTO_WRITE // NO_WRITE
    ),
    mesh,
    dimensionedScalar("zero", p_rgh.dimensions(), 0.0),
    ppbt
);

Info << "Pressure correction field boundary types:"
     << pp.boundaryField().types() << endl;


// Set pp boundary values
// boundaryFieldRef() return a (non-const) reference to the boundary field
// refCast converts the generic boundary field to a prescribed type

// set fixed value BC to zero
forAll(pp.boundaryField(), patchi)
{
    if (isType<fixedValueFvPatchScalarField>(pp.boundaryField()[patchi]))
    {
        fixedValueFvPatchScalarField& ppbound =
           refCast<fixedValueFvPatchScalarField>(pp.boundaryFieldRef()[patchi]);

        ppbound == scalarField(ppbound.size(),0.0);
    }
}

// p_rgh fixedGrad -> pp same grad
// p_rgh fixedFlux -> pp set zero grad for initialization, update grad at ppEqn
forAll(p_rgh.boundaryField(), patchi)
{
    if(isType<fixedGradientFvPatchScalarField>(p_rgh.boundaryField()[patchi]))
    {
        fixedGradientFvPatchScalarField& ppGradPatch =
            refCast<fixedGradientFvPatchScalarField>
                (pp.boundaryFieldRef()[patchi]);
        
        fixedGradientFvPatchScalarField& p_rghGradPatch =
            refCast<fixedGradientFvPatchScalarField>
                (p_rgh.boundaryFieldRef()[patchi]);

        ppGradPatch.gradient() = p_rghGradPatch.gradient();
    }
    else if(
      isType<fixedFluxPressureFvPatchScalarField>(p_rgh.boundaryField()[patchi])
    )
    {
        fixedGradientFvPatchScalarField& ppGradPatch =
            refCast<fixedGradientFvPatchScalarField>
                (pp.boundaryFieldRef()[patchi]);

        ppGradPatch.gradient() = 0.0;
    }
}