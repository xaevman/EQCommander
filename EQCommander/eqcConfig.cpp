//
//  eqcConfig.cpp
//  EQCommander
//

#include "eqcMain.h"

/******************************************************************************
*
* Const
*
****/
const char * APP_CONFIG         = "./config/EQCommander.ini";
const char * CONFIG_LINE_REGEX  = "^(\\S*?)\\s*=\\s*(.*)\\s*$";
const char * CFG_LOG_CHAR_REGEX = ".*eqlog_(.*)_52.txt$";
const char * CMD_LOG_KEY        = "cmdLog";
const char * CMD_CHANNEL_KEY    = "cmdChannel";
const char * SLAVE_CHAR_KEY     = "slaveChar";
const char * STAT_CHANNEL_KEY   = "statChannel";


/******************************************************************************
*
* EQCmdConfig
*
****/
EQCmdConfig::EQCmdConfig () {
    m_statChanNum = 1;
}
EQCmdConfig::~EQCmdConfig () {
    m_charNames.clear();
}

//-----------------------------------------------------------------------------
unsigned int EQCmdConfig::GetFirstCharPsn () {
    map<string, unsigned int>::iterator it;
    for (it = m_charNames.begin(); it != m_charNames.end(); ++it) {
        if (it->second > 0)
            return it->second;
    }
    
    return 0;
}

//-----------------------------------------------------------------------------
void EQCmdConfig::GetTargetPsn (
    CmdData *              data,
    vector<unsigned int> * targets
) {
    if (data->Target == "All" && data->Sender == "You") {
        map<string, unsigned int>::iterator it;
        for (it = m_charNames.begin(); it != m_charNames.end(); ++it)
            targets->push_back(it->second);
    } else if (m_charNames.count(data->Target) > 0) {
        unsigned int psn = m_charNames.find(data->Target)->second;
        targets->push_back(psn);
    } else if (data->Sender != "You" && data->Target == m_masterCharName) {
        targets->push_back(m_masterPsn);
    }
}

//-----------------------------------------------------------------------------
void EQCmdConfig::LoadConfig () {
    ifstream file;
    string   thisLine;
    smatch   results;
    
    m_charNames.clear();
    
    file.open(APP_CONFIG);
    
    if (!file.is_open()) {
        cout << "Unable to open config file: " << APP_CONFIG << endl;
        return;
    }
    
    while (!file.eof()) {
        getline(file, thisLine);
        
        if (thisLine.size() < 3)
            continue;
        
        if (thisLine.at(0) == ';')
            continue;
        
        if (!regex_match(thisLine, results, regex(CONFIG_LINE_REGEX)))
            continue;
        
        if (results.size() < 3)
            continue;
        
        if (results[1] == CMD_LOG_KEY) {
            m_cmdLogPath = results[2];
            cout << "Config::CmdLogPath = " << results[2] << endl;
        } else if (results[1] == CMD_CHANNEL_KEY) {
            m_cmdChannel = results[2];
            cout << "Config::CmdChannel = " << results[2] << endl;
        } else if (results[1] == SLAVE_CHAR_KEY) {
            m_charNames.insert(pair<string, unsigned int>(results[2], 0));
            cout << "Config::SlaveChar = " << results[2] << endl;
        } else if (results[1] == STAT_CHANNEL_KEY) {
            m_statChanNum = atoi(results[2].str().c_str());
            cout << "Config::StatChannelNumber = " << results[2] << endl;
        }
    }
    
    file.close();
    
    regex_match(m_cmdLogPath, results, regex(CFG_LOG_CHAR_REGEX));
    if (results.size() < 2)
        return;
    
    m_masterCharName = results[1];
    cout << "Config::MasterChar = " << m_masterCharName << endl;
}

//-----------------------------------------------------------------------------
void EQCmdConfig::RegisterMasterPsn (unsigned int psn) {
    m_masterPsn = psn;
}

//-----------------------------------------------------------------------------
void EQCmdConfig::RegisterTargetPsn (string charName, unsigned int psn) {
    if (charName == "You")
        return;
    
    if (m_charNames.count(charName) < 1)
        return;
    else {
        m_charNames[charName] = psn;
        cout << "Register: " << charName << " at " << psn << endl;
    }
}
