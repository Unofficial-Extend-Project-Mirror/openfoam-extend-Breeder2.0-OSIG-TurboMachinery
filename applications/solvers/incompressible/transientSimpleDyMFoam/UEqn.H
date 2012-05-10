    fvVectorMatrix UEqn
    (
        fvm::ddt(U)
      + fvm::div(phi, U)
      + turbulence->divDevReff(U)
    );

    UEqn.relax();

    // Solve the momentum equation
    solve(UEqn == -fvc::grad(p));
