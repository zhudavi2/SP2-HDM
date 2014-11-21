/**************************************************************
*
* golem_mod.h
*
* Description
* ===========
*  GMod, structure that contains every information about a MOD
*
* Owner
* =====
*  Frederic O'Reilly
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#ifndef _GOLEM_MOD_H_
#define _GOLEM_MOD_H_

namespace SDK
{
   struct GMod
   {
      GString        m_sName;
      GString        m_sID;
      GString        m_sPath;
      GString        m_sVersion;
      GMod           *m_pParentMod;
      UINT8          m_iVersionMajor;
      UINT8          m_iVersionMinor;
      UINT8          m_iVersionRevision;
      GMD5Signature  m_Signature;
   };
}

#endif
