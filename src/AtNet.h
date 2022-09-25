//============================================================================================
// AtNet.h - Network messages in Airline Tycoon:
//============================================================================================

void NetGenericSync (long SyncId);
void NetGenericSync (long SyncId, long Par);
void NetGenericAsync (long SyncId, long Par, long player=-1);

//Messages for creating a new game
static const ULONG ATNET_WANNAJOIN          = 0xadaa0000;  //Server, I want to join, Please send list of players and their names
static const ULONG ATNET_SORRYFULL          = 0xadaa0001;  //Server: Sorry, you can't join: too many players
static const ULONG ATNET_PUSHNAMES          = 0xadaa0002;  //Server: Sending list of players and their names
static const ULONG ATNET_SELECTPLAYER       = 0xadaa0003;  //Server, I want to be player x
static const ULONG ATNET_UNSELECTPLAYER     = 0xadaa0004;  //Server, don't want to be player x any more
static const ULONG ATNET_ENTERNAME          = 0xadaa0005;  //Server, the new name of player x is y
static const ULONG ATNET_WANNALEAVE         = 0xadaa0006;  //Server, I don't like your group any more
static const ULONG ATNET_BEGINGAME          = 0xadaa0007;  //Server: Okay, start the game
static const ULONG ATNET_SAVGEGAMECHECK     = 0xadaa0008;  //Server: Join? Do you have the right savegame?
static const ULONG ATNET_WANNAJOIN2         = 0xadaa0009;  //Server, Yes, I have the right savegame. Please let me join now!
static const ULONG ATNET_BEGINGAMELOADING   = 0xadaa0010;  //Server: Okay, start the game by loading the savegame
static const ULONG ATNET_WANNAJOIN2NO       = 0xadaa0011;  //Server: Oops, now that I look at it: your savegame is not right! You're the wrong person. We can't accept you.
static const ULONG ATNET_SORRYVERSION       = 0xadaa0012;  //Server: You don't have the right version to join the game

//Messages for maintaining the game
static const ULONG ATNET_ALIVE              = 0xadaa0100;  //Hello all, I'm still with you
static const ULONG ATNET_PLAYERDROPOUT      = 0xadaa0101;  //Hello all, this player is no longer with us, he's now a computer player

//Day control
static const ULONG ATNET_DAYFINISH          = 0xadaa0200;  //Hello all, I'm going home
static const ULONG ATNET_DAYBACK            = 0xadaa0201;  //Hello all, I forgot something (I'm back on the same day)
static const ULONG ATNET_DAYFINISHALL       = 0xadaa0202;  //Server: All Players new day now
static const ULONG ATNET_READYFORMORNING    = 0xadaa0203;  //Hello all, I'm ready for the morning briefing
static const ULONG ATNET_READYFORBRIEFING   = 0xadaa0204;  //Hello all, I'm ready for the morning briefing

//Speed control:
static const ULONG ATNET_SETSPEED           = 0xadaa0300;  //Hello all, I want to have max speed x
static const ULONG ATNET_FORCESPEED         = 0xadaa0301;  //Hello all, Now ALL use speed x!
static const ULONG ATNET_TIMEPING           = 0xadaa0302;  //Hello all, it's now hh:mm:ss.

//Chatting:
static const ULONG ATNET_CHATMESSAGE        = 0xadaa0401;  //Hello x, <generic>
static const ULONG ATNET_CHATSTOP           = 0xadaa0402;  //Hello x, nice talking to you
static const ULONG ATNET_CHATBROADCAST      = 0xadaa0403;  //Hello x/y/z, <generic>
static const ULONG ATNET_CHATMONEY          = 0xadaa0404;  //Hey x, here's some cash

//Pausing and options menu:
static const ULONG ATNET_PAUSE              = 0xadaa0500;  //Hello all, I need a break / had enough break
static const ULONG ATNET_OPTIONS            = 0xadaa0501;  //Hello all, I'm entering / leaving options
static const ULONG ATNET_ACTIVATEAPP        = 0xadaa0502;  //Hello all, I'm (dis-)activated

