//
//  eqcCmdLog.cpp
//  EQCommander
//

#include "eqcMain.h"

/******************************************************************************
*
* EQCmdLog
*
****/
EQCmdLog::EQCmdLog () {}
EQCmdLog::~EQCmdLog () {
    Close();
}

//-----------------------------------------------------------------------------
void EQCmdLog::Close () {
    m_file.close();
    delete(m_cmdParser);
    delete(m_dmgParser);
}

//-----------------------------------------------------------------------------
void EQCmdLog::OpenCmdLog (
    EQCmdParser * cmdParser,
    EQDmgParser * dmgParser,
    string path
) {
    m_cmdParser = cmdParser;
    m_dmgParser = dmgParser;
    
    m_file.open(path);
    
    ReadCmdLog(false);
}

//-----------------------------------------------------------------------------
void EQCmdLog::ReadCmdLog (bool capture) {
    if (m_cmdParser == NULL || m_dmgParser == NULL)
        return;
    
    if (!m_file.is_open())
        return;
    
    string thisLine;
    
    m_file.clear();
    m_file.sync();
    
    for(;;) {
        while (getline(m_file, thisLine)) {
            if (capture) {
                m_cmdParser->ParseLine(thisLine);
                m_dmgParser->ParseDmgLine(thisLine);
            }
            
            if (m_file.eof())
                break;
        }
        
        if (m_file.eof())
            break;
    }
}
