from tkinter import *
import pandas as pd
from tkinter import filedialog
from tkinter import ttk
from tkinter.filedialog import asksaveasfile
import os.path
from ttkthemes import ThemedStyle
import collections 
app=Tk()

width= app.winfo_screenwidth() 
height= app.winfo_screenheight()
#setting tkinter window size
app.geometry("%dx%d" % (width*(0.6), height))

app.title("configure_input.txt")

#framework control knobs
header=Label(text="Framework Control Knobs",font='Verdana 25 bold')
header.place(x=150,y=20)
#app['background']='#856ff8'
style=ttk.Style()
style.configure("BW.TLabel", foreground="black", background="white")
Def_Scrollbar = Scrollbar(app)
Def_Scrollbar.pack(side='right',fill='y')
# heading= Label(text="input structures", fg="black", bg="blue", width="500", height="3", font="10")

# heading.pack(pady=20)

global required1
required1=0


STR_LENGTH = StringVar()
NumOfJobs =StringVar()
NumCoresPerDevice = StringVar()
NumofCPUs = StringVar()
NumOfSensors = StringVar()
NumOfTempBand = StringVar()
PLATFORM_GPU = StringVar()
PLATFORM_CPU = StringVar()
G_BIG = StringVar()
G_LITTLE = StringVar()
C_BIG = StringVar()
TempInterval = StringVar()
numOfHyperperiod = StringVar()
LOG_LEVEL=StringVar()
LOG_SCHEDULER=StringVar()
LOG_PROFILE=StringVar()
CONTROLLER_MODE=StringVar()
FACTOR=StringVar()
SAFE_MODE=StringVar()
EVENTS_ENABLE=StringVar()
RACE_TO_IDLE=StringVar()
time_buffer=StringVar()
isProfileMode=StringVar()
micro_kernel_device=StringVar()
refresh_data_per_hyperperiod=StringVar()
monitorTemp=StringVar()
generatePlot=StringVar()
LOCAL_RECOVERY_ENABLE=StringVar()
GLOBAL_RECOVERY_ENABLE=StringVar()
THERMAL_THRESHOLD_H=StringVar()
THERMAL_THRESHOLD_L=StringVar()
CREATE_SUB_KERNEL=StringVar()
READ_INPUT_DATA=StringVar()
FREQUENCY_MODE=StringVar()
MIGRATION_MODE=StringVar()
TASK_SHIFT_MODE=StringVar()

STR_LENGTH.set("0")
NumOfJobs.set("0")
NumCoresPerDevice.set("0")
NumofCPUs.set("0")
NumOfSensors.set("0")
NumOfTempBand.set("0")
PLATFORM_GPU.set("0")
PLATFORM_CPU.set("0")
G_BIG.set("0")
G_LITTLE.set("0")
C_BIG.set("0")
TempInterval.set("0")
numOfHyperperiod.set("0")
LOG_LEVEL.set("0")
LOG_SCHEDULER.set("0")
LOG_PROFILE.set("0")
CONTROLLER_MODE.set("0")
FACTOR.set("0")
SAFE_MODE.set("0")
EVENTS_ENABLE.set("0")
RACE_TO_IDLE.set("0")
time_buffer.set("0")
isProfileMode.set("0")
micro_kernel_device.set("0")
refresh_data_per_hyperperiod.set("0")
monitorTemp.set("0")
generatePlot.set("0")
LOCAL_RECOVERY_ENABLE.set("0")
GLOBAL_RECOVERY_ENABLE.set("0")
THERMAL_THRESHOLD_H.set("0")
THERMAL_THRESHOLD_L.set("0")
CREATE_SUB_KERNEL.set("0")
READ_INPUT_DATA.set("0")
FREQUENCY_MODE.set("0")
MIGRATION_MODE.set("0")
TASK_SHIFT_MODE.set("0")


def DarkenLabel():
    label.config(bg="gray")