//Player movement and control
static const ULONG ATNET_PLAYERPOS          = 0xadaa0600;  //Hello all, I'm at (x,y) and walking towards (tx,ty)
static const ULONG ATNET_ENTERROOM          = 0xadaa0601;  //Hello all, I just entered room xxx. Does anybody have a problem with that?
static const ULONG ATNET_ENTERROOMOK        = 0xadaa0602;  //Hello x, you can use the room
static const ULONG ATNET_ENTERROOMBAD       = 0xadaa0603;  //Hello x, the room was taken. Please leave.
static const ULONG ATNET_LEAVEROOM          = 0xadaa0604;  //Hello all, I just left room xxx.
static const ULONG ATNET_PLAYERSTOP         = 0xadaa0605;  //Hello all, I won't go another step
static const ULONG ATNET_CAFFEINE           = 0xadaa0606;  //Hello all, I just drank the drowning cow energy drink
static const ULONG ATNET_GIMMICK            = 0xadaa0607;  //Hello all, I just started my gimmick
static const ULONG ATNET_PLAYERLOOK         = 0xadaa0608;  //Hello all, please let player X look at direction y

//Cheating
static const ULONG ATNET_CHEAT              = 0xadaa0700;  //Hello all, player x used cheat y

//Dialogs between two players:
static const ULONG ATNET_DIALOG_REQUEST     = 0xadaa0800;  //Hello x, can we talk for a moment?
static const ULONG ATNET_DIALOG_NO          = 0xadaa0801;  //Hello y, not now, I'm just busy entering/leaving a room
static const ULONG ATNET_DIALOG_YES         = 0xadaa0802;  //Hello y, sure, let's talk
static const ULONG ATNET_DIALOG_SAY         = 0xadaa0803;  //Hey y, I choose Dialog Option n
static const ULONG ATNET_DIALOG_END         = 0xadaa0804;  //Hey y, I gotta go
static const ULONG ATNET_DIALOG_START       = 0xadaa0805;  //Hey y, I'm next to you now, display the dialog window
static const ULONG ATNET_DIALOG_TEXT        = 0xadaa0806;  //Hey y, I selected this text from a choice window
static const ULONG ATNET_DIALOG_NEXT        = 0xadaa0807;  //Hey y, I clicked on the dialog and it's your turn to say something
static const ULONG ATNET_DIALOG_DRUNK       = 0xadaa0808;  //Hey y, I just gave you alocohol
static const ULONG ATNET_DIALOG_LOCK        = 0xadaa0809;  //Hey all, I just started talking to the computer player x, so stop moving him
static const ULONG ATNET_DIALOG_UNLOCK      = 0xadaa0810;  //Hey all, I just stopped talking to the computer player x, you can move him once again
static const ULONG ATNET_DIALOG_KOOP        = 0xadaa0811;  //Hey all, we're cooperating like this ....
static const ULONG ATNET_DIALOG_DROPITEM    = 0xadaa0812;  //Hey y, please drop that item

//Dialog startup using telefone:
static const ULONG ATNET_PHONE_DIAL         = 0xadaa0900;  //Hey x, I'm dialing your number
static const ULONG ATNET_PHONE_ACCEPT       = 0xadaa0901;  //Yes y, I accept the call, let's start the dialog
static const ULONG ATNET_PHONE_BUSY         = 0xadaa0902;  //Sorry y, I'm currently talking/in a menu
static const ULONG ATNET_PHONE_NOTHOME      = 0xadaa0903;  //Sorry y, I'm not in my office or I have already gone home

//Synchronising of the players:
static const ULONG ATNET_SYNC_IMAGE         = 0xadaa1000;  //Synchronizes the images of the different players
static const ULONG ATNET_SYNC_MONEY         = 0xadaa1001;  //Synchronizes money, credit and shares
static const ULONG ATNET_SYNC_ROUTES        = 0xadaa1002;  //Synchronizes Routes, Rentroutes
static const ULONG ATNET_SYNC_FLAGS         = 0xadaa1003;  //Synchronizes Flags saying which Items are taken and so on
static const ULONG ATNET_SYNC_ITEMS         = 0xadaa1004;  //Synchronizes all items
static const ULONG ATNET_ADD_SYMPATHIE      = 0xadaa1005;  //Changes the sympathie of one robot for another person by a value
static const ULONG ATNET_ADD_EXPLOSION      = 0xadaa1006;  //Start an explosion on another computer
static const ULONG ATNET_SYNC_OFFICEFLAG    = 0xadaa1007;  //Synchronizes the office State flag
static const ULONG ATNET_SYNC_PLANES        = 0xadaa1008;  //Flugzeuge von einem Spieler komplett übertragen
static const ULONG ATNET_SYNC_MEETING       = 0xadaa1009;  //Die typischen Sachen vom Meeting synchronisieren

//Synchronizing the robots:
static const ULONG ATNET_ROBOT_EXECUTE      = 0xadaa1100;  //Hey all, robot x is gonna act at hh:mm:ss sharp

