#include "StdAfx.h"
#include "common.h"
#include ".\YSocketDevice.h"
#include "OPCIniFile.h"
#include "ItemBrowseDlg.h"
#include "YSocketItem.h"
#include "ModelDll.h"
#include "IniFile.h"

extern CModelDllApp theApp;

YSocketDevice::YSocketDevice(LPCSTR pszAppPath)
{
	bTurn = false;
	m_bStopRun = false;
	m_hWorkerThread = INVALID_HANDLE_VALUE;

	m_nUseLog = 0;
	CString strConfigFile(pszAppPath);
	strConfigFile += _T("\\EMS.ini");
	InitConfigFile(strConfigFile);

	COPCIniFile file;
	if (file.Open(strConfigFile, CFile::modeRead))
	{
		CArchive ar(&file, CArchive::load);
		LoadBoolFromFile(ar);
		LoadFloatFromFile(ar);
		file.Close();
	}
	YOPCItem* pItem = GetItemById(1054);
	// 	LoadBool();
	// 	LoadFloat();
	// 	LoadLong();
	// 	LoadShort();
	// 	LoadString();
}

YSocketDevice::~YSocketDevice(void)
{
	POSITION pos = y_ItemArray.GetStartPosition();
	YSocketItem* pItem = NULL;
	CString strName;
	while (pos) {
		y_ItemArray.GetNextAssoc(pos, strName, (CObject*&)pItem);
		if (pItem)
		{
			delete pItem;
			pItem = NULL;
		}
	}
	y_ItemArray.RemoveAll();
}

void YSocketDevice::Serialize(CArchive& ar)
{
	if (ar.IsStoring()) {
		//		Store(ar);
	}
	else {
		Load(ar);
	}
}

BOOL YSocketDevice::InitConfigFile(CString strFile)
{
	CIniFile iniFile(strFile);

	m_bUpdate = iniFile.GetBool("param", "Update", TRUE);

	m_lRate = iniFile.GetUInt("param", "UpdateRate", 5000);

	y_lUpdateTimer = m_lRate / 1000;

	m_nUseLog = iniFile.GetUInt("LogInfo", "Enable", 0);

	m_strDeviceDescription = iniFile.GetString("param", "DevDesc", "SIM");

	m_nBunchSize = iniFile.GetUInt("param", "UpdateSize", 10);
	return TRUE;
}

void YSocketDevice::Load(CArchive& ar)
{
	LoadItems(ar);
}


void YSocketDevice::LoadFloatFromFile(CArchive& ar)
{
	COPCIniFile* pIniFile = static_cast<COPCIniFile*>(ar.GetFile());
	YOPCItem* pItem = NULL;
	int nItems = 0;
	CString strTmp = ("FloatItemList");
	int dwItemId = 0;
	CString strItemName, strItemDesc, strValue;

	if (pIniFile->ReadNoSeqSection(strTmp)) {
		nItems = pIniFile->GetItemsCount(strTmp, "Item");
		for (int i = 0; i < nItems && !pIniFile->Endof(); i++)
		{
			try {
				if (pIniFile->ReadIniItem("Item", strTmp))
				{
					if (!pIniFile->ExtractSubValue(strTmp, strValue, 1))
						throw new CItemException(CItemException::invalidId, pIniFile->GetFileName());
					dwItemId = atoi(strValue);

					dwItemId = GetItemsCount() + 1;

					if (!pIniFile->ExtractSubValue(strTmp, strItemName, 2))strItemName = _T("Unknown");
					if (!pIniFile->ExtractSubValue(strTmp, strItemDesc, 3)) strItemDesc = _T("Unknown");
					pItem = new YFloatItem(dwItemId, strItemName, strItemDesc);
					if (GetItemByName(pItem->GetName()))
						delete pItem;
					else
						y_ItemArray.SetAt(pItem->GetName(), (CObject*)pItem);
				}
			}
			catch (CItemException* e) {
				if (pItem) {
					delete pItem;
					pItem = NULL;
				}
				e->Delete();
			}
		}
	}
}

