/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     | Website:  https://openfoam.org
    \\  /    A nd           | Copyright (C) 2011-2018 OpenFOAM Foundation
     \\/     M anipulation  |
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

Application
    superDeliciousVanilla (the code name for the new unified solver)

Description
    Transient solver for wind-energy and atmospheric boundary layer flows.
    The incompressible equations are solved along with a Boussinesq buoyancy
    term. There is flexibility in turbulence modeling. Although the solver
    is primarily meant for and tested for large-eddy simulations, it could
    also be used with a RANS turbulence model.

    The code includes planetary Coriolis forces, pressure gradient forces
    driving the wind, ability to use mesoscale forcings, coupling with
    atmospheric-style lower rough-wall boundary conditions, and coupling
    with actuator turbine models.

\*---------------------------------------------------------------------------*/

#include "fvCFD.H"
#include "singlePhaseTransportModel.H"
#include "turbulentTransportModel.H"
#include "wallDist.H"
#include "fixedFluxPressureFvPatchScalarField.H"
#include "timeVaryingMappedInletOutletFvPatchField.H"
#include "interpolateXY.H"
#include "fvOptions.H"
#include "spaeceControl.H"
#include "ABL.H"
#include "defineBlendingFunction.H" // for divergence scheme blending

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

int main(int argc, char *argv[])
{
    #include "postProcess.H"

    #include "setRootCaseLists.H"
    #include "createTime.H"
    #include "createMesh.H"
    #include "createFields.H"
    #include "createTimeControls.H"
    #include "CourantNo.H"
    #include "setInitialDeltaT.H"
    #include "computeDivergence.H"
    #include "createDivSchemeBlendingField.H"

    // -- Validate the turbulence fields after construction.
    //    Update derived fields as required
    turbulence->validate();

    #include "updateBoundaryConditions.H"

    // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //
    Info << nl << "Starting time loop" << endl;
    // Time stepping loop.
    while (runTime.loop())
    {
        #include "readTimeControls.H"
        #include "CourantNo.H"
        #include "setDeltaT.H"
        #include "updateDivSchemeBlendingField.H"

        Info << "Time = " << runTime.timeName() << tab
             << "Time Step = " << runTime.timeIndex() << endl;

        {
            // Extrapolate fields for explicit terms
            #include "extrapolateFields.H"

            while (spaece.correct())
            {
                // Update the source terms.
                momentumSourceTerm.update(true);

                // momentum prediction
                #include "UEqn.H"

                // pressure correction
                #include "pEqn.H"

                // solve for turbulent transport variables and update fields
                #include "turbulenceCorrect.H"
            }

            // Compute the continuity errors.
            #include "computeDivergence.H"
        }

        // Update timeVaryingMappedInletOutlet parameters
        #include "updateFixesValue.H"

        // Write the solution at write time.
        runTime.write();

        // Report timing.
        Info << "ExecutionTime = " << runTime.elapsedCpuTime() << " s"
             << "  ClockTime = " << runTime.elapsedClockTime() << " s"
             << nl << endl;
    }

    Info << "Ending the simulation" << endl;

    return 0;
}


// ************************************************************************* //
