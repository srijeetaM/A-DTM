#include "core.h"
#define NumOfPlatforms 2
using namespace std;
unsigned long long int write_buffers_begin;
//Global data structures
FILE *fp,*t_result,*tmp_result,*m_result,*r_result,*ts_result,*output_data,*o_result;
KernelLaunchInfo* micro_kernel_klinfo;
std::map<int,JobInfo*> jobMap;
std::map<int,DAGInfo*> dagMap;
std::map<unsigned long long int,Temperature*> tempMap;
std::map<pair<int, int>,std::vector <KernelLaunchInfo*>> taskMap;
std::map<std::string,pair<KernelInfo*,KernelInfo*>> kernel_cl_queue;
std::vector<std::vector< int>> nodes_matrix;//If that particular node of each dag has finished
std::vector<pair <int, std::string>> trace_queue;
std::vector<pair<int, int>> global_recovery_tasks;
std::vector <std::vector<std::vector<KernelLaunchInfo* >>> ready_buffer(NumOfPlatforms);
//std::vector<KernelLaunchInfo* > ready_buffer;
std::vector <std::vector<std::vector<KernelLaunchInfo* >>> task_queue(NumOfPlatforms);
std::vector<std::vector<DeviceSpecification*>> deviceSpec(NumOfPlatforms);
std::vector< std::vector<cl_device_id> > all_devices(NumOfPlatforms) ;
std::vector< std::vector<cl_command_queue> > all_cmd_qs(NumOfPlatforms);
std::vector<cl_context> all_ctxs(NumOfPlatforms);
std::vector< std::vector<int>> deviceAvailability(NumOfPlatforms);
std::vector<std::vector<bool>> dev_available(NumOfPlatforms);
std::condition_variable cv_dev_available;
std::condition_variable cv_rbuffer_available;
std::vector<std::vector<DAGTime*>> DAGtimeMatrix;



void load_config(std::string filename)
{
    std::ifstream cFile (filename);
    if (cFile.is_open())
    {
        std::string line;
        while(getline(cFile, line)){
            line.erase(std::remove_if(line.begin(), line.end(), ::isspace), line.end());
            if(line[0] == '#' || line.empty())
                continue;
            auto delimiterPos = line.find("=");
            auto name = line.substr(0, delimiterPos);
            auto value = line.substr(delimiterPos + 1);
            // std::cout << name << " " << value << '\n';
            if(name.compare("STR_LENGTH")==0)
                STR_LENGTH=atoi(value.c_str());
            // else if(name.compare("NumOfPlatforms")==0)
            //     NumOfPlatforms=atoi(value.c_str());
            else if(name.compare("NumOfJobs")==0)
                NumOfJobs=atoi(value.c_str());
            else if(name.compare("NumCoresPerDevice")==0)
                NumCoresPerDevice=atoi(value.c_str());
            else if(name.compare("NumofCPUs")==0)
                NumofCPUs=atoi(value.c_str());
            else if(name.compare("NumOfSensors")==0)
                NumOfSensors=atoi(value.c_str());
            else if(name.compare("NumOfTempBand")==0)
                NumOfTempBand=atoi(value.c_str());
            else if(name.compare("PLATFORM_GPU")==0)
                PLATFORM_GPU=atoi(value.c_str());
            else if(name.compare("PLATFORM_CPU")==0)
                PLATFORM_CPU=atoi(value.c_str());
            else if(name.compare("G_BIG")==0)
                G_BIG=atoi(value.c_str());
            else if(name.compare("G_LITTLE")==0)
                G_LITTLE=atoi(value.c_str());
            // else if(name.compare("NumNodes_0")==0)
            //      NumNodes_0=atoi(value.c_str());
            // else if(name.compare("NumNodes_1")==0)
            //      NumNodes_1=atoi(value.c_str());
            else if(name.compare("TempInterval")==0)
                TempInterval=atoi(value.c_str());
            else if(name.compare("numOfHyperperiod")==0)
                numOfHyperperiod=atoi(value.c_str());
            else if(name.compare("LOG_LEVEL")==0)
                LOG_LEVEL=atoi(value.c_str());
            else if(name.compare("LOG_SCHEDULER")==0)
                LOG_SCHEDULER=atoi(value.c_str());
            else if(name.compare("LOG_PROFILE")==0)
                LOG_PROFILE=atoi(value.c_str());
            else if(name.compare("POLE")==0)
                POLE=atof(value.c_str());
            else if(name.compare("CONTROLLER_MODE")==0)
                CONTROLLER_MODE=atoi(value.c_str());
            // else if(name.compare("micro_kernel_device")==0)
            //     micro_kernel_device=atoi(value.c_str());
            else if(name.compare("time_buffer")==0)
                time_buffer=atoi(value.c_str());
            else if(name.compare("FACTOR")==0)
                FACTOR=atoi(value.c_str());
            else if(name.compare("SAFE_MODE")==0)
                SAFE=atoi(value.c_str());
            else if(name.compare("isProfileMode")==0)
                isProfileMode=atoi(value.c_str());
            else if(name.compare("monitorTemp")==0)
                monitorTemp=atoi(value.c_str());
            else if(name.compare("generatePlot")==0)
                generatePlot=atoi(value.c_str());
            else if(name.compare("RACE_TO_IDLE")==0)
                raceToIdle=atoi(value.c_str());
            else if(name.compare("EVENTS_ENABLE")==0)
                EVENTS_ENABLE = atoi(value.c_str());
            else if(name.compare("LOCAL_RECOVERY_ENABLE")==0)
                LOCAL_RECOVERY_ENABLE = atoi(value.c_str());
            else if(name.compare("GLOBAL_RECOVERY_ENABLE")==0)
                GLOBAL_RECOVERY_ENABLE = atoi(value.c_str());
            else if(name.compare("THERMAL_THRESHOLD_H")==0)
                THERMAL_THRESHOLD_H = atoi(value.c_str());
            else if(name.compare("THERMAL_THRESHOLD_L")==0)
                THERMAL_THRESHOLD_L = atoi(value.c_str());
            else if(name.compare("CREATE_SUB_KERNEL")==0)
                CREATE_SUB_KERNEL = atoi(value.c_str());
            else if(name.compare("refresh_data_per_hyperperiod")==0)
                refresh_data_per_hyperperiod = atoi(value.c_str()); 
            else if(name.compare("READ_INPUT_DATA")==0)
                READ_INPUT_DATA = atoi(value.c_str()); 
            else if(name.compare("FREQUENCY_MODE")==0)
                FREQUENCY_MODE = atoi(value.c_str()); 
            else if(name.compare("MIGRATION_MODE")==0)
                MIGRATION_MODE = atoi(value.c_str()); 
            else if(name.compare("TASK_SHIFT_MODE")==0)
                TASK_SHIFT_MODE = atoi(value.c_str()); 
      
        }        
    }
    else {
        std::cerr << "Couldn't open config file for reading.\n";
    }
    
    // populate_global_values(ans);
}

void read_dag_structure(const char* directory)
{    
    DIR* FD;
    struct dirent* in_file;
    FILE    *entry_file;
    char line[1024];
    
    /* Scanning the in directory */
    if (NULL == (FD = opendir (directory))) 
    {
        fprintf(stderr, "Error : Failed to open dag_structure directory - %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    while ((in_file = readdir(FD))) 
    {
        if (!strcmp (in_file->d_name, "."))
            continue;
        if (!strcmp (in_file->d_name, ".."))    
            continue;
        /* Open directory entry file for common operation */
        /* TODO : change permissions to meet your need! */
        JobInfo * job=new JobInfo();
        int jid=atoi(in_file->d_name);
        job->jobID=jid;
        std::string s = std::to_string(jid);
        char const *pchar = s.c_str();
        char filename[200];
        strcpy (filename,directory);
        strcat (filename,pchar);
        entry_file = fopen(filename, "r+");
        if (entry_file == NULL)
        {
            fprintf(stderr, "Error : Failed to open entry file %s - %s\n",filename, strerror(errno));     
            printf( "Error : Failed to open entry file %s - %s\n",filename, strerror(errno));     
            exit(EXIT_FAILURE);
        }
        //printf("entered read_dag_structure: file read %s - %d  \n",filename,atoi(in_file->d_name));
        
       // printf("jobid %d\n",job->jobID); 
        if(fgets(line, 1024, entry_file) != NULL)
        {
            const char* tok;
            tok = strtok(line, " ");
            job->numOfNodes  = atoi(tok);
            tok = strtok(0, "\n");
            job->numOfEdges  = atoi(tok); 
           // printf("read_dag_structure:1 numOfNodes %d numOfEdges %d \n",job->numOfNodes,job->numOfEdges);           
        }
        
        for(int i=0;i<job->numOfNodes;i++)
        {
            job->dependency.push_back(std::vector<int>());
            job->successors.push_back(std::vector<int>());
        }
       // printf("read_dag_structure:2 numOfNodes %d numOfEdges %d \n",job->numOfNodes,job->numOfEdges);
        
        while (fgets(line, 1024, entry_file) != NULL)
        {
            const char* tok;
            tok = strtok(line, " ");
            int p  = atoi(tok);
            tok = strtok(0, "\n");
            int s  = atoi(tok);
            job->dependency[s].push_back(p);
            job->successors[p].push_back(s);
        }
        // for(int i=0;i<job->numOfNodes;i++)
        // {
        //     printf("\n %d- ",i);
        //     for (int j=0;j<job->successors[i].size();j++)
        //         printf("%d ",job->successors[i][j]);
            
        // }
        jobMap.insert( pair<int, JobInfo*>(jid, job) ); 
        //printf("read_dag_structure: JOB<%d>: %u\n",jid,job);
        fclose(entry_file);
    }   
}

std::map<int,int> read_dag_file(const char* filename)
{
    FILE *file;
    file = fopen(filename, "r");
    printf("entered read_dag_file\n");
    
    if (!file) 
    {
        fprintf(stderr, "Can't open input file: dag_history.\n");
        exit(EXIT_FAILURE);
    }
    
    for (int j = 0; j < NumOfJobs; j++)
    {
        DAGtimeMatrix.push_back(std::vector<DAGTime*>() );
    }
    
    char line[1024];
    unsigned long long int hp;
    std::vector<unsigned long long int> periods;
    int count=0;
    int jobCount=0;
    std::map<int,int> tasks_per_hp;
    int last_hp=0;
    int dags=0;
    while (fgets(line, 1024, file))   
    {  
        // printf("%s\n",line);
        // char* tmp = strdup(line);
        char* tmp = (char*)calloc(strlen(line) + 1 ,1);
        strcpy(tmp, line);
        // printf("tmp: %s\n",tmp);

        DAGTime* dagTime=new DAGTime();
        // printf("%s\n",line);
        const char* tok;
        tok = strtok(line, " ");
        //globalid
        int gdag = atoi(tok);  
        // printf("gdag:%d\n",gdag); 
        tok = strtok(0, " ");  
        //jobid  
        int job = atoi(tok); 
        tok = strtok(0, " ");
        int HP = atoi(tok);
        dagTime->arrivalHyperperiod=HP;
        tok = strtok(0, " ");
        dagTime->arrivalTime=atoi(tok)*1000;
        // printf(" %llu",dagTime->arrivalTime);  
        tok = strtok(0, "\n");
        dagTime->deadline=atoi(tok)*1000;
        dagTime->safe_mode=0;
        dagTime->deadlineViolated=0;

        if(HP==last_hp)
        	dags++;
        if(HP>last_hp)
        {
        	printf("tasks_per_hp: %d %d %d\n",HP,last_hp,dags);
        	tasks_per_hp.insert(pair<int, int>(last_hp, dags)); 
        	for(int h=last_hp+1;h<HP;h++)
        		tasks_per_hp.insert(pair<int, int>(h, dags)); 
        	last_hp=dagTime->arrivalHyperperiod;
        	dags++;
        }

        if(DAGtimeMatrix[job].size()==0)
            jobCount++;

        DAGtimeMatrix[job].push_back(dagTime);
        // printf("\nsize DAGtimeMatrix[i][j] %llu",DAGtimeMatrix[i][DAGtimeMatrix[i].size()-1]->arrivalTime);
        
                
        DAGInfo *dag=new DAGInfo();
        dag->globalDAGID=gdag;
        dag->jobID=job;
        dagMap.insert( pair<int, DAGInfo*>(gdag, dag) );
        if(tmp)
            free(tmp);

        if(jobCount<NumOfJobs)//
            periods.push_back((unsigned long long int)dagTime->deadline-dagTime->arrivalTime);
        
        
        numOfDAGs=gdag+1;
        // printf("numOfDAGs: %d - %d\n",numOfDAGs,gdag);
        count++;
    }
    if(tasks_per_hp.find(last_hp)==tasks_per_hp.end())
    	tasks_per_hp.insert(pair<int, int>(last_hp, dags));

    for(int h=last_hp+1;h<numOfHyperperiod;h++)
        tasks_per_hp.insert(pair<int, int>(h, dags)); 

    //printf("tasks_per_hp: %d %d\n", last_hp,dags);
        	

    printf("Total number of DAGs : %d \n",numOfDAGs);
    
    unsigned long long int arr[periods.size()];
    std::copy(periods.begin(), periods.end(), arr);
    // printf("before lcm\n");
    hyper_period=findlcm(arr,periods.size());
    printf("hyper_period: %llu\n",hyper_period);

    return tasks_per_hp;
}

int count_dag_from_file(const char* filename)
{
    FILE *file;
    file = fopen(filename, "r");
    char line[1024];
 
    if (!file) 
    {
        fprintf(stderr, "Can't open input file.: dag_history file\n");
        exit(EXIT_FAILURE);
    }
    int count=0;
    while (fgets(line, 1024, file))   
    {  
        count++;
    }
    return count;
}

const char* parse_file_name(const char* filename)
{
    // char* tmp = strdup(filename);
    char* tmp = (char*)calloc(strlen(filename) + 1,1 );
    strcpy(tmp, filename);
    const char* tok;
    tok = strtok(tmp, "_");
    tok = strtok(0, "_");
    tok = strtok(0, ".");

        
    return tok;
}

void build_kernel_object(const char* filename)
{
    // printf("\n%s", filename);
    KernelInfo *cl_info_0,*cl_info_1;
    cl_info_0=new KernelInfo();   
    cl_info_1=new KernelInfo();    
    build_kernel_from_info(*cl_info_0, filename, all_devices, all_ctxs);//Build opencl kernel
    build_kernel_from_info(*cl_info_1, filename, all_devices, all_ctxs);
    kernel_cl_queue[filename]=make_pair(cl_info_0,cl_info_1);
    printf("build_kernel_object filename: %s\n",filename);
}

void build_all_kernel_objects(const char* directory){

    DIR* FD;
    struct dirent* in_folder;
    FILE    *entry_file;
    char line[1024];
    
    /* Scanning the in directory */
    if (NULL == (FD = opendir (directory))) 
    {
        fprintf(stderr, "Error : Failed to open %s directory - %s\n",directory, strerror(errno));
        exit(EXIT_FAILURE);
    }
    while ((in_folder = readdir(FD)) != NULL)
    {
        if (in_folder->d_type == DT_DIR) {
            char path[1024];
            if (strcmp(in_folder->d_name, ".") == 0 || strcmp(in_folder->d_name, "..") == 0)
                continue;

            char foldername[200];
            strcpy (foldername,directory);
            strcat (foldername,in_folder->d_name);
            strcat (foldername,"/");

            DIR* FD1;
            struct dirent* in_file;
            if (NULL == (FD1 = opendir (foldername))) 
            {
                fprintf(stderr, "Error : Failed to open %s directory - %s\n",foldername, strerror(errno));
                exit(EXIT_FAILURE);
            }
            while ((in_file = readdir(FD1)) != NULL)
            {
                
                if (strcmp(in_file->d_name, ".") == 0 || strcmp(in_file->d_name, "..") == 0)
                    continue;
                char filename[1024];
                strcpy (filename,foldername);
                strcat (filename,in_file->d_name);
                printf("\n%s", filename);
                KernelInfo *cl_info_0,*cl_info_1;
                cl_info_0=new KernelInfo();   
                cl_info_1=new KernelInfo();    
                build_kernel_from_info(*cl_info_0, filename, all_devices, all_ctxs);//Build opencl kernel
                build_kernel_from_info(*cl_info_1, filename, all_devices, all_ctxs);
                kernel_cl_queue[filename]=make_pair(cl_info_0,cl_info_1);
                // printf("build_all_kernel_objects filename: %s\n",filename);
            }            
        }
    }    
}

//read the input file traces
int read_trace_file(const char* filename)
{
    FILE *file;
    file = fopen(filename, "r");  

    if (!file) 
    {
        fprintf(stderr, "Can't open input file. trace file: %s\n",filename);
        exit(EXIT_FAILURE);
    }

    int i = 0;
    char line[1024];
    int count=1;
    while (fgets(line, 1024, file))   
    {  
        std::string trace=std::string(line);
        trace_queue.push_back(make_pair(count,trace));
        count++;
    }
    return count-1;
}

//create output file and dump all log information and outputs
void create_output_file(const char* filename )
{      
    char op_file[STR_LENGTH]; 
    time_t t;
    time(&t);
    sprintf(op_file,"./output/log/log_%s.stats",ctime(&t) );    
    char *p = op_file;
    for (; *p; ++p)
    {
        if (*p == ' ' || *p == '\n')
            *p = '_';
    }
    printf("log dumped at %s\n",op_file);
    fp = fopen(op_file, "w+");
    
    char op_file1[STR_LENGTH]; 
    sprintf(op_file1,"./output/time/time_%s",ctime(&t) );
    char *q = op_file1;
    for (; *q; ++q)
    {
        if (*q == ' ' || *q == '\n')
              *q = '_';
    }
    t_result=fopen(op_file1, "w+");
    printf("time log dumped at %s\n",op_file1);

    char op_file2[STR_LENGTH]; 
    sprintf(op_file2,"./output/temperature/tmp_MM_%s",ctime(&t));
    char *r = op_file2;
    for (; *r; ++r)
    {
        if (*r == ' ' || *r == '\n')
              *r = '_';
    }
    tmp_result=fopen(op_file2, "w+");
    printf("temperature dumped at %s\n",op_file2);
    
    char op_file5[STR_LENGTH]; 
    sprintf(op_file5,"./output/output/op_%s",ctime(&t) );
    char *tm = op_file5;
    for (; *tm; ++tm)
    {
        if (*tm == ' ' || *tm == '\n')
              *tm = '_';
    }
    o_result=fopen(op_file5, "w+");
    printf("Output dumped at %s\n",op_file5);

    char op_file4[STR_LENGTH]; 
    string f=getFileName(std::string(filename),true, '/');
    // char cstr[f.size() + 1];
    // strcpy(cstr, f.c_str());
    // strip_ext(cstr);
    const char* tok=parse_file_name(f.c_str());    
    printf("File: %s\n",tok);

    sprintf(op_file4,"./output/plot_history.txt"); 
    r_result=fopen(op_file4, "a+");
    printf("result dumped at %s\n",op_file4);
    fprintf(r_result,"%s:",tok);

    char op_file3[STR_LENGTH]; 
    sprintf(op_file3,"./output/task_set/task_set_history_%s.stats",tok );
    // sprintf(op_file3,"./output/timestamp/timestamp_%s",ctime(&t) );
    char *s = op_file3;
    for (; *s; ++s)
    {
        if (*s == ' ' || *s == '\n')
              *s = '_';
    }
    m_result=fopen(op_file3, "w+");

    // char op_file5[STR_LENGTH]; 
    // sprintf(op_file5,"./output/timestamp/ts_%s.stats",tok );
    // // sprintf(op_file3,"./output/timestamp/timestamp_%s",ctime(&t) );
    // char *s = op_file5;
    // for (; *s; ++s)
    // {
    //     if (*s == ' ' || *s == '\n')
    //           *s = '_';
    // }
    // ts_result=fopen(op_file5, "w+");
    // printf("timestamp dumped at %s\n",op_file5);


}

void get_all_devices() 
{
    //std::vector< std::vector<cl_device_id> > all_devices;
    if (LOG_LEVEL >=1)
    {    
        fprintf(fp,"get_all_devices: BEGIN \n");
    }
    cl_int err;
    cl_uint numPlatforms;    
    err=clGetPlatformIDs(0, NULL, &numPlatforms);
    if (numPlatforms == 0)
    {
        fprintf(fp,"\tFound 0 platforms!\n");
        exit(EXIT_FAILURE);
    }
    //std::vector< std::vector<cl_device_id> > all_devices(numPlatforms) ;
    cl_device_type device_type;
    cl_platform_id platforms[numPlatforms];    
    for(int i=0;i<numPlatforms;i++)
    {
        err=clGetPlatformIDs(numPlatforms, platforms, NULL);    
        cl_uint numDevices;     
        err=clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_ALL, 0, NULL, &numDevices);
        cl_device_id devices[numDevices];   
        
        err=clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_ALL, numDevices, devices, NULL);
        if (numDevices == 0 || devices == NULL)
        {
            fprintf(fp,"\nNo device");
            exit(EXIT_FAILURE);
        }   
        err=clGetDeviceInfo(devices[0], CL_DEVICE_TYPE, sizeof(device_type), &device_type, NULL);
        if(device_type == CL_DEVICE_TYPE_GPU)
        {     
            for(cl_device_id d:devices)
            {
                all_devices[i].push_back(d);
                std::vector<KernelLaunchInfo*> klg ;
                std::vector<KernelLaunchInfo*> w0;
                ready_buffer[0].push_back(klg);
                task_queue[0].push_back(w0);
                deviceAvailability[0].push_back(1);
                dev_available[0].push_back(true);
            }
        }
        else if(device_type == CL_DEVICE_TYPE_CPU)
        {                    
            const cl_device_partition_property  properties[3] = { CL_DEVICE_PARTITION_EQUALLY,NumCoresPerDevice,0 };
            //const cl_device_partition_property  properties[5] = { CL_DEVICE_PARTITION_BY_COUNTS, 4, 3, CL_DEVICE_PARTITION_BY_COUNTS_LIST_END, 0 };
            cl_uint num_sub_devices;    
            err = clCreateSubDevices(devices[0],properties,0,NULL,&num_sub_devices);  
            //fprintf(fp,"\nnum_sub_devices %d ",num_sub_devices);           
            cl_device_id sub_devices[num_sub_devices];
            //sustart_timeb_devices= (cl_device_id*)malloc(num_sub_devices * sizeof(cl_device_id));    
            err = clCreateSubDevices(devices[0],properties,num_sub_devices*sizeof(cl_device_id),sub_devices,NULL);    
            int count=0;
            for(cl_device_id d:sub_devices)
            {   
                //if(count>=(NumofCPUs/NumCoresPerDevice)/2)  //if 2nd sub device is big core
                //if(count<(NumofCPUs/NumCoresPerDevice)/2) //if 1st sub device is big core
                if(count<=(NumofCPUs/NumCoresPerDevice)/2) //considering both sub devices
                {   all_devices[1].push_back(d);
                    std::vector<KernelLaunchInfo*> klc;
                    std::vector<KernelLaunchInfo*> w1;
                    ready_buffer[1].push_back(klc);
                    task_queue[1].push_back(w1);
                    deviceAvailability[1].push_back(1);
                    dev_available[1].push_back(true);
                }
                count++;
            }
        }   
    }
    if (LOG_LEVEL >=1)
    {    
        fprintf(fp,"get_all_devices: END\n");
    }
    //return all_devices;
}

int get_partitioned_kernel_size(int total_size, int num_of_partitions, int i)
{
    int size_p;
    if(num_of_partitions==1)
    {
        size_p = total_size;
    }
    else if(num_of_partitions>1)
    {
        size_p=total_size/num_of_partitions;
        if(i==num_of_partitions )
            size_p=total_size-((i-1)*size_p);
    }
    return size_p;
}

void reset_kl_info()
{
    
    for (auto itr = taskMap.begin(); itr != taskMap.end(); ++itr)       
    {
        std::vector <KernelLaunchInfo*> klinfo_vectors=itr->second;

        KernelLaunchInfo* klinfo=klinfo_vectors[0];
        klinfo->queued=0;
        KernelInfo* cl_info_0=klinfo->task->kernels[0];
        // klinfo->task->data.clear();
        reset_nonpersistent_host_arrays(*cl_info_0, klinfo->task->data);
        
            // printf("data size: %d\n",klinfo->task->data.size());
        if(klinfo->control_mode!=2)
            klinfo->size = cl_info_0->globalWorkSize[0]*cl_info_0->globalWorkSize[1]*cl_info_0->globalWorkSize[2];
        else
        {
            klinfo->size = klinfo->task->task_size;
        }
        klinfo->offset = 0;
        klinfo->priority=0;
        
        printf("reset: %s - %d size %u offset %u \n",klinfo->task->taskID.c_str(),klinfo->queued,klinfo->size,klinfo->offset);
        klinfo->reset=1;
        int total_size=klinfo->size;
        int p_offset=0;
        for(int i=1;i<klinfo_vectors.size();i++)
        {
            KernelLaunchInfo* klinfo_p=klinfo_vectors[i]; //partitioned kernels
            klinfo_p->queued=0;
            
                // printf("data size: %d\n",klinfo->task->data.size());
            klinfo_p->size = get_partitioned_kernel_size(total_size, klinfo_p->task->partition_count, i);
            
            klinfo_p->offset = p_offset*(i-1);
            p_offset=klinfo_p->size ;
            klinfo_p->priority=0;
            
            printf("---reset: %s - %d size %u offset %u \n",klinfo_p->task->taskID.c_str(),klinfo_p->queued,klinfo_p->size,klinfo_p->offset);
            klinfo_p->reset=1;
        }
    }
    //reset dagtimematrix
    for(int i=0;i<DAGtimeMatrix.size();i++)
    {
        for(int j=0;j<DAGtimeMatrix[i].size();j++)
        {
            DAGtimeMatrix[i][j]->safe_mode=0;
            DAGtimeMatrix[i][j]->deadlineViolated=0;
        }
    }
}

void reset_launch_info()
{
    printf("TaskMap size: %lu\n",taskMap.size());

    for (auto itr = taskMap.begin(); itr != taskMap.end(); ++itr)       
    {
        
        std::vector <KernelLaunchInfo*> klinfo_vectors=itr->second;
        KernelLaunchInfo* klinfo=klinfo_vectors[0];
        klinfo->reset=0;
        klinfo->queued=0;
        klinfo->finished=0;
        KernelInfo* cl_info_0=klinfo->task->kernels[0];
        // klinfo->task->data.clear();
        if(refresh_data_per_hyperperiod)
            host_array_initialize(*cl_info_0, klinfo->task->data);
        
        // printf("data size: %d\n",klinfo->task->data.size());
        if(klinfo->control_mode!=2)
            klinfo->size = cl_info_0->globalWorkSize[0]*cl_info_0->globalWorkSize[1]*cl_info_0->globalWorkSize[2];
        else
        {
            klinfo->size = klinfo->task->task_size;
        }
        klinfo->offset = 0;
        klinfo->priority=0;
        
        //printf("reset: %s - %d size %u offset %u \n",klinfo->task->taskID.c_str(),klinfo->queued,klinfo->size,klinfo->offset);
        
        klinfo->reset=1;
        klinfo->not_done_count=klinfo->size;
        
        klinfo->full_task_klinfo=klinfo;
        klinfo->task->majorViolationPerHP=0;

        if(CREATE_SUB_KERNEL==1)
        {   
            int total_size=klinfo->size;
            for(int i=1;i<klinfo_vectors.size();i++)
            {
                KernelLaunchInfo* klinfo_p=klinfo_vectors[i]; //partitioned kernels
                klinfo_p->queued=0;
                
                    // printf("data size: %d\n",klinfo->task->data.size());
                klinfo_p->size = get_partitioned_kernel_size(total_size, klinfo_p->task->partition_count, i);
                
                klinfo_p->offset = klinfo_p->size*(i-1);
                klinfo_p->priority=0;
                klinfo_p->full_task_klinfo=klinfo;
                klinfo_p->finished=0;

                
                //printf("---reset: %s - %d size %u offset %u \n",klinfo_p->task->taskID.c_str(),klinfo_p->queued,klinfo_p->size,klinfo_p->offset);
                klinfo_p->reset=1;
            }
        }
    }
    //reset dagtimematrix
    for(int i=0;i<DAGtimeMatrix.size();i++)
    {
        for(int j=0;j<DAGtimeMatrix[i].size();j++)
        {
            DAGtimeMatrix[i][j]->safe_mode=0;
            DAGtimeMatrix[i][j]->deadlineViolated=0;
        }
    }

    //reset tmp_result
    tempMap.clear(); 

    //reset global variables
    rblock=0;
    nlock=0;
    devlock=0;
    nKernels=0;
    nTasks=0;
    dags_arrived=0;
    SchedulerFinish=0;
    stop_scheduler=0; 

    printf("reset_launch_info done\n");
}


//Initialise host , craete command queue for both cpu and gpu
void host_initialize(std::vector<std::vector<cl_device_id>>& all_devices, std::vector<cl_context>& ctxs, std::vector< std::vector<cl_command_queue> >& cmd_qs) 
{   
    if (LOG_LEVEL >=1)
        fprintf(fp,"host_initialize: BEGIN\n");
    

    cl_int status;
    cl_device_id* gpu=&(all_devices[PLATFORM_GPU][0]);
    cl_device_id* cpu=&(all_devices[PLATFORM_CPU][0]);
    int num_gpus=all_devices[PLATFORM_GPU].size();
    int num_cpus=all_devices[PLATFORM_CPU].size();    

    cl_platform_id gpu_platform;
    clGetDeviceInfo(all_devices[PLATFORM_GPU][0], CL_DEVICE_PLATFORM, sizeof(gpu_platform), &gpu_platform, NULL);
    cl_context_properties gps[3] = {CL_CONTEXT_PLATFORM, (cl_context_properties)gpu_platform, 0 };
    ctxs[PLATFORM_GPU] = clCreateContext(gps, num_gpus, gpu, NULL, NULL, &status);
    check(status, "\nCreating GPU Context");
    if(status!=CL_SUCCESS)
        fprintf(fp, "\tError in creating Contexts.\n");

    cl_platform_id cpu_platform;
    clGetDeviceInfo(all_devices[PLATFORM_CPU][0], CL_DEVICE_PLATFORM, sizeof(cpu_platform), &cpu_platform, NULL);
    cl_context_properties cps[3] = {CL_CONTEXT_PLATFORM, (cl_context_properties)cpu_platform, 0 };
    ctxs[PLATFORM_CPU] = clCreateContext(cps, num_cpus, cpu, NULL, NULL, &status);
    check(status, "\nCreating CPU Context");
    if(status!=CL_SUCCESS)
        fprintf(fp, "\tError in creating Contexts.\n");

    // printf("Creating 0: %u, 1: %u\n",ctxs[0],ctxs[1]);
    if (LOG_LEVEL >=1)
    {
        fprintf(fp, "\tCreated %d Contexts.\n",NumOfPlatforms);
    }

    cmd_qs[PLATFORM_GPU] = create_command_queue_for_each(gpu, num_gpus, ctxs[PLATFORM_GPU]);
    cmd_qs[PLATFORM_CPU] = create_command_queue_for_each(cpu, num_cpus, ctxs[PLATFORM_CPU]);
    // std::vector<cl_command_queue> gpu_cq=create_command_queue_for_each(gpu, num_gpus, ctxs[PLATFORM_GPU]);
    // std::vector<cl_command_queue> cpu_cq=create_command_queue_for_each(cpu, num_cpus, ctxs[PLATFORM_CPU]);
    // cmd_qs.push_back(gpu_cq);
    // cmd_qs.push_back(cpu_cq);

    if (LOG_LEVEL >=1)
        fprintf(fp,"host_initialize: END \n");
    
  
}

void get_device_specification(char *filename){
    //DeviceSpecification cpuB,cpuL,gpu;
    //std::vector<DeviceConfig> configures_g, configures_b,configures_l;


    FILE *file;
    file = fopen(filename, "r");

    if (!file) {
        fprintf(stderr, "Can't open file.\n");
        exit(EXIT_FAILURE);
    }
    
    
    char line[1024];
    while (fgets(line, 1024, file))
    {
        // char* tmp = strdup(line);
        char* tmp = (char*)calloc(strlen(line) + 1,1);
        strcpy(tmp, line);
        // printf("get_device_specification: tmp: %s\n",tmp);

        DeviceConfig* d_config=new DeviceConfig;
        const char* tok;
        tok = strtok(line, ",");
        int p = atoi(tok);
        // printf("p:%d\n",p);
        tok = strtok(0, ",");
        int d = atoi(tok);
        // printf("d:%d\n",d);
        tok = strtok(0, ",");
        d_config->frequency=atoi(tok);
        // printf("f:%u\n",d_config->frequency);
        tok = strtok(0, "\n");
        d_config->speedup=atof(tok);
        // printf("s:%f\n",d_config->speedup);
        // tok = strtok(line, "\n");
        // d_config.powerup=atof(tok);

        if(deviceSpec[p].size()<d+1)
        { 

          // printf("new dev:\n");
          DeviceSpecification* ds=new DeviceSpecification(); 
          deviceSpec[p].push_back(ds);
        }
        // printf("deviceSpec:size %d\n",deviceSpec[p].size());
        deviceSpec[p][d]->device_config.push_back(d_config);
        if(tmp)
            free(tmp);
    }   
    for(int i=0;i<NumOfPlatforms;i++)
    {
      for(int j=0;j<deviceSpec[i].size();j++)
      {
        deviceSpec[i][j]->lowFrequencyBound=  deviceSpec[i][j]->device_config[0]->frequency;
        deviceSpec[i][j]->midFrequency=  deviceSpec[i][j]->device_config[deviceSpec[i][j]->device_config.size() / 2]->frequency;
        deviceSpec[i][j]->highFrequencyBound= deviceSpec[i][j]->device_config[deviceSpec[i][j]->device_config.size() - 1]->frequency;
    
      }
    }
    // deviceSpec[0].midFrequency=  deviceSpec[0].device_config[deviceSpec[0].device_config.size() / 2].frequency;
    // deviceSpec[0].highFrequencyBound= deviceSpec[0].device_config[deviceSpec[0].device_config.size() - 1].frequency;
    // deviceSpec[1].lowFrequencyBound= deviceSpec[1].device_config[0].frequency;
    // deviceSpec[1].midFrequency=  deviceSpec[1].device_config[deviceSpec[1].device_config.size() / 2].frequency;
    // deviceSpec[1].highFrequencyBound= deviceSpec[1].device_config[deviceSpec[1].device_config.size() - 1].frequency;
    // deviceSpec[2].lowFrequencyBound= deviceSpec[2].device_config[0].frequency;
    // deviceSpec[2].midFrequency=  deviceSpec[2].device_config[deviceSpec[2].device_config.size() / 2].frequency;
    // deviceSpec[2].highFrequencyBound= deviceSpec[2].device_config[deviceSpec[2].device_config.size() - 1].frequency;
    // deviceSpec[3].lowFrequencyBound= deviceSpec[3].device_config[0].frequency;
    // deviceSpec[3].midFrequency=  deviceSpec[3].device_config[deviceSpec[3].device_config.size() / 2].frequency;
    // deviceSpec[3].highFrequencyBound= deviceSpec[3].device_config[deviceSpec[3].device_config.size() - 1].frequency;
}