def save_info():
    global required1
    STR_LENGTH_info = STR_LENGTH.get()
    NumOfJobs_info = NumOfJobs.get()
    NumCoresPerDevice_info = NumCoresPerDevice.get()
    NumofCPUs_info = NumofCPUs.get()
    NumOfSensors_info = NumOfSensors.get()
    NumOfTempBand_info = NumOfTempBand.get()
    PLATFORM_GPU_info = PLATFORM_GPU.get()
    PLATFORM_CPU_info = PLATFORM_CPU.get()
    G_BIG_info = G_BIG.get()
    G_LITTLE_info = G_LITTLE.get()
    C_BIG_info = C_BIG.get()
    TempInterval_info = TempInterval.get()
    numOfHyperperiod_info = numOfHyperperiod.get()
    LOG_LEVEL_info=LOG_LEVEL.get()
    LOG_SCHEDULER_info=LOG_SCHEDULER.get()
    LOG_PROFILE_info=LOG_PROFILE.get()
    CONTROLLER_MODE_info=CONTROLLER_MODE.get()
    FACTOR_info=FACTOR.get()
    SAFE_MODE_info=SAFE_MODE.get()
    EVENTS_ENABLE_info=EVENTS_ENABLE.get()
    RACE_TO_IDLE_info=RACE_TO_IDLE.get()
    time_buffer_info= time_buffer.get()
    isProfileMode_info=isProfileMode.get()
    micro_kernel_device_info=micro_kernel_device.get()
    refresh_data_per_hyperperiod_info=refresh_data_per_hyperperiod.get()
    monitorTemp_info=monitorTemp.get()
    generatePlot_info= generatePlot.get()
    LOCAL_RECOVERY_ENABLE_info=LOCAL_RECOVERY_ENABLE.get()
    GLOBAL_RECOVERY_ENABLE_info=GLOBAL_RECOVERY_ENABLE.get()
    THERMAL_THRESHOLD_H_info=THERMAL_THRESHOLD_H.get()
    THERMAL_THRESHOLD_L_info=THERMAL_THRESHOLD_L.get()
    CREATE_SUB_KERNEL_info=CREATE_SUB_KERNEL.get()
    READ_INPUT_DATA_info=READ_INPUT_DATA.get()
    FREQUENCY_MODE_info= FREQUENCY_MODE.get()
    MIGRATION_MODE_info=MIGRATION_MODE.get()
    TASK_SHIFT_MODE_info= TASK_SHIFT_MODE.get()
    print(STR_LENGTH_info,NumOfJobs_info,NumCoresPerDevice_info,NumofCPUs_info,NumOfSensors_info,NumOfTempBand_info,PLATFORM_GPU_info,PLATFORM_CPU_info,G_BIG_info,G_LITTLE_info,C_BIG_info,TempInterval_info,numOfHyperperiod_info,LOG_LEVEL_info,LOG_SCHEDULER_info,LOG_PROFILE_info,CONTROLLER_MODE_info,FACTOR_info,SAFE_MODE_info,EVENTS_ENABLE_info,RACE_TO_IDLE_info,time_buffer_info,isProfileMode_info,micro_kernel_device_info,refresh_data_per_hyperperiod_info,monitorTemp_info,generatePlot_info,LOCAL_RECOVERY_ENABLE_info,GLOBAL_RECOVERY_ENABLE_info,THERMAL_THRESHOLD_H_info,THERMAL_THRESHOLD_L_info,CREATE_SUB_KERNEL_info,READ_INPUT_DATA_info,FREQUENCY_MODE_info,MIGRATION_MODE_info,TASK_SHIFT_MODE_info)
    
    save_path4 = str('E:/gem5/framework_structure/new_folder/')
    folder_name=(str(required1)+"input_structure")
    folder=os.path.join(save_path4,folder_name)
    name_of_file4=(str("configure_input"))
    complete_file_name4=os.path.join(folder,name_of_file4+'.txt')
    os.makedirs(folder)
    
    required1=required1+1
    
    text_file=open(complete_file_name4,'w')
    ##text_file=open(text_file,'w')
    text_file.write("STR_LENGTH = "+ STR_LENGTH_info)
    text_file.write("\n")
    text_file.write("NumOfJobs  = "+ NumOfJobs_info)
    text_file.write("\n")
    text_file.write("NumCoresPerDevice = "+ NumCoresPerDevice_info)
    text_file.write("\n")
    text_file.write("NumofCPUs = "+ NumofCPUs_info)
    text_file.write("\n")
    text_file.write("NumOfSensors = "+ NumOfSensors_info)
    text_file.write("\n")
    text_file.write("NumOfTempBand = "+ NumOfTempBand_info)
    text_file.write("\n")
    text_file.write("PLATFORM_GPU  = "+ PLATFORM_GPU_info)
    text_file.write("\n")
    text_file.write("PLATFORM_CPU = "+ PLATFORM_CPU_info)
    text_file.write("\n")
    text_file.write("G_BIG = "+ G_BIG_info)
    text_file.write("\n")
    text_file.write("G_LITTLE = "+ G_LITTLE_info)
    text_file.write("\n")
    text_file.write("C_BIG = "+ C_BIG_info)
    text_file.write("\n")
    text_file.write("TempInterval = "+ TempInterval_info)
    text_file.write("\n")
    text_file.write("LOG_LEVEL = "+ LOG_LEVEL_info)
    text_file.write("\n")
    text_file.write("LOG_SCHEDULER = "+ LOG_SCHEDULER_info)
    text_file.write("\n")
    text_file.write("LOG_PROFILE = "+ LOG_PROFILE_info)
    text_file.write("\n")
    text_file.write("CONTROLLER_MODE = "+ CONTROLLER_MODE_info)
    text_file.write("\n")
    text_file.write("SAFE_MODE = "+ SAFE_MODE_info)
    text_file.write("\n")
    text_file.write("EVENTS_ENABLE = "+ EVENTS_ENABLE_info)
    text_file.write("\n")
    text_file.write("RACE_TO_IDLE = "+ RACE_TO_IDLE_info)
    text_file.write("\n")
    text_file.write("time_buffer = "+ time_buffer_info)
    text_file.write("\n")
    text_file.write("isProfileMode = "+ isProfileMode_info)
    text_file.write("\n")
    text_file.write("micro_kernel_device = "+ micro_kernel_device_info)
    text_file.write("\n")
    text_file.write("refresh_data_per_hyperperiod = "+ refresh_data_per_hyperperiod_info)
    text_file.write("\n")
    text_file.write("monitorTemp = "+ monitorTemp_info)
    text_file.write("\n")
    text_file.write("generatePlot = "+ generatePlot_info)
    text_file.write("\n")
    text_file.write("LOCAL_RECOVERY_ENABLE = "+ LOCAL_RECOVERY_ENABLE_info)
    text_file.write("\n")
    text_file.write("GLOBAL_RECOVERY_ENABLE = "+ GLOBAL_RECOVERY_ENABLE_info)
    text_file.write("\n")
    text_file.write("THERMAL_THRESHOLD_H = "+ THERMAL_THRESHOLD_H_info)
    text_file.write("\n")
    text_file.write("THERMAL_THRESHOLD_L = "+ THERMAL_THRESHOLD_L_info)
    text_file.write("\n")
    text_file.write("CREATE_SUB_KERNEL = "+ CREATE_SUB_KERNEL_info)
    text_file.write("\n")
    text_file.write("READ_INPUT_DATA = "+ READ_INPUT_DATA_info)
    text_file.write("\n")
    text_file.write("FREQUENCY_MODE = "+ FREQUENCY_MODE_info)
    text_file.write("\n")
    text_file.write("MIGRATION_MODE = "+ MIGRATION_MODE_info)
    text_file.write("\n")
    text_file.write("TASK_SHIFT_MODE = "+ TASK_SHIFT_MODE_info)
    text_file.write("\n")
    
    text_file.close()
    
