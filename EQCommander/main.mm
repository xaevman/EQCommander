//
//  main.mm
//  EQCommander
//

#import <Foundation/Foundation.h>
#import <time.h>
#import "eqcMain.h"

/******************************************************************************
*
* Constants
*
****/
const CFStringRef EQ_PROCESS_NAME = CFSTR("EverQuest");


/******************************************************************************
*
* Forward declarations
*
****/
void ScanProcesses ();
void SendCommand (CharCommand cmd);


/******************************************************************************
*
* Statics
*
****/
static EQCommander s_com;


/******************************************************************************
*
* Entry point
*
****/

//-----------------------------------------------------------------------------
int main (int argc, const char * argv[]) {
    cout << "EQCommander v1: Hello!" << endl;

    InitMacros();
    
    for (;;) {
        s_com.BeginUpdate();
        
        long now = time(NULL);

        ScanProcesses();
        vector<CharCommand> * cmdList = s_com.GetNewCommands();
        
        s_com.EndUpdate();
        
        vector<CharCommand>::iterator it = cmdList->begin();
        while (it != cmdList->end()) {
            if (it->RunTime <= now) {
                SendCommand(*it);
                cmdList->erase(it);
            } else {
                ++it;
            }
        }
        
        usleep(100 * 1000); // ms * us
    }
    
    return 0;
}

//-----------------------------------------------------------------------------
void ScanProcesses () {
    ProcessSerialNumber psn  = { kNoProcess, kNoProcess };
    
    GetNextProcess(&psn);
    while (psn.lowLongOfPSN != kNoProcess) {
        GetNextProcess(&psn);
        
        if (psn.lowLongOfPSN == kNoProcess)
            break;
        
        CFStringRef name;
        CopyProcessName(&psn, &name);
        
        if (CFStringCompare(name, EQ_PROCESS_NAME, 0) == kCFCompareEqualTo)
            s_com.TouchClient(psn.lowLongOfPSN);
        
        CFRelease(name);
    }
}

//-----------------------------------------------------------------------------
void SendCommand (CharCommand cmd) {
    ProcessSerialNumber psn = {
        0,
        cmd.ProcessSerialNumber
    };

    vector<int>::iterator it;
    for (it = cmd.Command.begin(); it != cmd.Command.end(); ++it) {
        CGEventRef keyDown = NULL;
        CGEventRef keyUp   = NULL;
        
        if (*it == KEY_SHIFT) {
            ++it;
            
            keyDown = CGEventCreateKeyboardEvent(NULL, (CGKeyCode)*it, true);
            keyUp   = CGEventCreateKeyboardEvent(NULL, (CGKeyCode)*it, false);
            
            CGEventSetFlags(keyDown, kCGEventFlagMaskShift);
        
            CGEventPostToPSN(&psn, keyDown);
            CGEventPostToPSN(&psn, keyUp);
        } else {
            keyDown = CGEventCreateKeyboardEvent(NULL, (CGKeyCode)*it, true);
            keyUp   = CGEventCreateKeyboardEvent(NULL, (CGKeyCode)*it, false);
        
            CGEventPostToPSN(&psn, keyDown);
            CGEventPostToPSN(&psn, keyUp);
        }
        
        if (keyDown != NULL)
            CFRelease(keyDown);
        if (keyUp != NULL)
            CFRelease(keyUp);
        
        usleep(1000);
    }
}
