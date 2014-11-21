#include "golem_sdk_api.h"
/**************************************************************
*
* golem_picture_box.h
*
* Description
* ===========
*  Describes the interface of class GPictureBox
*  The GPicture box is a generic picture container for the GUI
*
* Owner
* =====
*  Mathieu Tremblay
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/

#ifndef _GOLEM_PICTURE_BOX_H_
#define _GOLEM_PICTURE_BOX_H_

/*!
* Generic container for a picture in the gui objects
**/
namespace SDK
{
   namespace GUI
   {
      class GOLEM_SDK_API GPictureBox : public GFrame
      {
      public:

         static const GString TypeName;
      protected:
         friend class GUI::GManager;
         static GUI::GBaseObject* New();

         GPictureBox();
         virtual ~GPictureBox();

      private:
      };
   }
}
#endif //_GOLEM_PICTURE_BOX_H_