void print_all_device_info(std::vector<std::vector<cl_device_id>>& all_devices)
{
    fprintf(fp,"\nDEVICE INFO:\n");
    for(int i=0;i<all_devices.size();i++)
    {   for(int j=0;j<all_devices[i].size();j++){
            //fprintf(fp,"DeviceId@print_all_device_info: %u @ %d-%d",all_devices[i][j],i,j);
            print_device_info(all_devices[i][j],i,j);

        }
    }
}

pair <int, int> dag_to_job_id(int dag)
{
    int job=-1;
    int inst=-1;
    auto it = dagMap.find(dag);
    if(it != dagMap.end())
    {
        job=it->second->jobID;
        inst=it->second->instanceID;
    }
    return make_pair(job,inst ); 
}

int job_to_dag_id(int job, int inst)
{
    int dag =-1;
    for (auto it = dagMap.begin(); it != dagMap.end(); it++) 
    {
        if(it->second->jobID==job && it->second->instanceID==inst)
        {
            dag=it->second->globalDAGID;
            break;
        }
    }
    return dag; 
}


void initialise_nodes_matrix()
{

    if (LOG_SCHEDULER >=1)
        fprintf(fp,"initialise_nodes_matrix: BEGIN \n");
    
    for(int d=0 ;d<=numOfDAGs;d++)
    {
        int job_id = -1;
        job_id = dag_to_job_id(d).first;
        nodes_matrix.push_back(std::vector<int>());
        // ######################numOfNodes is set to 1 for independent tasks##################
        int numberOfNodes=1;
        if(isProfileMode!=1)
        {   auto it = jobMap.find(job_id);
            if(it != jobMap.end())
                numberOfNodes=it->second->numOfNodes;            
        }
        for (int k=0;k<numberOfNodes;k++) 
          nodes_matrix[d].push_back(0);        
    } 

    // printf("\nnodes_matrix size %d\n",nodes_matrix.size());
    // for(int i=0;i<nodes_matrix.size();i++)
    // {
    //     for(int j=0;j<nodes_matrix[i].size();j++)
    //         printf("%d ",nodes_matrix[i][j]);
    //     printf("\n");
    // }

    if (LOG_SCHEDULER >=1)
        fprintf(fp,"initialise_nodes_matrix: END\n");
}


int check_dependency(std::vector <int> deps,int dag)
{
    if (LOG_SCHEDULER >=1)
        fprintf(fp,"check_dependency: BEGIN: %llu \n",get_current_time());
    
    for(int i =0;i<deps.size();i++)
        if(nodes_matrix[dag][deps[i]]==0)
            return 0;                
        
    if (LOG_SCHEDULER >=1){
        fprintf(fp,"check_dependency: END: %llu \n",get_current_time());
        fflush(fp);
    }

    return 1;  
}




//parse trace input
int parse_trace_input(int* index)
{
    std::cout << trace_queue[*index].second<<"\n"; 
    printf("Parsing trace %d\n", *index);
    if (LOG_SCHEDULER >=1)
        fprintf(fp,"parse_trace_input: BEGIN \n");

     
    // printf("Index %d \n",*index);
    char *input = new char[trace_queue[*index].second.length() + 1];
    strcpy(input, trace_queue[*index].second.c_str());    
    char* tok,dep;
    tok = strtok(input, ",");//dag number

    std::string file_name = std::string("./tinfo/DAG_");    
    file_name = file_name + std::string(tok);
    int job=atoi(tok);
    //std::string unique_task_id = "D" + std::string(tok);
    //unique_task_id=unique_task_id+"-N";
    //std::string dep_id = std::string(unique_task_id.c_str());
    tok = strtok(0, ",");//node number   
    file_name = file_name + "/node_" + std::string(tok);
    //unique_task_id=unique_task_id+std::string(tok)+"-";
    
    tok = strtok(0, ","); //fused variant
    file_name = file_name + ":" + std::string(tok);
    //unique_task_id=unique_task_id+std::string(tok);
    // printf("File Name: %s \n",file_name.c_str());    
    //std::cout <<"Parsing filename: "<<file_name;
    printf("\n");
    std::vector<int> nodes;
    if(atoi(tok)!=-1)
    {
        std::stringstream ss(tok);
        int i;
        while (ss >> i)
        {   
            nodes.push_back(i);
            //printf("\nNodes: %d",i);
            if (ss.peek() == '_')
            ss.ignore();
        }  
    }
    // printf("nodes.size: %u\n",nodes.size());
    tok = strtok(0, ",");
    std::vector<int> deps; //vector of dependent nodes in the dag
    if(atoi(tok)!=-1)
    {
        std::stringstream ss(tok);
        int i;
        while (ss >> i)
        {   
            deps.push_back(i);
            //printf("\nDep %d",i);
            if (ss.peek() == '_')
            ss.ignore();
        }  
    }  
    
    tok = strtok(0, ",");
    int platform =atoi(tok);    
    tok = strtok(0, ",");
    int device =atoi(tok);
    // printf("\ndevices: %d-%d",platform,device);    
    tok = strtok(0, ",");
    float exTime_G =(unsigned long long int )(atoi(tok)*1000);
    tok = strtok(0, ",");
    float exTime_C =(unsigned long long int )(atoi(tok)*1000);
    // printf("\nexTime: %f",exTime);
    //tok = strtok(0, ",");
    //unsigned long long int arrival =(unsigned long long int )(atoi(tok)*1000);
    // printf("\narrival: %d",arrival);
    tok = strtok(0, ",");
    unsigned long long int deadline =(unsigned long long int )(atoi(tok)*1000);
    // printf("\ndeadline: %d",deadline);
    tok = strtok(0, ",");
    int isTerminal =(atoi(tok));
    // printf("\nisTerminal: %d",isTerminal);
    tok = strtok(0, ",");
    int dag =(atoi(tok));
    // printf("\ndag: %d",dag);
    tok = strtok(0, ",");
    int inst =(atoi(tok));
    // printf("\ninst: %d",inst);
    tok = strtok(0, "\n");
    int freq =(atoi(tok));
    // printf("freq: %d\n",freq); 
    delete [] input,tok,dep;  
    if (freq==-1)     
    {
        freq=deviceSpec[platform][device]->midFrequency;
    }
    // printf("frequency: %u\nfile_name: %s\n",freq,file_name.c_str()); 

    auto it = dagMap.find(dag);
    if(it != dagMap.end())
    {           
        it->second->instanceID=inst;            
    }

    KernelInfo *cl_info_0,*cl_info_1;
    cl_info_0 = (kernel_cl_queue.at(file_name.c_str())).first;
    cl_info_1 = (kernel_cl_queue.at(file_name.c_str())).second;

    //Populate TaskInfo
    TaskInfo* task_info=new TaskInfo();
    task_info->traceID=trace_queue[*index].first;
    (task_info->taskID).assign(file_name.c_str());
    // printf("taskID: %s\n",task_info->taskID.c_str()); 

    DAGInfo *dinfo= new DAGInfo();
    dinfo->globalDAGID=dag;
    dinfo->jobID=job;
    dinfo->instanceID=inst;
    task_info->dagInfo=dinfo;
    for(int i=0;i<nodes.size();i++)
        task_info->nodes.push_back(nodes[i]);
    task_info->kernels.push_back(cl_info_0);
    task_info->kernels.push_back(cl_info_1);
    for(int i=0;i<deps.size();i++)
        task_info->dependency.push_back(deps[i]);
    task_info->WCET.push_back(exTime_G);
    task_info->WCET.push_back(exTime_C);
    //task_info->exTime=exTime;
    task_info->arrival=DAGtimeMatrix[job][inst]->arrivalTime;
    //task_info->deadline=DAGtimeMatrix[job][inst]->deadline*;
    //task_info->arrival=arrival;
    // printf("arrival :%llu\n",task_info->arrival);
    task_info->deadline=deadline;
    task_info->isTerminal=isTerminal;    
    task_info->task_size= cl_info_0->globalWorkSize[0]*cl_info_0->globalWorkSize[1]*cl_info_0->globalWorkSize[2];
    
    if (CREATE_SUB_KERNEL)
    {    
        if(task_info->task_size>262144)
            task_info->partition_count=4;
        else if(task_info->task_size>131072)
            task_info->partition_count=2;
        else 
            task_info->partition_count=1;


    }
    //########################hard codedrigth now########
    task_info->basespeed.push_back(task_info->task_size/854761.5);
    if(!refresh_data_per_hyperperiod)
        host_array_initialize(*cl_info_0, task_info->data);//populate input host arrays
    task_info->majorViolationPerHP=0;
    task_info->totalMajorViolation=0;
    //printf("Task populated\n");

    //Populate KernelLaunchInfo
    KernelLaunchInfo* launch_info=new KernelLaunchInfo(); 
    // trace_queue.erase(trace_queue.begin()+*index);
    // *index=*index-1;
    launch_info->task=task_info;
    // printf("task to launch_info\n");
    launch_info->platform_pos =platform;
    launch_info->device_pos =device;
    // printf("p; %d d: %d\n",launch_info->platform_pos,launch_info->device_pos);
    //launch_info->device_index = get_dev_index(platform,device);
    launch_info->size = cl_info_0->globalWorkSize[0]*cl_info_0->globalWorkSize[1]*cl_info_0->globalWorkSize[2];
    //launch_info->size = C_SZ;
    launch_info->offset =0;
    launch_info->released_b=0;
    launch_info->released_d=0;
    // printf("sz; %d off: %d\n",launch_info->size,launch_info->offset);
    launch_info->finished =0;
    //launch_info->frequency=deviceSpec[launch_info->device_index].device_config[deviceSpec[launch_info->device_index].device_config.size() / 2].frequency;
    launch_info->frequency=(unsigned int)freq;
    launch_info->control_mode=CONTROLLER_MODE;
    //printf("freq: %u\n",launch_info->frequency);

    //if 0th node if present in fused variant add tp dagtimematrix
    if (std::find(nodes.begin(), nodes.end(), 0) != nodes.end())
    {    
        //printf("inside if loop DAGtimeMatrix[%d][%d] size %d %d\n",job,inst,DAGtimeMatrix.size(),DAGtimeMatrix[job].size());
        DAGtimeMatrix[job][inst]->klinfo=launch_info;
        //printf("inside if loop end\n");
        
    }
    // DAGtimeMatrix[job][inst]->device=device;    

    // std::string task_name=std::to_string(task_info->dagInfo->globalDAGID)+"-";
    // for(int i=0;i<nodes.size();i++)
    //     task_name+=std::to_string(nodes[i]);
    //printf("Before populating taskMap\n");
    std::vector<KernelLaunchInfo*> launch_info_vector;
    launch_info_vector.push_back(launch_info);
    for(int i=0;i<nodes.size();i++)
    {
        //taskMap.insert( pair<pair <int, int>, std::vector<KernelLaunchInfo*>>(make_pair(task_info->dagInfo->globalDAGID,nodes[i]), launch_info_vector) ); 
        
        //taskMap.insert(make_pair(task_info->dagInfo->globalDAGID,nodes[i]),launch_info); 
        // printf("\nparse_trace_input: MAP<%d-%d> : %u",task_info->dagInfo->globalDAGID,nodes[i],launch_info);

        if(CREATE_SUB_KERNEL==1)
        {
            for(int p=0;p<launch_info_vector[0]->task->partition_count;p++)
            {
                KernelLaunchInfo* launch_info_p=new KernelLaunchInfo(); 


                launch_info_p->task=launch_info->task;
                launch_info_p->platform_pos=launch_info->platform_pos;
                launch_info_p->tmp_platform_pos=launch_info->platform_pos;
                launch_info_p->device_pos=launch_info->device_pos;
                launch_info_p->tmp_device_pos=launch_info->device_pos;
                launch_info_p->size=launch_info->size;
                launch_info_p->offset=launch_info->offset;
                launch_info_p->released_b=launch_info->released_b;
                launch_info_p->released_d=launch_info->released_d;
                launch_info_p->frequency=launch_info->frequency;
                launch_info_p->tmp_frequency=launch_info->frequency;
                launch_info_p->tmp_idle_slot=0;
               
                //copy_launch_info(launch_info,launch_info_p);

                launch_info_vector.push_back(launch_info_p);
            }

        }
        taskMap.insert( pair<pair <int, int>, std::vector<KernelLaunchInfo*>>(make_pair(task_info->dagInfo->globalDAGID,nodes[i]), launch_info_vector) ); 
        // print_launch_info(*launch_info);    
    }

       
    if(deps.size()==0)
    {   
        // printf("\narrivaltime job %d instance %d %llu",task_info->dagInfo->jobID,task_info->dagInfo->instanceID,DAGtimeMatrix[task_info->dagInfo->jobID][task_info->dagInfo->instanceID]->arrivalTime)   ;  
        //if(DAGtimeMatrix[task_info->dagInfo->jobID][task_info->dagInfo->instanceID]->arrivalTime==0)
        //     ready_buffer[platform][device].push_back(launch_info); // populate the task-device queue with the index of cl_info
        //else    
            task_queue[platform][device].push_back(launch_info);
    
    }
    
    if (LOG_SCHEDULER >=1)
        fprintf(fp,"parse_trace_input: END\n");

    return dag;
   
}

int task_dev_queue_empty()
{
    if (LOG_SCHEDULER >=1)
        fprintf(fp,"task_dev_queue_empty: BEGIN: %llu \n",get_current_time());
    
    for(int i=0;i<NumOfPlatforms;i++){
        int NumOfDevsPerPlatform=all_devices[i].size();
        for(int j=0;j<NumOfDevsPerPlatform;j++)
            if(ready_buffer[i][j].size()>0 || task_queue[i][j].size()>0 )
                return 0;
    }
    if (LOG_SCHEDULER >=1){
        fprintf(fp,"task_dev_queue_empty: END: %llu \n",get_current_time());
        fflush(fp);
    }
    
    return 1;
    
}

void populate_task_queue()
{
    if (LOG_SCHEDULER >=1)
        fprintf(fp,"\npopulate_task_queue: BEGIN \n");

    printf("trace_queue size: %lu\n",trace_queue.size());
    for(int i=0;i<trace_queue.size();i++)
    {
        // printf("Index: %d\n",i);
        int *index=&i;
        parse_trace_input(index);     
    } 
    // printf("parse_trace_input end\n");
    initialise_nodes_matrix();
    printf("\nsize of taskQueue: [GPU0][GPU1][CPU0] ->%lu-%lu-%lu\n",task_queue[0][0].size(),task_queue[0][1].size(),task_queue[1][0].size());
    
    // printf("initialise_nodes_matrix\n");
    if (LOG_SCHEDULER >=1)
        fprintf(fp,"populate_task_queue: END\n");
    
}

/*
int modify_dag_task_data(const char* filename1,const char* filename2, int traceCount)
{
    FILE *file;
    file = fopen(filename1, "r");
    
    if (!file) 
    {
        fprintf(stderr, "Can't open input file. dag_history file\n");
        exit(EXIT_FAILURE);
    }    
  
    char line[1024];
    unsigned long long int hp;
    std::vector<unsigned long long int> periods;
    periods.push_back((unsigned long long int)hyper_period);
    int count=0;
    while (fgets(line, 1024, file))   
    {  

        if(count>=numOfDAGs)
        {
            // printf("%s\n",line);
            char* tmp = strdup(line);
            // char* tmp = (char*)malloc(strlen(line) + 1);
            // strcpy(tmp, line);

            DAGTime* dagTime=new DAGTime();
            // printf("%s\n",line);
            const char* tok;
            tok = strtok(line, " ");
            //globalid
            int gdag = atoi(tok);  
            // printf("gdag:%d\n",gdag); 
            tok = strtok(0, " ");  
            //jobid  
            int job = atoi(tok); 
            tok = strtok(0, " ");
            dagTime->arrivalTime=atoi(tok)*1000;
            // printf(" %llu",dagTime->arrivalTime);  
            tok = strtok(0, "\n");
            dagTime->deadline=atof(tok)*1000;
            dagTime->safe_mode=0;
            dagTime->deadlineViolated=0;
            if(count<NumOfJobs)
                periods.push_back((unsigned long long int)dagTime->deadline);
            
            // printf(" %f\n",dagTime->deadline);
            // instances[i]=0;
            DAGtimeMatrix[job].push_back(dagTime);
            // printf("\nsize DAGtimeMatrix[i][j] %llu",DAGtimeMatrix[i][DAGtimeMatrix[i].size()-1]->arrivalTime);
            DAGInfo *dag=new DAGInfo();
            dag->globalDAGID=gdag;
            dag->jobID=job;
            dagMap.insert( pair<int, DAGInfo*>(gdag, dag) );
            if(tmp)
                free(tmp);
            numOfDAGs=gdag+1;
        }
        count++;
    }
    // printf("before copy\n");
    
    unsigned long long int arr[periods.size()];
    std::copy(periods.begin(), periods.end(), arr);
    // printf("before lcm\n");
    hyper_period=findlcm(arr,periods.size());
    printf("new hyper_period: %llu\n",hyper_period);


    //READ TRACE FILE

    file = fopen(filename2, "r");  

    if (!file) 
    {
        fprintf(stderr, "Can't open input file. trace file\n");
        exit(EXIT_FAILURE);
    }

    int i = 0;
    count=0;
    while (fgets(line, 1024, file))   
    {  

        if(count>=traceCount)
        {
            std::string trace=std::string(line);
            trace_queue.push_back(make_pair(count+1,trace));

            //POPULATE TASK_QUEUE
            int *index=&count;
            int d =parse_trace_input(index);  

            //add to nodes_matrix            
            int job_id= dag_to_job_id(d).first;
            nodes_matrix.push_back(std::vector<int>());
            
            int numberOfNodes;
            if(isProfileMode==1)
                numberOfNodes=1;
            else
                numberOfNodes=(job_id%2==0?NumNodes_0:NumNodes_1);
            for (int k=0;k<numberOfNodes;k++) 
            nodes_matrix[d].push_back(0);         

        }
        count++;
    }

    return count-1;
}
*/

void create_micro_kernel(int platform)
{
    KernelInfo *mcl_info;    
    mcl_info=new KernelInfo();     
    build_kernel_from_info(*mcl_info, "./tinfo/tinfo_micro_kernel", all_devices, all_ctxs);
    TaskInfo* task_info=new TaskInfo();
    task_info->kernels.push_back(mcl_info);
    host_array_initialize(*task_info->kernels[0], task_info->data);
    task_info->traceID=-1; 
    task_info->isTerminal=1;    
    task_info->task_size= mcl_info->globalWorkSize[0]*mcl_info->globalWorkSize[1]*mcl_info->globalWorkSize[2];
    KernelLaunchInfo* launch_info=new KernelLaunchInfo();
    launch_info->task=task_info;
    launch_info->platform_pos =platform;
    launch_info->device_pos =0;
    launch_info->size = task_info->task_size;
    launch_info->offset =0;
    launch_info->priority=0;
    launch_info->frequency=deviceSpec[platform][0]->midFrequency;    
    micro_kernel_klinfo=launch_info;
}


void *run_kernel(void *vargp){  
    cpu_set_t cpuset;
    pthread_t thread_id_scheduler = pthread_self(); 
    CPU_ZERO(&cpuset);
    CPU_SET(3, &cpuset);    
    int s = pthread_setaffinity_np(thread_id_scheduler, sizeof(cpu_set_t), &cpuset);
    int rc;
    pthread_attr_t attr;
    struct sched_param param;
    // params.sched_priority = sched_get_priority_max(SCHED_RR);
    // pthread_setschedparam(thread_id_scheduler,SCHED_RR,&params);
    rc = pthread_attr_init (&attr);
    
    (param.sched_priority)++;
    rc = pthread_attr_setschedparam (&attr, &param);
    int traceID=*((int *) vargp);
    printf("\nrun_kernel %lu on core %d \n",pthread_self(),sched_getcpu());

    // KernelLaunchInfo *kl_info = (KernelLaunchInfo *) vargp;
    // printf("Obtaining Kernel Launch Info Object\n ");    
    
    KernelLaunchInfo *kl_info = taskMap.begin()->second[0];
    printf("Obtained Kernel Launch Info Object for trace %d\n",kl_info->task->traceID);    
    
    if (LOG_SCHEDULER >=1)
            fprintf(fp,"\n\nrun_kernel: BEGIN: %llu \n",get_current_time());

    std::string filename = "./profile_statistics/trace_"+to_string(traceID)+".stats";
    std::string microkernel_filename = "./profile_statistics/microkernel_for_trace_"+to_string(kl_info->task->traceID)+".stats";
    ofstream ofs, ofs_microkernel;
    ofs.open(filename,std::ios_base::app|std::ios_base::out);
    ofs_microkernel.open(microkernel_filename,std::ios_base::app|std::ios_base::out);

    for(int i = 0 ;i<PROFILE_ITERATIONS; i++)
    {
        printf("Dispatching microkernel\n");
        if(micro_kernel_device!=-1)
        {
            dispatch(*micro_kernel_klinfo);
            micro_kernel_klinfo->finished=0;
        }
        
        kl_info->kex.get_image_start_time = get_current_time();
        // reset_nonpersistent_host_arrays(*(kl_info->task->kernels[0]),kl_info->task->data);
        kl_info->kex.get_image_end_time = get_current_time();
        printf("Dispatching kernel to be profiled\n");
        dispatch(*kl_info);
        kl_info->finished=0;
        
        // int bound = 0;
        // if(micro_kernel_device!=-1)
        //     bound = 2;
        // else
        //     bound = 1;
        
        
        // while(num_profile_kernels < bound )
        // {

        //     printf("HOST %d\n",num_profile_kernels);
            
        // }
        std::unique_lock<std::mutex> lk(run_kernel_lock);
        cv.wait(lk, []{return processed;});
        num_profile_kernels=0;
        micro_kernel_klinfo->finished=0;
        printf("microkernel finished!\n");
        kl_info->finished=0;
        printf("profile kernel finished!\n");
        printf("Dumping profile statistics\n");
        if(EVENTS_ENABLE)
        {
            PERSISTENCE_DEBUG printf("Dumping event information and host information data\n");
            dump_profile_statistics(kl_info,ofs);
            dump_profile_statistics(micro_kernel_klinfo,ofs_microkernel);
            printf("Releasing kernel events\n");
            release_kernel_events(kl_info->ke);
            release_kernel_events(micro_kernel_klinfo->ke);
        }
        else
        {
            PERSISTENCE_DEBUG printf("Dumping only host information data\n");
            dump_total_time(kl_info,ofs);
            dump_total_time(micro_kernel_klinfo,ofs_microkernel);
        }
        printf("Resetting kl info\n");
        reset_kl_info();
        printf("Finishing iteration %d\n",i);
        processed = false;
           
    }    
    
    // release_buffers(micro_kernel_klinfo->io);
    // release_host_arrays(micro_kernel_klinfo->task->data);

    ofs.close();
    ofs_microkernel.close();
    

    if(get_current_time()<hyper_period)
        std::this_thread::sleep_for(std::chrono::microseconds(hyper_period-get_current_time()));

    if (LOG_SCHEDULER >=1){
        fprintf(fp,"run_scheduler: END: %llu \n",get_current_time());
        fflush(fp);
    }
    printf("exiting\n");
}


void *run_scheduler(void *vargp){  
      cpu_set_t cpuset;
    pthread_t thread_id_scheduler = pthread_self(); 
    CPU_ZERO(&cpuset);
    CPU_SET(3, &cpuset);    
    int s = pthread_setaffinity_np(thread_id_scheduler, sizeof(cpu_set_t), &cpuset);
    struct sched_param params;
    params.sched_priority = sched_get_priority_max(SCHED_RR);
    pthread_setschedparam(thread_id_scheduler,SCHED_RR,&params);
    printf("\nrun_scheduler %u on core %d \n",pthread_self(),sched_getcpu());

    struct sched_param param;
    // params.sched_priority = sched_get_priority_max(SCHED_RR);
    // pthread_setschedparam(thread_id_scheduler,SCHED_RR,&params);
    pthread_attr_t attr;
    int rc = pthread_attr_init (&attr);    
    (param.sched_priority)++;
    rc = pthread_attr_setschedparam (&attr, &param);

    std::pair<int, int> tasks_per_hp=*((std::pair<int, int> *) vargp);
    cur_hyper_period = tasks_per_hp.first;
    taskcount = tasks_per_hp.second;

    if (LOG_SCHEDULER >=1)
        fprintf(fp,"\n\nrun_scheduler: BEGIN: %llu \n",get_current_time());

    while(nTasks<taskcount){
        dispatch_from_queue();  

    }
    if(get_current_time()<hyper_period)
        std::this_thread::sleep_for(std::chrono::microseconds(hyper_period-get_current_time()));

    
    // printf("taskcount: %d, no_micro_kernel: %d, nTasks: %d\n",taskcount,no_micro_kernel,nTasks);

    
    if (LOG_SCHEDULER >=1){
        fprintf(fp,"run_scheduler: END: %llu \n",get_current_time());
        fflush(fp);
    }
}

bool is_any_dev_available()
{
    //printf("************is_dev_available is called: \n");
    if(nTasks==taskcount)
        return true;
    for(int p=0;p<NumOfPlatforms;p++)
    {
        for(int d=0;d<all_devices[p].size();d++)
        {  
            //printf("is_dev_available p-d: %d %d, dev_available[p][d]: %d , ready_buffer[p][d].size(): %d \n",p,d,(dev_available[p][d]?1:0),ready_buffer[p][d].size());
            if(dev_available[p][d]==true && ready_buffer[p][d].size()>0 )
            {    
                //printf("is_dev_available is true \n");
                return true;
            }
            //printf("is_dev_available is false: %d %d \n",p,d);
        }
    }
    return false;
}

bool is_rbuffer_filled()
{
    
    if(nTasks==taskcount)
        return true;
    for(int p=0;p<NumOfPlatforms;p++)
    {
        for(int d=0;d<all_devices[p].size();d++)
        {  
            if(ready_buffer[p][d].size()>0 )
            {    
                return true;
            }
        }
    }
    return false;
}

bool is_dev_available(int platform, int device)
{
    
    if(dev_available[platform][device]==true )
        return true;
    else
        return false;
}

std::pair<int, int> empty_device()
{
    for(int p=0;p<NumOfPlatforms;p++)
    {
        for(int d=0;d<all_devices[p].size();d++)
        {  
            if(dev_available[p][d]==true  )
                return make_pair(p,d);
        }
    }
    return make_pair(-1,-1);
}

std::vector<std::pair<int, int>> ready_buffer_available()
{
    std::vector<std::pair<int, int>> available_ready_buffers;
    for(int p=0;p<NumOfPlatforms;p++)
    {
        for(int d=0;d<all_devices[p].size();d++)
        {  
            if(ready_buffer[p][d].size()>0)
                available_ready_buffers.push_back(make_pair(p,d));
        }
    }
    return available_ready_buffers;
}


void dispatch_from_queue( )
{
    if (LOG_SCHEDULER >=1)
        fprintf(fp,"dispatch_from_queue: BEGIN: %llu \n",get_current_time());
    
    // printf("dispatch_from_queue: BEGIN: %llu \n",get_current_time());

    printf("dispatch_from_queue: BEGIN: %llu \n",get_current_time());
    

    std::vector<std::pair<int, int>> r_buffer= ready_buffer_available(); 
    //printf("dispatch_from_queue: ready_buffer_available size: %d \n",ready_buffer_available().size());

    if(r_buffer.size()>0)
    {
        for(int i=0;i<r_buffer.size();i++)
        {
            int p=r_buffer[i].first;
            int d=r_buffer[i].second;
        
    
            if(dev_available[p][d])
            {    
                
                //printf("ready bufefr size: %d p: %d d: %d\n",ready_buffer[p][d].size(),p,d);
                fprintf(fp,"\tDispatching from dispatch_from_queue_%d-%d: %d %d\n",p,d,ready_buffer[p][d].size(),task_queue[p][d].size());
                //printf("deviceAvailability %d %d %d rbsize %d wqsize %d \n",p,d,deviceAvailability[p][d],ready_buffer[p][d].size(),task_queue[p][d].size());

                mtx_rblock.lock();
                KernelLaunchInfo* kl_info=ready_buffer[p][d][0];
                //printf("\n\nready_buffer[%d][%d].size %d\n",p,d,ready_buffer[p][d].size());
                ready_buffer[p][d].erase(ready_buffer[p][d].begin()+0);
                //printf("\n\nready_buffer[%d][%d].size new %d\n",p,d,ready_buffer[p][d].size());
                mtx_rblock.unlock();
                
                // rblock=0;
                //printf("\tdispatch_from_queue: Dispatching...");
                //print_launch_info(*kl_info);
                dispatch(*kl_info);                
            }
        }
    }
    std::unique_lock<std::mutex> lk(run_scheduler_lock);
    if (dags_arrived<numOfDAGs)
    {    cv_rbuffer_available.wait(lk, []{return is_rbuffer_filled();});
        printf("Conditional variable cv_rbuffer_available unlocked : %llu \n",get_current_time());
    }
    cv_dev_available.wait(lk, []{return is_any_dev_available();});
    printf("Conditional variable cv_dev_available unlocked : %llu \n",get_current_time());

    if (LOG_SCHEDULER >=1){
        fprintf(fp,"dispatch_from_queue: END: %llu \n",get_current_time());
        fflush(fp);
    }
}

/*
void *taskArrivalUpdation(void *vargp){
    if (LOG_SCHEDULER >=1){
        fprintf(fp,"taskArrivalUpdation: BEGIN: %llu \n",get_current_time());
        fflush(fp);
    }
    cpu_set_t cpuset;
    pthread_t thread_id_scheduler = pthread_self(); 
    CPU_ZERO(&cpuset);
    CPU_SET(1, &cpuset);    
    int s = pthread_setaffinity_np(thread_id_scheduler, sizeof(cpu_set_t), &cpuset);
    int t=0;

    for (auto itr = taskMap.begin(); itr != taskMap.end(); ++itr) 
    { 
        KernelLaunchInfo* kl_info = itr->second ;
        unsigned long long int arrival_time =kl_info->task->arrival;
        if(get_current_time()<arrival_time)
        {
            std::this_thread::sleep_for(std::chrono::microseconds(arrival_time-get_current_time()));
        }
        
    } 
    if (LOG_SCHEDULER >=1)
    {
        fprintf(fp,"taskArrivalUpdation: END: %llu \n",get_current_time());
        fflush(fp);
    }
}
*/

