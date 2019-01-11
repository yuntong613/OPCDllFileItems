#pragma once
#include "YOPCDevice.h"
#include "Log.h"

#include "resource.h"

class YSocketDevice :
	public YOPCDevice
{
public:
	enum{ DEVICENAME = IDS_DEVICENAME};
	YSocketDevice(LPCSTR pszAppPath);
	virtual ~YSocketDevice(void);
	virtual bool SetDeviceItemValue(CBaseItem* pAppItem);
	virtual void OnUpdate();
	virtual void HandleData(BYTE* pBuf,int nLen);
	virtual void Serialize(CArchive& ar);
	BOOL InitConfigFile(CString strFile);
	void Load(CArchive& ar);
	void LoadFloatFromFile(CArchive& ar);
	void LoadBoolFromFile(CArchive& ar);
	void LoadItems(CArchive& ar);
	int QueryOnce();
	virtual void BeginUpdateThread();
	virtual void EndUpdateThread();

	void LoadBool();
	void LoadShort();
	void LoadString();
	void LoadFloat();
	void LoadLong();

	void OutPutLog(CString strMsg);
public:
	bool bTurn;
	bool m_bStopRun;
	HANDLE m_hWorkerThread;
protected:
	long y_lUpdateTimer;
	int m_nUseLog;
	CLog m_Log;
	
	int m_nBunchSize;
};
