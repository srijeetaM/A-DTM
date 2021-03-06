// System includes
#include <stdio.h> 
#include <stdlib.h>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cctype>
#include <vector>
#include <tuple>
#include <string>
#include <algorithm>
#include <cmath> 
#include <queue>
#include <map>
#include <unistd.h>
#include <ctime>
#include <time.h>
#include <thread> 
#include <pthread.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/wait.h>
#include<sys/resource.h>
#include <sys/types.h>
#include <dirent.h>
#include <string>
#include <fstream>
#include <iostream>
#include <streambuf>
#include <mutex>  
#include <condition_variable>

#define PERSISTENCE_DEBUG if(0)

// OpenCL includes
#define CL_USE_DEPRECATED_OPENCL_1_2_APIS
// #define CL_USE_DEPRECATED_OPENCL_1_1_APIS
#include <CL/cl.h>
// #undef CL_VERSION_1_2
// #include <CL/cl.hpp>
#include <CL/cl_ext.h>
// Custom includes
#include "string.h"
#define PROFILE_ITERATIONS 1

/**************************** Global Variables ****************************************/
unsigned long long int BUFFER_TIME=1000;
unsigned int MIN_IDLE_SLOT=100000;
int EVENTS_ENABLE;
int microkernel_start=0;
int profilekernel_start=0;
int STR_LENGTH; 
int NumOfJobs;
int NumCoresPerDevice;
int NumofCPUs;
int NumOfSensors;
int NumOfTempBand;
int PLATFORM_GPU;
int PLATFORM_CPU;
int G_BIG;
int G_LITTLE;
int C_BIG;
int TempInterval;
int POLE;
int CONTROLLER_MODE;
int numOfHyperperiod;
int LOG_LEVEL;
int LOG_SCHEDULER;
int LOG_PROFILE;
int micro_kernel_device;
int refresh_data_per_hyperperiod;
int READ_INPUT_DATA;
int time_buffer;
int FACTOR;
int SAFE;
int isProfileMode;
int LOCAL_RECOVERY_ENABLE;
int GLOBAL_RECOVERY_ENABLE;
int monitorTemp;
int generatePlot;
int numOfDAGs=0;
int taskcount=0;
int cur_hyper_period=0;
int dags_arrived;
int raceToIdle;
int THERMAL_THRESHOLD_H;
int THERMAL_THRESHOLD_L;
int CREATE_SUB_KERNEL;
int FREQUENCY_MODE;
int MIGRATION_MODE;
int TASK_SHIFT_MODE;
int no_micro_kernel=0;
int num_profile_kernels=0; // required for profiling scripts
int BIG_CORE=0;
int rblock;
int nlock;
int devlock;
int nKernels;
int nTasks;
int SchedulerFinish;
int stop_scheduler;
unsigned long long int safe_duration=0;
unsigned long long int notify_cb_buffer_c=0;
unsigned long long int notify_cb_buffer_g=0; 
unsigned long long int START_TIME;
unsigned long long int hyper_period;
std::mutex mtx_rblock;
std::mutex mtx_nlock;
std::mutex mtx_devlock;
std::mutex mtx_kernel_finished;
std::mutex mtx_p_kernels_size;
std::mutex bigCore_lock;
std::mutex run_kernel_lock;
std::mutex run_scheduler_lock;
std::string outputbuffer;
bool processed = false;
bool bigCoreCheck = false;
std::condition_variable cv;
std::condition_variable cv1;
float utilisation_threshold=1.5;
unsigned int flops_threshold=16384;



/**************************** Helper functions ****************************************/

//OpenCL Error Logs

void print_profile_event_status(cl_int status)
{
    if(status==CL_SUCCESS)
        printf("CL_SUCCESS\n");
    else if(status==CL_PROFILING_INFO_NOT_AVAILABLE)
        printf("CL_PROFILING_INFO_NOT_AVAILABLE \n");
    else if(status==CL_INVALID_VALUE)
        printf("CL_INVALID_VALUE \n");
    else if(status==CL_INVALID_EVENT)
        printf("CL_INVALID_EVENT \n");
    else if(status==CL_OUT_OF_RESOURCES)
        printf("CL_OUT_OF_RESOURCES \n");
    else if(status==CL_OUT_OF_HOST_MEMORY)
        printf("CL_OUT_OF_HOST_MEMORY \n");
}

