// Update BCs before time-marching in case anything needs updating,
// e.g. after change of BC or using mapFields to interpolate initial field.
Info << "Updating boundary conditions" << endl;
U.correctBoundaryConditions();

// Update phi from U, when phi is missing (generally the case when mapped)
IOobject phiHeader
(
    "phi",
    U.instance(), // runTime.timeName(),
    mesh,
    IOobject::NO_READ
);

if(phiHeader.typeHeaderOk<surfaceScalarField>(true))
{
    Info << "Updaing phi from U" << endl;
    phi = fvc::flux(U);
}

#include "turbulenceCorrect.H"