# def save_in():
#     print(SAFE_MODE.get())
#     print(EVENTS_ENABLE.get())
    
    
STR_LENGTH_text=Label(text="STR_LENGTH",font='Verdana 10 bold')
NumOfJobs_text=Label(text="NumOfJobs",font='Verdana 10 bold')
NumCoresPerDevice_text=Label(text="NumCoresPerDevice",font='Verdana 10 bold')
NumofCPUs_text=Label(text="NumofCPUs",font='Verdana 10 bold')
NumOfSensors_text=Label(text="NumOfSensors",font='Verdana 10 bold')
NumOfTempBand_text=Label(text="NumOfTempBand",font='Verdana 10 bold')
PLATFORM_GPU_text=Label(text="PLATFORM_GPU",font='Verdana 10 bold')
PLATFORM_CPU_text=Label(text="PLATFORM_CPU",font='Verdana 10 bold')
G_BIG_text=Label(text="G_BIG",font='Verdana 10 bold')
G_LITTLE_text=Label(text="G_LITTLE",font='Verdana 10 bold')
C_BIG_text=Label(text="C_BIG",font='Verdana 10 bold')
TempInterval_text=Label(text="TempInterval",font='Verdana 10 bold')
numOfHyperperiod_text=Label(text="numOfHyperperiod",font='Verdana 10 bold')
LOG_LEVEL_text=Label(text="LOG_LEVEL",font='Verdana 10 bold')
LOG_SCHEDULER_text=Label(text="LOG_SCHEDULER",font='Verdana 10 bold')
LOG_PROFILE_text=Label(text="LOG_PROFILE",font='Verdana 10 bold')
CONTROLLER_MODE_text=Label(text="CONTROLLER_MODE",font='Verdana 10 bold')
FACTOR_text=Label(text="FACTOR",font='Verdana 10 bold')
SAFE_MODE_text=Label(text="SAFE_MODE",font='Verdana 10 bold')
EVENTS_ENABLE_text=Label(text="EVENTS_ENABLE",font='Verdana 10 bold')
RACE_TO_IDLE_text=Label(text="RACE_TO_IDLE",font='Verdana 10 bold')
time_buffer_text=Label(text="time_buffer",font='Verdana 10 bold')
isProfileMode_text=Label(text="isProfileMode",font='Verdana 10 bold')
micro_kernel_device_text=Label(text="micro_kernel_device",font='Verdana 10 bold')
refresh_data_per_hyperperiod_text=Label(text="refreshdata_per_hyperperiod",font='Verdana 10 bold')
monitorTemp_text=Label(text="monitorTemp",font='Verdana 10 bold')
generatePlot_text=Label(text="generatePlot",font='Verdana 10 bold')
LOCAL_RECOVERY_ENABLE_text=Label(text="LOCAL_RECOVERY_ENABLE",font='Verdana 10 bold')
GLOBAL_RECOVERY_ENABLE_text=Label(text="GLOBAL_RECOVERY_ENABLE",font='Verdana 10 bold')
THERMAL_THRESHOLD_H_text=Label(text="THERMAL_THRESHOLD_UPPER",font='Verdana 10 bold')
THERMAL_THRESHOLD_L_text=Label(text="THERMAL_THRESHOLD_LOWER",font='Verdana 10 bold')
CREATE_SUB_KERNEL_text=Label(text="CREATE_SUB_KERNEL",font='Verdana 10 bold')
READ_INPUT_DATA_text=Label(text="READ_INPUT_DATA",font='Verdana 10 bold')
FREQUENCY_MODE_text=Label(text="FREQUENCY_MODE",font='Verdana 10 bold')
MIGRATION_MODE_text=Label(text="MIGRATION_MODE",font='Verdana 10 bold')
TASK_SHIFT_MODE_text=Label(text="TASK_SHIFT_MODE",font='Verdana 10 bold')
    
    

    
SAFE_MODE_entry1=Radiobutton(app,text="0",value=0,variable=SAFE_MODE)
SAFE_MODE_entry2=Radiobutton(app,text="1",value=1,variable=SAFE_MODE)
EVENTS_ENABLE_entry1=Radiobutton(app,text="0",value=0,variable=EVENTS_ENABLE)
EVENTS_ENABLE_entry2=Radiobutton(app,text="1",value=1,variable=EVENTS_ENABLE)
LOG_LEVEL_entry1=Radiobutton(app,text="0",value=0,variable=LOG_LEVEL)
LOG_LEVEL_entry2=Radiobutton(app,text="1",value=1,variable=LOG_LEVEL)


