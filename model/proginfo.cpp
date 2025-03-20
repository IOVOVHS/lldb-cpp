#include "proginfo.h"

VarInfo::VarInfo(){}


/*
TraceInfo::TraceInfo(){}
*/

void VarInfo::print() const {
    std::cout << "Name: " << name << ", Func: " << func 
                << ", Decl: " << decl << ", Assigns: ";
    for (const auto& assign : assigns) {
        std::cout << assign << " ";
    }
    std::cout << std::endl;
}

/*
void Lexical_Block::print() const{
    std::cout << " level: " << level << " ,pos: " << pos.size() <<"\n";
    for(auto lex_blk:lex_blks){
        lex_blk->print();
    }
}*/

/*
void Func_Lexical::print(){
    std::cout << func_name << std::endl;
    lex_blk->print();
}*/

/*
Lexical_Block* get_obj_from_json(json j_lex_up){
    Lexical_Block* lex_up=new Lexical_Block();
    json locs=j_lex_up["locations"];
    lex_up->level=j_lex_up["level"].get<int>();
    for(auto loc:locs){
        int line = loc["line"].get<int>();
        int column = loc["column"].get<int>();
        lex_up->add_loc(line,column);   
    }
    json array = j_lex_up["lexical_blocks"];
    for(auto j_lex_down : array){
        Lexical_Block* lex_down = get_obj_from_json(j_lex_down);
        lex_up->lex_blks.push_back(lex_down);
    }
    return lex_up;
};

void Lexical_Block::add_loc(int line,int column){
    pos.push_back({line,column});
}

*/

bool VarInfo::isInited(std::vector<int>& lines){
    for(auto line:lines){
        if(find(assigns.begin(),assigns.end(),line)!=assigns.end()){
            return true;
        }
    }
    return false;
}


void VarInfo::add_assigns_executed(int assign){
    assigns_executed.push_back(assign);
}


/*
void TraceInfo::getInitedVarInfo(std::vector<VarInfo*> varInfos, int stopped_line,const std::string& func_name){
    //找出已经运行的行
    auto all_lines = func_lines[func_name];
    int index = all_lines.size();
    while(index>=0){
        if(stopped_line==all_lines[index]){
            break;
        }
        index--;
    }
    
    std::vector<int> executed_lines;
    
    //过滤出赋值行
    for(auto varinfo:varInfos){
        auto lines = func_lines[varinfo->func];
        for(auto assign:varinfo->assigns){
            if(find(lines.begin(),lines.end(),assign)==lines.end()){
                varinfo->add_assigns_executed(assign);
            }
        }
    }
}
*/