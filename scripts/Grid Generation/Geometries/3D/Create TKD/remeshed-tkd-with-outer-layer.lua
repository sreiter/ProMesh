-- pm-declare-name: Create Remeshed TKD With Outer Layer
-- pm-declare-input: innerSubset | inner subset | integer | val = 0; min = -1
-- pm-declare-input: outerSubset | outer subset | integer | val = 1; min = -1
-- pm-declare-input: a | a | double | val = 0.3; min = 0; step = 0.1
-- pm-declare-input: w | w | double | val = 1; min = 0; step = 0.1
-- pm-declare-input: h | h | double | val = 0.25; min = 0; step = 0.1
-- pm-declare-input: d | d | double | val = 0.05; min = 0; step = 0.1

newMesh = Mesh()

CreateTKDWithOuterLayer (newMesh, innerSubset, outerSubset, a, w, h, d)
SelectElementsByIndexRange (newMesh, "11,17,16,10,9,15,14,8,7,13,12,6", "134,131,109,126,129,121,124,116,119,111,114,106,143,142,141,140,144-146,135-139", "", "", true)
RefineWithSnapPoints (newMesh)
SelectElementsByIndexRange (newMesh, "37,11", "", "", "", true)
local projDist = ProjectedDistance (newMesh, Vec3d(0,0,1))
SelectElementsByIndexRange (newMesh, "52-54,63,62,61,60,59,58,57,56,55,69,68,64-67,75,74,73,72,71,70", "134,151,150,147,146,143,142,139,138,135,130,131,163,162,153,156,157,160,175,174,171,170,167,166", "", "", true)
MoveVerticesToProjectedEdgeLength (newMesh, projDist, Vec3d(0,0,1))
SelectElementsByIndexRange (newMesh, "", "137,136,129,132,133,152,149,148,145,144,141,140,154,155,158,159,176,173,172,169,168,165,164,161,40,36,35,31,60,56,55,51,50,46,45,41,52,54,57,59,32,34,37,39,42,44,47,49", "", "", true)
Refine (newMesh)
SelectElementsByIndexRange (newMesh, "88-99,82-87,76-81,123,112-122,107-111,100-106", "223,202,203,206,207,210,211,214,215,218,219,222,188,189,192,193,196,197,200,177,180,181,184,185,272,250,252,254,256,258,260,262,264,266,268,270,239,241,243,245,247,225,227,229,231,233,235,237", "", "", true)
MoveVerticesToProjectedEdgeLength (newMesh, projDist, Vec3d(0,0,1))
SelectElementsByIndexRange (newMesh, "85,91,87,89,77,99,79,97,81,95,83,93,114,105,116,103,118,101,120,111,122,109,112,107", "133,89,137,132,92,136,131,95,135,129,98,138,130,83,139,134,86,140,41,42,37-40", "", "", true)
RefineWithSnapPoints (newMesh)
SelectElementsByIndexRange (newMesh, "144-147,142,143,139-141,124-138", "371,374,375,378,379,370,364,365,367,334,335,337,340,341,343,346,347,349,352,353,355,358,359,361", "", "", true)
MoveVerticesToProjectedEdgeLength (newMesh, projDist, Vec3d(0,0,1))
SelectElementsByIndexRange (newMesh, "82,94,92,84,86,90,88,76,78,98,96,80,123,108,113,106,115,104,117,102,100,119,121,110", "418,363,366,368,422,383,333,336,338,387,394,344,342,339,390,397,345,348,350,401,408,356,354,351,404,411,357,360,362,415,433,380,434,423,369,424,425,372,426,427,373,428,430,376,429,431,377,432", "", "", true)
RefineWithSnapPoints (newMesh)
SelectElementsByIndexRange (newMesh, "190-195,180,179,178,181-183,186,185,184,187-189,163-177,152,151,150,149,148,153-162", "476,474,472,482,479,478,448,450,452,458,455,454,460,462,464,470,467,466,426,422,424,419,418,436,433,432,430,428,446,444,442,439,438,388,389,392,394,396,398,399,402,404,406,416,413,412,410,408", "", "", true)
MoveVerticesToProjectedEdgeLength (newMesh, projDist, Vec3d(0,0,1))
SelectElementsByIndexRange (newMesh, "", "", "", "190,95,180,94,182,93,184,92,186,91,188,90,138,96,141,97,142,98,132,99,134,100,101,137,139,102,103,136,104,135,105,133,143,106,140,107", true)
CreateHexahedraFromSelectedPrisms (newMesh)

SelectAll (newMesh)
AssignSubsetColors (newMesh)
SetSubsetName (newMesh, innerSubset, "inner")
SetSubsetName (newMesh, outerSubset, "outer")

CopySelection (newMesh, mesh)
