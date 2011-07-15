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

int strlen( char* buf )
{

  int liBufPos = 0;

  for ( ; *buf != 0; *buf++ )
  {
    liBufPos++;
  }

  return liBufPos;
}


void printf( char* buf )
{
  DWORD tmp;
  WriteConsole( GetStdHandle(STD_OUTPUT_HANDLE), buf, strlen(buf), &tmp, 0 );
}


int strcmp( char* buf, char* buf2 )
{
  int liBufPos;

  if ( strlen(buf) != strlen(buf2) ) return -1;

  for ( liBufPos = 0; liBufPos < strlen(buf); liBufPos++ )
  {

    if ( buf[liBufPos] != buf2[liBufPos] )
    {
      return -1;
    }

  }

  return 0;
}


