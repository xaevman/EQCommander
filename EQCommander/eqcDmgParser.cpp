//
//  eqcDmgParser
//  EQCommander
//

#include "eqcMain.h"


/******************************************************************************
*
* Const
*
****/
const unsigned int MIN_PARSE_DATA_POINTS = 100;
const char *       MOB_TYPE              = "Mob";
const char *       YOU_TYPE              = "You";

const char * REGEX_ATK_OFF   = "^\\[.*\\] Auto attack is off\\.$";
const char * REGEX_XP_GAINED = "^\\[.*\\] You gain.*experience!!&";
const char * REGEX_MOB_SLAIN = "^\\[.*\\] .* slain .*!$";
const char * REGEX_DIED      = "^\\[.*\\] .* died\\.$";
const char * REGEX_ATK_ON    = "^\\[.*\\] Auto attack is on\\.$";

// You
const char * REGEX_DMG       = "^\\[.*\\] You (\\w*) (.*) for (\\d*) points of damage\\.$";
const char * REGEX_MISS      = "^\\[.*\\] You try to (\\w*) (.*), but miss!$";
const char * REGEX_MOB_EVADE = "^\\[.*\\] You try to (\\w*) (.*), but (.*) (\\w*)!$";

// Heals
const char * REGEX_HEAL      = "^\\[.*\\] You have been healed .*$";

// Mob
const char * REGEX_MOB_DMG   = "^\\[.*\\] (.*) (\\w*) YOU for (\\d*) points of damage\\.$";
const char * REGEX_MOB_MISS  = "^\\[.*\\] (.*) tries to (\\w*) YOU, but misses!$";
const char * REGEX_EVADE     = "^\\[.*\\] (.*) tries to (\\w*) YOU, but YOU (\\w*)!$";


/******************************************************************************
*
* Helpers
*
****/

//-----------------------------------------------------------------------------
float CalcEvadedPercent (WeaponStat * stats) {
    unsigned int evadeCnt = stats->swings - (stats->hits + stats->misses);
    return ((float)evadeCnt / (float)stats->swings) * 100.0f;
}

//-----------------------------------------------------------------------------
float CalcHitPercent (WeaponStat * stats) {
    return ((float)stats->hits / (float)stats->swings) * 100.0f;
}

//-----------------------------------------------------------------------------
float CalcMissPercent (WeaponStat * stats) {
    return ((float)stats->misses / (float)stats->swings) * 100.0f;
}

//-----------------------------------------------------------------------------
float CalcMitigatePercent (WeaponStat * stats) {
    float avgDmg = (float)stats->dmgTotal / (float)stats->hits;
    float ratio  = avgDmg / (float)stats->max;
    return 100.0f - (ratio * 100.0f);
}

//-----------------------------------------------------------------------------
void ParseEvadeCounter (WeaponStat * stats, string eType) {
    if (eType == "riposte" || eType == "ripostes")
        ++stats->ripostes;
    else if (eType == "block" || eType == "blocks")
        ++stats->blocks;
    else if (eType == "parry" || eType == "parries")
        ++stats->parries;
    else if (eType == "dodge" || eType == "dodges")
        ++stats->dodges;
}

//-----------------------------------------------------------------------------
void PrintStats (
    stringstream * out,
    WeaponStat * you,
    WeaponStat * opponent,
    double parseLen,
    unsigned int outChan
) {
    // dps
    double dps    = (double)you->dmgTotal / parseLen;
    double avgDmg = (double)you->dmgTotal / (double)you->swings;
    *out << setprecision(2) << "/" << outChan << "     DPS:      " << dps << endl;
    
    // counters
    //out << "    Swings:   " << you->swings   << endl;
    //out << "    Hits:     " << you->hits     << endl;
    *out << "/" << outChan << "     TotalDmg: " << you->dmgTotal << endl;
    *out << "/" << outChan << "     AvgDmg:   " << setprecision(2) << avgDmg << endl;
    *out << "/" << outChan << "     MaxDmg:   " << you->max      << endl;
    
    // ratios
    float hitP      = CalcHitPercent(you);
    float missP     = CalcMissPercent(you);
    float mobEvadeP = CalcEvadedPercent(you);
    *out << "/" << outChan << "     Hit / Miss / Mob Evade %: "
        << setprecision(2)
        << hitP  << " / "
        << missP << " / "
        << mobEvadeP
        << endl;
    
    float evadedP = CalcEvadedPercent(opponent);
    *out << "/" << outChan << "     Evasion: "
        << setprecision(2)
        << evadedP << " %"
        << endl;
    
    float mitP = CalcMitigatePercent(opponent);
    *out << "/" << outChan << "     Mitigation: "
        << mitP << "+ %"
        << endl;
}


