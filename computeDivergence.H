// Info << "Time before computing divergence = " << runTime.elapsedCpuTime() << " s" << endl;
// Calculate divergence of velocity flux and display
{
    // Compute divergence cell by cell and report statistics.
    volScalarField contErr = fvc::div(phi);
    scalar minLocalContErr = min(contErr).value();
    reduce(minLocalContErr, minOp<scalar>());

    scalar maxLocalContErr = max(contErr).value();
    reduce(maxLocalContErr, maxOp<scalar>());

    scalar avgLocalContErr  = contErr.weightedAverage(mesh.V()).value();
    scalar avgGlobalContErr = mag(contErr)().weightedAverage(mesh.V()).value();

    Info<< "Continuity Report:" << nl
        << "   -Local Cell Continuity Error:" << nl
        << "       minimum: " << minLocalContErr << nl
        << "       maximum: " << maxLocalContErr << nl
        << "       weighted mean: " << avgLocalContErr << nl
        << "       weighted mag-mean: " << avgGlobalContErr << endl;
}
{
    // Compute global divergence over domain boundaries and report statistics.
    Info << "   -Boundary Flux:" << endl;

    scalar globalSumPhiBoundary = 0.0;
    // scalar globalSumAreaBoundary = 0.0;
    forAll(phi.boundaryField(), patchi)
    {
        if ( !mesh.boundary()[patchi].coupled() )
        {
            word boundaryName = mesh.boundary()[patchi].name();
            scalar sumPhiBoundary = 0.0;
            // scalar sumAreaBoundary = 0.0;
            const fvsPatchScalarField& phip = phi.boundaryField()[patchi];
            forAll(phip,i)
            {
                sumPhiBoundary += phip[i];
                // sumAreaBoundary += mesh.boundary()[patchi].magSf()[i];
            }

            reduce(sumPhiBoundary, sumOp<scalar>());
            // reduce(sumAreaBoundary, sumOp<scalar>());

            globalSumPhiBoundary += sumPhiBoundary;
            // globalSumAreaBoundary += sumAreaBoundary;

            // Info << "       " << boundaryName << " - flux: " << sumPhiBoundary << tab << "/ area: " << sumAreaBoundary << endl;
            Info << "       " << boundaryName << " - flux: " << sumPhiBoundary << endl;
        }
    }

    // Info << "       total - flux: " << globalSumPhiBoundary << tab << "/ area: " << globalSumAreaBoundary << endl;
    Info << "       total - flux: " << globalSumPhiBoundary << endl;
}