void *taskToReadyB(void *vargp){

    if (LOG_SCHEDULER >=1){
        fprintf(fp,"taskToReadyB: BEGIN: %llu \n",get_current_time());
        fflush(fp);
    }
    cpu_set_t cpuset;
    pthread_t thread_id_scheduler = pthread_self(); 
    CPU_ZERO(&cpuset);
    CPU_SET(1, &cpuset);    
    int s = pthread_setaffinity_np(thread_id_scheduler, sizeof(cpu_set_t), &cpuset);
    printf("\ntaskToReadyB %u thread on core %d curtime: %llu\n",pthread_self(),sched_getcpu(),get_current_time());
    int count=0;
    for (auto element : dagMap) 
    {
   		//printf("Entered dagmap: %d\n",count++);
        int gdag = element.first;
        DAGInfo* dinfo = element.second;
        int job= dinfo->jobID;
        int instance= dinfo->instanceID;
        int arrivalHyperperiod = DAGtimeMatrix[job][instance]->arrivalHyperperiod;
        printf("arrivalHyperperiod: %d, cur_hyper_period: %d\n",arrivalHyperperiod,cur_hyper_period);
        if(cur_hyper_period>=arrivalHyperperiod)
        {
        	//printf("Entered loop\n");
	        unsigned long long int arrival_time = DAGtimeMatrix[job][instance]->arrivalTime;
	        KernelLaunchInfo* klinfo;
	        std::vector<KernelLaunchInfo*> klinfo_vector;
	        std::map<pair<int, int>,std::vector<KernelLaunchInfo*>>::iterator iter;
	        

	        iter = taskMap.find(make_pair(gdag,0));
	        if(iter != taskMap.end())
	        {   klinfo_vector = iter->second;
	            klinfo = klinfo_vector[0];
	            //printf("taskToReadyB: Found global dag %d in TaskMap\n",gdag);
	        }
	        else
	        {
	            printf("taskToReadyB: Can not find global dag %d in TaskMap\n",gdag);
	        } 

	        if(get_current_time()<arrival_time)
	        {
	            std::this_thread::sleep_for(std::chrono::microseconds(arrival_time-get_current_time()));
	        }
	        //printf("taskToReadyB: push to ready buffer klinfo->queued %d klinfo->control_mode %d\n",klinfo->queued,klinfo->control_mode);
	        if(CREATE_SUB_KERNEL==0)
	        {
	            if(klinfo->queued==0 && (klinfo->control_mode!=3 || (klinfo->control_mode==3 && get_current_time()>=klinfo->start)))
	            {   
	                fprintf(fp,"\ttaskToReadyB: curTime: %llu ArrivalTime: %llu\n",get_current_time(),DAGtimeMatrix[job][instance]->arrivalTime);
	                printf("\t####taskToReadyB: curTime: %llu ArrivalTime: %llu ####\n",get_current_time(),DAGtimeMatrix[job][instance]->arrivalTime);
	                mtx_rblock.lock();                 
	                klinfo->queued=1;
	                ready_buffer[klinfo->platform_pos][klinfo->device_pos].push_back(klinfo); 
	                dags_arrived++;  
	                mtx_rblock.unlock();
	                cv_rbuffer_available.notify_one();
	            }  
	        }
	        else if (CREATE_SUB_KERNEL==1)
	        {    
	            for(int i=1;i<klinfo_vector.size();i++)
	            {   
	                KernelLaunchInfo* klinfo_p=klinfo_vector[i];
	                if(klinfo_p->queued==0 && (klinfo_p->control_mode!=3 || (klinfo_p->control_mode==3 && get_current_time()>=klinfo_p->start)))
	                {   
	                    fprintf(fp,"\t---taskToReadyB: curTime: %llu ArrivalTime: %llu\n",get_current_time(),DAGtimeMatrix[job][instance]->arrivalTime);
	                    //printf("\t---####taskToReadyB: curTime: %llu ArrivalTime: %llu ####\n",get_current_time(),DAGtimeMatrix[job][instance]->arrivalTime);
	                    mtx_rblock.lock();                 
	                    klinfo_p->queued=1;
	                    ready_buffer[klinfo_p->platform_pos][klinfo_p->device_pos].push_back(klinfo_p); 
	                    dags_arrived++;  
	                    mtx_rblock.unlock();
	                    cv_rbuffer_available.notify_one();
	                }  
	            }

	        }
	        //printf("Exit loop\n");
	    }
    }

    /*
    while(get_current_time()<=hyper_period)
    {
        for(int p=0;p<NumOfPlatforms;p++)
        {
            for(int d=0;d<all_devices[p].size();d++)
            { 
                int pos_index=-1; 
                // printf("\ntask_queue[%d][%d].size(): %d\n",p,d,task_queue[p][d].size());
                for(int q=0;q<task_queue[p][d].size();q++)
                {        
                    int job = task_queue[p][d][q]->task->dagInfo->jobID;
                    int globalDag = task_queue[p][d][q]->task->dagInfo->globalDAGID;
                    int instance = task_queue[p][d][q]->task->dagInfo->instanceID;
                    // printf("\njob %d globalDag %d instance %d",job,globalDag,instance);
                    if(task_queue[p][d][q]->queued==0 && get_current_time() >= DAGtimeMatrix[job][instance]->arrivalTime && (task_queue[p][d][q]->control_mode!=3 || (task_queue[p][d][q]->control_mode==3 && get_current_time()>=task_queue[p][d][q]->start)))
                    {   
                        // printf("\ttaskToReadyB: pos_index: %d curTime: %llu ArrivalTime: %llu\n",pos_index,get_current_time(),DAGtimeMatrix[job][instance]->arrivalTime);
                        fprintf(fp,"\ttaskToReadyB: pos_index: %d curTime: %llu ArrivalTime: %llu\n",q,get_current_time(),DAGtimeMatrix[job][instance]->arrivalTime);

                        pos_index=q;                       
                        
                        break;
                    }
                }  
                
                if(pos_index!=-1)  
                {
                    // while (test_and_set(&rblock,0, 1))
                    //             ;
                    mtx_rblock.lock();                 
                    // fprintf(fp,"\ttaskToReadyB: Moving from waiting to ready queue_%d-%d: %d %d (%d)\n",p,d,ready_buffer[p][d].size(),task_queue[p][d].size(),pos_index);
                    // printf("\ttaskToReadyB: %d-%d: pos_index: %d curTime: %llu size w%d r%d \n",p,d,pos_index,get_current_time(),task_queue[p][d].size(),ready_buffer[p][d].size());
                    task_queue[p][d][pos_index]->queued=1;
                    ready_buffer[p][d].push_back(task_queue[p][d][pos_index]);   
                    // task_queue[p][d].erase(task_queue[p][d].begin()+pos_index); 
                    mtx_rblock.unlock();                    
                    // rblock=0;        
                }
            }
        }
    }
    */
    if (LOG_SCHEDULER >=1){
        fprintf(fp,"taskToReadyB: END: %llu \n",get_current_time());
        //printf("taskToReadyB: END: %llu \n",get_current_time());
        fflush(fp);
    }

}


int chunk_factor(KernelLaunchInfo* kl)
{
    //kl->task->deadline
    return 2;

}



std::vector<cl_command_queue> create_command_queue_for_each(cl_device_id *devs, int num_devs, cl_context ctx) {

    if (LOG_LEVEL >=1)
        fprintf(fp,"create_command_queue_for_each: BEGIN \n");
    

    int i;
    cl_int status;
    std::vector<cl_command_queue> cmd_qs;    
    for (i = 0; i < num_devs; ++i)
    {
        cmd_qs.push_back(clCreateCommandQueue(ctx, devs[i], CL_QUEUE_PROFILING_ENABLE, &status));
        //fprintf(fp,"\nCreateCommandQueue: %u",devs[i]);
        check(status, "\nCreating Command Queue");
    }
    if (LOG_LEVEL >=1)
    {
        fprintf(fp, "\tCreated %d Command Queues.\n", num_devs);
        fprintf(fp,"create_command_queue_for_each: END \n");
    }
    return cmd_qs;
}

std::vector<cl_program> build_kernel_from_info(KernelInfo& ki, const char *info_file_name, std::vector<std::vector<cl_device_id>>& all_devices, std::vector<cl_context>& ctxs) 
{    if (LOG_LEVEL >=1)
        fprintf(fp,"build_kernel_from_info: BEGIN \n");
    cl_device_id* cpu=&(all_devices[1][0]);
    int num_gpus=all_devices[0].size();
    int num_cpus=all_devices[1].size();
    ki = *(assign_kernel_info(info_file_name));
    if (LOG_LEVEL >=1)
    {
        fprintf(fp, "\tCreated Kernel Info Data Structures from %s\n",info_file_name);
        fprintf(fp,"build_kernel_from_info: END \n");
    }
    return build_kernel(ki, all_devices , ctxs);
}

std::vector<cl_program> build_kernel(KernelInfo& ki,  std::vector<std::vector<cl_device_id>>& all_devices, std::vector<cl_context>& ctxs) 
{
    if (LOG_LEVEL >=1)
        fprintf(fp,"build_kernel: BEGIN \n");

    cl_int status;   
    cl_device_id* gpu=&(all_devices[PLATFORM_GPU][0]);
    cl_device_id* cpu=&(all_devices[PLATFORM_CPU][0]);
    int num_gpus=all_devices[PLATFORM_GPU].size();
    int num_cpus=all_devices[PLATFORM_CPU].size();


    // printf("0: %u, 1: %u",ctxs[0],ctxs[1]);
    std::vector<cl_program> programs(NumOfPlatforms);
    std::string kernel_file = ki.kernelSource;
    std::string kernel_file_bin_cpu = "src/"+kernel_file.erase(ki.kernelSource.length()-3)+"_cpu.bin";
    std::string kernel_file_bin_gpu = "src/"+kernel_file.erase(ki.kernelSource.length()-3)+"_gpu.bin";
    if(!file_exists(kernel_file_bin_cpu) && !file_exists(kernel_file_bin_gpu))
    {
        //printf("Before Compiled program %s size %d gpu %u cpu %u \n",ki.kernelSource.c_str(), programs.size(), programs[PLATFORM_GPU] ,programs[PLATFORM_CPU]);
        programs[PLATFORM_GPU] = cl_compile_program(ki.kernelSource.c_str(), ctxs[PLATFORM_GPU], PLATFORM_GPU);
        programs[PLATFORM_CPU] = cl_compile_program(ki.kernelSource.c_str(), ctxs[PLATFORM_CPU],PLATFORM_CPU);
        //printf("Compiled program %s size %d gpu %u cpu %u \n",ki.kernelSource.c_str(), programs.size(), programs[PLATFORM_GPU] ,programs[PLATFORM_CPU]);
        if (LOG_LEVEL >=1)
        {
            fprintf(fp, "\tComplied Programs: %s\n",ki.kernelSource.c_str());
        }
        // printf("\tComplied Programs: %s\n",ki.kernelSource.c_str());
        //string options="-g";

        status = clBuildProgram(programs[PLATFORM_GPU], num_gpus, gpu, ki.options.c_str(), NULL, NULL);

        if(status==CL_SUCCESS)
        {   fprintf(fp," CL_SUCCESS\n");
            //printf(" CL_SUCCESS in clBuildProgram GPU\n");
        }
        else
            printf(" Failed in clBuildProgram in GPU %d \n",status);
        

        check(status, "Failed in building program GPU: ");
        // printf("Building Program GPU\n");


        // if(status != CL_SUCCESS){   
        //     size_t len = 0;      
        //     clBuildProgram(programs[0], num_gpus, gpu, ki.options.c_str(), NULL, NULL);
        //     clGetProgramBuildInfo(programs[0], deviceId, CL_PROGRAM_BUILD_LOG, 0, NULL, &len);  
        //     char *buffer = calloc(len, sizeof(char));                                    //     ret = clGetProgramBuildInfo(program, deviceId, CL_PROGRAM_BUILD_LOG, len, buffer, NULL);                                                                      //     fprintf(fp, " %s\n", buffer);                                                // }       
        //printf("status %d Before Building Program CPU num_cpus %d cpu %u gpu %u , %s \n",status,num_cpus, cpu ,gpu, ki.options.c_str());
        
        status = clBuildProgram(programs[PLATFORM_CPU], num_cpus, cpu, ki.options.c_str(), NULL, NULL);
        
        if(status==CL_SUCCESS)
        {   fprintf(fp," CL_SUCCESS\n");
            //printf(" CL_SUCCESS in clBuildProgram in CPU\n");
        }
        else
            printf(" failed in clBuildProgram in CPU %d \n",status);

        check(status, "Failed in building program CPU: ");

        // printf("Building Program CPU\n");

        if (LOG_LEVEL >=1)
        {
            fprintf(fp,"\tBuilding %s program\n",ki.KernelName.c_str());
        }
        
       // write_binaries(kernel_file_bin_cpu,programs[PLATFORM_CPU],num_cpus,PLATFORM_CPU);
       // write_binaries(kernel_file_bin_gpu,programs[PLATFORM_GPU],num_gpus,PLATFORM_GPU);
       // printf("Building Program CPU done\n");
    }
    
    ki.kernelObjects.push_back(clCreateKernel(programs[PLATFORM_GPU], ki.KernelName.c_str(), &status));
    check(status, "\nCreating Kernel GPU\n");
    ki.kernelObjects.push_back(clCreateKernel(programs[PLATFORM_CPU], ki.KernelName.c_str(), &status));
    check(status, "\nCreating Kernel CPU\n");
    if (LOG_LEVEL >=1)
    {
        fprintf(fp,"build_kernel: END \n");
    }    
    // printf("build_kernel: END \n");
    return programs;
}

cl_program cl_compile_program(const char* kernel_file_name, cl_context ctx, int platform_type) 
{
    if (LOG_LEVEL >=1)
    {
        fprintf(fp,"cl_compile_program: BEGIN \n");
    }
    cl_int status;
    char ksrc[100] = "src/";
    std::ifstream f(strcat(ksrc, kernel_file_name));
    std::stringstream sbuffer;
    sbuffer << f.rdbuf();
    std::string kernel_file_src = sbuffer.str();
    const char* program_src = kernel_file_src.c_str();
    
    
    //printf("####################cl_compile_program: platform_type: %d -> %u , %u\n",platform_type,ctx,all_ctxs[platform_type]);
    

    cl_program program = clCreateProgramWithSource(ctx, 1, (const char**)&program_src, NULL, &status);
    
    if(status==CL_SUCCESS)
    {    fprintf(fp,"clCreateProgramWithSource CL_SUCCESS\n");
         //printf("#########clCreateProgramWithSource CL_SUCCESS\n");
    }
    else if(status==CL_INVALID_CONTEXT)
        fprintf(fp,"clCreateProgramWithSource Error: CL_INVALID_CONTEXT\n");
    else if(status==CL_INVALID_VALUE)
        fprintf(fp,"clCreateProgramWithSource Error: CL_INVALID_VALUE\n");
    else if(status==CL_OUT_OF_HOST_MEMORY)
        fprintf(fp,"clCreateProgramWithSource Error: CL_OUT_OF_HOST_MEMORY\n");

    check(status, "Creating Program With Source");
    if (LOG_LEVEL >=1)
    {
        fprintf(fp,"cl_compile_program: END \n");
    }

    return program;

}

KernelInfo* assign_kernel_info(const char * info_file_name) {
    if (LOG_LEVEL >=1)
    {    
        fprintf(fp,"assign_kernel_info: BEGIN \n");
        fprintf(fp,"\tKERNEL INFORMATION: %s\n",info_file_name);
    }
    //printf("Opening file %s\n",info_file_name);
    FILE* kernel_info_file = fopen(info_file_name, "r");
    KernelInfo* kernel_info = new KernelInfo();
    char line[1024];
    int length, i, is_buffer, gpu_index, cpu_index, index;
    char **list, **sublist;
    kernel_info->configured[PLATFORM_CPU]=0;
    kernel_info->configured[PLATFORM_GPU]=0;
    if (!kernel_info_file) 
    {
        fprintf(fp,"\tCan't open %s file.\n",info_file_name);
        if(kernel_info_file==NULL)
            fprintf(fp,"\tFile in empty\n");
        exit(EXIT_FAILURE);
    }
    else if (kernel_info_file != NULL) 
    {
        kernel_info -> localWorkSize[0] = 0;
        while (fgets(line, sizeof(line), kernel_info_file) != NULL)
        {
            //fprintf(fp,"\nassign_kernel_info: %s",line);
            length = strlen(line);
            if(length > 1) 
            {                
                if(line[length-1] == '\n')
                    line[length-1]='\0';
                else
                    line[length]='\0';
                list = strsplit(line, "=");
                splitstring linestring(line);                         
                vector <string> slist = linestring.split('=');
                
                if (!strcmp(slist[0].c_str(), "KernelName")) 
                {                    
                    (kernel_info -> KernelName).assign(slist[1].c_str());
                }
                else if (!strcmp(slist[0].c_str(), "KernelSource")) 
                {
                    (kernel_info -> kernelSource).assign(slist[1].c_str());
                }
                else if (!strcmp(slist[0].c_str(), "workDimension")) 
                {
                    kernel_info -> workDimension = atoi(slist[1].c_str());   
                }
                else if (!strcmp(slist[0].c_str(), "globalWorkSize")) 
                {                    
                  
                    splitstring liststring(str_to_char_array(slist[1]));
                    vector <string> ssublist = liststring.split(',');
                       
                    for (i = 0; i < 3; ++i) 
                    {
                        kernel_info -> globalWorkSize[i] = 1;
                    }
                    for(i = 0;i<ssublist.size(); ++i) 
                    {         
                        kernel_info -> globalWorkSize[i] = atoi(ssublist[i].c_str());
                    }
                }
                else if (!strcmp(slist[0].c_str(), "localWorkSize")) 
                {
                    splitstring liststring(str_to_char_array(slist[1]));
                    vector<string> ssublist=liststring.split(',');
                    for (i = 0; i < 3; ++i) 
                    {
                            kernel_info -> localWorkSize[i] = 0;
                    }
                    for(i = 0;i<ssublist.size(); ++i) 
                    {         
                            kernel_info -> localWorkSize[i] = atoi(ssublist[i].c_str());
                    }
                }
                else if (!strcmp(slist[0].c_str(), "inputBuffers")) 
                {
                    splitstring liststring(str_to_char_array(slist[1]));
                    vector<string> ssublist=liststring.split(',');
                    //printf("Updating input buffer information\n");
                    
                    for (i = 0; i<ssublist.size(); i+=4) 
                    {
                        (kernel_info -> inputBuffers).push_back(std::tuple<std::string, unsigned int, unsigned int, unsigned int>(ssublist[i], atoi(ssublist[i+1].c_str()), atoi(ssublist[i+2].c_str()), atoi(ssublist[i+3].c_str())));   
                    }
                    kernel_info -> noInputBuffers = (kernel_info -> inputBuffers).size();
                    
                }
                else if (!strcmp(slist[0].c_str(), "outputBuffers")) 
                {
                    splitstring liststring(str_to_char_array(slist[1]));
                    vector<string> ssublist=liststring.split(',');
                    //printf("Updating output buffer information\n");
                    
                    for (i = 0; i<ssublist.size(); i+=4) 
                    {
                        (kernel_info -> outputBuffers).push_back(std::tuple<std::string, unsigned int, unsigned int, unsigned int>(ssublist[i], atoi(ssublist[i+1].c_str()), atoi(ssublist[i+2].c_str()), atoi(ssublist[i+3].c_str()) ));   
                    }
                    kernel_info -> noOutputBuffers = (kernel_info -> outputBuffers).size();   
                    // printf("outputBuffers size: %d : %d\n",kernel_info -> noOutputBuffers,kernel_info->outputBuffers.size());
                    
                }
                else if (!strcmp(slist[0].c_str(), "ioBuffers")) 
                {
                    splitstring liststring(str_to_char_array(slist[1]));
                    vector<string> ssublist=liststring.split(',');                    
                    printf("Updating input output buffer information\n");

                    for (i = 0; i<ssublist.size(); i+=4) 
                    {
                        (kernel_info -> ioBuffers).push_back(std::tuple<std::string, unsigned int, unsigned int, unsigned int>(ssublist[i], atoi(ssublist[i+1].c_str()), atoi(ssublist[i+2].c_str()), atoi(ssublist[i+3].c_str()) ));   
                    }
                    kernel_info -> noIOBuffers = (kernel_info -> ioBuffers).size();   
                    
                }
                else if (!strcmp(slist[0].c_str(), "varArguments")) 
                {
                    splitstring liststring(str_to_char_array(slist[1]));
                    vector<string> ssublist=liststring.split(',');
  
                    for (i = 0; i<ssublist.size(); i+=3) 
                    {
                        //index = store_variable_kernel_arg(str_to_char_array(ssublist[i]), str_to_char_array(ssublist[i+1]));
                        //(kernel_info -> varArguments).push_back(std::tuple<std::string, unsigned int, unsigned int>(ssublist[i], index, atoi(ssublist[i+2].c_str())));   
                        (kernel_info -> varArguments).push_back(std::tuple<std::string, unsigned int, unsigned int>(ssublist[i], atoi(ssublist[i+1].c_str()), atoi(ssublist[i+2].c_str())));
                    }
 
                }
                else if (!strcmp(slist[0].c_str(), "localArguments")) 
                {
                    splitstring liststring(str_to_char_array(slist[1]));
                    vector<string> ssublist=liststring.split(',');
                    
                    for (i = 0; i<ssublist.size(); i+=3) 
                    {
                        (kernel_info -> localArguments).push_back(std::tuple<std::string, unsigned int, unsigned int>(ssublist[i], atoi(ssublist[i+1].c_str()), atoi(ssublist[i+2].c_str())));   
                    }
                }
                else if (!strcmp(slist[0].c_str(), "data_outflow")) 
                {
                    splitstring liststring(str_to_char_array(slist[1]));
                    vector<string> ssublist=liststring.split(',');
                    
                    for (i = 0; i<ssublist.size(); i+=3) 
                    {
                        (kernel_info -> data_outflow).push_back(std::tuple<unsigned int, unsigned int, unsigned int>(atoi(ssublist[i].c_str()), atoi(ssublist[i+1].c_str()), atoi(ssublist[i+2].c_str())));   
                    }
                }
                else if (!strcmp(slist[0].c_str(), "nonPartition")) // buffers thatc an not be prtitioned
                {                    
                  
                    splitstring liststring(str_to_char_array(slist[1]));
                    vector <string> ssublist = liststring.split(',');
                       
                    for(i = 0;i<ssublist.size(); ++i) 
                    {         
                        (kernel_info -> nonPartition).push_back(atoi(ssublist[i].c_str()));
                    }
                }
                else if (!strcmp(slist[0].c_str(), "options"))
                {                    
                  
                    (kernel_info -> options).assign(slist[1]+"="+slist[2]+"="+slist[3]+"="+slist[4]+"="+slist[5]+"="+slist[6]+"="+slist[7]+"="+slist[8]);
                    printf("slist: %s\n",kernel_info -> options.c_str());
                }
                else if (!strcmp(slist[0].c_str(), "FLOPS_per_thread"))
                {                    
                  
                    kernel_info -> FLOPS_per_thread = atoi(slist[1].c_str()); 
                }
            }
            
        }
    }

    if (LOG_LEVEL >=1)
    {    
        fprintf(fp,"assign_kernel_info: END \n");        
    }
    return kernel_info;
}

void reset_nonpersistent_host_arrays(KernelInfo& ki, std::vector<void*>& data)
{
    for (int i = 0; i < ki.inputBuffers.size(); ++i)
    {
        int persistent = std::get<3>(ki.inputBuffers.at(i));
        std::string type =std::get<0>(ki.inputBuffers.at(i));
        int size = std::get<1>(ki.inputBuffers.at(i));
        if (!persistent && !READ_INPUT_DATA)
            array_randomize(data[i], type, size);
        if (!persistent && READ_INPUT_DATA)
        {    
            void* input;
            array_input(data.back(), type, size,input);
        }
    }   
}

void host_array_initialize(KernelInfo& ki, std::vector<void*>& data) {

    if (LOG_LEVEL >=1)
    {    
        fprintf(fp,"host_array_initialize: BEGIN\n");
    }

    int i;
    std::string type; 
    int size;
    for (i = 0; i < ki.inputBuffers.size(); ++i)
    {
        type = std::get<0>(ki.inputBuffers.at(i));
        size = std::get<1>(ki.inputBuffers.at(i));
        data.push_back(array_allocate(type, size));
        if(!READ_INPUT_DATA)
            array_randomize(data.back(), type, size);
        else
        {    
            void* input;
            array_input(data.back(), type, size,input);
        }
    }
    if (LOG_LEVEL >=1)
    {
        fprintf(fp, "\tInitialised and populated Input host array.\n");
    }
    for (i = 0; i < ki.outputBuffers.size(); ++i)
    {
        type = std::get<0>(ki.outputBuffers.at(i));
        size = std::get<1>(ki.outputBuffers.at(i));
        data.push_back(array_allocate(type, size));
        output_initialize(data.back(), type, size);
    }
    //printf("data size: %d\n",data.size());
    if (LOG_LEVEL >=1)
    {    
        fprintf(fp,"host_array_initialize: END \n");
    }
}

void array_randomize(void* data, std::string type, int size) {

    if (LOG_LEVEL >=1)
    {    
        fprintf(fp,"array_randomize: BEGIN \n");
    }

    int i;
    if( type.compare("int") == 0) 
    {
        int* A;
        A = (int*)data;
        for (i = 0; i < size; ++i)
        {
            A[i] = rand() % 5000 - 2500;
            //A[i] = i;
        }
    }
    else if(type.compare("uint") == 0) 
    {
        uint* A;
        A = (uint*)data;
        for (i = 0; i < size; ++i)
        {
            A[i] = rand() % 5000;
        }
    }
    else if(type.compare("short") == 0) 
    {
        short* A;
        A = (short*)data;
        for (i = 0; i < size; ++i)
        {
            A[i] = rand() % 5000;
        }
    }
    else if(type.compare("ushort") == 0) 
    {
        ushort* A;
        A = (ushort*)data;
        for (i = 0; i < size; ++i)
        {
            A[i] = rand() % 5000;
        }
    }
    else if(type.compare("long") == 0) 
    {
        long* A;
        A = (long*)data;
        for (i = 0; i < size; ++i)
        {
            A[i] = rand() % 5000 - 2500;
        }
    }
    else if(type.compare("ulong") == 0) 
    {
        ulong* A;
        A = (ulong*)data;
        for (i = 0; i < size; ++i)
        {
            A[i] = rand() % 5000;
        }
    }
    else if(type.compare("float") == 0) 
    {
        float* A;
        A = (float*)data;
        for (i = 0; i < size; ++i)
        {
            A[i] = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/500)) - 250.0;
        }
    }
    else if(type.compare("double") == 0) 
    {
        double* A;
        A = (double*)data;
        for (i = 0; i < size; ++i)
        {
            A[i] = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/500)) -250.0;
        }
    }
    else if(type.compare("char") == 0) 
    {
        char* A;
        A = (char*)data;
        for (i = 0; i < size; ++i)
        {
            A[i] = 32 + rand() % 91;
        }
    }
    else {
        fprintf(fp, "\tCannot find the required variable host array datatype %s\n", type.c_str());
        exit(-37);
    }

    if (LOG_LEVEL >=1)
    {    
        fprintf(fp,"array_randomize: END\n");
    }
}

void array_input(void* data, std::string type, int size,void* input) {

    if (LOG_LEVEL >=1)
    {    
        fprintf(fp,"array_randomize: BEGIN \n");
    }

    int i;
    if( type.compare("int") == 0) 
    {
        int* A;
        A = (int*)data;
        for (i = 0; i < size; ++i)
        {
            //A[i] = rand() % 5000 - 2500;
            A[i] = i;
        }
    }
    else if(type.compare("uint") == 0) 
    {
        uint* A;
        A = (uint*)data;
        for (i = 0; i < size; ++i)
        {
            A[i] = i;
        }
    }
    else if(type.compare("short") == 0) 
    {
        short* A;
        A = (short*)data;
        for (i = 0; i < size; ++i)
        {
            A[i] = i;
        }
    }
    else if(type.compare("ushort") == 0) 
    {
        ushort* A;
        A = (ushort*)data;
        for (i = 0; i < size; ++i)
        {
            A[i] = i;
        }
    }
    else if(type.compare("long") == 0) 
    {
        long* A;
        A = (long*)data;
        for (i = 0; i < size; ++i)
        {
            A[i] = i;
        }
    }
    else if(type.compare("ulong") == 0) 
    {
        ulong* A;
        A = (ulong*)data;
        for (i = 0; i < size; ++i)
        {
            A[i] = i;
        }
    }
    else if(type.compare("float") == 0) 
    {
        float* A;
        A = (float*)data;
        for (i = 0; i < size; ++i)
        {
            A[i] = static_cast <float> (i);
        }
    }
    else if(type.compare("double") == 0) 
    {
        double* A;
        A = (double*)data;
        for (i = 0; i < size; ++i)
        {
            A[i] = static_cast <float> (i);
        }
    }
    else if(type.compare("char") == 0) 
    {
        char* A;
        A = (char*)data;
        for (i = 0; i < size; ++i)
        {
            A[i] = i;
        }
    }
    else {
        fprintf(fp, "\tCannot find the required variable host array datatype %s\n", type.c_str());
        exit(-37);
    }

    if (LOG_LEVEL >=1)
    {    
        fprintf(fp,"array_randomize: END\n");
    }
}

void output_initialize(void* data, std::string type, int size)
{
    
    if (LOG_LEVEL >=1)
    {    
        fprintf(fp,"output_initialize: BEGIN \n");
    }
    int i;
    if( type.compare("int") == 0) 
    {
        int* A;
        A = (int*)data;
        for (i = 0; i < size; ++i)
        {
            // A[i] = rand() % 5000 - 2500;
            A[i] = 0;
        }
    }
    else if(type.compare("uint") == 0) 
    {
        uint* A;
        A = (uint*)data;
        for (i = 0; i < size; ++i)
        {
            A[i] = 0;
        }
    }
    else if(type.compare("short") == 0) 
    {
        short* A;
        A = (short*)data;
        for (i = 0; i < size; ++i)
        {
            A[i] = 0;
        }
    }
    else if(type.compare("ushort") == 0) 
    {
        ushort* A;
        A = (ushort*)data;
        for (i = 0; i < size; ++i)
        {
            A[i] = 0;
        }
    }
    else if(type.compare("long") == 0) 
    {
        long* A;
        A = (long*)data;
        for (i = 0; i < size; ++i)
        {
            A[i] = 0;
        }
    }
    else if(type.compare("ulong") == 0) 
    {
        ulong* A;
        A = (ulong*)data;
        for (i = 0; i < size; ++i)
        {
            A[i] = 0;
        }
    }
    else if(type.compare("float") == 0) 
    {
        float* A;
        A = (float*)data;
        for (i = 0; i < size; ++i)
        {
            A[i] = 0.0;
        }
    }
    else if(type.compare("double") == 0) 
    {
        double* A;
        A = (double*)data;
        for (i = 0; i < size; ++i)
        {
            A[i] = 0.0;
        }
    }
    else if(type.compare("char") == 0) 
    {
        char* A;
        A = (char*)data;
        for (i = 0; i < size; ++i)
        {
            A[i] = 32;
        }
    }
    else {
        fprintf(fp, "\tCannot find the required variable host array datatype %s\n", type.c_str());
        exit(-37);
    }
    if (LOG_LEVEL >=1)
    {    
        fprintf(fp,"output_initialize: END \n");
    }

}

