#tinfo

from tkinter import *

import pandas as pd
from tkinter import filedialog
from tkinter import ttk
from tkinter.filedialog import asksaveasfile
import os.path
from ttkthemes import ThemedStyle
import collections 

def save_info():
    global no_of_tasks
    no_of_tasks=tasks.get()
    app.destroy()
    
app=Tk()
app.title("no_of_tasks ")
app.maxsize(width=700 ,  height=500)
app.minsize(width=700 ,  height=500)

#app.geometry("1000x500")
#app['background']='#856ff8'
style=ttk.Style()
style.configure("BW.TLabel", foreground="black", background="white")
Def_Scrollbar = Scrollbar(app)
Def_Scrollbar.pack(side='right',fill='y')

tasks=IntVar()

heading=Label(app,text="Taking Input for Task Scheduling",font='Verdana 25 bold')
heading.place(x=60,y=150)

label_tasks=ttk.Label(text="Enter No. of tasks",font='Verdana 10 bold')
label_tasks.place(x=80,y=220)
a=Entry(app,width=40,textvariable=tasks)

a.place(x=220,y=223)

button=Button(app,text="get",font='Verdana 10 bold',command=save_info)
button.place(x=200, y=293)
#button.pack(pady=20,padx=30)

app.mainloop()

global counter
counter=0
global kernelname
global text_file
global complete_name_1


while(counter<int(no_of_tasks)):
    save_path = str('.')
    root = Tk()
    root.title("input for kernal"+ str(counter))
    root.geometry("1000x1000")
    Def_Scrollbar = Scrollbar(root)
    Def_Scrollbar.pack(side='right',fill='y')
#     style = ThemedStyle(app)
#     style.set_theme("scidgrey")
    #root['background']='#856ff8'
    
    print("kernel")
    kernelname=StringVar()
    def export_source_file():
        global complete_name_1
        save_path1 = str('E:/gem5/framework_structure/new_folder/')
        global kernelname
        
        
        
        folder_name=("DAG_"+str(counter))
        folder=os.path.join(save_path1,folder_name)
        name_of_file1=("edlenet_"+str(counter)+"_C_"+str(counter))
        
        complete_name_1=os.path.join(folder,name_of_file1 + ".txt")
        print(complete_name_1)
        os.makedirs(folder)
        
        with open(complete_name_1, "w") as output_file:
            text1=source_file.get(1.0,"end-1c")
            output_file.write(text1)
        
        
        ##root.destroy()
    

    kernel_type_options=[
        '2D Convolution','2 Matrix Multiplications','3 Matxix Multiplications','Matrix Transpose and Vector Multiplication','Scalar Vector and Matrix Multiplication','General Matrix Multiply']
    kernelname_drop=ttk.Combobox(root,values=kernel_type_options,width=30)
    kernelname_drop.pack(pady=30)

    label1=Label(root)
    label1.pack()

    def clear_search(event):
        source_file.delete(1.0,END)
    
    global source_file
    Label(text="source_file",font='Verdana 10 bold').place(x=15,y=30)
    source_file=Text(root,height=15,width=30)
    source_file.insert(1.0,"Enter the source file")
    source_file.pack()
    source_file.bind("<Button-1>",clear_search)
    source_file.place(x=15,y=50)
    btn_Save=Button(root,text="Save_source_file",font='Verdana 10 bold',command=export_source_file)
    btn_Save.pack()
    btn_Save.place(x=15,y=310)
    globalWorkSize=IntVar()
    inputBuffers=StringVar()
    inputBuffers2=StringVar()
    inputBuffers3=StringVar()
    outputBuffers=StringVar()
    ##flopsperthread=StringVar()
    no_input_buffers=IntVar()
    no_output_buffers=IntVar()
    global text_file
    def show_info1():
        global complete_file_name2
        
        global text_file
        global kernelname
        kernelname=kernelname_drop.get()
        print(kernelname)
        kernelname_info=kernelname
        globalWorkSize_info=globalWorkSize.get()
        work_dim=workdimension_drop.get()
        
        global save_path2
        global name_of_file2
        