const char *getErrorString(cl_int error)
{
switch(error){
    // run-time and JIT compiler errors
    case 0: return "CL_SUCCESS";
    case -1: return "CL_DEVICE_NOT_FOUND";
    case -2: return "CL_DEVICE_NOT_AVAILABLE";
    case -3: return "CL_COMPILER_NOT_AVAILABLE";
    case -4: return "CL_MEM_OBJECT_ALLOCATION_FAILURE";
    case -5: return "CL_OUT_OF_RESOURCES";
    case -6: return "CL_OUT_OF_HOST_MEMORY";
    case -7: return "CL_PROFILING_INFO_NOT_AVAILABLE";
    case -8: return "CL_MEM_COPY_OVERLAP";
    case -9: return "CL_IMAGE_FORMAT_MISMATCH";
    case -10: return "CL_IMAGE_FORMAT_NOT_SUPPORTED";
    case -11: return "CL_BUILD_PROGRAM_FAILURE";
    case -12: return "CL_MAP_FAILURE";
    case -13: return "CL_MISALIGNED_SUB_BUFFER_OFFSET";
    case -14: return "CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST";
    case -15: return "CL_COMPILE_PROGRAM_FAILURE";
    case -16: return "CL_LINKER_NOT_AVAILABLE";
    case -17: return "CL_LINK_PROGRAM_FAILURE";
    case -18: return "CL_DEVICE_PARTITION_FAILED";
    case -19: return "CL_KERNEL_ARG_INFO_NOT_AVAILABLE";

    // compile-time errors
    case -30: return "CL_INVALID_VALUE";
    case -31: return "CL_INVALID_DEVICE_TYPE";
    case -32: return "CL_INVALID_PLATFORM";
    case -33: return "CL_INVALID_DEVICE";
    case -34: return "CL_INVALID_CONTEXT";
    case -35: return "CL_INVALID_QUEUE_PROPERTIES";
    case -36: return "CL_INVALID_COMMAND_QUEUE";
    case -37: return "CL_INVALID_HOST_PTR";
    case -38: return "CL_INVALID_MEM_OBJECT";
    case -39: return "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR";
    case -40: return "CL_INVALID_IMAGE_SIZE";
    case -41: return "CL_INVALID_SAMPLER";
    case -42: return "CL_INVALID_BINARY";
    case -43: return "CL_INVALID_BUILD_OPTIONS";
    case -44: return "CL_INVALID_PROGRAM";
    case -45: return "CL_INVALID_PROGRAM_EXECUTABLE";
    case -46: return "CL_INVALID_KERNEL_NAME";
    case -47: return "CL_INVALID_KERNEL_DEFINITION";
    case -48: return "CL_INVALID_KERNEL";
    case -49: return "CL_INVALID_ARG_INDEX";
    case -50: return "CL_INVALID_ARG_VALUE";
    case -51: return "CL_INVALID_ARG_SIZE";
    case -52: return "CL_INVALID_KERNEL_ARGS";
    case -53: return "CL_INVALID_WORK_DIMENSION";
    case -54: return "CL_INVALID_WORK_GROUP_SIZE";
    case -55: return "CL_INVALID_WORK_ITEM_SIZE";
    case -56: return "CL_INVALID_GLOBAL_OFFSET";
    case -57: return "CL_INVALID_EVENT_WAIT_LIST";
    case -58: return "CL_INVALID_EVENT";
    case -59: return "CL_INVALID_OPERATION";
    case -60: return "CL_INVALID_GL_OBJECT";
    case -61: return "CL_INVALID_BUFFER_SIZE";
    case -62: return "CL_INVALID_MIP_LEVEL";
    case -63: return "CL_INVALID_GLOBAL_WORK_SIZE";
    case -64: return "CL_INVALID_PROPERTY";
    case -65: return "CL_INVALID_IMAGE_DESCRIPTOR";
    case -66: return "CL_INVALID_COMPILER_OPTIONS";
    case -67: return "CL_INVALID_LINKER_OPTIONS";
    case -68: return "CL_INVALID_DEVICE_PARTITION_COUNT";

    // extension errors
    case -1000: return "CL_INVALID_GL_SHAREGROUP_REFERENCE_KHR";
    case -1001: return "CL_PLATFORM_NOT_FOUND_KHR";
    case -1002: return "CL_INVALID_D3D10_DEVICE_KHR";
    case -1003: return "CL_INVALID_D3D10_RESOURCE_KHR";
    case -1004: return "CL_D3D10_RESOURCE_ALREADY_ACQUIRED_KHR";
    case -1005: return "CL_D3D10_RESOURCE_NOT_ACQUIRED_KHR";
    default: return "Unknown OpenCL error";
    }
}
void check(cl_int status, const char* str) {
    if(status != CL_SUCCESS) {
        // fprintf(fp,"Failed : %s. Error %d\n", str, status);
        printf("\nFailed status %s , Error %d-> %s\n", str,status,getErrorString(status));
        exit(EXIT_FAILURE);
    }
}

