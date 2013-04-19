

// Includes, system
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#ifdef _WIN32
#  define WINDOWS_LEAN_AND_MEAN
#  define NOMINMAX
#  include <windows.h>
#endif

// OpenGL Graphics includes
#include <GL/glew.h>
#if defined (__APPLE__) || defined(MACOSX)
#include <GLUT/glut.h>
#else
#include <GL/freeglut.h>
#endif

// Includes, cuda
#include <cuda_runtime.h>
#include <cuda_gl_interop.h>

// Utilities and timing functions
#include <helper_functions.h>    // includes cuda.h and cuda_runtime_api.h
#include <timer.h>               // timing functions

// CUDA helper functions
#include <helper_cuda.h>         // helper functions for CUDA error check
#include <helper_cuda_gl.h>      // helper functions for CUDA/GL interop
#include "cuda_utils.cuh"

#include <vector_types.h>

#define MAX_EPSILON_ERROR 10.0f
#define THRESHOLD         0.30f
#define REFRESH_DELAY     10    //ms
#define TIME_STEP         0.001 // s
#define MAX_FORCE         5.0   // N

////////////////////////////////////////////////////////////////////////////////
// Constants
const unsigned int window_width  = 512;
const unsigned int window_height = 512;

const unsigned int mesh_width    = 400;//256;
const unsigned int mesh_height   = 400;//256;

int simulation_step = 0; 
time_t SimTimerStart;
time_t SimTimerStop;  

// VBO variables
GLuint vbo;
struct cudaGraphicsResource *cuda_vbo_resource;
void *d_vbo_buffer = NULL;

float g_fAnim = 0.0;

// Mouse controls
int mouse_old_x, mouse_old_y;
int mouse_buttons = 0;
float rotate_x = 0.0, rotate_y = 0.0;
float translate_z = -3.0;

StopWatchInterface *timer = NULL;
StopWatchInterface *simTimer = NULL; 

// Auto-Verification Code
int fpsCount = 0;        // FPS count for averaging
int fpsLimit = 1;        // FPS limit for sampling
int g_Index = 0;
float avgFPS = 0.0f;
unsigned int frameCount = 0;
unsigned int g_TotalErrors = 0;
bool g_bQAReadback = false;

int *pArgc = NULL;
char **pArgv = NULL;

#define MAX(a,b) ((a > b) ? a : b)
#define MIN(a,b) ((a < b) ? a : b) 

////////////////////////////////////////////////////////////////////////////////
// Declaration, forward
bool runTest(int argc, char **argv, char *ref_file);
void cleanup();

// GL functionality
bool initGL(int *argc, char **argv);
void createVBO(GLuint *vbo, struct cudaGraphicsResource **vbo_res,
               unsigned int vbo_res_flags);
void deleteVBO(GLuint *vbo, struct cudaGraphicsResource *vbo_res);
bool animate = false; 

// Rendering callbacks
void display();
void keyboard(unsigned char key, int x, int y);
void mouse(int button, int state, int x, int y);
void motion(int x, int y);
void timerEvent(int value);

// CUDA functionality
void initCuda(struct cudaGraphicsResource **vbo_resource); 
void runCuda(struct cudaGraphicsResource **vbo_resource);
const int NUM_BLOCKS = 1024;
const int NUM_YBLOCKS = 10; 
const int THREADS_PER_BLOCK = MIN(1024, mesh_width*mesh_height); 
const char *sSDKsample = "CUDA Cloth Example";

// On device global memory for storing body (point) attributes
__device__ float3 point_velocities[mesh_width*mesh_height];
__device__ float3 point_forces[mesh_width*mesh_height]; 
__device__ bool point_is_fixed[mesh_width*mesh_height]; 
float3 *dev_pos; 
float3 *dev_forces;