#         global text_file
        save_path2 = str('.')
        folder_name=("DAG_"+str(counter))
        folder=os.path.join(save_path2,folder_name)
        name_of_file2=("node_"str(counter)+"_"+str(counter))
        
        print(kernelname_info,work_dim,globalWorkSize_info)
        complete_file_name2=os.path.join(folder,name_of_file2)
        os.makedirs(folder)
        text_file=open(complete_file_name2,'w')
        text_file.write("KernelName=" + kernelname_info)
        text_file.write("\n")
        
        global complete_name_1
        kernal_source=complete_name_1
        text_file.write("KernelSource="+ str(complete_name_1))
        text_file.write("\n")
        
        text_file.write("workDimension=" + str(work_dim))
        text_file.write("\n")
        text_file.write("globalWorkSize="+ str(globalWorkSize_info))
        text_file.write("\n")

        #text_file.close()
    
    work_dim_options= [
        1,2,3,
    ]

    data_type_options=[
        'int', 'float','string','complex','str','bool',
    ]
    def callback2():
        l3.configure(text=workdimension_drop.get())

    workdimension_drop=ttk.Combobox(root,values=work_dim_options,width=30)
    workdimension_drop.pack(pady=20)
    l3=Label(root,text="")
    l3.pack()
    style= ttk.Style()
    style.theme_use('clam')
    style.configure("TCombobox", fieldbackground= "grey", background= "white")

    workdimension_text=Label(text="workDimension ",font='Verdana 10 bold')
    globalWorkSize_text=Label(text= "globalWorkSize ",font='Verdana 10 bold')
    no_input_buffers_text=Label(text="no_input_buffers",font='Verdana 10 bold')
    inputBuffers_text=Label(text = 'no. of inputBuffers',font='Verdana 10 bold')
    outputBuffers_text=Label(text = 'no. of outputBuffers',font='Verdana 10 bold')
    globalWorkSize_entry=Entry(textvariable=globalWorkSize, width=20,font='Verdana 10 bold')
    kernelname_text= Label(text="KernelName",font='Verdana 10 bold')
    kernelname_text.place(x=15,y=370)
    kernelname_drop.place(x=170,y=370)
    workdimension_text.place(x=15,y=430)
    workdimension_drop.place(x=170, y=430)
    globalWorkSize_text.place(x=15, y=500)
    globalWorkSize_entry.place(x=170,y=500)
    
    button_tinfo = Button(root,text="Save Data",font='Verdana 10 bold',command=show_info1)

    button_tinfo.place(x=15,y=570 )


    btn_exit=Button(root,text="exit",command=root.destroy,width="20",height="2",bg="grey")
    btn_exit.pack()
    btn_exit.place(x=790,y=750 )   
    