//FILE IO

inline bool file_exists(const std::string& name) 
{
  struct stat buffer;   
  return (stat (name.c_str(), &buffer) == 0); 
}

int write_file(const char *name, const unsigned char *content, size_t size) 
{
  FILE *fp = fopen(name, "wb+");
  if (!fp) 
    return -1;
  
  fwrite(content, size, 1, fp);
  fclose(fp);
  return 0;
}

int read_file(unsigned char **output, size_t *size, const char *name) 
{
  FILE* fp = fopen(name, "rb");
  if (!fp) 
    return -1;
  

  fseek(fp, 0, SEEK_END);
  *size = ftell(fp);
  fseek(fp, 0, SEEK_SET);

  *output = (unsigned char *)malloc(*size);
  if (!*output) 
  {
    fclose(fp);
    return -1;
  }

  fread(*output, *size, 1, fp);
  fclose(fp);
  return 0;
}



cl_int write_binaries(std::string filename,cl_program program, unsigned num_devices,
                      cl_uint platform_idx) {
  unsigned i;
  cl_int err = CL_SUCCESS;
  size_t *binaries_size = NULL;
  unsigned char **binaries_ptr = NULL;
  printf("Generating binaries for %d devices of platform %d\n",num_devices,platform_idx);
  // Read the binaries size
  size_t binaries_size_alloc_size = sizeof(size_t) * num_devices;
  binaries_size = (size_t *)malloc(binaries_size_alloc_size);
  if (!binaries_size) {
    err = CL_OUT_OF_HOST_MEMORY;
   
  }

  err = clGetProgramInfo(program, CL_PROGRAM_BINARY_SIZES,
                         binaries_size_alloc_size, binaries_size, NULL);
  if (err != CL_SUCCESS) {
    
  }

  // Read the binaries
  size_t binaries_ptr_alloc_size = sizeof(unsigned char *) * num_devices;
  binaries_ptr = (unsigned char **)malloc(binaries_ptr_alloc_size);
  if (!binaries_ptr) {
    err = CL_OUT_OF_HOST_MEMORY;
    
  }
  memset(binaries_ptr, 0, binaries_ptr_alloc_size);
  for (i = 0; i < num_devices; ++i) {
    binaries_ptr[i] = (unsigned char *)malloc(binaries_size[i]);
    if (!binaries_ptr[i]) {
      err = CL_OUT_OF_HOST_MEMORY;
      
    }
  }

  err = clGetProgramInfo(program, CL_PROGRAM_BINARIES, binaries_ptr_alloc_size,
                         binaries_ptr, NULL);


  // Write the binaries to file
  for (i = 0; i < num_devices; ++i) {
    // Create output file name
 
    // Write the binary to the output file
    write_file(filename.c_str(), binaries_ptr[i], binaries_size[i]);
  }

  return err;
}


/**************************** Helper functions ****************************************/

typedef struct _jobinfo
{  
    int jobID;
    int numOfNodes;
    int numOfEdges;
    std::vector <std::vector <int>> dependency;
    std::vector <std::vector <int>> successors;
}JobInfo;


typedef struct _daginfo
{  
    int globalDAGID;
    int jobID;
    int instanceID;
    
}DAGInfo;

