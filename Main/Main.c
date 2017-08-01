/********************************************************************************
*  File:		Main.c															*
*  Purpose:		Main routine.													*
********************************************************************************/


/** Includes *******************************************************************/
#include <Utilities.h>
#include "../UsbNotifier/UsbNotifier.h"


/** Functions ******************************************************************/

/********************************************************************************
*  Function:	wmain															*
*  Purpose:		Main routine.													*
********************************************************************************/
INT
wmain(
	__in INT nArgs, 
	__in_ecount(nArgs) PWSTR* ppwszArgs
)
{
	RETSTATUS eStatus = RETSTATUS_INVALID_VALUE;

	DEBUG_ENTER();

	// Unreferenced parameters
	UNREFERENCED_PARAMETER(nArgs);
	UNREFERENCED_PARAMETER(ppwszArgs);

	// Initialize the window class
	eStatus = USBNOTIFIER_Loop();
	if (RETSTATUS_FAILED(eStatus))
	{
		DEBUG_MSG(LOG_SEV_ERROR,
			"USBNOTIFIER_Loop() failed (eStatus=0x%.8x).",
			eStatus);
		goto lblCleanup;
	}

	// Success
	eStatus = RETSTATUS_SUCCESS;

lblCleanup:

	// Return result
	DEBUG_LEAVE_STATUS(eStatus);
	return eStatus;
}
