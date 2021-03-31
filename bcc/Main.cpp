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
//---------------------------------------------------------------------------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Main.h"
SYS sys;
// ---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

//---------------------------------------------------------------------------
void __fastcall SetDirName(LPSTR t, LPCSTR pName)
{
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char name[_MAX_FNAME];
	char ext[_MAX_EXT];
	AnsiString	Dir;

	::_splitpath( pName, drive, dir, name, ext );
	Dir = drive;
	Dir += dir;
	strncpy(t, Dir.c_str(), MAX_PATH-1);
}
//---------------------------------------------------------------------------
void __fastcall SetEXT(LPSTR t, LPCSTR pExt)
{
	LPSTR p = LASTP(t);
	for( ; p > t; p-- ){
		if( *p == '.' ){
			p++;
			strcpy(p, pExt);
			return;
		}
	}
}
//---------------------------------------------------------------------------
WORD __fastcall htow(LPCSTR p)
{
	int d = 0;
	for( ; *p; p++ ){
		if( (*p != ' ') && (*p != '$') ){
			d = d << 4;
			d += *p & 0x0f;
			if( *p >= 'A' ) d += 9;
		}
	}
	return WORD(d);
}
//---------------------------------------------------------------------------

bool Is64bitOS(){
  char *ptr = getenv("ProgramW6432");
  if( ptr != NULL )
    return true;
  else
    return false;
}

