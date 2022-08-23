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


#ifndef MainH
#define MainH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <Buttons.hpp>
#include <Inifiles.hpp>
#include <mmsystem.h>
#include <stdlib.h>
#include <stdio.h>
//---------------------------------------------------------------------------
#define VERSION "0.2"
#define NDEBUG      // Remove this symbol, if you would like to do debugging
#include <assert.h>
#define ASSERT(c)   assert(c)
//---------------------------------------------------------------------------
#define	MeasureAccuracy FALSE
#if MeasureAccuracy
#include <stdio.h>
#endif
//---------------------------------------------------------------------------
#define PRINT_ORIGINAL_CHAR TRUE
#undef PRINT_ORIGINAL_CHAR
#define ADDITIONAL_BAUDRATE TRUE
#undef ADDITIONAL_BAUDRATE
//---------------------------------------------------------------------------
template <class T> T LASTP(T p){
	int l = strlen(p);
	if( l ) p += (l-1);
	return p;
};
enum {
	ptTXD,
	ptRTS,
	ptDTR,
};
enum {
	ptDL,
	ptDH,
	ptSTROBE,
};
void __fastcall SetDirName(LPSTR t, LPCSTR pName);

//---------------------------------------------------------------------------
//typedef void (__stdcall *dlSetDriverPath)(LPSTR pPath);
//typedef void (__stdcall *dlSetDLLPath)(LPSTR pPath);
//typedef void (__stdcall *dlOpenDriver)(void);
//typedef void (__stdcall *dlCloseDriver)(void);
//typedef BOOL (__stdcall *dlActiveHW)(void);
//typedef BYTE (__stdcall *dlReadPort)(WORD Address);
//typedef void (__stdcall *dlWritePort)(WORD Address, BYTE Data);
typedef short (_stdcall *dlReadPort)(short portaddr);
typedef void (_stdcall *dlWritePort)(short portaddr, short datum);
//typedef LPSTR (__stdcall *dlLastError)(void);
//typedef BYTE (__stdcall *dlLPTNumPorts)(void);
//typedef void (__stdcall *dlSetLPTNumber)(BYTE n);
//typedef WORD (__stdcall *dlLPTBasePort)(void);

class CDLPort {
private:
	HANDLE			m_hDLib;
	//HANDLE			m_hLib;
	AnsiString		m_LastError;
	AnsiString		m_Path;

public:
	//dlLPTNumPorts	LPTNumPorts;
	//dlSetLPTNumber	SetLPTNumber;
	//dlLPTBasePort	LPTBasePort;

private:
	int __fastcall Open(void);
	void __fastcall Close(void);
	BOOL __fastcall IsFile(LPCSTR pName);

public:
	__fastcall CDLPort(LPCSTR pPath);
	__fastcall ~CDLPort();
	BOOL __fastcall IsOpen(void){return (m_hDLib!=NULL);};
};

typedef struct {	// Should not put a class into the member
	DWORD	m_dwVersion;
	int		m_WinNT;
	char	m_BgnDir[MAX_PATH];
	char	m_ModuleName[MAX_PATH];
	CDLPort	*m_pDLPort;
}SYS;
extern SYS	sys;
//---------------------------------------------------------------------------
class CFSK {
private:
	volatile HANDLE	m_hPort;

	volatile WORD	m_wPortA;
	volatile BYTE	m_bPortD;

	volatile int	m_BLen;

	volatile int	m_nFSK;
	volatile int	m_nPTT;

	volatile int	m_Sequence;
	volatile int	m_Count;
	volatile int	m_BCount;
	volatile int	m_NowD;

	volatile int	m_Idle;

	volatile int	m_invFSK;
	volatile int	m_oFSK;
	volatile int	m_aFSK;

	volatile int	m_invPTT;
	volatile int	m_oPTT;
	volatile int	m_aPTT;