///////////////////////////////////////////////////////////////////////////////
// Initialize the positions of all the cloth particles.  
///////////////////////////////////////////////////////////////////////////////
__global__ void initialize_particles(float4 *pos, unsigned int width, unsigned int height) {
    unsigned int tid = blockIdx.x*blockDim.x + threadIdx.x;   
    unsigned int y = tid / width; 
    unsigned int x = tid % width; 


    if (tid < width*height) {
        // Calculate uv coordinates (w = 0.0) 
        float u = x / (float) width;
        float v = y / (float) height;
        u = u*((float)width)/((float)width-1);
        v = v*((float)height)/((float)height-1);

        // Set vertex position
        pos[tid] = make_float4(u, 0.0f, v, 1.0f);
        
        // Initialize velocity
        point_velocities[tid] = make_float3(0.0, 0.0, 0.0); 
        
        // Fix the top corners
        //if ((x==0 || x==width-1) && y==height-1) 
        if (x==0 && y==0 || x==0 && y==mesh_height-1 || x==mesh_width-1 && y==0 || x==mesh_width-1 && y==mesh_height-1)
            point_is_fixed[tid] = true; 
        else
            point_is_fixed[tid] = false; 
    }
}


// Given indices p1 and p2, calculates the 
__device__ void update_forces(float4 *pos, int p1, int p2, float l0, float k) {

    //assert(p1 > 0 && p1 < 5*5);
    //assert(p2 > 0 && p2 < 5*5);   NOTE! This broke the crap out of this
    float4 P1 = pos[p1]; 
    float4 P2 = pos[p2];
    float3 L = make_float3( (P2.x-P1.x),        // Vector from p1 to p2
                            (P2.y-P1.y),
                            (P2.z-P1.z)  ); 
    //float l0 = 0.1;  // TODO: this should not be hard-coded 
    float dist = sqrt( L.x*L.x +            // Distance between points
                       L.y*L.y +
                       L.z*L.z   );
    float3 Fint = make_float3( k * (L.x - (l0/dist)*L.x),
                               k * (L.y - (l0/dist)*L.y),
                               k * (L.z - (l0/dist)*L.z)  );  
    
    atomicAddf(&point_forces[p1].x, Fint.x); 
    atomicAddf(&point_forces[p1].y, Fint.y);
    atomicAddf(&point_forces[p1].z, Fint.z);
    
    atomicAddf(&point_forces[p2].x, -Fint.x); 
    atomicAddf(&point_forces[p2].y, -Fint.y);
    atomicAddf(&point_forces[p2].z, -Fint.z);
    
    //printf("Updating %d and %d.  Dist: %f\n",p1,p2,dist);
}


// Here, we manually move particles back from constraint violation
// Assumes that p1 is not static. 
__device__ float4 getCorrection(float4 *pos, unsigned int p1, unsigned int p2, float l0) {
    float4 correction = make_float4(0.0, 0.0, 0.0, 0.0); 
    if (point_is_fixed[p1] && point_is_fixed[p2])
        return correction;
    float4 P1 = pos[p1]; 
    float4 P2 = pos[p2];
    float4 n = make_float4( (P2.x-P1.x),        // Vector from p1 to p2
                            (P2.y-P1.y),
                            (P2.z-P1.z), 0.0 ); 
  
    float l1 = length(n);                // Distance between points
    normalize(n); 
    
    float deltaP = l1 - (1.1) * l0;
    if (deltaP > 0) {
        if (point_is_fixed[p2])
            correction += deltaP * n;
        else
            correction += (deltaP/2.0) * n;
    }
    return correction; 
}