# create one set of widgets in a frame
    def show_input():
        global text_file
        master = Tk()
        master.withdraw()
        master.geometry("5x5")
        my_frame = Frame()
        my_frame.place(x=540,y=200 )
        tasks=StringVar
        myentries=[]
        #my_entry_label=Label(tkin=my_frame,text='Enter input buffer')
        #my_entry_label.place(x=400,y=450)
        
        for num in range(1, 7):
            my_label=Label(master=my_frame)
            my_label.place(x=700,y=350+40*num)
            
            f=Entry(master=my_frame,textvariable=tasks,width=30)
            f.config({"background": "ivory3"})
            f.place(x=800,y=400+40*num)
            myentries.append(f)
        
        def detect_selection(eventobject):
            the_selection = eventobject.widget.get()
    
            for item in my_frame.winfo_children():
                item.grid_remove()
            show_idx__wigets = 2
            if the_selection == '1':
                change_labeltext(('Input_Buffer-1'), 2)
            if the_selection == '2':
                show_idx__wigets = 4
                change_labeltext(('Input_Buffer-1', 'Input_Buffer-2'), 4)
            if the_selection == '3':
                show_idx__wigets=6
                change_labeltext(('Input_Buffer-1', 'Input_Buffer-2', 'Input_Buffer-3'), 6)
            if the_selection == '4':
                show_idx__wigets = 8
                change_labeltext(('Input_Buffer-1', 'Input_Buffer-2', 'Input_Buffer-3', 'Input_Buffer-4'), 8)
            if the_selection == '5':
                show_idx__wigets = 10
                change_labeltext(('Input_Buffer-1', 'Input_Buffer-2', 'Input_Buffer-3', 'Input_Buffer-4', 'Input_Buffer-5'), 10)
            if the_selection == '6':
                show_idx__wigets = 10
                change_labeltext(('Input_Buffer-1', 'Input_Buffer-2', 'Input_Buffer-3', 'Input_Buffer-4', 'Input_Buffer-5','Input_Buffer-6'), 12)

            show_widgets(show_idx__wigets)
        
        def show_info():
            text_file.write("input buffers=")
            ##my_entries=list(filter(None,myentries))
            #my_entries=(list(filter(lambda x: x, myentries)))
            for entries in myentries :
                print(entries.get()+'\n')
                if(entries.get()):
                    text_file.write(entries.get())
                    text_file.write(",")
                
            text_file.write("\n")
           # text_file.close() 
            master.deiconify()
            master.destroy()
                    
        def show_widgets(upto_widgetposition):
            Label(text='Enter datatype,size,position,persistance seperated by comma',font='Verdana 10 bold').place(x=420,y=160)
            for item in my_frame.winfo_children()[:upto_widgetposition]:
                item.grid()
        
        def change_labeltext(labeltext, upto_widgetposition):
            for txt, item in zip(
                labeltext, my_frame.winfo_children()[:upto_widgetposition:2]):
                item['text'] = txt  
        ##item_get['text']=item['text'].get()
        #print(item['text'])
        label_input=Label(text='enter no. of input buffers',font='Verdana 10 bold')
        label_input.place(x=420,y=120 )
        button=Button(root,text='save_input_buffer',font='Verdana 10 bold',command=show_info)
        button.place(x=800,y=120 )
        combo = ttk.Combobox(root,
                     values=(1,2,3,4,5))
        combo.place(x=630,y=120)
        combo.bind('<<ComboboxSelected>>', detect_selection)
        style= ttk.Style()
        style.theme_use('clam')
        style.configure("TCombobox", fieldbackground= "grey", background= "white")
        p=show_info
        
    
    def show_output():
        global text_file
        master=Tk()
        master.withdraw()
        master.geometry("5x5")
        new_frame=Frame()
        new_frame.place(x=540,y=580)
        modi=StringVar
        new_entries=[]
        for num in range(1,6):
            new_label=Label(master=new_frame)
            new_label.place(x=900,y=390+40*num)
            e=Entry(master=new_frame,width=30)
            e.config({"background": "ivory3"})
            e.place(x=950,y=440+40*num)
            new_entries.append(e)
            
        #def click_inp1(*args):
         #   e.delete(0, 'end')
        def new_detect_selection(eventobject):
            new_selection = eventobject.widget.get()
            for item in new_frame.winfo_children():
                item.grid_remove() 
            new_idx__wigets = 2
            if new_selection == '1':
                new_change_labeltext(('Output_Buffer-1'), 2)
            if new_selection == '2':
                new_idx__wigets = 4
                new_change_labeltext(('Output_Buffer-1', 'Output_Buffer-2'), 4)
            if new_selection == '3':
                new_idx__wigets = 6
                new_change_labeltext(('Output_Buffer-1', 'Output_Buffer-2', 'Output_Buffer-3'), 6)
            if new_selection == '4':
                new_idx__wigets = 8
                new_change_labeltext(('Output_Buffer-1', 'Output_Buffer-2', 'Output_Buffer-3', 'Output_Buffer-4'), 8)
            if new_selection == '5':
                new_idx__wigets = 10
                new_change_labeltext(('Output_Buffer-1', 'Output_Buffer-2', 'Output_Buffer-3', 'Output_Buffer-4','Output_Buffer-5'), 10)
            if new_selection == '6':
                new_idx__wigets = 12
                new_change_labeltext(('Output_Buffer-1', 'Output_Buffer-2', 'Output_Buffer-3', 'Output_Buffer-4','Output_Buffer-5','Output_Buffer-6'), 12)
            new_show_widgets(new_idx__wigets)
        
        def new_show_info():
            global text_file
            ##newentries=list(filter(None,new_entries))
            #newentries=(list(filter(lambda x: x, new_entries)))
            text_file.write("output buffers=")
            for entries in new_entries:
                print(entries.get()+'\n')
                if(entries.get()):
                    text_file.write(entries.get())
                    text_file.write(",")
            text_file.write("\n")
            text_file.close()
            master.deiconify()
            master.destroy()
            
        def new_show_widgets(upto_widgetposition):
            Label(text='Enter datatype,size,position,persistance seperated by comma',font='Verdana 10 bold').place(x=420,y=540)
            
            for item in new_frame.winfo_children()[:upto_widgetposition]:
                item.grid()
        def new_change_labeltext(labeltext, upto_widgetposition):
            for txt, item in zip(
                labeltext, new_frame.winfo_children()[:upto_widgetposition:2]):
                item['text'] = txt 
                
        button=Button(root,text='save_outBuf',font='Verdana 10 bold',command=new_show_info)
        button.place(x=800,y=500 )
        
        label_out=Label(text="enter no. of output buffers",font='Verdana 10 bold')
        label_out.place(x=420,y=500 )
        
        combo = ttk.Combobox(root,
                     values=(1,2,3,4,5))
        combo.place(x=630,y=500)
        combo.bind('<<ComboboxSelected>>', new_detect_selection)
        style= ttk.Style()
        style.theme_use('clam')
        style.configure("TCombobox", fieldbackground= "grey", background= "white")

        
    button=Button(root,text='For input Buffer ',font='Verdana 10 bold',command=show_input)
    button.place(x=400,y=50 )
    button3=Button(root,text='For output buffer ',font='Verdana 10 bold',command=show_output)
    button3.place(x=400,y=430 )
    counter=counter+1;
    root.mainloop()


    
    
