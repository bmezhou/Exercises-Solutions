#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#pragma warning( disable : 4996 )
#include <CL/cl.h>

#define UNIX

enum accessMode { MAPPED, DIRECT };
enum memoryMode { PAGEABLE, PINNED };

int runTest(cl_device_id clSelectedDeviceID, cl_uint deviceCount);

void testBandwidth(cl_context cxGPUContext, cl_device_id clSelectedDeviceID);

double deviceToHost(cl_context cxGPUContext, cl_device_id clSelectedDeviceID, cl_command_queue cqCommandQueue, accessMode accMode, memoryMode memMode);

void oclCheckError(int ciErrNum, cl_int errResult);
double shrDeltaT(int iCounterID );


int main ()
{
    int ciErrNum;
    char chBuffer[1024];
    // printf("Hello \n");
    
    cl_platform_id clSelectedPlatformID = NULL;
    cl_device_id clSelectedDeviceID = NULL;
    cl_uint num_platforms; 

    // 
    // cl_int ciErrNum = oclGetPlatformID (&clSelectedPlatformID);
    ciErrNum = clGetPlatformIDs (0, NULL, &num_platforms);
    if (ciErrNum != CL_SUCCESS || num_platforms == 0)
    {
        return -1;
    }
    else
    {
        printf("The number of OpenCL platform(s) is/are %d\n", num_platforms);
        cl_platform_id* clPlatformIDs;
        clPlatformIDs = (cl_platform_id *)malloc(sizeof(cl_platform_id) * num_platforms);
        ciErrNum = clGetPlatformIDs (num_platforms, clPlatformIDs, NULL);

        // Use the first platform.

        clSelectedPlatformID = clPlatformIDs[0];

        ciErrNum =  clGetPlatformInfo( clSelectedPlatformID, CL_PLATFORM_NAME, 1024, &chBuffer, NULL );

        printf("%s\n", chBuffer);
    }

    //
    cl_uint ciDeviceCount;
    ciErrNum = clGetDeviceIDs (clSelectedPlatformID, CL_DEVICE_TYPE_ALL, 0, NULL, &ciDeviceCount);
    if (ciErrNum != CL_SUCCESS)
    {
        printf(" Error %i in clGetDeviceIDs call !!!\n\n", ciErrNum);
        return ciErrNum;
    }
    else if (ciDeviceCount == 0)
    {
        printf(" There are no devices supporting OpenCL (return code %i)\n\n", ciErrNum);
        return ciErrNum;
    } 
    else
    {
        printf("The number of available devices is/are %d\n", ciDeviceCount);
        
        cl_device_id* device_id = (cl_device_id*)malloc(sizeof(cl_device_id) * ciDeviceCount);
        ciErrNum = clGetDeviceIDs(clSelectedPlatformID, CL_DEVICE_TYPE_ALL, ciDeviceCount, device_id, NULL);

        clSelectedDeviceID = device_id[0];
        
    }

    
    cl_char device_name[1024] = {0};

    ciErrNum = clGetDeviceInfo(clSelectedDeviceID, CL_DEVICE_NAME, sizeof(device_name), &device_name, NULL);

    if (ciErrNum != CL_SUCCESS)
    {
        return -1;
    }
    else{
        printf("\nDevice name: %s\n", device_name);
        ciErrNum = clGetDeviceInfo(clSelectedDeviceID, CL_DEVICE_OPENCL_C_VERSION, sizeof(device_name), &device_name, NULL);
        printf("OCL version: %s\n", device_name);
    }

    int retVal = runTest(clSelectedDeviceID, ciDeviceCount);

    return 0;
}

int runTest(cl_device_id clSelectedDeviceID, cl_uint deviceCount)
{
    cl_context cxGPUContext = clCreateContext(0, deviceCount, &clSelectedDeviceID, NULL, NULL, NULL);
    
    if (cxGPUContext == (cl_context)0)
    {
        printf("Context error\n");
        return -11;
    }

    testBandwidth(cxGPUContext, clSelectedDeviceID);
    

    clReleaseContext(cxGPUContext);
    return 0;
}

void testBandwidth(cl_context cxGPUContext, cl_device_id clSelectedDeviceID)
{
    cl_command_queue cqCommandQueue = clCreateCommandQueue(cxGPUContext, clSelectedDeviceID, CL_QUEUE_PROFILING_ENABLE, NULL);
    // cl_command_queue cqCommandQueue = clCreateCommandQueueWithProperties(cxGPUContext, clSelectedDeviceID, CL_QUEUE_PROPERTIES, NULL);

    // Compute
    // enum accessMode { MAPPED, DIRECT };
    // enum memoryMode { PAGEABLE, PINNED };
    double timeComsumed = deviceToHost( cxGPUContext,  clSelectedDeviceID, cqCommandQueue, DIRECT, PINNED);

    printf("%f\n", timeComsumed);

    return;
}

