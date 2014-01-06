//
//  eqcMain.cpp
//  EQCommander
//

#include "eqcMain.h"

/******************************************************************************
*
* EQCommander
*
****/
EQCommander::EQCommander () {
    m_cmdParser = new EQCmdParser(&m_cmdList, this);
    m_cmdLog    = new EQCmdLog();
    m_config    = new EQCmdConfig;
    m_dmgParser = new EQDmgParser(this);
    
    m_config->LoadConfig();
    m_cmdParser->Init();
    m_dmgParser->Init();
    m_cmdLog->OpenCmdLog(m_cmdParser, m_dmgParser, m_config->GetCmdLogPath());
}
EQCommander::~EQCommander () {
    m_cmdLog->Close();
    
    delete m_cmdLog;
    delete m_cmdParser;
    delete m_dmgParser;
    delete m_config;
}

//-----------------------------------------------------------------------------
void EQCommander::BeginUpdate () {
    m_beginTime = time(NULL);
}

//-----------------------------------------------------------------------------
void EQCommander::EndUpdate () {
    map<unsigned int, time_t>::iterator mt = m_clientMap.begin();
    while (mt != m_clientMap.end()){
        if (mt->second < m_beginTime) {
            cout << "Client unregistered: " << mt->first << endl;
            m_clientMap.erase(mt++);
        } else {
            ++mt;
        }
    }
}

//-----------------------------------------------------------------------------
EQCmdParser * EQCommander::GetCmdParser () {
    return m_cmdParser;
}

//-----------------------------------------------------------------------------
EQCmdConfig * EQCommander::GetConfig () {
    return m_config;
}

//-----------------------------------------------------------------------------
EQDmgParser * EQCommander::GetDmgParser () {
    return m_dmgParser;
}

//-----------------------------------------------------------------------------
unsigned int EQCommander::GetFirstPsn () {
    map<unsigned int, time_t>::iterator it = m_clientMap.begin();
    return it->first;
}

//-----------------------------------------------------------------------------
vector<CharCommand> * EQCommander::GetNewCommands () {
    m_cmdLog->ReadCmdLog(true);
    
    return &m_cmdList;
}

//-----------------------------------------------------------------------------
vector<unsigned int> EQCommander::GetPsnList () {
    vector<unsigned int> list;
    map<unsigned int, time_t>::iterator it;
    for (it = m_clientMap.begin(); it != m_clientMap.end(); ++it)
        list.push_back(it->first);
    
    return list;
}

//-----------------------------------------------------------------------------
void EQCommander::TouchClient (unsigned int psn) {
    if (psn < 1)
        return;
    
    if (m_clientMap.count(psn) < 1) {
        m_clientMap.insert(pair<unsigned int, time_t>(
            psn,
            time(NULL)
        ));
    
        cout << "Client registered: " << psn << endl;
        cout << m_clientMap.size() << " clients total" << endl;
    } else {
        m_clientMap[psn] = time(NULL);
    }
}


/******************************************************************************
*
* Helpers
*
****/

