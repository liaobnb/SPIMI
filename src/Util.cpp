#include "../include/Util.h"

Util::Util()
{
    //ctor
}

Util::~Util()
{
    //dtor
}


//从给定目录下，将目录下的文件名读入到数组中
vector<string> Util::getFiles(string path) {
    
    // include in dirent ,the  unix library
    DIR *dp;
    struct dirent *dirp;

    //return a vector,which will use to open all of the file under the fold ,we put in the filepath
    vector<string> file_names;
    if((dp=opendir(path.c_str()))==NULL){
        perror("opendir error");
        exit(1);
    }

    //function readdir() used to check ,if the dp objetc is NULL
    //the dir tree will include
    //..
    //.
    //find that the dirp 
    while((dirp=readdir(dp))!=NULL){
        if((strcmp(dirp->d_name,".")==0)||(strcmp(dirp->d_name,"..")==0))
            continue;
        file_names.push_back(dirp->d_name);
    }
    return file_names;
}

//调用系统函数，创建shell子进程，执行字符串中的命令
void Util::delFile(string file) {
    file = "rm -f " + file;
    system(file.c_str());
}