///////////////////////////////////////////////////////////////////////////////
// Simple kernel to modify vertex positions of the particles
///////////////////////////////////////////////////////////////////////////////
#define K_STRUCTURAL 5.0
#define K_SHEAR 2.0
#define K_BEND 2.0
#define DAMPING 0.05
__global__ void update_particles(float4 *pos, unsigned int width, unsigned int height, float time, float3 *dev_pos, float3 *dev_forces)
{
    unsigned int tid = blockIdx.x*blockDim.x + threadIdx.x;     // Thread ID    
    unsigned int y = tid / width;                               // x and y grid coordinates
    unsigned int x = tid % width; 

    if (tid < width*height) {       // Asserts that the thread points to a valid index
        // Initialize force as zero + gravity
        point_forces[tid] = make_float3(0.0, 0.0, 0.0); 
       
        __syncthreads();  // Don't allow changes until all forces have been reset
                
        // Update force for each neighbor particle
        // Structural springs
        float l0 = 1.0/(width-1);  
        
        if (x < width-1)                    // Structural springs
            update_forces( pos, tid, tid+1, l0, K_STRUCTURAL );         // 1-left
        if (y < height-1)
            update_forces( pos, tid, tid+width, l0, K_STRUCTURAL );     // 1-above
        if (x < width-1 && y < height-1)    // Shear springs X
            update_forces( pos, tid, tid+width+1, 1.41421356237*l0, K_SHEAR);  // up-left
        if (x < width-1 && y > 0)
            update_forces( pos, tid, tid-width+1, 1.41421356237*l0, K_SHEAR);  // down-left
        if (x < width-2)                    // Flexion springs + (2 away)
            update_forces( pos, tid, tid+2, 2*l0, K_BEND);              // 2-left
        if (y < height-2) 
            update_forces( pos, tid, tid+2*width, 2*l0, K_BEND);        // 2-above
        
        __syncthreads(); // Don't allow integration until forces are completely updated
        
        if (!point_is_fixed[tid]) {
            // Limit the maximum force on each particle
            point_forces[tid].x = MIN(point_forces[tid].x, MAX_FORCE);
            point_forces[tid].x = MAX(point_forces[tid].x, -MAX_FORCE);  
            point_forces[tid].y = MIN(point_forces[tid].y, MAX_FORCE);  
            point_forces[tid].y = MAX(point_forces[tid].y, -MAX_FORCE); 
            point_forces[tid].z = MIN(point_forces[tid].z, MAX_FORCE); 
            point_forces[tid].z = MAX(point_forces[tid].z, -MAX_FORCE); 
        
            // Kinematic update (include damping) 
            // This doesn't need to be atomic since we sync
            float mass = 0.1 / mesh_width*mesh_height; 
            atomicAddf(&point_forces[tid].x, mass *  0.0 - DAMPING * point_velocities[tid].x);
            atomicAddf(&point_forces[tid].y, mass * -9.8 - DAMPING * point_velocities[tid].y);
            atomicAddf(&point_forces[tid].z, mass *  0.0 - DAMPING * point_velocities[tid].z);    
            
            point_velocities[tid].x += (point_forces[tid].x/mass) * TIME_STEP;  
            point_velocities[tid].y += (point_forces[tid].y/mass) * TIME_STEP;
            point_velocities[tid].z += (point_forces[tid].z/mass) * TIME_STEP;
                                          
            // Integrate velocity over time step
            pos[tid].x += point_velocities[tid].x * TIME_STEP;
            pos[tid].y += point_velocities[tid].y * TIME_STEP;
            pos[tid].z += point_velocities[tid].z * TIME_STEP;    
        }
        
        __syncthreads();  // Interesting question, what would happen if this was inside the if() above
            
        for (int constraint_iter=0; constraint_iter<MAX(width,6); constraint_iter++) {
            
            float4 correction = make_float4(0.0, 0.0, 0.0, 0.0); 
            // Determine correction from constraint violation
            if (!point_is_fixed[tid]) {
                // Structural springs +
                if (x < width-1)                    
                    correction += getCorrection( pos, tid, tid+1, l0 );              // 1-left
                if (x > 0)                    
                    correction += getCorrection( pos, tid, tid-1, l0 );              // 1-right
                if (y < height-1)
                    correction += getCorrection( pos, tid, tid+width, l0 );          // 1-above
                if (y > 0)
                    correction += getCorrection( pos, tid, tid-width, l0 );          // 1-below
                // Shear springs X    
                if (x < width-1 && y < height-1)    
                    correction += getCorrection( pos, tid, tid+width+1, 1.41421356237*l0 ); // up-left   
                if (x > 0 && y < height-1) 
                    correction += getCorrection( pos, tid, tid+width-1, 1.41421356237*l0 ); // up-right
                if (x < width-1 && y > 0)
                    correction += getCorrection( pos, tid, tid-width+1, 1.41421356237*l0 ); // down-left
                if (x > 0 && y > 0)
                    correction += getCorrection( pos, tid, tid-width-1, 1.41421356237*l0 ); // down-right
            }
            
            __syncthreads(); 
            
            // Update position from correction
            pos[tid] += 0.95*correction; 
            
            __syncthreads(); 
            
            //printf("[%d] : (%f, %f, %f, %f)\n",tid,correction.x, correction.y, correction.z, correction.w);
        
        } // End of constraint correction iterations
        
        
        // Debug info
        dev_pos[tid].x = pos[tid].x;  //  TODO: this is for debugging
        dev_pos[tid].y = pos[tid].y;
        dev_pos[tid].z = pos[tid].z; 
        
        dev_forces[tid].x = point_forces[tid].x;
        dev_forces[tid].y = point_forces[tid].y;
        dev_forces[tid].z = point_forces[tid].z;
                 
    }
}


