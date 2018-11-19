// main.cpp : Defines the entry point for the console application.
//

#include "deps.h" //only need CL_ReadDemoMessage from this?
#include "demo_utils.h"
#include "demo_common.h"
#include "utils.h"
#include "q_string.h" //added

#ifdef WIN32
#include <io.h>
#include <fcntl.h>
#endif

#define LOOPSTARTS 0
#define	SKIP_STILL 0
#define METADATA 0
#define SPEEDOMETER 0
#define CHATLOG 0
#define TRAIL_BUFSIZE 8192 //Write to file in 8kb chunks

#if CHATLOG
#define	OUTPUT_EXTENSION	"log"
#elif SPEEDOMETER == 2
#define	OUTPUT_EXTENSION	"srt"
#elif SPEEDOMETER == 1
#define	OUTPUT_EXTENSION	"txt"
#else
#define	OUTPUT_EXTENSION	"cfg"
#endif

void TimeToString(int duration_ms, char *timeStr, size_t strSize) {
	int hours, minutes, seconds, milliseconds;
	hours = (int)((duration_ms / (1000 * 60 * 60)) % 24); //wait wut
	minutes = (int)((duration_ms / (1000 * 60)) % 60);
	seconds = (int)(duration_ms / 1000) % 60;
	milliseconds = duration_ms % 1000;
	Com_sprintf(timeStr, strSize, "%02i:%02i:%02i,%03i", hours, minutes, seconds, milliseconds);
}

#if CHATLOG
#define MAX_STRINGED_SV_STRING 1024
char *RemoveStupidSquareSymbol(char *string) {
	char*	d;
	char*	s;
	int		c;

	s = string;
	d = string;
	while ((c = *s) != 0) {
		if (c != 0x19) {
			*d++ = c;
		}
		s++;
	}
	*d = '\0';

	return string;
}