typedef struct _kernelinfo
{   
    std::string KernelName;
    std::string kernelSource;
    unsigned int noInputBuffers;
    unsigned int noOutputBuffers;
    unsigned int noIOBuffers;
    std::vector <unsigned int> bufferInputID;
    std::vector <unsigned int> bufferOutputID;  
    cl_kernel kernelObject;
    std::vector<cl_kernel> kernelObjects;
    int workDimension;
    size_t globalWorkSize[3];
    size_t localWorkSize[3];
    // Data Type, Size, Position,Persistence
    std::vector < std::tuple <std::string, unsigned int, unsigned int, unsigned int> > inputBuffers;
    std::vector < std::tuple <std::string, unsigned int, unsigned int, unsigned int> > outputBuffers;
    std::vector < std::tuple <std::string, unsigned int, unsigned int, unsigned int> > ioBuffers;
    std::vector < std::tuple <std::string, unsigned int, unsigned int> > varArguments;
    std::vector < std::tuple <std::string, unsigned int, unsigned int> > localArguments;
    std::vector < unsigned long long int> nonPartition;  
    std::vector < std::tuple <unsigned int, unsigned int, unsigned int> >data_outflow;
    std::string options;
    std::vector <float> localSizeFactor;
    unsigned long long int chunkSizeTiming;
    std::map<int,int> configured;
    unsigned int FLOPS_per_thread; 

    

} KernelInfo;


typedef struct _taskinfo
{    
    std::string taskID;
    int traceID;        
    std::vector < int> nodes;
    std::vector <int> dependency;
    std::vector <KernelInfo *>kernels;
    DAGInfo * dagInfo;
    unsigned int task_size;
    int partition_count;
    float exTime;
    unsigned long long int arrival;
    unsigned long long int deadline; 
    std::vector<unsigned long long int> WCET;
    std::vector <float> basespeed;
    //float performance_goal;
    std::vector<void*>  data;
    int isTerminal;
    int majorViolationPerHP;
    int totalMajorViolation;
    int global_recovery_mode;

} TaskInfo;


typedef struct _event
{
    cl_event ev;
    std::string type;

    _event(cl_event e,std::string t)
    {
        ev = e;
        type = t;
    }
 
    unsigned long long int get_timestamp(std::string command)
    {
        unsigned long long int timestamp;
        cl_int status;
        if(command == "queued")
        {
            status = clGetEventProfilingInfo(this->ev, CL_PROFILING_COMMAND_QUEUED, sizeof(cl_ulong), &timestamp, NULL);
            // print_profile_event_status(status);
            check(status, "Getting event command queued profiling info");
        }
        else if(command == "submit")
        {
            status = clGetEventProfilingInfo(this->ev, CL_PROFILING_COMMAND_SUBMIT, sizeof(cl_ulong), &timestamp, NULL);
            // print_profile_event_status(status);
            check(status, "Getting event command submit profiling info");
        }
        else if(command == "start")
        {
            status = clGetEventProfilingInfo(this->ev, CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &timestamp, NULL);
            // print_profile_event_status(status);
            check(status, "Getting event command start profiling info");
        }
        else if (command == "end")
        {
            status = clGetEventProfilingInfo(this->ev, CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &timestamp, NULL);
            // print_profile_event_status(status);
            check(status, "Getting event command end profiling info");
        }
        else
        {
            printf("No such event %s exists",command.c_str());
            exit(EXIT_FAILURE);
        }
        return timestamp;
    }

    unsigned long long int get_queue_submit()
    {
        return ( this->get_timestamp("submit") - this->get_timestamp("queued"))/1000;

    }

    unsigned long long int get_submit_start()
    {
        return (this->get_timestamp("start") - this->get_timestamp("submit"))/1000;

    }

    unsigned long long int get_start_end()
    {
        return (this->get_timestamp("end") - this->get_timestamp("start"))/1000;

    }

    std::string dump_time()
    {
       std::stringstream ss;
       ss << this->type <<":"<< this->get_queue_submit() <<";"<< this->get_submit_start()<< ";"<<this->get_start_end()<<";";
       return ss.str();

    }

    void print()
    {
        if(this->type == "write")
            printf("\033[0;32m");
        else if(this->type == "exec")
            printf("\033[1;31m");
        else if(this->type == "read")
            printf("\033[0;34m");
        printf("----------------------------------------------------\n");
        printf("%s Queued -> Submit: %llu\n",this->type.c_str(),this->get_queue_submit());
        printf("%s Submit -> Start: %llu\n",this->type.c_str(),this->get_submit_start());
        printf("%s Start -> End: %llu\n",this->type.c_str(),this->get_start_end());
        printf("----------------------------------------------------\n");
        printf("\033[0m"); 
    }

    void dump()
    {
        if(this->type == "write")
            printf("\033[0;32m");
        else if(this->type == "exec")
            printf("\033[1;31m");
        else if(this->type == "read")
            printf("\033[0;34m");
        printf("%s Queued %llu, ",this->type.c_str(),this->get_timestamp("queued"));
        printf("Submitted %llu, ",this->get_timestamp("submit"));
        printf("Started %llu, ",this->get_timestamp("start"));
        printf("Ended %llu\n",this->get_timestamp("end"));
        printf("\033[0m"); 
    }
} Event;