// Execute the kernel
void launch_kernel(float4 *pos, unsigned int mesh_width, unsigned int mesh_height, float time)
{    
    dim3 block(NUM_BLOCKS, THREADS_PER_BLOCK, 1);
    dim3 grid(max(1,mesh_width / block.x), max(1,mesh_height / block.y), 1);
    //update_particles<<<grid,block>>>(pos, mesh_width, mesh_height, time);
    update_particles<<<NUM_BLOCKS,THREADS_PER_BLOCK>>>(pos, mesh_width, mesh_height, time, dev_pos, dev_forces);
    
    //printf("Time step: %d\n",simulation_step);
    if (simulation_step == 0) {
        printf("Starting simulation stopwatch...\n");
        sdkCreateTimer(&simTimer); 
        sdkStartTimer(&simTimer);
    }
    else if (simulation_step == 1000) {
        sdkStopTimer(&simTimer); 
        float seconds = sdkGetTimerValue(&simTimer) / 1000.0;
        printf("Time taken for first 1000 simulation steps of %d particles: %f seconds.\n", mesh_width*mesh_height, seconds);
    }
    simulation_step++; 
}



////////////////////////////////////////////////////////////////////////////////
// Program main
////////////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv)
{
    // TODO
    cudaMalloc( (void**) &dev_pos, mesh_width*mesh_height*sizeof(float3) );
    cudaMalloc( (void**) &dev_forces, mesh_width*mesh_height*sizeof(float3) ); 

    char *ref_file = NULL;

    pArgc = &argc;
    pArgv = argv;

    printf("%s starting...\n", sSDKsample);

    runTest(argc, argv, ref_file);

    cudaDeviceReset();
    printf("%s completed, returned %s\n", sSDKsample, (g_TotalErrors == 0) ? "OK" : "ERROR!");
    exit(g_TotalErrors == 0 ? EXIT_SUCCESS : EXIT_FAILURE);
}

void computeFPS()
{
    frameCount++;
    fpsCount++;

    if (fpsCount == fpsLimit)
    {
        avgFPS = 1.f / (sdkGetAverageTimerValue(&timer) / 1000.f);
        fpsCount = 0;
        fpsLimit = (int)MAX(avgFPS, 1.f);

        sdkResetTimer(&timer);
    }

    char fps[256];
    sprintf(fps, "Cuda GL Interop (VBO): %3.1f fps (Max 100Hz)", avgFPS);
    glutSetWindowTitle(fps);
}

////////////////////////////////////////////////////////////////////////////////
//! Initialize GL
////////////////////////////////////////////////////////////////////////////////
bool initGL(int *argc, char **argv)
{
    glutInit(argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
    glutInitWindowSize(window_width, window_height);
    glutCreateWindow("Cuda GL Interop (VBO)");
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutMotionFunc(motion);
    glutTimerFunc(REFRESH_DELAY, timerEvent,0);

    // initialize necessary OpenGL extensions
    glewInit();

    if (! glewIsSupported("GL_VERSION_2_0 "))
    {
        fprintf(stderr, "ERROR: Support for necessary OpenGL extensions missing.");
        fflush(stderr);
        return false;
    }

    // default initialization
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glDisable(GL_DEPTH_TEST);

    // viewport
    glViewport(0, 0, window_width, window_height);

    // projection
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (GLfloat)window_width / (GLfloat) window_height, 0.1, 100.0);

    SDK_CHECK_ERROR_GL();

    return true;
}