const char * stringRefs[][2] = { 
	{"MP_SVGAME_PLRENAME","renamed to"},
	{"MP_SVGAME_PLCONNECT","connected"},
	{"MP_SVGAME_PLENTER","entered the game"},
	{"MP_SVGAME_NOCHEATS","Cheats are not enabled on this server."},
	{"MP_SVGAME_MUSTBEALIVE","You must be alive to use this command."},
	{"MP_SVGAME_PRINTBLUETEAM","Blue team"},
	{"MP_SVGAME_PRINTREDTEAM","Red team"},
	{"MP_SVGAME_PRINTFREETEAM","Free team"},
	{"MP_SVGAME_PRINTSPECTEAM","Spectator team"},
	{"MP_SVGAME_NOSWITCH","May not switch teams more than once per 5 seconds."},
	{"MP_SVGAME_NOVOTE","Voting not allowed here."},
	{"MP_SVGAME_VOTEINPROGRESS","A vote is already in progress."},
	{"MP_SVGAME_MAXVOTES","You have called the maximum number of votes."},
	{"MP_SVGAME_NOSPECVOTE","Not allowed to call a vote as a spectator."},
	{"MP_SVGAME_PLCALLEDVOTE","called a vote"},
	{"MP_SVGAME_NOVOTEINPROG","No vote in progress."},
	{"MP_SVGAME_VOTEALREADY","Vote already cast."},
	{"MP_SVGAME_NOVOTEASSPEC","Not allowed to vote as spectator."},
	{"MP_SVGAME_TEAMVOTEALREADY","A team vote already in progress."},
	{"MP_SVGAME_MAXTEAMVOTES","You have called the maximum number of team votes."},
	{"MP_SVGAME_NOTEAMVOTEINPROG","No team vote in progress."},
	{"MP_SVGAME_TEAMVOTEALREADYCAST","Team vote already cast."},
	{"MP_SVGAME_VOTEFAILED","Vote failed."},
	{"MP_SVGAME_VOTEPASSED","Vote passed."},
	{"MP_SVGAME_FORCEPOWERCHANGED","Changes to your force power settings will take effect the next time you respawn."},
	{"MP_SVGAME_TEAMVOTEFAILED","Team vote failed."},
	{"MP_SVGAME_TEAMVOTEPASSED","Team vote passed."},
	{"MP_SVGAME_PLVOTECAST","Vote cast."},
	{"MP_SVGAME_PLTEAMVOTECAST","Team vote cast."},
	{"MP_SVGAME_NEWTEAMLEADER","is the new team leader."},
	{"MP_SVGAME_PLDUELCHALLENGE","has challenged you to a duel!"},
	{"MP_SVGAME_PLDUELACCEPT","has become engaged in a duel with"},
	{"MP_SVGAME_PLDUELWINNER","has defeated"},
	{"MP_SVGAME_PLDUELSTOP","The duel has been severed."},
	{"MP_SVGAME_JOINEDTHEBATTLE","joined the battle."},
	{"MP_SVGAME_JOINEDTHESPECTATORS","joined the spectators."},
	{"MP_SVGAME_JOINEDTHEREDTEAM","joined the red team."},
	{"MP_SVGAME_JOINEDTHEBLUETEAM","joined the blue team."},
	{"MP_SVGAME_VERSUS","vs."},
	{"MP_SVGAME_ATTEMPTDUELKILL","You may not end yourself in the midst of a duel."},
	{"MP_SVGAME_CANTDUEL_JUSTDID","You must wait 10 seconds after finishing a duel to initiate another duel, or have someone else challenge you."},
	{"MP_SVGAME_CANTDUEL_BUSY","You cannot initiate a duel until the other two players have finished their duel."},
	{"MP_SVGAME_FORCEAPPLIED","Your new force power settings have been applied."},
	{"MP_SVGAME_MUSTBELIGHT","You must be on the Light side of the force to join the Blue Team."},
	{"MP_SVGAME_MUSTBEDARK","You must be on the Dark side of the force to join the Red Team."},
	{"MP_SVGAME_TOOMANYRED_SWITCH","Red team has too many players, switch to the light side of the force and join blue."},
	{"MP_SVGAME_TOOMANYRED","Red team has too many players."},
	{"MP_SVGAME_TOOMANYBLUE_SWITCH","Blue team has too many players, switch to the dark side of the force and join red."},
	{"MP_SVGAME_TOOMANYBLUE","Blue team has too many players."},
	{"MP_SVGAME_PLDUELTIE","The duel is ended in a draw, both fighters have died!"},
	{"MP_SVGAME_PLDUELCHALLENGED","You have challenged"},
	{"MP_SVGAME_NODUEL_GAMETYPE","This gametype does not support saber dueling."},
	{"MP_SVGAME_BECOMEJM","has become the Jedi Master!"},
	{"MP_SVGAME_UNABLE_TO_ADD_BOT","Unable to add bot. All player slots are in use.Start server with more 'open' slots."},
	{"MP_SVGAME_CANNOT_KICK_HOST","Cannot kick host player."},
	{"MP_SVGAME_NOVOTE_MAPNOTSUPPORTEDBYGAME","You can't vote for this map, it isn't supported by the current gametype."},
	{"MP_SVGAME_WAS_KICKED","was kicked."},
	{"MP_SVGAME_BEGIN_DUEL","BEGIN"},
	{"MP_SVGAME_HIT_THE_KILL_LIMIT","hit the kill limit."},
	{"MP_SVGAME_SERVER_IS_FULL","Server is full."},
	{"MP_SVGAME_SERVER_DISCONNECTED","Server disconnected"},
	{"MP_SVGAME_SERVER_CONNECTION_TIMED_OUT","Server connection timed out."},
	{"MP_SVGAME_INVALID_PASSWORD","Invalid password"},
	{"MP_SVGAME_TIMELIMIT_HIT","Timelimit hit"},
	{"MP_SVGAME_ONLY_ADD_BOTS_AS_SERVER","You can only add bots as the server"},
	{"MP_SVGAME_DISCONNECTED","disconnected"},
	{"MP_SVGAME_NOCLASSSWITCH","You can only switch classes once every 5 seconds."},
	{"MP_SVGAME_NOCLASSTEAM","The class you've tried to select does not belong to the team you're on."},
	{"MP_SVGAME_NOVOICECHATASSPEC","You cannot voice chat as a spectator."},
	{"MP_SVGAME_DUELMORESINGLE","Need another single duelist"},
	{"MP_SVGAME_DUELMOREPAIRED","Need more paired duelists"},
	{"MP_SVGAME_HIT_CAPTURE_LIMIT","hit the capture limit."},
	{"MP_SVGAME_INVALID_ESCAPE_TO_MAIN","Invalid Password.  Press ESCAPE to return to Main Menu"},
	{"MP_SVGAME_CANCEL_PASSWORD","Close menu, do not attempt to connect"},
	{"MP_SVGAME_CANNOT_TASK_INTERMISSION","You cannot perform this task during the intermission."},
	{"MP_SVGAME_SAY","Say:"},
	{"MP_SVGAME_SAY_TEAM","Say Team:"},
	{"MP_SVGAME_SNAPSHOT_TIMEOUT","Timeout waiting for initial snapshot"},
	{"MP_SVGAME_CLIENT_CONN_WITH_PING","Client %i connecting with %i challenge ping"},
	{"MP_SVGAME_SERVER_FOR_HIGH_PING","Server is for high pings only"},
	{"MP_SVGAME_CLIENT_REJECTED_LOW_PING","Client %i rejected on a too low ping"},
	{"MP_SVGAME_SERVER_FOR_LOW_PING","Server is for low pings only"},
	{"MP_SVGAME_CLIENT_REJECTED_HIGH_PING","Client %i rejected on a too high ping"},
	{"MP_SVGAME_NONAMECHANGE","You must wait 5 seconds before changing your name again."},
	{"MP_SVGAME_TOO_MANY_INFO","Warning: Too many info changes, last info ignored"}
};
static const int MAX_STRING_REFS = ARRAY_LEN(stringRefs);