//Synchronizing the players:
static const ULONG ATNET_PLAYER_REFILL      = 0xadaa1200;  //Hey all, please refill lastminute/freight/...
static const ULONG ATNET_PLAYER_TOOK        = 0xadaa1201;  //Hey all, I just took this lastminute/freight/... order
static const ULONG ATNET_PLAYER_18UHR       = 0xadaa1202;  //18 Uhr, ende der Synchronisation

//Updating foreign flight plans and related things:
static const ULONG ATNET_FP_UPDATE          = 0xadaa1300;  //Broadcasts all flights of one plane when a human player updates his flighplan
static const ULONG ATNET_TAKE_ORDER         = 0xadaa1301;  //Hey all, I just took this order flight
static const ULONG ATNET_TAKE_FREIGHT       = 0xadaa1302;  //Hey all, I just took this freight order flight
static const ULONG ATNET_TAKE_CITY          = 0xadaa1303;  //Hey all, I just bid for a city or a gate
static const ULONG ATNET_TAKE_ROUTE         = 0xadaa1304;  //Hey all, I just rented or dropped a route
static const ULONG ATNET_ADVISOR            = 0xadaa1305;  //Hey all, I have just <generic>, maybe your advisors should display that
static const ULONG ATNET_BUY_USED           = 0xadaa1306;  //Hey all, please mark the used plane x as sold
static const ULONG ATNET_SELL_USED          = 0xadaa1307;  //Hey all, I just sold a plane of mine
static const ULONG ATNET_BUY_NEW            = 0xadaa1308;  //Hey all, I just bought x new planes
static const ULONG ATNET_PERSONNEL          = 0xadaa1309;  //Hey all, the people on my planes are like this
static const ULONG ATNET_PLANEPROPS         = 0xadaa1310;  //Hey all, I just changed my plane like this
static const ULONG ATNET_PLANEPERSONNEL     = 0xadaa1311;  //Hey all, I mapped my personnel like this
static const ULONG ATNET_BUY_NEWX           = 0xadaa1312;  //Hey all, I just bought x new xplanes

//Sabotage:
static const ULONG ATNET_SABOTAGE_ARAB      = 0xadaa1400;  //Hello all, I just ordered a sabotage from ArabAir
static const ULONG ATNET_SABOTAGE_DIRECT    = 0xadaa1401;  //Hello all, I just personally sabotaged

//Miscellaneous:
static const ULONG ATNET_EXPAND_AIRPORT     = 0xadaa1500;  //Hello all, I just let the Airport be expanded
static const ULONG ATNET_OVERTAKE           = 0xadaa1501;  //Hello all, I just bought company x
static const ULONG ATNET_WAITFORPLAYER      = 0xadaa1502;  //Hello all, please wait for me / don't wait for me
static const ULONG ATNET_TAKETHING          = 0xadaa1503;  //Hello all, I just took some object

//Load/Save:
static const ULONG ATNET_IO_SAVE            = 0xadaa1600;  //Hello all, Let's Save the game
static const ULONG ATNET_IO_LOADREQUEST     = 0xadaa1601;  //Hello all, do we all have Savegame x?
static const ULONG ATNET_IO_LOADREQUEST_OK  = 0xadaa1602;  //Hello x, yeah Savegame x will be fine
static const ULONG ATNET_IO_LOADREQUEST_BAD = 0xadaa1603;  //Hello x, no I don't have Savegame x
static const ULONG ATNET_IO_LOADREQUEST_DOIT= 0xadaa1604;  //Hello all, we can load now

//Testing & Debugging:
static const ULONG ATNET_CHECKRANDS         = 0xadaa1700;  //Hello all, let's compare our randoms from the last round time
static const ULONG ATNET_GENERICSYNC        = 0xadaa1701;  //Hello all, let's get in sync for a nonspecified reason
static const ULONG ATNET_GENERICSYNCX       = 0xadaa1702;  //Hello all, let's get in sync for a nonspecified reason
static const ULONG ATNET_GENERICASYNC       = 0xadaa1703;  //Hello all, let's get in sync for a nonspecified reason

//Weitere Synchronisierungen:
static const ULONG ATNET_BODYGUARD          = 0xadaa1800;  //Hello all, I got Bodyguard Rebate
static const ULONG ATNET_CHANGEMONEY        = 0xadaa1801;  //Hello all, I changed money due to something
static const ULONG ATNET_SYNCKEROSIN        = 0xadaa1802;  //Hello all, I here's my kerosine state
static const ULONG ATNET_SYNCGEHALT         = 0xadaa1803;  //Hello all, I'm paying this much for workers
static const ULONG ATNET_SYNCNUMFLUEGE      = 0xadaa1804;  //Hello all, I've accepted this many flights
static const ULONG ATNET_SYNCROUTECHANGE    = 0xadaa1805;  //Hello all, I've just change my route paramters

