# -*- coding: utf-8 -*-
"""
Created on Wed Jul  8 09:06:20 2020

@author: dell
"""
import os, shutil

def copyfile(src_file, dst_path):  #将文件拷贝至dst_path目录下
    if not os.path.isfile(src_file):
        print("%s not exist!"%(src_file))
    else:
        if not os.path.exists(dst_path):
            os.makedirs(dst_path)                #创建路径
        shutil.copy(src_file,dst_path)      #复制文件
        print("copy %s -> %s"%( src_file,dst_path))

def copyfiles(source_dir, target_dir):
    if not os.path.exists(source_dir) or not os.path.isdir(source_dir):
        print(f"{source_dir}' not exist!")

    if not os.path.exists(target_dir):
        os.makedirs(target_dir)
    
    for item in os.listdir(source_dir):
        source_path = os.path.join(source_dir, item)
        target_path = os.path.join(target_dir, item)

        if not os.path.isfile(source_path):
            print(f"skip: {source_path} (not file)")
            continue
        
        file_ext = os.path.splitext(item)[1].lower()
        if file_ext not in ('.lib', '.dll'):
            print(f"skip: {source_path} (not .lib or .dll file)")
            continue

        try:
            shutil.copy2(source_path, target_path)
            print(f"copy: {source_path} -> {target_path}")
        except Exception as e:
            print(f"copy {source_path} occur error: {e}")

def get_architecture_string(choice):
    """
    根据用户选择返回对应的架构字符串
    :param choice: 用户输入(1或2)
    :return: 架构字符串('x64'或'x86')
    """
    if choice == '1':
        return 'x64'
    elif choice == '2':
        return 'x86'
    else:
        raise ValueError("无效的输入，请输入1或2")



print("***********请选择当前架构**************:")
print("1. x64")
print("2. x86")

while True:
        choice = input("请输入您的选择(1或2): ").strip()
        try:
            arch = get_architecture_string(choice)
            break
        except ValueError as e:
            print(e)

last_path=os.path.abspath(os.path.dirname(os.getcwd()))  #上级目录


libirv4l2_header=last_path+'\\libir_SDK_release\include\\libirv4l2.h'
libiruvc_header=last_path+'\\libir_SDK_release\include\\libiruvc.h'
libiruart_header=last_path+'\\libir_SDK_release\include\\libiruart.h'
libirdfu_header=last_path+'\\libir_SDK_release\include\\libirdfu.h'
libiri2c_header=last_path+'\\libir_SDK_release\include\\libiri2c.h'
libirspi_header=last_path+'\\libir_SDK_release\include\\libirspi.h'
src_header_list1=[ libirv4l2_header,libiruvc_header, libiruart_header,libirdfu_header,libiri2c_header, libirspi_header]
dst_header_path1=os.getcwd()+'\drivers'

libircmd_header=last_path+'\\libir_SDK_release\include\\libircmd.h'
libircmd_temp_header=last_path+'\\libir_SDK_release\include\\libircmd_temp.h'
libircam_header=last_path+'\\libir_SDK_release\include\\libircam.h'
error_header=last_path+'\\libir_SDK_release\include\\error.h'
libirinfo_parse_header=last_path+'\\libir_SDK_release\include\\libir_infoparse.h'
src_header_list2=[libircmd_header, libircmd_temp_header, libircam_header,error_header,libirinfo_parse_header]
dst_header_path2=os.getcwd()+'\interfaces'

libirtemp_header=last_path+'\\libir_SDK_release\include\\libirtemp.h'
libirparse_header=last_path+'\\libir_SDK_release\include\\libirparse.h'
libirupgrade_header=last_path+'\\libir_SDK_release\include\\libirupgrade.h'
src_header_list3=[libirtemp_header,libirparse_header,libirupgrade_header]
dst_header_path3=os.getcwd()+'\other'

multi_point_lib_path=os.getcwd()+r'\sample\multi_point_calibration\msvc'
uart_cmd_lib_path=os.getcwd()+r'\sample\uart_cmd\msvc' 
upgrade_lib_path=os.getcwd()+r'\sample\upgrade\msvc'
usb_stream_lib_path=os.getcwd()+r'\sample\usb_stream_cmd\msvc'

opencv_dll_path=os.getcwd()+rf'\thirdparty\opencv2\libs\{arch}\dll'
opencv_lib_path=os.getcwd()+rf'\thirdparty\opencv2\libs\{arch}\lib'
pthread_dll_path=os.getcwd()+rf'\thirdparty\pthreads\libs\{arch}\dll'
pthread_lib_path=os.getcwd()+rf'\thirdparty\pthreads\libs\{arch}\lib'

#opencv_dll_path=os.getcwd()+r'\thirdparty\opencv2\libs\{arch}\dll\\opencv_world455.dll'
#opencv_lib_path=os.getcwd()+r'\thirdparty\opencv2\libs\{arch}\lib\\opencv_world455.lib'
#pthread_dll1_path=os.getcwd()+r'\thirdparty\pthreads\libs\{arch}\dll\\pthreadGC2.dll'
#pthread_dll2_path=os.getcwd()+r'\thirdparty\pthreads\libs\{arch}\dll\\pthreadVC2.dll'
#pthread_lib_path=os.getcwd()+r'\thirdparty\pthreads\libs\{arch}\lib\\pthreadVC2.lib'
#thirdparty_lib_list=[opencv_dll_path,opencv_lib_path,pthread_dll1_path,pthread_dll2_path,pthread_lib_path]


lib_path=last_path+f'\\libir_SDK_release\\windows\\{arch}\\Release\\dll'


for i in range(len(src_header_list1)):
    copyfile(src_header_list1[i],dst_header_path1)

for i in range(len(src_header_list2)):
    copyfile(src_header_list2[i],dst_header_path2)

for i in range(len(src_header_list3)):
    copyfile(src_header_list3[i],dst_header_path3)

# copy lib&dll

copyfiles(lib_path,multi_point_lib_path)
copyfiles(lib_path,usb_stream_lib_path)
copyfiles(lib_path,upgrade_lib_path)
copyfiles(lib_path,uart_cmd_lib_path)

copyfiles(opencv_dll_path,usb_stream_lib_path)
copyfiles(opencv_lib_path,usb_stream_lib_path)

copyfiles(pthread_dll_path,usb_stream_lib_path)
copyfiles(pthread_dll_path,uart_cmd_lib_path)
copyfiles(pthread_dll_path,upgrade_lib_path)

copyfiles(pthread_lib_path,usb_stream_lib_path)
copyfiles(pthread_lib_path,uart_cmd_lib_path)
copyfiles(pthread_dll_path,upgrade_lib_path)

input("copy completed!! Press any key to exit...")

