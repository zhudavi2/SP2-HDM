/**************************************************************
*
* sp2_battle_overview_Unit.h
*
* Description
* ===========
*
*
* Owner
* =====
* Jonathan Mercier
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/

#ifndef _SP2_BATTLE_OVERVIEW_UNIT_H_
#define _SP2_BATTLE_OVERVIEW_UNIT_H_

namespace SP2
{

   namespace EBattleOverviewUnitSide
   {
      enum Enum
      {
         Ally,
         Enemy,
         TotalSide
      };
   };

   class G2DComponent 
   {
   protected:
      CGFX_Model_Itf*         m_pModel;
      CGFX_Transform_2D_Itf*  m_pTransform; 

   public:
      G2DComponent();
      ~G2DComponent();

      REAL32 Height(void);
      REAL32 Width(void);

      GVector2D<REAL32> Position() const;
      void SetPosition(REAL32 in_iPositionX, REAL32 in_iPositionY);
      void SetSize(REAL32 in_iWidth, REAL32 in_iHeight);
      bool IsPointInside(GVector2D<REAL32> in_Position);

      CGFX_Model_Itf*          Model(void);
      CGFX_Transform_2D_Itf*   Transform(void);

   };

   class GTextSprite : public G2DComponent
   {
   private:
      //! A Sprite is composed of a mesh, a material and some texture.
      CGFX_Text_2D_Itf*       m_pTextPrimitive;

   public:
      GTextSprite(const GString in_Font = L"Arial",UINT16 in_iSize = 10);
      ~GTextSprite();

      void Text(const GString& in_Text);
      void SetColor(const GColorRGBInt& in_TextColor);
      CGFX_Text_2D_Itf*  TextPrimitive(void);
   };



   class GSprite : public G2DComponent
   {
   public:   
      //! A Sprite is composed of a mesh, a material and some texture.
      GFX::GMesh*             m_pMesh;
      CGFX_Texture_Itf*       m_pTexture;
      GFX::GMaterial*         m_pMaterial;

      //! Constructor and Destructor
      GSprite();
      ~GSprite();

      void LoadTexture(GString in_TextureFileName);
      void LoadAnimation(GString in_TextureFileName, CGFX_Track_Itf* in_pTrack);
      void SetTexture(CGFX_Texture_Itf* in_pTexture);

   private:
      void CreateMesh(void);
   };

      namespace UnitActionID
      {
         enum Enum
         {
            Activate = 0,
            Attack,
            Idle,
            Hurt,
            Deactivate,
            Explosion,
            Total
         };
      };

   class GBattleOverviewUnit : public G2DComponent
   {
      friend class GTutorials;
   public:
      enum UnitStatus
      {
         ATTACKING = 0,
         DEFENDING,
         TOTALSTATUS
      };


   private:
      GSprite        m_StatusIcon;
      GTextSprite    m_UnitHP;
      GTextSprite    m_TotalUnitHP;
      GTextSprite    m_HitHP;
      GTextSprite    m_UnitName;

      GSprite        m_IdleUnit;
      GSprite        m_AttackUnit;
      GSprite        m_HurtUnit;
      GSprite        m_Explosion;

      // Internal Data
      INT32 m_iUnitHP;
      INT32 m_iTotalUnitHP;
      UINT32 m_iTypeID;
      EBattleOverviewUnitSide::Enum m_eUnitSide;
      UnitStatus  m_eUnitStatus;

      void SetInformationPosition(void);

      // Aciton Management variables

      REAL32 m_fCurrentActionFinishTime;

      UINT32   m_iTotalAttack;
      UINT32   m_iTotalHit;
      UINT32   m_iAccumulatedHit;

      UnitActionID::Enum   m_eCurrentAction;
      UnitActionID::Enum   m_eNextAction;
      CGFX_Track_Itf*      m_pUnitActionTrack[UnitActionID::Total];
      bool                 m_bActivated;

      // Sounds
      void  PlayAttackSound(REAL32 in_fDelay);
      void  PlayIdleSound(REAL32 in_fDelay);
      void  PlayHurtSound(REAL32 in_fDelay);
      void  PlayActionSound(UnitActionID::Enum in_eAction,REAL32 in_fTimeToPlay);


   public:
      GBattleOverviewUnit(GUnitType *in_pUnitType, EBattleOverviewUnitSide::Enum in_eSide, const GString& in_UnitAnimationFolder);
      ~GBattleOverviewUnit();

      void     CreateActions(const GString& in_UnitAnimationFolder);

      void     OnProcess(float in_fTime);

      void     Activate(void);
      void     Attack(GBattleOverviewUnit *in_pTargetUnit);
      void     ReceivedHit(void);
      void     UpdateHP(void);
      void     Deactivate(void);

      void     UnitHP(UINT32 in_iUnitHP);
      UINT32   UnitHP(void);
      void     TotalUnitHP(UINT32 in_iTotalUnit);
      UINT32   TotalUnitHP(void);
      void     AccumulatedHit(UINT32 in_iTotalAccumulatedHit);
      UINT32   AccumulatedHit(void);
      void     HideTotalHP(bool in_bHideTotalHP);



      void     Selected(bool in_bUnitSelected);
      bool     IsStatusIconClick(GVector2D<REAL32> in_MousePositionInSpriteSpace);
      bool     IsInsideMouseClick(GVector2D<REAL32> in_MousePosition);
      void     Status(UnitStatus in_eStatus);
      
      UnitStatus Status(void);
      void     ResetHP(void);
      void     Reset(void);
      bool     Activated(void);
      UINT32   TypeID(void);
      EBattleOverviewUnitSide::Enum Side(void);
   };

} // End of Namespace SP2

#endif //_SP2_BATTLE_OVERVIEW_UNIT_H_