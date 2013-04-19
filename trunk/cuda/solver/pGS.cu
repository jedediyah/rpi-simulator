
// Attempt to code projected Gauss-Seidel using CUDA

#include <cuda_runtime.h>
#include <stdio.h>
#include "cublas_v2.h"

#define imin(a,b) (a<b?a:b)

//const int N = 33 * 1024;
//const int N = 200; 
const int N = 10;
const int threadsPerBlock = 256;
const int blocksPerGrid = imin( 32, (N+threadsPerBlock-1) / threadsPerBlock );

/////////////////////////////////////////////////
// Kernel functions

// adds to vectors a & b of length N, storing result in c
__global__ void add( int *a, int *b, int *c, int N ) {
    int tid = blockIdx.x;
    if (tid < N)
        c[tid] = a[tid] + b[tid];
}

// dot product of two vectors a and b, each of length N
__global__ void dot1( float *a, float *b, float *c, int N ) {
    __shared__ float cache[threadsPerBlock];
    int tid = threadIdx.x + blockIdx.x * blockDim.x;
    int cacheIndex = threadIdx.x;
    float temp = 0;
    while (tid < N) {
        temp += a[tid] * b[tid];
        tid += blockDim.x * gridDim.x; 
    }
    
    cache[cacheIndex] = temp; 
    
    __syncthreads(); 
    
    int i = blockDim.x/2;
    while (i != 0 ) {
        if (cacheIndex < i)
            cache[cacheIndex] += cache[cacheIndex + i];
        __syncthreads();
        i /= 2;
    }
    
    if (cacheIndex == 0)
        c[blockIdx.x] = cache[0];
        
}

/*
// float version of atomic add
__device__  float atomicAddf(float* address, float val) {
   float old = *address, assumed;

   do { assumed = old;
        old = __int_as_float( 
            atomicCAS((unsigned int*)address,
            __float_as_int(assumed),
            __float_as_int(val + assumed)) );
   } while (assumed != old);

   return old;
} 


__global__ void dot2( float *a, float *b, float *c, int N ) {
    __shared__ float temp[threadsPerBlock];
    int index = threadIdx.x + blockIdx.x * blockDim.x;
    temp[threadIdx.x] = a[index] * b[index];
    //c[0] = 0; 
    __syncthreads();
    if( 0 == threadIdx.x ) {
        float sum = 0;
        for( int i = 0; i < N; i++ )
            sum += temp[i];
            atomicAddf( c , sum );
    }
}
*/

// A specialized matrix_row * vector multiply 
// Returns the multiplication of row in A times x
__device__ float MatVecMultiply(float *A, float *x, int N, int row) {
    float sum = 0;
    for (int i=0; i<N; i++) 
        sum += A[N*row+i] * x[i];
    return sum; 
}

// Projected Gauss-Seidel
// For a matrix A, vectors b, and x0, all of length N, iterate  
__global__ void pGS( float *A, float *b, float *x0, float *x1, int N ) {
    int tid = threadIdx.x + blockIdx.x * blockDim.x;
    float LAMBDA = 0.707;   // LAMBDA is supposed to be 1<=L<=2 but I think that's for positive(b)? 
                            // I wonder if for this method, LAMBDA should be dynamic with the size N 
    float r0, r1;
    
    // First iteration
    r0 = b[tid] + MatVecMultiply(A, x0, N, tid);
    //x1[tid] = max(0, x0[tid] - ri / A[N*tid+tid]);  // Projection  
    x1[tid] = x0[tid] - LAMBDA*r0 / A[N*tid+tid];  // Successive Over-Relaxation (SOR), no projection
    __syncthreads();  
    
    for (int iters=0; iters<100; iters++) {
        r1 = b[tid] + MatVecMultiply(A, x1, N, tid); 
        x0[tid] = x1[tid] - LAMBDA*r1 / A[N*tid+tid];  // Successive Over-Relaxation (SOR), no projection
        __syncthreads();  
        
        if (tid == 0) {
            printf("(%d) r = %f   : ",iters, r1);
            for (int i=0; i<N; i++) 
                printf("%f, \t",x0[i]);
            printf("\n"); 
        }
        __syncthreads(); 
        
        r0 = b[tid] + MatVecMultiply(A, x0, N, tid); 
        x1[tid] = x0[tid] - LAMBDA*r0 / A[N*tid+tid];  // Successive Over-Relaxation (SOR), no projection
        __syncthreads();  
        
        if (tid == 0)
            printf("   (%d) residuals = %f, %f\n",iters,r1, r0);
            
        if (abs(r0) < 0.001)
            break; 
    }
    
    
}


