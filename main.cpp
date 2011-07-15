/* Copyright 2006-2011 Gima

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <windows.h>

#include "Commons.h"


extern "C" int entry()
{

  int liError       = ERR_OK;
  HANDLE lhProcess  = 0;
  int liPid;

  // obtain pid
  liError = GetPidByProcessName( "winlogon.exe", &liPid );
  if ( liError != ERR_OK )
  {

    switch ( liError )
    {
      case ERR_SNAPSHOT_FAILURE:
        printf( "Couldn't get a snapshot of processes\n" );
        break;

      case ERR_PROCESSFIRST_FAILURE:
        printf( "Couldn't enumerate processes\n" );
        break;

      case ERR_PID_NOT_FOUND:
        printf( "Requested process not found\n" );
        break;
    }

    goto _Finished;
  }

  // escalate privileges needed to access the process
  liError = GetDebugPrivileges();
  if ( liError != ERR_OK )
  {

    printf( "Couldn't get debug privileges: " );

    switch ( liError )
    {
      case ERR_CANT_OPEN_PROCESSTOKEN:
        printf( "can't open process token\n" );
        break;
      case ERR_CANT_LOOKUP_PRIVILEGEVALUE:
        printf( "can't look up privilege value\n" );
        break;
      case ERR_ADJUST_TOKEN_PRIVILEGES_FAILED:
        printf( "can't adjust token privileges\n" );
        break;
    }

    goto _Finished;
  }

  // open handle to process for manipulation
  lhProcess = OpenProcess( PROCESS_ALL_ACCESS, false, liPid );
  if ( lhProcess == 0 )
  {
    printf( "OpenProcess failed\n" );
    liError = ERR_OPENPROCESS_FAILED;
    goto _Finished;
  }

  // the real magic-doer
  liError = DisableSfc( lhProcess );
  if ( liError != ERR_OK )
  {

    printf( "Couldn't disable sfc: " );

    switch ( liError )
    {
      case ERR_MEMORYALLOCATION_FAILED:
        printf( "couldn't allocate memory in target process\n" );
        break;
      case ERR_CANT_GET_LOADLIBRARY_ADDR:
        printf( "couldn't get address of function loadlibrary\n" );
        break;
      case ERR_CANT_GET_GETPROCADDRESS_ADDR:
        printf( "couldn't get address of function getprocessaddress\n" );
        break;
      case ERR_WRITEPROCESS_STRUCT_FAILED:
        printf( "couldn't write helper structure to remote process\n" );
        break;
      case ERR_WRITEPROCESS_FUNC_FAILED:
        printf( "couldn't write thread function to remote process\n" );
        break;
      case ERR_CREATEREMOTETHREAD_FAILED:
        printf( "couldn't create remote thread\n" );
        break;
      case ERR_WAITFORTHREAD_FAILED:
        printf( "wait for thread to terminate failed\n" );
        break;
      case ERR_GET_THREAD_EXITCODE_FAILED:
        printf( "couldn't get thread's exit code\n" );
        break;
      case ERR_SFCOS_LOADLIBRARY_FAILED:
        printf( "couldn't load needed library\n" );
        break;
      case ERR_GETPROCADDRESS_FAILED:
        printf( "couldn't get address of ordinal 2\n" );
        break;
      case ERR_SFCTERMINATION_FAILED:
        printf( "call to ordinal 2 failed (it's disabled already?)\n" );
        break;
      default:
        printf( "unknown error" );
        break;
    }

    goto _Finished;
  }


  printf( "Successfully terminated sfc watcher thread\n" );

  _Finished:
  if ( lhProcess != 0 )
  {
    CloseHandle( lhProcess );
  }
	return liError;

}

