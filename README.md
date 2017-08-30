Basics includes
  1. A working pthread code without mutexes
    gcc <filename>.c -pthread(intel machines) | -lpthread (amd machines) -o <filenname>
  2. A working pthread code with mutexes
  3. A working code for mysql
      gcc <filename.c> 

Prototype_0:
  prerequisite: 
    sudo apt-get install mysql-server
    include <mysql/mysql.h>
    
    
  
  1.client code 
    gcc client.c
  2.authentication code 
  3.login.txt for db data LOAD
  
# CS744PolingSys
Multi Tier system  #######
sudo apt install libmysqlclient-dev                                      ### installation for linkin libraries with c ######
sudo apt-get install mysql-server                                       ### server msot important ####
mysql_config --cflags                                                   ### for including at the time of compiling (libraries)####
mysql_config --libs                                                     ### for linking at the time of compiling ####