/******************************************************************************
*
* EQDmgParser
*
****/
EQDmgParser::EQDmgParser (EQCommander * commander) {
    m_commander = commander;
    m_parsing   = false;
    m_dmgMap.clear();
}
EQDmgParser::~EQDmgParser () {
    m_dmgMap.clear();
}

//-----------------------------------------------------------------------------
void EQDmgParser::EndParse () {
    PrintResults();
    m_parsing = false;
}

//-----------------------------------------------------------------------------
WeaponStat * EQDmgParser::GetDmgStats (string type) {
    if (m_dmgMap.count((type)) < 1) {
        WeaponStat * newStat = new WeaponStat();
        newStat->blocks      = 0;
        newStat->dmgTotal    = 0;
        newStat->dodges      = 0;
        newStat->hits        = 0;
        newStat->max         = 0;
        newStat->misses      = 0;
        newStat->parries     = 0;
        newStat->ripostes    = 0;
        newStat->swings      = 0;
        newStat->type        = type;
        
        m_dmgMap.insert(pair<string, WeaponStat *>(type, newStat));
    }
    
    return m_dmgMap[type];
}

//-----------------------------------------------------------------------------
void EQDmgParser::Init () {
    m_config = m_commander->GetConfig();
    m_parser = m_commander->GetCmdParser();
}

//-----------------------------------------------------------------------------
void EQDmgParser::ParseDmgLine (string line) {    
    smatch results;
    
    if (regex_match(line, results, regex(REGEX_HEAL)))
        return;
    else if (regex_match(line, results, regex(REGEX_DMG)))
        ParseHit(&results);
    else if (regex_match(line, results, regex(REGEX_MOB_DMG)))
        ParseMobHit(&results);
    else if (regex_match(line, results, regex(REGEX_MISS)))
        ParseMiss(&results);
    else if (regex_match(line, results, regex(REGEX_MOB_MISS)))
        ParseMobMiss(&results);
    else if (regex_match(line, results, regex(REGEX_EVADE)))
        ParseEvade(&results);
    else if (regex_match(line, results, regex(REGEX_MOB_EVADE)))
        ParseMobEvade(&results);
    else if (regex_match(line, results, regex(REGEX_ATK_OFF)))
        EndParse();
    else if (regex_match(line, results, regex(REGEX_XP_GAINED)))
        EndParse();
    else if (regex_match(line, results, regex(REGEX_MOB_SLAIN)))
        EndParse();
    else if (regex_match(line, results, regex(REGEX_DIED)))
        EndParse();
}

//-----------------------------------------------------------------------------
void EQDmgParser::ParseEvade (smatch * results) {
    if (!m_parsing)
        StartParse();
    
    // "\\[.*\\] (.*) tries to (\\w*) YOU, but YOU (\\w*)!$"
    
    string target = (*results)[1];
    string eType  = (*results)[3];
    
    WeaponStat * stats;
    
    // overall - you
    stats = GetDmgStats(YOU_TYPE);
    ParseEvadeCounter(stats, eType);
    
    // overall - mob
    stats = GetDmgStats(MOB_TYPE);
    ++stats->swings;
}

//-----------------------------------------------------------------------------
void EQDmgParser::ParseHit (smatch * results) {
    if (!m_parsing)
        StartParse();

    // "\\[.*\\] You (\\w*) (.*) for (\\d*) points of damage.$"
        
    string        type   = (*results)[1];
    string        target = (*results)[2];
    string        dmgStr = (*results)[3];
    unsigned long dmg    = strtoul(dmgStr.c_str(), NULL, 0);
    
    WeaponStat * stats;
    
    // type-specific
    stats = GetDmgStats(type);
    ++stats->hits;
    ++stats->swings;
    stats->dmgTotal += dmg;
    if (dmg > stats->max)
        stats->max = dmg;
    
    // overall
    stats = GetDmgStats(YOU_TYPE);
    ++stats->hits;
    ++stats->swings;
    stats->dmgTotal += dmg;
    if (dmg > stats->max)
        stats->max = dmg;
}

