#include "StdAfx.h"
#include ".\yopcitem.h"

YOPCItem::YOPCItem(void)
{
}

YOPCItem::~YOPCItem(void)
{
}

long YOPCItem::GetLongValue()
{
	if (m_vItemValue.vt != VT_I4) return false;
	long iRet = m_vItemValue.lVal;
	return iRet;
}

float YOPCItem::GetFloatValue()
{
	if (m_vItemValue.vt != VT_R4) return false;
	float fRet = m_vItemValue.fltVal;
	return fRet;
}
