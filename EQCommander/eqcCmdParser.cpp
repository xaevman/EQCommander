//
//  eqcCmdParser.cpp
//  EQCommander
//

#include "eqcMain.h"


/******************************************************************************
*
* EQCmdParser
*
****/
EQCmdParser::EQCmdParser (vector<CharCommand> * runList, EQCommander * parent) {
    m_runList = runList;
    m_parent  = parent;
    m_trustList.clear();
    m_trustList.insert("You");
}
EQCmdParser::~EQCmdParser() {
    m_trustList.clear();
}

//-----------------------------------------------------------------------------
void EQCmdParser::Init () {
    m_dmgParser = m_parent->GetDmgParser();
    
    m_config = m_parent->GetConfig();
    
    char regex_buffer[STD_BUFFER];
    snprintf(
        regex_buffer,
        STD_BUFFER,
        "^\\[.*\\] (\\S*) tells? %s:\\d, \\'(\\S*) (\\S*)\\s?(.*)?\\'",
        m_config->GetCmdChannel().c_str()
    );
    
    m_cmdRegex = string(regex_buffer);
}

//-----------------------------------------------------------------------------
void EQCmdParser::ParseLine (string line) {
    if (m_config == NULL)
        return;
    
    CmdData * data = StrToCmdData(line);
    if (data == NULL)
        return;
    
    // once
    for (;;) {
        // handle sys commands
        if (data->Target == "Sys") {
            RunSysCmd(data);
            break;
        } else {
            if (m_trustList.find(data->Sender) == m_trustList.end())
                break;
        
            // try to get macro object
            EQCmdMacro * macro = FindMacro(data->Macro.c_str());
            if (macro == NULL)
                break;
        
            // feed args to macro to get resolved text
            macro->FillCommandText(data);
          
            // check to see if known client
            vector<unsigned int> targets;
            m_config->GetTargetPsn(data, &targets);
            
            long baseTime = time(NULL);
            vector<string>::iterator cmdIt;
    
            for (cmdIt = data->CommandLines.begin(); cmdIt != data->CommandLines.end(); ++cmdIt) {
                // handle pauses
                smatch results;
                if (regex_match(*cmdIt, results, regex("PAUSE (\\d*)"))) {
                    string temp = string(results[1]);
                    baseTime   += atol(temp.c_str());
                    continue;
                }
            
                // create CharCommand objects
                vector<unsigned int>::iterator targetIt;
                for (targetIt = targets.begin(); targetIt != targets.end(); ++targetIt) {
                    CharCommand * newCmd        = new CharCommand();
                    newCmd->ProcessSerialNumber = *targetIt;
                    newCmd->RunTime             = baseTime;
                    newCmd->Command             = StrToKeyCodes(*cmdIt);
                    m_runList->push_back(*newCmd);
                }
            }
            
            break;
        }
    }
    
    delete data;
}

//-----------------------------------------------------------------------------
void EQCmdParser::RunSysCmd (CmdData * data) {
    if (data->Macro == "ClearTrust") {
        m_trustList.clear();
        m_trustList.insert("You");
    } else if (data->Macro == "Identify") {
        if (data->Sender != "You")
            return;
        
        vector<unsigned int> psnList = m_parent->GetPsnList();
        
        vector<unsigned int>::iterator it;
        for (it = psnList.begin(); it != psnList.end(); ++it) {            
            EQCmdMacro * macro = FindMacro("sys_identify");
            if (macro == NULL)
                return;
            
            data->Args.clear();
            data->Args.push_back(to_string(*it));            
            macro->FillCommandText(data);
            
            vector<string>::iterator cmdIt;
            for (cmdIt = data->CommandLines.begin(); cmdIt != data->CommandLines.end(); ++cmdIt) {        
                CharCommand * newCmd        = new CharCommand();
                newCmd->ProcessSerialNumber = *it;
                newCmd->RunTime             = time(NULL);
                newCmd->Command             = StrToKeyCodes(*cmdIt);
                m_runList->push_back(*newCmd);
            }
        }
    } else if (data->Macro == "Register") {
        if (data->Args.size() < 1)
            return;
        
        if (data->Sender == "You") {
            m_config->RegisterMasterPsn(
                atoi(data->Args[0].c_str())
            );
            return;
        }
        
        m_config->RegisterTargetPsn(
            data->Sender.c_str(),
            atoi(data->Args[0].c_str())
        );
    } else if (data->Macro == "Rescan") {
        if (data->Sender != "You")
            return;
        
        InitMacros();
    } else if (data->Macro == "Trust") {
        if (data->Sender != "You")
            return;
        
        m_trustList.insert(data->Args[0]);
    } else if (data->Macro == "PrintDmgParse") {
        m_dmgParser->PrintResults();
    }
}

//-----------------------------------------------------------------------------
CmdData * EQCmdParser::StrToCmdData (string line) {
    if (m_config == NULL)
        return NULL;
    
    smatch results;
    if (!regex_match(line, results, regex(m_cmdRegex)))
        return NULL;
    
    cout << "Cmd::" << line << endl;
    
    if (results.size() < 4)
        return NULL;
    
    CmdData * newData = new CmdData();
    newData->Sender   = results[1];
    newData->Target   = results[2];
    newData->Macro    = results[3];
    
    if (results.size() < 5)
        return newData;
    
    string arg;
    stringstream argStream(results[4]);
    while (getline(argStream, arg, ' '))
        newData->Args.push_back(arg);
    
    return newData;
}

//-----------------------------------------------------------------------------
void EQCmdParser::SubmitCmd (CharCommand * cmd) {
    m_runList->push_back(*cmd);
}
