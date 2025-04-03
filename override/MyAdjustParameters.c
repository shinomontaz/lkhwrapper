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
#include "Heap.h"

void MyAdjustParameters(void) {
    int i, j, K;

    /* Adjust parameters */
    if (ProblemType == GCTSP) {
        Node *N;
        DimensionSaved = Dim = ++Dimension;
        N =  &NodeSet[Dimension];
        Link(&NodeSet[Dimension - 1], N);
        Link(N, FirstNode);
        N->Id = N->OriginalId = Dimension;
        if (MergeTourFiles >= 1)
            N->MergeSuc = (Node **) calloc(MergeTourFiles, sizeof(Node *));
        MTSPDepot = Dimension;
        Depot = &NodeSet[Dimension];
    }
    if (Seed == 0)
        Seed = (unsigned) (time(0) * (size_t) (&Seed));
    if (Precision == 0)
        Precision = 100;
    if (InitialStepSize == 0)
        InitialStepSize = 1;
    if (MaxSwaps < 0)
        MaxSwaps = Dimension;
    if (KickType > Dimension / 2)
        KickType = Dimension / 2;
    if (Runs == 0)
        Runs = 10;
    if (MaxCandidates > Dimension - 1)
        MaxCandidates = Dimension - 1;
    if (ExtraCandidates > Dimension - 1)
        ExtraCandidates = Dimension - 1;
    if (Scale < 1)
        Scale = 1;
    if (SubproblemSize >= Dimension)
        SubproblemSize = Dimension;
    else if (SubproblemSize == 0) {
        if (AscentCandidates > Dimension - 1)
            AscentCandidates = Dimension - 1;
        if (InitialPeriod < 0) {
            InitialPeriod = Dimension / 2;
            if (InitialPeriod < 100)
                InitialPeriod = 100;
        }
        if (Excess < 0)
            Excess = 1.0 / DimensionSaved * Salesmen;
        if (MaxTrials == -1)
            MaxTrials = Dimension;
        HeapMake(Dimension);
    }
    if (POPMUSIC_MaxNeighbors > Dimension - 1)
        POPMUSIC_MaxNeighbors = Dimension - 1;
    if (POPMUSIC_SampleSize > Dimension)
        POPMUSIC_SampleSize = Dimension;
    Depot = &NodeSet[MTSPDepot];
    if (ProblemType == CVRP ||
        ProblemType == CluVRP || ProblemType == SoftCluVRP) {
        Node *N;
        int MinSalesmen;
        if (Capacity <= 0)
            eprintf("CAPACITY not specified");
        TotalDemand = 0;
        N = FirstNode;
        do
            TotalDemand += N->Demand;
        while ((N = N->Suc) != FirstNode);
        MinSalesmen =
            TotalDemand / Capacity + (TotalDemand % Capacity != 0);
        if (Salesmen == 1) {
            Salesmen = MinSalesmen;
            if (Salesmen > Dimension)
                eprintf("CVRP: SALESMEN larger than DIMENSION");
        } else if (Salesmen < MinSalesmen)
            eprintf("CVRP: SALESMEN too small to meet demand");
        assert(Salesmen >= 1 && Salesmen <= Dimension);
        if (Salesmen == 1 && ProblemType == CVRP)
            ProblemType = TSP;
        Penalty = ProblemType == CVRP ? Penalty_CVRP : Penalty_CluVRP;
    } else if (ProblemType == SOP || ProblemType == M1_PDTSP ||
            ProblemType == PCTSP) {
        Constraint *Con;
        Node *Ni, *Nj;
        int n, k;
        OldDistance = Distance;
        Distance = Distance_SOP;
        if (ProblemType == M1_PDTSP) {
            for (i = 2; i < Dim; i++) {
                Ni = &NodeSet[i];
                for (k = n = 0; k < DemandDimension; k++) {
                    n = Ni->M_Demand[k];
                    if (n >= 0)
                        continue;
                    for (j = 2; j < Dim; j++) {
                        if (j == i)
                            continue;
                        Nj = &NodeSet[j];
                        if (Nj->M_Demand[k] == -n) {
                            Ni->C[j] = -1;
                            break;
                        }
                    }
                }
            }
        }
        for (j = 1; j <= Dim; j++) {
            Nj = &NodeSet[j];
            for (i = 1; i <= Dim; i++) {
                if (i != j && Nj->C[i] == -1) {
                    Ni = &NodeSet[i];
                    Con = (Constraint *) malloc(sizeof(Constraint));
                    Con->t1 = Ni;
                    Con->t2 = Nj;
                    Con->Suc = FirstConstraint;
                    FirstConstraint = Con;
                    Con->Next = Ni->FirstConstraint;
                    Ni->FirstConstraint = Con;
                }
            }
        }
        if (ProblemType != PCTSP)
            Salesmen = 1;
        Penalty = ProblemType == SOP ? Penalty_SOP :
                ProblemType == PCTSP ? Penalty_PCTSP : Penalty_M1_PDTSP;
    }
    if (ProblemType == TSPTW) {
        Salesmen = 1;
        Penalty = Penalty_TSPTW;
    } else
        TSPTW_Makespan = 0;
    if (Salesmen > 1) {
        if (Salesmen > Dim && MTSPMinSize > 0)
            eprintf("Too many salesmen/vehicles (>= DIMENSION)");
        MTSP2TSP();
    } else if (MTSPMaxSize == -1)
        MTSPMaxSize = Dimension - 1;
    if (ProblemType == STTSP)
        STTSP2TSP();
    if (ProblemType == ACVRP || ProblemType == ADCVRP)
        Penalty = Penalty_ACVRP;
    else if (ProblemType == CCVRP)
        Penalty = Penalty_CCVRP;
    else if (ProblemType == CTSP)
        Penalty = Penalty_CTSP;
    else if (ProblemType == CBTSP)
        Penalty = Penalty_CBTSP;
    else if (ProblemType == CTSP_D)
        Penalty = Penalty_CTSP_D;
    else if (ProblemType == CBnTSP)
        Penalty = Penalty_CBnTSP;
    else if (ProblemType == CVRPTW)
        Penalty = Penalty_CVRPTW;
    else if (ProblemType == GCTSP)
        Penalty = Penalty_GCTSP;
    else if (ProblemType == CCCTSP)
        Penalty = Penalty_CCCTSP;
    else if (ProblemType == KTSP)
        Penalty = Penalty_KTSP;
    else if (ProblemType == MLP)
        Penalty = Penalty_MLP;
    else if (ProblemType == MSCTSP)
        Penalty = Penalty_MSCTSP;
    else if (ProblemType == OVRP)
        Penalty = Penalty_OVRP;
    else if (ProblemType == PCTSP)
        Penalty = Penalty_PCTSP;
    else if (ProblemType == PDTSP)
        Penalty = Penalty_PDTSP;
    else if (ProblemType == PDTSPF)
        Penalty = Penalty_PDTSPF;
    else if (ProblemType == PDTSPL)
        Penalty = Penalty_PDTSPL;
    else if (ProblemType == PDPTW)
        Penalty = Penalty_PDPTW;
    else if (ProblemType == ONE_PDTSP)
        Penalty = Penalty_1_PDTSP;
    else if (ProblemType == M_PDTSP)
        Penalty = Penalty_M_PDTSP;
    else if (ProblemType == M1_PDTSP)
        Penalty = Penalty_M1_PDTSP;
    else if (ProblemType == PTSP)
        Penalty = Penalty_PTSP;
    else if (ProblemType == RCTVRP || ProblemType == RCTVRPTW)
        Penalty = Penalty_RCTVRP;
    else if (ProblemType == TRP)
        Penalty = Penalty_TRP;
    else if (ProblemType == TSPDL)
        Penalty = Penalty_TSPDL;
    else if (ProblemType == TSPPD)
        Penalty = Penalty_TSPPD;
    if (ProblemType == VRPB)
        Penalty = Penalty_VRPB;
    else if (ProblemType == VRPBTW)
        Penalty = Penalty_VRPBTW;
    else if (ProblemType == VRPPD)
        Penalty = Penalty_VRPPD;
    if (BWTSP_B > 0) {
        if (Penalty)
            eprintf("BWTSP not compatible with problem type %d\n", ProblemType);
        ProblemType = BWTSP;
        free(Type);
        Penalty = Penalty_BWTSP;
        if (BWTSP_L != INT_MAX)
            BWTSP_L *= Scale;
    }
    if (Penalty && (SubproblemSize > 0 || SubproblemTourFile))
        eprintf("Partitioning not implemented for constrained problems");
    Depot->DepotId = 1;
    for (i = Dim + 1; i <= DimensionSaved; i++)
        NodeSet[i].DepotId = i - Dim + 1;
    if (Dimension != DimensionSaved && ProblemType != PCTSP) {
        NodeSet[Depot->Id + DimensionSaved].DepotId = 1;
        for (i = Dim + 1; i <= Dim + Salesmen -1; i++)
            NodeSet[i + DimensionSaved].DepotId = i - Dim + 1;
    }
    if (ProblemType == PCTSP) {
        for (j = 1; j <= Salesmen; j++) {
            NodeSet[DimensionSaved + Depot->Id].ColorAllowed[j] = 0;
            for (i = Dim + 1; i <= DimensionSaved; i++)
                NodeSet[DimensionSaved + i].ColorAllowed[j] = 0;
        }
        NodeSet[DimensionSaved + Depot->Id].ColorAllowed[Depot->DepotId] = 1;
        for (i = Dim + 1; i <= DimensionSaved; i++)
            NodeSet[DimensionSaved + i].ColorAllowed[NodeSet[i].DepotId] = 1;
    }
    if (Scale < 1)
        Scale = 1;
    else {
        Node *Ni = FirstNode;
        do {
            Ni->Earliest *= Scale;
            Ni->Latest *= Scale;
            Ni->ServiceTime *= Scale;
        } while ((Ni = Ni->Suc) != FirstNode);
        ServiceTime *= Scale;
        RiskThreshold *= Scale;
        if (DistanceLimit != DBL_MAX)
            DistanceLimit *= Scale;
    }
    if (ServiceTime != 0) {
        for (i = 1; i <= Dim; i++)
            NodeSet[i].ServiceTime = ServiceTime;
        Depot->ServiceTime = 0;
    }
    if (ProblemType == MSCTSP)
        Scale *= -1;
    if (CostMatrix == 0 && Dimension <= MaxMatrixDimension &&
        Distance != 0 && Distance != Distance_1
        && Distance != Distance_EXPLICIT
        && Distance != Distance_LARGE && Distance != Distance_ATSP
        && Distance != Distance_MTSP && Distance != Distance_SPECIAL) {
        Node *Ni, *Nj;
        CostMatrix = (int *) calloc((size_t) Dim * (Dim - 1) / 2, sizeof(int));
        Ni = FirstNode->Suc;
        do {
            Ni->C =
                &CostMatrix[(size_t) (Ni->Id - 1) * (Ni->Id - 2) / 2] - 1;
            if (ProblemType != HPP || Ni->Id <= Dim)
                for (Nj = FirstNode; Nj != Ni; Nj = Nj->Suc)
                    Ni->C[Nj->Id] = Fixed(Ni, Nj) ? 0 : Distance(Ni, Nj);
            else
                for (Nj = FirstNode; Nj != Ni; Nj = Nj->Suc)
                    Ni->C[Nj->Id] = 0;
        }
        while ((Ni = Ni->Suc) != FirstNode);
        c = 0;
        WeightType = EXPLICIT;
    }
    if (ProblemType == TSPTW ||
        ProblemType == CVRPTW || ProblemType == VRPBTW ||
        ProblemType == PDPTW || ProblemType == RCTVRPTW ||
        ProblemType == KTSP) {
        M = INT_MAX / 2 / Precision;
        for (i = 1; i <= Dim; i++) {
            Node *Ni = &NodeSet[i];
            for (j = 1; j <= Dim; j++) {
                Node *Nj = &NodeSet[j];
                if (Ni != Nj &&
                    Ni->Earliest + Ni->ServiceTime + Ni->C[j] > Nj->Latest)
                    Ni->C[j] = M;
            }
        }
        if (ProblemType == TSPTW) {
            for (i = 1; i <= Dim; i++)
                for (j = 1; j <= Dim; j++)
                    if (j != i)
                        NodeSet[i].C[j] += NodeSet[i].ServiceTime;
        }
    }
    C = WeightType == EXPLICIT ? C_EXPLICIT : C_FUNCTION;
    D = WeightType == EXPLICIT ? D_EXPLICIT : D_FUNCTION;
    if (ProblemType != CVRP && ProblemType != CVRPTW &&
        ProblemType != CTSP && ProblemType != STTSP &&
        ProblemType != CBTSP && ProblemType != CBnTSP &&
        ProblemType != TSP && ProblemType != ATSP &&
        ProblemType != KTSP && ProblemType != CluVRP &&
        ProblemType != SoftCluVRP && ProblemType != GCTSP &&
        ProblemType != CCCTSP && ProblemType != MSCTSP &&
        ProblemType != PCTSP) {
        M = INT_MAX / 2 / Precision;
        for (i = Dim + 1; i <= DimensionSaved; i++) {
            for (j = 1; j <= DimensionSaved; j++) {
                if (j == i)
                    continue;
                if (j == MTSPDepot || j > Dim)
                    NodeSet[i].C[j] = NodeSet[MTSPDepot].C[j] = M;
                NodeSet[i].C[j] = NodeSet[MTSPDepot].C[j];
                NodeSet[j].C[i] = NodeSet[j].C[MTSPDepot];
            }
        }
        if (ProblemType == CCVRP || ProblemType == OVRP)
            for (i = 1; i <= Dim; i++)
                NodeSet[i].C[MTSPDepot] = 0;
    }
    if (Precision > 1 && CostMatrix) {
        for (i = 2; i <= Dim; i++) {
            Node *N = &NodeSet[i];
            for (j = 1; j < i; j++)
                if (N->C[j] * Precision / Precision != N->C[j])
                    eprintf("PRECISION (= %d) is too large", Precision);
        }
    }
    if (SubsequentMoveType == 0) {
        SubsequentMoveType = MoveType;
        SubsequentMoveTypeSpecial = MoveTypeSpecial;
    }
    K = MoveType >= SubsequentMoveType || !SubsequentPatching ?
        MoveType : SubsequentMoveType;
    if (PatchingC > K)
        PatchingC = K;
    if (PatchingA > 1 && PatchingA >= PatchingC)
        PatchingA = PatchingC > 2 ? PatchingC - 1 : 1;
    if (NonsequentialMoveType == -1 ||
        NonsequentialMoveType > K + PatchingC + PatchingA - 1)
        NonsequentialMoveType = K + PatchingC + PatchingA - 1;
    if (PatchingC >= 1) {
        BestMove = BestSubsequentMove = BestKOptMove;
        if (!SubsequentPatching && SubsequentMoveType <= 5) {
            MoveFunction BestOptMove[] =
                { 0, 0, Best2OptMove, Best3OptMove,
                Best4OptMove, Best5OptMove
            };
            BestSubsequentMove = BestOptMove[SubsequentMoveType];
        }
    } else {
        MoveFunction BestOptMove[] = { 0, 0, Best2OptMove, Best3OptMove,
            Best4OptMove, Best5OptMove
        };
        BestMove = MoveType <= 5 ? BestOptMove[MoveType] : BestKOptMove;
        BestSubsequentMove = SubsequentMoveType <= 5 ?
            BestOptMove[SubsequentMoveType] : BestKOptMove;
    }
    if (MoveTypeSpecial)
        BestMove = BestSpecialOptMove;
    if (SubsequentMoveTypeSpecial)
        BestSubsequentMove = BestSpecialOptMove;
    if (ProblemType == HCP || ProblemType == HPP)
        MaxCandidates = 0;
    if (TraceLevel >= 1) {
        printff("done\n");
        PrintParameters();
    } else
        printff("PROBLEM_FILE = %s\n",
                ProblemFileName ? ProblemFileName : "");
    if (ProblemType == MSCTSP)
        Optimum *= -1;
    fclose(ProblemFile);
    if (InitialTourFileName)
        ReadTour(InitialTourFileName, &InitialTourFile);
    if (InputTourFileName)
        ReadTour(InputTourFileName, &InputTourFile);
    if (SubproblemTourFileName && SubproblemSize > 0)
        ReadTour(SubproblemTourFileName, &SubproblemTourFile);
    if (MergeTourFiles >= 1) {
        free(MergeTourFile);
        MergeTourFile = (FILE **) malloc(MergeTourFiles * sizeof(FILE *));
        for (i = 0; i < MergeTourFiles; i++)
            ReadTour(MergeTourFileName[i], &MergeTourFile[i]);
    }
    free(LastLine);
    LastLine = 0;
}