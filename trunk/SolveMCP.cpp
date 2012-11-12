#include <iostream>
#include "Standalone_Path.h"
#include "SimpleLCP.h"
//#include "Matrix.h"
//#include "DiagMatrix.h"
#include "SolveMCP.h"

/**
* Wrapper to call Path's simple LCP solver
*/
bool SolveMCP(double* outZ, double *lowerBound, double *upperBound,
			  const csMatrix & B, double* b)
{

	assert(B.size1() == B.size2());
	bool result;

	unsigned int i = 0;

	size_t dimension = B.size1();
	if (dimension == 0) 
	{ // Nothing to do
		return true;
	}
	//cs_print(B._mat,0);
	int numNonZero = B._mat->nz ;
		
	assert(numNonZero >= 0);
	
	int* rowArray = new int[numNonZero];
	int* colArray = new int[numNonZero];
	double* valArray = B._mat->x;

	for (i=0; i< numNonZero; i++)
	{
		rowArray[i] = B._mat->i[i] +1;
		colArray[i] = B._mat->p[i] +1;
	}

	double* bArray = b;

	MCP_Termination status;
	// Call Path's solver
	SimpleLCP(dimension,
		numNonZero,
		rowArray,
		colArray,
		valArray,
		bArray,
		lowerBound,
		upperBound,
		& status,
		outZ);

	if (status != MCP_Solved)
	{
		std::cerr << "ERROR: The PATH MCP solver failed with error code "  << status << std::endl;
		result = false;
	}
	else
		result = true;

	// Cleanup
	delete[] rowArray;
	delete[] colArray;
	//delete[] valArray;
	//delete[] bArray;

	// Return types can be found in path_include/Types.h
	return result;
}
