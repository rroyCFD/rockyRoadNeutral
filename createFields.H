// create solution control object
spaeceControl spaece(mesh, "SPAeCE");

// Read ABLProperties with input data
IOdictionary ABLProperties
(
    IOobject
    (
        "ABLProperties",
        runTime.time().constant(),
        runTime,
        IOobject::MUST_READ,
        IOobject::NO_WRITE
    )
);

//- Automatically update fixesValue for timeVaryingMappedInletOutlet BC?
bool updateFixesValue(ABLProperties.lookupOrDefault<bool>("updateFixesValue",false));

// Create and read the modified pressure field [m^2/s^2].
Info << "Creating and reading the density-normalized modified pressure field, p_rgh" << endl;
volScalarField p_rgh
(
    IOobject
    (
        "p_rgh",
        runTime.timeName(),
        mesh,
        IOobject::MUST_READ,
        IOobject::AUTO_WRITE
    ),
    mesh
);


// Create and read the velocity field [m/s].
Info << "Creating and reading velocity field, U" << endl;
volVectorField U
(
    IOobject
    (
        "U",
        runTime.timeName(),
        mesh,
        IOobject::MUST_READ,
        IOobject::AUTO_WRITE
    ),
    mesh
);


// Create and calculate the velocity flux [m^3/s].
Info << "Creating and calculating velocity flux field, phi" << endl;
#include "createPhi.H"


// Set up the laminar (molecular) transport model.
Info << "Creating and reading laminar transport properties" << endl;
singlePhaseTransportModel laminarTransport(U, phi);


// Create the turbulence model.
Info << "Creating and reading turbulent transport properties" << endl;
autoPtr<incompressible::turbulenceModel> turbulence
(
    incompressible::turbulenceModel::New(U, phi, laminarTransport)
);


// Create Coriolis force object [m/s^2].
// Info << "Creating Coriolis force" << endl;
// CoriolisForce Coriolis(U);


// Create external source term objects [m/s^2 and K/s].
Info << "Creating and reading volumetric source terms for momentum and temperature" << endl;
drivingForce<vector> momentumSourceTerm(ABLProperties, "momentum",U);


// Create and read the wall shear stress field [m^2/s^2].
Info << "Creating and reading the wall shear stress field, Rwall" << endl;
volSymmTensorField Rwall
(
    IOobject
    (
        "Rwall",
        runTime.timeName(),
        mesh,
        IOobject::MUST_READ,
        IOobject::AUTO_WRITE
    ),
    mesh
);

// Create and read the density-normalied pressure field [m^2/s^2].
Info << "Creating and reading the density-normalized pressure field, p" << endl;
volScalarField p
(
    IOobject
    (
        "p",
        runTime.timeName(),
        mesh,
        IOobject::NO_READ,
        IOobject::AUTO_WRITE
    ),
    p_rgh
);


// Deal with reference pressure level.
Info << "Setting reference pressure level" << endl;
label pRefCell = 0;
scalar pRefValue = 0.0;

// pRefOn is true if need reference (i.e. value is not fixed at any boundary)
bool pRefOn = setRefCell (p, p_rgh, spaece.dict(), pRefCell, pRefValue );

// Create a reference height, which is the height of the pressure reference cell.
vector hRef_ = vector::zero;
if (pRefCell != -1)
{
    hRef_ = mesh.C()[pRefCell];
}
reduce(hRef_,sumOp<vector>());
dimensionedVector hRef("hRef",dimLength,hRef_);

// Initialize the gravity potential field
#include "readGravitationalAcceleration.H"
volScalarField gh("gh", g & (mesh.C() - hRef));
// surfaceScalarField ghf("ghf", g & (mesh.Cf() - hRef));


// Adjust the background pressure level to agree with the reference pressure specified
// at a certain location.
Info << "Adjusting the background pressure level to agree with reference level" << endl;
#include "adjustPressureLevel.H"

// The quantity p_rgh is required on cell faces (flux) to project the velocity flux given the
// updated pressure field, so add it to the list kept internally in OpenFOAM.
mesh.setFluxRequired(p_rgh.name());


// Allows the user to set fvOptions, such as additional source terms.
#include "createFvOptions.H"
