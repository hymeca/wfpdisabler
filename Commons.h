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
enum {  // errors used in functions, centralized
  // main
  ERR_OK,
  ERR_NO_PID,
  ERR_NO_DEBUGPRIVILEGES,
  ERR_OPENPROCESS_FAILED,
  ERR_SFC_TERMINATE_WATCHERTHREAD_FAILED,

  // GetPidByProcessName
  ERR_SNAPSHOT_FAILURE,
  ERR_PROCESSFIRST_FAILURE,
  ERR_PID_NOT_FOUND,

  // GetDebugPrivileges
  ERR_CANT_OPEN_PROCESSTOKEN,
  ERR_CANT_LOOKUP_PRIVILEGEVALUE,
  ERR_ADJUST_TOKEN_PRIVILEGES_FAILED,

  // DisableSfc
  ERR_MEMORYALLOCATION_FAILED,
  ERR_CANT_GET_LOADLIBRARY_ADDR,
  ERR_CANT_GET_GETPROCADDRESS_ADDR,
  ERR_WRITEPROCESS_STRUCT_FAILED,
  ERR_WRITEPROCESS_FUNC_FAILED,
  ERR_CREATEREMOTETHREAD_FAILED,
  ERR_WAITFORTHREAD_FAILED,
  ERR_GET_THREAD_EXITCODE_FAILED,

  // RemoteThreadProc
  ERR_SFCOS_LOADLIBRARY_FAILED,
  ERR_GETPROCADDRESS_FAILED,
  ERR_SFCTERMINATION_FAILED

};


// processes prototypes
int GetPidByProcessName( char* rpProcessName, int* rpPid );
int GetDebugPrivileges();
int DisableSfc( HANDLE rpProcess );

// replace few default functions with stripped down versions so no need to link to msvcrt -> smaller exe
void printf( char* buf );
int strcmp( char* buf, char* buf2 );


// ths structure is filled and copied to target process
struct HlpStruct
{
  void* lpLoadLibrary;
  void* lpGetProcAddress;
};


