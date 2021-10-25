#include "core.inl"
int main(int argc, char const *argv[])
{	
    char cmd[200];
    sprintf(cmd, "cpufreq-set -c 0 --max %uHz --min %uHz ",1500000000, 1500000000); 
    system(cmd);

    load_config("./configure_input.txt");
    printf("\nInput read from file: %s %s\n",argv[1],argv[2]);//argv[1] is trace_file, argv[2] is dag_history_file
    std::map<int,int> task_per_hp=read_dag_file(argv[2]); 
    printf("Input read_dag_file done\n");
    for(auto elem : task_per_hp)
    {
       printf("hp: %d dags: %d\n", elem.first ,elem.second);
    }
    
    read_dag_structure("./dag_structure/");
    // printf("Input read_dag_structure done\n");

    /*create Output dump file*/
    create_output_file(argv[1]);
      
    /* Populate all deviceIds in a vector of vector, command queues and context for each platform*/
    get_all_devices();
    host_initialize(all_devices, all_ctxs, all_cmd_qs);
    get_device_specification("speedup_config.csv"); 
    //printf("get_device_specification\n");
    /* Prit details of all devices*/

    print_all_device_info(all_devices);
    fflush(fp);     

    //printf("all device managed\n");

    //Build kernel objects
    int profile_trace_value;
    if(isProfileMode==1)
    {
      //int profile_trace_value = atoi(argv[3]);      
      build_kernel_object(argv[3]);//argv[3] is tinfo name to be built
      profile_trace_value = atoi(argv[4]);//argv[4] unique trace_id      
      micro_kernel_device = atoi(argv[5]);//argv[5] is micro_kernel_device
      printf("Finished building kernel objects!!\n");
    
    }
    else
    {
      build_all_kernel_objects("./tinfo/");
      printf("Finished building all kernel objects!!\n");
    }  

    /*get input file for trace*/
    int traceCount=read_trace_file(argv[1]);
    printf("\ntraceCount %d \n",traceCount);
    populate_task_queue();
    // printf("populate_task_queue DONE\n");
    // print_task_map();  

    if(micro_kernel_device!=-1)
      create_micro_kernel(micro_kernel_device);

    /*Start scheduler*/     

    
    sleep(30);
    
    int rc;
    pthread_attr_t attr;
    struct sched_param param;
    pthread_t scheduler = pthread_self(); 
    rc = pthread_attr_init (&attr);
    rc = pthread_attr_getschedparam (&attr, &param);
    (param.sched_priority)++;
    // param.sched_priority = sched_get_priority_max(SCHED_RR);
    // pthread_setschedparam(scheduler,SCHED_RR,&param);
    rc = pthread_attr_setschedparam (&attr, &param);

    pthread_t  thread_controller;

    //if()
    //create_sub_kernels();

    try
    {
        for(int i = 0 ;i<numOfHyperperiod; i++)
        { 
            reset_launch_info();

            printf("\nHYPER-PERIOD %d StartTime run_scheduler: %llu\n",i,get_current_time()); 
            fprintf(t_result,"\n___________Hyper-period: %d____________\n",i);
            // fflush(t_result); 

            pthread_t thread_taskQtoReadyB, thread_temperature_monitor,thread_scheduler;  

            struct timeval c_time;
            gettimeofday(&c_time,NULL); 
            START_TIME=(unsigned long long int )(c_time.tv_sec*1000000+c_time.tv_usec); 
            // cout << "\tTrying for DAG ID " << taskMap.begin()->first.first << '\t' << "Kernel ID " << taskMap.begin()->first.second << "KernelObject: "<<taskMap.begin()->second->kernel_index<<'\n';
            
            pthread_create(&thread_taskQtoReadyB, NULL, taskToReadyB, &traceCount); 

            
            if(monitorTemp==1)
              pthread_create(&thread_temperature_monitor, NULL, temperature_monitor, NULL);          
            
            while(is_rbuffer_filled()==false)
                sleep(0.001);
            if(isProfileMode==1)
              pthread_create(&thread_scheduler, &attr, run_kernel, &profile_trace_value);
            else
            {   if(task_per_hp.find(i)== task_per_hp.end())
                    printf("Key not found in task_per_hp for hp %d\n",i);
                else
                {
                    std::pair<int, int> trace_hp=make_pair(i,task_per_hp.find(i)->second);
                    pthread_create(&thread_scheduler, &attr, run_scheduler, &trace_hp);          
                }
            }
            pthread_join(thread_taskQtoReadyB, NULL); 
            printf("Thread ReadyBuffer update joined: %llu\n",get_current_time());          

            pthread_join(thread_scheduler, NULL);
            printf("Thread Thread scheduler joined: %llu\n",get_current_time());
            
            host_synchronize(all_cmd_qs);            
            SchedulerFinish=1; 
            
            if(CONTROLLER_MODE>0)
            {  
              int mode=CONTROLLER_MODE;
              pthread_create(&thread_controller, NULL, mode_controller,&mode ); 
            }
            if(generatePlot==1)
              generate_plot_data(traceCount);  

            if(monitorTemp==1)       
            { 
              pthread_join(thread_temperature_monitor, NULL); 
              printf("thread_temperature_monitor joined\n");
              print_tempMap();
              fprintf(tmp_result,"----------------%d-----------------\n",i+1);
            }
            
            if(GLOBAL_RECOVERY_ENABLE && global_recovery_tasks.size()>0)
            {
                printf("+++++++++ENTERED GLOBAL RECOVERY MODE++++++++++\n");
                global_recovery(global_recovery_tasks);
                global_recovery_tasks.clear(); 
                
            }

            printf("\n");
        }
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception caught : " << e.what() << std::endl;
    }    

    
    if(CONTROLLER_MODE>0)
    {   pthread_join(thread_controller, NULL);   
        printf("Temperature controller is done.\n");
    }
        
    /*Release objects*/    
    for (auto itr = taskMap.begin(); itr != taskMap.end(); ++itr)       
    { 
        //printf("release_buffers: %d-%d : %d, %d\n",itr->first.first,itr->first.second,itr->second[0]->released_b,itr->second[0]->released_d);
            
        if(itr->second[0]->released_b==0)
        {
            //printf("release_buffers: %d-%d \n",itr->first.first,itr->first.first);
                
          release_buffers(itr->second[0]->io);    
          itr->second[0]->released_b=1;
        }
        /*if(itr->second[0]->released_d==0)
        {

          release_host_arrays(itr->second[0]->task->data);
          itr->second[0]->released_d=1;
          printf("release_buffers:release_host_arrays done\n");
        }*/
    }
    
    release_everything(all_ctxs, all_cmd_qs);
    printf("released_everything\n");

    // std::ofstream ofs;
    // ofs.open(argv[2],std::ios_base::app);
    // printf("-------------------------Execution Statistics--------------------------------------\n");
    // // printf("dag \t\t task \t\t w_delay \t\t w \t\t e_delay \t\t e \t\t r_delay \t\t r \t\t k_ex \t\t h_ex \t\t h_over \t\t cb \t\t cb_over\n");
    // for (auto itr = taskMap.begin(); itr != taskMap.end(); ++itr) { 
    //   cout << "\tDAG ID " << itr->first.first << '\t' << "Kernel ID " << itr->first.second << '\n'; 
    //   dump_execution_time_statistics(itr->second,itr->first.first,itr->first.second,ofs);
    
    // }
    // ofs.close(); 
    

    fprintf(fp,"\n");
    fclose (fp); 
    fclose (t_result); 
    fclose (tmp_result);
    fclose (m_result);
    //fclose (o_result);
    printf("\n");
    printf("FINISHED!\n\n");    
	  return 0;
}
