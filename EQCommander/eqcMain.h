//
//  eqcMain.h
//  EQCommander
//

#ifndef __EQCommander__eqcMain__
#define __EQCommander__eqcMain__

#include <dirent.h>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <regex>
#include <set>
#include <sstream>
#include <string>
#include <time.h>
#include <vector>

using namespace std;

/******************************************************************************
*
* Const
*
****/

// Buffers
const int SMALL_BUFFER      = 128;
const int STD_BUFFER        = 1024;

// Parser Config
const int MAX_LOOP_COUNT    = 1000;

//-- ROW 1
const int KEY_ESC           = 53;
const int KEY_F1            = 122;
const int KEY_F2            = 120;
const int KEY_F3            = 99;
const int KEY_F4            = 118;
const int KEY_F5            = 96;
const int KEY_F6            = 97;
const int KEY_F7            = 98;
const int KEY_F8            = 100;
const int KEY_F9            = 101;
const int KEY_F10           = 109;
const int KEY_F11           = 103;
const int KEY_F12           = 111;

//-- ROW 2
const int KEY_BACKTICK      = 50;
const int KEY_1             = 18;
const int KEY_2             = 19;
const int KEY_3             = 20;
const int KEY_4             = 21;
const int KEY_5             = 23;
const int KEY_6             = 22;
const int KEY_7             = 26;
const int KEY_8             = 28;
const int KEY_9             = 25;
const int KEY_0             = 29;
const int KEY_MINUS         = 27;
const int KEY_EQUALS        = 24;
const int KEY_DELETE        = 51;

// -- ROW 3
const int KEY_TAB           = 48;
const int KEY_Q             = 12;
const int KEY_W             = 13;
const int KEY_E             = 14;
const int KEY_R             = 15;
const int KEY_T             = 17;
const int KEY_Y             = 16;
const int KEY_U             = 32;
const int KEY_I             = 34;
const int KEY_O             = 31;
const int KEY_P             = 35;
const int KEY_OPEN_BRACKET  = 33;
const int KEY_CLOSE_BRACKET = 30;
const int KEY_BACKSLASH     = 42;

// -- ROW 4
const int KEY_A             = 0;
const int KEY_S             = 1;
const int KEY_D             = 2;
const int KEY_F             = 3;
const int KEY_G             = 5;
const int KEY_H             = 4;
const int KEY_J             = 38;
const int KEY_K             = 40;
const int KEY_L             = 37;
const int KEY_SEMICOLON     = 41;
const int KEY_QUOTE         = 39;
const int KEY_ENTER         = 36;

// -- ROW 5
const int KEY_SHIFT         = 56;
const int KEY_Z             = 6;
const int KEY_X             = 7;
const int KEY_C             = 8;
const int KEY_V             = 9;
const int KEY_B             = 11;
const int KEY_N             = 45;
const int KEY_M             = 46;
const int KEY_COMMA         = 43;
const int KEY_PERIOD        = 47;
const int KEY_SLASH         = 44;

// -- ROW 6
const int KEY_SPACE         = 49;
const int KEY_UP            = 126;
const int KEY_DOWN          = 125;
const int KEY_LEFT          = 123;
const int KEY_RIGHT         = 124;

/******************************************************************************
*
* CharCommand
*
****/
struct CharCommand {
    vector<int>  Command;
    unsigned int ProcessSerialNumber;
    long         RunTime;
};

/******************************************************************************
*
* CmdData
*
****/
struct CmdData {
    vector<string> Args;
    vector<string> CommandLines;
    string         Macro;
    string         Target;
    string         Sender;
};

/******************************************************************************
*
* WeaponStat
*
****/
struct WeaponStat {
    unsigned int  blocks;
    unsigned long dmgTotal;
    unsigned int  dodges;
    unsigned int  hits;
    unsigned long max;
    unsigned int  misses;
    unsigned int  parries;
    unsigned int  ripostes;
    unsigned int  swings;
    string        type;
};

/******************************************************************************
*
* Forward declarations
*
****/
class EQCommander;
class EQCmdLog;
class EQCmdConfig;
class EQCmdParser;
class EQDmgParser;

/******************************************************************************
*
* EQCommander
*
****/
class EQCommander {
    private :
        time_t                    m_beginTime;
        vector<CharCommand>       m_cmdList;
        EQCmdLog    *             m_cmdLog;
        EQCmdParser *             m_cmdParser;
        EQCmdConfig *             m_config;
        EQDmgParser *             m_dmgParser;
        map<unsigned int, time_t> m_clientMap;
    
