// Compute the full pressure and adjust the pressure level.
p = p_rgh + gh;

if (pRefOn)
{
	// change in Buoyant background pressure may alter pRefValue at pRefCell
	// since the linear system enforces pRefCell value within the solver tolerance
	// pp[pRefCell] not-equal-to 0.0
    scalar currRefVal = getRefCellValue(p, pRefCell);
    Info << "p[pRefCell] (before adjustment): " << currRefVal << endl;

    dimensionedScalar pAdjust ("pAdjust", p.dimensions(), (pRefValue - currRefVal) );
    p     += pAdjust;
    p_rgh += pAdjust;

    Info << "p[pRefCell] (after  adjustment): " << getRefCellValue(p, pRefCell) << endl;
}