typedef struct _kernel_events
{
    std::vector<cl_event> write;
    cl_event barrier_write;
    cl_event exec;
    std::vector<cl_event> read;
    cl_event barrier_read;
    int is_profiled;

    std::string dump_times()
    {
        std::string time_info = "";
        for(unsigned int i=0;i<write.size();i++)
        {
            
            std::string type = "write";
            Event *e = new Event(write[i],type);
            time_info += e->dump_time();
        }
        PERSISTENCE_DEBUG cout << "completed write events\n";
        std::string type = "execute";
        
        Event *e = new Event(exec,type);
        time_info += e->dump_time();
        PERSISTENCE_DEBUG cout << "completed nd events\n";
        for(unsigned int i=0;i<read.size();i++)
        {
            std::string type = "read";
            Event *e = new Event(read[i],type);
            time_info += e->dump_time();
            PERSISTENCE_DEBUG cout <<"Read " <<i<<" done\n";
        }
        PERSISTENCE_DEBUG cout << "completed read events\n";
        return time_info;    
    }
    void print()
    {
        for(unsigned int i=0;i<write.size();i++)
        {
            std::string type = "write";
            Event *e = new Event(write[i],type);
            e->print();
        }
        std::string type = "execute";
        Event *e = new Event(exec,type);
        e->print();
        for(unsigned int i=0;i<read.size();i++)
        {
            std::string type = "read";
            Event *e = new Event(read[i],type);
            e->print();
        }


    }

    void dump()
    {
        for(unsigned int i=0;i<write.size();i++)
        {
            std::string type = "write";
            Event *e = new Event(write[i],type);
            e->dump();
        }
        std::string type = "execute";
        Event *e = new Event(exec,type);
        e->dump();
        for(unsigned int i=0;i<read.size();i++)
        {
            std::string type = "read";
            Event *e = new Event(read[i],type);
            e->dump();
        }


    }

} KernelEvents;

typedef struct _kernelexecutioninfo{  
    
    unsigned long long int  devStartTime;
    unsigned long long int  devEndTime;
    unsigned long long int  devTotalTime; 
    unsigned long long int  write_buffers_start;
    unsigned long long int  rel_start_time; //dispatch start Rtime 
    unsigned long long int  rel_end_time; //notify_callback start Rtime, read end Rtime 
    unsigned long long int  notify_callback_rel_start_time;
    unsigned long long int  notify_callback_rel_end_time;
    unsigned long long int  turnaroundTime; // diff bw rel_end_time and rel_start_time
    unsigned long long int  dispatchTime; // time difference between dispatch start and dispatch end

    unsigned long long int  writeQueued;
    unsigned long long int  writeSubmit;
    unsigned long long int  writeStart;
    unsigned long long int  writeEnd;
    unsigned long long int  writeTime;


    unsigned long long int  ndQueued;
    unsigned long long int  ndSubmit;
    unsigned long long int  ndStart;
    unsigned long long int  ndEnd;
    unsigned long long int  ndTime;


    unsigned long long int  readQueued; 
    unsigned long long int  readSubmit;   
    unsigned long long int  readStart;
    unsigned long long int  readEnd;
    unsigned long long int  readTime; 

    unsigned long long int  write_start_h;
    unsigned long long int  write_start;
    unsigned long long int  write_end;
    unsigned long long int  write_time;
    unsigned long long int  nd_start_h;
    unsigned long long int  nd_start;
    unsigned long long int  nd_end;
    unsigned long long int  nd_time;
    unsigned long long int  read_start_h;
    unsigned long long int  read_start;
    unsigned long long int  read_end;
    unsigned long long int  read_time;

    unsigned long long int frequency_change_time;
    unsigned long long int get_image_start_time;
    unsigned long long int get_image_end_time;
    

} KernelExecutionInfo;

