#pragma once
#include "client/client.h"

typedef struct demoContext_s {
	clientActive_t cl;
	clientConnection_t clc;
	clientStatic_t cls;

	struct {
		qboolean wasFirstCommandByte;
		qboolean firstCommandByteRead;
		int	preRecordingStartOffset;
	} preRecordingRelated;
} demoContext_t;

extern demoContext_t *ctx;
