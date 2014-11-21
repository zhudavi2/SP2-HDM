/**************************************************************
*
* golem_design_pattern_observer.h
*
* Description
* ===========
*  Design Pattern Observer-Subject
*  The Observer pattern defines an one-to-many dependency between a subject 
*  object and any number of observer objects so that when the subject object 
*  changes state, all its observer objects are notified and updated automatically. 
*  The Observer pattern is also known as Dependents and Publish-Subscribe. 
*  
*
* Owner
* =====
*  Jonathan Mercier
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/

// Include Guards
#ifndef _GOLEM_DESIGN_PATTERN_OBSERVER
#define _GOLEM_DESIGN_PATTERN_OBSERVER

// Includes
namespace Hector
{
   namespace DesignPattern
   {
      // Foward declaration
      class GSubject;

      // A Notification
      typedef UINT32 GSubjectNotification;

      /*!
      * Overview of the observer class 
	   * An observer 'observe' a specific object (a subject) and gets notify when it changes.
      */
      class GObserver
      {
      public:
         /*!
         * This function is called when the subject send a notification
         * @param in_Subject: The subject that has changed, this is used when observer have multiple subject
         * @param in_Notification  : The notification, a way to express what has changed in the subject.
         */
         virtual void OnSubjectChange(GSubject&  in_Subject,const GSubjectNotification& in_Notification,void* in_pAdditionalMsg ) = 0 ;
      };

      /*!
       * Overview of the subject class 
	    * The subject notify observer when certains states changes. Observer can attach or detach themselves 
       * from and to an observer.
       **/
      class GSubject
      {
      private:
         //typedef stdext::hash_set<GObserver *> GObserverList;
         typedef list<GObserver *> GObserverList;
         typedef stdext::hash_map<GSubjectNotification, GObserverList> GNotificationMap;

         //! List of observers per notification type
         GNotificationMap        m_Observers;

      protected:

         /*!
          * This function is called when the subject want to send a notification
          * @param in_Notification     : The notification, a way to express what has changed in the subject.
          * @param in_pAdditionalMsg   : Parameter meaningful for the notification message.
          **/
         void NotifyObserver(const GSubjectNotification& in_Notification,void* in_pAdditionalMsg = NULL);

      public:
         /*!
          * Add an observer to the set. 
          * This observer will get notify on the NotifyObserver method.
          * @param in_observer  : The observer
          * @param in_vNotifications : List of notifications that the observer listens to
          **/
         void Attach(GObserver *in_observer, const vector<GSubjectNotification>& in_vNotifications);

         /*!
          * Add an observer to the set. 
          * This observer will get notify on the NotifyObserver method.
          * @param in_observer  : The observer
          * @param in_Notification : Single notification that the observer listens to
          **/
         void Attach(GObserver *in_observer, const GSubjectNotification& in_Notification);

         /*!
          * Remove an observer from the set for a specific notification.
          * @param in_observer  : The observer
          **/
         void Detach(GObserver *in_observer, const GSubjectNotification& in_Notification);

         /*!
          * Remove an observer from the set for all notifications.
          * @param in_observer  : The observer
          **/
         void Detach(GObserver *in_observer);
      };
   }
}

#endif // _GOLEM_DESIGN_PATTERN_OBSERVER