typedef struct _kernellaunchinfo{
    TaskInfo *task;
    int kernel_index;
    KernelEvents ke;
    KernelExecutionInfo kex;
    std::vector<cl_mem> io;
    int platform_pos;
    int device_pos;
    int device_index;
    unsigned int size;
    unsigned int offset;
    int queued;  
    int reset; 
    int released_b; 
    int released_d;
    unsigned int frequency;
    unsigned int tmp_frequency;
    unsigned int tmp_idle_slot;
    int tmp_platform_pos;
    int tmp_device_pos;
    int control_mode;
    int priority;
    unsigned long long int start;
    float expected_speed;
    // float measured_speed;
    float last_speedup;
    int finished = 0;    
    int recovery_mode;
    unsigned int FLOPS_processed;
    unsigned int not_done_count;
    struct _kernellaunchinfo* full_task_klinfo;
} KernelLaunchInfo;

typedef struct _DAGtime
{
    KernelLaunchInfo* klinfo;
    // int platform;
    // int device;
    unsigned long long int startTime;
    unsigned long long int finishTime;
    unsigned long long int arrivalTime;
    unsigned long long int writeStart;
    unsigned long long int ndStart;
    unsigned long long int readStart;
    unsigned long long int deadline;
    int arrivalHyperperiod;
    float turnaroundTime;
    float makespan;
    float lateness;
    int safe_mode;
    int deadlineViolated;

}DAGTime;


typedef struct _devConfig{
    
    unsigned int frequency;
    float speedup;
    float powerup;
    
} DeviceConfig;

typedef struct _deviceSpecification{
    
    std::vector<DeviceConfig*> device_config;    
    unsigned int lowFrequencyBound; //= frequencies[0];
    unsigned int highFrequencyBound;
    unsigned int midFrequency; //= frequencies[frequencies.size() - 1]; 
} DeviceSpecification;

typedef struct _temperature{
    
    std::vector<int> sensors;
    int cpuB_0;
    int cpuB_1;
    int cpuB_2;
    int cpuB_3;
    int gpu_4;
    int max;
    int avg;
    int maxC;
    int avgC;
    unsigned int frequency_C_b;
    unsigned int frequency_C_l;
    unsigned int frequency_G;

} Temperature;

typedef struct _interval{
    unsigned long long int start;
    unsigned long long int end;
    unsigned long long int intervalWidth;
}Interval;