cl_event dispatch(KernelLaunchInfo& kl_info ) {
    // while (test_and_set(&nlock,0, 1))
    //         ;
    mtx_nlock.lock(); 
    nKernels++;
    if(!CREATE_SUB_KERNEL) //nTasks <=number of tasks in trace file
        nTasks++;
    else if(kl_info.full_task_klinfo->not_done_count==kl_info.full_task_klinfo->size)
        nTasks++;
    
    mtx_nlock.unlock();    

    mtx_devlock.lock();         
    deviceAvailability[kl_info.platform_pos][kl_info.device_pos]=0;
    dev_available[kl_info.platform_pos][kl_info.device_pos]=false;
    mtx_devlock.unlock();    


    if(kl_info.task->traceID==-1)
        no_micro_kernel++;
        
    kl_info.kex.rel_start_time=get_current_time();
    struct timeval start,end;  
    gettimeofday(&start,NULL); 
    unsigned long long int  start_time_launch=(unsigned long long int )(start.tv_sec*1000000+start.tv_usec);
    // kl_info.kex.start_time=start_time_launch;
    PERSISTENCE_DEBUG printf("dispatch %d: BEGIN: %llu\n",kl_info.task->traceID,get_current_time());
    // kl_info.finished=0;
    // printf("DISPATCHING\n");
    
    
    if (LOG_SCHEDULER >=1 || LOG_LEVEL>=1)
        fprintf(fp,"dispatch: BEGIN: %llu \n",get_current_time());
    
    int i;
    int index=0;
    
    cl_int err;
    cl_event ev,barrier_ev_write,barrier_ev_read;
    cl_ulong g,c;
    cl_int status;
    string devn;   
    
    unsigned int datasize=kl_info.size;
    unsigned int dataoffset=kl_info.offset;  
    int platform=kl_info.platform_pos;
    int device=kl_info.device_pos;

    if(kl_info.priority==0)
        change_frequency(kl_info.frequency,  platform,  device);
    else if(SAFE && kl_info.priority==1)
    {   change_frequency(deviceSpec[platform][device]->highFrequencyBound, platform,  device);
        printf("\t+++SAFE MODE ON for d-%d i-%d +++\n",kl_info.task->dagInfo->jobID,kl_info.task->dagInfo->instanceID);

        DAGtimeMatrix[kl_info.task->dagInfo->jobID][kl_info.task->dagInfo->instanceID]->safe_mode++;
    }



    if(kl_info.recovery_mode==1) //frequency
    {
        change_frequency(kl_info.tmp_frequency,  kl_info.platform_pos,  kl_info.device_pos);
        printf("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&recovery_mode enetred: Frequency tuned: %llu\n",kl_info.tmp_frequency);
    }
    else if(kl_info.recovery_mode==2) //migration
    {
        platform=kl_info.tmp_platform_pos;
        device=kl_info.tmp_device_pos;
        printf("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&recovery_mode enetred: Migration to: %d-%d\n",platform,device);
    }
    else if(kl_info.recovery_mode==3) //idle slot
    {
        std::this_thread::sleep_for(std::chrono::microseconds(kl_info.tmp_idle_slot));
        printf("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&recovery_mode enetred: idleslot: %u\n",kl_info.tmp_idle_slot);

    }
    




    cl_context ctx=all_ctxs[kl_info.platform_pos];
    cl_command_queue cmd_q=all_cmd_qs[kl_info.platform_pos][kl_info.device_pos];
    //clFlush(cmd_q);
    cl_device_id device_id=all_devices[kl_info.platform_pos][kl_info.device_pos];
    ev = clCreateUserEvent(ctx, NULL);

      
    // printf("Device available: %d\n",device_available);
    if (LOG_LEVEL >=1){
        fprintf(fp,"\tStarting kernel %d:%s on %s device %d-%d (id-%u) at time %llu\n",kl_info.task->traceID,kl_info.task->taskID.c_str(),devn.c_str(),kl_info.platform_pos,kl_info.device_pos, device_id,get_current_time());
    }
    
    // printf("\nSize: %d Node: %d\n",kl_info.task->nodes.size(),kl_info.task->nodes[0]);
    if (std::find(kl_info.task->nodes.begin(), kl_info.task->nodes.end(), 0) != kl_info.task->nodes.end()){
        DAGtimeMatrix[kl_info.task->dagInfo->jobID][kl_info.task->dagInfo->instanceID]->startTime=get_current_time();
    }

    unsigned long long int curTime=get_current_time();
    // printf("curTime: %llu\n",curTime);
    // if(curTime+kl_info.task->exTime>kl_info.task->deadline)
    //     kl_info.frequency=deviceSpec[kl_info.device_index].highFrequencyBound;
    // else
    //     kl_info.frequency=deviceSpec[kl_info.device_index].lowFrequencyBound;
    outputbuffer+="Kernel_D: "+std::to_string(kl_info.task->traceID)+kl_info.task->taskID+" start_time_launch: "+std::to_string(curTime)+" Frequency: "+std::to_string(kl_info.frequency)+"\n";
    //" ReadybufferSize: "+std::to_string(ready_buffer[kl_info.platform_pos][kl_info.device_pos].size())+"Waitingqueuesize: "+std::to_string(waiting_queue[kl_info.platform_pos][kl_info.device_pos].size())+"\n";
    //printf("%s",outputbuffer.c_str());
    // printf("\nstart_time_launch: %llu WExT: %f Deadline:%f Frequency:%u\n",curTime,kl_info.task->exTime,kl_info.task->deadline,kl_info.frequency);
    // 
    // print_launch_info(kl_info);    
    
    
    // printf("Freq: %u\n",kl_info.frequency);
    // kl_info.priority=0;

    

    // printf("before create buffer\n");
    //Creating buffers for device
    KernelInfo *kernel_info = kl_info.task->kernels[index];
    PERSISTENCE_DEBUG printf("Configured kernel status: %d\n",kernel_info->configured[kl_info.platform_pos]);
    if (!kernel_info->configured[kl_info.platform_pos])
    {    cl_create_buffers(ctx, (KernelInfo&)*(kl_info.task->kernels[index]), kl_info.io, kl_info.task->data, datasize,dataoffset,kl_info.platform_pos);
        kl_info.released_b=0;
    }
    //printf("after create buffer index: %d kl_info.task->kernels size %d \n",index, kl_info.task->kernels.size());

    
    //setting kernel arguments
    cl_set_kernel_args((KernelInfo&)*(kl_info.task->kernels[index]), kl_info.io, kl_info.platform_pos,datasize);     
    
    //printf("cl_set_kernel_args\n");

    kl_info.kex.write_start_h=get_current_time();


    if(LOG_PROFILE)    
    {
        Temperature* tmp=new Temperature();
        tmp->sensors.push_back(0);tmp->sensors.push_back(0);tmp->sensors.push_back(0);tmp->sensors.push_back(0);tmp->sensors.push_back(0);
        tempMap.insert( pair<unsigned long long int, Temperature*>(get_current_time(), tmp) );  
    }

    PERSISTENCE_DEBUG printf("clEnqueueBuffer with datasize %d\n",datasize);
    kl_info.ke.write = cl_enqueue_write_buffers(&(kl_info.kex),cmd_q, *(kl_info.task->kernels[index]), kl_info.io, kl_info.task->data, datasize, dataoffset, ev,kl_info.platform_pos);
    // status = clEnqueueBarrierWithWaitList ( cmd_q ,kl_info.task->kernels[index]->noInputBuffers ,&(kl_info.ke.write[0]) ,&barrier_ev_write );
    // kl_info.ke.barrier_write = barrier_ev_write;
    clSetUserEventStatus(ev, CL_COMPLETE); 
    clFlush(cmd_q);

    kl_info.kex.nd_start_h=get_current_time();
    PERSISTENCE_DEBUG printf("clEnqueueNDRangeKernel with datasize %d\n",datasize);
    kl_info.ke.exec = cl_enqueue_nd_range_kernel(&(kl_info.kex),cmd_q, *(kl_info.task->kernels[index]), kl_info.platform_pos, datasize,kl_info.ke.write.back());//kl_info.ke.write.back());
    
    clFlush(cmd_q);
    PERSISTENCE_DEBUG printf("clEnqueueReadBuffer with datasize %d\n",datasize);
    kl_info.kex.read_start_h=get_current_time();
    kl_info.ke.read = cl_enqueue_read_buffers(&(kl_info.kex),cmd_q, *(kl_info.task->kernels[index]),kl_info.io, kl_info.task->data, datasize, dataoffset,kl_info.ke.exec,kl_info.platform_pos);
    
   cl_int status1=clFlush(cmd_q);
    // status = clEnqueueBarrierWithWaitList ( cmd_q ,kl_info.task->kernels[index]->outputBuffers.size(),&(kl_info.ke.read[0]) ,&barrier_ev_read );    
    // kl_info.ke.barrier_read = barrier_ev_read;
    //   if(status1==CL_SUCCESS)
    //        printf("Command queue [%d][%d] flushed: !!\n",kl_info.platform_pos,kl_info.device_pos);
    //    else
    //        printf("Command queue [%d][%d] not flushed: !!\n",kl_info.platform_pos,kl_info.device_pos);
    //status = clSetEventCallback(kl_info.ke.read[kl_info.ke.read.size()-1], CL_COMPLETE, &notify_callback_update_release, (void*)&kl_info);
    PERSISTENCE_DEBUG printf("clSetEventCallback %d\n",datasize);
    status = clSetEventCallback(kl_info.ke.read.back(), CL_COMPLETE, &notify_callback_update_release, (void*)&kl_info);    
    PERSISTENCE_DEBUG printf("Settting event callback done\n");
    clFlush(cmd_q);

    gettimeofday(&end,NULL);
    unsigned long long int  end_time=(unsigned long long int )(end.tv_sec*1000000+end.tv_usec);
    unsigned long long int timing =end_time-start_time_launch;
    kl_info.kex.dispatchTime=timing;
    // kl_info.kex.first_event.push_back(kl_info.ke.write.front());
    // kl_info.kex.last_event.push_back(kl_info.ke.read.back());
    //gettimeofday(&ki.hostTime, NULL);
    

    if (LOG_LEVEL >=1){
        fprintf(fp,"\tTotal time taken for dispatching %llu micro seconds\n", (unsigned long long int )(timing));
    }
    // printf("\tTotal time taken for dispatching %d trace kernel %llu micro seconds\n", kl_info.task->traceID, (unsigned long long int )(timing));
    if (LOG_SCHEDULER >=1 || LOG_LEVEL>=1){
        fprintf(fp,"dispatch: END: %llu \n",get_current_time());
        fflush(fp);
    }
    PERSISTENCE_DEBUG printf("dispatch %d: END: %llu\n",kl_info.task->traceID,get_current_time());
    if(isProfileMode)
        kernel_info->configured[kl_info.platform_pos]=1;
    return kl_info.ke.read.back();
}

unsigned int calculate_ip_buffer_size(unsigned int size,int ip_index,KernelInfo& ki){

    if ( LOG_LEVEL>=1)
        fprintf(fp,"calculate_ip_buffer_size: BEGIN: %llu \n",get_current_time());

    unsigned int buffersize;
    int flag=0;
    for(int j=0;j<ki.nonPartition.size();j++)
    {    if(std::get<2>(ki.inputBuffers.at(ip_index)) == ki.nonPartition[j])
        {
            buffersize = get_sizeof( std::get<0>(ki.inputBuffers.at(ip_index))) *  std::get<1>(ki.inputBuffers.at(ip_index));
            flag=1;
            //printf("calculate_ip_buffer_size: for non partitioned buffer: %u\n",buffersize);
        }
    }
    if(flag==0) //Buffers that can be partitioned
    {   
        //buffersize = get_sizeof( std::get<0>(ki.inputBuffers.at(ip_index))) *  std::get<1>(ki.inputBuffers.at(ip_index)) * ((float)size/(ki.globalWorkSize[0]*ki.globalWorkSize[1]*ki.globalWorkSize[2])); 
        buffersize = (unsigned int)(get_sizeof( std::get<0>(ki.inputBuffers.at(ip_index))) *  std::get<1>(ki.inputBuffers.at(ip_index)) * ((float)size/(ki.globalWorkSize[0]*ki.globalWorkSize[1]*ki.globalWorkSize[2]))); 
        //printf("calculate_ip_buffer_size: for partitioned buffer %u * %u * (%u/%u)%f = %u\n",get_sizeof( std::get<0>(ki.inputBuffers.at(ip_index))),std::get<1>(ki.inputBuffers.at(ip_index)),size,(ki.globalWorkSize[0]*ki.globalWorkSize[1]*ki.globalWorkSize[2]),((float)size/(ki.globalWorkSize[0]*ki.globalWorkSize[1]*ki.globalWorkSize[2])),buffersize);
    }
    
    if (LOG_LEVEL>=1){
        fprintf(fp,"calculate_ip_buffer_size: END: %llu \n",get_current_time());
        fflush(fp);
    }
    return buffersize;
}
        
unsigned int calculate_ip_buffer_offest(unsigned int dataoffset,int ip_index,KernelInfo& ki){

    if (LOG_LEVEL>=1)
        fprintf(fp,"calculate_ip_buffer_offest: BEGIN: %llu \n",get_current_time());

    unsigned int bufferoffset;int flag=0;
    for(int j=0;j<ki.nonPartition.size();j++)
        if(std::get<2>(ki.inputBuffers.at(ip_index)) == ki.nonPartition[j]){
            bufferoffset=0;
            flag=1;
        }
    if(flag==0)
         bufferoffset = get_sizeof(std::get<0>(ki.inputBuffers.at(ip_index))) * dataoffset * (std::get<1>(ki.inputBuffers.at(ip_index)) / (ki.globalWorkSize[0]*ki.globalWorkSize[1]*ki.globalWorkSize[2]));          
    
    if (LOG_LEVEL>=1){
        fprintf(fp,"calculate_ip_buffer_offest: END: %llu \n",get_current_time());
        fflush(fp);
    }

    return bufferoffset;
}

unsigned int calculate_op_buffer_size(unsigned int size,int op_index,KernelInfo& ki){

    if (LOG_LEVEL>=1)
        fprintf(fp,"calculate_op_buffer_size: BEGIN: %llu \n",get_current_time());

    unsigned int buffersize;int flag=0;
    for(int j=0;j<ki.nonPartition.size();j++)
        if(std::get<2>(ki.outputBuffers.at(op_index)) == ki.nonPartition[j]){
            buffersize = get_sizeof( std::get<0>(ki.outputBuffers.at(op_index))) *  std::get<1>(ki.outputBuffers.at(op_index));
            flag=1;
        }
    if(flag==0)
        buffersize = get_sizeof( std::get<0>(ki.outputBuffers.at(op_index))) *  (std::get<1>(ki.outputBuffers.at(op_index)) * ((float)size/(ki.globalWorkSize[0]*ki.globalWorkSize[1]*ki.globalWorkSize[2]))); 
    
    if (LOG_LEVEL>=1){
        fprintf(fp,"calculate_op_buffer_size: END: %llu \n",get_current_time());
        fflush(fp);
    }

    return buffersize;
}

unsigned int calculate_op_buffer_offset(unsigned int dataoffset,int op_index,KernelInfo& ki){

    if (LOG_LEVEL>=1)
        fprintf(fp,"calculate_op_buffer_offset: BEGIN: %llu \n",get_current_time());


    unsigned int bufferoffset;int flag=0;
    for(int j=0;j<ki.nonPartition.size();j++)
        if(std::get<2>(ki.outputBuffers.at(op_index)) == ki.nonPartition[j]){
            bufferoffset=0;
            flag=1;
        }
    if(flag==0)
        bufferoffset = get_sizeof(std::get<0>(ki.outputBuffers.at(op_index))) * dataoffset * (std::get<1>(ki.outputBuffers.at(op_index)) / (ki.globalWorkSize[0]*ki.globalWorkSize[1]*ki.globalWorkSize[2]));          
    
    if (LOG_LEVEL>=1){
        fprintf(fp,"calculate_op_buffer_offset: END: %llu \n",get_current_time());
        fflush(fp);
    }

    return bufferoffset;
}

void cl_create_buffers(cl_context& ctx, KernelInfo& ki, std::vector<cl_mem>& io, std::vector<void*>& data, unsigned int size,unsigned int dataoffset,int dtype) {

    if (LOG_LEVEL>=1)
        fprintf(fp,"cl_create_buffers: BEGIN: %llu \n",get_current_time());

    int i;
    unsigned int datasize, buffer_offset;
    cl_int status;
    
    
    for (i = 0; i < ki.inputBuffers.size(); ++i)
    {
        PERSISTENCE_DEBUG printf("Creating input buffer\n");
        datasize=calculate_ip_buffer_size(size,i,ki);
        buffer_offset=calculate_ip_buffer_offest(dataoffset,i,ki);  
        //printf("cl_create_buffers: datasize: %u, size: %u, buffer_offset: %u, offset: %u \n",datasize,size,buffer_offset,dataoffset);
        if(dtype==PLATFORM_GPU)
            io.push_back(clCreateBuffer(ctx, CL_MEM_READ_ONLY | CL_MEM_ALLOC_HOST_PTR, datasize, NULL, &status));
        else
            io.push_back(clCreateBuffer(ctx, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, datasize, data[i]+ buffer_offset, &status));
        // io.push_back(clCreateBuffer(ctx, CL_MEM_READ_ONLY |CL_MEM_COPY_HOST_PTR | CL_MEM_ALLOC_HOST_PTR, datasize, data[i]+ buffer_offset, &status));
        //printf("cl_create_buffers:Input Data Start: %u , data: %f  ,datasize: %u, buffer_offset:%u  \n",data[i]+buffer_offset,*((float*)(data[i]+buffer_offset)),datasize,buffer_offset);
        if(LOG_LEVEL==1)
            fprintf(fp,"\tcl_create_buffers:Input Data Start: %u , data: %f  ,datasize: %u, buffer_offset:%u  \n",data[i]+buffer_offset,*((float*)(data[i]+buffer_offset)),datasize,buffer_offset);

        //ki.bufferInputID.push_back(io.size() - 1);
        check(status, "Creating Read Only Buffer");
        // printf("%d - size %d -%d \n",i,io.size(),ki.inputBuffers.size());
        
    }
    for (i = 0; i < ki.outputBuffers.size(); ++i)
    {     
        PERSISTENCE_DEBUG printf("Creating output buffer\n");
        datasize=calculate_op_buffer_size(size,i,ki);
        buffer_offset=calculate_op_buffer_offset(dataoffset,i,ki);   
        if(dtype==PLATFORM_GPU)
            io.push_back(clCreateBuffer(ctx, CL_MEM_WRITE_ONLY | CL_MEM_ALLOC_HOST_PTR, datasize, NULL, &status));
        else
            io.push_back(clCreateBuffer(ctx, CL_MEM_WRITE_ONLY | CL_MEM_USE_HOST_PTR, datasize, data[i + ki.inputBuffers.size()]+ buffer_offset, &status));
        // io.push_back(clCreateBuffer(ctx, CL_MEM_WRITE_ONLY | CL_MEM_COPY_HOST_PTR | CL_MEM_ALLOC_HOST_PTR, datasize, data[i + ki.inputBuffers.size()]+ buffer_offset, &status));

        // printf("cl_create_buffers:Output Data Start: %u , data: %f  ,datasize: %u, buffer_offset:%u  \n",data[i+ ki.inputBuffers.size()]+buffer_offset,*((float*)(data[i+ ki.inputBuffers.size()]+buffer_offset)),datasize,buffer_offset);
        // if(LOG_LEVEL==1)
        //     fprintf(fp,"\ncl_create_buffers:Output Data Start: %u , data: %f",data[i+ ki.inputBuffers.size()]+buffer_offset,*((float*)(data[i+ ki.inputBuffers.size()]+buffer_offset)));
        check(status, "Creating Write Only Buffer");
    }
    if (LOG_LEVEL>=1){
        fprintf(fp,"cl_create_buffers: END: %llu \n",get_current_time());
        fflush(fp);
    }
    PERSISTENCE_DEBUG printf("clCreateBuffers completed with %lu input buffers and $%lu output buffers\n",ki.inputBuffers.size(), ki.outputBuffers.size());
}
void cl_set_kernel_args(KernelInfo& ki, std::vector<cl_mem>& io, int platform_pos,unsigned int datasize) {
    
    if (LOG_LEVEL>=1)
        fprintf(fp,"cl_set_kernel_args: BEGIN: %llu \n",get_current_time());
    
    //object is 0 if gpu, 1 if cpu
    int i, j;
    cl_int status;
    int index;
    int object=platform_pos;
    if(object != -1)
    {
        for (i = 0; i < ki.inputBuffers.size(); ++i)
        {
            status = clSetKernelArg(ki.kernelObjects[object], std::get<2>(ki.inputBuffers.at(i)), sizeof(cl_mem), &io.at(i));
            check(status, "Setting Input Buffer Kernel Argument");
        }
        for (i = 0; i < ki.noOutputBuffers; ++i)
        {
            status = clSetKernelArg(ki.kernelObjects[object], std::get<2>(ki.outputBuffers.at(i)), sizeof(cl_mem), &io.at(i+ki.inputBuffers.size()));
            check(status, "Setting Output Buffer Kernel Argument");
        }
        ////////////////////////////////////////////////////////////////////////
        
        for( i = 0; i< ki.varArguments.size(); i++) 
        {
            //The first varArgument contains no of elements
            int flag=0;
            for(int j=0;j<ki.nonPartition.size();j++)
                if(ki.nonPartition[j]== std::get<2>(ki.varArguments.at(i)))
                {
                    index = std::get<1>(ki.varArguments.at(i));
                    flag=1;
                }
            if(flag==0)
                index =  std::get<1>(ki.varArguments.at(i)) * ((float)datasize/(ki.globalWorkSize[0]*ki.globalWorkSize[1]*ki.globalWorkSize[2]));

            status = clSetKernelArg(ki.kernelObjects[object], std::get<2>(ki.varArguments.at(i)), get_sizeof(std::get<0>(ki.varArguments.at(i))), &index);
            check(status, "Setting Variable Kernel Argument");
            
        }  
              
        for( i=0; i< ki.localArguments.size(); i++) 
        {
            status = clSetKernelArg(ki.kernelObjects[object], std::get<2>(ki.localArguments.at(i)), get_sizeof(std::get<0>(ki.localArguments.at(i))) * std::get<1>(ki.localArguments.at(i)), NULL);
        }
    }
    if (object == -1)
    {
        fprintf(fp,"\nImplementation Not required.");
        // for (i = 0; i < ki.noInputBuffers; ++i)
        // {
        //     status = clSetKernelArg(ki.kernelObject, i, sizeof(cl_mem), &io.at(ki.bufferInputID.at(i)));
        //     check(status, "Setting Kernel Argument");
        // }
        // for (i = 0; i < ki.noOutputBuffers; ++i)
        // {
        //     status = clSetKernelArg(ki.kernelObject, i + ki.noInputBuffers, sizeof(cl_mem), &io.at(ki.bufferOutputID.at(i)));
        //     check(status, "Setting Kernel Argument");
        // }
        // for( i = 0; i< ki.variableArguments.size(); i++) {
        //     status = clSetKernelArg(ki.kernelObject, i, get_sizeof(ki.kernelArguments.at(i).first), get_global_pointer(ki.kernelArguments.at(i).first, ki.kernelArguments.at(i).second.));
        //     check(status, "Setting Kernel Argument");
        // }
    }
    if (LOG_LEVEL>=1){
        fprintf(fp,"cl_set_kernel_args: END: %llu \n",get_current_time());
        fflush(fp);
    }
    
}

std::vector<cl_event> cl_enqueue_write_buffers(KernelExecutionInfo *di , cl_command_queue cmd_q, KernelInfo& ki, std::vector<cl_mem>& io, std::vector<void*>& data, unsigned int size,unsigned int dataoffset, cl_event dep, int dtype) {

    if (LOG_LEVEL>=1)
        fprintf(fp,"cl_enqueue_write_buffers: BEGIN: %llu \n",get_current_time());

    write_buffers_begin=get_current_time();
    di->write_buffers_start= write_buffers_begin;
    // struct timeval start_write;
    // gettimeofday(&start_write,NULL); 
    // unsigned long long int start_write_time=(unsigned long long int )(start_write.tv_sec*1000000+start_write.tv_usec);
    // di->write_start=start_write_time;
    di->write_start=get_current_time();
    // DAGtimeMatrix[kl_info.task->dagInfo->jobID][kl_info.task->dagInfo->instanceID]->writeStart=get_current_time();
    int i;
    unsigned int datasize, buffer_offset, element_offset;
    cl_int status;
    int num_buffers = 0;

    if(ki.configured[dtype])
        for(int i =0;i<ki.inputBuffers.size();i++)
        {
            if(!std::get<3>(ki.inputBuffers.at(i)))
                num_buffers+=1;
        }
    else
    {
        num_buffers = ki.noInputBuffers;
    }
    PERSISTENCE_DEBUG printf("Number of events to be set for write buffers: %d\n",num_buffers);
    std::vector<cl_event> finish(num_buffers);
    if(!EVENTS_ENABLE)
        finish.clear();
    cl_uint mem_size;
    void* host_mem;
    int counter = 0;
    for (i = 0; i < ki.inputBuffers.size(); ++i)
    {
        datasize=calculate_ip_buffer_size(size,i,ki);
        buffer_offset=calculate_ip_buffer_offest(dataoffset,i,ki);  
        //printf("\nBUFFER_INFO cl_enqueue_write_buffers: datasize:%u offset:%u\n",datasize,buffer_offset); 
        //printf("\nBUFFER_INFO cl_enqueue_write_buffers: data:%u\n",sizeof(data[i])); 
        
        //fprintf(fp,"\ncl_enqueue_write_buffers:Data Start: %u , data: %f",data[i]+buffer_offset,*((float*)(data[i]+buffer_offset)));
        //fprintf(fp,"\ncl_enqueue_write_buffers:Data end: %u , data: %f  ",data[i]+buffer_offset+datasize-4,*((float*)(data[i]+buffer_offset+datasize-4)));  
        struct timeval c_time;
        gettimeofday(&c_time,NULL);
        unsigned long long int current_time=(unsigned long long int )(c_time.tv_sec*1000000000+c_time.tv_usec*1000);  
        // printf("get_curtime_write_start: %llu \n\\\\*****************************************************\n",current_time);
        int persistent = std::get<3>(ki.inputBuffers.at(i));
        if(!ki.configured[dtype]) // not run once
        {
            PERSISTENCE_DEBUG printf("writeBuffer: kernel is deployed for the first time\n");
            if(EVENTS_ENABLE)
            {
                if (dep != NULL && counter==0)
                    status = clEnqueueWriteBuffer(cmd_q, io.at(i), CL_FALSE, 0, datasize, data[i] + buffer_offset, 1, &dep, &(finish[counter]));     
                else if(dep != NULL && counter>0)
                    status = clEnqueueWriteBuffer(cmd_q, io.at(i), CL_FALSE, 0, datasize, data[i] + buffer_offset, 1, &(finish[counter-1]), &(finish[counter]));
                counter++;
            }
            else
            {
                if (dep != NULL && counter==0)
                    status = clEnqueueWriteBuffer(cmd_q, io.at(i), CL_FALSE, 0, datasize, data[i] + buffer_offset, 1, &dep, NULL);     
                else if(dep != NULL && counter>0)
                    status = clEnqueueWriteBuffer(cmd_q, io.at(i), CL_FALSE, 0, datasize, data[i] + buffer_offset, 0, NULL, NULL);
                counter++;   
            }
        }
        else if(!persistent) // run once and buffer is not persistent i.e. image needs to be written
        {
            PERSISTENCE_DEBUG printf("writeBuffer: Kernel has been deployed already. Setting only persistent buffers\n");
            if(EVENTS_ENABLE)
            {
                if (dep != NULL && counter==0)
                    status = clEnqueueWriteBuffer(cmd_q, io.at(i), CL_FALSE, 0, datasize, data[i] + buffer_offset, 1, &dep, &(finish[counter]));     
                else if(dep != NULL && counter>0)
                    status = clEnqueueWriteBuffer(cmd_q, io.at(i), CL_FALSE, 0, datasize, data[i] + buffer_offset, 1, &(finish[counter-1]), &(finish[counter]));
                counter++;
            }
            else
            {
                if (dep != NULL && counter==0)
                    status = clEnqueueWriteBuffer(cmd_q, io.at(i), CL_FALSE, 0, datasize, data[i] + buffer_offset, 1, &dep, NULL);     
                else if(dep != NULL && counter>0)
                    status = clEnqueueWriteBuffer(cmd_q, io.at(i), CL_FALSE, 0, datasize, data[i] + buffer_offset, 0, NULL, NULL);
                counter++;   
            }
        }
        fprintf(fp,"\tInputBufferSize: %d\n",datasize);
        fprintf(fp,"\tInputBufferOffset: %d\n",buffer_offset);
        check(status, "Enqueing Write Buffers"); 
        //fprintf(fp,"\ncl_enqueue_write_buffers: finishEvent%d.write: %u - %u",i,&finish[i],finish[i] );
        
    }
    PERSISTENCE_DEBUG printf("clEnqueueWriteBuffer with %d buffers\n",num_buffers);
    if (LOG_LEVEL>=1){
        fprintf(fp,"cl_enqueue_write_buffers: END: %llu \n",get_current_time());
        fflush(fp);
    }

    return finish;
    
}


cl_event cl_enqueue_nd_range_kernel(KernelExecutionInfo *di,cl_command_queue cmd_q, KernelInfo& ki, int object,unsigned int size, cl_event dep) {

    if (LOG_LEVEL>=1)
        fprintf(fp,"cl_enqueue_nd_range_kernel: BEGIN: %llu \n",get_current_time());

    // struct timeval start_nd;
    // gettimeofday(&start_nd,NULL); 
    // unsigned long long int  start_nd_time=(unsigned long long int )(start_nd.tv_sec*1000000+start_nd.tv_usec);
    // di->nd_start=start_nd_time;
    di->nd_start = get_current_time();
    di->write_end=di->nd_start;
    di->write_time=di->write_end - di->write_start;

    //object is 0 if gpu, 1 if cpu
    cl_int status;
    int i;
    cl_event finish;
    size_t globalWorkSize[3];
    for (i = 0; i < 3; ++i)
    {
        globalWorkSize[i] = ki.globalWorkSize[i];
    }
    // if(globalWorkSize[0] > size)
    //     fprintf(fp,"\nCHUNK SIZE CAN NOT BE SMALLER THAN GLOBAL WORK SIZE ALONG DIMENSION 0"); 

    globalWorkSize[0] = size/(globalWorkSize[1]*globalWorkSize[2]);
    //fprintf(fp,"\nGLOBALWORKSIZE: %u",globalWorkSize[0]);
    
    if(object == 0 || object == 1)
    {    if(ki.localWorkSize[0] != 0)
        {
            PERSISTENCE_DEBUG printf("ND Range kernel when local work size is known\n");
           // fprintf(fp,"Local WorkSize set manually\n");
            if(EVENTS_ENABLE)
                status = clEnqueueNDRangeKernel(cmd_q, ki.kernelObjects[object], ki.workDimension, 0, globalWorkSize, ki.localWorkSize, 1, &dep, &finish);
            else
                status = clEnqueueNDRangeKernel(cmd_q, ki.kernelObjects[object], ki.workDimension, 0, globalWorkSize, ki.localWorkSize, 0, NULL, NULL);
        }
        else
        {
            //fprintf(fp,"Local WorkSize set during runtime \n");
            PERSISTENCE_DEBUG printf("ND Range kernel when local work size is not known\n");
            // size_t localWorkSize[3];
            // localWorkSize[0]=256;
            // localWorkSize[1]=1;
            // localWorkSize[2]=1;
            if(EVENTS_ENABLE)
                status = clEnqueueNDRangeKernel(cmd_q, ki.kernelObjects[object], ki.workDimension, 0, globalWorkSize, NULL, 1, &dep, &finish);
            else
            status = clEnqueueNDRangeKernel(cmd_q, ki.kernelObjects[object], ki.workDimension, 0, globalWorkSize, NULL, 0, NULL, NULL);
            // status = clEnqueueNDRangeKernel(cmd_q, ki.kernelObjects[object], ki.workDimension, 0, globalWorkSize, localWorkSize, 1, &dep, &finish);
        }
        char msg[256];
        sprintf(msg, "Enqued ND Range Kernel with WD %d GWS %lu %lu %lu ",ki.workDimension, globalWorkSize[0], globalWorkSize[1], globalWorkSize[2]);
        check(status, msg);
        if (LOG_LEVEL >=1)
            fprintf(fp,"\tEnqued ND Range Kernel with WD %d GWS %lu %lu %lu\n ",ki.workDimension, globalWorkSize[0], globalWorkSize[1], globalWorkSize[2]);

        //fprintf(fp,"\ncl_enqueue_nd_range_kernel: finishEvent%d.exec: %u - %u",i,&finish,finish );
    }
    
    if(object == -1) {
        if(ki.localWorkSize[0] != 0)
            if(EVENTS_ENABLE)
                status = clEnqueueNDRangeKernel(cmd_q, ki.kernelObject, ki.workDimension, 0, globalWorkSize, ki.localWorkSize, 1, &dep, &finish);
            else
                status = clEnqueueNDRangeKernel(cmd_q, ki.kernelObject, ki.workDimension, 0, globalWorkSize, ki.localWorkSize, 0, NULL, NULL);
        else
            if(EVENTS_ENABLE)
                status = clEnqueueNDRangeKernel(cmd_q, ki.kernelObject, ki.workDimension, 0, globalWorkSize, NULL, 1, &dep, &finish);
            else
                status = clEnqueueNDRangeKernel(cmd_q, ki.kernelObject, ki.workDimension, 0, globalWorkSize, NULL, 0, NULL, NULL);
        check(status, "Enqueing ND Range Kernel");       
    }
    if (LOG_LEVEL>=1){
        fprintf(fp,"cl_enqueue_nd_range_kernel: END: %llu \n",get_current_time());
        fflush(fp);
    }
    PERSISTENCE_DEBUG printf("%d Enqueued kernel\n",__LINE__);
    return finish;
}


std::vector<cl_event> cl_enqueue_read_buffers(KernelExecutionInfo *di,cl_command_queue cmd_q, KernelInfo& ki, std::vector<cl_mem>& io, std::vector<void*>& data, unsigned int size,unsigned int dataoffset, cl_event dep, int dtype) {

    if (LOG_LEVEL>=1)
        fprintf(fp,"cl_enqueue_read_buffers: BEGIN: %llu \n",get_current_time());

    // struct timeval start_read;
    // gettimeofday(&start_read,NULL); 
    // unsigned long long int  start_read_time=(unsigned long long int )(start_read.tv_sec*1000000+start_read.tv_usec);
    // di->read_start=start_read_time;
    di->read_start=get_current_time();
    di->nd_end=di->read_start;
    di->nd_time=di->nd_end - di->nd_start;

    int i;
    unsigned int datasize, buffer_offset, element_offset;
    int num_buffers = 0;
    if(ki.configured[dtype])
        for(int i =0;i<ki.outputBuffers.size();i++)
        {
            if(!std::get<3>(ki.outputBuffers.at(i)))
                num_buffers+=1;
        }
    else
    {
        num_buffers = ki.noOutputBuffers;
    }
    PERSISTENCE_DEBUG printf("Read Event setup for %d buffers\n",num_buffers);
    std::vector<cl_event> finish(num_buffers);
    if(!EVENTS_ENABLE)
        finish.resize(1);
    cl_int status;
    int counter = 0;
    for (i = 0; i < ki.outputBuffers.size(); ++i)
    {
        datasize=calculate_op_buffer_size(size,i,ki);
        buffer_offset=calculate_op_buffer_offset(dataoffset,i,ki); 
        //printf("\nBUFFER_INFO cl_enqueue_read_buffers: datasize:%u offset:%u\n",datasize,buffer_offset); 
        //printf("\nBUFFER_INFO cl_enqueue_read_buffers: data:%u\n",sizeof(data[i]));
        // fprintf(fp,"\tOutputBufferSize: %d\n",datasize);

        fprintf(fp,"\tOutputBufferOffset: %d\n",buffer_offset);             
        int persistent = std::get<3>(ki.inputBuffers.at(i));
        if(!ki.configured[dtype])
        { // not run once
            PERSISTENCE_DEBUG printf("readBuffer: Kernel has not been deployed yet.\n");
            
            if (dep != NULL && i==0)
            {
                if(EVENTS_ENABLE)
                    status = clEnqueueReadBuffer(cmd_q, io.at(i + ki.inputBuffers.size()), CL_FALSE, 0, datasize, data[i + ki.inputBuffers.size()] + buffer_offset, 1, &dep, &finish[i]);    
                else
                {
                    if(counter!=num_buffers-1)
                        status = clEnqueueReadBuffer(cmd_q, io.at(i + ki.inputBuffers.size()), CL_FALSE, 0, datasize, data[i + ki.inputBuffers.size()] + buffer_offset, 0, NULL, NULL);    
                    else
                        status = clEnqueueReadBuffer(cmd_q, io.at(i + ki.inputBuffers.size()), CL_FALSE, 0, datasize, data[i + ki.inputBuffers.size()] + buffer_offset, 0, NULL, &finish[0]);    
                }
            }
            else if(dep != NULL && i>0)
            {
                if(EVENTS_ENABLE)
                    status = clEnqueueReadBuffer(cmd_q, io.at(i + ki.inputBuffers.size()), CL_FALSE, 0, datasize, data[i + ki.inputBuffers.size()] + buffer_offset, 1, &finish[i-1], &finish[i]);
                else
                {
                    if(counter!=num_buffers-1)
                        status = clEnqueueReadBuffer(cmd_q, io.at(i + ki.inputBuffers.size()), CL_FALSE, 0, datasize, data[i + ki.inputBuffers.size()] + buffer_offset, 0, NULL, NULL);
                    else
                        status = clEnqueueReadBuffer(cmd_q, io.at(i + ki.inputBuffers.size()), CL_FALSE, 0, datasize, data[i + ki.inputBuffers.size()] + buffer_offset, 0, NULL, &finish[0]);                        
                }
            }
            counter++;
        }
        else if(!persistent)
        {
            PERSISTENCE_DEBUG printf("readBuffer: Kernel has  already been deployed once.\n");
            if (dep != NULL && i==0)
            {
                if(EVENTS_ENABLE)
                    status = clEnqueueReadBuffer(cmd_q, io.at(i + ki.inputBuffers.size()), CL_FALSE, 0, datasize, data[i + ki.inputBuffers.size()] + buffer_offset, 1, &dep, &finish[counter]);    
                else
                {
                    if(counter!=num_buffers-1)
                        status = clEnqueueReadBuffer(cmd_q, io.at(i + ki.inputBuffers.size()), CL_FALSE, 0, datasize, data[i + ki.inputBuffers.size()] + buffer_offset, 0, NULL, NULL);    
                    else
                        status = clEnqueueReadBuffer(cmd_q, io.at(i + ki.inputBuffers.size()), CL_FALSE, 0, datasize, data[i + ki.inputBuffers.size()] + buffer_offset, 0, NULL, &finish[0]);    
                }
            }
            else if(dep != NULL && i>0)
            {
                if(EVENTS_ENABLE)
                    status = clEnqueueReadBuffer(cmd_q, io.at(i + ki.inputBuffers.size()), CL_FALSE, 0, datasize, data[i + ki.inputBuffers.size()] + buffer_offset, 1, &finish[counter-1], &finish[counter]);
                       
                else
                {
                    if(counter!=num_buffers-1)
                        status = clEnqueueReadBuffer(cmd_q, io.at(i + ki.inputBuffers.size()), CL_FALSE, 0, datasize, data[i + ki.inputBuffers.size()] + buffer_offset, 0, NULL, NULL);
                    else
                        status = clEnqueueReadBuffer(cmd_q, io.at(i + ki.inputBuffers.size()), CL_FALSE, 0, datasize, data[i + ki.inputBuffers.size()] + buffer_offset, 0, NULL, &finish[0]);
                }
            }
            counter++;
        }
        fprintf(fp,"\tOutputBufferSize: %d\n",datasize);
        fprintf(fp,"\tOutputBufferOffset: %d\n",buffer_offset);        
        // for(int d=0;d<size;d++)
        //     fprintf(fp,"\tcl_enqueue_read_buffers:Output Data Start: %d - %u , data: %f - %f\n",d,data[i + ki.inputBuffers.size()]+buffer_offset,*((float*)(data[i + ki.inputBuffers.size()]+buffer_offset)+d),*((float*)(io.at(i + ki.inputBuffers.size()))+d));
        check(status, "Enqueing Read Buffers");
        
    }
        
    check(status,"Trigger event callback function");
    if (LOG_LEVEL>=1){
        fprintf(fp,"cl_enqueue_read_buffers: END: %llu \n",get_current_time());
        fflush(fp);
    }
    return finish;
}