//-----------------------------------------------------------------------------
void EQDmgParser::ParseMiss (smatch * results) {
    if (!m_parsing)
        StartParse();

    // "\\[.*\\] You try to (\\w*) (.*), but miss!$"
    
    string type   = (*results)[1];
    string target = (*results)[2];
    
    WeaponStat * stats;
    
    // type-specific
    stats = GetDmgStats(type);
    ++stats->misses;
    ++stats->swings;
    
    // overall
    stats = GetDmgStats(YOU_TYPE);
    ++stats->misses;
    ++stats->swings;
}

//-----------------------------------------------------------------------------
void EQDmgParser::ParseMobEvade (smatch * results) {
    if (!m_parsing)
        return;
    
    // "\\[.*\\] You try to (\\w*) (.*), but (.*) (\\w*)!$"
    
    string target = (*results)[2];
    string eType  = (*results)[4];
    
    WeaponStat * stats;
    
    // overall - you
    stats = GetDmgStats(YOU_TYPE);
    ++stats->swings;
    
    // overall - mob
    stats = GetDmgStats(MOB_TYPE);
    ParseEvadeCounter(stats, eType);
}

//-----------------------------------------------------------------------------
void EQDmgParser::ParseMobHit (smatch * results) {
    if (!m_parsing)
        return;
    
    // "\\[.*\\] (.*) (\\w*) YOU for (\\d*) points of damage\\.$"
    
    string        target = (*results)[1];
    string        type   = (*results)[2];
    string        dmgStr = (*results)[3];
    unsigned long dmg    = strtoul(dmgStr.c_str(), NULL, 0);
    
    WeaponStat * stats;
    
    // overall
    stats = GetDmgStats(MOB_TYPE);
    ++stats->hits;
    ++stats->swings;
    stats->dmgTotal += dmg;
    if (dmg > stats->max)
        stats->max = dmg;
}

//-----------------------------------------------------------------------------
void EQDmgParser::ParseMobMiss (smatch * results) {
    if (!m_parsing)
        return;
    
    // "\\[.*\\] (.*) tries to (\\w*) YOU, but misses!$"
    
    string target = (*results)[1];
    string type   = (*results)[2];
    
    WeaponStat * stats;
    
    // overall
    stats = GetDmgStats(MOB_TYPE);
    ++stats->misses;
    ++stats->swings;
}

//-----------------------------------------------------------------------------
void EQDmgParser::PrintResults () {
    CharCommand * newCmd;
    stringstream  out;
    
    unsigned int  outChan   = m_config->GetStatChanNum();
    double        parseLen  = difftime(time(NULL), m_startTime);
    unsigned int  targetPsn = m_config->GetFirstCharPsn();

    WeaponStat *  youStats  = GetDmgStats(YOU_TYPE);
    WeaponStat *  mobStats  = GetDmgStats(MOB_TYPE);
    
    if (youStats->swings + mobStats->swings < MIN_PARSE_DATA_POINTS) {
        cout << "EQDmgParser::Ignoring parse. Not enough data points" << endl;
        return;
    }
    
    out << "/" << outChan << " ==== " << m_config->GetMasterCharName() << " ====" << endl;
    PrintStats(&out, youStats, mobStats, parseLen, outChan);
    if (targetPsn > 0) {
        newCmd = new CharCommand();
        newCmd->ProcessSerialNumber = targetPsn;
        newCmd->RunTime             = time(NULL);
        newCmd->Command             = StrToKeyCodes(out.str());
        m_parser->SubmitCmd(newCmd);
    }
    cout << out.str();
}

//-----------------------------------------------------------------------------
void EQDmgParser::StartParse () {
    m_parsing = true;
    cout << "EQDmgParser::StartParse" << endl;
    m_dmgMap.clear();
    m_startTime = time(NULL);
}
