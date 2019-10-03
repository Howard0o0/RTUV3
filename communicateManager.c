#include "communicateManager.h"
#include "string.h"

static PT_CommunicateDev g_ptCommunicateDevHead;

int RegisterCommunicateDev(PT_CommunicateDev ptCommunicateDev)
{
	PT_CommunicateDev ptTmp;

	if (!g_ptCommunicateDevHead)
	{
		g_ptCommunicateDevHead   = ptCommunicateDev;
		ptCommunicateDev->ptNext = NULL;
	}
	else
	{
		ptTmp = g_ptCommunicateDevHead;
		while (ptTmp->ptNext)
		{
			ptTmp = ptTmp->ptNext;
		}
		ptTmp->ptNext	  = ptCommunicateDev;
		ptCommunicateDev->ptNext = NULL;
	}

	return 0;
}

void ShowCommunicateDevs(void)
{
	int i = 0;
	PT_CommunicateDev ptTmp = g_ptCommunicateDevHead;

	while (ptTmp)
	{
		printf("No.[%02d] %s\n", i++, ptTmp->name);
		ptTmp = ptTmp->ptNext;
	}
}

PT_CommunicateDev getCommunicateDev(char *devName)
{
    PT_CommunicateDev ptTmp = g_ptCommunicateDevHead;

	while (ptTmp)
	{
		if(strcmp(ptTmp->name,devName) == 0)
        {
            return ptTmp;
        }
		ptTmp = ptTmp->ptNext;
	}

    return NULL;
}
