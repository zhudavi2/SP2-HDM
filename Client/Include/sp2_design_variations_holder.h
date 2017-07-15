/**************************************************************
*
* golem_designvariationholder.h
*
* Description
* ===========
*  Describes the interface of class GDesignVariationHolder
*
* Owner
* =====
*  Nicolas Hatier
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/

#ifndef _GOLEM_DESIGNVARIATIONHOLDER_H_
#define _GOLEM_DESIGNVARIATIONHOLDER_H_

const INT32 c_iRootParentId = -1;

namespace SP2
{
   class GDesignVariationHolder
   {
   public:
      struct GType
      {
         struct GPiece
         {
            struct GVariation
            {
               GFile             Model;
               GString           DefaultTexture;
               GFile             ColorizeTexture;
               vector<UINT32>    Connectable;
            };

            GString              Name;
            INT32                ParentID;
            INT32                Colorize;
            GString              ParentName;
            vector<GVariation>   Variations;

            GPiece();
         };

         GFile                   DefaultModel;
         GPiece*                 Pieces[c_iMaxDesignPieceCount];

         GType();
      };

      ~GDesignVariationHolder();

      bool                       Load(GFile& in_File);

      GFile                   &  DefaultModel() {return m_DefaultModel;}
      GType                   *  Type(UINT32 in_iTypeID);

   protected:
   private:
      friend class GDataAccessLayerClient;
      GFile                   m_DefaultModel;
      hash_map<UINT32, GType> m_Types;
   };
}
#endif //_GOLEM_DESIGNVARIATIONHOLDER_H_
