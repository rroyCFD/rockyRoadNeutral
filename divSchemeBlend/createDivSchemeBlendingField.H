// Create the blending factor fields.
Info << "Creating divergence scheme blending factor field, UBlendingFactor" << endl;
surfaceScalarField UBlendingFactor
(
    IOobject
    (
        "UBlendingFactor",
        runTime.timeName(),
        mesh,
        IOobject::READ_IF_PRESENT,
        IOobject::AUTO_WRITE
    ),
    mesh,
    1.0
);

Info << "Creating divergence scheme blending factor field, TBlendingFactor" << endl;
surfaceScalarField TBlendingFactor
(
    IOobject
    (
        "TBlendingFactor",
        runTime.timeName(),
        mesh,
        IOobject::READ_IF_PRESENT,
        IOobject::AUTO_WRITE
    ),
    mesh,
    1.0
);

// Read blending information.
dictionary schemeBlendDict = mesh.schemesDict().subDict("schemeBlending");
// Info << schemeBlendDict << endl;

List<List<scalar> > profileTable(schemeBlendDict.lookup("faceSizeBlendingTable"));
scalarField faceAreaList(profileTable.size(),0.0);
scalarField UBlendingList(profileTable.size(),0.0);
scalarField TBlendingList(profileTable.size(),0.0);

forAll(faceAreaList,i)
{
    faceAreaList[i] = profileTable[i][0];
    UBlendingList[i] = profileTable[i][1];
    TBlendingList[i] = profileTable[i][2];
}

scalar zBlending1 = readScalar(schemeBlendDict.lookup("heightBlending_z1"));
scalar zBlending2 = readScalar(schemeBlendDict.lookup("heightBlending_z2"));

scalar blendingFactorUz1 = readScalar(schemeBlendDict.lookup("heightBlendingFactorU_z1"));
scalar blendingFactorUz2 = readScalar(schemeBlendDict.lookup("heightBlendingFactorU_z2"));

scalar blendingFactorTz1 = readScalar(schemeBlendDict.lookup("heightBlendingFactorT_z1"));
scalar blendingFactorTz2 = readScalar(schemeBlendDict.lookup("heightBlendingFactorT_z2"));

bool useWallDistZ(schemeBlendDict.lookupOrDefault<bool>("useWallDistZ",false));


// Create the old value of these quantities for checking for updated files.
List<List<scalar> > profileTableOld = profileTable;

scalar  zBlending1Old, zBlending2Old,
        blendingFactorUz1Old, blendingFactorUz2Old,
        blendingFactorTz1Old,  blendingFactorTz2Old,
        useWallDistZOld;

Info << "Creating scheme blending factor field" << endl;
#include "makeDivSchemeBlendingField.H"