//licence GPL-2.0
/*
 * Entry of the whole project
 *
 *Copyright (C) 2018,2021  Liao Bnb
*/


#include <iostream>
#include "include/Spimi.h"
#include <fstream>
#include <vector>
#include "include/Indexer.h"
using namespace std;

int main()
{
    // main objetc of the whole function
    Spimi spimi;
    
    //the path of the dockment
    string path = "shakespeare-merchant.trec";
    int splitNum = 10;

    //command args ,init start
    spimi.start(path, splitNum, "dict", "index");

    cout<<"文档数量:"<<spimi.getDocNum()<<endl;
    cout<<"词条数量:"<<spimi.getTokenNum()<<endl;
    cout<<"文档平均长度:"<<spimi.getAveLen()<<endl;

    Indexer indexer;
    indexer.readFromFile("dict", "index");
    cout<<"词项数目:"<<indexer.getItemNum()<<endl;
    int df;
    cout<<"小写字母单词: 输出倒排索引\n例如输入balabala"<<endl;
    while(true) {
        string s;
        cin>>s;
        vector<int> v = indexer.find(s, df);
        cout<<"df:"<<df<<" docID:";
        for (int i = 0; i < v.size(); i++) {
            cout<<" "<<v[i];
        }
        cout<<endl;
    }
    return 0;
}