int test_and_set(int *lock,int testval, int newval){
   
    int oldval;
    oldval=*lock;
    if(oldval==testval)
        *lock=newval;
    return oldval;
}
// return <type of error,platform> type of error -> 0: no error, 1: minor error, 2: major error but no minor error,3: major and minor, platform -> 0: GPU, 1: CPU, 2: both
pair<int, int> check_thermal_violation(unsigned long long int start,unsigned long long int end)
{
    if (LOG_LEVEL>=1)
        fprintf(fp,"check_thermal_violation: BEGIN: %llu \n",get_current_time());
    fflush(fp);

    int minor_v_CPU=0;
    int minor_v_GPU=0;
    int major_v_CPU=0;
    int major_v_GPU=0;

    for(auto elem : tempMap)
    {
        unsigned long long int time=elem.first;
        Temperature* tmp=elem.second;

        if(time>=start && time<=end)
        {
            for(int i=0;i<NumOfSensors;i++)
            {
                if(tmp->sensors[i]>THERMAL_THRESHOLD_H)
                {
                    if(i>=0 && i<4) 
                        major_v_CPU=1;
                    else if(i==4)
                        major_v_GPU=1;

                }    
                if(tmp->sensors[i]>THERMAL_THRESHOLD_L && tmp->sensors[i]<THERMAL_THRESHOLD_H)
                {
                    if(i>=0 && i<4) 
                        minor_v_CPU=1;
                    else if(i==4)
                        minor_v_GPU=1;
                }

            }
        }

        //fprintf(tmp_result,"%llu %d %d %d %d %d\n",elem.first, elem.second->sensors[0],elem.second->sensors[1],elem.second->sensors[2],elem.second->sensors[3],elem.second->sensors[4] );
    }


    if (LOG_LEVEL>=1)
        fprintf(fp,"check_thermal_violation: END: %llu \n",get_current_time());
    fflush(fp);

    if(!minor_v_CPU && !minor_v_GPU && !major_v_CPU && !major_v_GPU)
        return make_pair(0,-1);

    else if(minor_v_CPU && minor_v_GPU && !major_v_CPU && !major_v_GPU)
        return make_pair(1,2);

    else if(minor_v_CPU && !minor_v_GPU && !major_v_CPU && !major_v_GPU)
        return make_pair(1,1);

    else if(!minor_v_CPU && minor_v_GPU && !major_v_CPU && !major_v_GPU)
        return make_pair(1,0);

    else if(major_v_CPU && major_v_GPU)
        return make_pair(2,2);

    else if(major_v_CPU && !minor_v_GPU && !major_v_GPU)
        return make_pair(2,1);

    else if(!major_v_CPU && !minor_v_CPU && major_v_GPU)
        return make_pair(2,0);

    else if(major_v_CPU && minor_v_GPU)
        return make_pair(3,1);

    else if(minor_v_CPU && major_v_GPU)
        return make_pair(3,0);

    

    return make_pair(-1,-1);

}


void CL_CALLBACK notify_callback_update_release (cl_event event, cl_int event_command_exec_status, void *user_data)
{   
    
    unsigned long long int end=get_current_time(); 

    cpu_set_t cpuset;
    pthread_t thread_id_scheduler = pthread_self(); 
    CPU_ZERO(&cpuset);
    CPU_SET(0, &cpuset);    
    int s = pthread_setaffinity_np(thread_id_scheduler, sizeof(cpu_set_t), &cpuset);

    // struct timeval end_read;
    // gettimeofday(&end_read,NULL);       
    if (LOG_LEVEL>=1)
        fprintf(fp,"notify_callback_update_release: BEGIN: %llu \n",get_current_time());
    fflush(fp);
    cl_int err;
    KernelLaunchInfo *kl=(KernelLaunchInfo*)user_data;
    printf("\n++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\nnotify_callback_update_release %d: BEGIN: %llu \n",kl->task->traceID,get_current_time());
    kl->kex.rel_end_time=end;
    kl->kex.notify_callback_rel_start_time=get_current_time();
    // unsigned long long int  end_rtime=(unsigned long long int )(end_read.tv_sec*1000000+end_read.tv_usec);
    // kl->kex.read_end=end_rtime;
    kl->kex.read_end=end;
    kl->kex.read_time=kl->kex.read_end - kl->kex.read_start;
    
    unsigned long long int  timing = kl->kex.rel_end_time - kl->kex.rel_start_time; 
    kl->kex.turnaroundTime=timing;

    // printf("***********************notify_callback_start-Write_start: %llu *****************************\n",end-write_buffers_begin);
    printf("notify_callback_update_release %lu on core %d \n",pthread_self(),sched_getcpu());

    if(kl->task->traceID!=-1 )
    {     
        fprintf(t_result,"%s_%u %d-%d %u %llu : ",kl->task->taskID.c_str(),kl->size,kl->platform_pos,kl->device_pos,kl->frequency,timing);
        fflush(t_result);          
        
        if(LOG_PROFILE==1)
        {
            profile_events(*kl);
            //task name, size, paltform, device,freq, ex time
            
        }        
        // fclose (t_result);
        // printf("%s : %llu \n",kl->task->taskID.c_str(),timing);  
    }     

    //std::thread::id tid = std::this_thread::get_id();
    if (LOG_LEVEL >=1)
    {
        fprintf(fp,"\tNOTIFY_CALLBACK: %d: %d-%d - %llu\n",kl->task->traceID ,kl->platform_pos,kl->device_pos,get_current_time()) ; 
        fflush(fp);
    }
    
    if (LOG_LEVEL >=1)
    {
        fprintf(fp,"\tEnd time for %s in device %d-%d : %llu\n",kl->task->taskID.c_str(),kl->platform_pos,kl->device_pos,kl->kex.rel_end_time);
        fflush(fp);
    }
    // printf("\tEnd time in device %d-%d : %llu\n",kl->platform_pos,kl->device_pos,kl->kex.rel_end_time);

    //release buffers and kernel events
    unsigned long long int release_buff_Kevent_time=get_current_time();   
    if(kl->released_b==0)
    {
        release_buffers(kl->io);   
        kl->released_b=1;
    }
    release_kernel_events(kl->ke);
    release_buff_Kevent_time=get_current_time()-release_buff_Kevent_time;



    unsigned long long int ncb_all_calculations=get_current_time();
    unsigned long long int freqChange_time=0;
    unsigned long long int node_mark_time=0;
    unsigned long long int terminalNodeCal_time=0;
    unsigned long long int successorNodeTransfer_time=0;
    unsigned long long int releaseHostArray_time=0;
    unsigned long long int AddReadyBuffer_time=0;
    unsigned long long int partitioning_time=0;
    unsigned long long int safeModeForRL_time=0;
    unsigned long long int mode_change_time=0;

    unsigned long long int ncb_buffer;
    if(kl->platform_pos==1)
        ncb_buffer=notify_cb_buffer_c;
    else
        ncb_buffer=notify_cb_buffer_g;
    unsigned long long int local_d=kl->task->deadline;//

    std::string name; 
    //printf("Callback traceid %d\n",kl->task->traceID);

    if(isProfileMode==1) // for starting micro kernel repeatatively in case of profiling
    {
        std::unique_lock<std::mutex> lk(run_kernel_lock);
        num_profile_kernels++;
        if(num_profile_kernels == 2)
            processed = true;
        lk.unlock();
        if(processed)
            cv.notify_one();
        
        if(kl->task->traceID!=-1)
        {
            name=kl->task->taskID.c_str();
            kl->finished=1;
            printf("Resetting profilekernel_start to 1\n");
        }
        else
        {
            name="micro_kernel";  
            kl->finished=1; 
            printf("Resetting microkernel_start to 1\n");
        }
    }
    if(kl->task->traceID!=-1) // for any other kernels other than micro kernel
    {   

         //printf("******Kernel Stat: %d %u %d-%d %u %llu %llu %llu %llu *******\n",kl->task->traceID,kl->size,kl->platform_pos,kl->device_pos,kl->frequency,timing,kl->kex.rel_start_time,kl->kex.rel_end_time,kl->kex.notify_callback_rel_end_time);
        
    
   

        freqChange_time=get_current_time();  
        if(kl->priority==1)
        {   safe_duration+=get_current_time()-DAGtimeMatrix[kl->task->dagInfo->jobID][kl->task->dagInfo->instanceID]->startTime;
            printf("safe_duration: %llu\n",safe_duration);
        }
        if(raceToIdle)
        {
            unsigned int min_freq=deviceSpec[kl->platform_pos][kl->device_pos]->lowFrequencyBound;
            change_frequency(min_freq,  kl->platform_pos,  kl->device_pos);
        }
        freqChange_time = get_current_time()-freqChange_time;
        kl->kex.frequency_change_time = freqChange_time;
        // printf("freq: %u \n",min_freq);
        //printf("\nKERNEL %d:%s  OFFSET-SIZE %u-%u \tDEVICE %d-%d FREQUENCY %uHZ TIME %llu ms. ",,,,kl->platform_pos,kl->device_pos,kl->frequency,timing);  
        //outputbuffer+="KERNEL_F: "+std::to_string(kl->task->traceID)+kl->task->taskID+" OFFSET-SIZE: "+std::to_string(kl->offset)+"-"+std::to_string(kl->size)+" Device: "+std::to_string(kl->device_index)+ " extime: " + std::to_string(timing)+ "curtime: "+std::to_string(get_current_time())+"\n";  
        // print_launch_info(*kl);
        
        kl->recovery_mode=0;


        unsigned int total_sz =kl->task->task_size;
        // printf("offset: %u size: %u total_sz: %u task size: %u\n",kl->offset,kl->size,kl->offset+kl->size,total_sz);


        int job =kl->task->dagInfo->jobID;
        int instance=kl->task->dagInfo->instanceID;
        int global_dagid=kl->task->dagInfo->globalDAGID;
        // printf("job %d instance %d global_dagid %d",job,instance,global_dagid);


        if(CREATE_SUB_KERNEL==0 )
        {
            //not partitoned and full task done
            if(kl->offset+kl->size==total_sz)
            {
                //kl->task->task_completed=1;
                kl->offset=total_sz; 
                kl->size=0;               
                
                // printf("kl->task->nodes.size(): %u\n",kl->task->nodes.size());               
                node_mark_time=get_current_time();
                for(int n=0;n<kl->task->nodes.size();n++)
                {
                    nodes_matrix[kl->task->dagInfo->globalDAGID][kl->task->nodes[n]]=1;
                    // printf("nodes marked: nodes_matrix[%d][%d] = %d\n",kl->task->dagInfo->globalDAGID,kl->task->nodes[n],nodes_matrix[kl->task->dagInfo->globalDAGID][kl->task->nodes[n]]); 
                }  
                // printf("nodes marked\n"); 
                node_mark_time=get_current_time()-node_mark_time;

                dump_output(kl);

                
                
                if(kl->task->isTerminal==1) // if terminal node then update timing profiling values
                {
                    terminalNodeCal_time=get_current_time();
                    // printf("j-i: %d-%d \n",job,instance);
                    DAGtimeMatrix[job][instance]->writeStart=kl->kex.write_start ;
                    DAGtimeMatrix[job][instance]->ndStart=kl->kex.nd_start ;
                    DAGtimeMatrix[job][instance]->readStart=kl->kex.read_start ;
                    DAGtimeMatrix[job][instance]->finishTime=end;
                    DAGtimeMatrix[job][instance]->turnaroundTime=DAGtimeMatrix[job][instance]->finishTime-DAGtimeMatrix[job][instance]->startTime;
                    DAGtimeMatrix[job][instance]->makespan=DAGtimeMatrix[job][instance]->finishTime-DAGtimeMatrix[job][instance]->arrivalTime;
                    DAGtimeMatrix[job][instance]->lateness=DAGtimeMatrix[job][instance]->finishTime-DAGtimeMatrix[job][instance]->deadline;
                    // printf("\nDagID: %d FinishTime:%llu\n",global_dagid,end);
                    // printf("DagID: %d : %d-%d StartTime:%llu FinishTime:%llu ArrivalTime:%llu Deadline:%f Lateness:%f makespan:%f\n",global_dagid,job,instance,DAGtimeMatrix[job][instance]->startTime,DAGtimeMatrix[job][instance]->finishTime,DAGtimeMatrix[job][instance]->arrivalTime,DAGtimeMatrix[job][instance]->deadline,DAGtimeMatrix[job][instance]->lateness,DAGtimeMatrix[job][instance]->makespan);
                    //Dump lateness and deadline violation %
                    
                    if(kl->kex.rel_end_time>local_d)
                    {   DAGtimeMatrix[job][instance]->deadlineViolated=DAGtimeMatrix[job][instance]->deadlineViolated+1;
                        //printf("########DEADLINE VIOLATED#######: %d\n",DAGtimeMatrix[job][instance]->deadlineViolated);
                        if(SAFE && instance<DAGtimeMatrix[job].size()-2)
                        {
                            for(int i=instance+1;i<DAGtimeMatrix[job].size();i++)
                                DAGtimeMatrix[job][i]->klinfo->priority=1;
                        }    
                    }

                    //kl->not_done_count=
                    terminalNodeCal_time=get_current_time()-terminalNodeCal_time;
                }   
                

                //populate input buffer for successor nodes  if not terminal node           
                else
                {
                    successorNodeTransfer_time=get_current_time();
                    int of_sz=kl->task->kernels[0]->data_outflow.size();  
                    // printf("of_sz %d\n",of_sz);
                    for(int s=0;s<of_sz;s++)
                    {
                        int suc_node=std::get<0>(kl->task->kernels[0]->data_outflow.at(s));
                        int suc_pos=std::get<1>(kl->task->kernels[0]->data_outflow.at(s));
                        int cur_pos=std::get<2>(kl->task->kernels[0]->data_outflow.at(s));

                        // printf("data_outflow_%d suc_node: %d suc_pos: %d cur_pos: %d\n",s,suc_node,suc_pos,cur_pos);
                        // printf("global_dagid,suc_node %d %d \n",global_dagid,suc_node );
                        // print_task_map();
                        std::map<pair<int, int>,std::vector<KernelLaunchInfo*>>::iterator iter;
                        iter = taskMap.find(make_pair(global_dagid,suc_node));        

                        // printf("global_dagid,suc_node %d %d \n",global_dagid,suc_node );
                        //print_task_map();


                        if(iter != taskMap.end())
                        {
                            KernelLaunchInfo* kl_suc = iter->second[0]; 
                            // printf("size of kl_suc->task->data[suc_pos]: %u\n",sizeof(kl_suc->task->data[suc_pos]));
                            printf("size of kl->task->data[%d]: %u\n",cur_pos,sizeof(kl->task->data[cur_pos]));
                            
                            trasfer_data_to_ipbuf(kl, kl_suc,cur_pos,suc_pos);
                            printf("trasfer_data_to_ipbuf\n");

                        }
                        else 
                        {
                            printf("Not found in taskMap\n");
                        }
                    }
                    // printf("Done1\n");
                    successorNodeTransfer_time=get_current_time()-successorNodeTransfer_time;
                }                       

                
                AddReadyBuffer_time=get_current_time();
                childrenToReadyBuffer(*kl)  ;  
                //printf("childrenToReadyBuffer\n");
                AddReadyBuffer_time=get_current_time()-AddReadyBuffer_time;            

            }
            else 
            {   
                partitioning_time=get_current_time();  
                /*
                if(kl->offset==0)
                    kl->task->kernels[0]->chunkSizeTiming=timing;
                
                int fc=(int)log2((float)kl->size/C_SZ);
                //if(kl->offset!=0 && fc==0)
                //    fc=
                printf("\nfc:%d time: %llu - %llu",fc,kl->task->kernels[0]->chunkSizeTiming,timing);
                
                if(kl->task->kernels[0]->localSizeFactor.size()==fc){
                    kl->task->kernels[0]->localSizeFactor.push_back(timing/(float)kl->task->kernels[0]->chunkSizeTiming);

                    if(timing*chunk_factor(kl)>(kl->task->exTime)*(kl->size*chunk_factor(kl)/(float)total_sz))
                        kl->frequency=deviceSpec[kl->device_index].highFrequencyBound;
                    else
                        kl->frequency=deviceSpec[kl->device_index].midFrequency;

                    printf("\ntimedifference: %f->%f expectedExtime %llu",kl->task->exTime,(kl->task->exTime)*(kl->size*chunk_factor(kl)/(float)total_sz),timing*chunk_factor(kl));
                }
                else{
                    kl->task->kernels[0]->localSizeFactor[fc]=(timing/(float)kl->task->kernels[0]->chunkSizeTiming+kl->task->kernels[0]->localSizeFactor[fc])/2.0;

                    if(fc<kl->task->kernels[0]->localSizeFactor.size()-1){
                        if(timing*kl->task->kernels[0]->localSizeFactor[fc+1]>(kl->task->exTime)*(kl->size*chunk_factor(kl)/(float)total_sz))
                            kl->frequency=deviceSpec[kl->device_index].highFrequencyBound;
                        else 
                            kl->frequency=deviceSpec[kl->device_index].midFrequency;  

                        printf("\nlocalSizeFactor:%f timedifference: %f->%f expectedExtime %llu*%f=%f",kl->task->kernels[0]->localSizeFactor[fc],kl->task->exTime,(kl->task->exTime)*(kl->size*chunk_factor(kl)/(float)total_sz),timing,kl->task->kernels[0]->localSizeFactor[fc+1],timing*kl->task->kernels[0]->localSizeFactor[fc+1]); 
                    }
                }
                
                change_frequency(kl->frequency,  kl->platform_pos,  kl->device_pos);       

                */

                kl->offset=kl->offset+kl->size;
                kl->size=kl->size*chunk_factor(kl);   
                if(kl->offset+kl->size > total_sz)
                    kl->size=total_sz - kl->offset;
                // printf("notifycallback_else_chunking\n");

                partitioning_time=get_current_time()-partitioning_time;
            }  

        }

        else if(CREATE_SUB_KERNEL==1)
        {
            mtx_p_kernels_size.lock();
            kl->full_task_klinfo->not_done_count=kl->full_task_klinfo->not_done_count-kl->size;
            mtx_p_kernels_size.unlock();

            if(kl->full_task_klinfo->not_done_count==total_sz-kl->size) //if 1st sub kernel processed update the time for kex start time
            {
                kl->full_task_klinfo->kex.write_start=kl->kex.write_start;
                kl->full_task_klinfo->kex.nd_start=kl->kex.nd_start;
                kl->full_task_klinfo->kex.read_start=kl->kex.read_start;
                kl->full_task_klinfo->kex.rel_start_time=kl->kex.rel_start_time;
            }    

            if(kl->full_task_klinfo->not_done_count<0)
                printf("notifycallback: kl->full_task_klinfo->not_done_count<0\n");

            //if full task is finished
            if(kl->full_task_klinfo->not_done_count==0)
            {
                //kl->offset=total_sz; 
                //kl->size=0;   

                kl->full_task_klinfo->kex.rel_end_time  = end;

                node_mark_time=get_current_time();
                for(int n=0;n<kl->task->nodes.size();n++)
                {
                    nodes_matrix[kl->task->dagInfo->globalDAGID][kl->task->nodes[n]]=1;
                    // printf("nodes marked: nodes_matrix[%d][%d] = %d\n",kl->task->dagInfo->globalDAGID,kl->task->nodes[n],nodes_matrix[kl->task->dagInfo->globalDAGID][kl->task->nodes[n]]); 
                } 
                node_mark_time=get_current_time()-node_mark_time;


                if(kl->task->isTerminal==1) //if it belongs to terminal node in dag
                {
                    terminalNodeCal_time=get_current_time();
                    // printf("j-i: %d-%d \n",job,instance);
                    DAGtimeMatrix[job][instance]->writeStart=kl->full_task_klinfo->kex.write_start ;
                    DAGtimeMatrix[job][instance]->ndStart=kl->full_task_klinfo->kex.nd_start ;
                    DAGtimeMatrix[job][instance]->readStart=kl->full_task_klinfo->kex.read_start ;
                    DAGtimeMatrix[job][instance]->finishTime=end;
                    DAGtimeMatrix[job][instance]->turnaroundTime=DAGtimeMatrix[job][instance]->finishTime-DAGtimeMatrix[job][instance]->startTime;
                    DAGtimeMatrix[job][instance]->makespan=DAGtimeMatrix[job][instance]->finishTime-DAGtimeMatrix[job][instance]->arrivalTime;
                    DAGtimeMatrix[job][instance]->lateness=DAGtimeMatrix[job][instance]->finishTime-DAGtimeMatrix[job][instance]->deadline;
                    // printf("\nDagID: %d FinishTime:%llu\n",global_dagid,end);
                    // printf("DagID: %d : %d-%d StartTime:%llu FinishTime:%llu ArrivalTime:%llu Deadline:%f Lateness:%f makespan:%f\n",global_dagid,job,instance,DAGtimeMatrix[job][instance]->startTime,DAGtimeMatrix[job][instance]->finishTime,DAGtimeMatrix[job][instance]->arrivalTime,DAGtimeMatrix[job][instance]->deadline,DAGtimeMatrix[job][instance]->lateness,DAGtimeMatrix[job][instance]->makespan);

                    //Dump lateness and deadline violation                   
                    if(kl->kex.rel_end_time>local_d)
                    {   DAGtimeMatrix[job][instance]->deadlineViolated=DAGtimeMatrix[job][instance]->deadlineViolated+1;
                        //printf("########DEADLINE VIOLATED#######: %d\n",DAGtimeMatrix[job][instance]->deadlineViolated);
                        if(SAFE && instance<DAGtimeMatrix[job].size()-2)
                        {
                            for(int i=instance+1;i<DAGtimeMatrix[job].size();i++)
                                DAGtimeMatrix[job][i]->klinfo->priority=1;
                        }    
                    }
                    terminalNodeCal_time=get_current_time()-terminalNodeCal_time;
                }
                else //if it is not terminal node then update and populate input buffer for successor nodes     
                {

                    successorNodeTransfer_time=get_current_time();
                    int of_sz=kl->task->kernels[0]->data_outflow.size();  
                    // printf("of_sz %d\n",of_sz);
                    for(int s=0;s<of_sz;s++)
                    {
                        int suc_node=std::get<0>(kl->task->kernels[0]->data_outflow.at(s));
                        int suc_pos=std::get<1>(kl->task->kernels[0]->data_outflow.at(s));
                        int cur_pos=std::get<2>(kl->task->kernels[0]->data_outflow.at(s));

                        // printf("data_outflow_%d suc_node: %d suc_pos: %d cur_pos: %d\n",s,suc_node,suc_pos,cur_pos);
                        // printf("global_dagid,suc_node %d %d \n",global_dagid,suc_node );
                        // print_task_map();
                        std::map<pair<int, int>,std::vector<KernelLaunchInfo*>>::iterator iter;
                        iter = taskMap.find(make_pair(global_dagid,suc_node));        
                        // printf("global_dagid,suc_node %d %d \n",global_dagid,suc_node );
                        //print_task_map();

                        if(iter != taskMap.end()) //if found
                        {
                            KernelLaunchInfo* kl_suc = iter->second[0]; 
                            // printf("size of kl_suc->task->data[suc_pos]: %u\n",sizeof(kl_suc->task->data[suc_pos]));
                            printf("size of kl->task->data[%d]: %u\n",cur_pos,sizeof(kl->task->data[cur_pos]));                            
                            trasfer_data_to_ipbuf(kl->full_task_klinfo, kl_suc,cur_pos,suc_pos);
                            printf("trasfer_data_to_ipbuf\n");

                        }
                        else 
                        {
                            printf("Not found in taskMap\n");
                        }
                    }
                    // printf("Done1\n");
                    successorNodeTransfer_time=get_current_time()-successorNodeTransfer_time;

                    AddReadyBuffer_time=get_current_time();
                    childrenToReadyBuffer(*kl)  ;  
                    //printf("childrenToReadyBuffer\n");
                    AddReadyBuffer_time=get_current_time()-AddReadyBuffer_time;    
                }
            }
            /*else if(kl->full_task_klinfo->not_done_count>0) //full task not completed,sub kernels are finished
            {
                printf("notifycallback: kl->full_task_klinfo->not_done_count>0\n");
            }*/

        }

        printf("******Kernel Stat: %d p-d: %d-%d size: %u / %u left: %u freq:%u total-time:%llu nd-time: %llu relative start-end:%llu-%llu *******\n",kl->task->traceID,kl->platform_pos,kl->device_pos,kl->size,kl->task->task_size,kl->not_done_count,kl->frequency,timing,kl->kex.nd_time ,kl->kex.rel_start_time,kl->kex.rel_end_time);

        if(LOG_PROFILE)
        {
            Temperature* tmp=new Temperature();
            tmp->sensors.push_back(1);tmp->sensors.push_back(1);tmp->sensors.push_back(1);tmp->sensors.push_back(1);tmp->sensors.push_back(1);
            tempMap.insert( pair<unsigned long long int, Temperature*>(get_current_time(), tmp) );  
        }
    

        //*******************Mode control in VLSID****************************

        //change mode frequency
        mode_change_time=get_current_time();
        // printf("control_mode: %d\n",kl->control_mode);
        
        if(kl->control_mode==1)    
        {
            local_controller(kl);        
        }
        if(kl->control_mode==3)    
        {
            shift_task(kl);        
        }
            
        if (LOG_LEVEL >=1)
            fprintf(fp,"\tKERNEL %s EXECUTION FINISHED IN DEVICE %d-%d with %uHz frequency size %u offset %u in %llu ms. time.\n",kl->task->taskID.c_str(),kl->platform_pos,kl->device_pos,kl->frequency,kl->size,kl->offset,timing);
            fflush(fp);
        
        mode_change_time = get_current_time()-mode_change_time;




        //***********************AAAI extension****************
        //For safe RL, increase frequency or priority of successor nodes of a DAG    
        if (SAFE)
        {
            safeModeForRL_time=get_current_time();  
            // unsigned long long int local_rel_d=kl->task->deadline+ncb_buffer+time_buffer+(unsigned long long int)(time_factor*kl->task->exTime);
            // printf("local_rel_d: %llu\n",local_rel_d)  ;      
            if(kl->task->isTerminal!=1 && kl->kex.rel_end_time>local_d)
            {
                //change frequency of successors if not at max
                //if(kl->frequency<deviceSpec[kl->platform_pos][kl->device_pos]->highFrequencyBound)
                //{

                    // printf("\tsafe_mode ON\n");
                    
                    
                    // printf("trace: %d jobid: %d ,instance: %d \n",kl->task->traceID,jobid,instance); 
                    // std::vector<std::vector<DAGTime>> l_timeMatrix(NumOfJobs);
                    // for(int job=0;job<NumOfJobs;job++)
                    // {   
                    //     for(int in=0;in<DAGtimeMatrix[job].size();in++)
                    //     {    
                    //         DAGTime inst=*(DAGtimeMatrix[job][in]);
                    //         l_timeMatrix[job].push_back(inst); 
                    //     }
                    // }   
                    // print_DAGTime(l_timeMatrix);

                    // DAGtimeMatrix[jobid][instance]->safe_mode++;   

                    //printf("trace: %d #safe_mode: %d \n",kl->task->traceID,DAGtimeMatrix[jobid][instance]->safe_mode);  
                    auto it = jobMap.find(job);
                    if(it != jobMap.end())
                    {           
                        JobInfo *job = it->second; 
                        //printf("Found in jobMap %d\n",jobid);
                        for(int i=0;i<kl->task->nodes.size();i++)
                        {   for(int j=0;j<job->successors[kl->task->nodes[i]].size();j++)
                            {   
                                int s_node=job->successors[kl->task->nodes[i]][j];
                                //printf("s_node: %d\n",job->successors[kl->task->nodes[i]][j]);
                                std::map<pair<int, int>,std::vector<KernelLaunchInfo*>>::iterator iter;
                                iter = taskMap.find(make_pair(global_dagid,s_node)); 
                                if(iter != taskMap.end())
                                {
                                    //printf("Successor kernel launch info is found\n");
                                    KernelLaunchInfo* s_kl=iter->second[0]; 
                                    s_kl->priority=1;    
                                    //printf("Successor kernel priority %d:\n",s_kl->priority); 
                                                            
                                }
                                else
                                {
                                    printf("Successor kernel launch info is not found\n");
                                }
                            }
                        }
                    }
            }
            safeModeForRL_time=get_current_time()-safeModeForRL_time;
        }
        


        //*************** Thermal recovery technique VLSID ext.******************

        //check thermal profile for this task
        std::pair<int, int>  thermal_p=check_thermal_violation(kl->kex.rel_start_time,kl->kex.rel_end_time);

        if(thermal_p.first==0)
            printf("--------NO THERMAL VIOLATION---------\n");
        else if(thermal_p.first==1)
        {   
            printf("--------MINOR THERMAL VIOLATION IN %s PLATFORM --------\n",(thermal_p.second==2?"BOTH":(thermal_p.second==0?"GPU":"CPU")));
            if(LOCAL_RECOVERY_ENABLE)
                local_recovery(thermal_p,kl);

        }
        else if(thermal_p.first>1 )
        {   
            if(LOCAL_RECOVERY_ENABLE)
                local_recovery(thermal_p,kl);
            kl->task->totalMajorViolation=kl->task->totalMajorViolation+1;
            kl->task->majorViolationPerHP=kl->task->majorViolationPerHP+1;
            if(kl->task->totalMajorViolation>2*kl->task->majorViolationPerHP)
                global_recovery_tasks.push_back(make_pair(kl->task->dagInfo->globalDAGID,kl->task->dagInfo->instanceID));

            if(thermal_p.first==2)
                printf("--------MAJOR THERMAL VIOLATION IN %s PLATFORM --------\n",(thermal_p.second==2?"BOTH":(thermal_p.second==0?"GPU":"CPU")));
            else if(thermal_p.first==3)
                printf("--------MAJOR THERMAL VIOLATION IN %s AND MINOR VIOLATION IN %s--------\n",(thermal_p.second==0?"GPU":"CPU"),(thermal_p.second==0?"CPU":"GPU"));
            

        }

        
        
    }

    ncb_all_calculations=get_current_time()-ncb_all_calculations;
    //Mutex and global vriable updates    

    unsigned long long int locks_print_time=get_current_time();

    // number of kernels update for host synchronise
    mtx_nlock.lock(); 
    nKernels--;
    mtx_nlock.unlock();  

    //device available signal
    mtx_devlock.lock();         
    dev_available[kl->platform_pos][kl->device_pos]=true;
    mtx_devlock.unlock();
    //printf("#####notify_callback_update_release: dev available updated dev_available[%d][%d]\n",kl->platform_pos,kl->device_pos,(dev_available[kl->platform_pos][kl->device_pos]?1:0));
    cv_dev_available.notify_one();


    
    if (LOG_LEVEL>=1){
        fprintf(fp,"notify_callback_update_release: END: %llu \n",get_current_time()); 
        fflush(fp);
    }   
        
    if(kl->platform_pos==1)
        notify_cb_buffer_c+=get_current_time()-end;
    else
        notify_cb_buffer_g+=get_current_time()-end;


    
    kl->kex.notify_callback_rel_end_time=get_current_time();        

    if(kl->task->traceID!=-1 )
    {
        fprintf(t_result," %llu %llu %llu \n",kl->kex.rel_start_time,kl->kex.rel_end_time,kl->kex.notify_callback_rel_end_time);
        fflush(t_result);
        if(LOG_PROFILE==1)
        {
            //dump_profile_event_timing(kl->kex);

            //dispatch start time, notify start time, notify finish time
            
            // fclose (t_result);            

            locks_print_time=get_current_time()-locks_print_time;
            printf("------------------------------------------------------------\n");
            printf("Execution time: %llu \n",kl->kex.notify_callback_rel_start_time-write_buffers_begin);
            printf("Notify_callback time start end: %llu - %llu: %llu\n",kl->kex.notify_callback_rel_end_time-write_buffers_begin,end-write_buffers_begin,(kl->kex.notify_callback_rel_end_time-write_buffers_begin)-(end-write_buffers_begin));
            printf("Notify_callback time analysis release-calculation-lockprint: \n %llu - %llu - %llu\n",release_buff_Kevent_time,ncb_all_calculations,locks_print_time);
            printf("freqChange_time: %llu\n",freqChange_time);
            printf("node_mark_time : %llu\n",node_mark_time);
            printf("terminalNodeCal_time : %llu\n",terminalNodeCal_time);
            printf("successorNodeTransfer_time : %llu\n",successorNodeTransfer_time);
            printf("releaseHostArray_time : %llu\n",releaseHostArray_time);
            printf("AddReadyBuffer_time: %llu\n",AddReadyBuffer_time); 
            printf("partitioning_time : %llu\n",partitioning_time);
            printf("safeModeForRL_time : %llu\n",safeModeForRL_time);
            printf("mode_change_time: %llu\n",mode_change_time);
            printf("------------------------------------------------------------\n");
        }        
    }
    printf("notify_callback_update_release for task %d: END: %llu \n++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n",kl->task->traceID,get_current_time());  

}