double deviceToHost(cl_context cxGPUContext, cl_device_id clSelectedDeviceID, cl_command_queue cqCommandQueue, accessMode accMode, memoryMode memMode)
{
    double elapsedTimeInSec = 0.0;
    double bandwidthInMBs = 0.0;
    unsigned char *h_data = NULL;
    cl_mem cmPinnedData = NULL;
    cl_mem cmDevData = NULL;
    cl_int ciErrNum = CL_SUCCESS;
    
    unsigned int MEMCOPY_ITERATIONS = 5;
    unsigned int memSize = 1024 * 1024 * 256;

    //allocate and init host memory, pinned or conventional
    if(memMode == PINNED)
    {
        // Create a host buffer
        cmPinnedData = clCreateBuffer(cxGPUContext, CL_MEM_READ_WRITE | CL_MEM_ALLOC_HOST_PTR, memSize, NULL, &ciErrNum);
        oclCheckError(ciErrNum, CL_SUCCESS);

        // Get a mapped pointer
        h_data = (unsigned char*)clEnqueueMapBuffer(cqCommandQueue, cmPinnedData, CL_TRUE, CL_MAP_WRITE, 0, memSize, 0, NULL, NULL, &ciErrNum);
        oclCheckError(ciErrNum, CL_SUCCESS);

        //initialize 
        for(unsigned int i = 0; i < memSize/sizeof(unsigned char); i++)
        {
            h_data[i] = (unsigned char)(i & 0xff);
        }

        // unmap and make data in the host buffer valid
        ciErrNum = clEnqueueUnmapMemObject(cqCommandQueue, cmPinnedData, (void*)h_data, 0, NULL, NULL);
        oclCheckError(ciErrNum, CL_SUCCESS);
    }
    else 
    {
        // standard host alloc
        h_data = (unsigned char *)malloc(memSize);

        //initialize 
        for(unsigned int i = 0; i < memSize/sizeof(unsigned char); i++)
        {
            h_data[i] = (unsigned char)(i & 0xff);
        }
    }

    // allocate device memory 
    cmDevData = clCreateBuffer(cxGPUContext, CL_MEM_READ_WRITE, memSize, NULL, &ciErrNum);
    oclCheckError(ciErrNum, CL_SUCCESS);

    // initialize device memory 
    if(memMode == PINNED)
    {
	    // Get a mapped pointer
        h_data = (unsigned char*)clEnqueueMapBuffer(cqCommandQueue, cmPinnedData, CL_TRUE, CL_MAP_WRITE, 0, memSize, 0, NULL, NULL, &ciErrNum);	        

        ciErrNum = clEnqueueWriteBuffer(cqCommandQueue, cmDevData, CL_FALSE, 0, memSize, h_data, 0, NULL, NULL);
        oclCheckError(ciErrNum, CL_SUCCESS);
    }
    else
    {
        ciErrNum = clEnqueueWriteBuffer(cqCommandQueue, cmDevData, CL_FALSE, 0, memSize, h_data, 0, NULL, NULL);
        oclCheckError(ciErrNum, CL_SUCCESS);
    }
    oclCheckError(ciErrNum, CL_SUCCESS);
    
    // ciErrNum = clFinish(cqCommandQueue);

    // Overwrite host buffer.
    for (unsigned int i = 0; i < memSize; i ++)
    {
        h_data[i] = 0; //(unsigned char)(i & 0xff);
    }

    // Sync queue to host, start timer 0, and copy data from GPU to Host
    ciErrNum = clFinish(cqCommandQueue);
    shrDeltaT(0);

    if(accMode == DIRECT)
    { 
        // DIRECT:  API access to device buffer 
        for(unsigned int i = 0; i < MEMCOPY_ITERATIONS; i++)
        {
            ciErrNum = clEnqueueReadBuffer(cqCommandQueue, cmDevData, CL_FALSE, 0, memSize, h_data, 0, NULL, NULL);
            oclCheckError(ciErrNum, CL_SUCCESS);
        }
        ciErrNum = clFinish(cqCommandQueue);
        oclCheckError(ciErrNum, CL_SUCCESS);
    } 
    else 
    {
        // MAPPED: mapped pointers to device buffer for conventional pointer access
        void* dm_idata = clEnqueueMapBuffer(cqCommandQueue, cmDevData, CL_TRUE, CL_MAP_WRITE, 0, memSize, 0, NULL, NULL, &ciErrNum);
        oclCheckError(ciErrNum, CL_SUCCESS);
        for(unsigned int i = 0; i < MEMCOPY_ITERATIONS; i++)
        {
            memcpy(h_data, dm_idata, memSize);
        }
        ciErrNum = clEnqueueUnmapMemObject(cqCommandQueue, cmDevData, dm_idata, 0, NULL, NULL);
        oclCheckError(ciErrNum, CL_SUCCESS);
    }
    
    //get the the elapsed time in seconds
    elapsedTimeInSec = shrDeltaT(0);
    
    //calculate bandwidth in MB/s
    bandwidthInMBs = ((double)memSize * (double)MEMCOPY_ITERATIONS) / (elapsedTimeInSec * (double)(1 << 20));

    // Check

    int correct = 0;
    for (unsigned int i = 0; i < memSize; i ++)
    {
        unsigned char tmp = (unsigned char)(i & 0xff) - h_data[i];
        correct += tmp;
    }

    printf("Correct check: %d\n", correct);


    //clean up memory
    if(cmDevData)clReleaseMemObject(cmDevData);
    if(cmPinnedData) 
    {
	    clEnqueueUnmapMemObject(cqCommandQueue, cmPinnedData, (void*)h_data, 0, NULL, NULL);	
	    clReleaseMemObject(cmPinnedData);	
    }
    h_data = NULL;

    return bandwidthInMBs;
}