START_NAME_MAP(ATNET)
DEFINE_NAME_ENTRY(ATNET_WANNAJOIN,)
DEFINE_NAME_ENTRY(ATNET_SORRYFULL,)
DEFINE_NAME_ENTRY(ATNET_PUSHNAMES,)
DEFINE_NAME_ENTRY(ATNET_SELECTPLAYER,)
DEFINE_NAME_ENTRY(ATNET_UNSELECTPLAYER,)
DEFINE_NAME_ENTRY(ATNET_ENTERNAME,)
DEFINE_NAME_ENTRY(ATNET_WANNALEAVE,)
DEFINE_NAME_ENTRY(ATNET_BEGINGAME,)
DEFINE_NAME_ENTRY(ATNET_SAVGEGAMECHECK,)
DEFINE_NAME_ENTRY(ATNET_WANNAJOIN2,)
DEFINE_NAME_ENTRY(ATNET_BEGINGAMELOADING,)
DEFINE_NAME_ENTRY(ATNET_WANNAJOIN2NO,)
DEFINE_NAME_ENTRY(ATNET_SORRYVERSION,)
DEFINE_NAME_ENTRY(ATNET_ALIVE,)
DEFINE_NAME_ENTRY(ATNET_PLAYERDROPOUT,)
DEFINE_NAME_ENTRY(ATNET_DAYFINISH,)
DEFINE_NAME_ENTRY(ATNET_DAYBACK,)
DEFINE_NAME_ENTRY(ATNET_DAYFINISHALL,)
DEFINE_NAME_ENTRY(ATNET_READYFORMORNING,)
DEFINE_NAME_ENTRY(ATNET_READYFORBRIEFING,)
DEFINE_NAME_ENTRY(ATNET_SETSPEED,)
DEFINE_NAME_ENTRY(ATNET_FORCESPEED,)
DEFINE_NAME_ENTRY(ATNET_TIMEPING,)
DEFINE_NAME_ENTRY(ATNET_CHATMESSAGE,)
DEFINE_NAME_ENTRY(ATNET_CHATSTOP,)
DEFINE_NAME_ENTRY(ATNET_CHATBROADCAST,)
DEFINE_NAME_ENTRY(ATNET_CHATMONEY,)
DEFINE_NAME_ENTRY(ATNET_PAUSE,)
DEFINE_NAME_ENTRY(ATNET_OPTIONS,)
DEFINE_NAME_ENTRY(ATNET_ACTIVATEAPP,)
DEFINE_NAME_ENTRY(ATNET_PLAYERPOS,)
DEFINE_NAME_ENTRY(ATNET_ENTERROOM,)
DEFINE_NAME_ENTRY(ATNET_ENTERROOMOK,)
DEFINE_NAME_ENTRY(ATNET_ENTERROOMBAD,)
DEFINE_NAME_ENTRY(ATNET_LEAVEROOM,)
DEFINE_NAME_ENTRY(ATNET_PLAYERSTOP,)
DEFINE_NAME_ENTRY(ATNET_CAFFEINE,)
DEFINE_NAME_ENTRY(ATNET_GIMMICK,)
DEFINE_NAME_ENTRY(ATNET_PLAYERLOOK,)
DEFINE_NAME_ENTRY(ATNET_CHEAT,)
DEFINE_NAME_ENTRY(ATNET_DIALOG_REQUEST,)
DEFINE_NAME_ENTRY(ATNET_DIALOG_NO,)
DEFINE_NAME_ENTRY(ATNET_DIALOG_YES,)
DEFINE_NAME_ENTRY(ATNET_DIALOG_SAY,)
DEFINE_NAME_ENTRY(ATNET_DIALOG_END,)
DEFINE_NAME_ENTRY(ATNET_DIALOG_START,)
DEFINE_NAME_ENTRY(ATNET_DIALOG_TEXT,)
DEFINE_NAME_ENTRY(ATNET_DIALOG_NEXT,)
DEFINE_NAME_ENTRY(ATNET_DIALOG_DRUNK,)
DEFINE_NAME_ENTRY(ATNET_DIALOG_LOCK,)
DEFINE_NAME_ENTRY(ATNET_DIALOG_UNLOCK,)
DEFINE_NAME_ENTRY(ATNET_DIALOG_KOOP,)
DEFINE_NAME_ENTRY(ATNET_DIALOG_DROPITEM,)
DEFINE_NAME_ENTRY(ATNET_PHONE_DIAL,)
DEFINE_NAME_ENTRY(ATNET_PHONE_ACCEPT,)
DEFINE_NAME_ENTRY(ATNET_PHONE_BUSY,)
DEFINE_NAME_ENTRY(ATNET_PHONE_NOTHOME,)
DEFINE_NAME_ENTRY(ATNET_SYNC_IMAGE,)
DEFINE_NAME_ENTRY(ATNET_SYNC_MONEY,)
DEFINE_NAME_ENTRY(ATNET_SYNC_ROUTES,)
DEFINE_NAME_ENTRY(ATNET_SYNC_FLAGS,)
DEFINE_NAME_ENTRY(ATNET_SYNC_ITEMS,)
DEFINE_NAME_ENTRY(ATNET_ADD_SYMPATHIE,)
DEFINE_NAME_ENTRY(ATNET_ADD_EXPLOSION,)
DEFINE_NAME_ENTRY(ATNET_SYNC_OFFICEFLAG,)
DEFINE_NAME_ENTRY(ATNET_SYNC_PLANES,)
DEFINE_NAME_ENTRY(ATNET_SYNC_MEETING,)
DEFINE_NAME_ENTRY(ATNET_ROBOT_EXECUTE,)
DEFINE_NAME_ENTRY(ATNET_PLAYER_REFILL,)
DEFINE_NAME_ENTRY(ATNET_PLAYER_TOOK,)
DEFINE_NAME_ENTRY(ATNET_PLAYER_18UHR,)
DEFINE_NAME_ENTRY(ATNET_FP_UPDATE,)
DEFINE_NAME_ENTRY(ATNET_TAKE_ORDER,)
DEFINE_NAME_ENTRY(ATNET_TAKE_FREIGHT,)
DEFINE_NAME_ENTRY(ATNET_TAKE_CITY,)
DEFINE_NAME_ENTRY(ATNET_TAKE_ROUTE,)
DEFINE_NAME_ENTRY(ATNET_ADVISOR,)
DEFINE_NAME_ENTRY(ATNET_BUY_USED,)
DEFINE_NAME_ENTRY(ATNET_SELL_USED,)
DEFINE_NAME_ENTRY(ATNET_BUY_NEW,)
DEFINE_NAME_ENTRY(ATNET_PERSONNEL,)
DEFINE_NAME_ENTRY(ATNET_PLANEPROPS,)
DEFINE_NAME_ENTRY(ATNET_PLANEPERSONNEL,)
DEFINE_NAME_ENTRY(ATNET_BUY_NEWX,)
DEFINE_NAME_ENTRY(ATNET_SABOTAGE_ARAB,)
DEFINE_NAME_ENTRY(ATNET_SABOTAGE_DIRECT,)
DEFINE_NAME_ENTRY(ATNET_EXPAND_AIRPORT,)
DEFINE_NAME_ENTRY(ATNET_OVERTAKE,)
DEFINE_NAME_ENTRY(ATNET_WAITFORPLAYER,)
DEFINE_NAME_ENTRY(ATNET_TAKETHING,)
DEFINE_NAME_ENTRY(ATNET_IO_SAVE,)
DEFINE_NAME_ENTRY(ATNET_IO_LOADREQUEST,)
DEFINE_NAME_ENTRY(ATNET_IO_LOADREQUEST_OK,)
DEFINE_NAME_ENTRY(ATNET_IO_LOADREQUEST_BAD,)
DEFINE_NAME_ENTRY(ATNET_IO_LOADREQUEST_DOIT,)
DEFINE_NAME_ENTRY(ATNET_CHECKRANDS,)
DEFINE_NAME_ENTRY(ATNET_GENERICSYNC,)
DEFINE_NAME_ENTRY(ATNET_GENERICSYNCX,)
DEFINE_NAME_ENTRY(ATNET_GENERICASYNC,)
DEFINE_NAME_ENTRY(ATNET_BODYGUARD,"I got Bodyguard Rebate")
DEFINE_NAME_ENTRY(ATNET_CHANGEMONEY, "I changed money due to something")
DEFINE_NAME_ENTRY(ATNET_SYNCKEROSIN, "I here's my kerosine state")
DEFINE_NAME_ENTRY(ATNET_SYNCGEHALT, "I'm paying this much for workers")
DEFINE_NAME_ENTRY(ATNET_SYNCNUMFLUEGE, "I've accepted this many flights")
DEFINE_NAME_ENTRY(ATNET_SYNCROUTECHANGE, "I've just change my route paramters")
END_NAME_MAP
