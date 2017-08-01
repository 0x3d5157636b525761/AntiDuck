/********************************************************************************
*  File:		Utilities.h														*
*  Purpose:		General purpose utilities (macros and definitions).				*
********************************************************************************/
#pragma once


/** Includes *******************************************************************/
#ifdef _KERNEL_MODE
#include <fltKernel.h>
#include <ntddk.h>
#include <wdm.h>
#else					// _KERNEL_MODE
#include <Windows.h>
#include <process.h>
#include <sal.h>
#include <stdio.h>
#include <strsafe.h>
#include <initguid.h>
#endif					// _KERNEL_MODE

/** Typedefs *******************************************************************/

/********************************************************************************
*  Enum:		LOG_SEV															*
*  Purpose:		Log severity.													*
********************************************************************************/
typedef enum
{
	LOG_SEV_TRACE,
	LOG_SEV_INFO,
	LOG_SEV_ERROR,
	LOG_SEV_CRITICAL
} LOG_SEV, *PLOG_SEV;


/********************************************************************************
*  Typedef:		RETSTATUS														*
*  Purpose:		Defines a return status.										*
********************************************************************************/
#ifndef _KERNEL_MODE
typedef LONG RETSTATUS;
#endif	// _KERNEL_MODE


/********************************************************************************
*  Callback:	PFN_THREAD_ROUTINE												*
*  Purpose:		Defines a thread main routine.									*
*  Parameters:	@ pvParams ~[inout]~ Optional parameters for the routine.		*
*  Returns:		An arbitrary unsigned value.									*
********************************************************************************/
#ifndef _KERNEL_MODE
typedef UINT (WINAPI *PFN_THREAD_ROUTINE)(
	__inout_opt PVOID pvParams
);
#endif	// _KERNEL_MODE


/** Constants ******************************************************************/

/********************************************************************************
*  Constant:	RETSTATUS_INVALID_VALUE											*
*  Purpose:		A return status that states an internal error.					*
********************************************************************************/
#ifndef _KERNEL_MODE
#define RETSTATUS_INVALID_VALUE (-1)
#endif	// _KERNEL_MODE


/********************************************************************************
*  Constant:	RETSTATUS_SUCCESS												*
*  Purpose:		A return status that states a general success.					*
********************************************************************************/
#ifndef _KERNEL_MODE
#define RETSTATUS_SUCCESS (0)
#endif	// _KERNEL_MODE



/********************************************************************************
*  Constant:	MILISECONDS_IN_SECOND											*
*  Purpose:		The number of miliseconds in a second.							*
********************************************************************************/
#define MILISECONDS_IN_SECOND (1000)


/********************************************************************************
*  Constant:	BINARY_BASE														*
*  Purpose:		Binary base.													*
********************************************************************************/
#define BINARY_BASE (2)


/********************************************************************************
*  Constant:	OCTAL_BASE														*
*  Purpose:		Octal base.														*
********************************************************************************/
#define OCTAL_BASE (8)


/********************************************************************************
*  Constant:	DECIMAL_BASE													*
*  Purpose:		Decimal base.													*
********************************************************************************/
#define DECIMAL_BASE (10)


/********************************************************************************
*  Constant:	HEXADECIMAL_BASE												*
*  Purpose:		Hexadecimal base.												*
********************************************************************************/
#define HEXADECIMAL_BASE (16)


/********************************************************************************
*  Constant:	MAX_PATH														*
*  Purpose:		Maximum path length in characters.								*
********************************************************************************/
#ifndef MAX_PATH
#define MAX_PATH (260)
#endif		// MAX_PATH


/** Macros *********************************************************************/

/********************************************************************************
*  Macro:		_DEBUG_MSGS														*
*  Purpose:		Defines a unified definition for userland and kernel debug.		*
*  Remarks:		* Will work in debug builds, unless _NO_DEBUG_MSGS is defined.	*
********************************************************************************/
#ifndef _NO_DEBUG_MSGS
#if DBG
#define _DEBUG_MSGS
#endif		// DBG
#ifdef _DEBUG
#define _DEBUG_MSGS
#endif		// _DEBUG
#endif		// _NO_DEBUG_MSGS

