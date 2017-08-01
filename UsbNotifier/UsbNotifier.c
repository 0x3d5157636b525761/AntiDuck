/********************************************************************************
*  File:		UsbNotifier.c													*
*  Purpose:		USB notifier module.											*
********************************************************************************/


/** Includes *******************************************************************/
#include "UsbNotifier.h"
#include <dbt.h>
#include <Hidclass.h>


/** Constants ******************************************************************/

/********************************************************************************
*  Constant:	WND_CLASS_NAME													*
*  Purpose:		The registered window class name.								*
********************************************************************************/
#define WND_CLASS_NAME (L"USBRND_WindowClass")

/********************************************************************************
*  Constant:	WND_TITLE														*
*  Purpose:		The main window title.											*
********************************************************************************/
#define WND_TITLE (L"USBRND!")

/********************************************************************************
*  Constant:	KEYBOARD_HID_GUID_STRING										*
*  Purpose:		The GUID for a keyboard HID interface.							*
********************************************************************************/
#define KEYBOARD_HID_GUID_STRING (L"{884b96c3-56ef-11d1-bc8c-00a0c91405dd}")



/** Typedefs *******************************************************************/

/********************************************************************************
*  Structure:	USBNOTIFIER_CONTEXT												*
*  Purpose:		The module context.												*
********************************************************************************/
typedef struct _USBNOTIFIER_CONTEXT
{
	HDEVNOTIFY hDeviceNotify;						// Device notification handle
} USBNOTIFIER_CONTEXT, *PUSBNOTIFIER_CONTEXT;



/** Globals ********************************************************************/

/********************************************************************************
*  Global:		g_tContext														*
*  Purpose:		The module context.												*
********************************************************************************/
static
USBNOTIFIER_CONTEXT
g_tContext = { 0 };


/** Functions ******************************************************************/

/********************************************************************************
*  Function:	usbnotifier_RegisterDevice										*
*  Purpose:		Registers the device interface to the given window.				*
*  Parameters:	@ hWnd ~[in]~ The window to get the notifications.				*
*				@ pwszText ~[in]~ The message.									*
*				@ phDeviceNotify ~[out]~ Gets the device notify handle.			*
*  Returns:		A RETSTATUS.													*
*  Remarks:		* Free returned handle with UnregisterDeviceNotification.		*
********************************************************************************/
static
RETSTATUS
usbnotifier_RegisterDevice(
	__in __notnull HWND hWnd,
	__out PHDEVNOTIFY phDeviceNotify
)
{
	RETSTATUS eStatus = RETSTATUS_INVALID_VALUE;
	DEV_BROADCAST_DEVICEINTERFACE tNotificationFilter = { 0 };
	HRESULT hrError = E_UNEXPECTED;
	HDEVNOTIFY hDeviceNotify = NULL;

	DEBUG_ENTER();

	// Validations
	ASSERT(NULL != hWnd);
	ASSERT(NULL != phDeviceNotify);

	// Set the correct guid
	hrError = IIDFromString(KEYBOARD_HID_GUID_STRING, &(tNotificationFilter.dbcc_classguid));
	if (FAILED(hrError))
	{
		eStatus = DEBUG_RETMSG(DEBUG_GEN_FAIL_STATUS(),
			LOG_SEV_ERROR,
			"IIDFromString() failure (hrError=0x%.8x).",
			hrError);
		goto lblCleanup;
	}

	// Initialize other simple members
	tNotificationFilter.dbcc_size = sizeof(tNotificationFilter);
	tNotificationFilter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
	
	// Register for the device notification
	hDeviceNotify = RegisterDeviceNotificationW(hWnd, &tNotificationFilter, DEVICE_NOTIFY_WINDOW_HANDLE);
	if (NULL == hDeviceNotify)
	{
		eStatus = DEBUG_RETMSG(DEBUG_GEN_FAIL_STATUS(),
			LOG_SEV_ERROR,
			"RegisterDeviceNotificationW() failure (LastError=%lu).",
			GetLastError());
		goto lblCleanup;
	}

	// Success
	eStatus = RETSTATUS_SUCCESS;

lblCleanup:

	// Return result
	DEBUG_LEAVE_STATUS(eStatus);
	return eStatus;
}

/********************************************************************************
*  Function:	usbnotifier_MessagePump											*
*  Purpose:		The module's message pump.										*
*  Parameters:	@ hWnd ~[inout]~ The window to handle.							*
*  Remarks:		* Does not contain telemetries on purpose.						*
********************************************************************************/
static
VOID
usbnotifier_MessagePump(
	__inout HWND hWnd
)
{
	MSG tMsg = { 0 };

	// Unreferenced parameters
	UNREFERENCED_PARAMETER(hWnd);

	// Get all messages for any window that belongs to this thread without any filtering
	while (0 < GetMessageW(&tMsg, NULL, 0, 0))
	{
		(VOID)TranslateMessage(&tMsg);
		(VOID)DispatchMessageW(&tMsg);
	}
}