void dump_output(KernelLaunchInfo* kl)
{
    fprintf(o_result,"Output data: Kernel name: %s_%u \n\n",kl->task->taskID.c_str(),kl->size);
    
        //datasize=calculate_op_buffer_size(size,i,ki);
        //buffer_offset=calculate_op_buffer_offset(dataoffset,i,ki); 
        //data[i + ki.inputBuffers.size()] + buffer_offset
        KernelInfo* ki = (kl->task->kernels[0]);
        printf("outputBuffers.size: %d, kl->size: %u\n",ki->outputBuffers.size(),kl->size);
        for (int i = 0; i < ki->outputBuffers.size(); ++i)
        {
            int offset=kl->size;
            unsigned int buffer_offset=calculate_op_buffer_offset(offset,i,*ki); 
            void* data_output_start=kl->task->data[i + ki->inputBuffers.size()] + buffer_offset;
            printf("data_output_start: %u, data0: %f data1: %f\n",data_output_start,*(float *)data_output_start,*((float *)data_output_start+1));
            for (int j=0;j<std::get<1>(ki->outputBuffers.at(i));j++)
            {
               
                fprintf(o_result,"%f\n",*((float *)data_output_start+j));

            }
            fprintf(o_result,"\n\n");
        }
    
    fflush(o_result);
    
}

//return the recovery mode selected //0 no change, 1 freq tuning, 2 migration, 3 idle slot insertion
int local_recovery(std::pair<int, int> t_violation, KernelLaunchInfo* kl)
{

    //identify next kernel in violated pltform in ready buffer 
    KernelLaunchInfo* next_klinfo;
    int c_platform=-1;
    if(t_violation.second==0 && ready_buffer[PLATFORM_GPU][0].size()>0)
    {
        next_klinfo=ready_buffer[PLATFORM_GPU][0][0];
        c_platform=PLATFORM_CPU;
        printf("local_recovery GPU: next in GPU\n");
    }
    else if(t_violation.second==0 && ready_buffer[PLATFORM_GPU][0].size()==0)
    {
        printf("local_recovery GPU: no kernel in ready_buffer\n");
        next_klinfo=NULL;
    }
    else if(t_violation.second==1 && ready_buffer[PLATFORM_CPU][0].size()>0)
    {
        next_klinfo=ready_buffer[PLATFORM_CPU][0][0];
        c_platform=PLATFORM_GPU;
        printf("local_recovery CPU: next in CPU\n");
    }
    else if(t_violation.second==1 && ready_buffer[PLATFORM_CPU][0].size()==0)
    {
        printf("local_recovery CPU: no kernel in ready_buffer\n");
        next_klinfo=NULL;
    }
    else if(t_violation.second==2)
    {
        if(ready_buffer[PLATFORM_CPU][0].size()>0)
        {   next_klinfo=ready_buffer[PLATFORM_CPU][0][0];
            printf("local_recovery in BOTH: next in CPU\n");
        }
        else if(ready_buffer[PLATFORM_GPU][0].size()>0)
        {   next_klinfo=ready_buffer[PLATFORM_GPU][0][0];
            printf("local_recovery in BOTH: next in GPU\n");
        }
    }
    else 
    {    next_klinfo=NULL;
         printf("local_recovery not in any condition\n");
    }

    int recovery_mode=0;
    if(next_klinfo!=NULL)
    {   recovery_mode=find_local_recovery_mode(t_violation,next_klinfo);
        printf("local_recovery : recovery_mode: %d\n",recovery_mode);
    }
    else
        printf("local_recovery : No recovery_mode\n");

    
   
    return recovery_mode;
}

int find_local_recovery_mode(std::pair<int, int> t_violation, KernelLaunchInfo* next_klinfo)
{
    //find the local deadline for next task
    unsigned long long int local_dl=BUFFER_TIME+get_current_time()+get_local_deadline(next_klinfo);
    //printf("find_local_recovery_mode: local_dl: %llu \n",local_dl);

    std::vector<unsigned int> WCET_p=get_partioined_WCET(next_klinfo);

    int r_mode=-1;
    if( t_violation.second==1 || t_violation.second==0) //minor violation in CPU
    {
        int h_p=t_violation.second; //hot platform
        int c_p=(t_violation.second==0?1:0); //cool platform
        //printf("find_local_recovery_mode: h_p %d c_p %d \n", h_p,c_p);
        int freq_index=get_frequency_index(h_p,next_klinfo->frequency) ;
        //printf("find_local_recovery_mode: freq_index %d, wcet[%d] %llu\n",freq_index,h_p,WCET_p[h_p]);
        
        unsigned int idle_slot=local_dl-(estimate_WCET(WCET_p[h_p],freq_index,h_p)+BUFFER_TIME+get_current_time());        
        unsigned int estimated_extme=estimate_WCET(WCET_p[h_p],freq_index,h_p);
        unsigned int estimated_extme_freq_diminished=estimate_WCET(WCET_p[h_p],(freq_index-1>0?freq_index-1:0),h_p);

        printf("find_local_recovery_mode: idle_slot = l_dl(%llu) - ex_time(%llu: %u+%u+%llu) = %u\n", local_dl,(estimate_WCET(WCET_p[h_p],freq_index,h_p)+BUFFER_TIME+get_current_time()),estimate_WCET(WCET_p[h_p],freq_index,h_p),BUFFER_TIME, get_current_time(),idle_slot);
        printf("find_local_recovery_mode: migration = r_buf[%d].sz: %d dev_avl: %d est_ex_time: %llu\n",c_p,ready_buffer[c_p][0].size(),(dev_available[c_p][0]?1:0),estimate_WCET(WCET_p[c_p],deviceSpec[c_p][0]->device_config.size()-1,c_p)+get_current_time());
        printf("find_local_recovery_mode: frequency = freq_index %d estimated_extme %u estimated_extime_less freq %u final _time %llu deadline %llu const_factro_add %llu\n",freq_index,estimated_extme,estimated_extme_freq_diminished,estimated_extme_freq_diminished+BUFFER_TIME+get_current_time(),local_dl ,BUFFER_TIME+get_current_time());
        
        //migration mode if c_platform is available (mode 2)
        if(ready_buffer[c_p][0].size()==0 && dev_available[c_p][0] && estimate_WCET(WCET_p[c_p],deviceSpec[c_p][0]->device_config.size()-1,c_p)+get_current_time()<local_dl)
        {   r_mode=2;
            next_klinfo->tmp_platform_pos=c_p;
            next_klinfo->tmp_device_pos=0;
            next_klinfo->recovery_mode=2;
            printf("find_local_recovery_mode: migration %d\n",next_klinfo->recovery_mode);
        }
        //idle slot insertion (mode3)
        else if(idle_slot>MIN_IDLE_SLOT)    
        {
            r_mode=3;
            next_klinfo->tmp_idle_slot=MIN_IDLE_SLOT;
            next_klinfo->recovery_mode=3;
            printf("find_local_recovery_mode: idle_slot %d\n",next_klinfo->recovery_mode);
        }
        //frequency
        else if(freq_index>0 && estimated_extme_freq_diminished+BUFFER_TIME+get_current_time()<local_dl) //tune frequency (mode1)
        {
            r_mode=1;
            next_klinfo->tmp_frequency=deviceSpec[h_p][0]->device_config[freq_index-1]->frequency;
            next_klinfo->recovery_mode=1;
            printf("find_local_recovery_mode %d : frequency %u\n",next_klinfo->recovery_mode,next_klinfo->tmp_frequency);
        }
        else
        {
            r_mode=0;
            next_klinfo->recovery_mode=0;
        }
    }
    else if ( t_violation.second==2)// violation in both
    {
        int h_p=(ready_buffer[PLATFORM_CPU][0].size()>0?1:0);//hot platform
        int c_p=1-h_p; //cool platform

        int freq_index=get_frequency_index(h_p,next_klinfo->frequency) ;
        unsigned int idle_slot=local_dl-(estimate_WCET(WCET_p[h_p],freq_index,h_p)+BUFFER_TIME+get_current_time());        
        unsigned int estimated_extme=estimate_WCET(WCET_p[h_p],freq_index,h_p);
        unsigned int estimated_extme_freq_diminished=estimate_WCET(WCET_p[h_p],(freq_index-1>0?freq_index-1:0),h_p);



        printf("find_local_recovery_mode: idle_slot = l_dl(%llu) - ex_time(%llu: %u+%u+%llu) = %u\n", local_dl,(estimate_WCET(WCET_p[h_p],freq_index,h_p)+BUFFER_TIME+get_current_time()),estimate_WCET(WCET_p[h_p],freq_index,h_p),BUFFER_TIME, get_current_time(),idle_slot);
        printf("find_local_recovery_mode: frequency = freq_index %d estimated_extme %u estimated_extime_less freq %u final _time %llu deadline %llu const_factro_add %llu\n",freq_index,estimated_extme,estimated_extme_freq_diminished,estimated_extme_freq_diminished+BUFFER_TIME+get_current_time(),local_dl ,BUFFER_TIME+get_current_time());
        

        if(idle_slot>MIN_IDLE_SLOT) 
        {
            r_mode=3;
            next_klinfo->tmp_idle_slot=MIN_IDLE_SLOT;
            next_klinfo->recovery_mode=3;
            printf("find_local_recovery_mode: idle_slot %d\n",next_klinfo->recovery_mode);
        
        }
        else if(freq_index>0 && estimated_extme_freq_diminished+BUFFER_TIME+get_current_time()<local_dl) //tune frequency (mode1)
        {
            r_mode=1;
            next_klinfo->tmp_frequency=deviceSpec[h_p][0]->device_config[freq_index-1]->frequency;
            next_klinfo->recovery_mode=1;
            printf("find_local_recovery_mode %d: frequency %d\n",next_klinfo->recovery_mode,next_klinfo->tmp_frequency);
        
        }
        else
        {
            r_mode=0;
            next_klinfo->recovery_mode=0;
        }
    }
    return r_mode;
}

unsigned long long int get_local_deadline(KernelLaunchInfo* klinfo)
{
    float task_threads_left=(float)klinfo->full_task_klinfo->not_done_count/klinfo->full_task_klinfo->size;
    unsigned long long int task_left_time=klinfo->task->deadline-get_current_time();
    unsigned long long int local_dl=(unsigned long long int)(klinfo->task->deadline-klinfo->task->arrival) * task_threads_left;
    
    //printf("get_local_deadline: task_threads_left: %f  task_left_time: %llu local_dl: %llu\n",task_threads_left,task_left_time,local_dl);
    
    return local_dl;
}

std::vector<unsigned int> get_partioined_WCET(KernelLaunchInfo* klinfo)
{
    std::vector<unsigned int> wcet;
    float task_threads_left=(float)klinfo->full_task_klinfo->not_done_count/klinfo->full_task_klinfo->size;
    wcet.push_back((unsigned int)(klinfo->task->WCET[PLATFORM_GPU]*task_threads_left));
    wcet.push_back((unsigned int)(klinfo->task->WCET[PLATFORM_CPU]*task_threads_left));
    return wcet;
}

int get_frequency_index(int platform, unsigned int frequency)
{
    for(int i=0;i<deviceSpec[platform][0]->device_config.size();i++)
    {
        if(deviceSpec[platform][0]->device_config[i]->frequency ==frequency)
        {   
            //printf("get_frequency_index: frequency %u index %d\n",frequency,i);
            return i;
        }
    }
    return -1;
}

unsigned long long int estimate_WCET(unsigned long long int wcet,int freq_index,int platform)
{
    //printf("estimate_WCET: wcet %llu freq_index %d freq %u speedup %f\n",wcet,freq_index,deviceSpec[platform][0]->device_config[freq_index]->frequency,deviceSpec[platform][0]->device_config[freq_index]->speedup);
    unsigned long long int est_wcet= (unsigned long long int)( (float)wcet/deviceSpec[platform][0]->device_config[freq_index]->speedup);
    //printf("estimate_WCET: est_wcet %llu\n",est_wcet);
    return est_wcet;
}

int global_recovery(std::vector<std::pair<int, int>> violated_tasks)
{
    for(int i=0;i<violated_tasks.size();i++)
    {
        std::map<pair<int, int>,std::vector<KernelLaunchInfo*>>::iterator iter;
        iter = taskMap.find(violated_tasks[i]);
        if(iter != taskMap.end()) //if found
        {
            std::vector<KernelLaunchInfo*> kl_vector_v = iter->second; 
            KernelLaunchInfo* klinfo_v=kl_vector_v[0];
            unsigned long long int start=klinfo_v->kex.rel_start_time;
            unsigned long long int end=klinfo_v->kex.rel_end_time;
                        

            std::vector<float> utilisation(NumOfPlatforms, 0.0);
            std::vector<unsigned int> FLOPS_processed(NumOfPlatforms, 0);

            for (auto itr = taskMap.begin(); itr != taskMap.end(); ++itr)       
            {
                std::vector <KernelLaunchInfo*> klinfo_vectors=itr->second;
                if((klinfo_vectors[0]->kex.rel_start_time<start && klinfo_vectors[0]->kex.rel_end_time>end) || (klinfo_vectors[0]->kex.rel_start_time>start && klinfo_vectors[0]->kex.rel_end_time<end) || (klinfo_vectors[0]->kex.rel_start_time>start && klinfo_vectors[0]->kex.rel_end_time>start && klinfo_vectors[0]->kex.rel_end_time<end) || (klinfo_vectors[0]->kex.rel_start_time>start && klinfo_vectors[0]->kex.rel_start_time<end && klinfo_vectors[0]->kex.rel_end_time>end) )
                {
                    if(klinfo_vectors.size()>0)
                    {
                        for(int j=1;j<klinfo_vectors.size();j++)
                        { 
                            KernelLaunchInfo* kl = klinfo_vectors[j];       
                            if(kl->kex.rel_start_time>start && kl->kex.rel_end_time<end)
                            {
                                utilisation[kl->platform_pos]=utilisation[kl->platform_pos]+(float)(kl->kex.rel_end_time-kl->kex.rel_start_time)/(end-start);
                                FLOPS_processed[kl->platform_pos]=FLOPS_processed[kl->platform_pos]+kl->size*kl->task->kernels[0]->FLOPS_per_thread;
                                printf("global_recovery: utilisation[%d]: %f\n",kl->platform_pos,utilisation[kl->platform_pos]);
                                printf("global_recovery: FLOPS[%d]: %u\n",kl->platform_pos,FLOPS_processed[kl->platform_pos]);
               
                            }
                        }
                    }
                    else
                    {
                        KernelLaunchInfo* kl = klinfo_vectors[0];
                        utilisation[kl->platform_pos]=utilisation[kl->platform_pos]+(float)(kl->kex.rel_end_time-kl->kex.rel_start_time)/(end-start);
                        FLOPS_processed[kl->platform_pos]=FLOPS_processed[kl->platform_pos]+kl->size*kl->task->kernels[0]->FLOPS_per_thread;
                        printf("global_recovery: utilisation[%d]: %f\n",kl->platform_pos,utilisation[kl->platform_pos]);
                        printf("global_recovery: FLOPS[%d]: %u\n",kl->platform_pos,FLOPS_processed[kl->platform_pos]);
               
                    }
                }
            }

            enter_global_recovery_mode(kl_vector_v, utilisation,FLOPS_processed);

            klinfo_v->task->majorViolationPerHP=0;

        }
        else 
        {
            print_task_map();
            printf("global_recovery: Not found in taskMap g_id: %d instance: %d\n",violated_tasks[i].first, violated_tasks[i].second);
        }
    }
    return 0;
}





int enter_global_recovery_mode (std::vector<KernelLaunchInfo*> kl_vector_v, std::vector<float> utilisation,std::vector<unsigned int> FLOPS_processed)
{
    
    //frequency mode    
    if( FREQUENCY_MODE || (utilisation[PLATFORM_GPU]>utilisation_threshold && utilisation[PLATFORM_CPU]>utilisation_threshold && FLOPS_processed[PLATFORM_GPU]>flops_threshold && FLOPS_processed[PLATFORM_CPU]>flops_threshold )) //utilisation is high in both and flops r high in both
    {
        unsigned int f=kl_vector_v[0]->frequency;
        printf("Current frequency: %u\n",f);
            
        if(kl_vector_v.size()>1)
        {
            unsigned int min_f=f;
            for(int i=1;i<kl_vector_v.size();i++)
            {   
            // printf("sub_kernel=%d, total partiton size: %d freq=%u\n",i,kl_vector_v.size(),kl_vector_v[i]->tmp_frequency); 
                if(kl_vector_v[i]->tmp_frequency <f)
                    min_f=kl_vector_v[i]->tmp_frequency;
            }

            kl_vector_v[0]->frequency=min_f;
            for(int i=1;i<kl_vector_v.size();i++)
            {    
                if(min_f==0 || min_f==f)
                    kl_vector_v[i]->frequency= (kl_vector_v[0]->platform_pos==0?350000000:f-100000000);
                else
                    kl_vector_v[i]->frequency= min_f;
            }

            printf("enter_global_recovery_mode: new frequency set to %u from %u \n",kl_vector_v[0]->frequency,f);
            
        }
        else if(kl_vector_v.size()==1)
        {            
            if(CREATE_SUB_KERNEL==0)
                kl_vector_v[0]->frequency= (kl_vector_v[0]->platform_pos==0?350000000:f-100000000);
            
            else 
                kl_vector_v[0]->frequency=kl_vector_v[0]->tmp_frequency;

            printf("enter_global_recovery_mode: new frequency set from %u to %u\n",f,kl_vector_v[0]->frequency);
            
        }
        
    }
    //migration mode 
    if (MIGRATION_MODE || ( (utilisation[PLATFORM_GPU]>utilisation_threshold && utilisation[PLATFORM_CPU]<utilisation_threshold && FLOPS_processed[PLATFORM_GPU]<=flops_threshold) && (utilisation[PLATFORM_CPU]>utilisation_threshold && utilisation[PLATFORM_GPU]<utilisation_threshold && FLOPS_processed[PLATFORM_CPU]<=flops_threshold ) )) //utilisation is high in one and low in another and flops are low in that platform
    {
        if(kl_vector_v.size()>1)
        {
            for(int i=1;i<kl_vector_v.size();i++)
            {
                if(kl_vector_v[i]->platform_pos!=kl_vector_v[i]->tmp_platform_pos)
                {
                    kl_vector_v[i]->platform_pos=kl_vector_v[i]->tmp_platform_pos;
                    kl_vector_v[i]->device_pos=kl_vector_v[i]->tmp_device_pos;
                    printf("enter_global_recovery_mode: new platform set from %d to %d \n",kl_vector_v[i]->platform_pos,kl_vector_v[i]->tmp_platform_pos);
                }
                else
                {
                    printf("enter_global_recovery_mode: no new platform set\n");
                }
            }
        }
        else if(kl_vector_v.size()==1 && kl_vector_v[0]->platform_pos!=kl_vector_v[0]->tmp_platform_pos)
        {
            printf("enter_global_recovery_mode: new platform set from %d to %d \n",kl_vector_v[0]->platform_pos,kl_vector_v[0]->tmp_platform_pos);
            
            if(CREATE_SUB_KERNEL==0)
            {
                kl_vector_v[0]->platform_pos= (kl_vector_v[0]->platform_pos==0?1:0);
                kl_vector_v[0]->device_pos= 0;
            }
            else
            {    kl_vector_v[0]->platform_pos=kl_vector_v[0]->tmp_platform_pos;
                kl_vector_v[0]->device_pos=kl_vector_v[0]->tmp_device_pos;
            }
            
        }
        else
        {
            printf("enter_global_recovery_mode: Migration: No platform change\n");
        }
        
    }  
   // else if(utilisation[PLATFORM_CPU]>utilisation_threshold && utilisation[PLATFORM_GPU]<utilisation_threshold && FLOPS_processed[PLATFORM_CPU]<=flops_threshold ) //utilisation is high in one and low in another and flops are low in that platform
 
    //idle shift insertion 
    if(TASK_SHIFT_MODE || (utilisation[PLATFORM_GPU]>utilisation_threshold && utilisation[PLATFORM_CPU]>utilisation_threshold && FLOPS_processed[PLATFORM_GPU]<=flops_threshold && FLOPS_processed[PLATFORM_CPU]<=flops_threshold) ) //utilisation is high in both and flops are low
    {
        if(kl_vector_v.size()>1)
        {
            for(int i=1;i<kl_vector_v.size();i++)
            {
                if(kl_vector_v[i]->tmp_idle_slot>0)
                {
                    kl_vector_v[i]->recovery_mode=3;
                    printf("enter_global_recovery_mode: idle slot set\n");
                }
                else
                {
                    printf("enter_global_recovery_mode: no idle slot set\n");
                }
            }
        }
        else if(kl_vector_v.size()==1 && kl_vector_v[0]->tmp_idle_slot>0)
        {
            printf("enter_global_recovery_mode: idle slot set\n");
            kl_vector_v[0]->recovery_mode=3;
            
        }
        else
        {
            printf("enter_global_recovery_mode: Migration: No platform change\n");
        }
    }
    else 
    {
        printf("enter_global_recovery_mode: other condition: utilisation[0]: %c (%f) ; utilisation[1]: %c (%f) ; FLOPS_processed[0]:  %c (%u) ; FLOPS_processed[1]:  %c (%u) \n", (utilisation[PLATFORM_GPU]>utilisation_threshold?'H':'L') ,utilisation[PLATFORM_GPU],(utilisation[PLATFORM_CPU]>utilisation_threshold?'H':'L') ,utilisation[PLATFORM_CPU],(FLOPS_processed[PLATFORM_GPU]>flops_threshold?'H':'L'),FLOPS_processed[PLATFORM_GPU],(FLOPS_processed[PLATFORM_CPU]>flops_threshold?'H':'L'),FLOPS_processed[PLATFORM_CPU]);
    }

    return 0;
}



void dump_profile_statistics(KernelLaunchInfo *kl,std::ofstream &ofs)
{
    std::string timing_information = "";
    KernelExecutionInfo kex = kl->kex;
    KernelEvents ke = kl->ke;
    cout<<"KERNEL EVENTS : write size " <<ke.write.size() << " read size " <<ke.read.size()<<"\n";
    timing_information += ke.dump_times();
    stringstream ss;
    ss <<"getimage_time:" << kex.get_image_end_time - kex.get_image_start_time << ";host_time:" << kex.rel_end_time-kex.rel_start_time <<"\n";
    timing_information +=ss.str();
    PERSISTENCE_DEBUG cout<<timing_information;
    ofs << timing_information;
    // ofs.close();    
}
void dump_total_time(KernelLaunchInfo *kl,std::ofstream &ofs)
{
    std::string timing_information = "";
    KernelExecutionInfo kex = kl->kex;
    stringstream ss;
    ss <<"getimage_time:" << kex.get_image_end_time - kex.get_image_start_time << ";host_time:" << kex.rel_end_time-kex.rel_start_time <<"\n";
    timing_information +=ss.str();
    PERSISTENCE_DEBUG cout<<timing_information;
    ofs << timing_information;
    // ofs.close();    
}
void dump_execution_time_statistics(KernelLaunchInfo *kl,int dag_id, int task_id, std::ofstream &ofs)
{

    KernelExecutionInfo kex = kl->kex;
    KernelEvents ke = kl->ke;

    ke.print();
    printf("-------------------------TIMESTAMP DUMP BEGINS---------------------------------\n");
    ke.dump();
    printf("-------------------------TIMESTAMP DUMP ENDS-----------------------------------\n");
    // unsigned long long int w = convert_to_relative_time(kex.writeEnd,kex.writeStart);
    // unsigned long long int e = convert_to_relative_time(kex.ndEnd,kex.ndStart);
    // unsigned long long int r = convert_to_relative_time(kex.readEnd,kex.readStart);
    
    // unsigned long long int w_queue_submit = convert_to_relative_time(kex.writeSubmit,kex.writeQueued);
    // unsigned long long int w_submit_start = convert_to_relative_time(kex.writeStart,kex.writeSubmit);

    // unsigned long long int e_queue_submit = convert_to_relative_time(kex.ndSubmit,kex.ndQueued);
    // unsigned long long int e_submit_start = convert_to_relative_time(kex.ndStart,kex.ndSubmit);

    // unsigned long long int r_queue_submit = convert_to_relative_time(kex.readSubmit,kex.readQueued);
    // unsigned long long int r_submit_start = convert_to_relative_time(kex.readStart,kex.readSubmit);

    // unsigned long long int w_delay = convert_to_relative_time(kex.writeStart,kex.writeQueued);
    // unsigned long long int e_delay = convert_to_relative_time(kex.ndStart,kex.writeEnd);// - convert_to_relative_time(kex.writeEnd,kex.writeQueued);
    // unsigned long long int r_delay = convert_to_relative_time(kex.readStart,kex.writeQueued) - convert_to_relative_time(kex.ndEnd,kex.writeQueued);
    
    unsigned long long int host_side_time = kex.notify_callback_rel_start_time-kex.write_buffers_start;
    double percent_host_overhead= (double)(host_side_time)/convert_to_relative_time(kex.readEnd,kex.writeQueued);
    // printf("Write: queue-->submit delay: %llu, submit-->start delay %llu\n",w_queue_submit,w_submit_start);
    // printf("Execute: queue-->submit delay: %llu, submit-->start delay %llu\n",e_queue_submit,e_submit_start);
    // printf("Read: queue-->submit delay: %llu, submit-->start delay %llu\n",r_queue_submit,r_submit_start);
    // printf("Write time %llu Execution Time %llu Read Time %llu ",w,e,r);
    unsigned long long int callback_overhead = kex.notify_callback_rel_end_time-kex.rel_end_time;
    double percent_callback_overhead = (double)callback_overhead/host_side_time;
    // printf("SUBMIT-->START for NDRange %llu\n",e_submit_start);
    // std::ofstream ofs(filename,std::ofstream::out);
    // ofs << dag_id <<"\t\t"<<task_id<<"\t\t"<<w_delay<<"\t\t"<<w<<"\t\t"<<e_delay<<"\t\t"<<e<<"\t\t"<<r_delay<<"\t\t"<<r<<"\t\t"<<w+e+r<<"\t\t"<<host_side_time<<"\t\t"<<percent_host_overhead<<"\t\t"<<callback_overhead<<"\t\t"<<percent_callback_overhead<<"\n";
    // printf("%d \t\t %d \t\t %llu \t\t %llu \t\t %llu \t\t %llu \t\t %llu \t\t %llu \t\t %llu \t\t %llu  \t\t %lf \t\t %llu \t\t %lf\n",dag_id,task_id,w_delay,w,e_delay,e,r_delay,r,w+e+r,host_side_time,percent_host_overhead,callback_overhead,percent_callback_overhead);
    
    // dump_profile_event_timing(kex);
    
    // unsigned long long int end = kex.rel_end_time;
    // unsigned long long int write_buffers_begin = kex.write_buffers_start;
    // printf("------------------------------------------------------------\n");
    // double percent_host_overhead= (double)(kex.notify_callback_rel_start_time-write_buffers_begin)/convert_to_relative_time(kex.readEnd,kex.writeQueued);
    // printf("Execution time from host perspective: %llu \n",kex.notify_callback_rel_start_time-write_buffers_begin);
    // printf("Notify_callback time start end: %llu - %llu: %llu\n",kex.notify_callback_rel_end_time-write_buffers_begin,end-write_buffers_begin,(kex.notify_callback_rel_end_time-write_buffers_begin)-(end-write_buffers_begin));
    // printf("Host Overhead %lf\n",percent_host_overhead);
    // double percent_callback_overhead = (double)((kex.notify_callback_rel_end_time-write_buffers_begin)-(end-write_buffers_begin)) / (kex.notify_callback_rel_end_time-write_buffers_begin);
    // printf("Notify Callback Overhead %lf\n",percent_callback_overhead);    
    // printf("freqChange_time: %llu\n",kex.frequency_change_time);
    
    // printf("------------------------------------------------------------\n");


}
unsigned long long int convert_to_relative_time( unsigned long long int t,unsigned long long int ref)
{
    // return (t/1000-START_TIME);
    return (t-ref)/1000;
}
void dump_profile_event_timing(KernelExecutionInfo kex)
{
    // printf("Dispatch_start: %llu\n",kex.rel_start_time);
    // printf("N_callback_start: %llu\n",kex.notify_callback_rel_start_time);
    // printf("N_callback_end: %llu\n",kex.notify_callback_rel_end_time);
    // printf("Read_end-Dispatch_start: %llu\n",kex.read_end-kex.rel_start_time);
    // printf("N_callback_end-Dispatch_start: %llu\n\n",kex.notify_callback_rel_end_time-kex.rel_start_time);


    // unsigned long long int ref=kex.writeQueued;

    // //write
    // printf("write_start_h write_start_d nd_start_d write_time_t\n");
    // printf("%llu %llu %llu %llu \n",kex.write_start_h, kex.write_start , kex.write_end , kex.write_time);
    // printf("writeQueued_p writeSubmit_p writeStart_p writeEnd_p writeTime_p\n");
    // printf("%llu %llu %llu %llu %llu\n\n",convert_to_relative_time(kex.writeQueued,ref),convert_to_relative_time(kex.writeSubmit,ref),convert_to_relative_time(kex.writeStart,ref),convert_to_relative_time(kex.writeEnd,ref),convert_to_relative_time(kex.writeEnd,ref)-convert_to_relative_time(kex.writeStart,ref));

    // //nd_range
    // printf("nd_start_h nd_start_d read_start_d nd_time_t\n");
    // printf("%llu %llu %llu %llu \n",kex.nd_start_h, kex.nd_start , kex.nd_end , kex.nd_time);
    // printf("ndQueued_p ndSubmit_p ndStart_p ndEnd_p ndTime_p\n");
    // printf("%llu %llu %llu %llu %llu\n\n",convert_to_relative_time(kex.ndQueued,ref),convert_to_relative_time(kex.ndSubmit,ref),convert_to_relative_time(kex.ndStart,ref),convert_to_relative_time(kex.ndEnd,ref),convert_to_relative_time(kex.ndEnd,ref)-convert_to_relative_time(kex.ndStart,ref));


    // //read
    // printf("read_start_h read_start_d ncb_start read_time_t\n");
    // printf("%llu %llu %llu %llu \n",kex.read_start_h, kex.read_start , kex.read_end , kex.read_time);
    // printf("readQueued_p readSubmit_p readStart_p readEnd_p readTime_p\n");
    // printf("%llu %llu %llu %llu %llu\n\n",convert_to_relative_time(kex.readQueued,ref),convert_to_relative_time(kex.readSubmit,ref),convert_to_relative_time(kex.readStart,ref),convert_to_relative_time(kex.readEnd,ref),convert_to_relative_time(kex.readEnd,ref)-convert_to_relative_time(kex.readStart,ref));
    unsigned long long int w = convert_to_relative_time(kex.writeEnd,kex.writeStart);
    unsigned long long int e = convert_to_relative_time(kex.ndEnd,kex.ndStart);
    unsigned long long int r = convert_to_relative_time(kex.readEnd,kex.readStart);
    
    unsigned long long int w_queue_submit = convert_to_relative_time(kex.writeSubmit,kex.writeQueued);
    unsigned long long int w_submit_start = convert_to_relative_time(kex.writeStart,kex.writeSubmit);

    unsigned long long int e_queue_submit = convert_to_relative_time(kex.ndSubmit,kex.ndQueued);
    unsigned long long int e_submit_start = convert_to_relative_time(kex.ndStart,kex.ndSubmit);

    unsigned long long int r_queue_submit = convert_to_relative_time(kex.readSubmit,kex.readQueued);
    unsigned long long int r_submit_start = convert_to_relative_time(kex.readStart,kex.readSubmit);


    
    printf("Write: queue-->submit delay: %llu, submit-->start delay %llu\n",w_queue_submit,w_submit_start);
    printf("Execute: queue-->submit delay: %llu, submit-->start delay %llu\n",e_queue_submit,e_submit_start);
    printf("Read: queue-->submit delay: %llu, submit-->start delay %llu\n",r_queue_submit,r_submit_start);
    printf("Write time %llu Execution Time %llu Read Time %llu ",w,e,r);

    printf("%OpenCL execution time (w+e+r) %llu, Delay %llu\n",convert_to_relative_time(kex.readEnd,kex.writeQueued),convert_to_relative_time(kex.readEnd,kex.writeQueued)-(w+e+r));
    
}
void print_task_map()
{
    for (auto itr = taskMap.begin(); itr != taskMap.end(); ++itr) { 
        for(int i=0;i<itr->second.size();i++)
        {       cout << '\t' << itr->first.first 
                << '\t' << itr->first.second
                << '\t' << itr->second[i] << '\n'; 
        }
    } 
}

void print_job_map()
{
    for (auto itr = jobMap.begin(); itr != jobMap.end(); ++itr) { 
        cout << '\t' << itr->first
             << '\t' << itr->second << '\n'; 
    } 
}