const char *SE_GetString( const char *psPackageAndStringReference ) {
	int i;
	char sReference[256];
	assert(strlen(psPackageAndStringReference) < sizeof(sReference) );
	Q_strncpyz(sReference, psPackageAndStringReference, sizeof(sReference) );
	Q_strupr(sReference);

	for (i=0; i<MAX_STRING_REFS; i++) {
		if (!Q_stricmp(sReference, stringRefs[i][0]))
			return stringRefs[i][1];
	}
	return psPackageAndStringReference;
}

static qboolean CL_SE_GetStringTextString( const char *text, char *buffer, int bufferLength ) {
	const char *str;

	assert( text && buffer );

	str = SE_GetString( text );

	if ( str[0] ) {
		Q_strncpyz( buffer, str, bufferLength );
		return qtrue;
	}

	Com_sprintf( buffer, bufferLength, "??%s", str );
	return qfalse;
}

const char *CG_GetStringEdString(char *refSection, char *refName) {
	static char text[2][1024];	//just incase it's nested
	static int		index = 0;

	index ^= 1;
	CL_SE_GetStringTextString(va("%s_%s", refSection, refName), text[index], sizeof(text[0]));
	//trap->SE_GetStringTextString(va("%s_%s", refSection, refName), text[index], sizeof(text[0]));
	return text[index];
}

void CG_CheckSVStringEdRef(char *buf, const char *str) { //I don't really like doing this. But it utilizes the system that was already in place.
	int i = 0;
	int b = 0;
	int strLen = 0;
	qboolean gotStrip = qfalse;

	if (!str || !str[0])
	{
		if (str)
		{
			strcpy(buf, str);
		}
		return;
	}

	strcpy(buf, str);

	strLen = strlen(str);

	if (strLen >= MAX_STRINGED_SV_STRING)
	{
		return;
	}

	while (i < strLen && str[i])
	{
		gotStrip = qfalse;

		if (str[i] == '@' && (i+1) < strLen)
		{
			if (str[i+1] == '@' && (i+2) < strLen)
			{
				if (str[i+2] == '@' && (i+3) < strLen)
				{ //@@@ should mean to insert a StringEd reference here, so insert it into buf at the current place
					char stringRef[MAX_STRINGED_SV_STRING];
					int r = 0;

					while (i < strLen && str[i] == '@')
					{
						i++;
					}

					while (i < strLen && str[i] && str[i] != ' ' && str[i] != ':' && str[i] != '.' && str[i] != '\n')
					{
						stringRef[r] = str[i];
						r++;
						i++;
					}
					stringRef[r] = 0;

					buf[b] = 0;
					Q_strcat(buf, MAX_STRINGED_SV_STRING, CG_GetStringEdString("MP_SVGAME", stringRef));
					b = strlen(buf);
				}
			}
		}

		if (!gotStrip)
		{
			buf[b] = str[i];
			b++;
		}
		i++;
	}

	buf[b] = 0;
}
#endif