    public :
        EQCommander ();
        ~EQCommander ();
    
        void BeginUpdate ();
        void EndUpdate ();
        EQCmdParser * GetCmdParser ();
        EQCmdConfig * GetConfig ();
        EQDmgParser * GetDmgParser ();
        unsigned int GetFirstPsn ();
        vector<CharCommand> * GetNewCommands ();
        vector<unsigned int> GetPsnList ();
        void TouchClient (unsigned int psn);
};

/******************************************************************************
*
* EQCmdConfig
*
****/
class EQCmdConfig {
    private:
        map<string, unsigned int> m_charNames;
        string                    m_cmdChannel;
        string                    m_cmdLogPath;
        string                    m_masterCharName;
        unsigned int              m_masterPsn;
        unsigned int              m_statChanNum;
    
    public:
        EQCmdConfig ();
        ~EQCmdConfig ();
    
        void Close ();
        string GetCmdChannel () { return m_cmdChannel; }
        string GetCmdLogPath () { return m_cmdLogPath; }
        unsigned int GetFirstCharPsn ();
        string GetMasterCharName () { return m_masterCharName; }
        unsigned int GetStatChanNum () { return m_statChanNum; }
        void GetTargetPsn (
            CmdData *              data,
            vector<unsigned int> * targets
        );
        void LoadConfig ();
        void RegisterMasterPsn (unsigned int psn);
        void RegisterTargetPsn (string charName, unsigned int psn);
};

/******************************************************************************
*
* EQCmdParser
*
****/
class EQCmdParser {
    private:
        vector<CharCommand> * m_runList;
        string                m_cmdRegex;
        EQCmdConfig *         m_config;
        EQCommander *         m_parent;
        EQDmgParser *         m_dmgParser;
        set<string>           m_trustList;
    
        void RunSysCmd (CmdData * data);
        CmdData * StrToCmdData (string line);
    
    public:
        EQCmdParser (vector<CharCommand> * runList, EQCommander * parent);
        ~EQCmdParser ();
    
        void Init ();
        void ParseLine (string line);
        void SubmitCmd (CharCommand * cmd);
};

/******************************************************************************
*
* EQCmdLog
*
****/
class EQCmdLog {
    private:
        EQCmdParser * m_cmdParser;
        EQDmgParser * m_dmgParser;
        ifstream      m_file;
    
        void ParseCmdLine (string line);

    public:
        EQCmdLog ();
        ~EQCmdLog ();    
    
        void Close ();
        void ReadCmdLog (bool capture);
        void OpenCmdLog (
            EQCmdParser * cmdParser,
            EQDmgParser * dmgParser,
            string path)
        ;
};

/******************************************************************************
*
* EQCmdMacro
*
****/
class EQCmdMacro {
    private:
        string         m_name;
        vector<string> m_rawLines;
    
    public:
        EQCmdMacro (const char * dir, const char * path);
        ~EQCmdMacro ();
    
        void FillCommandText (CmdData * cmdData);
        string ResolveCommandText (string rawLine, vector<string> * args);
};

/******************************************************************************
*
* EQDmgParser
*
****/
class EQDmgParser {
    private:
        EQCommander *             m_commander;
        EQCmdConfig *             m_config;
        map<string, WeaponStat *> m_dmgMap;
        EQCmdParser *             m_parser;
        bool                      m_parsing;
        time_t                    m_startTime;

        void EndParse ();
        void ParseEvade (smatch * results);
        void ParseHit (smatch * results);
        void ParseMiss (smatch * results);
        void ParseMobEvade (smatch * results);
        void ParseMobHit (smatch * results);
        void ParseMobMiss (smatch * results);
        WeaponStat * GetDmgStats (string type);
        void StartParse ();
    
    public:
        EQDmgParser (EQCommander * commander);
        ~EQDmgParser ();
    
        void Init ();
        void ParseDmgLine (string line);
        void PrintResults ();
};

/******************************************************************************
*
* Helpers
*
****/
void AddKeyCode (char c, vector<int> * results);
EQCmdMacro * FindMacro (string name);
void InitMacros ();
vector<int> StrToKeyCodes (string src);



#endif /* defined(__EQCommander__eqcMain__) */
