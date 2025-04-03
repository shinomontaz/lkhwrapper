#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <float.h>
#include "LKH.h"
#include "GainType.h"
#include "Genetic.h"
#include "My.h"

static void MyRead_NODE_COORD_SECTION(NodeCoord *nodeCoords, int numNodes);
static void MyRead_EDGE_WEIGHT_TYPE(enum EdgeWeightTypes ewt);

void MyReadProblemFromStruct(MyProblem *problem)
{
    printff("MyReadProblemFromStruct!\n");
    int i, j, K;
    char *Line, *Keyword;

    FreeStructures();

    FirstNode = 0;
    WeightType = WeightFormat = ProblemType = -1;
    CoordType = NO_COORDS;
    Name = CopyWrapper("Unnamed");
    Type = EdgeWeightType = EdgeWeightFormat = 0;
    EdgeDataFormat = NodeCoordType = DisplayDataType = 0;
    Distance = 0;
    C = 0;
    c = 0;

    ProblemType = problem->ProblemType;
    Dimension = problem->Dimension;
    DimensionSaved = Dim = Dimension;
    
    Capacity = (int)problem->Capacity;

    printff("MyReadProblemFromStruct: ProblemType 1: %d, Dimension:%d\n", ProblemType, Dimension);
    
    MyRead_EDGE_WEIGHT_TYPE(problem->EdgeWeightType);
    MyRead_NODE_COORD_SECTION(problem->nodeCoords, problem->Dimension);

    MyAdjustParameters();
}

// Function to read node coordinates from a structure
void MyRead_NODE_COORD_SECTION(NodeCoord *nodeCoords, int numNodes) {

    printff("MyRead_NODE_COORD_SECTION\n");

    Node *N;
    int Id, i;

    CheckSpecificationPartWrapper();

    if (!FirstNode) {
        CreateNodesWrapper();
    }
    N = FirstNode;

    do {
        N->V = 0;
    } while ((N = N->Suc) != FirstNode);

    if (ProblemType == HPP)
        Dimension--;

    for (i = 0; i < numNodes; i++) {
        Id = nodeCoords[i].Id;

        if (Id <= 0 || Id > Dimension) {
            eprintf("NODE_COORD_SECTION: Node number out of range: %d", Id);
        }

        N = &NodeSet[Id];
        if (N->V == 1) {
            eprintf("NODE_COORD_SECTION: Node number occurs twice: %d", N->Id);
        }

        N->V = 1;
        N->X = nodeCoords[i].X;
        N->Y = nodeCoords[i].Y;

        if (CoordType == THREED_COORDS) {
            N->Z = nodeCoords[i].Z;
        }
    }

    N = FirstNode;
    do
        if (!N->V && N->Id <= Dim)
            break;
    while ((N = N->Suc) != FirstNode);
    if (!N->V)
        eprintf("NODE_COORD_SECTION: No coordinates given for node %d",
                N->Id);
    if (ProblemType == HPP)
        Dimension++;
    if (Asymmetric)
        Convert2FullMatrixWrapper();
}

void MyRead_EDGE_WEIGHT_TYPE(enum EdgeWeightTypes ewt) {
    printff("MyRead_EDGE_WEIGHT_TYPE: EdgeWeightTypes %d\n", ewt);
    switch (ewt) {
        case ATT:
            WeightType = ATT;
            Distance = Distance_ATT;
            c = c_ATT;
            CoordType = TWOD_COORDS;
            break;
        case CEIL_2D:
            WeightType = CEIL_2D;
            Distance = Distance_CEIL_2D;
            c = c_CEIL_2D;
            CoordType = TWOD_COORDS;
            break;
        case CEIL_3D:
            WeightType = CEIL_3D;
            Distance = Distance_CEIL_3D;
            c = c_CEIL_3D;
            CoordType = THREED_COORDS;
            break;
        case EUC_2D:
            WeightType = EUC_2D;
            Distance = Distance_EUC_2D;
            c = c_EUC_2D;
            CoordType = TWOD_COORDS;
            if (Scale == -1)
                Scale = 1000;
            break;
        case EUC_3D:
            WeightType = EUC_3D;
            Distance = Distance_EUC_3D;
            c = c_EUC_3D;
            CoordType = THREED_COORDS;
            if (Scale == -1)
                Scale = 1000;
            break;
        case EXPLICIT:
            WeightType = EXPLICIT;
            Distance = Distance_EXPLICIT;
            if (Scale < 1)
                Scale = 1;
            break;
        case FLOOR_2D:
            WeightType = FLOOR_2D;
            Distance = Distance_FLOOR_2D;
            c = c_FLOOR_2D;
            CoordType = TWOD_COORDS;
            break;
        case FLOOR_3D:
            WeightType = FLOOR_3D;
            Distance = Distance_FLOOR_3D;
            c = c_FLOOR_3D;
            CoordType = THREED_COORDS;
            break;
        case MAN_2D:
            WeightType = MAN_2D;
            Distance = Distance_MAN_2D;
            CoordType = TWOD_COORDS;
            break;
        case MAN_3D:
            WeightType = MAN_3D;
            Distance = Distance_MAN_3D;
            CoordType = THREED_COORDS;
            break;
        case MAX_2D:
            WeightType = MAX_2D;
            Distance = Distance_MAX_2D;
            CoordType = TWOD_COORDS;
            break;
        case MAX_3D:
            WeightType = MAX_3D;
            Distance = Distance_MAX_3D;
            CoordType = THREED_COORDS;
            break;
        case GEO:
            WeightType = GEO;
            Distance = Distance_GEO;
            c = c_GEO;
            CoordType = TWOD_COORDS;
            break;
        case GEOM:
            WeightType = GEOM;
            Distance = Distance_GEOM;
            c = c_GEOM;
            CoordType = TWOD_COORDS;
            break;
        case GEO_MEEUS:
            WeightType = GEO_MEEUS;
            Distance = Distance_GEO_MEEUS;
            c = c_GEO_MEEUS;
            CoordType = TWOD_COORDS;
            break;
        case GEOM_MEEUS:
            WeightType = GEOM_MEEUS;
            Distance = Distance_GEOM_MEEUS;
            c = c_GEOM_MEEUS;
            CoordType = TWOD_COORDS;
            break;
        case TOR_2D:
            WeightType = TOR_2D;
            Distance = Distance_TOR_2D;
            CoordType = TWOD_COORDS;
            break;
        case TOR_3D:
            WeightType = TOR_3D;
            Distance = Distance_TOR_3D;
            CoordType = THREED_COORDS;
            break;
        case XRAY1:
            WeightType = XRAY1;
            Distance = Distance_XRAY1;
            CoordType = THREED_COORDS;
            break;
        case XRAY2:
            WeightType = XRAY2;
            Distance = Distance_XRAY2;
            CoordType = THREED_COORDS;
            break;
        case SPECIAL:
            WeightType = SPECIAL;
            Distance = Distance_SPECIAL;
            break;
        default:
            eprintf("Unknown EDGE_WEIGHT_TYPE: %d", ewt);
            break;
    }
}