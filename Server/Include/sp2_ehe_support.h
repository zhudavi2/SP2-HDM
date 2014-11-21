/**************************************************************
*
* sp2_EHE_support.h
*
* Description
* ===========
*  EHE support are gameplay functions used by the EHE, mainly questions asked
*  and stuff like that.
*
* Owner
* =====
*  Jean-René Couture
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/

#ifndef _GOLEM_EHE_SUPPORT_H_
#define _GOLEM_EHE_SUPPORT_H_

using namespace EHE;

/*!
 * EHE support are gameplay functions used by the EHE, mainly questions asked
*  and stuff like that.
  **/
class GEHE_Support : public GSupportClass
{
   GEHE_Support();
   ~GEHE_Support();
public:

 
   static GEHE_Support& Instance();
   // gameplay init function
   bool Initialize();

   // pointer to this function is passed to the contextes to give them the possibility to 
   // evaluate relations
   GEntityDescription EHESystemEvaluateRelations(INT32 fromid, INT32 toid);

   // returns the number of recursive levels an entity will consider when thinking up plans
   INT32 EHESystemReturnRecursiveLevels(INT32 id);

protected:
private:
};

#define g_EHESupport       GEHE_Support::Instance()

#endif //_GOLEM_EHE_SUPPORT_H_