void YSocketDevice::LoadBoolFromFile(CArchive& ar)
{
	COPCIniFile* pIniFile = static_cast<COPCIniFile*>(ar.GetFile());
	YOPCItem* pItem = NULL;
	int nItems = 0;
	CString strTmp = ("BoolItemList");
	int dwItemId = 0;
	CString strItemName, strItemDesc, strValue;

	if (pIniFile->ReadNoSeqSection(strTmp)) {
		nItems = pIniFile->GetItemsCount(strTmp, "Item");
		for (int i = 0; i < nItems && !pIniFile->Endof(); i++)
		{
			try {
				if (pIniFile->ReadIniItem("Item", strTmp))
				{
					if (!pIniFile->ExtractSubValue(strTmp, strValue, 1))
						throw new CItemException(CItemException::invalidId, pIniFile->GetFileName());
					dwItemId = atoi(strValue);
					dwItemId = GetItemsCount() + 1;
					if (!pIniFile->ExtractSubValue(strTmp, strItemName, 2))strItemName = _T("Unknown");
					if (!pIniFile->ExtractSubValue(strTmp, strItemDesc, 3)) strItemDesc = _T("Unknown");
					pItem = new YBoolItem(dwItemId, strItemName, strItemDesc);
					if (GetItemByName(pItem->GetName()))delete pItem;
					else y_ItemArray.SetAt(pItem->GetName(), (CObject*)pItem);
				}
			}
			catch (CItemException* e) {
				if (pItem) {
					delete pItem;
					pItem = NULL;
				}
				e->Delete();
			}
		}
	}
}

void YSocketDevice::LoadItems(CArchive& ar)
{
	COPCIniFile* pIniFile = static_cast<COPCIniFile*>(ar.GetFile());
	YOPCItem* pItem = NULL;
	int nItems = 0;
	CString strTmp = ("ItemList");
	int dwItemId = 0;
	CString strItemName, strItemDesc, strValue;

	if (pIniFile->ReadNoSeqSection(strTmp)) {
		nItems = pIniFile->GetItemsCount(strTmp, "Item");
		for (int i = 0; i < nItems && !pIniFile->Endof(); i++)
		{
			try {
				if (pIniFile->ReadIniItem("Item", strTmp))
				{
					if (!pIniFile->ExtractSubValue(strTmp, strValue, 1))
						throw new CItemException(CItemException::invalidId, pIniFile->GetFileName());
					dwItemId = atoi(strValue);
					if (!pIniFile->ExtractSubValue(strTmp, strItemName, 2))strItemName = _T("Unknown");
					if (!pIniFile->ExtractSubValue(strTmp, strItemDesc, 3)) strItemDesc = _T("Unknown");
					pItem = new YShortItem(dwItemId, strItemName, strItemDesc);
					if (GetItemByName(pItem->GetName()))delete pItem;
					else y_ItemArray.SetAt(pItem->GetName(), (CObject*)pItem);
				}
			}
			catch (CItemException* e) {
				if (pItem) {
					delete pItem;
					pItem = NULL;
				}
				e->Delete();
			}
		}
	}
}

void YSocketDevice::LoadBool()
{
	YOPCItem* pItem = NULL;
	CString strItemName;
	CString strItemDesc;
	CString strValue;
	DWORD dwItemIId = 0L;
	for (int i = 1; i < 1000; i++)
	{
		dwItemIId = i;
		strItemName.Format("%s.P.%05d.B", m_strDeviceDescription, i);
		strItemDesc.Format("Bool%05d", i);
		pItem = new YBoolItem(dwItemIId, strItemName, strItemDesc);
		if (GetItemByName(pItem->GetName()))delete pItem;
		else y_ItemArray.SetAt(pItem->GetName(), (CObject*)pItem);
	}
}

