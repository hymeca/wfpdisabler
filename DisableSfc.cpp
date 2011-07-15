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


DWORD WINAPI RemoteThreadProc( LPVOID lpParameter );
void DummySizeHelper();


int DisableSfc( HANDLE rpProcess )
{

  int liError     = ERR_OK;
  HANDLE lhThread  = 0;

  // calculate memory needed to hold helper structure and thread function
  int liAllocationSize = (int)&DummySizeHelper - (int)&RemoteThreadProc;
  liAllocationSize += sizeof( HlpStruct );

  // get memory in target process with read,write and execute rights
  void* lpAllocatedMemory = VirtualAllocEx( rpProcess, 0, liAllocationSize, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE );

  if ( lpAllocatedMemory == 0 )
  {
    liError = ERR_MEMORYALLOCATION_FAILED;
    goto _Finished;
  }

  // initialize helper structure
  HlpStruct lsHlpStruct;
  lsHlpStruct.lpLoadLibrary = (void*)GetProcAddress( LoadLibrary( "kernel32.dll" ), "LoadLibraryA" );
  lsHlpStruct.lpGetProcAddress = (void*)GetProcAddress( LoadLibrary( "kernel32.dll" ), "GetProcAddress" );

  if ( lsHlpStruct.lpLoadLibrary == 0)
  {
    liError = ERR_CANT_GET_LOADLIBRARY_ADDR;
    goto _Finished;
  }

  if ( lsHlpStruct.lpGetProcAddress == 0)
  {
    liError = ERR_CANT_GET_GETPROCADDRESS_ADDR;
    goto _Finished;
  }

  // write helper structure to beginning of allocated memory
  if ( WriteProcessMemory( rpProcess, lpAllocatedMemory, (void*)&lsHlpStruct, sizeof( HlpStruct ), 0 ) == 0 )
  {
    liError = ERR_WRITEPROCESS_STRUCT_FAILED;
    goto _Finished;
  }

  // get address from allocated memory (beginning plus helper thread's size) to write remote thread function to
  void* lpMemoryAddrForFunc = (void*)( (int)lpAllocatedMemory + sizeof( HlpStruct ) );
  // get remote thread function size
  int liThreadFuncSize = liAllocationSize - sizeof( HlpStruct );

  // write remote thread function to allocated memory plus size of helper structure
  if ( WriteProcessMemory( rpProcess, lpMemoryAddrForFunc, (void*)&RemoteThreadProc, liThreadFuncSize, 0 ) == 0 )
  {
    liError = ERR_WRITEPROCESS_FUNC_FAILED;
    goto _Finished;
  }


  // create remote thread with entry address of written function and pass helper structure as parameter (allocated memory)
  lhThread = CreateRemoteThread( rpProcess, 0, 0, (WINAPI DWORD (*)(void*))lpMemoryAddrForFunc, lpAllocatedMemory, 0, 0 );
  if ( lhThread == 0 )
  {
    liError = ERR_CREATEREMOTETHREAD_FAILED;
    goto _Finished;
  }

  // wait for the thread to finish (exit)
  if ( WaitForSingleObject( lhThread, 5000 ) != WAIT_OBJECT_0 )
  {
    liError = ERR_WAITFORTHREAD_FAILED;
    goto _Finished;
  }

  if ( GetExitCodeThread( lhThread, (DWORD*)&liError ) == 0 )
  {
    liError = ERR_GET_THREAD_EXITCODE_FAILED;
    goto _Finished;
  }


  _Finished:
  if ( lpAllocatedMemory != 0 )
  {
    VirtualFreeEx( rpProcess, lpAllocatedMemory, 0, MEM_RELEASE );
  }

  if ( lhThread != 0 )
  {
    CloseHandle( lhThread );
  }

  return liError;

}


// thread function to be executed in target processes address space
DWORD WINAPI RemoteThreadProc( LPVOID lpParameter )
{

  int liError = ERR_OK;

  // assign "function variable" it's address from passed in parameter
  typedef HMODULE (*typedef_LoadLibrary)(char*);
  typedef_LoadLibrary lpLoadLibrary = (typedef_LoadLibrary)((HlpStruct*)lpParameter)->lpLoadLibrary;

  // assign "function variable" it's address from passed in parameter
  typedef void* (*typedef_GetProcAddress)(HMODULE,char*);
  typedef_GetProcAddress lpGetProcAddress = (typedef_GetProcAddress)((HlpStruct*)lpParameter)->lpGetProcAddress;

  // create a "string" held in stack (cant reference global variables, they are invalid in another processes address space)
  char lpSfcOsDll[] = {'s','f','c','_','o','s','.','d','l','l',0};

  // load the library with function call to address provided by passed in parameter
  HMODULE lLibModule = lpLoadLibrary( lpSfcOsDll );
  if ( lLibModule == 0 )
  {
    liError = ERR_SFCOS_LOADLIBRARY_FAILED;
    goto _Finished;
  }

  // assign "function variable" and address to it from call through previously assigned functions
  typedef DWORD (*typedef_SfcTerminateWatcherThread)();
  typedef_SfcTerminateWatcherThread lpSfcTerminateWatcherThread;
  lpSfcTerminateWatcherThread = (typedef_SfcTerminateWatcherThread)lpGetProcAddress( lLibModule, (char*)2 );

  if ( lpSfcTerminateWatcherThread == 0 )
  {
    liError = ERR_GETPROCADDRESS_FAILED;
    goto _Finished;
  }

  // call the function
  if ( lpSfcTerminateWatcherThread() == 0)
  {
    liError = ERR_SFCTERMINATION_FAILED;
    goto _Finished;
  }


  _Finished:

  // library that is loaded isn't free'd as it's already loaded before this call

  return liError;

}


void DummySizeHelper() { }


