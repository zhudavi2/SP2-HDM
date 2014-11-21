/**************************************************************
*
* golem_template.h
*
* Description
* ===========
*  Describes the interface of class GTemplate
*
* Owner
* =====
*  Mathieu Tremblay
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/

#ifndef _GOLEM_TEMPLATE_H_
#define _GOLEM_TEMPLATE_H_

namespace Event
{

   /*!
   * [Class description]
   **/
   class GTemplate : public GGameEvent
   {
   private:
      GTemplate();
   protected:
   public:

      ~GTemplate();

      //Serialization functions
      virtual bool Serialize(GIBuffer&) const;
      virtual bool Unserialize(GOBuffer&);

      //! GTemplate construction function
      static GGameEventSPtr New();

   };


}; //End of namespace Event

#endif //_GOLEM_TEMPLATE_H_