std::pair<int, int> get_buffer_index(KernelInfo* cur_kl, KernelInfo* suc_kl,int cur_pos,int suc_pos)
{
    int c_index=-1,s_index=-1;
    for (int i = 0; i < cur_kl->outputBuffers.size(); ++i)
    {
        std::string type = std::get<0>(cur_kl->outputBuffers.at(i));
        unsigned int size = std::get<1>(cur_kl->outputBuffers.at(i));
        unsigned int pos = std::get<2>(cur_kl->outputBuffers.at(i));
        if(pos==cur_pos)
        {   c_index=i;
            break;
        }
    }

    for (int i = 0; i < suc_kl->inputBuffers.size(); ++i)
    {
        std::string type = std::get<0>(suc_kl->inputBuffers.at(i));
        unsigned int size = std::get<1>(suc_kl->inputBuffers.at(i));
        unsigned int pos = std::get<2>(suc_kl->inputBuffers.at(i));
        if(pos==suc_pos)
        {   s_index=i;
            break;
        }
    }
    printf("inp_sz %d c_index: %d s_index %d \n",cur_kl->inputBuffers.size(),c_index,s_index);
    return make_pair(cur_kl->inputBuffers.size()+c_index,s_index);
}

unsigned int get_buffer_size(KernelInfo* suc_kl,int suc_pos)
{
    for (int i = 0; i < suc_kl->inputBuffers.size(); ++i)
    {
        unsigned int size = std::get<1>(suc_kl->inputBuffers.at(i));
        unsigned int pos = std::get<2>(suc_kl->inputBuffers.at(i));
        if(pos==suc_pos)
            return size;
    }
    printf("get_buffer_size: Size not found\n");
    return -1;
}

std::string get_buffer_type(KernelInfo* suc_kl,int suc_pos)
{
    for (int i = 0; i < suc_kl->inputBuffers.size(); ++i)
    {
        std::string type = std::get<0>(suc_kl->inputBuffers.at(i));
        unsigned int pos = std::get<2>(suc_kl->inputBuffers.at(i));
        if(pos==suc_pos)
            return type;
    }
    return NULL;
}


void trasfer_data_to_ipbuf(KernelLaunchInfo* cur_kl, KernelLaunchInfo* suc_kl,int cur_pos,int suc_pos)
{
    if (LOG_LEVEL >=1)
    {    
        fprintf(fp,"trasfer_data_to_ipbuf: BEGIN \n");
    }

    std::pair<int, int> indices=get_buffer_index(cur_kl->task->kernels[0],suc_kl->task->kernels[0],cur_pos,suc_pos);
    int size=get_buffer_size(suc_kl->task->kernels[0],suc_pos);
    std::string type=get_buffer_type(suc_kl->task->kernels[0],suc_pos);
    //printf("Size: %u Type: %s\n",size,type.c_str());
    int i;

    if( type.compare("int") == 0) 
    {
        int* A;
        int* B;
        A = (int*)cur_kl->task->data[indices.first];
        B = (int*)suc_kl->task->data[indices.second];
        for (i = 0; i < size; ++i)
        {
            B[i] = A[i];
        }
    }
    else if(type.compare("uint") == 0) 
    {
        uint* A;
        uint* B;
        A = (uint*)cur_kl->task->data[indices.first];
        B = (uint*)suc_kl->task->data[indices.second];
        for (i = 0; i < size; ++i)
        {
            B[i] = A[i];
        }
    }
    else if(type.compare("short") == 0) 
    {
        short* A;
        short* B;
        A = (short*)cur_kl->task->data[indices.first];
        B = (short*)suc_kl->task->data[indices.second];
        for (i = 0; i < size; ++i)
        {
            B[i] = A[i];
        }
    }
    else if(type.compare("ushort") == 0) 
    {
        ushort* A;
        ushort* B;
        A = (ushort*)cur_kl->task->data[indices.first];
        B = (ushort*)suc_kl->task->data[indices.second];
        for (i = 0; i < size; ++i)
        {
            B[i] = A[i];
        }
    }
    else if(type.compare("long") == 0) 
    {
        long* A;
        long* B;
        A = (long*)cur_kl->task->data[indices.first];
        B = (long*)suc_kl->task->data[indices.second];
        for (i = 0; i < size; ++i)
        {
            B[i] = A[i];
        }
    }
    else if(type.compare("ulong") == 0) 
    {
        ulong* A;
        ulong* B;
        A = (ulong*)cur_kl->task->data[indices.first];
        B = (ulong*)suc_kl->task->data[indices.second];
        for (i = 0; i < size; ++i)
        {
            B[i] = A[i];
        }
    }
    else if(type.compare("float") == 0) 
    {
        float* A;
        float* B;
        printf("FLOAT indices: %d %d size: %d %d\n",indices.first,indices.second,cur_kl->task->data.size(),suc_kl->task->data.size());
        A = (float*)cur_kl->task->data[indices.first];
        B = (float*)suc_kl->task->data[indices.second];        
        for (i = 0; i < size; ++i)
        {
            B[i] = A[i];            
        }
    }
    else if(type.compare("double") == 0) 
    {
        double* A;
        double* B;
        A = (double*)cur_kl->task->data[indices.first];
        B = (double*)suc_kl->task->data[indices.second];
        for (i = 0; i < size; ++i)
        {
            B[i] = A[i];
        }
    }
    else if(type.compare("char") == 0) 
    {
        char* A;
        char* B;
        A = (char*)cur_kl->task->data[indices.first];
        B = (char*)suc_kl->task->data[indices.second];
        for (i = 0; i < size; ++i)
        {
            B[i] = A[i];
        }
    }
    else {
        fprintf(fp, "\tCannot find the required variable host array datatype %s\n", type.c_str());
        exit(-37);
    }

    if (LOG_LEVEL >=1)
    {    
        fprintf(fp,"trasfer_data_to_ipbuf: END\n");
    }

}

int local_controller(KernelLaunchInfo* kl){
    float m_speed=((float)kl->task->task_size/kl->kex.turnaroundTime);
    float error=(kl->expected_speed-m_speed);
    float speed_up_cur= kl->last_speedup + (1-POLE)*(error/(float)kl->task->basespeed[0]);
    
    if(speed_up_cur<kl->expected_speed/(float)kl->task->basespeed[0]){
        kl->control_mode=0;
    }
    else
    {
        printf("local_controller: spdup_cur(%f)=last_spdup(%f)+ %f* error(%f) \n",speed_up_cur,kl->last_speedup,(1-POLE),error/kl->task->basespeed[0]);  
        int p=kl->platform_pos;
        int d=kl->device_pos;  
        int i=find_nearest_config(speed_up_cur,p,d);
        printf("devsup:%f freq:%u cfig:%d \n",deviceSpec[p][d]->device_config[i]->speedup,deviceSpec[p][d]->device_config[i]->frequency,i);
        kl->last_speedup=speed_up_cur;
        kl->frequency=(deviceSpec[p][d]->device_config[i]->frequency);
    }
}

int find_nearest_config(float s_up, int p,int d){
    float dif=10000000000.0;
    int pos;
    for(int i=0;i<deviceSpec[p][d]->device_config.size();i++){
        if(dif>abs(deviceSpec[p][d]->device_config[i]->speedup-s_up)){
            dif=abs(deviceSpec[p][d]->device_config[i]->speedup-s_up);
            //fprintf(fp,"\nconfigFind %d: %f - %f : %f",i,deviceSpec[device].device_config[i].speedup,s_up,dif);
            pos=i;
        }
    }
    //fprintf(fp,"\nPos:%d Freq:%u Spup:%f",pos,deviceSpec[device].device_config[pos].frequency,deviceSpec[device].device_config[pos].speedup);
    return pos;
}

void shift_task(KernelLaunchInfo* kl){
    
    if(kl->start<kl->task->deadline-kl->kex.turnaroundTime)
    {    kl->start=kl->task->deadline-kl->kex.turnaroundTime;
        printf("control mode 3: Arrival: %llu Start: %llu\n",kl->task->arrival,kl->start);
    }

}

void print_DAGTime(std::vector<std::vector<DAGTime>> &l_dagtime_matrix ){
    for(int job=0;job<NumOfJobs;job++)
    {   for(int instance=0;instance<DAGtimeMatrix[job].size();instance++)
        {   
            //printf("\njob %d inst %d\n",job,instance);
            DAGTime dagtime=l_dagtime_matrix[job][instance];
            // printf("%d %d : %d %d : %llu %llu %llu %.0f %.0f %llu %llu %llu : %d\n",job,instance,dagtime.klinfo->platform_pos ,dagtime.klinfo->device_pos , dagtime.startTime,dagtime.finishTime,dagtime.deadline,dagtime.turnaroundTime,dagtime.writeStart,dagtime.ndStart,dagtime.readStart,dagtime.safe_mode);
            // fprintf(m_result,"%d %d %d %d %llu %llu %llu %.0f %.0f %llu %llu %llu\n",job,instance,dagtime.klinfo->platform_pos ,dagtime.klinfo->device_pos , dagtime.arrivalTime,dagtime.startTime,dagtime.finishTime,dagtime.deadline,dagtime.turnaroundTime,dagtime.writeStart,dagtime.ndStart,dagtime.readStart);
            // fprintf(m_result,"%d,%d,%s,%s,%d,%llu,%llu,%d\n",dagtime->klinfo->task->dagInfo->globalDAGID,job,(dagtime.klinfo->platform_pos==0?"gpu":"cpu") ,dagtime.klinfo->device_pos , dagtime.startTime,dagtime.finishTime,dagtime->klinfo->task->isTerminal);
        }
    }
}


void childrenToReadyBuffer(KernelLaunchInfo& kl){

    if (LOG_SCHEDULER >=1 || LOG_LEVEL>=1){
        fprintf(fp,"childrenToReadyBuffer: BEGIN: %llu \n",get_current_time());
        fflush(fp);
    }
    
    int jobid=kl.task->dagInfo->jobID;
    int global_dagid=kl.task->dagInfo->globalDAGID;
    int instance=kl.task->dagInfo->instanceID;

    //printf("childrenToReadyBuffer: BEGIN: jobid: %d global_dagid: %d instance: %d\n",jobid,global_dagid,instance);
    //print_job_map();

    auto it = jobMap.find(jobid);    

    if(it != jobMap.end())
    {           
        std::vector<KernelLaunchInfo*> succssor_kernels;
        JobInfo *job = it->second; 
        
        for(int i=0;i<kl.task->nodes.size();i++)
        {
            
            for(int j=0;j<job->successors[kl.task->nodes[i]].size();j++)
            {   
                int s_node=job->successors[kl.task->nodes[i]][j];     
                // printf("global_dagid: %d Instance:%d , Current Node: %d, Successor node: %d\n",global_dagid,instance,kl.task->nodes[i],s_node);           
                // printf("Done?: %d\n",nodes_matrix[global_dagid][s_node]);
                if(nodes_matrix[global_dagid][s_node]!=1)
                {   
                    printf("global_dagid: %d Instance:%d , Current Node: %d, Successor node: %d\n",global_dagid,instance,kl.task->nodes[i],s_node);    
                    printf("Unfinished Successor node: %d\n",s_node);                
                    if(check_dependency(job->dependency[s_node],global_dagid)==1)   
                    {                       
                    //kl_suc=taskMap.at(make_pair(global_dagid,s_node));
                    
                        auto iter = taskMap.find(make_pair(global_dagid,s_node));               
                        
                        if(iter != taskMap.end())//successor node found meeting all dependency
                        {
                            std::vector<KernelLaunchInfo*> kl_suc_vector = iter->second; 
                            //print_launch_info(*kl_suc); 

                            if(CREATE_SUB_KERNEL==0 && kl_suc_vector.size()==1)
                            {
                                KernelLaunchInfo* kl_suc=kl_suc_vector[0];
                                if (std::find(succssor_kernels.begin(), succssor_kernels.end(), kl_suc) == succssor_kernels.end()) 
                                { 
                                    //printf("New successor node is: %d-%d \n",global_dagid,s_node);
                                    if(get_current_time() >= DAGtimeMatrix[kl.task->dagInfo->jobID][instance]->arrivalTime  )
                                    {
                                        int p=kl_suc->platform_pos;
                                        int d=kl_suc->device_pos;
                                        //printf("childrenToReadyBuffer: parent: %d-%d(%u) child: %d-%d(%u)   \n",global_dagid,kl.task->nodes[i],&kl,global_dagid,s_node,kl_suc);
                                        
                                        // while (test_and_set(&rblock,0, 1))
                                        //             ;
                                        mtx_rblock.lock();
                                        ready_buffer[p][d].push_back(kl_suc);  
                                        succssor_kernels.push_back(kl_suc) ;
                                        mtx_rblock.unlock();  
                                        cv_rbuffer_available.notify_one();

                                        fprintf(fp,"\tchildrenToReadyBuffer:Moving from waiting to ready queue_%d-%d: %d \n",p,d,ready_buffer[p][d].size());

                                        printf("\t########childrenToReadyBuffer:Moved from waiting to ready queue_%d-%d: %d ########\n",p,d,ready_buffer[p][d].size());
                                    }
                                }
                                else
                                {
                                    printf("successor node already added: %d-%d \n",global_dagid,s_node);
                                }

                            }
                            else if(CREATE_SUB_KERNEL==1 )   
                            {
                                for(int i=0;i<kl_suc_vector.size();i++)
                                {
                                    KernelLaunchInfo* kl_suc_p=kl_suc_vector[i];
                                    if (std::find(succssor_kernels.begin(), succssor_kernels.end(), kl_suc_p) == succssor_kernels.end()) 
                                    { 
                                        //printf("New successor node is: %d-%d \n",global_dagid,s_node);
                                        if(get_current_time() >= DAGtimeMatrix[kl.task->dagInfo->jobID][instance]->arrivalTime  )
                                        {
                                            int p=kl_suc_p->platform_pos;
                                            int d=kl_suc_p->device_pos;
                                            //printf("childrenToReadyBuffer: parent: %d-%d(%u) child: %d-%d(%u)   \n",global_dagid,kl.task->nodes[i],&kl,global_dagid,s_node,kl_suc);
                                            
                                            // while (test_and_set(&rblock,0, 1))
                                            //             ;
                                            mtx_rblock.lock();
                                            ready_buffer[p][d].push_back(kl_suc_p);  
                                            succssor_kernels.push_back(kl_suc_p) ;
                                            mtx_rblock.unlock();  
                                            cv_rbuffer_available.notify_one();

                                            fprintf(fp,"\t---childrenToReadyBuffer:Moving from waiting to ready queue_%d-%d: %d \n",p,d,ready_buffer[p][d].size());

                                            printf("\t---########childrenToReadyBuffer:Moved from waiting to ready queue_%d-%d: %d ########\n",p,d,ready_buffer[p][d].size());
                                        }
                                    }
                                    else
                                    {
                                        printf("---successor node already added: %d-%d \n",global_dagid,s_node);
                                    }

                                }
                            }             

                            
                        }
                        else 
                            printf("Not found in taskMap inner %d-%d\n",global_dagid,s_node);   
                    }  
                }                    
            }
        }
    }
    else 
        printf("Not found in jobMap outer %d\n",jobid);
                
    //JobInfo *job=jobMap.at(jobid);
    

    if (LOG_SCHEDULER >=1 || LOG_LEVEL>=1){
        fprintf(fp,"childrenToReadyBuffer: END: %llu \n",get_current_time());
        fflush(fp);
    }
}


void profile_events(KernelLaunchInfo& kl_info) {

    cl_int status;
    cl_ulong diff;

    // printEventStatus(kl_info.ke.write.back());    
    status = clGetEventProfilingInfo(kl_info.ke.write.front(), CL_PROFILING_COMMAND_QUEUED, sizeof(cl_ulong), &(kl_info.kex.writeQueued), NULL);
    check(status, "Getting event write command queued profiling info");
    status = clGetEventProfilingInfo(kl_info.ke.write.front(), CL_PROFILING_COMMAND_SUBMIT, sizeof(cl_ulong), &(kl_info.kex.writeSubmit), NULL);
    check(status, "Getting event write command submit profiling info");
    status = clGetEventProfilingInfo(kl_info.ke.write.front(), CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &(kl_info.kex.writeStart), NULL);
    check(status, "Getting event write command start profiling info");
    status = clGetEventProfilingInfo(kl_info.ke.write.back(), CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &(kl_info.kex.writeEnd), NULL);
    check(status, "Getting event write command end profiling info");
    // print_profile_event_status(status);
    

    // printEventStatus(kl_info.ke.exec); 
    status = clGetEventProfilingInfo(kl_info.ke.exec, CL_PROFILING_COMMAND_QUEUED, sizeof(cl_ulong), &(kl_info.kex.ndQueued), NULL);
    check(status, "Getting event exec command queued profiling info");
    status = clGetEventProfilingInfo(kl_info.ke.exec, CL_PROFILING_COMMAND_SUBMIT, sizeof(cl_ulong), &(kl_info.kex.ndSubmit), NULL);
    check(status, "Getting event exec command submit profiling info");
    status = clGetEventProfilingInfo(kl_info.ke.exec, CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &(kl_info.kex.ndStart), NULL);
    check(status, "Getting event exec command start profiling info");
    status = clGetEventProfilingInfo(kl_info.ke.exec, CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &(kl_info.kex.ndEnd), NULL);
    check(status, "Getting event exec command end profiling info");
    // print_profile_event_status(status);
    

    // printEventStatus(kl_info.ke.read.back()); 
    status = clGetEventProfilingInfo(kl_info.ke.read.front(), CL_PROFILING_COMMAND_QUEUED, sizeof(cl_ulong), &(kl_info.kex.readQueued), NULL);
    check(status, "Getting event read command queued profiling info");
    status = clGetEventProfilingInfo(kl_info.ke.read.front(), CL_PROFILING_COMMAND_SUBMIT, sizeof(cl_ulong), &(kl_info.kex.readSubmit), NULL);
    check(status, "Getting event read command submit profiling info");
    status = clGetEventProfilingInfo(kl_info.ke.read.front(), CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &(kl_info.kex.readStart), NULL);
    check(status, "Getting event read command start profiling info");
    status = clGetEventProfilingInfo(kl_info.ke.read.back(), CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &(kl_info.kex.readEnd), NULL);
    check(status, "Getting event read command end profiling info");
    // print_profile_event_status(status);
    
    
    kl_info.kex.devStartTime = kl_info.kex.writeStart;
    kl_info.kex.devEndTime = kl_info.kex.readEnd;
    //time from nano second to minisecond
    kl_info.kex.devTotalTime = (kl_info.kex.devEndTime - kl_info.kex.devStartTime); 
    kl_info.kex.writeTime = (kl_info.kex.writeEnd - kl_info.kex.writeStart);
    kl_info.kex.ndTime = (kl_info.kex.ndEnd - kl_info.kex.ndStart);
    kl_info.kex.readTime = (kl_info.kex.readEnd - kl_info.kex.readStart);
    //printf("\nProfiling of kernel Platform %d-%d frequency %u : Total:%llu, Write:%llu, Nd:%llu, Read:%llu", kl_info.platform_pos,kl_info.device_pos,kl_info.frequency, kl_info.kex.totalTime,kl_info.kex.writeTime,kl_info.kex.ndTime,kl_info.kex.readTime);
    //printf("=> %ld %ld\n", k->writeStart[0], k->writeStart[1]);
}


size_t get_sizeof(std::string str) {
    
    if( str.compare("int") == 0)
        return sizeof(int);
    else if(str.compare("uint") == 0)
        return sizeof(uint);
    else if(str.compare("short") == 0)
        return sizeof(short);
    else if(str.compare("ushort") == 0)
        return sizeof(ushort);
    else if(str.compare("long") == 0)
        return sizeof(long);
    else if(str.compare("ulong") == 0)
        return sizeof(ulong);
    else if(str.compare("bool") == 0)
        return sizeof(bool);
    else if(str.compare("float") == 0)
        return sizeof(float);
    else if(str.compare("double") == 0)
        return sizeof(double);
    else if(str.compare("char") == 0)
        return sizeof(char);
    else
        return sizeof(void*);
       
}


void* array_allocate(std::string type, int size) {

    if( type.compare("int") == 0) {
        return (int*)malloc(sizeof(int) * size);
    }
    else if(type.compare("uint") == 0) {
        return (uint*)malloc(sizeof(uint) * size);
    }
    else if(type.compare("short") == 0) {
        return (short*)malloc(sizeof(short) * size);
    }
    else if(type.compare("ushort") == 0) {
        return (ushort*)malloc(sizeof(ushort) * size);
    }
    else if(type.compare("long") == 0) {
        return (long*)malloc(sizeof(long) * size);
    }
    else if(type.compare("ulong") == 0) {
        return (ulong*)malloc(sizeof(ulong) * size);
    }
    else if(type.compare("float") == 0) {
        //printf("ALLOCATE %u\n",sizeof(float)*size);
        float* temp = (float*)malloc(sizeof(float) * size);
        // if(temp!=NULL)
        //     printf("temp: %u",temp);
        // else
        //     printf("NULL");
        return temp;
    }
    else if(type.compare("double") == 0) {
        return (double*)malloc(sizeof(double) * size);
    }
    else if(type.compare("char") == 0) {
        return (char*)malloc(sizeof(char) * size);
    }
    else {
        fprintf(fp," Cannot find the required variable host array datatype %s", type.c_str());
        exit(-37);
    }
}

void print_temperature(Temperature* tmp){
    printf("\nTemperature: %d %d %d %d %d",tmp->sensors[0],tmp->sensors[1],tmp->sensors[2],tmp->sensors[3],tmp->sensors[4]); 
}

void monitor_temperature()
{
    char filename[1024];
    Temperature * t=new Temperature();
    int max=0;
    int avg=0;
    int maxC=0;
    int avgC=0;
    for(int i=0;i<5;i++)
    {
        strcpy(filename,"/sys/class/thermal/thermal_zone"); 
        std::string s = std::to_string(i);
        char const *ichar = s.c_str();
        strcat(filename,ichar);
        strcat(filename,"/temp");
        FILE *fp; 
        fp = fopen(filename, "r"); // read mode
        if (fp == NULL)
        exit(EXIT_FAILURE); 
        char* tmp;
        char line[100];
        while (fgets(line, 10, fp))    
        {   
            // tmp = strdup(line); 
            tmp = (char*)calloc(strlen(line) + 1,1 );
            strcpy(tmp, line);
            // printf("monitor_temperature: tmp: %s\n",tmp);
        }
        fclose(fp);
        // std::vector<int> sensors;
        // t->sensors=sensors;
        // t->sensors.push_back(atoi(tmp));
        // printf("tmp %d\n",atoi(tmp));


        if(i==0){
            t->cpuB_0=atoi(tmp);
            t->sensors.push_back(t->cpuB_0);
            max=(t->cpuB_0>max?t->cpuB_0:max);
            avg+=t->cpuB_0;
        }
        else if(i==1){
            t->cpuB_1=atoi(tmp);
            t->sensors.push_back(t->cpuB_1);
            max=(t->cpuB_1>max?t->cpuB_1:max);
            avg+=t->cpuB_1;
        }
        else if(i==2){
            t->cpuB_2=atoi(tmp);
            t->sensors.push_back(t->cpuB_2);
            max=(t->cpuB_2>max?t->cpuB_2:max);
            avg+=t->cpuB_2;
        }
        else if(i==3){
            t->cpuB_3=atoi(tmp);
            t->sensors.push_back(t->cpuB_3);
            max=(t->cpuB_3>max?t->cpuB_3:max);
            avg+=t->cpuB_3;
        }
        else if(i==4){
            t->gpu_4=atoi(tmp); 
            t->sensors.push_back(t->gpu_4);  
            maxC=max;
            avgC=avg;
            max=(t->gpu_4>max?t->gpu_4:max);  
            avg+=t->gpu_4;        
        }
    }
    t->frequency_C_b=get_current_CPU_frequency(4);
    t->frequency_C_l=get_current_CPU_frequency(0);
    t->frequency_G=get_current_GPU_frequency();
    t->max=max;
    t->avg=avg/5;
    t->maxC=maxC;
    t->avgC=avgC/4;

    tempMap.insert( pair<unsigned long long int, Temperature*>(get_current_time(), t) );  
    //print_temperature(t)   ;  

    /*
    if(max>88000)
        change_frequency(1800000000,  1,  0);   
    else if (max<80000)
        change_frequency(2000000000,  1,  0);
    */
}

void *temperature_monitor(void *vargp){
    cpu_set_t cpuset;
    pthread_t thread_id_moniter = pthread_self(); 
    CPU_ZERO(&cpuset);
    CPU_SET(2, &cpuset);    
    int s = pthread_setaffinity_np(thread_id_moniter, sizeof(cpu_set_t), &cpuset);
    printf("\ntemperature_monitor %u thread on core %d CurTime %llu\n",pthread_self(),sched_getcpu(),get_current_time());

    while(SchedulerFinish!=1)
    {
        monitor_temperature();
        std::this_thread::sleep_for(std::chrono::microseconds(TempInterval));
    }
}

void print_tempMap(){
    
    for(auto elem : tempMap)
    {
        fprintf(tmp_result,"%llu %d %d %d %d %d %u %u %u %d %d\n",elem.first, elem.second->sensors[0],elem.second->sensors[1],elem.second->sensors[2],elem.second->sensors[3],elem.second->sensors[4],elem.second->frequency_C_b,elem.second->frequency_C_l,elem.second->frequency_G,elem.second->max,elem.second->avg );
    }

}

bool cmp(Interval* lhs, Interval* rhs)
{
  return lhs->intervalWidth < rhs->intervalWidth;
}

bool cmp_start(Interval* lhs, Interval* rhs)
{
    return lhs->start < rhs->start;
}
bool cmp_startTime(DAGTime &lhs, DAGTime &rhs)
{
    return lhs.startTime < rhs.startTime;
}

void get_intervals_per_band(std::map<unsigned long long int,Temperature> &temperatureMap,std::vector<std::vector<std::vector<Interval*>>> &intervals_per_tband)
{
  for(int i=0;i<NumOfTempBand;i++)
    { for(int j=0;j<NumOfSensors;j++)
      { std::vector<Interval*> cintvl;
        intervals_per_tband[i].push_back(cintvl);
      }
    }
    std::vector<int> curj(NumOfSensors,0);
    std::vector<int> prvj(NumOfSensors,-1);
    std::vector<int> prvprvj(NumOfSensors,-2);
    // printf("intervals_per_tband \n");
    int count=0;
    for(auto elem : temperatureMap)
    {
        fprintf(tmp_result,"%llu %d %d %d %d %d\n",elem.first, elem.second.sensors[0],elem.second.sensors[1],elem.second.sensors[2],elem.second.sensors[3],elem.second.sensors[4] );
        // printf("%llu %d %d %d %d %d\n",elem.first, elem.second.sensors[0],elem.second.sensors[1],elem.second.sensors[2],elem.second.sensors[3],elem.second.sensors[4] );
        
        for(int i=0;i<NumOfSensors;i++)
        {
          if(elem.second.sensors[i]<=50000)
            curj[i]=0;          
          else if(elem.second.sensors[i]>50000 && elem.second.sensors[i]<=60000)
            curj[i]=1;          
          else if(elem.second.sensors[i]>60000 && elem.second.sensors[i]<=70000)
            curj[i]=2;          
          else if(elem.second.sensors[i]>70000 && elem.second.sensors[i]<=80000)
            curj[i]=3;          
          else if(elem.second.sensors[i]>80000 && elem.second.sensors[i]<=85000)
            curj[i]=4;          
          else if(elem.second.sensors[i]>85000 && elem.second.sensors[i]<=90000)
            curj[i]=5;          
          else if(elem.second.sensors[i]>90000)
            curj[i]=6;
         
          // printf("Count %d sensor[%d]: band cur %d prv %d prvprv %d\n",count++,i,curj[i],prvj[i],prvprvj[i]);
          if(prvprvj[i]==curj[i] && prvj[i]==curj[i])
          {        
              // printf("case 1\n");      
              Interval* inv=intervals_per_tband[prvprvj[i]][i][intervals_per_tband[prvprvj[i]][i].size()-1];
              inv->end=elem.first;
              inv->intervalWidth=inv->end-inv->start;
               
          }
          else if(prvprvj[i]==curj[i] && prvj[i]!=curj[i])
          {
              // printf("case 2 start c %d p %d pp %d\n",curj[i],prvj[i],prvprvj[i]); 
              intervals_per_tband[prvj[i]][i].erase(intervals_per_tband[prvj[i]][i].end() - 1);
              Interval* inv=intervals_per_tband[prvprvj[i]][i].back();
              inv->end=elem.first;
              inv->intervalWidth=inv->end-inv->start;   
              // printf("case 2 end\n");           
          }          
          else if(prvprvj[i]!=curj[i] && prvj[i]==curj[i])
          {
              // printf("case 3\n"); 
              Interval* inv=intervals_per_tband[prvj[i]][i][intervals_per_tband[prvj[i]][i].size()-1];
              inv->end=elem.first;
              inv->intervalWidth=inv->end-inv->start;              
          }
          else if(prvprvj[i]!=curj[i] && prvj[i]!=curj[i])
          {   
              // printf("case 4 push to intervals_per_tband[%d][%d]\n",curj[i],i);           
              Interval* intrvl= new Interval();
              intrvl->start=elem.first;
              intrvl->end=elem.first;
              intrvl->intervalWidth=intrvl->end-intrvl->start;
              intervals_per_tband[curj[i]][i].push_back(intrvl);
              
          }      
          if(prvprvj[i]==curj[i] && prvj[i]!=curj[i])    
            prvprvj[i]=curj[i];
          else
            prvprvj[i]=prvj[i];
          prvj[i]=curj[i]; 

        } 

    }
    // printf("Print temp : \n");
    fprintf(tmp_result,"\n");
    
}
std::pair<int, int> get_max_band(std::vector <std::vector<std::vector<Interval*>>> &intervals_per_tband)
{
  int max_band=-1;
  std::pair<int, int> max_pair;
  for(int i=0;i<NumOfTempBand;i++)
  {
    for (int j=0;j<NumOfSensors;j++)
    {
      if(intervals_per_tband[i][j].size()>0 && i>max_band)
      { 
          max_band=i;
          max_pair=std::make_pair(max_band, j); 
        // for(auto elem:intervals_per_tband[i][j])
        //   printf("intervals[%d][%d] : %llu - %llu : %llu\n",i,j,elem->start,elem->end,elem->intervalWidth);
        // printf("\n");
      }
    }
  }
  return max_pair;
}

void get_global_interval(std::vector <std::vector<std::vector<Interval*>>> &intervals_per_tband,std::pair<int, int> max_pair,std::vector<Interval*> &global_intvl)
{
  int chosen_band=max_pair.first;
  for (int j=0;j<NumOfSensors;j++)
    {   
      if(intervals_per_tband[chosen_band][j].size()>0)
      {    
        auto it =std::max_element(intervals_per_tband[chosen_band][j].begin(),intervals_per_tband[chosen_band][j].end(),cmp);
        Interval* ivl =*it;
        intervals_per_tband[chosen_band][j].erase(it);
        unsigned long long int start=ivl->start;
        unsigned long long int end=ivl->end;
        // printf("start %llu end %llu\n",start,end);
        for (int i=0;i<NumOfSensors;i++)
        {
          if(i!=j)
          {   int pos=0;
              for(int pos=0;pos<intervals_per_tband[chosen_band][i].size();pos++)
              {
                  Interval* elem=intervals_per_tband[chosen_band][i][pos];
                  if(elem->start<start && elem->end>start)
                  {
                    // printf("case1 pos %d size %d\n",pos,intervals_per_tband[chosen_band][i].size());
                    start=elem->start;
                    intervals_per_tband[chosen_band][i].erase(intervals_per_tband[chosen_band][i].begin()+pos);
                    pos--;
                  }
                  else if(elem->start>start && elem->end<end)
                  {
                    // printf("case2 pos %d size %d\n",pos,intervals_per_tband[chosen_band][i].size());
                    intervals_per_tband[chosen_band][i].erase(intervals_per_tband[chosen_band][i].begin()+pos);
                    pos--;
                  }
                  else if(elem->start>start && elem->end>end)
                  {
                    // printf("case3 pos %d size %d\n",pos,intervals_per_tband[chosen_band][i].size());                    
                    end=elem->end;
                    intervals_per_tband[chosen_band][i].erase(intervals_per_tband[chosen_band][i].begin()+pos);
                    pos--;
                  }
                  // else 
                  //   printf("case4 pos %d \n",pos);
                 
              }
          }          
        }
        ivl->start=start;
        ivl->end=end;
        ivl->intervalWidth=ivl->end-ivl->start;
        global_intvl.push_back(ivl);
      }
    }
    std::sort(global_intvl.begin(), global_intvl.end(), cmp_start);


    for(int i=0;i<global_intvl.size()-1;i++)
    {   if(global_intvl[i+1]->start<=global_intvl[i]->end)
        {
          global_intvl[i]->end=global_intvl[i+1]->end;
          global_intvl.erase(global_intvl.begin()+i+1);
          i--;
        }
    }

}
void choose_task_list(std::vector<std::vector<std::vector<DAGTime>>>  &d_timeMatrix,Interval* global_intvl,std::pair<int, int> max_pair,std::vector<std::vector<std::vector<KernelLaunchInfo*>>> &taskList)
{       
    for(int i=0;i<NumOfPlatforms;i++)
    {   std::vector<std::vector<KernelLaunchInfo*>> p;
        taskList.push_back(p);  
        for(int j=0;j<all_devices[i].size();j++)
        {   std::vector<KernelLaunchInfo*> d;
            taskList[i].push_back(d);
        }      
    }
       
    unsigned long long int start=global_intvl->start;
    unsigned long long int end=global_intvl->end;
    for(int platform=0;platform<NumOfPlatforms;platform++)
    {   for(int dev=0;dev<all_devices[platform].size();dev++)
        {   
            for(auto inst : d_timeMatrix[platform][dev])
            {
                if(inst.startTime>=start && inst.finishTime<=end )
                {
                    taskList[platform][dev].push_back(inst.klinfo);
                }
                else if(inst.finishTime>start && inst.finishTime<=end )
                {
                    taskList[platform][dev].push_back(inst.klinfo);
                }
                else if(inst.startTime>start && inst.startTime<end)
                {
                    taskList[platform][dev].push_back(inst.klinfo);
                }
            }
        }
    }

}

