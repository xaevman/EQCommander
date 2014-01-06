# EQCommander
EQMac macro helper and damage parser

On 2013/11/18 SOE shut down the EQMac server (Al'Kabor) so this application is no longer very useful, but it was fun while it lasted :) For those who never got to experience it, the EQMac server was frozen in time after the Planes of Power expansion, though its lone developer - Hobart - was kind enough to implement bug fixes and some server-specific mechanics in the ensuing years. The server was known for its low population, so multi-boxing or multi-client play was the norm. There was something really fun about running a few clients on a macbook and furiously swiping between spaces to issue a few commands on each character at a time during battle. As a Guildwars player, it reminded me of a less polished version of heroes, and so I decided to attempt to write a helper application that would make the experience of running multiple clients in EQ more like that of running heroes in Guildwars.

### Partial feature list for  EQCommander
* Issue commands to characters or groups of characters by name.
* External macros with the ability to process runtime arguments. Not limited to EQ's basic macro lengths and syntax (though you could execute those macros too).
* Command scheduling and loops, to make playing bards possible. EQMac did not have some of the later bard helper macro elements that SOE added on PC later in EQ's life.
* Automatic damage parsing and statistics tracking. 
	Dps,Total, average, and max damage delt during parse
	Hit, miss, mob evaded % split
	Evasion %
	Estimated mitigation %
* A filtering system that keeps multiple players runningn EQCommander from accidently, or maliciously, issuing commands to other players' EQCommander instances. This same system can be used to allow another commander instance to temporarily issue commands to characters under your control. Running the group's cleric but need to afk? No problem, just 'trust' one of your buddies and they can take on healing duties while you go pay the pizza guy.

If this app was at all relevant anymore, I'd write some tuts and tips and tricks, but alas... it exists only for posterity now :)