	volatile int	m_ErrorAccess;
  int m_shift_state;

#if MeasureAccuracy
	LARGE_INTEGER	m_liFreq;
	LARGE_INTEGER	m_liPCur, m_liPOld;
	DWORDLONG		m_dlDiff;
#endif
private:
	inline int __fastcall IsOpen(void){ return m_hPort != INVALID_HANDLE_VALUE;};

public:
  volatile int m_Baud;
	volatile int	m_StgD;
	__fastcall CFSK(void);
	void __fastcall Init(void);
	void __fastcall Timer(void);
	void __fastcall SetPara(LONG para);
	inline void __fastcall Disable(void){m_hPort = INVALID_HANDLE_VALUE;};
	void __fastcall SetHandle(HANDLE hPort, int nFSK, int nPTT);
	void __fastcall PutByte(BYTE c){m_StgD = c;};
	void __fastcall SetPort(int port, int sw);
	void __fastcall SetPTT(int sw);
	inline int __fastcall IsBusy(void){
		return (m_StgD != -1) ? TRUE : FALSE;
	};
	inline void __fastcall SetInvFSK(int inv){
		m_invFSK = inv;
		m_aFSK = -1;
	};
	inline void __fastcall SetInvPTT(int inv){
		m_invPTT = inv;
		m_aPTT = -1;
	};
	inline void __fastcall SetDelay(int n){m_Count = n;};
	inline void __fastcall SetLPT(WORD adr){m_wPortA = adr;};
#if MeasureAccuracy
	inline DWORDLONG __fastcall GetPDiff(void){return m_dlDiff;};
	inline DWORDLONG __fastcall GetPFreq(void){return m_liFreq.QuadPart;};
#endif
  void __fastcall tinyIt( BYTE c );
  BYTE __fastcall baudot2ascii( BYTE c );
};
//---------------------------------------------------------------------------
class TExtFSK : public TForm
{
__published:	// IDE
	TMemo *Memo;
	TLabel *L1;
	TComboBox *PortName;
	TLabel *LComStat;
	TRadioGroup *RGFSK;
	TRadioGroup *RGPTT;
	TCheckBox *CBInvFSK;
	TCheckBox *CBInvPTT;
	TSpeedButton *SBMin;
  TLabel *LabelBaud;
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall PortNameChange(TObject *Sender);

	void __fastcall SBMinClick(TObject *Sender);
	void __fastcall RGFSKClick(TObject *Sender);
	void __fastcall CBInvFSKClick(TObject *Sender);
	void __fastcall CBInvPTTClick(TObject *Sender);
	void __fastcall RGPTTClick(TObject *Sender);

	
	void __fastcall FormPaint(TObject *Sender);
  //void __fastcall RadioGroupSpeedClick(TObject *Sender);
private:
	int		m_WindowState;
	int		m_DisEvent;
	int   m_ptt;
	int		m_X;
	AnsiString	m_IniName;

	TIMECAPS	m_TimeCaps;
	HANDLE	m_hPort;
	DCB		m_dcb;
	CFSK	m_fsk;
	UINT	m_uTimerID;

	WORD	m_wPortA;
	WORD	m_wLPTA;
	WORD	m_wLPT[3];

  int m_shift_state;
  int m_baud;

	void __fastcall ReadIniFile(void);
	void __fastcall WriteIniFile(void);

	inline int __fastcall IsOpen(void){ return m_hPort != INVALID_HANDLE_VALUE;};
	void __fastcall UpdateComStat(void);
	void __fastcall UpdatePort();
	void __fastcall OpenPort(void);
	BOOL __fastcall OpenPort_(void);
	void __fastcall ClosePort(void);
  void __fastcall setInvFsk( bool b );

public:
	__fastcall TExtFSK(TComponent* Owner);
	void __fastcall SetPara(LONG para);
	void __fastcall SetPTT(int sw, int msg);
	void __fastcall PutChar(BYTE c);
	inline int __fastcall IsBusy(void){return m_fsk.IsBusy();};

};
//---------------------------------------------------------------------------
#endif