LOG_SCHEDULER_entry1=Radiobutton(app,text="0",value=0,variable=LOG_SCHEDULER)
LOG_SCHEDULER_entry2=Radiobutton(app,text="1",value=1,variable=LOG_SCHEDULER)

LOG_PROFILE_entry1=Radiobutton(app,text="0",value=0,variable=LOG_PROFILE)
LOG_PROFILE_entry2=Radiobutton(app,text="1",value=1,variable=LOG_PROFILE)

RACE_TO_IDLE_entry1=Radiobutton(app,text="0",value=0,variable=RACE_TO_IDLE)
RACE_TO_IDLE_entry2=Radiobutton(app,text="1",value=1,variable=RACE_TO_IDLE)

isProfileMode_entry1=Radiobutton(app,text="0",value=0,variable=isProfileMode)         #radio biutton
isProfileMode_entry2=Radiobutton(app,text="1",value=1,variable=isProfileMode)

micro_kernel_device_entry1=Radiobutton(app,text="0",value=0,variable=micro_kernel_device)
micro_kernel_device_entry2=Radiobutton(app,text="1",value=1,variable=micro_kernel_device)

refresh_data_per_hyperperiod_entry1=Radiobutton(app,text="0",value=0,variable=refresh_data_per_hyperperiod)
refresh_data_per_hyperperiod_entry2=Radiobutton(app,text="1",value=1,variable=refresh_data_per_hyperperiod)

monitorTemp_entry1=Radiobutton(app,text="0",value=0,variable=monitorTemp)       #radio biutton
monitorTemp_entry2=Radiobutton(app,text="1",value=1,variable=monitorTemp)

generatePlot_entry1=Radiobutton(app,text="0",value=0,variable=generatePlot)      #radio biutton
generatePlot_entry2=Radiobutton(app,text="1",value=1,variable= generatePlot) 

LOCAL_RECOVERY_ENABLE_entry1=Radiobutton(app,text="0",value=0,variable=LOCAL_RECOVERY_ENABLE)  #radio biutton
LOCAL_RECOVERY_ENABLE_entry2=Radiobutton(app,text="1",value=1,variable=LOCAL_RECOVERY_ENABLE) 

GLOBAL_RECOVERY_ENABLE_entry1=Radiobutton(app,text="0",value=0,variable=GLOBAL_RECOVERY_ENABLE)
GLOBAL_RECOVERY_ENABLE_entry2=Radiobutton(app,text="1",value=1,variable=GLOBAL_RECOVERY_ENABLE)

CREATE_SUB_KERNEL_entry1=Radiobutton(app,text="0",value=0,variable=CREATE_SUB_KERNEL)
CREATE_SUB_KERNEL_entry2=Radiobutton(app,text="1",value=1,variable=CREATE_SUB_KERNEL)

READ_INPUT_DATA_entry1=Radiobutton(app,text="0",value=0,variable=READ_INPUT_DATA)
READ_INPUT_DATA_entry2=Radiobutton(app,text="1",value=1,variable=READ_INPUT_DATA)

FREQUENCY_MODE_entry1=Radiobutton(app,text="0",value=0,variable=FREQUENCY_MODE)
FREQUENCY_MODE_entry2=Radiobutton(app,text="1",value=1,variable= FREQUENCY_MODE)

MIGRATION_MODE_entry1=Radiobutton(app,text="0",value=0,variable=MIGRATION_MODE)
MIGRATION_MODE_entry2=Radiobutton(app,text="1",value=1,variable=MIGRATION_MODE)

TASK_SHIFT_MODE_entry1=Radiobutton(app,text="0",value=0,variable=TASK_SHIFT_MODE)
TASK_SHIFT_MODE_entry2=Radiobutton(app,text="1",value=1,variable=TASK_SHIFT_MODE)
CONTROLLER_MODE_entry=ttk.Combobox(app,values=[0,1,2],width=30)

SAFE_MODE_entry1.place(x=620,y=100)
SAFE_MODE_entry2.place(x=700,y=100)

EVENTS_ENABLE_entry1.place(x=620,y=130)
EVENTS_ENABLE_entry2.place(x=700,y=130)

TASK_SHIFT_MODE_entry1.place(x=180,y=740)
TASK_SHIFT_MODE_entry2.place(x=260,y=740)
MIGRATION_MODE_entry1.place(x=620,y=740)
MIGRATION_MODE_entry2.place(x=700,y=740)
FREQUENCY_MODE_entry1.place(x=620,y=700)
FREQUENCY_MODE_entry2.place(x=700,y=700)
READ_INPUT_DATA_entry1.place(x=620,y=660)
READ_INPUT_DATA_entry2.place(x=700,y=660)
CREATE_SUB_KERNEL_entry1.place(x=620,y=620)
CREATE_SUB_KERNEL_entry2.place(x=700,y=620)
GLOBAL_RECOVERY_ENABLE_entry1.place(x=620,y=500)
GLOBAL_RECOVERY_ENABLE_entry2.place(x=700,y=500)
LOCAL_RECOVERY_ENABLE_entry1.place(x=620,y=460)
LOCAL_RECOVERY_ENABLE_entry2.place(x=700,y=460)
generatePlot_entry1.place(x=620,y=420)
generatePlot_entry2.place(x=700,y=420)
monitorTemp_entry1.place(x=620,y=380)
monitorTemp_entry2.place(x=700,y=380)
isProfileMode_entry1.place(x=620,y=260)
micro_kernel_device_entry1.place(x=620,y=300)
refresh_data_per_hyperperiod_entry1.place(x=620,y=340)
isProfileMode_entry2.place(x=700,y=260)
micro_kernel_device_entry2.place(x=700,y=300)
refresh_data_per_hyperperiod_entry2.place(x=700,y=340)
RACE_TO_IDLE_entry1.place(x=620,y=180)
RACE_TO_IDLE_entry2.place(x=700,y=180)

LOG_LEVEL_entry1.place(x=620,y=220)
LOG_LEVEL_entry2.place(x=700,y=220)




STR_LENGTH_text.place(x=15,y=100)
NumOfJobs_text.place(x=15,y=140)
NumCoresPerDevice_text.place(x=15,y=180)
NumofCPUs_text.place(x=15,y=220)
NumOfSensors_text.place(x=15,y=260)
NumOfTempBand_text.place(x=15,y=300)
PLATFORM_GPU_text.place(x=15,y=340)
PLATFORM_CPU_text.place(x=15,y=380)
G_BIG_text.place(x=15,y=420)
G_LITTLE_text.place(x=15,y=460)
C_BIG_text.place(x=15,y=500)
TempInterval_text.place(x=15,y=540)
numOfHyperperiod_text.place(x=15,y=580)
  ##radio button
LOG_SCHEDULER_text.place(x=400,y=540)   ##radio button
LOG_PROFILE_text.place(x=400,y=580)   ##radio button

LOG_SCHEDULER_entry1.place(x=620,y=540)
LOG_PROFILE_entry1.place(x=620,y=580)

LOG_SCHEDULER_entry2.place(x=700,y=540)
LOG_PROFILE_entry2.place(x=700,y=580)

CONTROLLER_MODE_text.place(x=15,y=620)   #drop_down 0,1,2
#FACTOR_text.place(x=15,y=780)  ##no need

