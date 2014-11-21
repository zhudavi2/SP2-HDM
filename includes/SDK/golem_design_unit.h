/**************************************************************
*
* golem_unit.h
*
* Description
* ===========
*  Describes the interface of class GUnit
*
* Owner
* =====
*  Mathieu Tremblay
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/

#ifndef _GOLEM_UNIT_H_
#define _GOLEM_UNIT_H_

namespace SDK
{
   namespace Combat
   {
        namespace Design
        {

/*!
 * Class to represent a new unit design, must be derivated to add custom properties
 * to specific unit types (ex: HelicopterTransport has a member to define the amount
 * of passengers it can hold)
 **/
class GOLEM_SDK_API GUnit : public GSerializable
{
public:
   GUnit();
   virtual ~GUnit();

   //! Set the details of the unit design
   void Set(const UINT32     in_iID,      //Unique identifier
            const GString&   in_sName);

   //! Get the details of the unit design
   void Get(UINT32&            out_iID,
            GString&           out_sName) const;

   //! Get the unique identifier
   UINT32 Id() const;

   
   //! Get the name of the design
   const GString& Name() const;

   void Id   (const UINT32   in_iID);
   void Name (const GString& in_sName);

   virtual bool Serialize(GIBuffer&) const;
   virtual bool Unserialize(GOBuffer&);
	virtual REAL32 Cost() const = 0;

protected:
   UINT32      m_iId; //Unique identifier
   GString     m_sName;
   
private:
};//End class GUnit


};//End namespace design
};//End namespace Combat
};//End namespace 
#endif //_GOLEM_UNIT_H_
