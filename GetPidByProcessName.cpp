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
#include <tlhelp32.h>

#include "Commons.h"


int GetPidByProcessName( char* rpProcessName, int* rpPid )
{

  int liError = ERR_OK;

  // get snapshot of processes
  HANDLE liSnapshot = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );
  if ( liSnapshot == INVALID_HANDLE_VALUE )
  {
    liError = ERR_SNAPSHOT_FAILURE;
    goto _Finished;
  }


  // get first process
  PROCESSENTRY32 lProcessEntry;
  lProcessEntry.dwSize = sizeof( PROCESSENTRY32 );

  if ( Process32First( liSnapshot, &lProcessEntry ) == false )
  {
    liError = ERR_PROCESSFIRST_FAILURE;
    goto _Finished;
  }


  *rpPid = -1;  // so that at function end, it can be deducted if process was found or not (no pid with -1 exists afaik)

  // Loop every process
  do
  {

    if ( strcmp( lProcessEntry.szExeFile, rpProcessName ) == 0 )
    {
      // processes name matched with the one to find
      *rpPid = (int)lProcessEntry.th32ProcessID;
      break;
    }


  } while ( Process32Next( liSnapshot, &lProcessEntry ) == true );


  _Finished:

  if ( liSnapshot != INVALID_HANDLE_VALUE )
  {
    CloseHandle( liSnapshot );
  }

  // if there were no errors but pid has not changed, process was not found
  if ( (liError == ERR_OK) && (*rpPid == -1) )
  {
    liError = ERR_PID_NOT_FOUND;
  }


  return liError;

}


