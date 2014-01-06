//
//  eqcMacro.cpp
//  EQCommander
//

#include "eqcMain.h"

/******************************************************************************
*
* Statics
*
****/
const char * MACRO_DIR = "./macros";

static map<string, EQCmdMacro *> s_instances;


/******************************************************************************
*
* EQCmdMacro
*
****/
EQCmdMacro::EQCmdMacro (const char * dir, const char * name) {
    m_name = string(name);
    
    ifstream file;
    string   thisLine;
    
    char buffer[STD_BUFFER];
    snprintf(buffer, STD_BUFFER, "%s/%s", dir, name);
    
    file.open(buffer);
    
    if (!file.is_open())
        return;
    
    while(!file.eof()) {
        getline(file, thisLine);
        m_rawLines.push_back(thisLine);
    }
    
    s_instances[m_name] = this;
    
    file.close();
}
EQCmdMacro::~EQCmdMacro() {
    s_instances.erase(m_name);
}

//-----------------------------------------------------------------------------
void EQCmdMacro::FillCommandText (CmdData * cmdData) {
    cmdData->CommandLines.clear();
    
    string resolvedLine;
    
    vector<string>::iterator it;
    for (it = m_rawLines.begin(); it != m_rawLines.end(); ++it) {
        resolvedLine = ResolveCommandText(*it, &cmdData->Args);
        cmdData->CommandLines.push_back(resolvedLine);
        cmdData->CommandLines.push_back("\n");
    }
}

//-----------------------------------------------------------------------------
string EQCmdMacro::ResolveCommandText (string rawTxt, vector<string> * args) {
    char   buffer[SMALL_BUFFER];
    int    cursor  = 0;
    string cmdText = string(rawTxt);
    
    vector<string>::iterator it;
    for (it = args->begin(); it != args->end(); ++it) {
        snprintf(buffer, SMALL_BUFFER, "<arg%d>", cursor);
        cmdText = regex_replace(cmdText, regex(buffer), *it);
        ++cursor;
    }
    
    return cmdText;
}


/******************************************************************************
*
* Helpers
*
****/

//-----------------------------------------------------------------------------
void InitMacros () {
    DIR *           dirInfo;
    struct dirent * entry;
    
    dirInfo = opendir(MACRO_DIR);
    
    if (dirInfo == NULL) {
        delete dirInfo;
        return;
    }
    
    s_instances.clear();
    
    entry = readdir(dirInfo);
    while (entry != NULL) {
        if (entry->d_type != DT_REG) {
            entry = readdir(dirInfo);
            continue;
        }
        
        cout << "Macro found (" << entry->d_name << ")" << endl;
        
        new EQCmdMacro(MACRO_DIR, entry->d_name);
        
        entry = readdir(dirInfo);
    }
    
    delete dirInfo;
    delete entry;
}

//-----------------------------------------------------------------------------
EQCmdMacro * FindMacro (string name) {    
    if (s_instances.count(name))
        return s_instances.find(name)->second;
    else
        return NULL;
}
