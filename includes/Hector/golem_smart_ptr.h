/*
* Smart Pointer Template class
* simple reference counted pointer.
*
* The is a non-intrusive implementation that allocates an additional
* int and pointer for every counted object.
*
* 
* http://ootips.org/yonat/4dev/smart-pointers.html
*/



#ifndef _GOLEM_SMART_PTR_H
#define _GOLEM_SMART_PTR_H
#include "golem_mutex.h"

namespace Hector
{

   template <class X>
   class GSmartPtr
   {
   public:
      typedef X element_type;

      /*explicit counted_ptr(X* p = 0) // allocate a new counter
      : itsCounter(0) {if (p) itsCounter = new counter(p);}*/

      explicit GSmartPtr(X* p = 0)
      {
         itsCounter = new counter(p);
      }

      ~GSmartPtr()
      {
         release();
      }

      GSmartPtr(const GSmartPtr& r) throw()
      {
         acquire(r.itsCounter);
      }

      GSmartPtr& operator=(const GSmartPtr& r)
      {
         if (this != &r) 
         {
            release();
            acquire(r.itsCounter);
         }
         return *this;
      }

      bool operator==(const GSmartPtr& r) const
      {
         if(get() == r.get())
            return true;
         else
            return false;
      }

      bool operator!=(const GSmartPtr& r) const
      {
         if(get() != r.get())
         {
            return true;
         }
         else
            return false;
      }

      bool operator < (const GSmartPtr& r) const
      {
         if(get() < r.get())
            return true;
         else
            return false;
      }

      bool operator > (const GSmartPtr& r) const
      {
         if(get() > r.get())
            return true;
         else
            return false;
      }

      bool operator <= (const GSmartPtr& r) const
      {
         if(get() <= r.get())
            return true;
         else
            return false;
      }

      bool operator >= (const GSmartPtr& r) const
      {
         if(get() >= r.get())
            return true;
         else
            return false;
      }

      operator size_t() const
      {
         return reinterpret_cast<size_t>(get());
      }


      X& operator*() throw()   
      {
         return *itsCounter->ptr;
      }

      X* operator->() throw()   
      {
         return itsCounter->ptr;
      }
      
      const X& operator*()  const throw()   
      {
         return *itsCounter->ptr;
      }

      const X* operator->() const throw()   
      {
         return itsCounter->ptr;
      }

      X* get() throw()   
      {
         return itsCounter ? itsCounter->ptr : 0;
      }

      const X* get() const throw()   
      {
         return itsCounter ? itsCounter->ptr : 0;
      }

      operator X*&()
      {      
         return itsCounter->ptr; 
      }

      operator const X* () const
      {      
         return itsCounter->ptr; 
      }

      bool unique() const throw()
      {
         return (itsCounter ? itsCounter->count == 1 : true);
      }

  void IncRef()
      {
         // Increment the counter
         if (itsCounter) 
         {
            ++itsCounter->count;
         }
      }

   private:


      struct counter 
      {
         counter(X* p = 0, unsigned c = 1) : ptr(p), count(c) {}
         X*          ptr;
         unsigned    count;
      }* itsCounter;

      void acquire(counter* c) throw()
      { // increment the count
         itsCounter = c;
         if (c) ++c->count;
      }

      void release()
      { // decrement the count, delete if it is 0
         if (itsCounter) 
         {
            if (--itsCounter->count == 0) 
            {
               X* objPtr = itsCounter->ptr;
               SAFE_DELETE(objPtr);
               SAFE_DELETE(itsCounter);
               itsCounter = NULL;
            }
            itsCounter = 0;
         }
      }
   };

   typedef GSmartPtr<UINT8> GSPtrUINT8;
   typedef GSmartPtr<INT8> GSPtrINT8;
}

#endif