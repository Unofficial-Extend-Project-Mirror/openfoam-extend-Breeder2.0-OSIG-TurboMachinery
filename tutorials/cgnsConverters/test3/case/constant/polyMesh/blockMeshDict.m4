//process this file using: m4 -P blockMeshDict.m4 > blockMeshDict

//m4 definitions -----------------------------
m4_changecom(//)m4_changequote([,])
m4_define(calc, [m4_esyscmd(perl -e 'printf ($1)')])
m4_define(pi, 3.14159265358979323844)
m4_define(rad, [calc($1*pi/180.0)])
m4_define(VCOUNT, 0)
m4_define(vlabel, [[// ]Vertex $1 = VCOUNT m4_define($1, VCOUNT)m4_define([VCOUNT], m4_incr(VCOUNT))])

//Geometry -----------------------------------
// 3 planes levels
m4_define(zA, -1.0)
m4_define(zB,  1.0)
m4_define(zC,  3.0)

// Angular positions
m4_define(angleDelta, rad(25.0))
m4_define(angleA, rad( 0.0))
m4_define(angleB, rad(25.0))
m4_define(angleC, rad(50.0))
m4_define(angleD, rad(75.0))

m4_define(angleBX, calc(angleB+angleDelta))
m4_define(angleCX, calc(angleC+angleDelta))
m4_define(angleDX, calc(angleD+angleDelta))

// Radial dimensions
m4_define(r1, 1.0)
m4_define(r2, 4.0)

// Mesh parameters
m4_define(nCells, 9)
m4_define(BLOCKSIZE, nCells nCells nCells)
m4_define(grading, 10.0)

FoamFile
{
    version         2.0;
    format          ascii;

    root            "";
    case            "";
    instance        "";
    local           "";

    class           dictionary;
    object          blockMeshDict;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

convertToMeters 1;

vertices
(
//Plane A:
(calc(r1*cos(angleA)) calc(r1*sin(angleA)) zA) vlabel(A0)
(calc(r2*cos(angleA)) calc(r2*sin(angleA)) zA) vlabel(A1)
(calc(r1*cos(angleB)) calc(r1*sin(angleB)) zA) vlabel(A2)
(calc(r2*cos(angleB)) calc(r2*sin(angleB)) zA) vlabel(A3)
(calc(r1*cos(angleC)) calc(r1*sin(angleC)) zA) vlabel(A4)
(calc(r2*cos(angleC)) calc(r2*sin(angleC)) zA) vlabel(A5)

//Plane B:
(calc(r1*cos(angleA)) calc(r1*sin(angleA)) zB) vlabel(B0)
(calc(r2*cos(angleA)) calc(r2*sin(angleA)) zB) vlabel(B1)
(calc(r1*cos(angleB)) calc(r1*sin(angleB)) zB) vlabel(B2)
(calc(r2*cos(angleB)) calc(r2*sin(angleB)) zB) vlabel(B3)
(calc(r1*cos(angleC)) calc(r1*sin(angleC)) zB) vlabel(B4)
(calc(r2*cos(angleC)) calc(r2*sin(angleC)) zB) vlabel(B5)
(calc(r1*cos(angleD)) calc(r1*sin(angleD)) zB) vlabel(B6)
(calc(r2*cos(angleD)) calc(r2*sin(angleD)) zB) vlabel(B7)

//Plane C:
(calc(r1*cos(angleB )) calc(r1*sin(angleB )) zC) vlabel(C0)
(calc(r2*cos(angleBX)) calc(r2*sin(angleBX)) zC) vlabel(C1)
(calc(r1*cos(angleC )) calc(r1*sin(angleC )) zC) vlabel(C2)
(calc(r2*cos(angleCX)) calc(r2*sin(angleCX)) zC) vlabel(C3)
(calc(r1*cos(angleD )) calc(r1*sin(angleD )) zC) vlabel(C4)
(calc(r2*cos(angleDX)) calc(r2*sin(angleDX)) zC) vlabel(C5)

);

blocks          
(
    hex ( A0 A1 A3 A2 B0 B1 B3 B2 ) (BLOCKSIZE) simpleGrading (1 1 grading)
    hex ( A2 A3 A5 A4 B2 B3 B5 B4 ) (BLOCKSIZE) simpleGrading (1 1 grading)
    hex ( B2 B3 B5 B4 C0 C1 C3 C2 ) (BLOCKSIZE) simpleGrading (1 1 calc(1.0/grading))
    hex ( B4 B5 B7 B6 C2 C3 C5 C4 ) (BLOCKSIZE) simpleGrading (1 1 calc(1.0/grading))
);

edges           
(
	// --- PLANE A
    arc  A0 A2  (calc(r1*cos((angleA+angleB)/2)) calc(r1*sin((angleA+angleB)/2)) zA)
    arc  A2 A4  (calc(r1*cos((angleB+angleC)/2)) calc(r1*sin((angleB+angleC)/2)) zA)
    arc  A1 A3  (calc(r2*cos((angleA+angleB)/2)) calc(r2*sin((angleA+angleB)/2)) zA)
    arc  A3 A5  (calc(r2*cos((angleB+angleC)/2)) calc(r2*sin((angleB+angleC)/2)) zA)
    	// --- PLANE B
    arc  B0 B2  (calc(r1*cos((angleA+angleB)/2)) calc(r1*sin((angleA+angleB)/2)) zB)
    arc  B2 B4  (calc(r1*cos((angleB+angleC)/2)) calc(r1*sin((angleB+angleC)/2)) zB)
    arc  B4 B6  (calc(r1*cos((angleC+angleD)/2)) calc(r1*sin((angleC+angleD)/2)) zB)
    arc  B1 B3  (calc(r2*cos((angleA+angleB)/2)) calc(r2*sin((angleA+angleB)/2)) zB)
    arc  B3 B5  (calc(r2*cos((angleB+angleC)/2)) calc(r2*sin((angleB+angleC)/2)) zB)
    arc  B5 B7  (calc(r2*cos((angleC+angleD)/2)) calc(r2*sin((angleC+angleD)/2)) zB)
    	// --- PLANE C
    arc  C0 C2  (calc(r1*cos((angleB+angleC)/2)) calc(r1*sin((angleB+angleC)/2)) zC)
    arc  C2 C4  (calc(r1*cos((angleC+angleD)/2)) calc(r1*sin((angleC+angleD)/2)) zC)
    arc  C1 C3  (calc(r2*cos((angleBX+angleCX)/2)) calc(r2*sin((angleBX+angleCX)/2)) zC)
    arc  C3 C5  (calc(r2*cos((angleCX+angleDX)/2)) calc(r2*sin((angleCX+angleDX)/2)) zC)
    
    arc  C0 C1  (calc(0.5*(r1+r2)*cos(angleBX)) calc(0.5*(r1+r2)*sin(angleBX)) zC)
    arc  C2 C3  (calc(0.5*(r1+r2)*cos(angleCX)) calc(0.5*(r1+r2)*sin(angleCX)) zC)
    arc  C4 C5  (calc(0.5*(r1+r2)*cos(angleDX)) calc(0.5*(r1+r2)*sin(angleDX)) zC)
);

patches         
(
    patch inflow
    (
	( B3 B5 C3 C1 )
	( B5 B7 C5 C3 )
    )
    patch outflow
    (
        ( A0 B0 B2 A2 )
	( A2 B2 B4 A4 )
    )
    wall hub
    (
	( B2 C0 C2 B4 )
	( B4 C2 C4 B6 )
	(C1 C3 C2 C0)
	(C3 C5 C4 C2)
    )
    wall shroud
    (
        ( A1 A3 B3 B1 )
	( A3 A5 B5 B3 )
	(A0 A2 A3 A1)
	(A2 A4 A5 A3)
    )
    cyclic perio1
    (
        (A0 A1 B1 B0)
        (B0 B1 B3 B2)
        (B2 B3 C1 C0)
        
	(A4 B4 B5 A5)
        (B4 B5 B7 B6)
        (B6 C4 C5 B7)
    )
);

mergePatchPairs
(
);
