#include "../include/Spimi.h"

Spimi::Spimi()
{
    //ctor
}

Spimi::~Spimi()
{
    //dtor
}
int Spimi::getItemNum() {
    return itemNum;
}
int Spimi::getDocNum() {
    return docID;
}
int Spimi::getTokenNum() {
    return tokenNum;
}
double Spimi::getAveLen() {
    return tokenNum * 1.0 / docID;
}

//将去除重复的词项以及，存储她们的docID存入到词典中
void Spimi::updateDict(set<string> &s) {
    //cout<<"updateDict"<<endl;
    set<string>::iterator it = s.begin();
    for (; it != s.end(); it++) {
        dict.insert(*it, docID);
    }
}

string Spimi::trim(string& str) {
    int st;

    //去除文档中前缀
    if ((st = str.find("<title>")) != -1)  {
        str = str.substr(st + 7);
    }
    if ((st = str.find("<speaker>")) != -1)  {
        str = str.substr(st + 9);
    }
    if ((st = str.find("</title>")) != -1)  {
        str = str.substr(0, st);
    }
    if ((st = str.find("</speaker>")) != -1)  {
        str = str.substr(0, st);
    }
    int i = 0, j = str.size() - 1, t;

    //去除数字与下引号
    while(!isalpha(str[i]) && i <= j) {
        i++;
    }
    while(!isalpha(str[j]) && i <= j) {
        j--;
    }
    t = j;
    while(str[t] != '\'' && t >= 0) {
        t--;
    }
    if (t >= 0) j = t - 1;
    str = str.substr(i, j - i + 1);
    j = str.size();
    for (i = 0; i < j; i++)
        str[i] = tolower(str[i]);
    return str;
}


//每篇文档的词条存入到set中，到达文档结尾的时候集中处理
void Spimi::processDoc() {
    set<string> s;
    string str;
    while(in>>str) {
        if (str.find("</DOC>") != -1) {
            updateDict(s);
            return;
        }
        if (str.find("<!--") != -1) {
            while(in>>str) {
                if (str.find("</DOC>") != -1) {
                    break;
                }
            }
            updateDict(s);
            return;
        }
        str = trim(str);
        if (str == "") continue;
        tokenNum++;
        s.insert(str);
    }
}

//防止文档过于大，从而给定一个splitNum，指定多少个文档来存入到文件
void Spimi::processFile() {
    string str;
    while(in>>str) {
        if (str.find("<DOC>") != -1) {
            docID++;//当每次出现<DOCID>意味着文档的开头这时候，记录DOCID
            in>>str;
            cout<<str<<endl;
            processDoc();
        } else {
            cout<<"发生错误:"<<docID<<endl;
        }
        //printf("%d\n", docID);
        //cout<<docID<<endl;
        if (docID % splitNum == 0) {
            char name[100];
            sprintf(name, "./tmp/block%d", docID / splitNum);
            dict.writeToFile(name);
            dict.reset();
        }
    }
}




void Spimi::start(string path, int splitNum, string dictFile, string indexFile) {
    //dict objetc reset
    dict.reset();
    this->splitNum = splitNum;
    vector<string> v = util.getFiles(path);
    

    int size_p = v.size();
    docID = 0;
    tokenNum = 0;
    for (int i = 0; i < size_p ; i++) {
        in.open((path + "/" + v[i]).c_str(),ios::in);
        cout<<"processFile: "<<path + "/" + v[i]<<endl;
        processFile();
        cout<<"processFile Done"<<endl;
        in.close();
    }

    //cout<< (docID % splitNum) <<endl;
    //剩下的文档存入最后的文件中
    if (docID % splitNum) {
        cout<<"do here"<<endl;
        char name[100];
        sprintf(name, "./tmp/block%d", docID / splitNum + 1);
        dict.writeToFile(name);
    }

    string name = merge();
    cout<<name<<endl;
    generateDictIndex(name, dictFile, indexFile);
    util.delFile(name);
}

//将每splitNum个文档生成的中间所有表merge
string Spimi::merge() {
    return merger.merge("./tmp");
}

void Spimi::generateDictIndex(string file, string dictFile, string indexFile) {
    in.open(file.c_str(), ios::binary|ios::in);
    ofstream out2(indexFile.c_str(), ios::binary|ios::out);
    map<string, pair<int, int> > mp;
    char s[100], c;
    int offset = 0, len, t, df;
    while(!in.eof()) {
        c = -1;
        in.read(&c, sizeof(char));
        if (c == -1) break;
        in.read(s, c);
        s[c] = 0;
        in.read((char *)&df, sizeof(df));
        in.read((char *)&len, sizeof(int));
        //记录文档频率和词条在文件起始的字节位置
        mp[s] = make_pair<int, int>(df, offset);
        t = (len * 4) + 1 + 4;
        offset += t;
        char *buf = new char[t];
        in.read(buf, sizeof(char) * t);
        out2.write(buf, sizeof(char) * t);
        delete[] buf;
    }
    cd.generateDict(mp, offset);
    cd.writeToFile(dictFile);
    in.close();
    out2.close();
}


