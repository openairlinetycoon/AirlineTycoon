//------------------------------------------------------------------------------------
//Verschiendene CNetConn.CommStates für das Spiel:
#define CS_EMPTY    0 //Keine Netzwerkverbindung aktiv
#define CS_APPLYING 1 //Spieler ist drin und wartet ob er bleiben darf
#define CS_JOINED   2 //Spieler ist drin; es gab keinen Ressourcekonflikt
#define CS_REFUSED  3 //Spieler wurde nicht akzeptiert, da Session schon läuft
#define CS_LOST     4 //Spieler war drin; Verbindung ist abgerissen
#define CS_INGAME   5 //Spiel wurde gestartet

//------------------------------------------------------------------------------------
//Verschiendene CNetConn.GameStates für das Spiel:
#define GS_EMPTY          0 //Noch nicht verwendet
#define GS_STARTING       1 //Spiel wird hochgefahren
#define GS_INGAME         2 //Spiel läuft

//------------------------------------------------------------------------------------
//Hier sind die Messages zur Kommunikation mit den anderen Teilnehmern
//10xx Auf- und Abbau der Verbindung
#define MPM_PLEASE_BROADCAST      1000 //Ein neuer Spieler will infos
#define MPM_APPLYING              1001 //Spieler ist MPS_JOINING und will volles Mitglied werden
#define MPM_PLAYER_START_GAME     1002 //Spieler ist bereit zum starten
#define MPM_PLAYER_CANCEL_START   1003 //Spieler ist doch nicht mehr bereit zum starten
#define MPM_BROADCAST_PLAYERS     1004 //Server schickt Liste der Mitspieler
#define MPM_BROADCAST_PLAYING     1005 //Server schickt keine Liste, sondern sagt "Sorry, Game in progress"
#define MPM_CLIENT_LEAVING        1006 //Ein Client verabschiedet sich
#define MPM_SERVER_LEAVING        1007 //Der Server sagt "Ciao!"

//11xx Der tägliche Spielbeginn durch den Server
#define MPM_DAY_STARTUP           1100 //Basisdaten werden verschickt
#define MPM_DAY_CLIENT_READY      1101 //Dieser Client wäre dann so weit
#define MPM_DAY_START_NOW         1102 //Server sagt: "Jetzt geht das Spiel los"

//12xx Diverses
#define MPM_SEND_TEXT             1200 //Schickt einen Textkommentar an alle (Chat)

//20xx Time-slices
#define MPM_TIMESLICE             2000 //Daten über Spieler-Positionen, Raumbelegungen und ankommende Personen und Flugzeuge