//***************************************************************************
// CDLPort class
__fastcall CDLPort::CDLPort(LPCSTR pPath)
{
	m_hDLib = NULL;
	m_Path = pPath;
	LPSTR p = LASTP(m_Path.c_str());
	if (*p == '\\')
		* p = 0;
}
//---------------------------------------------------------------------------
__fastcall CDLPort::~CDLPort()
{
  /*
	if( m_hLib != NULL ){
		Close();
		::FreeLibrary(m_hLib);
		m_hLib = NULL;
	}
  */
	if( m_hDLib != NULL ){
		::FreeLibrary(m_hDLib);
		m_hDLib = NULL;
	}
}
//---------------------------------------------------------------------
BOOL __fastcall CDLPort::IsFile(LPCSTR pName)
{
	HANDLE hFile = ::CreateFile(pName,
					0,
					FILE_SHARE_READ, NULL,
					OPEN_EXISTING,
					0,
					NULL
	);
	if( hFile != INVALID_HANDLE_VALUE ){
		::CloseHandle(hFile);
	}
	return hFile != INVALID_HANDLE_VALUE;
}
//---------------------------------------------------------------------------
//***************************************************************************
// CFSK class
__fastcall CFSK::CFSK(void)
{
	m_wPortA = 0;
	m_bPortD = 0;

	m_ErrorAccess = 0;
	m_BLen = 5;
	Init();
#if MeasureAccuracy
	QueryPerformanceFrequency(&m_liFreq);
	m_liPOld.u.HighPart = -1;
#endif
}
//---------------------------------------------------------------------------
void __fastcall CFSK::Init(void)
{
	m_hPort = INVALID_HANDLE_VALUE;
	m_StgD = -1;
	m_nFSK = 0;
	m_nPTT = 2;
	m_Sequence = 0;
	m_Count = 0;
	m_oFSK = 1;
	m_aFSK = -1;
	m_oPTT = 0;
	m_aPTT = -1;
  m_shift_state = 0;
}
//---------------------------------------------------------------------------
void __fastcall CFSK::SetHandle(HANDLE hPort, int nFSK, int nPTT)
{
	m_hPort = INVALID_HANDLE_VALUE;
	m_nFSK = nFSK;
	m_nPTT = nPTT;
	m_aFSK = -1;
	m_aPTT = -1;
	m_hPort = hPort;
	m_ErrorAccess = 0;
}
//---------------------------------------------------------------------------
//	para:	Upper16bits	Speed(eg. 45)
//			Lower16bits	b1-b0	Stop (0-1, 1-1.5, 2-2)
//						b5-b2	Length
void __fastcall CFSK::SetPara(LONG para)
{
	m_BLen = (para >> 2) & 0x000f;
}
//---------------------------------------------------------------------------
// This function is called from the TimeProc(). and according to
//MSDN,  it may be an illegal operation.  MSDN said, Applications
//should not call any system-defined functions from inside a
//callback function,  except for several functions.
// However, the EscapeCommFunction() seems to be no problem on my
//PCs with Windows 2000 and Windows XP, but I am not sure if it
//works on every PC.
// BTW, EnterCriticalSection() and LeaveCriticalSection() had problem
//on this, and I gave up to use them....
//
void __fastcall CFSK::SetPort(int port, int sw)
{
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall CFSK::SetPTT(int sw)
{
  if( sw )
    tinyIt( '[' );
  else
    tinyIt( ']' );
	if( m_oPTT != sw ){
		m_oPTT = sw;
		if( sw ){
			SetDelay(8);
		}
	}
}
//---------------------------------------------------------------------------
const int ttable0[32] = {
  0x00,   //0 null is null
  'E',    //1
  0x0A,   //2
  'A',    //3
  ' ',    //4
  'S',    //5
  'I',    //6
  'U',    //7
  0x0D,   //8
  'D',    //9
  'R',    //10
  'J',    //11
  'N',    //12
  'F',    //13
  'C',    //14
  'K',    //15
  'T',    //16
  'Z',    //17
  'L',    //18
  'W',    //19
  'H',    //20
  'Y',    //21
  'P',    //22
  'Q',    //23
  'O',    //24
  'B',    //25
  'G',    //26
  0x00,   //27 figure
  'M',    //28
  'X',    //29
  'V',    //30
  0x00    //31 letter
};
const int ttable1[32] = {
  0x00,   //0 null is null
  '3',    //1
  0x0A,   //2
  '-',    //3
  ' ',    //4
  '\'',   //5
  '8',    //6
  '7',    //7
  0x0D,   //8
  '$',    //9
  '4',    //10
  '\'',   //11
  ',',    //12
  '!',    //13
  ':',    //14
  '(',    //15
  '5',    //16
  '+',    //17
  ')',    //18
  '2',    //19
  0x00,   //20
  '6',    //21
  '0',    //22
  '1',    //23
  '9',    //24
  '?',    //25
  '&',    //26
  0x00,   //27 figure
  '.',    //28
  '/',    //29
  ';',    //30
  0x00    //31 letter
};

BYTE __fastcall CFSK::baudot2ascii( BYTE c )
{
  if( m_shift_state == 0 ){
    return( ttable0[c] );
  }
  else{
    return( ttable1[c] );
  }
}

void __fastcall CFSK::tinyIt( BYTE c )
{
  BYTE d;
  switch( c ){
    case 0x1f: m_shift_state = 0; return;
    case 0x1b: m_shift_state = 1; return;
    case '[': d = c; break;
    case ']': d = c; break;
    default: d = baudot2ascii(c&0x1F); break;
  }
  BYTE data[2] = {0,0};
  DWORD sent;
  data[0] = d;
  WriteFile( m_hPort, data, 1, &sent, NULL );
}


//***************************************************************************
// TExtFSK (MainWindow) class
//---------------------------------------------------------------------------
__fastcall TExtFSK::TExtFSK(TComponent* Owner)
	: TForm(Owner)
{
	m_DisEvent = 1;
	Top = 0;
	Left = 0;
	m_hPort = INVALID_HANDLE_VALUE;
	m_wPortA = 0;
	m_wLPTA = 0;
	m_X = 0;

	m_IniName = sys.m_ModuleName;
	SetEXT(m_IniName.c_str(), "ini");
	PortName->ItemIndex = 0;
	ReadIniFile();
	m_WindowState = WindowState;

	OpenPort();
	UpdatePort();
	m_DisEvent = 0;

  AnsiString cap = "tinyfsk.fsk ";
  cap += VERSION;
  Caption = cap;
	SetPTT(0, FALSE);
}
//---------------------------------------------------------------------------

void __fastcall TExtFSK::FormClose(TObject *Sender, TCloseAction &Action)
{
	if( IsOpen() ){
		m_fsk.SetPort(RGPTT->ItemIndex, CBInvPTT->Checked);
	}
	ClosePort();
	WriteIniFile();
}
//---------------------------------------------------------------------------
void __fastcall TExtFSK::ReadIniFile(void)
{
	TMemIniFile *pIniFile = new TMemIniFile(m_IniName);

	if( !sys.m_WinNT || sys.m_pDLPort ){
		AnsiString as = pIniFile->ReadString("DirectAccess", "LPTADR", "0");
		m_wLPTA = htow(as.c_str());
		if( m_wLPTA ){
			char bf[32];
			wsprintf(bf, "LPT$%X", m_wLPTA);
			PortName->Items->Add(bf);
		}
	}
	Top = pIniFile->ReadInteger("Window", "Top", Top);
	Left = pIniFile->ReadInteger("Window", "Left", Left);
	WindowState = (TWindowState)pIniFile->ReadInteger("Window", "State", WindowState);
	AnsiString as = pIniFile->ReadString("Settings", "Port", "COM1");
	int n = PortName->Items->IndexOf(as);
	if( n < 0 ){
		n = atoi(as.c_str());
		if( n < 0 ) n = 0;
	}
	PortName->ItemIndex = n;
	RGFSK->ItemIndex = pIniFile->ReadInteger("Settings", "FSK", RGFSK->ItemIndex);
	RGPTT->ItemIndex = pIniFile->ReadInteger("Settings", "PTT", RGPTT->ItemIndex);
	CBInvFSK->Checked = pIniFile->ReadInteger("Settings", "InvFSK", CBInvFSK->Checked);
	CBInvPTT->Checked = pIniFile->ReadInteger("Settings", "InvPTT", CBInvPTT->Checked);
	delete pIniFile;
}
//---------------------------------------------------------------------------
void __fastcall TExtFSK::WriteIniFile(void)
{
	TMemIniFile *pIniFile = new TMemIniFile(m_IniName);
	pIniFile->WriteInteger("Window", "Top", Top);
	pIniFile->WriteInteger("Window", "Left", Left);
	pIniFile->WriteInteger("Window", "State", WindowState);
	pIniFile->WriteString("Settings", "Port", PortName->Items->Strings[PortName->ItemIndex]);
	pIniFile->WriteInteger("Settings", "FSK", RGFSK->ItemIndex);
	pIniFile->WriteInteger("Settings", "PTT", RGPTT->ItemIndex);
	pIniFile->WriteInteger("Settings", "InvFSK", CBInvFSK->Checked);
	pIniFile->WriteInteger("Settings", "InvPTT", CBInvPTT->Checked);
	if( !sys.m_WinNT || sys.m_pDLPort ){
		char bf[32];
		wsprintf(bf, "%X", m_wLPTA);
		pIniFile->WriteString("DirectAccess", "LPTADR", bf);
	}
	pIniFile->UpdateFile();
	delete pIniFile;
}
//---------------------------------------------------------------------------
void __fastcall TExtFSK::UpdatePort()
{
	if( sys.m_WinNT && !sys.m_pDLPort ) return;

	m_DisEvent++;
	if( m_wPortA ){
		RGFSK->Items->Strings[0] = "D0-D3";
		RGFSK->Items->Strings[1] = "D4-D7";
		RGFSK->Items->Strings[2] = "STROBE";
		RGPTT->Items->Strings[0] = "D0-D3";
		RGPTT->Items->Strings[1] = "D4-D7";
		RGPTT->Items->Strings[2] = "STROBE";
	}
	else {
		RGFSK->Items->Strings[0] = "TXD";
		RGFSK->Items->Strings[1] = "RTS";
		RGFSK->Items->Strings[2] = "DTR";
		RGPTT->Items->Strings[0] = "TXD";
		RGPTT->Items->Strings[1] = "RTS";
		RGPTT->Items->Strings[2] = "DTR";
	}
	m_DisEvent--;
}
//---------------------------------------------------------------------------
void __fastcall TExtFSK::UpdateComStat(void)
{
	char bf[128];

	if( m_wPortA ){
		wsprintf(bf, "Addr:%X", m_wPortA);
		LComStat->Color = clYellow;
	}
	else {
		wsprintf(bf, "Status:%s", m_hPort != INVALID_HANDLE_VALUE ? "OK" : "NG");
		LComStat->Color = m_hPort != INVALID_HANDLE_VALUE ? clBtnFace : clRed;
	}
	LComStat->Caption = bf;
}
//---------------------------------------------------------------------------
void __fastcall TExtFSK::OpenPort(void)
{
	ClosePort();
	OpenPort_();
	UpdateComStat();
	m_fsk.SetInvFSK(CBInvFSK->Checked);
	m_fsk.SetInvPTT(CBInvPTT->Checked);
	m_fsk.SetHandle(m_hPort, RGFSK->ItemIndex, RGPTT->ItemIndex);
}
//---------------------------------------------------------------------------
BOOL __fastcall TExtFSK::OpenPort_(void)
{
	AnsiString pname = PortName->Items->Strings[PortName->ItemIndex];

	m_wPortA = 0;
	m_fsk.SetLPT(m_wPortA);

	if( !strncmpi(pname.c_str(), "LPT", 3) ){
		WORD n = htow(pname.c_str() + 3);
		if( (n >= 1) && (n <= 3) ){
			n--;
			m_wPortA = m_wLPT[n];
		}
		else {
			m_wPortA = m_wLPTA;
		}
		if( m_wPortA ){
			m_fsk.SetLPT(m_wPortA);
			m_hPort = HANDLE(DWORD(INVALID_HANDLE_VALUE) + 1);
			return TRUE;
		}
	}
  if( pname.SubString(1,3) == "COM" ){
    pname = "\\\\.\\" + pname;
  }

	m_hPort = ::CreateFile( pname.c_str(),
					GENERIC_READ | GENERIC_WRITE,
					0, NULL,
					OPEN_EXISTING,
					FILE_ATTRIBUTE_NORMAL,
					NULL
	);
	if( m_hPort == INVALID_HANDLE_VALUE ) return FALSE;
	if( ::SetupComm( m_hPort, DWORD(256), DWORD(2) ) == FALSE ){
		::CloseHandle(m_hPort);
		m_hPort = INVALID_HANDLE_VALUE;
		return FALSE;
	}
	::PurgeComm( m_hPort, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR );

	COMMTIMEOUTS TimeOut;

	TimeOut.ReadIntervalTimeout = 0xffffffff;
	TimeOut.ReadTotalTimeoutMultiplier = 0;
	TimeOut.ReadTotalTimeoutConstant = 0;
	TimeOut.WriteTotalTimeoutMultiplier = 0;
	TimeOut.WriteTotalTimeoutConstant = 20000;
	if( !::SetCommTimeouts( m_hPort, &TimeOut ) ){
		::CloseHandle( m_hPort );
		m_hPort = INVALID_HANDLE_VALUE;
		return FALSE;
	}
	::GetCommState( m_hPort, &m_dcb );
	m_dcb.BaudRate = 9600;
	m_dcb.fBinary = TRUE;
	m_dcb.ByteSize = 8;
	m_dcb.Parity = NOPARITY;
	m_dcb.StopBits = ONESTOPBIT;
	m_dcb.XonChar = 0x11;	// XON
	m_dcb.XoffChar = 0x13;	// XOFF
	m_dcb.fParity = 0;
	m_dcb.fOutxCtsFlow = FALSE;
	m_dcb.fInX = m_dcb.fOutX = FALSE;
	m_dcb.fOutxDsrFlow = FALSE;
	m_dcb.EvtChar = 0x0d;
	m_dcb.fRtsControl = RTS_CONTROL_DISABLE;
	m_dcb.fDtrControl = DTR_CONTROL_DISABLE;
	m_dcb.fTXContinueOnXoff = FALSE;
	m_dcb.XonLim = USHORT(256/4);
	m_dcb.XoffLim = USHORT(256*3/4);
	m_dcb.DCBlength = sizeof( DCB );

	if( !::SetCommState( m_hPort, &m_dcb ) ){
		::CloseHandle( m_hPort );
		m_hPort = INVALID_HANDLE_VALUE;
		return FALSE;
	}

	if( !::SetCommMask( m_hPort, EV_RXFLAG ) ){
		::CloseHandle(m_hPort);
		m_hPort = INVALID_HANDLE_VALUE;
		return FALSE;
	}

	return TRUE;
}
//---------------------------------------------------------------------------
void __fastcall TExtFSK::ClosePort(void)
{
	if( m_wPortA ){
		m_wPortA = 0;
		m_hPort = INVALID_HANDLE_VALUE;
	}
	if( IsOpen() ){
		m_fsk.Disable();
		::CloseHandle(m_hPort);
		m_hPort = INVALID_HANDLE_VALUE;
	}
	UpdateComStat();
}
//---------------------------------------------------------------------------
//	para:	Upper16bits	Speed(eg. 45)
//			Lower16bits	b1-b0	Stop (0-1, 1-1.5, 2-2)
//						b5-b2	Length
void __fastcall TExtFSK::SetPara(LONG para)
{
	m_fsk.SetPara(para);
  char ss[32];
  char data[4];
  int s_baud;
  m_baud = para >> 16;
  data[0] = '~';
  switch( m_baud ){
#ifdef ADDITIONAL_BAUDRATE
    case 22:  data[1] = '2';  s_baud = 22;  strcpy( ss, "22" ); break;
#endif
    case 45:  data[1] = '4';  s_baud = 45;  strcpy( ss, "45.45" ); break;
    case 50:  data[1] = '5';  s_baud = 50;  strcpy( ss, "50" ); break;
#ifdef ADDITIONAL_BAUDRATE
    case 56:  data[1] = '6';  s_baud = 56;  strcpy( ss, "56" ); break;
#endif
    case 75:  data[1] = '7';  s_baud = 75;  strcpy( ss, "75" ); break;
#ifdef ADDITIONAL_BAUDRATE
    case 100: data[1] = 'A';  s_baud = 100;  strcpy( ss, "100" ); break;
#endif
    default:  data[1] = '4';  s_baud = 45;  strcpy( ss, "45.45" ); break;
  }
  data[2] = 0x0D;
  data[3] = 0x0A;
  DWORD sent;
  WriteFile( m_hPort, data, 4, &sent, NULL );
  setInvFsk(CBInvFSK->Checked);
  strcat( ss, " baud" );
  LabelBaud->Caption = ss;
  if( m_baud != s_baud )
    LabelBaud->Font->Color = clRed;
  else
    LabelBaud->Font->Color = clBlack;
}
//---------------------------------------------------------------------------
//PTT off is called when a parameter is changed in MMTTY
void __fastcall TExtFSK::SetPTT(int sw, int msg)
{
	m_ptt = sw;
	m_fsk.SetPTT(sw);
	m_X = 0;
	if( msg ){
		if( m_WindowState == wsMinimized) return;
		Memo->Lines->Add(sw ? "PTT ON" : "PTT OFF");
	}
}
//---------------------------------------------------------------------------

void __fastcall TExtFSK::PutChar(BYTE c)
{
	if( !m_ptt ) return;
  double sleep_time;
  if( c == 0x1f || c == 0x1b )
    sleep_time = 0.0;
  else
    sleep_time = ( 1.0/(float)m_baud ) * 1000.0 * 7.45;// * 0.80;
  m_fsk.m_StgD = c;
  m_fsk.tinyIt(c);
  Sleep( (int)sleep_time );
  m_fsk.m_StgD = -1;
	if( m_WindowState == wsMinimized) return;
#ifndef PRINT_ORIGINAL_CHAR
  if( c == 0x1f ){
    m_shift_state = 0;
    return;
  }
  else if( c == 0x1b ){
    m_shift_state = 1;
    return;
  }
  c = m_fsk.baudot2ascii(c);
#endif
	char bf[128];
	if( m_X ){
		int n = Memo->Lines->Count;
		if( n ) n--;
		strcpy(bf, Memo->Lines->Strings[n].c_str());
		wsprintf(&bf[strlen(bf)], " %02X", c);
		if( !m_ptt ) return;
		Memo->Lines->Strings[n] = bf;
	}
	else {
		wsprintf(bf, "%02X", c);
		Memo->Lines->Add(bf);
	}
	m_X++;
	if( m_X >= 8 ) m_X = 0;
#if MeasureAccuracy
	int d = int(m_fsk.GetPDiff() * 100000 / m_fsk.GetPFreq());
	if( d ){
		wsprintf(bf, "%u.%02ums", d / 100, d % 100);
		Caption = bf;
	}
#endif
}
//---------------------------------------------------------------------------
void __fastcall TExtFSK::PortNameChange(TObject *Sender)
{
	if( m_DisEvent ) return;

	OpenPort();
	UpdatePort();
}
//---------------------------------------------------------------------------
void __fastcall TExtFSK::SBMinClick(TObject *Sender)
{
	if( m_DisEvent ) return;

	WindowState = wsMinimized;
	m_WindowState = wsMinimized;

	Memo->Lines->Clear();
	m_X = 0;
}
//---------------------------------------------------------------------------
void __fastcall TExtFSK::RGFSKClick(TObject *Sender)
{
	if( m_DisEvent ) return;

	if( RGFSK->ItemIndex == RGPTT->ItemIndex ){
		m_DisEvent++;
		RGPTT->ItemIndex = (RGFSK->ItemIndex != ptRTS) ? ptRTS : ptDTR;
		m_DisEvent--;
	}
	m_fsk.SetHandle(m_hPort, RGFSK->ItemIndex, RGPTT->ItemIndex);
}
//---------------------------------------------------------------------------
void __fastcall TExtFSK::RGPTTClick(TObject *Sender)
{
	if( m_DisEvent ) return;

	if( RGFSK->ItemIndex == RGPTT->ItemIndex ){
		m_DisEvent++;
		RGFSK->ItemIndex = (RGPTT->ItemIndex != ptTXD) ? ptTXD : ptRTS;
		m_DisEvent--;
	}
	m_fsk.SetHandle(m_hPort, RGFSK->ItemIndex, RGPTT->ItemIndex);
}
//---------------------------------------------------------------------------
void __fastcall TExtFSK::setInvFsk( bool b )
{
  BYTE data[4] = { '~', 0, 0x0D, 0x0A };
  DWORD sent;
  if( b ){
    data[1] = '0';
  }
  else{
    data[1] = '1';
  }
  WriteFile( m_hPort, data, 4, &sent, NULL );
}
void __fastcall TExtFSK::CBInvFSKClick(TObject *Sender)
{
	if( m_DisEvent ) return;
	//m_fsk.SetInvFSK(CBInvFSK->Checked);
  setInvFsk(CBInvFSK->Checked);
}
//---------------------------------------------------------------------------
void __fastcall TExtFSK::CBInvPTTClick(TObject *Sender)
{
	if( m_DisEvent ) return;
	m_fsk.SetInvPTT(CBInvPTT->Checked);
}
//---------------------------------------------------------------------------
void __fastcall TExtFSK::FormPaint(TObject *Sender)
{
	m_WindowState = WindowState;
}
//---------------------------------------------------------------------------

