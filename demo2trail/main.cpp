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
#define TRAIL_BUFSIZE 8192 //Write to file in 8kb chunks

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
			trailFileName = va("%s.cfg", temp);
		}
		else { //if no extension -fixme
			demoFileName = va("%s.dm_26", input);
			trailFileName = va("%s.cfg", input);
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
			trailFileName = va("%s.cfg", temp);
		}
		else {
			trailFileName = va("%s.cfg", input);
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
	int lastStartTime = 0;
	int lastRaceTimer = 0;
	int raceTimer = 0;
	vec3_t lastOrigin = {0};
	qboolean dontReset;

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
		int firstServerCommand = ctx->clc.lastExecutedServerCommand;
		for ( ; ctx->clc.lastExecutedServerCommand < ctx->clc.serverCommandSequence; ctx->clc.lastExecutedServerCommand++ ) { // process any new server commands
			char *command = ctx->clc.serverCommands[ ctx->clc.lastExecutedServerCommand & ( MAX_RELIABLE_COMMANDS - 1 ) ];
			Cmd_TokenizeString( command );
			char *cmd = Cmd_Argv( 0 );
			if ( cmd[0] ) {
				firstServerCommand = ctx->clc.lastExecutedServerCommand;
			}
		}

		//If we have a timer now, and we didnt previously, reset file (this removes resets)
		//If we have a timer now, and its less than our previous timer, reset file (this removes loopstarts)
		//If current origin is same as previous... don't write anything...(possibly fucks up timing, but never happens in practice)

		int startTime = ctx->cl.snap.ps.duelTime;
				
		if (startTime)
			raceTimer = ctx->cl.snap.serverTime - startTime;
		else
			raceTimer = 0;

		if (raceTimer > 6000) //Dont clear file if we have gone more than 6sec into run, yikes.
			dontReset = qtrue;

		if (startTime && !dontReset) {
#if LOOPSTARTS
			if (!lastStartTime)
#else
			if (!lastStartTime || (raceTimer < lastRaceTimer))
#endif
				Q_strncpyz(buf, "", sizeof(buf));
		}

		lastStartTime = startTime;
		lastRaceTimer = raceTimer;

		if (startTime) { //Only write if they are in a race
			if (!VectorCompare(ctx->cl.snap.ps.origin, lastOrigin)) //Dont write if they are standing still
			{
				char *tmpMsg = NULL;
				tmpMsg = va("%i %i %i\n", (int)(ctx->cl.snap.ps.origin[0] + 0.5f), (int)(ctx->cl.snap.ps.origin[1] + 0.5f), (int)(ctx->cl.snap.ps.origin[2] + 0.5f));
				if (strlen(buf) + strlen(tmpMsg) >= sizeof(buf)) { //Write to file
					fwrite(buf, strlen(buf), 1, trailFile);
					Q_strncpyz(buf, "", sizeof(buf)); //buf[0] = '\0'; ?
				}
				Q_strcat(buf, sizeof(buf), tmpMsg);

				VectorCopy(ctx->cl.snap.ps.origin, lastOrigin);
			}
		}
	}

	fwrite(buf, strlen(buf), 1, trailFile); //Write remaining buf to file

	FS_FCloseFile( fp );
	fclose( trailFile );

	printf("Completed\n"); //Printing filenames here fucks up??? memory?

	return 0;
}

