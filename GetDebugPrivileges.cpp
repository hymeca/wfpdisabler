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


int GetDebugPrivileges()
{

  int liError   = ERR_OK;
  HANDLE lToken = 0; // hopefully token with value 0 dont exists

  // get process token
  if ( OpenProcessToken( GetCurrentProcess(), TOKEN_ALL_ACCESS, &lToken ) == 0 )
  {
    liError = ERR_CANT_OPEN_PROCESSTOKEN;
    goto _Finished;
  }

  // get 64bit value for identifying privilege to be looked up
  LUID lPrivilegeIdentifier;
  if ( LookupPrivilegeValue( 0, SE_DEBUG_NAME, &lPrivilegeIdentifier ) == 0 )
  {
    liError = ERR_CANT_LOOKUP_PRIVILEGEVALUE;
    goto _Finished;
  }

  // prefix privilege-setting array to enable the privilege looked up
  TOKEN_PRIVILEGES lTokenPrivileges;
  lTokenPrivileges.PrivilegeCount           = 1;
  lTokenPrivileges.Privileges[0].Luid       = lPrivilegeIdentifier;
  lTokenPrivileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

  // try to enable the privilege
  AdjustTokenPrivileges( lToken, false, &lTokenPrivileges, 0, 0, 0 );
  if ( GetLastError() != ERROR_SUCCESS )
  {
    liError = ERR_ADJUST_TOKEN_PRIVILEGES_FAILED;
    goto _Finished;
  }


  // if token has changed, close it
  _Finished:

  if ( lToken != 0 )
  {
    CloseHandle( lToken );
  }

  return liError;

}