void YSocketDevice::LoadShort()
{
	YOPCItem* pItem = NULL;
	CString strItemName;
	CString strItemDesc;
	CString strValue;
	DWORD dwItemIId = 0L;

	for (int i = 2000; i < 3000; i++)
	{
		dwItemIId = i;
		strItemName.Format("%s.P.%05d.S", m_strDeviceDescription, i);
		strItemDesc.Format("Short%05d", i);
		pItem = new YShortItem(dwItemIId, strItemName, strItemDesc);
		if (GetItemByName(pItem->GetName()))delete pItem;
		else y_ItemArray.SetAt(pItem->GetName(), (CObject*)pItem);
	}
}

void YSocketDevice::LoadString()
{
	YOPCItem* pItem = NULL;
	CString strItemName;
	CString strItemDesc;
	CString strValue;
	DWORD dwItemIId = 0L;

	for (int i = 3000; i < 4000; i++)
	{
		dwItemIId = i;
		strItemName.Format("%s.P.%05d.C", m_strDeviceDescription, i);
		strItemDesc.Format("String%05d", i);
		pItem = new YStringItem(dwItemIId, strItemName, strItemDesc);
		if (GetItemByName(pItem->GetName()))delete pItem;
		else y_ItemArray.SetAt(pItem->GetName(), (CObject*)pItem);
	}
}

void YSocketDevice::LoadFloat()
{
	YOPCItem* pItem = NULL;
	CString strItemName;
	CString strItemDesc;
	CString strValue;
	DWORD dwItemIId = 0L;

	for (int i = 1000; i < 2000; i++)
	{
		dwItemIId = i;
		strItemName.Format("%s.P.%05d.F", m_strDeviceDescription, i);
		strItemDesc.Format("Float%05d", i);
		pItem = new YFloatItem(dwItemIId, strItemName, strItemDesc);
		if (GetItemByName(pItem->GetName()))delete pItem;
		else y_ItemArray.SetAt(pItem->GetName(), (CObject*)pItem);
	}
}

void YSocketDevice::LoadLong()
{
	YOPCItem* pItem = NULL;
	CString strItemName;
	CString strItemDesc;
	CString strValue;
	DWORD dwItemIId = 0L;

	for (int i = 4000; i < 5000; i++)
	{
		dwItemIId = i;
		strItemName.Format("%s.P.%05d.L", m_strDeviceDescription, i);
		strItemDesc.Format("Long%05d", i);
		pItem = new YLongItem(dwItemIId, strItemName, strItemDesc);
		if (GetItemByName(pItem->GetName()))delete pItem;
		else y_ItemArray.SetAt(pItem->GetName(), (CObject*)pItem);
	}
}

void YSocketDevice::OnUpdate()
{
	if (!m_bUpdate)return;
	y_lUpdateTimer--;
	if (y_lUpdateTimer > 0)return;
	y_lUpdateTimer = m_lRate / 1000;
	QueryOnce();
}