////////////////////////////////////////////////////////////////////////////////
//! Run a simple test for CUDA
////////////////////////////////////////////////////////////////////////////////
bool runTest(int argc, char **argv, char *ref_file)
{
    // Create the CUTIL timer
    sdkCreateTimer(&timer);

    // First initialize OpenGL context, so we can properly set the GL for CUDA.
    // This is necessary in order to achieve optimal performance with OpenGL/CUDA interop.
    initGL(&argc, argv);

    // register callbacks
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);

    // create VBO
    createVBO(&vbo, &cuda_vbo_resource, cudaGraphicsMapFlagsWriteDiscard);

    // run the cuda part
    printf("Run the cuda part\n"); 
    initCuda(&cuda_vbo_resource); 
    runCuda(&cuda_vbo_resource);

    // start rendering mainloop
    glutMainLoop();
    atexit(cleanup);
   

    return true;
}

////////////////////////////////////////////////////////////////////////////////
// INIT the Cuda part of the computation
////////////////////////////////////////////////////////////////////////////////
void initCuda(struct cudaGraphicsResource **vbo_resource)
{
    // map OpenGL buffer object for writing from CUDA
    float4 *dptr;
    cudaGraphicsMapResources(1, vbo_resource, 0);
    size_t num_bytes;
    cudaGraphicsResourceGetMappedPointer((void **)&dptr, &num_bytes, *vbo_resource);
    printf("CUDA mapped VBO: May access %ld bytes\n", num_bytes);
    
    //dim3 block(NUM_BLOCKS, THREADS_PER_BLOCK, 1);
    //dim3 grid(max(1,mesh_width / block.x), max(1,mesh_height / block.y), 1);
    initialize_particles<<<NUM_BLOCKS,THREADS_PER_BLOCK>>>(dptr, mesh_width, mesh_height);
        
    launch_kernel(dptr, mesh_width, mesh_height, g_fAnim);
    // unmap buffer object
    checkCudaErrors(cudaGraphicsUnmapResources(1, vbo_resource, 0));
}


////////////////////////////////////////////////////////////////////////////////
//! Run the Cuda part of the computation
////////////////////////////////////////////////////////////////////////////////
void runCuda(struct cudaGraphicsResource **vbo_resource)
{
    if (animate) {
        // map OpenGL buffer object for writing from CUDA
        float4 *dptr;
        cudaGraphicsMapResources(1, vbo_resource, 0);
        size_t num_bytes;
        cudaGraphicsResourceGetMappedPointer((void **)&dptr, &num_bytes, *vbo_resource);
        //printf("CUDA mapped VBO: May access %ld bytes\n", num_bytes);
            
        launch_kernel(dptr, mesh_width, mesh_height, g_fAnim);

        // unmap buffer object
        checkCudaErrors(cudaGraphicsUnmapResources(1, vbo_resource, 0));
        // The reason we map and unmap every time is for synchronicity 
    }
}


