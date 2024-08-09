#include<bits/stdc++.h>
using namespace std;
struct SICXE_input{
    vector<string> instruction;
    vector<string> objcode;
    vector<string> M_record;
    vector<vector<string>> token;
    vector<int> loc;
    string base;
};
struct instruction_map{
    int opcode;
    int format;
};
map<string,instruction_map> OPTAB;
bool is_something(char a){
    if((a>='0' && a<='9') || (a>='A' && a<='Z') || (a>='a' && a<='z') || a=='+' || a=='#' || a=='@') return 1;
    return 0;
}
void read_file(SICXE_input &sicxe_input){
    ifstream in;
    in.open("SICXEinput.txt");
    string s;
    while(getline(in,s)){
        if(s[0]=='.') continue;
        stringstream s1(s);
        vector<string> tok;
        string tmp;
        while(getline(s1,tmp,'\t')){
            tok.push_back(tmp);
        }
        if(tok[1]=="BASE"){
            sicxe_input.base=tok[2];
            continue;
        }
        sicxe_input.instruction.push_back(s);
        sicxe_input.token.push_back(tok);
    }
    in.close();
}
map<string,instruction_map> build_sicxe_OPTAB(){
    map<std::string, instruction_map> mp = {
        {"ADD", {0x18, 3}},
        {"ADDF", {0x58, 3}},
        {"ADDR", {0x90, 2}},
        {"AND", {0x40, 3}},
        {"CLEAR", {0xB4, 2}},
        {"COMP", {0x28, 3}},
        {"COMPF", {0x88, 3}},
        {"COMPR", {0xA0, 2}},
        {"DIV", {0x24, 3}},
        {"DIVF", {0x64, 3}},
        {"DIVR", {0x9C, 2}},
        {"FIX", {0xC4, 1}},
        {"FLOAT", {0xC0, 1}},
        {"HIO", {0xF4, 1}},
        {"J", {0x3C, 3}},
        {"JEQ", {0x30, 3}},
        {"JGT", {0x34, 3}},
        {"JLT", {0x38, 3}},
        {"JSUB", {0x48, 3}},
        {"LDA", {0x00, 3}},
        {"LDB", {0x68, 3}},
        {"LDCH", {0x50, 3}},
        {"LDF", {0x70, 3}},
        {"LDL", {0x08, 3}},
        {"LDS", {0x6C, 3}},
        {"LDT", {0x74, 3}},
        {"LDX", {0x04, 3}},
        {"LPS", {0xD0, 3}},
        {"MUL", {0x20, 3}},
        {"MULF", {0x60, 3}},
        {"MULR", {0x98, 2}},
        {"NORM", {0xC8, 1}},
        {"OR", {0x44, 3}},
        {"RD", {0xD8, 3}},
        {"RMO", {0xAC, 2}},
        {"RSUB", {0x4C, 3}},
        {"SHIFTL", {0xA4, 2}},
        {"SHIFTR", {0xA8, 2}},
        {"SIO", {0xF0, 1}},
        {"SSK", {0xEC, 3}},
        {"STA", {0x0C, 3}},
        {"STB", {0x78, 3}},
        {"STCH", {0x54, 3}},
        {"STF", {0x80, 3}},
        {"STI", {0xD4, 3}},
        {"STL", {0x14, 3}},
        {"STS", {0x7C, 3}},
        {"STSW", {0xE8, 3}},
        {"STT", {0x84, 3}},
        {"STX", {0x10, 3}},
        {"SUB", {0x1C, 3}},
        {"SUBF", {0x5C, 3}},
        {"SUBR", {0x94, 2}},
        {"SVC", {0xB0, 2}},
        {"TD", {0xE0, 3}},
        {"TIO", {0xF8, 1}},
        {"TIX", {0x2C, 3}},
        {"TIXR", {0xB8, 2}},
        {"WD", {0xDC, 3}}
    };
    return mp;
}
bool in_optab(string instruction){
    auto it=OPTAB.find(instruction);
    if(it!=OPTAB.end()) return 1;
    return 0;
}
bool is_fake(string instruction){
    if(instruction=="START" || instruction=="END" || instruction=="BYTE" || instruction=="WORD" || instruction=="RESB" || instruction=="RESW" || instruction=="BASE") return 1;
    return 0;
}
bool is_re(string instruction){
    if(instruction=="RESW" || instruction=="RESB") return 1;
    return 0;
}
bool is_digit(char c){
    if(c>='0' && c<='9') return 1;
    return 0;
}
bool have_x(string content){
    for(int i=0;i<content.length();i++){
        if(content[i]==',') return 1;
    }
    return 0;
}
string confirm_M(int loc){
    stringstream ss;
    ss<<setw(6)<<setfill('0')<<uppercase<<hex<<loc+1;
    return "M"+ss.str()+"05";
}
map<string,int> pass_1(SICXE_input &sicxe_input){
    map<string,int> symtab;
    int loc=stoi(sicxe_input.token[0][2]);
    for(int i=1;i<sicxe_input.instruction.size();i++){
        sicxe_input.loc.push_back(loc);
        if(is_something(sicxe_input.token[i][0][0])){
            auto it=symtab.find(sicxe_input.token[i][0]);
            if(it!=symtab.end()){
                cout<<"your code have same label\n";
                exit(0);
            }
            symtab[sicxe_input.token[i][0]]=loc;
        }
        string operand=sicxe_input.token[i][1];
        string content;
        if(sicxe_input.token[i].size()>=3) content=sicxe_input.token[i][2];
        int flag=operand[0]=='+'?1:0;
        if(operand[0]=='+') operand=operand.substr(1,operand.length()-1);
        if(in_optab(operand) && !flag) loc+=OPTAB[operand].format;
        else if(in_optab(operand) && flag) loc+=4;
        else if(operand=="WORD") loc+=3;
        else if(operand=="RESW") loc+=stoi(content)*3;
        else if(operand=="RESB") loc+=stoi(content);
        else if(operand=="BYTE") loc+=content[0]=='X'?(content.length()-3)/2:content.length()-3;
        else if(!in_optab(operand) && !is_fake(operand)){
            cout<<"your operand is wrong\n";
            exit(0);
        }
    }
    return symtab;
}
void pass_2(SICXE_input &sicxe_input,map<string,int> SYMTAB){
    for(int i=1;i<sicxe_input.instruction.size()-1;i++){
        if(sicxe_input.token[i][1]=="BASE") continue;
        string operand=sicxe_input.token[i][1];
        string content;
        int e=(operand[0]=='+');
        operand=e?operand.substr(1,operand.length()-1):operand;
        if(sicxe_input.token[i].size()>=3) content=sicxe_input.token[i][2];
        if(in_optab(operand)){
            if(operand=="RSUB"){
                sicxe_input.objcode.push_back("4F0000");
            }
            else if(OPTAB[operand].format==3){
                if(content[0]=='@'){
                    int tmp=OPTAB[operand].opcode+2;
                    if(is_digit(content[1])){
                        int tmp1=stoi(content.substr(1,content.length()-1));
                        stringstream ss,ss1;
                        ss<<setw(2)<<setfill('0')<<uppercase<<hex<<tmp;
                        if(!e) ss1<<setw(4)<<setfill('0')<<uppercase<<hex<<tmp1;
                        else ss1<<1<<setw(5)<<setfill('0')<<uppercase<<hex<<tmp1;
                        string tmp2=ss.str()+ss1.str();
                        sicxe_input.objcode.push_back(tmp2);
                    }
                    else{
                        int x,b,p;
                        x=0;
                        int tmp1=SYMTAB[content.substr(1,content.length()-1)];
                        if(e==1){
                            sicxe_input.M_record.push_back(confirm_M(sicxe_input.loc[i-1]));
                            b=p=0;
                            int num=8*x+4*b+2*p+e;
                            stringstream ss,ss1,ss2;
                            ss<<setw(2)<<setfill('0')<<uppercase<<hex<<tmp;
                            ss1<<uppercase<<hex<<num;
                            ss2<<setw(5)<<setfill('0')<<uppercase<<hex<<tmp1;
                            string tmp2=ss.str()+ss1.str()+ss2.str();
                            sicxe_input.objcode.push_back(tmp2);
                        }
                        else{
                            if(tmp1-sicxe_input.loc[i]>=-2048 && tmp1-sicxe_input.loc[i]<=2047) b=0,p=1;
                            else b=1,p=0;
                            int tmp2;
                            if(p){
                                if(tmp1-sicxe_input.loc[i]>=0) tmp2=tmp1-sicxe_input.loc[i];
                                else tmp2=(~(-tmp1+sicxe_input.loc[i])+1);
                            }
                            else{
                                tmp2=tmp1-SYMTAB[sicxe_input.base];
                            }
                            int num=8*x+4*b+2*p+e;
                            stringstream ss,ss1,ss2;
                            ss<<setw(2)<<setfill('0')<<uppercase<<hex<<tmp;
                            ss1<<uppercase<<hex<<num;
                            ss2<<setw(3)<<setfill('0')<<uppercase<<hex<<tmp2;
                            string tmp3=ss.str()+ss1.str()+ss2.str().substr(ss2.str().length()-3);
                            sicxe_input.objcode.push_back(tmp3);
                        }
                    }
                }
                else if(content[0]=='#'){
                    int tmp=OPTAB[operand].opcode+1;
                    if(is_digit(content[1])){
                        int tmp1=stoi(content.substr(1,content.length()-1));
                        stringstream ss,ss1;
                        ss<<setw(2)<<setfill('0')<<uppercase<<hex<<tmp;
                        if(!e) ss1<<setw(4)<<setfill('0')<<uppercase<<hex<<tmp1;
                        else ss1<<1<<setw(5)<<setfill('0')<<uppercase<<hex<<tmp1;
                        string tmp2=ss.str()+ss1.str();
                        sicxe_input.objcode.push_back(tmp2);
                    }
                    else{
                        int x,b,p;
                        x=0;
                        int tmp1=SYMTAB[content.substr(1,content.length()-1)];
                        if(e==1){
                            sicxe_input.M_record.push_back(confirm_M(sicxe_input.loc[i-1]));
                            b=p=0;
                            int num=8*x+4*b+2*p+e;
                            stringstream ss,ss1,ss2;
                            ss<<setw(2)<<setfill('0')<<uppercase<<hex<<tmp;
                            ss1<<uppercase<<hex<<num;
                            ss2<<setw(5)<<setfill('0')<<uppercase<<hex<<tmp1;
                            string tmp2=ss.str()+ss1.str()+ss2.str();
                            sicxe_input.objcode.push_back(tmp2);
                        }
                        else{
                            if(tmp1-sicxe_input.loc[i]>=-2048 && tmp1-sicxe_input.loc[i]<=2047) b=0,p=1;
                            else b=1,p=0;
                            int tmp2;
                            if(p){
                                if(tmp1-sicxe_input.loc[i]>=0) tmp2=tmp1-sicxe_input.loc[i];
                                else tmp2=(~(-tmp1+sicxe_input.loc[i])+1);
                            }
                            else{
                                tmp2=tmp1-SYMTAB[sicxe_input.base];
                            }
                            int num=8*x+4*b+2*p+e;
                            stringstream ss,ss1,ss2;
                            ss<<setw(2)<<setfill('0')<<uppercase<<hex<<tmp;
                            ss1<<uppercase<<hex<<num;
                            ss2<<setw(3)<<setfill('0')<<uppercase<<hex<<tmp2;
                            string tmp3=ss.str()+ss1.str()+ss2.str().substr(ss2.str().length()-3);
                            sicxe_input.objcode.push_back(tmp3);
                        }
                    }
                }
                else{
                    int tmp=OPTAB[operand].opcode+3;
                    if(is_digit(content[0])){
                        int x=have_x(content);
                        int tmp1=stoi(x?content.substr(0,content.length()-2):content);
                        stringstream ss,ss1,ss2;
                        ss<<setw(2)<<setfill('0')<<uppercase<<hex<<tmp;
                        if(x) ss1<<8;
                        ss2<<setw(3)<<setfill('0')<<uppercase<<hex<<tmp1;
                        string tmp2=ss.str()+ss1.str();
                        sicxe_input.objcode.push_back(tmp2);
                    }
                    else{
                        int x,b,p;
                        x=have_x(content);
                        int tmp1=SYMTAB[x?content.substr(0,content.length()-2):content];
                        if(e==1){
                            sicxe_input.M_record.push_back(confirm_M(sicxe_input.loc[i-1]));
                            b=p=0;
                            int num=8*x+4*b+2*p+e;
                            stringstream ss,ss1,ss2;
                            ss<<setw(2)<<setfill('0')<<uppercase<<hex<<tmp;
                            ss1<<uppercase<<hex<<num;
                            ss2<<setw(5)<<setfill('0')<<uppercase<<hex<<tmp1;
                            string tmp2=ss.str()+ss1.str()+ss2.str();
                            sicxe_input.objcode.push_back(tmp2);
                        }
                        else{
                            if(tmp1-sicxe_input.loc[i]>=-2048 && tmp1-sicxe_input.loc[i]<=2047) b=0,p=1;
                            else b=1,p=0;
                            int tmp2;
                            if(p){
                                if(tmp1-sicxe_input.loc[i]>=0) tmp2=tmp1-sicxe_input.loc[i];
                                else tmp2=(~(-tmp1+sicxe_input.loc[i])+1);
                            }
                            else{
                                tmp2=tmp1-SYMTAB[sicxe_input.base];
                            }
                            int num=8*x+4*b+2*p+e;
                            stringstream ss,ss1,ss2;
                            ss<<setw(2)<<setfill('0')<<uppercase<<hex<<tmp;
                            ss1<<uppercase<<hex<<num;
                            ss2<<setw(3)<<setfill('0')<<uppercase<<hex<<tmp2;
                            string tmp3=ss.str()+ss1.str()+ss2.str().substr(ss2.str().length()-3);
                            sicxe_input.objcode.push_back(tmp3);
                        }
                    }
                }
            }
            else if(OPTAB[operand].format==2){
                map<string,int> registers={
                    {"A",0},
                    {"X",1},
                    {"L",2},
                    {"PC",8},
                    {"SW",9},
                    {"B",3},
                    {"S",4},
                    {"T",5},
                    {"F",6}
                };
                stringstream ss,ss1,ss2;
                int tmp=OPTAB[operand].opcode;
                ss<<setw(2)<<setfill('0')<<uppercase<<hex<<tmp;
                for(int j=0;j<content.length();j++){
                    if(content[j]==','){
                        ss1<<registers[content.substr(0,j)];
                        ss2<<registers[content.substr(j+1,content.length()-1-j)];
                        break;
                    }
                    if(j==content.length()-1){
                        ss1<<registers[content];
                        ss2<<0;
                    }
                }
                string tmp1=ss.str()+ss1.str()+ss2.str();
                sicxe_input.objcode.push_back(tmp1);
            }
            else{
                int tmp=OPTAB[operand].opcode;
                stringstream ss;
                ss<<setw(2)<<setfill('0')<<uppercase<<hex<<tmp;
                sicxe_input.objcode.push_back(ss.str());
            }
        }
        else if(operand=="BYTE"){
            if(content[0]=='X'){
                string tmp=content.substr(2,content.length()-3);
                sicxe_input.objcode.push_back(tmp);
            }
            else{
                string tmp="";
                for(int j=0;j<content.length()-3;j++){
                    stringstream ss;
                    int ascii=content[j+2];
                    ss<<uppercase<<hex<<ascii;
                    string tmp1=ss.str();
                    tmp=tmp+tmp1;
                }
                sicxe_input.objcode.push_back(tmp);
            }
        }
        else if(is_re(operand)){
            sicxe_input.objcode.push_back("------");
        }
    }
}
void write_file(SICXE_input sicxe_input){
    ofstream out;
    out.open("output.txt");
    if(!out){
        cout<<"can't open file";
        exit(0);
    }
    out<<'H'<<left<<setw(6)<<sicxe_input.token[0][0]<<right<<setw(6)<<setfill('0')<<uppercase<<hex<<sicxe_input.token[0][2]<<setw(6)<<setfill('0')<<uppercase<<hex<<sicxe_input.loc.back()<<'\n';
    string s="";
    int start_addr=sicxe_input.loc[0],f=0;
    for(int i=0;i<sicxe_input.objcode.size();i++){
        if(sicxe_input.objcode[i]=="------" && !f){
            f=1;
            stringstream ss;
            ss<<setw(6)<<setfill('0')<<uppercase<<hex<<start_addr<<setw(2)<<setfill('0')<<uppercase<<hex<<sicxe_input.loc[i]-start_addr;
            out<<'T'<<ss.str()<<s<<'\n';
            s="";
        }
        else if(sicxe_input.objcode[i]!="------"){
            if(f) start_addr=sicxe_input.loc[i];
            f=0;
            if(s.length()+sicxe_input.objcode[i].length()<=60){
                s=s+sicxe_input.objcode[i];
            }
            else{
                stringstream ss;
                ss<<setw(6)<<setfill('0')<<uppercase<<hex<<start_addr<<setw(2)<<setfill('0')<<uppercase<<hex<<sicxe_input.loc[i]-start_addr;
                out<<'T'<<ss.str()<<s<<'\n';
                s=sicxe_input.objcode[i];
                start_addr=sicxe_input.loc[i];
            }
        }
    }
    if(s!=""){
        stringstream ss;
        ss<<setw(6)<<setfill('0')<<uppercase<<hex<<start_addr<<setw(2)<<setfill('0')<<uppercase<<hex<<sicxe_input.loc.back()-start_addr;
        out<<'T'<<ss.str()<<s<<'\n';
    }
    for(int i=0;i<sicxe_input.M_record.size();i++){
        out<<sicxe_input.M_record[i]<<'\n';
    }
    out<<'E'<<setw(6)<<setfill('0')<<uppercase<<hex<<sicxe_input.token[0][2]<<'\n';
    out.close();
}
int main(){
    SICXE_input sicxe_input;
    map<string,int> SYMTAB;
    read_file(sicxe_input);
    OPTAB=build_sicxe_OPTAB();
    SYMTAB=pass_1(sicxe_input);
    pass_2(sicxe_input,SYMTAB);
    write_file(sicxe_input);
    return 0;
}
