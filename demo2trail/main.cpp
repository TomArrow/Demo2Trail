// DemoTrimmer.cpp : Defines the entry point for the console application.
//

#include "deps.h" //only need CL_ReadDemoMessage from this?
//#include "client/client.h"
#include "demo_utils.h"
#include "demo_common.h"
#include "utils.h"

#include "q_string.h" //added

//from deps.h
//#include "qcommon/q_shared.h"
//#include "qcommon/qcommon.h"

//from client.h
//#include "qcommon/q_shared.h"
//#include "qcommon/qcommon.h"
//#include "rd-common/tr_public.h"
//#include "keys.h"
//#include "snd_public.h"
//#include "game/bg_public.h"
//#include "cgame/cg_public.h"
//#include "ui/ui_public.h"

//qboolean CL_ReadDemoMessage(fileHandle_t demofile, msg_t *msg);

#ifdef WIN32
#include <io.h>
#include <fcntl.h>
#endif

int main(int argc, char** argv)
{
	//cl_shownet->integer = 0;
	//printf( "JKDemoMetadata v" VERSION " loaded\n");
	char *demoFileName = NULL;
	char *trailFileName = NULL;

	if (argc == 2) {//Input file specified, use same name for output file 
		char *input = argv[1];

		//Possible drag/drop
		//Check if extension..
		if (COM_CompareExtension(input, ".dm_26")) {
			char temp[MAX_QPATH]; //wtf

			//printf("Extension found\n");
			COM_StripExtension(input, temp, sizeof(temp));
			input = va("%s", temp);
		}

		demoFileName = va("%s.dm_26", input);
		trailFileName = va("%s.cfg", input);
		//printf("1Input file: %s Output file: %s\n", demoFileName, trailFileName);
		//system("pause");
	}
	else if (argc == 3) { //Input and output file specified
		char *input = argv[1];
		char *output = argv[2];

		demoFileName = va("%s.dm_26", input);
		trailFileName = va("%s.cfg", output);
		//printf("2Input file: %s Output file: %s\n", demoFileName, trailFileName);
		//system("pause");
	}
	else {
		printf("No file specified.\n"
			"Usage: \"%s\" filename outfile (optional)\n", argv[0]);
		//system("pause");
		return -1;
	}

	printf("Input file: %s Output file: %s\n", demoFileName, trailFileName);



	/*
	if ( argc < 3 ) {
		printf( "No file specified.\n"
				"Usage: \"%s\" filename.dm_26 outfile.cfg\n", argv[0] );
		//system( "pause" );
		return -1;
	}
	*/

	/*
	if (argc < 5) {
		printf("No file specified.\n"
			"Usage: \"%s\" filename.dm_26 outfile.dm_26 starthh:mm:ss.sss endhh:mm:ss.sss\n", argv[0]);
		//system( "pause" );
		return -1;
	}
	*/

	/*
	if ( strlen( argv[3] ) != 12 ) {
		printf( "Invalid start time format %s.  Must be hh:mm:ss.sss\n", argv[3] );
		return -1;
	}
	argv[3][2] = argv[3][5]  = argv[3][8] = '\0';
	int startTime = atoi( argv[3] );
	startTime = ( startTime * 60 ) + atoi( argv[3] + 3 );
	startTime = ( startTime * 60 ) + atoi( argv[3] + 6 );
	startTime = ( startTime * 1000 ) + atoi( argv[3] + 9 );

	if ( strlen( argv[4] ) != 12 ) {
		printf( "Invalid end time format %s.  Must be hh:mm:ss.sss\n", argv[4] );
		return -1;
	}
	argv[4][2] = argv[4][5]  = argv[4][8] = '\0';
	int endTime = atoi( argv[4] );
	endTime = ( endTime * 60 ) + atoi( argv[4] + 3 );
	endTime = ( endTime * 60 ) + atoi( argv[4] + 6 );
	endTime = ( endTime * 1000 ) + atoi( argv[4] + 9 );
	*/

	//char *filename = argv[1];
	fileHandle_t fp;
	FS_FOpenFileRead(demoFileName, &fp, qfalse );
	if ( !fp ) {
		printf( "File %s not found.\n", demoFileName);
		//system( "pause" );
		return -1;
	}

	//char *outFilename = argv[2];
	FILE *trailFile;
	if ( !Q_strncmp(trailFileName, "-", 2 ) ) {
		trailFile = stdout;
#ifdef WIN32
		setmode(fileno(stdout), O_BINARY);
#else
		freopen( NULL, "wb", stdout );
#endif
	} else {
		trailFile = fopen(trailFileName, "wb" );
	}
	if ( !trailFile) {
		printf( "Couldn't open output file\n" );
		return -1;
	}


	//fileHandle_t tf;
	//FS_FOpenFileWrite(outFilename, qfalse);




	int framesSaved = 0;
	qboolean demoFinished = qfalse;
	qboolean startedRace = qfalse;
	char buf[8192] = {0};//noo
	while ( !demoFinished ) {
		msg_t msg;
		byte msgData[ MAX_MSGLEN ];
		MSG_Init( &msg, msgData, sizeof( msgData ) );
		demoFinished = CL_ReadDemoMessage( fp, &msg ) ? qfalse : qtrue;

		if ( demoFinished ) {
			//write rest of buffer?
			//printf("stopping 1\n");
			break;
		}
		try {
			CL_ParseServerMessage( &msg );
		} catch ( int ) {
			// thrown code means it wasn't a fatal error, so we can still dump what we had
			//printf("stopping 2\n");
			break;
		}

		if ( !ctx->cl.newSnapshots ) {
			continue;
		}
		int firstServerCommand = ctx->clc.lastExecutedServerCommand;
		// process any new server commands
		for ( ; ctx->clc.lastExecutedServerCommand < ctx->clc.serverCommandSequence; ctx->clc.lastExecutedServerCommand++ ) {
			char *command = ctx->clc.serverCommands[ ctx->clc.lastExecutedServerCommand & ( MAX_RELIABLE_COMMANDS - 1 ) ];
			Cmd_TokenizeString( command );
			char *cmd = Cmd_Argv( 0 );
			if ( cmd[0] ) {
				firstServerCommand = ctx->clc.lastExecutedServerCommand;
			}
			if ( !strcmp( cmd, "cs" ) ) {
				CL_ConfigstringModified(); //dont need?
			}
		}

		/*if ( getCurrentTime() > endTime ) {
			// finish up
			int len = -1;
			fwrite (&len, 4, 1, metaFile);
			fwrite (&len, 4, 1, metaFile);
			printf("stopping 3 %i %i\n", getCurrentTime(), endTime);
			break;
		} else */
		if ( framesSaved > 0 ) {

			//printf("Origin %.0f %.0f %.0f\n", ctx->cl.snap.ps.origin[0], ctx->cl.snap.ps.origin[1], ctx->cl.snap.ps.origin[2]);

			if ( framesSaved > Q_max( 10, ctx->cl.snap.messageNum - ctx->cl.snap.deltaNum ) ) { //what
				//CL_WriteDemoMessage( &msg, 0, metaFile );
			} else {
				//writeDeltaSnapshot( firstServerCommand, metaFile, qfalse );
			}
			framesSaved++;
		} else /*if ( getCurrentTime() > startTime )*/ {

			//printf("Origin START? %.0f %.0f %.0f\n", ctx->cl.snap.ps.origin[0], ctx->cl.snap.ps.origin[1], ctx->cl.snap.ps.origin[2]);

			//writeDemoHeader( metaFile );
			//writeDeltaSnapshot( firstServerCommand, metaFile, qtrue );
			// copy rest
			framesSaved = 1;
		}

		//How 2 trim neadless shit at start/end(?) of demo:

		//If tele bit was flipped, reset file?  Nvm, that will break courses with teleports.
		//If we had a timer and now we dont, reset file.  (this fucks up after we finish course)
		//If we had a timer and now we have a different timer, reset. (that fucks loopstarts?)
		//If we have a timer now, and we didnt previously, reset file (This fucks up prejumps, but oh wel?)

		//If current origin is same as previous... don't write anything...? 
		//(this fucks the timing.. but really are there any courses where people pause - besides mountain? and if so it can be done without the pause?)


		if (!startedRace && ctx->cl.snap.ps.duelTime) {
			Q_strncpyz(buf, "", sizeof(buf));
			startedRace = qtrue;
			//printf("Starting race\n");
		}
		else if (!ctx->cl.snap.ps.duelTime && startedRace) { //Oh no this will only show last second of race after finishing if we reset here.
			startedRace = qfalse;
			//printf("Stopping race\n");
			//Reset file
			//Q_strncpyz(buf, "", sizeof(buf)); //2789
		}


		{
			char *tmpMsg = NULL;
			tmpMsg = va("%i %i %i\n", (int)(ctx->cl.snap.ps.origin[0] + 0.5f), (int)(ctx->cl.snap.ps.origin[1] + 0.5f), (int)(ctx->cl.snap.ps.origin[2] + 0.5f));
		
			if (strlen(buf) + strlen(tmpMsg) >= sizeof(buf)) {
				//Write to file

				//FS_Write(buf, strlen(buf), tf);
				fwrite(buf, strlen(buf), 1, trailFile);
				Q_strncpyz(buf, "", sizeof(buf));
				//printf("resetting buf!\n");

				//buf[0] = '\0'; //instead of cpyz??
			}

			//printf("race? %i time? %i\n", startedRace, ctx->cl.snap.ps.duelTime);

			//if racetimer ?  to avoid shit at end of file
			Q_strcat(buf, sizeof(buf), tmpMsg);
		}










	}

	//Write remaining buf to file

	fwrite(buf, strlen(buf), 1, trailFile);

	/*int bufSize = 1024 * 10;
	char *buf = (char *) calloc( bufSize, 1 );
	int lenRead = 0;

	// the rest should just work.  in theory lol.
	while ( ( lenRead = FS_Read( buf, bufSize, fp ) ) > 0 ) {
		fwrite( buf, 1, lenRead, metaFile );
	}

	free( buf );*/

	FS_FCloseFile( fp );
	fclose( trailFile );

	printf("Completed\n");

	return 0;
}