void oclCheckError(int ciErrNum, cl_int errResult)
{
    if (ciErrNum != errResult)
    {
        exit(ciErrNum);
    }
}

double shrDeltaT(int iCounterID = 0)
{
    // local var for computation of microseconds since last call
    double DeltaT;

    #ifdef _WIN32 // Windows version of precision host timer

        // Variables that need to retain state between calls
        static LARGE_INTEGER liOldCount[3] = { {0, 0}, {0, 0}, {0, 0} };

        // locals for new count, new freq and new time delta 
	    LARGE_INTEGER liNewCount, liFreq;
	    if (QueryPerformanceFrequency(&liFreq))
	    {
		    // Get new counter reading
		    QueryPerformanceCounter(&liNewCount);

		    if (iCounterID >= 0 && iCounterID <= 2) 
		    {
			    // Calculate time difference for timer 0.  (zero when called the first time) 
			    DeltaT = liOldCount[iCounterID].LowPart ? (((double)liNewCount.QuadPart - (double)liOldCount[iCounterID].QuadPart) / (double)liFreq.QuadPart) : 0.0;
			    // Reset old count to new
			    liOldCount[iCounterID] = liNewCount;
			}
			else 
			{
		        // Requested counter ID out of range
		        DeltaT = -9999.0;
			}
			
		    // Returns time difference in seconds sunce the last call
		    return DeltaT;
	    }
	    else
	    {
		    // No high resolution performance counter
		    return -9999.0;
	    }
    #elif defined(UNIX) // Linux version of precision host timer. See http://www.informit.com/articles/article.aspx?p=23618&seqNum=8
        static struct timeval _NewTime;  // new wall clock time (struct representation in seconds and microseconds)
        static struct timeval _OldTime[3]; // old wall clock timers 0, 1, 2 (struct representation in seconds and microseconds)

        // Get new counter reading
        gettimeofday(&_NewTime, NULL);

		if (iCounterID >= 0 && iCounterID <= 2) 
		{
		    // Calculate time difference for timer (iCounterID).  (zero when called the first time) 
		    DeltaT =  ((double)_NewTime.tv_sec + 1.0e-6 * (double)_NewTime.tv_usec) - ((double)_OldTime[iCounterID].tv_sec + 1.0e-6 * (double)_OldTime[iCounterID].tv_usec);
		    // Reset old timer (iCounterID) to new timer
		    _OldTime[iCounterID].tv_sec  = _NewTime.tv_sec;
		    _OldTime[iCounterID].tv_usec = _NewTime.tv_usec;
		}
		else 
		{
	        // Requested counterID is out of rangewith respect to available counters
	        DeltaT = -9999.0;
		}

	    // Returns time difference in seconds sunce the last call
	    return DeltaT;

	#elif defined (__APPLE__) || defined (MACOSX)
        static time_t _NewTime;
        static time_t _OldTime[3];

        _NewTime  = clock();

		if (iCounterID >= 0 && iCounterID <= 2) 
		{
		    // Calculate time difference for timer (iCounterID).  (zero when called the first time) 
		    DeltaT = double(_NewTime-_OldTime[iCounterID])/CLOCKS_PER_SEC;

		    // Reset old time (iCounterID) to the new one
		    _OldTime[iCounterID].tv_sec  = _NewTime.tv_sec;
		    _OldTime[iCounterID].tv_usec = _NewTime.tv_usec;
		}
		else 
		{
	        // Requested counter ID out of range
	        DeltaT = -9999.0;
		}
        return DeltaT;
        #else
        printf("shrDeltaT returning early\n");
	#endif
} 