/********************************************************************************
*  Macro:		LOG_FUNC														*
*  Purpose:		Defines the log function (can be overridden).					*
********************************************************************************/
#ifndef LOG_FUNC
#ifdef _KERNEL_MODE
#define LOG_FUNC DbgPrint
#else	// _KERNEL_MODE
#define LOG_FUNC printf
#endif	// _KERNEL_MODE
#endif	// LOG_FUNC


/********************************************************************************
*  Macro:		FORCE_SEMICOLON_START											*
*  Purpose:		Forces a macro to end with a semicolon (along with				*
*				FORCE_SEMICOLON_END).											*
********************************************************************************/
#define FORCE_SEMICOLON_START		do				\
									{

/********************************************************************************
*  Macro:		FORCE_SEMICOLON_END												*
*  Purpose:		Forces a macro to end with a semicolon (along with				*
*				FORCE_SEMICOLON_START).											*
********************************************************************************/
#define FORCE_SEMICOLON_END			} while (FALSE)




/********************************************************************************
*  Macro:		DEBUG_MSG														*
*  Purpose:		Logs a normal log message.										*
*  Parameters:	@ eSev ~[in]~ The severity.										*
*				@ pszFormat ~[in]~ The format string to log.					*
*				@ <ellipsis> ~[in]~ Format string arguments.					*
*  Remarks:		* If _DEBUG_MSGS is not defined, this does nothing.				*
********************************************************************************/
#ifdef _DEBUG_MSGS
#define DEBUG_MSG(eSev, pszFormat, ...)		FORCE_SEMICOLON_START																\
											(VOID)LOG_FUNC("[%d] %s: " ## pszFormat "\n", (eSev), __FUNCTION__, __VA_ARGS__);	\
											FORCE_SEMICOLON_END
#else		// _DEBUG_MSGS
#define DEBUG_MSG(eSev, pszFormat, ...)		FORCE_SEMICOLON_START														\
											FORCE_SEMICOLON_END
#endif		// _DEBUG_MSGS


/********************************************************************************
*  Macro:		DEBUG_ENTER														*
*  Purpose:		Logs the function when entering.								*
*  Remarks:		* If _DEBUG_MSGS is not defined, this does nothing.				*
*				* Always given LOG_SEV_TRACE.									*
********************************************************************************/
#define DEBUG_ENTER()		DEBUG_MSG(LOG_SEV_TRACE, "entering.")

/********************************************************************************
*  Macro:		DEBUG_LEAVE														*
*  Purpose:		Logs the function when leaving.									*
*  Remarks:		* If _DEBUG_MSGS is not defined, this does nothing.				*
*				* Always given LOG_SEV_TRACE.									*
********************************************************************************/
#define DEBUG_LEAVE()		DEBUG_MSG(LOG_SEV_TRACE, "leaving.")


/********************************************************************************
*  Macro:		DEBUG_LEAVE_STATUS												*
*  Purpose:		Logs the function when leaving with an NT status.				*
*  Parameters:	@ eStatus ~[in]~ The status to log.								*
*  Remarks:		* If _DEBUG_MSGS is not defined, this does nothing.				*
*				* Always given LOG_SEV_TRACE.									*
********************************************************************************/
#define DEBUG_LEAVE_STATUS(eStatus)		DEBUG_MSG(LOG_SEV_TRACE, "leaving with status 0x%.8x.", (eStatus))


/********************************************************************************
*  Macro:		DEBUG_RETMSG													*
*  Purpose:		Logs an error and returns a given status.						*
*  Parameters:	@ eStatus ~[in]~ The status to return.							*
*				@ eSev ~[in]~ The severity.										*
*				@ pszFormat ~[in]~ The format string to log.					*
*				@ <ellipsis> ~[in]~ Format string arguments.					*
*  Returns:		The given status.												*
*  Remarks:		* If _DEBUG_MSGS is not defined, this just returns the status.	*
*				* Statuses are generated anew based on the source file. This	*
*					means that editing versioned release files is prohibited.	*
********************************************************************************/
#define DEBUG_RETMSG(eStatus, eSev, pszFormat, ...)		(eStatus);													\
														DEBUG_MSG((eSev), pszFormat, __VA_ARGS__)

/********************************************************************************
*  Macro:		DEBUG_GEN_FAIL_STATUS											*
*  Purpose:		Generates a failure return status.								*
*  Returns:		A RETSTATUS unique to the line in the current module.			*
********************************************************************************/
#ifndef _KERNEL_MODE
#define DEBUG_GEN_FAIL_STATUS()							(-__LINE__)
#endif	// _KERNEL_MODE


/********************************************************************************
*  Macro:		CLOSE_TO_VALUE													*
*  Purpose:		Closes a variable with the given close function, unless it's	*
*				already set to a given value. The variable is set to the given	*
*				value after successfully invoking the close function.			*
*  Parameters:	@ pvVar ~[inout]~ The variable.									*
*				@ pvValue ~[in]~ The value to check against (or set).			*
*				@ pfnCloseFunc ~[in]~ The close function.						*
********************************************************************************/
#define CLOSE_TO_VALUE(pvVar, pvValue, pfnCloseFunc)			FORCE_SEMICOLON_START							\
																if ((pvValue) != (pvVar))						\
																{												\
																	(VOID)(pfnCloseFunc(pvVar));				\
																	(pvVar) = (pvValue);						\
																}												\
																FORCE_SEMICOLON_END


/********************************************************************************
*  Macro:		CLOSE															*
*  Purpose:		Closes a variable with the given close function, unless it's	*
*				NULL. The value of the variable is NULL after invoking the		*
*				close function.													*
*  Parameters:	@ pvVar ~[inout]~ The variable.									*
*				@ pfnCloseFunc ~[in]~ The close function.						*
********************************************************************************/
#define CLOSE(pvVar, pfnCloseFunc)				CLOSE_TO_VALUE((pvVar), NULL, (pfnCloseFunc))


/********************************************************************************
*  Macro:		CLOSE_HANDLE													*
*  Purpose:		Closes a handle with a NULL default value using CloseHandle.	*
*  Parameters:	@ hObject ~[inout]~ The handle variable.						*
********************************************************************************/
#ifdef _KERNEL_MODE
#define CLOSE_HANDLE(hObject)					CLOSE((hObject), ZwClose)
#else
#define CLOSE_HANDLE(hObject)					CLOSE((hObject), CloseHandle)
#endif

/********************************************************************************
*  Macro:		CLOSE_FILE_HANDLE												*
*  Purpose:		Closes a handle with an INVALID_HANDLE_VALUE default value		*
*				using CloseHandle.												*
*  Parameters:	@ hObject ~[inout]~ The handle variable.						*
********************************************************************************/
#ifdef _KERNEL_MODE
#define CLOSE_FILE_HANDLE(hObject)				CLOSE_TO_VALUE((hObject), INVALID_HANDLE_VALUE, ZwClose)
#else
#define CLOSE_FILE_HANDLE(hObject)				CLOSE_TO_VALUE((hObject), INVALID_HANDLE_VALUE, CloseHandle)
#endif


/********************************************************************************
*  Macro:		SET_UNLESS_NULL													*
*  Purpose:		Sets a simple value to a pointer, unless the pointer is NULL.	*
*  Parameters:	@ ppvVar ~[inout]~ The variable.								*
*				@ pvValue ~[in]~ The simple value.								*
*  Remarks:		* Use COPY_UNLESS_NULL for buffers.								*
********************************************************************************/
#define SET_UNLESS_NULL(ppvVar, pvValue)		FORCE_SEMICOLON_START			\
												if (NULL != (ppvVar))			\
												{								\
													(*(ppvVar)) = (pvValue);	\
												}								\
												FORCE_SEMICOLON_END



/********************************************************************************
*  Macro:		IS_FLAG_ON														*
*  Purpose:		Indicates whether the given value has the given flag turned on.	*
*  Parameters:	@ dwValue ~[in]~ The value.										*
*				@ dwFlag ~[in]~ The flag to check.								*
*  Returns:		A boolean value.												*
********************************************************************************/
#define IS_FLAG_ON(dwValue, dwFlag)				((dwFlag) == ((dwValue) & (dwFlag)))

/********************************************************************************
*  Macro:		RETSTATUS_FAILED												*
*  Purpose:		Determines if a RETSTATUS is a failure or not.					*
*  Parameters:	@ eStatus ~[in]~ The status.									*
*  Returns:		A boolean value.												*
********************************************************************************/
#ifndef _KERNEL_MODE
#define RETSTATUS_FAILED(eStatus)		(0 > (eStatus))
#endif	// _KERNEL_MODE

/********************************************************************************
*  Macro:		RETSTATUS_SUCCEEDED												*
*  Purpose:		Determines if a RETSTATUS is a success or not.					*
*  Parameters:	@ eStatus ~[in]~ The status.									*
*  Returns:		A boolean value.												*
********************************************************************************/
#ifndef _KERNEL_MODE
#define RETSTATUS_SUCCEEDED(eStatus)	(!RETSTATUS_FAILED(eStatus))
#endif	// _KERNEL_MODE


/********************************************************************************
*  Macro:		ABS																*
*  Purpose:		Absolute value of a value.										*
*  Parameters:	@ iValue ~[in]~ The value.										*
*  Returns:		The absolute value.												*
*  Remarks:		* Do not supply anything that causes side-effects, as the value	*
*					is evaluated multiple times. Common scenarios:				*
*					1. Auto incremeant \ decremenat (e.g. ABS(iValue++)).		*
*					2. Function invocation (e.g. SomeFunc(iValue)).				*
*					3. Assignments (e.g. ABS(iValue *= 2)).						*
********************************************************************************/
#define ABS(iValue)								(0 > (iValue) ? -(iValue) : (iValue))


/********************************************************************************
*  Macro:		MAX																*
*  Purpose:		Returns the maximum value of the given two.						*
*  Parameters:	@ iFirstValue ~[in]~ The first value.							*
*				@ iSecondValue ~[in]~ The second value.							*
*  Returns:		The maximum of the two.											*
*  Remarks:		* Do not supply anything that causes side-effects, as the value	*
*					is evaluated multiple times. Common scenarios:				*
*					1. Auto incremeant \ decremenat (e.g. ABS(iValue++)).		*
*					2. Function invocation (e.g. SomeFunc(iValue)).				*
*					3. Assignments (e.g. ABS(iValue *= 2)).						*
********************************************************************************/
#define MAX(iFirstValue, iSecondValue)			((iFirstValue) > (iSecondValue) ? (iFirstValue) : (iSecondValue))


/********************************************************************************
*  Macro:		MIN																*
*  Purpose:		Returns the minimum value of the given two.						*
*  Parameters:	@ iFirstValue ~[in]~ The first value.							*
*				@ iSecondValue ~[in]~ The second value.							*
*  Returns:		The minimum of the two.											*
*  Remarks:		* Do not supply anything that causes side-effects, as the value	*
*					is evaluated multiple times. Common scenarios:				*
*					1. Auto incremeant \ decremenat (e.g. ABS(iValue++)).		*
*					2. Function invocation (e.g. SomeFunc(iValue)).				*
*					3. Assignments (e.g. ABS(iValue *= 2)).						*
********************************************************************************/
#define MIN(iFirstValue, iSecondValue)			((iFirstValue) > (iSecondValue) ? (iSecondValue) : (iFirstValue))


/********************************************************************************
*  Macro:		CEIL															*
*  Purpose:		Returns the ceiling of a fraction.								*
*  Parameters:	@ nNumerator ~[in]~ The numerator.								*
*				@ nDenominator ~[in]~ The denominator.							*
*  Returns:		The ceiling of the fraction.									*
*  Remarks:		* The denominator is assume to be non-zero.						*
********************************************************************************/
#define CEIL(nNumerator, nDenominator)			(((nNumerator) + (nDenominator) - 1) / (nDenominator))



/********************************************************************************
*  Macro:		COMPILE_TIME_ASSERT												*
*  Purpose:		A compile-time assert.											*
*  Parameters:	@ cond ~[in]~ The condition (expression).						*
********************************************************************************/
#define COMPILE_TIME_ASSERT(cond)				FORCE_SEMICOLON_START	\
												switch (0)				\
												{						\
													case 0:				\
													case (cond):		\
													;					\
												}						\
												FORCE_SEMICOLON_END


/********************************************************************************
*  Macro:		ASSERT															*
*  Purpose:		Asserts a condition.											*
*  Parameters:	@ cond ~[in]~ The condition (expression).						*
*  Remarks:		* If _DEBUG_MSGS is not defined, this just returns the status.	*
*				* Debug-breaks if condition is not met (on DEBUG only).			*
********************************************************************************/
#ifndef ASSERT
#ifdef _DEBUG_MSGS
#define ASSERT(cond)							FORCE_SEMICOLON_START												\
												if (!(cond))														\
												{																	\
													DEBUG_MSG(LOG_SEV_CRITICAL, "Internal error (%s).", #cond);		\
													DebugBreak();													\
												}																	\
												FORCE_SEMICOLON_END
#else		// _DEBUG_MSGS
#define ASSERT(cond)							FORCE_SEMICOLON_START												\
												FORCE_SEMICOLON_END
#endif		// _DEBUG_MSGS
#endif		// ASSERT


/********************************************************************************
*  Macro:		ALLOCZ															*
*  Purpose:		Allocates a blob.												*
*  Parameters:	@ ePoolType ~[in]~ The pool type. Only supplied in Kernel mode.	*
*				@ cbBytes ~[in]~ Number of bytes to allocate.					*
*  Returns:		A new memory blob on success, or NULL on failure.				*
*  Remarks:		* ALLOC_TAG must be previously defined in Kernel mode.			*
*				* Free with FREE.												*
********************************************************************************/
#ifdef _KERNEL_MODE
#define ALLOCZ(ePoolType, cbBytes)		(utilities_SafeMemZero(ExAllocatePoolWithTag((ePoolType), (cbBytes), (ALLOC_TAG)), (cbBytes)))
#else	// _KERNEL_MODE
#define ALLOCZ(cbBytes)					(HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, (cbBytes)))
#endif	// _KERNEL_MODE


/********************************************************************************
*  Macro:		FREE															*
*  Purpose:		Frees a blob.													*
*  Parameters:	@ pvMem ~[in]~ The memory to free.								*
*  Remarks:		* ALLOC_TAG must be previously defined in Kernel mode.			*
********************************************************************************/
#ifdef _KERNEL_MODE
#define FREE(pvMem)						FORCE_SEMICOLON_START									\
										if (NULL != (pvMem))									\
										{														\
											(VOID)(ExFreePoolWithTag((pvMem), (ALLOC_TAG)));	\
											(pvMem) = NULL;										\
										}														\
										FORCE_SEMICOLON_END

#else	// _KERNEL_MODE
#define FREE(pvMem)						FORCE_SEMICOLON_START									\
										if (NULL != (pvMem))									\
										{														\
											(VOID)(HeapFree(GetProcessHeap(), 0, (pvMem)));		\
											(pvMem) = NULL;										\
										}														\
										FORCE_SEMICOLON_END
#endif	// _KERNEL_MODE


/********************************************************************************
*  Macro:		BEGIN_THREAD													*
*  Purpose:		Starts a thread.												*
*  Parameters:	@ pfnRoutine ~[in]~ The thread routine.							*
*				@ pvArgs ~[inout]~ Optional arguments (or NULL if unused).		*
*				@ dwFlags ~[in]~ Can be either 0 or CREATE_SUSPENDED.			*
*  Returns:		A handle to the created process.								*
*  Remarks:		* Thread routine must be of type PFN_THREAD_ROUTINE.			*
********************************************************************************/
#ifndef _KERNEL_MODE
#define BEGIN_THREAD(pfnRoutine, pvArgs, dwFlags)		((HANDLE)(_beginthreadex(NULL, 0, (PFN_THREAD_ROUTINE)(pfnRoutine), (pvArgs), (dwFlags), NULL)))
#endif	// _KERNEL_MODE




/** Functions ******************************************************************/

/********************************************************************************
*  Function:	utilities_SafeMemZero											*
*  Purpose:		Safely zeros memory.											*
*  Parameters:	@ pvMem ~[inout]~ The memory, or NULL.							*
*				@ cbBytes ~[in]~ Number of bytes.								*
*  Returns:		The memory pointer.												*
*  Remarks:		* Acts like memset, but safely disregards NULL pointers.		*
*				* Defined as static and inline to be included in object files.	*
********************************************************************************/
static
__inline
PVOID
utilities_SafeMemZero(
	__inout_bcount_opt(cbBytes) PVOID pvMem,
	__in SIZE_T cbBytes
)
{
	// Safely zero memory
	if (NULL != pvMem)
	{
		RtlZeroMemory(pvMem, cbBytes);
	}

	// Return result
	return pvMem;
}