int YSocketDevice::QueryOnce()
{
	CString strValue;
	CMapWordToOb pArray;
	HWND hWnd;
	GET_EXE_HWND(hWnd);
	int n = 0;
	YOPCItem* pItem = NULL;
	YOPCItem* pFind = NULL;
	int nNum = 0;
	int nType = 0;
	do
	{
		if (m_bStopRun)
			return 0;

		pFind = NULL;
		nNum = (rand() % GetItemsCount());
		if (!pArray.Lookup(nNum, (CObject*&)pFind))
		{
			pItem = GetItemById(nNum);
			if (pItem == NULL)
				continue;
			pItem->GetTypeString(&nType);
#pragma region 更新值选项

			switch (nType)
			{
			case 0://BOOL
			{
				if (!bTurn)
					pItem->SetVarValue("FALSE");
				else
					pItem->SetVarValue("TRUE");
			}
			break;
			case 1://"BYTE"
			{

			}
			break;
			case 2://"Int"
			{
				if (!bTurn)
					pItem->SetVarValue("0");
				else
					pItem->SetVarValue("1");
			}
			break;
			case 3:// SHORT3
			{
				if (!bTurn)
					pItem->SetVarValue("0");
				else
					pItem->SetVarValue("1");
			}
			break;
			case 4:// LONG 4
			{
				if (!bTurn)
				{
					strValue.Format("%d", (rand() % 100));
				}
				else {
					strValue.Format("%d", (rand() % 100));
				}
				pItem->SetVarValue(strValue);
			}
			break;
			case 5:// FLOAT
			{
				if (!bTurn)
				{
					strValue.Format("%f", (float)(rand() % 1000) / 100.0f);
				}
				else {
					strValue.Format("%f", (float)(rand() % 1000) / 100.0f);
				}
				strValue.Format("%f", atof(strValue) + pItem->GetFloatValue());
				pItem->SetVarValue(strValue);
			}
			break;
			case 6:// DOUBLE 6
			{
				if (!bTurn)
				{
					strValue.Format("%f", (float)(rand() % 5000) / 100.0f);
				}
				else {
					strValue.Format("%f", (float)(rand() % 5000) / 100.0f);
				}
				pItem->SetVarValue(strValue);
			}
			break;
			case 7://BSTR 7
			{
				if (!bTurn) {
					strValue.Format("%d", (rand() % 10) * 1111);
					pItem->SetVarValue(strValue);
				}
				else {
					strValue.Format("%d", (rand() % 10) * 1111);
					pItem->SetVarValue(strValue);
				}
			}
			break;
			}
#pragma endregion 更新值选项
			pArray.SetAt(nNum, (CObject*)pItem);
			::SendMessage(hWnd, CW_UPDATEITEMVALUE, DIC_ITEM_UPDATE, (LPARAM)pItem);
		}
	} while (pArray.GetCount() != m_nBunchSize);



	bTurn = !bTurn;

	return 0;
}

DWORD __stdcall WorkThread(LPVOID pParam)
{
	YSocketDevice* pThis = (YSocketDevice*)pParam;

	while (!pThis->m_bStopRun)
	{
		pThis->OnUpdate();
		Sleep(1000);
	}
	return 0;
}

void YSocketDevice::BeginUpdateThread()
{
	m_bStopRun = false;
	m_hWorkerThread = ::CreateThread(NULL, 0, WorkThread, this, 0, NULL);
}

void YSocketDevice::EndUpdateThread()
{
	if (m_hWorkerThread != INVALID_HANDLE_VALUE)
	{
		m_bStopRun = true;
		WaitForSingleObject(m_hWorkerThread, INFINITE);
		m_hWorkerThread = INVALID_HANDLE_VALUE;
	}

}

void YSocketDevice::HandleData(BYTE* pBuf, int nLen)
{

}

bool YSocketDevice::SetDeviceItemValue(CBaseItem* pAppItem)
{
	YOPCItem* ppItem = (YOPCItem*)pAppItem;
	YOPCItem* pItem = GetItemById(ppItem->m_dwItemID);
	if (pItem == NULL)return false;
	int nType = 0; CString strType;
	CString strValue;
	strType = pAppItem->GetTypeString(&nType);
	if (strType.CompareNoCase("Short") == 0)
	{
		strValue.Format("%d", ppItem->m_vItemValue.iVal);
		pItem->OnUpdate(strValue);
	}
	if (strType.CompareNoCase("Bool") == 0)
	{
		if (VARIANT_TRUE == ppItem->m_vItemValue.boolVal)
			pItem->OnUpdate("TRUE");
		else
			pItem->OnUpdate("FALSE");
	}
	if (strType.CompareNoCase("Float") == 0)
	{
		strValue.Format("%g", ppItem->m_vItemValue.fltVal);
		pItem->OnUpdate(strValue);
	}
	if (strType.CompareNoCase("String") == 0)
	{
		strValue.Format("%s", ppItem->m_vItemValue.bstrVal);
		pItem->OnUpdate(strValue);
	}
	if (strType.CompareNoCase("Long") == 0)
	{
		strValue.Format("%d", ppItem->m_vItemValue.lVal);
		pItem->OnUpdate(strValue);
	}
	return true;
}

void YSocketDevice::OutPutLog(CString strMsg)
{
	if (m_nUseLog)
		m_Log.Write(strMsg);
}