//-----------------------------------------------------------------------------
void AddKeyCode (char c, vector<int> * results) {
    if (isupper(c))
        results->push_back(KEY_SHIFT);

    switch (c) {
        case 'a':
        case 'A':
            results->push_back(KEY_A);
            break;
        
        case 'b':
        case 'B':
            results->push_back(KEY_B);
            break;
        
        case 'c':
        case 'C':
            results->push_back(KEY_C);
            break;    
        
        case 'd':
        case 'D':
            results->push_back(KEY_D);
            break;    
        
        case 'e':
        case 'E':
            results->push_back(KEY_E);
            break;
        
        case 'f':
        case 'F':
            results->push_back(KEY_F);
            break;    
        
        case 'g':
        case 'G':
            results->push_back(KEY_G);
            break;    
        
        case 'h':
        case 'H':
            results->push_back(KEY_H);
            break;   
        
        case 'i':
        case 'I':
            results->push_back(KEY_I);
            break;    
        
        case 'j':
        case 'J':
            results->push_back(KEY_J);
            break;    
        
        case 'k':
        case 'K':
            results->push_back(KEY_K);
            break;    
        
        case 'l':
        case 'L':
            results->push_back(KEY_L);
            break;    
        
        case 'm':
        case 'M':
            results->push_back(KEY_M);
            break;
        
        case 'n':
        case 'N':
            results->push_back(KEY_N);
            break;
        
        case 'o':
        case 'O':
            results->push_back(KEY_O);
            break;
        
        case 'p':
        case 'P':
            results->push_back(KEY_P);
            break;
        
        case 'q':
        case 'Q':
            results->push_back(KEY_Q);
            break;
        
        case 'r':
        case 'R':
            results->push_back(KEY_R);
            break;
        
        case 's':
        case 'S':
            results->push_back(KEY_S);
            break;
        
        case 't':
        case 'T':
            results->push_back(KEY_T);
            break;
        
        case 'u':
        case 'U':
            results->push_back(KEY_U);
            break;
        
        case 'v':
        case 'V':
            results->push_back(KEY_V);
            break;
        
        case 'w':
        case 'W':
            results->push_back(KEY_W);
            break;
        
        case 'x':
        case 'X':
            results->push_back(KEY_X);
            break;
        
        case 'y':
        case 'Y':
            results->push_back(KEY_Y);
            break;
        
        case 'z':
        case 'Z':
            results->push_back(KEY_Z);
            break;
        
        case ' ':
            results->push_back(KEY_SPACE);
            break;
        
        case '~':
            results->push_back(KEY_SHIFT);
        case '`':
            results->push_back(KEY_BACKTICK);
            break;
        
        case '!':
            results->push_back(KEY_SHIFT);
        case '1':
            results->push_back(KEY_1);
            break;
        
        case '@':
            results->push_back(KEY_SHIFT);
        case '2':
            results->push_back(KEY_2);
            break;
        
        case '#':
            results->push_back(KEY_SHIFT);
        case '3':
            results->push_back(KEY_3);
            break;
        
        case '$':
            results->push_back(KEY_SHIFT);
        case '4':
            results->push_back(KEY_4);
            break;
        
        case '%':
            results->push_back(KEY_SHIFT);
        case '5':
            results->push_back(KEY_5);
            break;
        
        case '^':
            results->push_back(KEY_SHIFT);
        case '6':
            results->push_back(KEY_6);
            break;
        
        case '&':
            results->push_back(KEY_SHIFT);
        case '7':
            results->push_back(KEY_7);
            break;
        
        case '*':
            results->push_back(KEY_SHIFT);
        case '8':
            results->push_back(KEY_8);
            break;
        
        case '(':
            results->push_back(KEY_SHIFT);
        case '9':
            results->push_back(KEY_9);
            break;
        
        case ')':
            results->push_back(KEY_SHIFT);
        case '0':
            results->push_back(KEY_0);
            break;
        
        case '_':
            results->push_back(KEY_SHIFT);
        case '-':
            results->push_back(KEY_MINUS);
            break;
        
        case '+':
            results->push_back(KEY_SHIFT);
        case '=':
            results->push_back(KEY_EQUALS);
            break;
        
        case '\b':
            results->push_back(KEY_DELETE);
            break;
            
        case '{':
            results->push_back(KEY_SHIFT);
        case '[':
            results->push_back(KEY_OPEN_BRACKET);
            break;
            
        case '}':
            results->push_back(KEY_SHIFT);
        case ']':
            results->push_back(KEY_CLOSE_BRACKET);
            break;
            
        case '|':
            results->push_back(KEY_SHIFT);
        case '\\':
            results->push_back(KEY_BACKSLASH);
            break;
            
        case ':':
            results->push_back(KEY_SHIFT);
        case ';':
            results->push_back(KEY_SEMICOLON);
            break;
            
        case '"':
            results->push_back(KEY_SHIFT);
        case '\'':
            results->push_back(KEY_QUOTE);
            break;
            
        case '<':
            results->push_back(KEY_SHIFT);
        case ',':
            results->push_back(KEY_COMMA);
            break;
            
        case '>':
            results->push_back(KEY_SHIFT);
        case '.':
            results->push_back(KEY_PERIOD);
            break;
            
        case '?':
            results->push_back(KEY_SHIFT);
        case '/':
            results->push_back(KEY_SLASH);
            break;
            
        case '\n':
            results->push_back(KEY_ENTER);
            break;
    }
}

//-----------------------------------------------------------------------------
vector<int> StrToKeyCodes (string src) {
    vector<int> results;
    
    string::iterator it;
    for (it = src.begin(); it != src.end(); ++it)
        AddKeyCode(*it, &results);
    
    return results;
}
