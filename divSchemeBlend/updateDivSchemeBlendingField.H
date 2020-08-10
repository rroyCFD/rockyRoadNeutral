// Re-read blending information
{
    schemeBlendDict = mesh.schemesDict().subDict("schemeBlending");
    {
        List<List<scalar> > temp(schemeBlendDict.lookup("faceSizeBlendingTable"));
        profileTable = temp;
    }

    // resize and setSize are alias function
    // faceAreaList.resize(profileTable.size(), 0.0);
    faceAreaList.setSize(profileTable.size(), 0.0);
    UBlendingList.setSize(profileTable.size(),0.0);
    TBlendingList.setSize(profileTable.size(),0.0);

    forAll(faceAreaList,i)
    {
        faceAreaList[i] = profileTable[i][0];
        UBlendingList[i] = profileTable[i][1];
        TBlendingList[i] = profileTable[i][2];
    }


    zBlending1 = readScalar(schemeBlendDict.lookup("heightBlending_z1"));
    zBlending2 = readScalar(schemeBlendDict.lookup("heightBlending_z2"));

    blendingFactorUz1 = readScalar(schemeBlendDict.lookup("heightBlendingFactorU_z1"));
    blendingFactorTz1 = readScalar(schemeBlendDict.lookup("heightBlendingFactorT_z1"));
    blendingFactorUz2 = readScalar(schemeBlendDict.lookup("heightBlendingFactorU_z2"));
    blendingFactorTz2 = readScalar(schemeBlendDict.lookup("heightBlendingFactorT_z2"));


    // bool useWallDistZ(schemeBlendDict.lookupOrDefault<bool>("useWallDistZ",false));
    useWallDistZ = (schemeBlendDict.lookupOrDefault<bool>("useWallDistZ",false));


    // Check to see if the blending factors or face sizes have changed.
    // If so, recalculate the U and T blending factor field.
    if ((profileTable != profileTableOld) ||
        (zBlending1 != zBlending1Old) ||
        (zBlending2 != zBlending2Old) ||
        (blendingFactorUz1 != blendingFactorUz1Old) ||
        (blendingFactorTz1 != blendingFactorTz1Old) ||
        (blendingFactorUz2 != blendingFactorUz2Old) ||
        (blendingFactorTz2 != blendingFactorTz2Old) ||
        (useWallDistZ != useWallDistZOld)
    )
    {
        Info << "Updating scheme blending factor field" << endl;
        #include "makeDivSchemeBlendingField.H"
    }
}