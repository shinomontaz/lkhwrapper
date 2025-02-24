package main

/*
   #cgo CFLAGS: -I../lkh/INCLUDE
   #cgo LDFLAGS: -L../ -llkh
   #include <stdlib.h>
   #include <stdio.h>
   #include "LKH.h"
   #include "my.h"
*/
import "C"
import "unsafe"

func main() {
	params := C.createDefaultMyParameters()
	params.Runs = 1

	var problem C.MyProblem
	problem.ProblemType = C.TSP
	problem.Dimension = 4
	problem.EdgeWeightType = C.EUC_2D

	problem.nodeCoords = (*C.NodeCoord)(C.malloc(C.size_t(10) * C.size_t(unsafe.Sizeof(C.NodeCoord{}))))
	defer C.free(unsafe.Pointer(problem.nodeCoords))

	coords := [][2]float64{
		{0, 0},
		{10, -5.3},
		{1.0, -2.1},
		{11, -3},
	}

	for i, p := range coords {
		(*C.NodeCoord)(unsafe.Pointer(uintptr(unsafe.Pointer(problem.nodeCoords)) + uintptr(i)*unsafe.Sizeof(C.NodeCoord{}))).Id = C.int(i + 1)
		(*C.NodeCoord)(unsafe.Pointer(uintptr(unsafe.Pointer(problem.nodeCoords)) + uintptr(i)*unsafe.Sizeof(C.NodeCoord{}))).X = C.double(p[0])
		(*C.NodeCoord)(unsafe.Pointer(uintptr(unsafe.Pointer(problem.nodeCoords)) + uintptr(i)*unsafe.Sizeof(C.NodeCoord{}))).Y = C.double(p[1])
	}

	C.ReadParametersFromStruct(&params)
	C.MyReadProblemFromStruct(&problem)

	C.MySolve()
}
