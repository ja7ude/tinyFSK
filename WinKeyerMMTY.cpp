//Copyright+LGPL

//-----------------------------------------------------------------------------------------------------------------------------------------------
// Copyright 2000-2017 Makoto Mori, Nobuyuki Oba
//-----------------------------------------------------------------------------------------------------------------------------------------------
// This file is part of tinyfsk.fsk.

// tinyfsk.fsk is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// tinyfsk.fsk is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

// You should have received a copy of the GNU Lesser General Public License along with tinyfsk.fsk.  If not, see
// <http://www.gnu.org/licenses/>.
//-----------------------------------------------------------------------------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
#include "Main.h"
//---------------------------------------------------------------------------
extern "C" void __declspec(dllexport) __stdcall extfskClose(void);
//---------------------------------------------------------------------------
USEFORM("Main.cpp", ExtFSK);
TExtFSK *pMain;
//---------------------------------------------------------------------------
int WINAPI DllEntryPoint(HINSTANCE hinst, unsigned long reason, void*)
{
	switch(reason){
		case DLL_PROCESS_ATTACH:
			pMain = NULL;

			memset(&sys, 0, sizeof(sys));
			::GetModuleFileName(hinst, sys.m_ModuleName, sizeof(sys.m_ModuleName));
			::SetDirName(sys.m_BgnDir, sys.m_ModuleName);
			sys.m_dwVersion = ::GetVersion();
			sys.m_WinNT = (sys.m_dwVersion < 0x80000000) ? TRUE : FALSE;

			sys.m_pDLPort = NULL;
			if( sys.m_WinNT ){
				sys.m_pDLPort = new CDLPort(sys.m_BgnDir);
				ASSERT(sys.m_pDLPort);
				if( sys.m_pDLPort && (!sys.m_pDLPort->IsOpen()) ){
					delete sys.m_pDLPort;
					sys.m_pDLPort = NULL;
				}
            }
			break;
		case DLL_PROCESS_DETACH:
			extfskClose();

			if( sys.m_pDLPort ){
				delete sys.m_pDLPort;
				sys.m_pDLPort = NULL;
			}
			break;
	}
	return 1;
}
//---------------------------------------------------------------------------
extern "C" LONG __declspec(dllexport) __stdcall
extfskOpen(LONG para)
{
	extfskClose();

	pMain = new TExtFSK(NULL);
	ASSERT(pMain);
	if( pMain != NULL ){
                pMain->m_para = para;
		pMain->SetPara();
		pMain->Show();
		return TRUE;
	}
	else {
		return FALSE;
	}
}

//---------------------------------------------------------------------------
extern "C" void __declspec(dllexport) __stdcall
extfskClose(void)
{
	if( pMain != NULL ){
		pMain->Close();
		delete pMain;
		pMain = NULL;
	}
}

//---------------------------------------------------------------------------
extern "C" LONG __declspec(dllexport) __stdcall
extfskIsTxBusy(void)
{
	if( pMain == NULL ) return FALSE;

	return pMain->IsBusy();
}

//---------------------------------------------------------------------------
extern "C" void __declspec(dllexport) __stdcall
extfskPutChar(BYTE c)
{
	if( pMain == NULL ) return;

	pMain->PutChar(c);
}

//---------------------------------------------------------------------------
extern "C" void __declspec(dllexport) __stdcall
extfskSetPTT(LONG tx)
{
	if( pMain == NULL ) return;

	pMain->SetPTT(tx & 1, TRUE);
}