int main(int argc, char** argv)
{
	char *demoFileName = NULL;
	char *trailFileName = NULL;

	if (argc == 2) {//Input file specified, use same name for output file 
		char *input = argv[1];

		if (COM_CompareExtension(input, ".dm_26")) { //Possible drag/drop, check extension and strip
			char temp[256]; //wtf - max path?
			COM_StripExtension(input, temp, sizeof(temp));
			demoFileName = va("%s", input);
			trailFileName = va("%s.%s", temp, OUTPUT_EXTENSION);
		}
		else { //if no extension -fixme
			demoFileName = va("%s.dm_26", input);
			trailFileName = va("%s.%s", input, OUTPUT_EXTENSION);
		}
	}
	else if (argc == 3) { //Input and output file specified
		char *input = argv[1];
		char *output = argv[2];

		if (COM_CompareExtension(input, ".dm_26")) {
			demoFileName = va("%s", input);
		}
		else { //if no extension -fixme
			demoFileName = va("%s.dm_26", input);
		}

		if (COM_CompareExtension(output, ".dm_26")) {
			char temp[256]; //wtf - max path?
			COM_StripExtension(output, temp, sizeof(temp));
			trailFileName = va("%s.%s", temp, OUTPUT_EXTENSION);
		}
		else {
			trailFileName = va("%s.%s", input, OUTPUT_EXTENSION);
		}
	}
	else {
		printf("No file specified.\n"
			"Usage: \"%s\" filename outfile (optional)\n", argv[0]);
		return -1;
	}

	printf("Input file: %s Output file: %s\n", demoFileName, trailFileName);

	fileHandle_t fp;
	FS_FOpenFileRead(demoFileName, &fp, qfalse );
	if ( !fp ) {
		printf( "File %s not found.\n", demoFileName);
		return -1;
	}

	FILE *trailFile = fopen(trailFileName, "wb" );
	if ( !trailFile) {
		printf( "Couldn't open output file\n" );
		return -1;
	}

	char buf[TRAIL_BUFSIZE] = { 0 };
	qboolean demoFinished = qfalse;
	qboolean dontReset = qfalse;
	int lastStartTime = 0;
	int lastRaceTimer = 0;
#if SKIP_STILL
	vec3_t lastOrigin = {0};
#endif
#if SPEEDOMETER == 2
	int frameCount = 0;
#endif
	qboolean gotInfo = qfalse;

	while ( !demoFinished ) {
		msg_t msg;
		byte msgData[ MAX_MSGLEN ];
		MSG_Init( &msg, msgData, sizeof( msgData ) );
		demoFinished = CL_ReadDemoMessage( fp, &msg ) ? qfalse : qtrue;

		if ( demoFinished ) {
			break;
		}
		try {
			CL_ParseServerMessage( &msg );
		} catch ( int ) { // thrown code means it wasn't a fatal error, so we can still dump what we had
			break;
		}

		if ( !ctx->cl.newSnapshots ) {
			continue;
		}

#if CHATLOG //write detected sv_fps to first line of file
		int i;
		if (!gotInfo)
		{
			const char *info = ctx->cl.gameState.stringData + ctx->cl.gameState.stringOffsets[CS_SERVERINFO];
			char metadata[1024] = {0};

			Q_strncpyz(metadata, va("DEMO HOSTNAME: %s\n", (Info_ValueForKey(info, "sv_hostname"))), sizeof(metadata));
			Q_strcat(metadata, sizeof(metadata), va("DEMO MODNAME: %s\n", (Info_ValueForKey(info, "gamename"))));
			Q_strcat(metadata, sizeof(metadata), va("DEMO MAPNAME: %s\n", (Info_ValueForKey(info, "mapname"))));
			Q_strcat(metadata, sizeof(metadata), "----------------------------------\n");
			fwrite(metadata, strlen(metadata), 1, trailFile);
			gotInfo = qtrue;
		}
#endif

		int firstServerCommand = ctx->clc.lastExecutedServerCommand;
		for ( ; ctx->clc.lastExecutedServerCommand < ctx->clc.serverCommandSequence; ctx->clc.lastExecutedServerCommand++ ) { // process any new server commands
			char *command = ctx->clc.serverCommands[ ctx->clc.lastExecutedServerCommand & ( MAX_RELIABLE_COMMANDS - 1 ) ];
			Cmd_TokenizeString( command );
			char *cmd = Cmd_Argv( 0 );
			if ( cmd[0] ) {
				firstServerCommand = ctx->clc.lastExecutedServerCommand;
#if CHATLOG
				int command = 0;

				if (!strcmp(cmd, "chat"))
					command = 1;
				else if (!strcmp(cmd, "tchat"))
					command = 2;
				else if (!strcmp(cmd, "print"))
					command = 3;

				if (command) {
					char *tmpMsg = NULL;
					char *cmd2 = Cmd_Argv( 1 );

					RemoveStupidSquareSymbol(cmd2);

					if (command == 1 || command == 2) {
						tmpMsg = va("^0%i^7: %s\n", i, cmd2);
						i++;
					}
					else if (command == 3) {
						char strEd[MAX_STRINGED_SV_STRING] = {0};

						CG_CheckSVStringEdRef( strEd, cmd2 );
						tmpMsg = va("%s", strEd);
					}

					if (strlen(buf) + strlen(tmpMsg) >= sizeof(buf)) { //Write to file
						fwrite(buf, strlen(buf), 1, trailFile);
						Q_strncpyz(buf, "", sizeof(buf)); //buf[0] = '\0'; ?
					}
					Q_strcat(buf, sizeof(buf), tmpMsg);
				}
#endif
			}
		}

#if !CHATLOG
	{
	#if METADATA //write detected sv_fps to first line of file
			if (!gotInfo)
			{
				const char *info = ctx->cl.gameState.stringData + ctx->cl.gameState.stringOffsets[CS_SERVERINFO];
				char fpsInfo[32] = {0};
				int svfps = atoi(Info_ValueForKey(info, "sv_fps"));

				Q_strncpyz(fpsInfo, va("sv_fps %i\n", svfps), sizeof(fpsInfo));
				fwrite(fpsInfo, strlen(fpsInfo), 1, trailFile);
				gotInfo = qtrue;
			}
	#endif

			//If we have a timer now, and we didnt previously, reset file (this removes resets)
			//If we have a timer now, and its less than our previous timer, reset file (this removes loopstarts)
			//If current origin is same as previous... don't write anything...(possibly fucks up timing, but never happens in practice)

			int startTime = ctx->cl.snap.ps.duelTime;
			int raceTimer = 0;
				
			if (startTime)
				raceTimer = ctx->cl.snap.serverTime - startTime;

			if (raceTimer > 6000) //Dont clear file if we have gone more than 6sec into run, yikes.
				dontReset = qtrue;

			if (startTime && !dontReset) {
	#if LOOPSTARTS
				if (!lastStartTime)
	#else
				if (!lastStartTime || (raceTimer < lastRaceTimer))
	#endif
				{
					Q_strncpyz(buf, "", sizeof(buf));//Also clear entire file.. how w/o crash?
				}
			}

			if (startTime) { //Only write if they are in a race
	#if SKIP_STILL
				if (!VectorCompare(ctx->cl.snap.ps.origin, lastOrigin)) //Dont write if they are standing still
	#endif
				{
					char *tmpMsg = NULL;
	#if SPEEDOMETER == 2
					{
					char time1[32], time2[32];
					int speed = (int)(sqrt(ctx->cl.snap.ps.velocity[0] * ctx->cl.snap.ps.velocity[0] + ctx->cl.snap.ps.velocity[1] * ctx->cl.snap.ps.velocity[1]) + 0.5f);

					TimeToString(frameCount*25, time1, sizeof(time1));
					TimeToString((frameCount+1)*25, time2, sizeof(time1));

					tmpMsg = va("%i\n%s --> %s\n%i\n\n", frameCount+1, time1, time2, speed);
					frameCount++;
					}
	#elif SPEEDOMETER == 1
					tmpMsg = va("%i\n", (int)(sqrt(ctx->cl.snap.ps.velocity[0] * ctx->cl.snap.ps.velocity[0] + ctx->cl.snap.ps.velocity[1] * ctx->cl.snap.ps.velocity[1])+0.5f));
	#else
					tmpMsg = va("%i %i %i\n", (int)(ctx->cl.snap.ps.origin[0] + 0.5f), (int)(ctx->cl.snap.ps.origin[1] + 0.5f), (int)(ctx->cl.snap.ps.origin[2] + 0.5f));
	#endif
					if (strlen(buf) + strlen(tmpMsg) >= sizeof(buf)) { //Write to file
						fwrite(buf, strlen(buf), 1, trailFile);
						Q_strncpyz(buf, "", sizeof(buf)); //buf[0] = '\0'; ?
					}
					Q_strcat(buf, sizeof(buf), tmpMsg);
	#if SKIP_STILL
					VectorCopy(ctx->cl.snap.ps.origin, lastOrigin);
	#endif
				}
			}

			lastStartTime = startTime;
			lastRaceTimer = raceTimer;
		}
	#endif
	}

	fwrite(buf, strlen(buf), 1, trailFile); //Write remaining buf to file

	FS_FCloseFile( fp );
	fclose( trailFile );

	printf("Completed\n"); //Printing filenames here fucks up??? memory?

	return 0;
}