bool is_dev_available();
unsigned long long int convert_to_relative_time( unsigned long long int t,unsigned long long int ref);
void dump_profile_event_timing(KernelExecutionInfo kex);
void dump_execution_time_statistics(KernelLaunchInfo *kl,int dag, int task,std::ofstream &ofs);
void dump_profile_statistics(KernelLaunchInfo *kl,std::ofstream &ofs);
void dump_total_time(KernelLaunchInfo *kl,std::ofstream &ofs);
void create_micro_kernel(int platform, int device);
void strip_ext(char *fname);
std::string getFileName(std::string filePath, bool withExtension, char seperator);
void generate_plot_data(int traceCount);
unsigned int get_buffer_size(KernelInfo* suc_kl,int suc_pos);
std::string get_buffer_type(KernelInfo* suc_kl,int suc_pos);
void trasfer_data_to_ipbuf(KernelLaunchInfo* cur_kl, KernelLaunchInfo* suc_kl,int cur_pos,int suc_pos);
void print_task_map();
void print_job_map();
pair <int, int> dag_to_job_id(int dag);
int job_to_dag_id(int job, int inst);
void populate_global_values(std::map<std::string,std::string>);
void load_config(std::string filename);
void find_max_idle_slot(Interval* intvl_gap,std::vector<std::vector<KernelLaunchInfo*>> &taskList,std::vector<Interval*> &idle_slots);
void choose_task_list(std::vector<std::vector<std::vector<DAGTime>>>  &d_timeMatrix,Interval* global_intvl,std::pair<int, int> max_pair,std::vector<std::vector<std::vector<KernelLaunchInfo*>>> &taskList);
void convert_time_matrix(std::vector<std::vector<DAGTime>> &l_timeMatrix,std::vector<std::vector<std::vector<DAGTime>>> &d_timeMatrix);
void get_idle_slot_list(std::vector<std::vector<DAGTime>> &l_timeMatrix,Interval* global_intvl,std::pair<int,int> max_pair, std::vector<std::vector<Interval*>> &idleSlots);
void shift_task(KernelLaunchInfo* kl);
int find_nearest_config(float s_up, int platform,int device);
int local_controller(KernelLaunchInfo* kl);
float calculate_speed(KernelLaunchInfo* klinfo);
// void choose_task_list(std::vector<std::vector<DAGTime>> &l_timeMatrix,std::vector<Interval*> &global_intvl,std::pair<int, int> max_pair,std::vector<KernelLaunchInfo*> &taskList);
void choose_task_list(std::vector<std::vector<DAGTime>> &l_timeMatrix,Interval* global_intvl,std::pair<int, int> max_pair,std::vector<KernelLaunchInfo*> &taskList);
void get_global_interval(std::vector <std::vector<std::vector<Interval*>>> &intervals_per_tband,std::pair<int, int> max_pair,std::vector<Interval*> &global_intvl);
std::pair<int, int> get_max_band(std::vector <std::vector<std::vector<Interval*>>> &intervals_per_tband);
void get_intervals_per_band(std::map<unsigned long long int,Temperature> &temperatureMap,std::vector <std::vector<std::vector<Interval*>>> &intervals_per_tband);
void choose_intervals();
void reset_launch_info();
void print_DAGTime();
void print_DAGTime(std::vector<std::vector<DAGTime>> &l_dagtime_matrix );
void print_tempMap();
void print_temperature(Temperature* tmp);
void print_temperature(Temperature* tmp);
void monitor_temperature();
void CL_CALLBACK notify_callback_print (cl_event event, cl_int event_command_exec_status, void *user_data);
void childrenToReadyBuffer(KernelLaunchInfo& klinfo);
void build_kernel_object(const char* filename);
void build_all_kernel_objects(const char* directory);
void read_dag_structure(const char* directory);
void *taskToReadyB(void *vargp);
const char* parse_file_name(const char* filename);
std::map<int,int> read_dag_file(const char* filename);
void populate_task_queue();
void initialise_nodes_matrix();
int task_dev_queue_empty();
int check_dependancy(std::vector <string> deps,int dag);
void create_output_file(const char* op_file);
int read_trace_file(const char* filename);
void *run_scheduler(void *vargp);
void *run_kernel(void *vargp);
void run_scheduler(int taskcount);
int chunk_factor(KernelLaunchInfo* kl);
unsigned long long int get_current_time();
int parse_trace_input(int* index);
void add_task_to_queue(char *ip);
void modify_dag_task_data();
void print_launch_info(KernelLaunchInfo& launchinfo);
void change_frequency(unsigned int frequency, int platform_pos, int device_pos);
void dispatch_from_queue();
int get_dev_index(int platform,int device);
void get_device_specification(char *filename);
//std::vector<DeviceSpecification> get_device_specification(char *filename);
void copy_launch_info(KernelLaunchInfo& srcinfo,KernelLaunchInfo& dstinfo);
void get_all_devices() ;
cl_uint get_sub_devices(cl_device_id device_id,std::vector< std::vector<cl_device_id> >* all_devices)  ;
void print_device_info(cl_device_id device_id,int i,int j);
void print_all_device_info(std::vector<std::vector<cl_device_id>>& all_devices);
void check(cl_int status, const char* str);
void host_initialize(std::vector<std::vector<cl_device_id>>& all_devices, std::vector<cl_context>& ctxs, std::vector< std::vector<cl_command_queue> >& cmd_qs);
std::vector<cl_command_queue> create_command_queue_for_each(cl_device_id *devs, int num_devs, cl_context ctx);
std::vector<cl_program> build_kernel_from_info(KernelInfo& ki, const char *info_file_name, std::vector<std::vector<cl_device_id>>& all_devices, std::vector<cl_context>& ctxs);
std::vector<cl_program> build_kernel(KernelInfo& ki,  std::vector<std::vector<cl_device_id>>& all_devices, std::vector<cl_context>& ctxs);
cl_program cl_compile_program(const char* kernel_file_name, cl_context ctx,int platform_type);
KernelInfo* assign_kernel_info(const char * info_file_name);
void profile_events(KernelLaunchInfo& kl_info);
int store_variable_kernel_arg(std::string type, std::string value);
void host_array_initialize(KernelInfo& ki, std::vector<void*>& data);
void reset_nonpersistent_host_arrays(KernelInfo& ki, std::vector<void*>& data);
void array_randomize(void* data, std::string type, int size);
void array_input(void* data, std::string type, int size,void* input);
void output_initialize(void* data, std::string type, int size);
void* array_allocate(std::string type, int size);
size_t get_sizeof(std::string str);
//void dispatch(KernelLaunchInfo &kl_info,cl_context ctx, cl_command_queue cmd_q, KernelInfo& ki, std::vector<void*>& data, cl_device_id device_id);
cl_event dispatch(KernelLaunchInfo &kl_info);
std::vector<std::pair<int, int>> ready_buffer_available();
bool is_rbuffer_filled();
void update_and_release (KernelLaunchInfo *kl);
unsigned int calculate_ip_buffer_size(unsigned int size,int ip_index,KernelInfo& ki);
unsigned int calculate_ip_buffer_offest(unsigned int dataoffset,int ip_index,KernelInfo& ki);
unsigned int calculate_op_buffer_size(unsigned int size,int op_index,KernelInfo& ki);
unsigned int calculate_op_buffer_offset(unsigned int dataoffset,int op_index,KernelInfo& ki);
void cl_create_buffers(cl_context& ctx, KernelInfo& ki, std::vector<cl_mem>& io, std::vector<void*>& data, unsigned int size,unsigned int dataoffset,int dtype);
void cl_set_kernel_args(KernelInfo& ki, std::vector<cl_mem>& io, int object,unsigned int datasize) ;
std::vector<cl_event> cl_enqueue_write_buffers(KernelExecutionInfo *di , cl_command_queue cmd_q, KernelInfo& ki, std::vector<cl_mem>& io, std::vector<void*>& data, unsigned int size,unsigned int dataoffset, cl_event dep, int dtype);
std::vector<cl_event> cl_enqueue_read_buffers(KernelExecutionInfo *di,cl_command_queue cmd_q, KernelInfo& ki, std::vector<cl_mem>& io, std::vector<void*>& data,unsigned int size,unsigned int dataoffset, cl_event dep, int dtype);
cl_event cl_enqueue_nd_range_kernel(KernelExecutionInfo *di,cl_command_queue cmd_q, KernelInfo& ki, int object,unsigned int size, cl_event dep);
void* get_global_pointer(std::string str, int index);
void CL_CALLBACK notify_callback_update_release (cl_event event, cl_int event_command_exec_status, void *user_data);
void CL_CALLBACK notify_callback_eventstatus (cl_event event, cl_int event_command_exec_status, void *user_data);
void CL_CALLBACK notify_callback_buffer_free (cl_event event, cl_int event_command_exec_status, void *user_data);
void CL_CALLBACK notify_callback_update_exinfo (cl_event event, cl_int event_command_exec_status, void *user_data);
int compare_and_swap(int *word,int testval,int newval);
int test_and_set(int *rqlock,int testval, int newval);
unsigned long long int  gcd(unsigned long long int  a, unsigned long long int  b);
unsigned long long int findlcm(unsigned long long int arr[], int n) ;
int local_recovery(std::pair<int, int> t_violation, KernelLaunchInfo* klinfo);
int find_local_recovery_mode(std::pair<int, int> t_violation, KernelLaunchInfo* next_klinfo);
int global_recovery(std::pair<int, int>,KernelLaunchInfo* klinfo);
int enter_global_recovery_mode(std::vector<KernelLaunchInfo*> kl_vector_v, std::vector<float> utilisation,std::vector<unsigned int> FLOPS_processed);
unsigned long long int get_local_deadline(KernelLaunchInfo* klinfo);
int get_frequency_index(int platform, unsigned int frequency);
unsigned long long int estimate_WCET(unsigned long long int wcet_atmin_freq,int freq_index,int platform);
std::vector<unsigned int> get_partioined_WCET(KernelLaunchInfo* klinfo);
unsigned int get_current_CPU_frequency(int d);
unsigned int get_current_GPU_frequency();
void dump_output(KernelLaunchInfo* klinfo);

void *take_input( void *ptr );
void printEventStatus(cl_event ev);
void printEventStatus(cl_int status, cl_event ev);
// void print_profile_event_status(cl_int status);
void host_synchronize(std::vector< std::vector<cl_command_queue>>& cmd_qs);
void release_host_arrays(std:: vector <void*> &data);
void release_buffers( std::vector<cl_mem>& buffers);
void release_kernel_events(KernelEvents& k);
void release_programs( std::vector<cl_program> all_programs );
void release_everything(std::vector<cl_context>& ctxs, std::vector< std::vector<cl_command_queue> >& cmd_qs);
void reset();
/// Global vectors to store variable kernel argument values.
std::vector<int> gint;
std::vector<uint> guint;
std::vector<short> gshort;
std::vector<ushort> gushort;
std::vector<long> glong;
std::vector<ulong> gulong;
std::vector<bool> gbool;
std::vector<float> gfloat;
std::vector<double> gdouble;
std::vector<char> gchar;
