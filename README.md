# A-DTM
Adaptive Dynamic Thermal Manager
=================================

Recent trends in real-time applications have raised the demand for high-throughput embedded platforms with integrated CPU-GPU based Systems-On-Chip (SoCs). The enhanced performance of such SoCs, however comes at the cost of increased power consumption, resulting in significant heat dissipation and high on-chip temperatures. The prolonged occurrences of high on-chip temperature can cause accelerated in-circuit ageing, which severely degrades the long-term performance and reliability of the chip. Violation of thermal constraints leads to on-board dynamic thermal management kicking-in, which may result in timing unpredictability for real-time tasks due to transient performance degradation. Recent work in adaptive software design have explored this issue from a control theoretic stand-point, striving for smooth thermal envelopes by tuning the core frequency.Existing techniques do not handle thermal violations for periodic real-time task sets in the presence of dynamic events like change of task periodicity, more so in the context of heterogeneous SoCs with integrated CPU-GPUs.

This work presents an OpenCL runtime extension for thermal-aware scheduling of periodic, real-time tasks on heterogeneous multi-core platforms. The framework A-DTM or Adaptive Dynamic Thermal Manager mitigates dynamic thermal violations by adaptively tuning task mapping parameters, with the eventual control objective of satisfying both platform-level thermal constraints and task-level deadline constraints. Multiple platform-level control actions like task migration, frequency tuning and idle slot insertion are used as the task mapping parameters. 

![A-DTM overview](TECS_workflow_new1.png?style=centerme)


The work flow of the framework is depicted in the given figure. For a given hyper-period, the framework dispatch each task in the platform P folowing the last updated schedule S. The thermal profile during the execution of the each task is analysed to identify the peak temperature in each device in the platform. If there is no thermal violation, the framework does not take any action and continues with the existing task mapping decisions in S. If there is any kind of thermal violation, the temperature is reduced by the recovery controller that work in two distinct modes - local and global recovery mode. In the global recovery mode, a closed feedback loop is used to adaptively change the task mapping of the tasks responsible for thermal violation. It takes suitable control actions such as modifying device frequency, migrating task from hotter to cooler device, increasing idleness between consecutive tasks. In this context, feedback refers to the difference in expected and the actual performance with respect to peak temperature and latency of the concerned task. Based on the observed feedback, the current thermal behaviour and the resource availability of the platform P, a control action is selected such that the peak temperature drops below the thermal threshold while ensuring there is no deadline violation.

The details work mechanism is covered in our <a href="https://dl.acm.org/doi/10.1145/3477028"> paper</a> published in ACM Transactions on Embedded Computing Systems.



The framework is build on OpenCL,C++ and python. We briefly present the project hierarchy and a short description for every important folder and file present in the project. An initial setup description is also provided for OpenCL supported heterogeneous platform.


Project Hierarchy
-----------------

<pre>
<code>
.
├── <b>input_framework_parameters.py</b> (Python script to take input for setting up the different knob parameters for the framework.)
├── <b>input_kernel_files.py</b> (Python script to take input for the OpenCL kernels in the task set.)
├── <b>input_initial_schedule.py</b> (Python script to take input for setting up the initial schedule of the task set.)
├── <b>configure.py</b> (Configures certain parameters required by the framework)
├── <b>core.h</b> (File containing all header libraries and data structures.)
├── <b>core.inl</b> (File contaning code for all functions used in the framework.)
├── <b>string.h</b> (File containing all header libraries, function declaration for string functions used in framework.)
├── <b>string.inl</b> (File contaning code for all string functions used in the framework.)
├── <b>main.cpp</b> (File containing driver code for the framework.)
└── <b>src</b> (Folder containing the source code for each OpenCL kernel.)
└── <b>tinfo</b> (Folder containing the kernel parameters for each OpenCL kernel.)
└── <b>trace</b> (Folder containing the scheduling details for each task in the task set.)
└── <b>dag_history</b> (Folder containing the initial schedule for the task set.)
└── <b>dag_structure</b> (Folder containing the structure of each task in the task set .)
└── <b>output</b> (Folder containing all the output of the framework.)
    ├── <b>log</b> (Folder containing log files generated as a result of running the framework.)
    ├── <b>temperature</b> (Folder containing thermal data of the platform obtained from the thermal sensors while running each OpenCL task.)
    ├── <b>time</b> (Folder containing latency details of each OpenCL task.)
    ├── <b>output</b> (Folder containing data dump outputs after execution of individual OpenCL kernels )
  </code>
  </pre>
 

Software Dependencies
----------------------

+ Python 2.7 
+ Tkinter python GUI library
+ C++ 4.2 Standard Library headers
+ OpenCL Runtime Device Drivers (Minimum 1.2)
+ PoCL v1.6

Platform Setup
------------------

+ Platform must have CPU and GPU with OpenCL vendor support. We have used ODROID XU4 platform for evaluation. For installing OpenCL in Odroid XU4, This <a href="https://www.cnx-software.com/2018/05/13/how-to-get-started-with-opencl-on-odroid-xu4-board-with-arm-mali-t628mp6-gpu"> link </a> may be refered for installing OpenCL in Odroid.
+ If openCL driver not available for CPU, an open-source OpenCL runtime library called <a href="https://github.com/pocl/pocl">Portable OpenCL (PoCL) </a> that provides OpenCL support for the CPUs can be used. This enables CPU to act as both a host processor and an OpenCL device. <a href="http://portablecl.org/docs/html/install.html"> This documentation </a> can be refered for installing PoCL.
+ Separate the CPUs for OpenCL kernel computing (big CPU) and host programming (LITTLE CPU). The OS services are to be binded to the LITTLE CPUs by modifying the OS boot file settings and burning to the OS image. We have used eMMC Module Reader Board to edit the boot.inl file and burn the edited OS in the eMMC card.
+ The tripping point of the platform need to be set by modifying the sys files. We have used this command in Odroid XU4.
```sh
$ sudo echo 90000 | sudo tee /sys/devices/virtual/thermal/thermal_zone{0,1,2,3}/trip_point_4_temp
```

Framework commands
-------------------

#### To compile:

+ To compile
```sh
$ g++ main.cpp -O3 -lm -lOpenCL -lpthread -I /usr/include/CL_1_2/ -o adtm
```


####  Input formats:

+ To take inputs
```sh
$ python input_framework_parameters.py 
$ python input_kernel_files.py
$ python input_initial_schedule.py
```

+ Taking inputs for task set


![A-DTM overview](TECS_workflow_new1.png?style=centerme)

#### To test:
+ To run program
```sh
$ sudo taskset -c 4-7 ./adtm trace/trace_<counter>.stats ./dag_history/dag_history_<counter>.stats 
```

####  Output formats:


