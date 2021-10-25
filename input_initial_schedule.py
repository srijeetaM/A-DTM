from tkinter import *
import pandas as pd
from tkinter import filedialog
from tkinter import ttk
from tkinter.filedialog import asksaveasfile
import os.path
from ttkthemes import ThemedStyle
import collections 
app=Tk()
app.title("trace")
width= app.winfo_screenwidth() 
height= app.winfo_screenheight()
#setting tkinter window size
app.geometry("%dx%d" % (width, height))


global counter
global var
counter=0
a_dict = collections.defaultdict(list)
a_dict['global_id']={}
a_dict['unique_dag_id']={}
a_dict['platform_id']={}
a_dict['platform']={}
a_dict['deadline']={}
a_dict['wcet_cpu']={}
a_dict['wcet_gpu']={}
a_dict['input_arrival_time']={}
a_dict['hyper_period_entry']={}
a_dict['input_task_period']={}
a_dict['frequency']={}

Convolution=-1
Matrix_Multiplications_2=-1
Matxix_Multiplications_3=-1
Matrix_Transpose_and_Vector_Multiplication=-1
Scalar_Vector_and_Matrix_Multiplication=-1
General_Matrix_Multiply=-1


#app.configure(yscrollcommand=my_scrollbar.set)

unique_dag_id = StringVar()
wcet_cpu = StringVar()
wcet_gpu = StringVar()
deadline = StringVar()
input_arrival_time = StringVar()
hyper_period_entry = StringVar()
input_task_period=StringVar()
frequency=StringVar()
last_county=StringVar()

global k
k=0
def selo(event,platform_id_drop,counter):
    if(platform_id_drop.get()=='gpu'):
        combobox_b = ttk.Combobox(app,values=['177000000' ,'266000000' ,'350000000' ,'420000000' ,'480000000', '543000000' , '600000000'])
        combobox_b.place(x=1390,y=50+60*(counter+1))
        a_dict['frequency'][counter-1]=( combobox_b )
    elif(platform_id_drop.get()=='cpu'):
        combobox_b = ttk.Combobox(app,values=['200000000', '300000000', '400000000', '500000000', '600000000', '700000000', '800000000', '900000000', '1000000000' ,'1100000000', '1200000000', '1300000000', '1400000000', '1500000000' ,'1600000000', '1700000000', '1800000000', '1900000000', '2000000000'])
        combobox_b.place(x=1390,y=50+60*(counter+1))
        a_dict['frequency'][counter-1]=( combobox_b )

def save_trace1():
    
    
    save_path3 = str('.')
#     folder_name=(str(counter)+"dag_info")
#     folder=os.path.join(save_path3,folder_name)
    name_of_file_3=("dag_history_"+str(counter)+".stats")
    complete_file_name_3=os.path.join(save_path3,name_of_file_3)
    os.makedirs(folder)

    global text_file_3
    global text_file_4
        
    print(a_dict['platform_id'][0])
    
#     folder_name=(str(counter)+"trace")
#     folder=os.path.join(save_path3,folder_name)
    name_of_file_4=("trace_"+str(counter)+".stats")
    complete_file_name_4=os.path.join(save_path3,name_of_file_4)
   # os.makedirs(folder)
    text_file_4=open(complete_file_name_4,'w')
    text_file_3=open(complete_file_name_3,'w')
    global k
    for i in range(0,counter):
        if(a_dict['unique_dag_id'][i]=='2D Convolution'):
            Convolution=Convolution+1
            k=Convolution
            
        elif(a_dict['unique_dag_id'][i]=='2 Matrix Multiplications'):
            Matrix_Multiplications_2=Matrix_Multiplications_2+1
            k=Matrix_Multiplications_2
        elif(a_dict['unique_dag_id'][i]=='3 Matrix Multiplications'):
            Matxix_Multiplications_3=Matxix_Multiplications_3+1
            k=Matxix_Multiplications_3
        elif(a_dict['unique_dag_id'][i]=='Matrix Transpose and Vector Multiplication'):
            Matrix_Transpose_and_Vector_Multiplication=Matrix_Transpose_and_Vector_Multiplication+1
            k=Matrix_Transpose_and_Vector_Multiplication
        elif(a_dict['unique_dag_id'][i]=='Scalar Vector and Matrix Multiplication'):
            Scalar_Vector_and_Matrix_Multiplication=Scalar_Vector_and_Matrix_Multiplication+1
            k=Scalar_Vector_and_Matrix_Multiplication
        elif(a_dict['unique_dag_id'][i]=='General Matrix Multiply'):
            General_Matrix_Multiply=General_Matrix_Multiply+1
            k=General_Matrix_Multiply
            
            
        text_file_3.write(str(k))
        text_file_4.write(str(k))
        text_file_3.write(",")
        text_file_3.write(str(i))
        text_file_3.write(",")
        text_file_3.write(a_dict['hyper_period_entry'][i].get())
        text_file_3.write(",")
        text_file_3.write(a_dict['input_task_period'][i].get())
        text_file_3.write(",")
        text_file_3.write(a_dict['deadline'][i].get())
        text_file_3.write("\n")
    text_file_3.close()  
        
#     folder_name=(str(counter)+"trace")
#     folder=os.path.join(save_path3,folder_name)
#     name_of_file_4=(str(counter)+"trace")
#     complete_file_name_4=os.path.join(folder,name_of_file_4)
#     os.makedirs(folder)
#     text_file_4=open(complete_file_name_4,'w')
    for i in range(0,counter):
        
       # text_file_4.write(str(k))
        text_file_4.write(",")        
        text_file_4.write("0")
        text_file_4.write(",")
        text_file_4.write("0")                 
        text_file_4.write(",")
        text_file_4.write("-1")
        text_file_4.write(",")
        if(a_dict['platform_id'][i]=='cpu'):
            text_file_4.write(str(0))
        else:
            text_file_4.write(str(1))
        text_file_4.write(",")
        text_file_4.write("0")                 
        text_file_4.write(",")
        text_file_4.write(a_dict['wcet_cpu'][i].get())                 
        text_file_4.write(",")
        text_file_4.write(a_dict['wcet_gpu'][i].get())                 
        text_file_4.write(",")
        text_file_4.write(a_dict['deadline'][i].get())
        text_file_4.write(",")
        text_file_4.write("1")
        text_file_4.write(",")
        text_file_4.write(str(i))
        text_file_4.write(",")
        text_file_4.write("0")
        text_file_4.write(",")
        text_file_4.write(a_dict['frequency'][i].get())
        text_file_4.write("\n")
        
    text_file_4.close()
    app.destroy()

        
        
global_id_label=Label(text='Job instance id ',font='Verdana 8 bold')
platform_id_label=Label(text='Platform',font='Verdana 8 bold')
unique_dag_id_label=Label(text='kernel',font='Verdana 8 bold')
wcet_cpu_label=Label(text='WCET for CPU',font='Verdana 8 bold')
wcet_gpu_label=Label(text='WCET for GPU',font='Verdana 8 bold')
deadline_label=Label(text='Task deadline',font='Verdana 8 bold')
input_arrival_time_label=Label(text='Arrival time',font='Verdana 8 bold')
hyper_period_entry_label=Label(text='Hyper period entry',font='Verdana 8 bold')
input_task_period_label=Label(text='Task period',font='Verdana 8 bold')
freq_label=Label(text='Device Frequency',font='Verdana 8 bold')
platform_id_options=[
    'cpu','gpu',
]
kernel_options=['2D Convolution','2 Matrix Multiplications','3 Matxix Multiplications','Matrix Transpose and Vector Multiplication','Scalar Vector and Matrix Multiplication','General Matrix Multiply']

global_id_label.place(x=50,y=50)
platform_id_label.place(x=200,y=50)
unique_dag_id_label.place(x=330,y=50)
wcet_cpu_label.place(x=500,y=50)
wcet_gpu_label.place(x=650,y=50)
deadline_label.place(x=800,y=50)
input_arrival_time_label.place(x=950,y=50)
hyper_period_entry_label.place(x=1100,y=50)
input_task_period_label.place(x=1250,y=50)
freq_label.place(x=1390,y=50)


def inc_counter():
    global counter
    counter=counter+1
    button1.config(text='add')
    print(counter)
    global_id_entry=Label(text=str(counter-1),font='Verdana 10 bold')
    global_id_entry.place(x=50,y=50+60*(counter+1))                          
    a_dict['global_id'][counter-1]=counter-1
    platform_id_drop=ttk.Combobox(app,values=['cpu','gpu'])
    platform_id_drop.place(x=170,y=50+60*(counter+1))
    platform_id_drop.bind("<<ComboboxSelected>>",lambda event,platform_id_drop=platform_id_drop,counter=counter: selo(event,platform_id_drop,counter))
    a_dict['platform_id'][counter-1]=platform_id_drop
    unique_dag_id_entry=ttk.Combobox(app,values=kernel_options)
    unique_dag_id_entry.place(x=330,y=50+60*(counter+1)) 
    a_dict['unique_dag_id'][counter-1]=unique_dag_id_entry
    
    wcet_cpu_entry=Entry(app)
    wcet_cpu_entry.place(x=500,y=50+60*(counter+1)) 
    a_dict['wcet_cpu'][counter-1]=wcet_cpu_entry
    
    wcet_gpu_entry=Entry(app)
    wcet_gpu_entry.place(x=650,y=50+60*(counter+1))
    a_dict['wcet_gpu'][counter-1]=wcet_gpu_entry

    deadline_entry=Entry(app)
    deadline_entry.place(x=800,y=50+60*(counter+1))   
    a_dict['deadline'][counter-1]=(deadline_entry)

    input_arrival_time_entry=Entry(app)
    input_arrival_time_entry.place(x=950,y=50+60*(counter+1))  
    a_dict['input_arrival_time'][counter-1]=(input_arrival_time_entry)

    hyper_period_entry_entry=Entry(app)
    hyper_period_entry_entry.place(x=1100,y=50+60*(counter+1))  
    a_dict['hyper_period_entry'][counter-1]=(hyper_period_entry_entry)

    input_task_period_entry=Entry(app)
    input_task_period_entry.place(x=1250,y=50+60*(counter+1)) 
    a_dict['input_task_period'][counter-1]=( input_task_period_entry)
    
    
    
button1=Button(app,text="Add",font='Verdana 10 bold',command=inc_counter,width="10",height="1",bg="grey")
button1.place(x=800,y=0)
button=Button(app,text="Save",font='Verdana 10 bold',command=save_trace1,width="10",height="1",bg="grey")
button.place(x=600,y=0)
app.mainloop()