////////////////////////////////////////////////////////////////////////////////
//! Create VBO
////////////////////////////////////////////////////////////////////////////////
void createVBO(GLuint *vbo, struct cudaGraphicsResource **vbo_res,
               unsigned int vbo_res_flags)
{
    assert(vbo);

    // create buffer object
    glGenBuffers(1, vbo);
    glBindBuffer(GL_ARRAY_BUFFER, *vbo);

    // initialize buffer object
    unsigned int size = mesh_width * mesh_height * 4 * sizeof(float);
    glBufferData(GL_ARRAY_BUFFER, size, 0, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // register this buffer object with CUDA
    cudaGraphicsGLRegisterBuffer(vbo_res, *vbo, vbo_res_flags);

    SDK_CHECK_ERROR_GL();
}

////////////////////////////////////////////////////////////////////////////////
//! Delete VBO
////////////////////////////////////////////////////////////////////////////////
void deleteVBO(GLuint *vbo, struct cudaGraphicsResource *vbo_res)
{

    // unregister this buffer object with CUDA
    cudaGraphicsUnregisterResource(vbo_res);

    glBindBuffer(1, *vbo);
    glDeleteBuffers(1, vbo);

    *vbo = 0;
}

////////////////////////////////////////////////////////////////////////////////
//! Display callback
////////////////////////////////////////////////////////////////////////////////
void display()
{
    sdkStartTimer(&timer);

    // run CUDA kernel to generate vertex positions
    runCuda(&cuda_vbo_resource);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // set view matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0.0, 0.0, translate_z);
    glRotatef(rotate_x, 1.0, 0.0, 0.0);
    glRotatef(rotate_y, 0.0, 1.0, 0.0);

    // render from the vbo
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexPointer(4, GL_FLOAT, 0, 0);

    // Draw points
    glPointSize(1.0); 
    glEnableClientState(GL_VERTEX_ARRAY);
        glColor3f(0.3, 0.3, 0.4);
        glDrawArrays(GL_POINTS, 0, mesh_width * mesh_height);
        //glDrawArrays(GL_TRIANGLES, 0, 300); 
    glDisableClientState(GL_VERTEX_ARRAY);
    
    // Draw axes
    // X axis
    glLineWidth(1.0); 
    glColor3f(1.0, 0.0, 0.0);
    glBegin(GL_LINES);
        glVertex3f(0.0, 0.0, 0.0);
        glVertex3f(1.0, 0.0, 0.0);
    glEnd();
    // Z axis
    glColor3f(0.0, 1.0, 0.0);
    glBegin(GL_LINES);
        glVertex3f(0.0, 0.0, 0.0);
        glVertex3f(0.0, 0.0, 1.0);
    glEnd();
    
    /*
    // Draw forces (REALLY INEFFICIENTLY)
    float3 host_pos[mesh_width*mesh_height];
    float3 host_force[mesh_width*mesh_height]; 
    
    cudaMemcpy( host_pos, dev_pos, mesh_width*mesh_height*sizeof(float3), cudaMemcpyDeviceToHost); 
    cudaMemcpy( host_force, dev_forces, mesh_width*mesh_height*sizeof(float3), cudaMemcpyDeviceToHost); 
    cudaMemcpy( host_force, point_forces, mesh_width*mesh_height*sizeof(float3), cudaMemcpyDeviceToHost);  
    
    glColor3f(0.0, 1.0, 0.0);
    float forceScale = 0.2; 
    for (int i=0; i<mesh_width*mesh_height; i++) {
        float3 p = host_pos[i];
        float3 f = forceScale*host_force[i];
        glBegin(GL_LINES);
            glVertex3f(p.x, p.y, p.z);
            glVertex3f(p.x+f.x, p.y+f.y, p.z+f.z);
        glEnd();
    }*/
    
    
    glutSwapBuffers();

    if (animate)
        g_fAnim += 0.01f;

    sdkStopTimer(&timer);
    computeFPS();
}

void timerEvent(int value)
{
    glutPostRedisplay();
    glutTimerFunc(REFRESH_DELAY, timerEvent,0);
}

void cleanup()
{
    sdkDeleteTimer(&timer);

    if (vbo)
    {
        deleteVBO(&vbo, cuda_vbo_resource);
    }
}


////////////////////////////////////////////////////////////////////////////////
//! Keyboard events handler
////////////////////////////////////////////////////////////////////////////////
void keyboard(unsigned char key, int /*x*/, int /*y*/)
{
    switch (key)
    {
        case (27) :
            exit(EXIT_SUCCESS);
            break;
        case (113) :
            exit(EXIT_SUCCESS);
            break;
        case (32) : 
            animate = !animate; 
            break;
    }
}

////////////////////////////////////////////////////////////////////////////////
//! Mouse event handlers
////////////////////////////////////////////////////////////////////////////////
void mouse(int button, int state, int x, int y)
{
    if (state == GLUT_DOWN)
    {
        mouse_buttons |= 1<<button;
    }
    else if (state == GLUT_UP)
    {
        mouse_buttons = 0;
    }

    mouse_old_x = x;
    mouse_old_y = y;
}

void motion(int x, int y)
{
    float dx, dy;
    dx = (float)(x - mouse_old_x);
    dy = (float)(y - mouse_old_y);

    if (mouse_buttons & 1)
    {
        rotate_x += dy * 0.2f;
        rotate_y += dx * 0.2f;
    }
    else if (mouse_buttons & 4)
    {
        translate_z += dy * 0.01f;
    }

    mouse_old_x = x;
    mouse_old_y = y;
}



