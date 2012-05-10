fvVectorMatrix UEqn
(
    fvm::ddt(rho, U)
    + fvm::div(phi, U)
    + turbulence->divDevRhoReff(U)
);

srfZones.addSu(UEqn);

solve(UEqn == -fvc::grad(p));