/********************************************************************************
*  Function:	USBNOTIFIER_WinProcCallback										*
*  Purpose:		The callback for our window procedure.							*
*  Parameters:	@ hWnd ~[in]~ The window.										*
*				@ dwMessage ~[in]~ The message.									*
*				@ tWparam ~[in]~ The WPARAM window message parameter.			*
*				@ tLparam ~[in]~ The LPARAM window message parameter.			*
*  Returns:		An LRESULT.														*
********************************************************************************/
LRESULT
WINAPI
USBNOTIFIER_WinProcCallback(
	__inout HWND hWnd,
	__in UINT dwMessage,
	__inout WPARAM tWparam,
	__inout LPARAM tLparam
)
{
	RETSTATUS eStatus = RETSTATUS_INVALID_VALUE;
	LRESULT lRet = 1;

	// Act according to the message
	switch (dwMessage)
	{
	case WM_CREATE:

		// Register the device
		eStatus = usbnotifier_RegisterDevice(hWnd, &(g_tContext.hDeviceNotify));
		if (RETSTATUS_FAILED(eStatus))
		{
			// Terminate on failure
			ExitProcess(eStatus);
		}
		break;

	case WM_DEVICECHANGE:

		// Lock on arrival
		if (DBT_DEVICEARRIVAL == tWparam)
		{
			DEBUG_MSG(LOG_SEV_INFO, "Identified keyboard. Locking.");
			(VOID)LockWorkStation();
		}
		break;
	
	case WM_CLOSE:

		// Unregister notification (best-effort)
		(VOID)UnregisterDeviceNotification(g_tContext.hDeviceNotify);
		(VOID)DestroyWindow(hWnd);
		break;

	case WM_DESTROY:

		// Quit message
		(VOID)PostQuitMessage(0);
		break;

	default:

		// Send all other messages on to the default windows handler
		lRet = DefWindowProcW(hWnd, dwMessage, tWparam, tLparam);
		break;
	}

	// Return the result
	return lRet;
}

/********************************************************************************
*  Function:	usbnotifier_InitWindowClass										*
*  Purpose:		Initializes the window class.									*
*  Returns:		A RETSTATUS.													*
********************************************************************************/
static
RETSTATUS
usbnotifier_InitWindowClass(VOID)
{
	RETSTATUS eStatus = RETSTATUS_INVALID_VALUE;
	WNDCLASSEX tWndClass = { 0 };

	DEBUG_ENTER();

	// Build the window class
	tWndClass.cbSize = sizeof(tWndClass);
	tWndClass.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
	tWndClass.hInstance = GetModuleHandleW(NULL);
	tWndClass.lpfnWndProc = USBNOTIFIER_WinProcCallback;
	tWndClass.cbClsExtra = 0;
	tWndClass.cbWndExtra = 0;
	tWndClass.hIcon = LoadIcon(0, IDI_SHIELD);
	tWndClass.hbrBackground = CreateSolidBrush(RGB(0, 0, 0));
	tWndClass.hCursor = LoadCursor(0, IDC_ARROW);
	tWndClass.lpszClassName = WND_CLASS_NAME;
	tWndClass.lpszMenuName = NULL;
	tWndClass.hIconSm = tWndClass.hIcon;

	// Register the window class
	if (!RegisterClassExW(&tWndClass))
	{
		eStatus = DEBUG_RETMSG(DEBUG_GEN_FAIL_STATUS(),
			LOG_SEV_ERROR,
			"RegisterClassExW() failure (LastError=%lu).",
			GetLastError());
		goto lblCleanup;
	}

	// Success
	eStatus = RETSTATUS_SUCCESS;

lblCleanup:

	// Return result
	DEBUG_LEAVE_STATUS(eStatus);
	return eStatus;
}


/********************************************************************************
*  Function:	USBNOTIFIER_Loop												*
********************************************************************************/
RETSTATUS
USBNOTIFIER_Loop(VOID)
{
	RETSTATUS eStatus = RETSTATUS_INVALID_VALUE;
	HWND hMainWindow = NULL;

	DEBUG_ENTER();

	// Initialize the window class
	eStatus = usbnotifier_InitWindowClass();
	if (RETSTATUS_FAILED(eStatus))
	{
		DEBUG_MSG(LOG_SEV_ERROR,
			"usbnotifier_InitWindowClass() failed (eStatus=0x%.8x).",
			eStatus);
		goto lblCleanup;
	}

	// Main app window
	hMainWindow = CreateWindowExW(WS_EX_CLIENTEDGE | WS_EX_APPWINDOW,
		WND_CLASS_NAME,
		WND_TITLE,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		0,
		1,
		1,
		NULL,
		NULL,
		GetModuleHandleW(NULL),
		NULL);
	if (hMainWindow == NULL)
	{
		eStatus = DEBUG_RETMSG(DEBUG_GEN_FAIL_STATUS(),
			LOG_SEV_ERROR,
			"CreateWindowExW() failure (LastError=%lu).",
			GetLastError());
		goto lblCleanup;
	}

	// Invoke the mssage pump
	usbnotifier_MessagePump(hMainWindow);

	// Success
	eStatus = RETSTATUS_SUCCESS;

lblCleanup:

	// Return result
	DEBUG_LEAVE_STATUS(eStatus);
	return eStatus;
}