void find_max_idle_slot(Interval* intvl_gap,std::vector<std::vector<KernelLaunchInfo*>> &taskList,std::vector<Interval*> &idle_slots)
{
    unsigned long long int start=intvl_gap->start;
    unsigned long long int end=intvl_gap->end;
    printf("taskList size %d\n",taskList.size());
    for(auto dev: taskList)
    {   
        std::vector<Interval*> inv;
        printf("No of KernelLaunchInfo: %d\n",dev.size());
        if(dev.size()==0)
        {   
            Interval* i=new Interval();
            i->start=start;
            i->end=end;
            i->intervalWidth=i->end-i->start;
            idle_slots.push_back(i);
        }
        if(dev.size()==1)
        {
            Interval* i=new Interval();
            KernelLaunchInfo* cur_kl=dev[0];
            if(cur_kl->kex.rel_start_time<start && cur_kl->kex.rel_end_time>=start)
            {
                i->start=cur_kl->kex.rel_end_time;
                i->end=end;                
            }
            else if(cur_kl->kex.rel_start_time>start && cur_kl->kex.rel_end_time<end )
            {
                unsigned long long int first=cur_kl->kex.rel_start_time - start;
                unsigned long long int second=end - cur_kl->kex.rel_end_time;
                if(first>second)
                {
                    i->start=start;
                    i->end=cur_kl->kex.rel_start_time;
                }
                else
                {
                    i->start=cur_kl->kex.rel_end_time;
                    i->end=end;
                }                
            }
            else if(cur_kl->kex.rel_start_time<end && cur_kl->kex.rel_end_time>end )
            {
                i->start=start;
                i->end=cur_kl->kex.rel_start_time;    
            }
            i->intervalWidth=i->end-i->start;
            idle_slots.push_back(i);

        }
        else if(dev.size()>1)
        {   
            
            for(int k=0;k<dev.size()-1;k++)
            {   
                KernelLaunchInfo* cur_kl=dev[k];
                KernelLaunchInfo* nxt_kl=dev[k+1];
                Interval* i=new Interval();
                // printf("time: start: %llu end: %llu\n",start,end);
                // printf("curkl %llu - %llu \n",cur_kl->kex.rel_start_time,cur_kl->kex.rel_end_time);
                // printf("nexkl %llu - %llu\n",nxt_kl->kex.rel_start_time,nxt_kl->kex.rel_end_time);
                if(cur_kl->kex.rel_start_time<start && cur_kl->kex.rel_end_time>=start && nxt_kl->kex.rel_start_time<end)
                {
                    i->start=cur_kl->kex.rel_end_time;
                    i->end=nxt_kl->kex.rel_start_time;
                    
                }
                else if(cur_kl->kex.rel_end_time<start && nxt_kl->kex.rel_start_time>start && nxt_kl->kex.rel_start_time<end)
                {
                    i->start=start;
                    i->end=nxt_kl->kex.rel_start_time;
                }
                else if(cur_kl->kex.rel_end_time>start && nxt_kl->kex.rel_start_time>start && nxt_kl->kex.rel_start_time<end)
                {
                    i->start=cur_kl->kex.rel_end_time;
                    i->end=nxt_kl->kex.rel_start_time;
                }
                else if(cur_kl->kex.rel_end_time>start && nxt_kl->kex.rel_start_time>end )
                {
                    i->start=cur_kl->kex.rel_end_time;
                    i->end=end;
                }
                
                i->intervalWidth=i->end-i->start;
                printf("interval:%llu\n",i->intervalWidth);
                // if(i->intervalWidth!=0)
                inv.push_back(i);
            }
            std::sort(inv.begin(), inv.end(), cmp);
            idle_slots.push_back(inv[inv.size()-1]);
        }
    }

}


void convert_time_matrix(std::vector<std::vector<DAGTime>> &l_timeMatrix,std::vector<std::vector<std::vector<DAGTime>>> &d_timeMatrix)
{
    for(int i=0;i<NumOfPlatforms;i++)
    {   std::vector<std::vector<DAGTime>> p;
        d_timeMatrix.push_back(p);  
        for(int j=0;j<all_devices[i].size();j++)
        {   std::vector<DAGTime> d;
            d_timeMatrix[i].push_back(d);
        }      
    }
   
    for(auto job : l_timeMatrix)
    {
        for(auto inst : job)
        {
            int p=inst.klinfo->platform_pos;
            int d=inst.klinfo->device_pos;
            d_timeMatrix[p][d].push_back(inst);
        }
    }
    for(int p=0;p<NumOfPlatforms;p++)
    {
        for(int d=0;d<all_devices[p].size();d++)
        {
            std::sort(d_timeMatrix[p][d].begin(), d_timeMatrix[p][d].end(), cmp_startTime);
        }
    }
}

void print_intervals_per(std::vector <std::vector<std::vector<Interval*>>> & intervalList)
{
    printf("Intervels per temperature band. \n");
    for(int i=0;i<intervalList.size();i++)
    { 
        printf("Temperature Band: %d\n",i);
        for(int j=0;j<intervalList[i].size();j++)
        {   printf("\tSensor: %d\n",j);
            {
                for( auto elem: intervalList[i][j])
                {
                    printf("\t%llu - %llu : %llu\n",elem->start,elem->end,elem->intervalWidth);
                }
            }            
        }
    }
}

void print_task_list(std::vector<std::vector<std::vector<std::vector<KernelLaunchInfo*>>>> &taskList)
{
    for(int i=0;i<taskList.size();i++)
    {   printf("Global_interval instance: %d\n",i);
        for(int j=0;j<taskList[i].size();j++)
        {
            for(int k=0;k<taskList[i][j].size();k++)
            {
                if(taskList[i][j][k].size()==0)
                    printf("  No task. \n");
                else
                {
                    printf("  Platform %d device %d\n",j,k);
                    for( auto elem: taskList[i][j][k])
                    {
                        printf("\tKernel info index: %d\n",elem->task->traceID);
                    }
                }
            }
        }
    }
}

void generate_plot_data(int traceCount)
{
    float max_lateness=-10000000.0;
    float avg_lateness=0.0;
    int safe_mode_count=0;
    int deadlineViolation=0;

    for(int i=0;i<DAGtimeMatrix.size();i++)
    {
        for(int j=0;j<DAGtimeMatrix[i].size();j++)
        {
            DAGTime * d_time=DAGtimeMatrix[i][j];

            avg_lateness+=d_time->lateness;

            if(d_time->lateness>max_lateness)
                max_lateness=d_time->lateness;

            safe_mode_count=safe_mode_count+d_time->safe_mode;

            deadlineViolation=deadlineViolation+d_time->deadlineViolated;

            fprintf(m_result,"%d,%d,%s,%d,%llu,%llu,%d\n",d_time->klinfo->task->dagInfo->globalDAGID,i,(d_time->klinfo->platform_pos==0?"gpu":"cpu") ,d_time->klinfo->device_pos , d_time->startTime,d_time->finishTime,d_time->klinfo->task->isTerminal);
            fflush(m_result);
        }
    }
    float d_v_percentage;
    if (traceCount!=0)
    {   avg_lateness=avg_lateness/(float)traceCount;
        d_v_percentage=(deadlineViolation*100/(float)traceCount);
    }

    // printf("deadlineViolation: %d, traceCount: %d, d_v_percentage:%f",deadlineViolation,traceCount,d_v_percentage);
    fprintf(r_result,"%f,%f,%f,%d,%d,%llu\n",d_v_percentage,max_lateness,avg_lateness,deadlineViolation,safe_mode_count,safe_duration);
    fflush(r_result);
    
}



void *mode_controller(void *vargp){
    
    int mode=*((int *) vargp);
    printf("Mode: %d\n",mode);
    std::vector<std::vector<DAGTime>> l_timeMatrix(NumOfJobs);
    for(int job=0;job<NumOfJobs;job++)
    {   
        for(int instance=0;instance<DAGtimeMatrix[job].size();instance++)
        {    
            DAGTime inst=*(DAGtimeMatrix[job][instance]);
            l_timeMatrix[job].push_back(inst); 
        }
    }   
    print_DAGTime(l_timeMatrix);
    fprintf(m_result,"\n"); 
    // printf("Print time : %llu\n",get_current_time());       
    std::map<unsigned long long int,Temperature> temperatureMap;
    map<unsigned long long int,Temperature*>::iterator it;
    // int count=0;
    // printf("tempMap size %d \n",tempMap.size());
    for(it = tempMap.begin(); it != tempMap.end(); it++ )
    {
      temperatureMap.insert( pair<unsigned long long int, Temperature>(it->first, *it->second) );
    }    
    tempMap.clear();   

    std::vector<std::vector<std::vector<DAGTime>>> d_timeMatrix;

    convert_time_matrix(l_timeMatrix,d_timeMatrix);


    //instance,platform,device,launchinfo list
    std::vector<std::vector<std::vector<std::vector<KernelLaunchInfo*>>>> taskList;
    std::vector<std::vector<std::vector<Interval*>>> idleSlotList;
    std::vector<Interval*> global_intvl;
    std::pair<int, int> max_pair;
    std::vector <std::vector<std::vector<Interval*>>> intervals_per_tband(NumOfTempBand);
    get_intervals_per_band(temperatureMap,intervals_per_tband);
    print_intervals_per(intervals_per_tband);
    max_pair=get_max_band(intervals_per_tband);
    int flag=1;
    while(flag==1 || flag==2)
    {//get_intervals_per_band
        std::vector <std::vector<std::vector<Interval*>>> local_interval=intervals_per_tband;
                
        if(flag==2)
        {   max_pair.first=max_pair.first-1;
            global_intvl.clear();
            taskList.clear();
        }
        printf("max band %d\n",max_pair.first);    
        get_global_interval(local_interval,max_pair,global_intvl);
        
        //print global intervals
        for(auto elem : global_intvl)
        {    printf("global intervals: %llu - %llu : %llu\n",elem->start,elem->end,elem->intervalWidth);
            std::vector<std::vector<std::vector<KernelLaunchInfo*>>> tl;
            taskList.push_back(tl);
        }
                
        //choose_task_list
        
        int nint=0;
        int p=(max_pair.second<4?1:0);
        for(int i=0;i<global_intvl.size();i++)
        {  
            // std::vector<std::vector<std::vector<KernelLaunchInfo*>>> tl;
            choose_task_list(d_timeMatrix,global_intvl[i],max_pair,taskList[i]);
            
            for(auto elem : taskList[i][p])
            {   
                // printf("tasks: %d\n",elem.size()); 
                nint+=elem.size(); 

            }       
        }        

        print_task_list(taskList);

        if(nint==0)
            flag=2;   
        else  
        {   flag=0; 
            printf("Number of task instances:%d \n",nint);
        }  
    }

    print_task_list(taskList);
 
    if(max_pair.first>=2)
    {   int p=(max_pair.second<4?1:0);
        // printf("Platform %d global_intvl size %d\n",p,global_intvl.size());
        for(int g=0;g<global_intvl.size();g++)     
        {   for(int d=0;d<taskList[g][p].size();d++)
            {   
                // printf("tasklistsize[%d][%d][%d] %d \n",g,p,d,taskList[g][p][d].size());
                if(taskList[g][p][d].size()>0)
                {    for(int e=0; e<taskList[g][p][d].size();e++)
                    {   //print_launch_info(*elem);
                        KernelLaunchInfo* elem = taskList[g][p][d][e];
                        elem->control_mode=mode;
                        //********************************************************
                        if(mode==1)//frequency
                        {

                            unsigned long long int  finishtime;
                            if(taskList[g][p][d].size()==1)
                                finishtime=elem->task->deadline;
                            else if(e<taskList[g][p][d].size()-1)
                            {
                                    finishtime=std::min(taskList[g][p][d][e+1]->kex.rel_start_time,elem->task->deadline);
                            }
                            else if (e==taskList[g][p][d].size()-1)
                                finishtime=elem->task->deadline;
                            
                            unsigned long long int taskSlot=finishtime-elem->kex.rel_start_time;

                            float sp= ((float)elem->task->task_size/taskSlot);
                            //   printf("\nex_speedup (%.0f-%.0f)%.0f/%.0f : %f\n",(float)elem->task->deadline,(float)elem->kex.rel_start_time,(float)elem->task->task_size,(float)taskSlot,sp);
                            elem->expected_speed=sp;
                            float m_speed=calculate_speed(elem);
                            if(elem->last_speedup==0.0)
                                elem->last_speedup=m_speed/(float)elem->task->basespeed[0];
                        
                        }
                        else if (mode==2)//partition
                        {   printf("Mode2 Partitioning:\n");
                            int p_rev=(max_pair.second<4?0:1);
                            Interval* inv=new Interval();
                            inv->start = elem->kex.rel_start_time;
                            inv->end = elem->task->deadline;
                            inv->intervalWidth=inv->end -inv->start;
                            
                            std::vector<Interval*> idle_slots;
                            printf("Idle slot: %llu-%llu:%llu\n",inv->start,inv->end,inv->intervalWidth);
                            find_max_idle_slot(inv,taskList[g][p_rev],idle_slots);
                            printf("Idle_slot size: %d\n",idle_slots.size());
                            //std::vector<float> partition_value(idle_slots.size());
                            
                            if(idle_slots.size()==0)
                                printf("No gap in other device: %d",idle_slots.size());
                            else
                            {
                                float total_partition=0.0;
                                for(int dv=0;dv<idle_slots.size();dv++)
                                {    
                                    printf("Max slots: %llu-%llu: %llu \n",idle_slots[dv]->start,idle_slots[dv]->end,idle_slots[dv]->intervalWidth);
                                    float partition_value=.25;
                                    total_partition+=partition_value;
                                    if(partition_value>=0.0625)
                                    {
                                        KernelLaunchInfo* kl_new=new KernelLaunchInfo();
                                        TaskInfo* tinfo=new TaskInfo();
                                        tinfo->taskID=elem->task->taskID;
                                        tinfo->traceID=elem->task->traceID;        
                                        tinfo->nodes=elem->task->nodes;
                                        tinfo->dependency=elem->task->dependency;
                                        tinfo->kernels=elem->task->kernels;
                                        tinfo->dagInfo=elem->task->dagInfo;
                                        tinfo->task_size=partition_value*elem->size;
                                        tinfo->arrival=elem->task->arrival;
                                        tinfo->deadline=elem->task->deadline;    
                                        tinfo->data=elem->task->data;
                                        tinfo->isTerminal=elem->task->isTerminal;
                                        kl_new->task=tinfo;
                                        kl_new->kernel_index=elem->kernel_index;
                                        kl_new->platform_pos=p_rev;
                                        kl_new->device_pos=dv;
                                        kl_new->size=partition_value*elem->size;
                                        kl_new->queued=0;    
                                        kl_new->start=idle_slots[dv]->start;
                                        task_queue[p_rev][dv].push_back(kl_new);
                                    }
                                }
                                if(total_partition!=0.0)
                                {
                                    unsigned int sze=elem->task->task_size-(elem->task->task_size*total_partition);
                                    printf("sz:%u\n",sze);
                                    
                                    elem->task->task_size=sze;
                                    elem->size=sze;
                                }
                            }    
                            
                        }
                        else if(mode==3)//displacement
                        {
                            
                        }
                    }
                }
            }
        
        }      
    }
    
    printf("\ncontroller thread end : %llu\n",get_current_time() );
}

unsigned long long int  gcd(unsigned long long int  a, unsigned long long int  b) 
{ 
    if (b == 0) 
        return a; 
    return gcd(b, a % b); 
} 

unsigned long long int findlcm(unsigned long long int arr[], int n) 
{ 
    // Initialize result 
    unsigned long long int  ans = arr[0]; 
  
    // ans contains LCM of arr[0], ..arr[i] 
    // after i'th iteration, 
    printf("Period: %u\n",arr[0]);
    for (unsigned long long int i = 1; i < n; i++) 
    {   
        printf("Period: %u\n",arr[i]);
        ans = (((arr[i] * ans)) / 
                (gcd(arr[i], ans))); 
    }
    return ans; 
} 


float calculate_speed(KernelLaunchInfo* klinfo)
{
  float speed=((float)klinfo->task->task_size/klinfo->kex.turnaroundTime);
//   printf("mes_speed (%.0f-%.0f-%.0f)%.0f/%.0f : %f\n",(float)klinfo->kex.rel_end_time,(float)klinfo->kex.rel_start_time,(float)klinfo->task->arrival,(float)klinfo->task->task_size,(float)klinfo->kex.turnaroundTime,speed);
          
  return speed;
}

unsigned int get_current_GPU_frequency()
{

    char line[1024];
    unsigned int freq;
    FILE *fp; 
    fp = fopen("/sys/devices/platform/11800000.mali/devfreq/devfreq0/cur_freq", "r"); // read mode
    if (fp == NULL)
        exit(EXIT_FAILURE); 
    char* tmp;
    while (fgets(line, 1024, fp))    
    {   
        // tmp = strdup(line); 
        tmp = (char*)calloc(strlen(line) + 1,1 );
        strcpy(tmp, line);
        //printf("change_frequency cur: %s freq: %u\n",tmp,frequency);
    }  
    fclose(fp);     
    char* tok;
    tok = strtok(tmp, "\n");
    freq=(unsigned int)atoi(tok);
    return freq;

}

unsigned int get_current_CPU_frequency(int d)
{

    char line[1024];
    unsigned int freq;
    FILE *fp; 
    std::string str = "/sys/devices/system/cpu/cpu";
    str=str+std::to_string(d)+"/cpufreq/scaling_cur_freq";
    fp = fopen(str.c_str(), "r"); // read mode
    if (fp == NULL)
        exit(EXIT_FAILURE); 
    char* tmp;
    while (fgets(line, 1024, fp))    
    {   
        // tmp = strdup(line);   
        tmp = (char*)calloc(strlen(line) + 1,1);
        strcpy(tmp, line);
        //printf("change_frequency tmp: %s",tmp);
    }
    fclose(fp);     
    char* tok;
    tok = strtok(tmp, "\n");
    freq=(unsigned int)atoi(tok);
    return freq;

}


void change_frequency(unsigned int frequency, int platform_pos, int device_pos)
{
    
    char cmd[200];
    int ret;
    if(platform_pos  == 0) {
        //devn="GPU"; 
        char line[1024];
        unsigned int freq;
        FILE *fp; 
        fp = fopen("/sys/devices/platform/11800000.mali/devfreq/devfreq0/cur_freq", "r"); // read mode
        if (fp == NULL)
        exit(EXIT_FAILURE); 
        char* tmp;
        while (fgets(line, 1024, fp))    
        {   
            // tmp = strdup(line); 
            tmp = (char*)calloc(strlen(line) + 1,1 );
            strcpy(tmp, line);
            //printf("change_frequency cur: %s freq: %u\n",tmp,frequency);
        }  
        fclose(fp);     
        char* tok;
        tok = strtok(tmp, "\n");
        freq=(unsigned int)atoi(tok);
        if(frequency>=freq){
            sprintf(cmd, "echo %u > /sys/devices/platform/11800000.mali/devfreq/devfreq0/max_freq ", frequency);
            ret = system(cmd);

            sprintf(cmd, "echo %u > /sys/devices/platform/11800000.mali/devfreq/devfreq0/min_freq ", frequency);
            ret = system(cmd);
        }
        else{
            sprintf(cmd, "echo %u > /sys/devices/platform/11800000.mali/devfreq/devfreq0/min_freq ", frequency);
            ret = system(cmd);

            sprintf(cmd, "echo %u > /sys/devices/platform/11800000.mali/devfreq/devfreq0/max_freq ", frequency);
            ret = system(cmd);
        }

    }
    else if(platform_pos  == 1) {
        //devn="CPU";
        //sprintf(cmd, "echo odroid | sudo -S cpufreq-set -c %d --min %u --max %u", device_pos, frequency, frequency);
        //int c=NumofCPUs/2;
        // int d=(device_pos<c?0:4);
        int d=4;
        char line[1024];
        unsigned int freq;
        FILE *fp; 
        std::string str = "/sys/devices/system/cpu/cpu";
        str=str+std::to_string(d)+"/cpufreq/scaling_cur_freq";
        fp = fopen(str.c_str(), "r"); // read mode
        if (fp == NULL)
            exit(EXIT_FAILURE); 
        char* tmp;
        while (fgets(line, 1024, fp))    
        {   
            // tmp = strdup(line);   
            tmp = (char*)calloc(strlen(line) + 1,1);
            strcpy(tmp, line);
            //printf("change_frequency tmp: %s",tmp);
        }
        fclose(fp);     
        char* tok;
        tok = strtok(tmp, "\n");
        freq=(unsigned int)atoi(tok);
        //printf("frequency: %u\n",freq);
        sprintf(cmd, "cpufreq-set -c %d --max %uHz --min %uHz ",d, frequency, frequency); 
         
        // if(frequency>=freq) 
        // {    sprintf(cmd, "cpufreq-set -c %d --max %uHz --min %uHz ",d, frequency, frequency);        
        // }
        // else
        // {    sprintf(cmd, "cpufreq-set -c %d --min %uHz --max %uHz",d, frequency, frequency);        
        // }   
        ret = system(cmd);


    }
    
}

unsigned long long int get_current_time(){
    struct timeval c_time;
    gettimeofday(&c_time,NULL);
    unsigned long long int current_time=(unsigned long long int )(c_time.tv_sec*1000000+c_time.tv_usec);
    //return (unsigned long long int)(current_time - start_time)*timeFactor;
    return (unsigned long long int)(current_time - START_TIME)*FACTOR;
}

int get_dev_index(int platform,int device){
    if(platform==0)
        return (platform+device);
    // else if(platform==1 && device <3 )
    //     return 2;
    // else if(platform==1 && device >=3 )
    //     return 3;
    else if(platform==1 && device <=3 )
         return 3;
}



void print_device_info(cl_device_id device_id,int i,int j)
{
    cl_int err;
    cl_char platform_name[STR_LENGTH] = {0};
    cl_char vendor_name[STR_LENGTH] = {0};    
    cl_char device_name[STR_LENGTH] = {0};  
    cl_bool device_available;
    cl_uint device_freq;
    cl_uint no_of_parallel_cores;
    cl_platform_id platform_id;
    cl_device_type device_type;

    err = clGetDeviceInfo(device_id, CL_DEVICE_NAME, sizeof(device_name), &device_name, NULL);
    err = clGetDeviceInfo(device_id, CL_DEVICE_TYPE, sizeof(device_type), &device_type, NULL);
    err = clGetDeviceInfo(device_id, CL_DEVICE_VENDOR, sizeof(vendor_name), &vendor_name, NULL);
    err = clGetDeviceInfo(device_id, CL_DEVICE_PLATFORM, sizeof(platform_id), &platform_id, NULL);
    err = clGetPlatformInfo(platform_id, CL_PLATFORM_NAME, sizeof(platform_name), &platform_name, NULL);
    err = clGetDeviceInfo(device_id, CL_DEVICE_MAX_CLOCK_FREQUENCY, sizeof(device_freq), &device_freq, NULL);
    err = clGetDeviceInfo(device_id, CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(no_of_parallel_cores), &no_of_parallel_cores, NULL);
    err = clGetDeviceInfo(device_id, CL_DEVICE_AVAILABLE, sizeof(device_available), &device_available, NULL);

    if(device_type  == CL_DEVICE_TYPE_GPU)
        fprintf(fp,"\nDeviceID_%d-%d: %u,\nDeviceName: %s, \nType: GPU, \nPlatform: %s, \nVendor: %s, \nMaxClockFrequency: %d, \nMaxParallelCore: %d, \nAvailability: %d\n",i,j,device_id,device_name,platform_name,vendor_name,device_freq,no_of_parallel_cores,device_available);
    if(device_type  == CL_DEVICE_TYPE_CPU)
        fprintf(fp,"\nDeviceID_%d-%d: %u,\nDeviceName: %s, \nType: CPU, \nPlatform: %s, \nVendor: %s, \nMaxClockFrequency: %d, \nMaxParallelCore: %d, \nAvailability: %d\n",i,j,device_id,device_name,platform_name,vendor_name,device_freq,no_of_parallel_cores,device_available);
    
}

void print_launch_info(KernelLaunchInfo& launchinfo){
    printf("\nLaunchinfo:\n");
    printf("\tKernel Id: %d:%s\n",launchinfo.task->traceID,launchinfo.task->taskID.c_str()) ;
    printf("\tJob ID %d, Instance ID %d, DAG ID %d\n",launchinfo.task->dagInfo->jobID,launchinfo.task->dagInfo->instanceID,launchinfo.task->dagInfo->globalDAGID);
    printf("\tPlatform: %d Device: %d \n",launchinfo.platform_pos,launchinfo.device_pos);
    printf("\tSize: %u\n",launchinfo.size);
    printf("\tOffset: %u\n",launchinfo.offset);
    printf("\tfrequency: %u\n",launchinfo.frequency);
    printf("\t Control Mode: %d\n",launchinfo.control_mode);
    printf("\tExecution time: %llu\n",(unsigned long long int)launchinfo.kex.turnaroundTime);
    // printf("\tMeasured_speed: %f\n",launchinfo.measured_speed);    
    printf("\tLast_speedup: %f\n",launchinfo.last_speedup);
    //printf("\n");
}

void copy_launch_info(KernelLaunchInfo* srcinfo,KernelLaunchInfo* dstinfo)
{
    dstinfo->task=srcinfo->task;
    dstinfo->kernel_index=srcinfo->kernel_index;
    //dstinfo->io=srcinfo->io;
    dstinfo->platform_pos=srcinfo->platform_pos;
    dstinfo->device_pos=srcinfo->device_pos;
    dstinfo->device_index=srcinfo->device_index;
    dstinfo->size=srcinfo->size;
    dstinfo->offset=srcinfo->offset;
    dstinfo->queued=srcinfo->queued;  
    dstinfo->reset=srcinfo->reset; 
    dstinfo->released_b=srcinfo->released_b; 
    dstinfo->released_d=srcinfo->released_d;
    dstinfo->frequency=srcinfo->frequency;
    dstinfo->control_mode=srcinfo->control_mode;
    dstinfo->priority=srcinfo->priority;
    dstinfo->not_done_count=srcinfo->not_done_count;
    dstinfo->full_task_klinfo=srcinfo->full_task_klinfo;
}

void printEventStatus(cl_event ev){  
    cl_int ev_status;
    cl_command_type ev_type;
    clGetEventInfo(ev,CL_EVENT_COMMAND_TYPE,sizeof(ev_type),&ev_type,NULL);      
    clGetEventInfo(ev,CL_EVENT_COMMAND_EXECUTION_STATUS,sizeof(ev_status),&ev_status,NULL);
    printf("\nEvent type: ");
    if(ev_type==CL_COMMAND_NDRANGE_KERNEL)
        printf(" CL_COMMAND_NDRANGE_KERNEL \n");
    else if(ev_type == CL_COMMAND_READ_BUFFER)
        printf(" CL_COMMAND_READ_BUFFER\n");
    else if(ev_type==CL_COMMAND_WRITE_BUFFER)
        printf(" CL_COMMAND_WRITE_BUFFER\n");
    else if(ev_type ==CL_COMMAND_BARRIER)
        printf(" CL_COMMAND_BARRIER\n");
    else if(ev_type ==CL_COMMAND_TASK)
        printf(" CL_COMMAND_TASK\n");

    printf("  Event status:");
    if(ev_status==CL_QUEUED)
        printf(" CL_QUEUED\n");
    else if(ev_status == CL_SUBMITTED)
        printf(" CL_SUBMITTED\n");
    else if(ev_status==CL_RUNNING)
        printf(" CL_RUNNING\n");
    else if(ev_status ==CL_COMPLETE)
        printf(" CL_COMPLETE\n");
    else
        printf(" ERROR\n");
}

void printEventStatus(cl_int status, cl_event ev){   

    cl_int ev_status;

    if (status==CL_SUCCESS){ 
        fprintf(fp,"\nPRINT EVENT STATUS: CL_SUCCESS");       
        clGetEventInfo(ev,CL_EVENT_COMMAND_EXECUTION_STATUS,sizeof(ev_status),&ev_status,NULL);
        if(ev_status==CL_QUEUED)
            fprintf(fp,"\nEvent status: CL_QUEUED");
        else if(ev_status == CL_SUBMITTED)
            fprintf(fp,"\nEvent status: CL_SUBMITTED");
        else if(ev_status==CL_RUNNING)
            fprintf(fp,"\nEvent status: CL_RUNNING");
        else if(ev_status ==CL_COMPLETE)
            fprintf(fp,"\nEvent status: CL_COMPLETE");
    }
    else 
    {   //fprintf(fp,"\nPRINT EVENT STATUS: FAILED"); 
        if(status==CL_INVALID_COMMAND_QUEUE)
            fprintf(fp,"\nEvent status: FAILED: CL_INVALID_COMMAND_QUEUE");
        else if(status==CL_INVALID_EVENT_WAIT_LIST)
            fprintf(fp,"\nEvent status: FAILED: CL_INVALID_EVENT_WAIT_LIST");
        else if(status==CL_OUT_OF_RESOURCES)
            fprintf(fp,"\nEvent status: FAILED: CL_OUT_OF_RESOURCES");
        else if(status==CL_OUT_OF_HOST_MEMORY)
            fprintf(fp,"\nEvent status: FAILED: CL_OUT_OF_HOST_MEMORY");
    }    
}

/*void print_span_info(FILE *fp)
{
    //double timing = (double)((end.tv_sec - start.tv_sec)*1000000 + (end.tv_usec - start.tv_usec));
    
    for(int i=0;i<6;i++)
    {
        fprintf(fp,"Device %d: ",i);
        for (std::map<std::string ,std::pair<struct timeval, struct timeval> >::iterator it=SpanInfo[i].begin(); it!=SpanInfo[i].end(); ++it)
        {
            std::string KernelName=it->first;
            struct timeval start=(it->second).first;
            struct timeval end=(it->second).second;
            double start_time = (double)((start.tv_sec*1000000 + start.tv_usec));
            double end_time = (double)((end.tv_sec*1000000 + end.tv_usec));
            fprintf(fp,"%s, %lf, %lf;",KernelName.c_str(),start_time,end_time);
            fprintf(fp, "%s, %lf, %lf;",KernelName.c_str(),start_time,end_time);
        }
        fprintf(fp,"\n");
    }

    fclose(fp);
}
*/

std::string getFileName(std::string filePath, bool withExtension = true, char seperator = '/')
{
    // Get last dot position
    std::size_t dotPos = filePath.rfind('.');
    std::size_t sepPos = filePath.rfind(seperator);
 
    if(sepPos != std::string::npos)
    {
        return filePath.substr(sepPos + 1, filePath.size() - (withExtension || dotPos != std::string::npos ? 1 : dotPos) );
    }
    return "";
}

void strip_ext(char *fname)
{
    char *end = fname + strlen(fname);

    while (end > fname && *end != '.') {
        --end;
    }

    if (end > fname) {
        *end = '\0';
    }
}

void host_synchronize(std::vector< std::vector<cl_command_queue>>& cmd_qs){

    printf("Synchronizing host with %d kernels\n",nKernels);

    while(nKernels>0)
        sleep(0.001);

    for(int i=0;i<NumOfPlatforms;i++) 
        for(int j=0;j<cmd_qs[i].size();j++)
        {  
            clFlush(cmd_qs[i][j]);
            clFinish(cmd_qs[i][j]);

        }    

    printf("Host Synchronised end: %d\n",nKernels);
        
    // while (nCPU < NumOfSysCPUs || nGPU < NumOfSysGPUs)
    //     ;
    // while(!callback_queue.second.empty())
    // {
       
    //    while(callback_queue.second.top()==false)
    //     ;
    //    fprintf(fp,"\nThreadId: %u",callback_queue.first.pop()); 
    //    callback_queue.second.pop(); 
    // }
   
    //for event in events:
    //    event.wait()
  
        
}

void release_buffers( std::vector<cl_mem>& buffers) {

    int i, n;
    n = buffers.size();
    for (i = 0; i < n; ++i)
    {   
        clReleaseMemObject(buffers[i]);
    }
    buffers.clear();

    return;
}

void release_host_arrays(std:: vector <void*> &data){
    fprintf(fp,"release_host_arrays begin\n");
    int i,n;
    n=data.size();
    int nc=0;
    printf("n=%d\n",n);
    for(i=0;i<n;i++)
    {
        printf("i=%d\n",i);
        if (data[i]!=NULL)
            free(data[i]);
        else 
        {    nc++;
            //printf("i=%d => NULL \n",i);
        }
    }
    printf("not free data %d\n",nc);
    data.clear();
    fprintf(fp,"release_host_arrays end\n");
    return;
} 

void release_kernel_events(KernelEvents& k) {
    int i;
    for (i = 0; i < k.write.size(); ++i)
    {
        clReleaseEvent(k.write[i]);
    }
    clReleaseEvent(k.exec);
    for (i = 0; i < k.read.size(); ++i)
    {
        clReleaseEvent(k.read[i]);
    }
    k.write.clear();
    k.read.clear();
    return;
}

void release_programs( std::vector<cl_program> all_programs ){
    for (int i = 0; i < all_programs.size(); ++i)
    {
        clReleaseProgram(all_programs[i]);
    }

}

void release_everything(std::vector<cl_context>& ctxs, std::vector< std::vector<cl_command_queue> >& cmd_qs) {

    int i, j;
    for (j = 0; j < NumOfPlatforms; ++j)
    {
        for (i = 0; i < cmd_qs[j].size(); ++i)
        {
            clFinish(cmd_qs[j][i]);
            
        }
    }
    
    for (j = 0; j < NumOfPlatforms; ++j)
    {
        for (i = 0; i < cmd_qs[j].size(); ++i)
        {
            cl_int status=clReleaseCommandQueue(cmd_qs[j][i]);
            // if(status==CL_SUCCESS)
            //     printf("clReleaseCommandQueue success%d %d\n",j,i);
            // else
            //     printf("clReleaseCommandQueue failed%d %d\n",j,i);
        }
    }
    
    for (i = 0; i < ctxs.size(); ++i)
    {
        cl_int status=clReleaseContext(ctxs[i]);
        // if(status==CL_SUCCESS)
        //     printf("clReleaseContext success %d\n",i);
        // else
        //     printf("clReleaseContext failed %d\n",i);
    }
    
    // printf("Exiting from  release_everything\n");
}