// Main
int main() {

    float *x1, *a, c, *partial_c;
    float *dev_A, *dev_x0, *dev_x1, *dev_a, *dev_b, *dev_partial_c;
    
    //A = (float*)malloc( N*N*sizeof(float) );
    x1 = (float*)malloc( N*sizeof(float) );
    a = (float*)malloc( N*sizeof(float) );
    //b = (float*)malloc( N*sizeof(float) );
    
    partial_c = (float*)malloc( blocksPerGrid*sizeof(float) );
    
    cudaMalloc( (void**) &dev_A, N*N*sizeof(float) );
    cudaMalloc( (void**) &dev_x0, N*sizeof(float) );
    cudaMalloc( (void**) &dev_x1, N*sizeof(float) );
    cudaMalloc( (void**) &dev_a, N*sizeof(float) );
    cudaMalloc( (void**) &dev_b, N*sizeof(float) );
    cudaMalloc( (void**) &dev_partial_c, blocksPerGrid*sizeof(float) );
    
    // Test case
    float A[100] ={ 38.2883,   2.2700,    3.2359,    3.6405,    1.7018,    2.6147,    2.6089,    3.5436,    3.2964,    2.7359,
                    2.2700,   22.3140,    2.4651,    2.3220,    1.2521,    2.2821,    1.8419,    2.4564,    2.1646,    2.4552,
                    3.2359,    2.4651,   46.1522,    3.9220,    2.2500,    3.8552,    3.2061,    4.0181,    3.4000,    4.2448,
                    3.6405,    2.3220,    3.9220,   52.2668,    2.3026,    3.2100,    3.0781,    4.1019,    3.9669,    3.8830,
                    1.7018,    1.2521,    2.2500,    2.3026,   13.5953,    1.8844,    1.6628,    2.2282,    2.0793,    2.3255,
                    2.6147,    2.2821,    3.8552,    3.2100,    1.8844,   36.4673,    2.5989,    3.7060,    2.5412,    3.7806,
                    2.6089,    1.8419,    3.2061,    3.0781,    1.6628,    2.5989,   25.4442,    3.1507,    2.5339,    2.7645,
                    3.5436,    2.4564,    4.0181,    4.1019,    2.2282,    3.7060,    3.1507,   46.4464,    3.2219,    3.8510,
                    3.2964,    2.1646,    3.4000,    3.9669,    2.0793,    2.5412,    2.5339,    3.2219,   43.1067,    3.7971,
                    2.7359,    2.4552,    4.2448,    3.8830,    2.3255,    3.7806,    2.7645,    3.8510,    3.7971,   49.5411 };
    float b[10] = { 9.838233, 1.661702, 6.239759, 2.908003, 0.016799, 4.226084, 2.732327, 5.940454, 3.192733, 2.837930  }; 
    for (int i=0; i<N; i++) b[i] = -b[i]; 
    float x0[10] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 }; 
    
    cudaMemcpy( dev_A, A, N*N*sizeof(float), cudaMemcpyHostToDevice ); 
    cudaMemcpy( dev_b, b, N*sizeof(float), cudaMemcpyHostToDevice ); 
    cudaMemcpy( dev_x0, x0, N*sizeof(float), cudaMemcpyHostToDevice ); 
    
    pGS<<<1,N>>>( dev_A, dev_b, dev_x0, dev_x1, N ); 
    
    cudaMemcpy( x1, dev_x1, N*sizeof(float), cudaMemcpyDeviceToHost );
    
    printf("Solution x = ");
    for (int i=0;i<N;i++)
        printf("%f, ",x1[i]);
    printf("\n");
    
    /*
    for (int i=0; i<N; i++) {
        a[i] = i;
        b[i] = i*2; 
    }
    float result = 0;
    for (int i=0; i<N; i++) {
        result += a[i]*b[i];
    }
    printf("Executing %d length dot product on %d blocks per grid with %d threads per block\n",N,blocksPerGrid,threadsPerBlock);
    printf("CPU result: %f\n",result);
    cudaMemcpy( dev_a, a, N*sizeof(float), cudaMemcpyHostToDevice ); 
    cudaMemcpy( dev_b, b, N*sizeof(float), cudaMemcpyHostToDevice );
    dot2<<<blocksPerGrid,threadsPerBlock>>>( dev_a, dev_b, dev_partial_c, N );
    cudaMemcpy( partial_c, dev_partial_c, blocksPerGrid*sizeof(float), cudaMemcpyDeviceToHost ); 
    printf(" The first index: %f\n",partial_c[0]);
    c = 0;
    for (int i=0; i<blocksPerGrid; i++) {
        c += partial_c[i];
    } 
    printf( "The dot product is: %f\n", c );
    */ 
    
    cudaFree( dev_A );
    cudaFree( dev_x0 );
    cudaFree( dev_x1 );
    cudaFree( dev_a );
    cudaFree( dev_b );
    cudaFree( dev_partial_c );
    
    //free( A );
    //free( x0 );
    free( x1 ); 
    free( a );
    //free( b );
    free( partial_c ); 
    
    return 0; 

}