SAFE_MODE_text.place(x=400,y=100)   ##radio 0,1
EVENTS_ENABLE_text.place(x=400,y=140)     #radio biutton
RACE_TO_IDLE_text.place(x=400,y=180)    #radio biutton
LOG_LEVEL_text.place(x=400,y=220)               
isProfileMode_text.place(x=400,y=260)           #radio biutton
micro_kernel_device_text.place(x=400,y=300)         #radio biutton
refresh_data_per_hyperperiod_text.place(x=400,y=340)   #radio biutton
monitorTemp_text.place(x=400,y=380)             #radio biutton
generatePlot_text.place(x=400,y=420)             #radio biutton
LOCAL_RECOVERY_ENABLE_text.place(x=400,y=460)         #radio biutton
GLOBAL_RECOVERY_ENABLE_text.place(x=400,y=500)     #radio biutton
THERMAL_THRESHOLD_H_text.place(x=15,y=660)        
THERMAL_THRESHOLD_L_text.place(x=15,y=700)
CREATE_SUB_KERNEL_text.place(x=400,y=620)      #radio biutton
READ_INPUT_DATA_text.place(x=400,y=660)           #radio biutton 
FREQUENCY_MODE_text.place(x=400,y=700)  #radio biutton
MIGRATION_MODE_text.place(x=400,y=740)#radio biutton
TASK_SHIFT_MODE_text.place(x=15,y=740)#radio biutton


STR_LENGTH_entry=Entry(textvariable=STR_LENGTH,width=30)
NumOfJobs_entry=Entry(textvariable=NumOfJobs,width=30)
NumCoresPerDevice_entry=Entry(textvariable=NumCoresPerDevice,width=30)
NumofCPUs_entry=Entry(textvariable=NumofCPUs,width=30)
NumOfSensors_entry=Entry(textvariable=NumOfSensors,width=30)
NumOfTempBand_entry=Entry(textvariable=NumOfTempBand,width=30)
PLATFORM_GPU_entry=Entry(textvariable=PLATFORM_GPU,width=30)
PLATFORM_CPU_entry=Entry(textvariable=PLATFORM_CPU,width=30)
G_BIG_entry=Entry(textvariable=G_BIG,width=30)
G_LITTLE_entry=Entry(textvariable=G_LITTLE,width=30)
C_BIG_entry=Entry(textvariable=C_BIG,width=30)
TempInterval_entry=Entry(textvariable=TempInterval,width=30)
numOfHyperperiod_entry=Entry(textvariable=numOfHyperperiod,width=30)

THERMAL_THRESHOLD_H_entry=Entry(textvariable=THERMAL_THRESHOLD_H,width=30)
THERMAL_THRESHOLD_L_entry=Entry(textvariable=THERMAL_THRESHOLD_L,width=30)


STR_LENGTH_entry.place(x=180,y=100)
NumOfJobs_entry.place(x=180,y=140)
NumCoresPerDevice_entry.place(x=180,y=180)
NumofCPUs_entry.place(x=180,y=220)
NumOfSensors_entry.place(x=180,y=260)
NumOfTempBand_entry.place(x=180,y=300)
PLATFORM_GPU_entry.place(x=180,y=340)
PLATFORM_CPU_entry.place(x=180,y=380)
G_BIG_entry.place(x=180,y=420)
G_LITTLE_entry.place(x=180,y=460)
C_BIG_entry.place(x=180,y=500)
TempInterval_entry.place(x=180,y=540)
numOfHyperperiod_entry.place(x=180,y=580)
CONTROLLER_MODE_entry.place(x=180,y=620)

THERMAL_THRESHOLD_H_entry.place(x=180,y=660)
THERMAL_THRESHOLD_L_entry.place(x=180,y=700)



button = Button(app,text="SaveData",font='Verdana 10 bold',command=save_info,width="10",height="1",bg="grey")

button.place(x=15,y=800)
app.mainloop()