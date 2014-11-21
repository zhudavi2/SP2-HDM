/**************************************************************
*
* sp2_battle_overview_unit.cpp
*
* Description
* ===========
*  See corresponding .h file
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/

// Includes
#include "golem_pch.hpp"
#include "..\include\sp2_battle_overview_unit.h"


// Constant
const GColorRGBInt   c_DefaultTextColor(255, 255, 255, 255);


/*******************************************************************/
/* G2DComponent
/*******************************************************************/
G2DComponent::G2DComponent()
{
   // Create model and transforms
   m_pModel       = g_Joshua.Renderer()->Create_Model();
   m_pTransform   = g_Joshua.Renderer()->Create_Transform_2D();

   // Setup Transform 
   m_pTransform->Position().X = 0;
   m_pTransform->Position().Y = 0;
   m_pTransform->Size().X = 0;
   m_pTransform->Size().Y = 0;

   // Assign it
   m_pModel->Set_Transform(m_pTransform);
}

bool G2DComponent::IsPointInside(GVector2D<REAL32> in_Position)
{
   return ( in_Position.x > m_pTransform->Position().X && in_Position.x < m_pTransform->Position().X + m_pTransform->Size().X && 
            in_Position.y > m_pTransform->Position().Y && in_Position.y < m_pTransform->Position().Y + m_pTransform->Size().Y);
}

G2DComponent::~G2DComponent()
{
   SAFE_RELEASE(m_pTransform);
   SAFE_RELEASE(m_pModel);
} 

REAL32 G2DComponent::Height(void)
{
   return m_pTransform->Size().Y;
}

REAL32 G2DComponent::Width(void)
{
   return m_pTransform->Size().X;
}

GVector2D<REAL32> G2DComponent::Position() const
{
   GVector2D<REAL32> l_Position;
   l_Position.x =    m_pTransform->Position().X;
   l_Position.y =    m_pTransform->Position().Y;
   return l_Position;
}

CGFX_Model_Itf* G2DComponent::Model(void)
{
   return m_pModel;
}


CGFX_Transform_2D_Itf*   G2DComponent::Transform(void)
{
   return m_pTransform;
}

void G2DComponent::SetPosition(REAL32 in_iPositionX, REAL32 in_iPositionY)
{
   m_pTransform->Position().X = in_iPositionX;
   m_pTransform->Position().Y = in_iPositionY;
}

void G2DComponent::SetSize(REAL32 in_iWidth, REAL32 in_iHeight)
{
   m_pTransform->Size().X = in_iWidth;
   m_pTransform->Size().Y = in_iHeight;
}


/*******************************************************************/
/* GTextSprite
/*******************************************************************/
GTextSprite::GTextSprite(const GString in_Font,UINT16 in_iSize)
{
   // Create Text 2D
   m_pTextPrimitive = g_Joshua.Renderer()->Create_Text_2D();

   // Assign this primitive to this model
   m_pModel->Set_Primitive(m_pTextPrimitive);

   // Init Text to empty String
   m_pTextPrimitive->Init(in_Font, in_iSize,false);
   m_pTextPrimitive->Color() = c_DefaultTextColor;
   m_pTextPrimitive->Text("");

   // Create Transforms
   m_pTransform->Size().X = (REAL32) m_pTextPrimitive->Width();
   m_pTransform->Size().Y = (REAL32) m_pTextPrimitive->Height();
}

GTextSprite::~GTextSprite()
{
   SAFE_RELEASE(m_pTextPrimitive);
}



void GTextSprite::Text(const GString& in_Text)
{
   m_pTextPrimitive->Text(in_Text);
   m_pTransform->Size().X = (REAL32) m_pTextPrimitive->Width();
   m_pTransform->Size().Y = (REAL32) m_pTextPrimitive->Height();
}

void GTextSprite::SetColor(const GColorRGBInt& in_TextColor)
{
    m_pTextPrimitive->Color() = in_TextColor;
}

CGFX_Text_2D_Itf*  GTextSprite::TextPrimitive(void)
{
   return m_pTextPrimitive;
}


/*******************************************************************/
/* GSprite
/*******************************************************************/

GSprite::GSprite()
{
   // Set texture pointer to null
   m_pTexture = NULL;

   // Create Mesh
   CreateMesh();
   m_pModel->Set_Primitive(m_pMesh);

   // Create Material
   GFX::GCascadePixelShader* l_pPS;

   m_pMaterial = g_Joshua.Renderer()->Helper()->CreateCompleteMaterial(GFX::VSTYPE_DEF_NO_LIGHT, GFX::PSTYPE_DEF_SIMPLE_TXTR, NULL, &l_pPS);
   m_pMaterial->RenderStates(0)->Value(GFX::RSTATE_ZBUFFER_WRITE_ENABLE)= false;
   m_pMaterial->RenderStates(0)->Value(GFX::RSTATE_ALPHA_BLEND_ENABLE)  = true;

   m_pMaterial->RenderStates(0)->Value(GFX::RSTATE_ALPHA_SOURCE_BLEND)  = GFX::BLEND_SRCALPHA;
   m_pMaterial->RenderStates(0)->Value(GFX::RSTATE_ALPHA_DEST_BLEND)    = GFX::BLEND_INVSRCALPHA;

   m_pMaterial->RenderStates(0)->Value(GFX::RSTATE_ALPHA_TEST_ENABLE)   = true;
   m_pMaterial->RenderStates(0)->Value(GFX::RSTATE_ALPHA_TEST_REFERENCE)= 16;

   l_pPS->ColorFactor() = GColorRGBReal(0.5f, 0.5f, 0.5f, 1);

   l_pPS->Stage(0)->ColorArgument2(GFX::ARG_COLOR_FACTOR);
   l_pPS->Stage(0)->ColorOperation(GFX::OPERATOR_MULTIPLY_2X);
   l_pPS->Stage(0)->AlphaArgument2(GFX::ARG_COLOR_FACTOR);

   m_pModel->Set_Material(m_pMaterial);

   SAFE_RELEASE(l_pPS);

}

GSprite::~GSprite()
{
   // Release its texture
   SAFE_RELEASE(m_pTexture);

   // Release its Material
   SAFE_RELEASE(m_pMaterial);

   // Release the mesh
   SAFE_RELEASE(m_pMesh);
}



void GSprite::SetTexture(CGFX_Texture_Itf* in_pTexture)
{
   gassert(in_pTexture,"Texture pointer is null");

   // If we have a texture on that sprite, remove it.
   if(m_pTexture)
   {
      SAFE_RELEASE(m_pTexture);
      m_pTexture = NULL;
   }
   
   // Assign new texture
   m_pTexture = in_pTexture;
   m_pTexture->Inc_Ref();

   const UINT32 NB_SPRITE_VERTICES = 4;

   const GVector2D<REAL32> c_SizeInv(1.f / m_pTexture->Width(), 1.f / m_pTexture->Height() );
   const GVector2D<REAL32> c_UVOffset(0, 0);
   const GVector2D<REAL32> c_UVSize(c_UVOffset.x + m_pTexture->PartialWidth()  * c_SizeInv.x,
      c_UVOffset.y + m_pTexture->PartialHeight() * c_SizeInv.y);

   const REAL32 c_fSpriteUVs[NB_SPRITE_VERTICES][2] =
   {
      { c_UVOffset.x,   c_UVOffset.y },
      { c_UVOffset.x,   c_UVSize.y   },
      { c_UVSize.x,     c_UVOffset.y },
      { c_UVSize.x,     c_UVSize.y   },
   };

   m_pMesh->Shape().NbTxtrs(1);
   m_pMesh->Shape().SetupUVCoords(0, (GVector2D<REAL32>*) c_fSpriteUVs, NB_SPRITE_VERTICES);

   // Set Sprite size with texture size
   m_pTransform->Size().X = (REAL32) m_pTexture->PartialWidth();
   m_pTransform->Size().Y = (REAL32) m_pTexture->PartialHeight();

   // Setup Texture in Material
   m_pMaterial->PixelShader(0)->TextureSampler(0)->Texture(m_pTexture);
   m_pMaterial->PixelShader(0)->TextureSampler(0)->MagnificationFilter(GFX::FILTER_POINT);
   m_pMaterial->PixelShader(0)->TextureSampler(0)->AddressingModeU(GFX::ADDRESS_CLAMP);
   m_pMaterial->PixelShader(0)->TextureSampler(0)->AddressingModeV(GFX::ADDRESS_CLAMP);
}

void GSprite::LoadTexture(GString in_TextureFileName)
{
   // Try to open the file
   GFile l_File;
   if(g_Joshua.FileManager()->File(in_TextureFileName, l_File) )
   {
      // Create the texture
      CGFX_Texture_Itf* l_pTxtr = 
         g_Joshua.Renderer()->TextureManager()->Create_Texture(l_File, true, in_TextureFileName);
      SetTexture(l_pTxtr);

      SAFE_RELEASE(l_pTxtr);
   }
   else
   {
      gassert(false,"Texture not found");
   }
}


void GSprite::LoadAnimation(GString in_AnimationFileName,CGFX_Track_Itf* in_pTrack)
{
   // Validate some input
   gassert(in_pTrack,"Invalid track pointer");

   // Try to open the file
   GFile l_File;
   if(g_Joshua.FileManager()->File(in_AnimationFileName, l_File) )
   {

      // Load animated sequence
      CGFX_Texture_Itf* l_pCombatTxtr =g_Joshua.Renderer()->Helper()->LoadTextureSequence(in_AnimationFileName,true);
       
      // Setup texture with frame zero
      SetTexture(( (GFX::GTextureSequence*) l_pCombatTxtr)->Frame(0));

      m_pMaterial->PixelShader(0)->TextureSampler(0)->Texture(l_pCombatTxtr);

      // Get Sequence
      GFX::GTextureSequence* l_pCombatSequence = (GFX::GTextureSequence*) l_pCombatTxtr;

      CGFX_Animation_Linear_Itf* l_pCombatAnim =
         g_Joshua.Renderer()->Helper()->AnimateTextureSequence( l_pCombatSequence,
         l_pCombatSequence->Controller(),   30.0f);

      CGFX_Action_Itf* l_pCombatAction = g_Joshua.Renderer()->Create_Action();
      l_pCombatAction->Add_Animation(l_pCombatAnim);
      l_pCombatAction->Length(l_pCombatAnim->Key(l_pCombatAnim->Get_Nb_Keys() - 1).m_Time);

      in_pTrack->Set_Action(l_pCombatAction);
      in_pTrack->Playback_Count(1);

      SAFE_RELEASE(l_pCombatAnim);
      SAFE_RELEASE(l_pCombatAction);
      SAFE_RELEASE(l_pCombatTxtr);
   }
   else
   {
      gassert(false,"Texture not found");
   }
}

void GSprite::CreateMesh(void)
{
   // Create the mesh
   m_pMesh = g_Joshua.Renderer()->CreateMesh();

   const UINT32 NB_SPRITE_VERTICES = 4;

   static REAL32 s_fSpritePositions[NB_SPRITE_VERTICES][3] =
   {
      { 0.f,  0.f,  0.f},
      { 0.f,  1.0f,  0.f},
      { 1.f,  0.f,  0.f},
      { 1.f,  1.0f,  0.f},
   };

   const UINT32 NB_SPRITE_TRIANGLES = 2;

   static UINT32 s_iSpriteTris[NB_SPRITE_TRIANGLES][3] =
   {
      {0, 3, 1},
      {0, 2, 3},
   };

   m_pMesh->Shape().SetupPositions( (GVector3D<REAL32>*) s_fSpritePositions, NB_SPRITE_VERTICES);
   m_pMesh->NbFacetGroups(1);
   m_pMesh->FacetGroup(0).Facets().SetupIndexes( (UINT32*) s_iSpriteTris, NB_SPRITE_TRIANGLES * 3);
}


GBattleOverviewUnit::GBattleOverviewUnit(GUnitType *in_pUnitType, EBattleOverviewUnitSide::Enum in_eSide, const GString& in_UnitAnimationFolder)
{
   // Initialize variable
   m_iTotalUnitHP    = 0;
   m_iUnitHP         = 0;
   m_iTypeID         = 0;
   m_iAccumulatedHit = 0;
   m_iTotalAttack    = 0;
   m_iTotalHit       = 0;
   m_bActivated      = false;

   // Set Children
   m_pModel->Add_Child(m_IdleUnit.Model());
   m_pModel->Add_Child(m_AttackUnit.Model());
   m_pModel->Add_Child(m_HurtUnit.Model());
   m_pModel->Add_Child(m_Explosion.Model());
   m_pModel->Add_Child(m_UnitHP.Model());
   m_pModel->Add_Child(m_TotalUnitHP.Model());
   m_pModel->Add_Child(m_StatusIcon.Model());
   m_pModel->Add_Child(m_UnitName.Model());
   m_pModel->Add_Child(m_HitHP.Model());

   // Set unit side
   m_eUnitSide = in_eSide;

   // Set Unit Type
   m_iTypeID = in_pUnitType->Id()-1;

   // Set Label
   m_UnitHP.Text("0");
   m_TotalUnitHP.Text("0");

   // Set Unit Name
   m_UnitName.Text(in_pUnitType->Name());

   // Create Infos
   m_StatusIcon.LoadTexture("data\\Combat_Overview\\UI\\Attack.tga");

   CreateActions(in_UnitAnimationFolder);
   if(in_pUnitType->Category() != EUnitCategory::Air)
   {
      // Replace Action position 
      if(m_eUnitSide  == EBattleOverviewUnitSide::Ally)
      { 
         m_AttackUnit.SetPosition(0,m_IdleUnit.Height() - m_AttackUnit.Height());
         m_Explosion.SetPosition(m_IdleUnit.Width() - m_Explosion.Width(),m_IdleUnit.Height() - m_Explosion.Height());

      }
      else
      {
         m_Explosion.SetPosition(0,m_IdleUnit.Height() - m_Explosion.Height());
         m_AttackUnit.SetPosition(m_IdleUnit.Width() - m_AttackUnit.Width(),m_IdleUnit.Height() - m_AttackUnit.Height());
      }
   }
   else
   {
      if(m_eUnitSide  == EBattleOverviewUnitSide::Enemy)
      { 
         m_AttackUnit.SetPosition(m_IdleUnit.Width() - m_AttackUnit.Width(),0);
         
      }
      else
      {
         m_Explosion.SetPosition(m_IdleUnit.Width() - m_Explosion.Width(),0);
      }
   }
   // Set size 
   m_pTransform->Size().X = m_IdleUnit.Width();
   m_pTransform->Size().Y = m_IdleUnit.Height();

   // Set all decorating objects (hit point, unit name, status etc...)
   SetInformationPosition();
}


GBattleOverviewUnit::~GBattleOverviewUnit()
{
   // Remove action from Animation system
   for(UINT32 i=0;i<UnitActionID::Total;i++)
   {
      // Remove track from anim system
      g_Joshua.GUIManager()->Scene()->Get_Animation_System()->Remove_Track(m_pUnitActionTrack[i]);

      // Release it
      SAFE_RELEASE(m_pUnitActionTrack[i]);
   }
}


void GBattleOverviewUnit::CreateActions(const GString& in_UnitAnimationFolder)
{
   // Create possible Action
   for(UINT32 i=0;i<UnitActionID::Total;i++)
   {
      // Create a track
      m_pUnitActionTrack[i] = g_Joshua.Renderer()->Create_Track();
      g_Joshua.GUIManager()->Scene()->Get_Animation_System()->Add_Track(m_pUnitActionTrack[i]);
   }

   // Those animation should extrapolation because there are responsible for either overlay or visibility setting.
   m_pUnitActionTrack[UnitActionID::Activate]->ExtrapolationAfter(true);
   m_pUnitActionTrack[UnitActionID::Explosion]->ExtrapolationAfter(true);

   // Build Activate Track
   CGFX_Action_Itf* l_pAction                = g_Joshua.Renderer()->Create_Action();
   CGFX_Animation_Linear_Itf *l_pAnimation   = g_Joshua.Renderer()->Create_Animation_Linear();
   CGFX_Key_Linear   l_key;
   l_key.m_Time = 0 ;
   l_key.m_Value = 0 ;
   l_pAnimation->Add_Key(l_key);

   l_key.m_Time = 1 ;
   l_key.m_Value = 1 ;
   l_pAnimation->Add_Key(l_key);

   l_pAnimation->Setup();

   // Create Animation Multiple and connects all the parameters
   GFX::GAnimationMultiple*   l_pFadeInAnimation =  g_Joshua.Renderer()->CreateAnimationMultiple();
   l_pFadeInAnimation->Animation(l_pAnimation);
   l_pFadeInAnimation->AddParamConnection(m_IdleUnit.m_pMaterial->PixelShader(0),GFX::PARAM_COLOR_FACTOR_A);
   l_pFadeInAnimation->AddParamConnection(m_StatusIcon.m_pMaterial->PixelShader(0),GFX::PARAM_COLOR_FACTOR_A);
   l_pFadeInAnimation->AddParamConnection(m_TotalUnitHP.TextPrimitive(),GFX::PARAM_COLOR_A);
   l_pFadeInAnimation->AddParamConnection(m_UnitHP.TextPrimitive(),GFX::PARAM_COLOR_A);
   l_pFadeInAnimation->AddParamConnection(m_UnitName.TextPrimitive(),GFX::PARAM_COLOR_A);
   
   // Add this animation to this action
   l_pAction->Add_Animation(l_pFadeInAnimation);

   // Add this action to the track
   l_pAction->Length(2.f);
   m_pUnitActionTrack[UnitActionID::Activate]->Set_Action(l_pAction);
   


   // Free temporary pointer;
   SAFE_RELEASE(l_pAnimation);
   SAFE_RELEASE(l_pFadeInAnimation);
   SAFE_RELEASE(l_pAction);

   // Create Folder string
   GString l_AnimationFilename = "Data\\Combat_Overview\\";
   l_AnimationFilename += in_UnitAnimationFolder;
   l_AnimationFilename += "\\";
   if(m_eUnitSide == EBattleOverviewUnitSide::Ally)
   {
      l_AnimationFilename+="Ally\\";
   }
   else
   {
      l_AnimationFilename+="Enemy\\";
   }

   l_AnimationFilename+=c_UnitTypeXMLName[m_iTypeID];
   l_AnimationFilename+=L"\\";

   {
   CGFX_Animation_Static_Itf *l_pAnimation;
   // Build Idle Action

   m_IdleUnit.LoadAnimation(l_AnimationFilename + "Idle.ifl",m_pUnitActionTrack[UnitActionID::Idle]);
   l_pAction = m_pUnitActionTrack[UnitActionID::Idle]->Get_Action();

   //  Make Idle animation visible
   l_pAnimation   = g_Joshua.Renderer()->Create_Animation_Static();
   l_pAnimation->Set_Value(1);
   l_pAnimation->Setup(); 
   l_pAnimation->Connect(m_IdleUnit.Model(),GFX::PARAM_VISIBILITY);
   l_pAction->Add_Animation(l_pAnimation);
   SAFE_RELEASE(l_pAnimation);

   // Make the attack unit invisible
   l_pAnimation   = g_Joshua.Renderer()->Create_Animation_Static();
   l_pAnimation->Set_Value(0);
   l_pAnimation->Setup();
   l_pAnimation->Connect(m_AttackUnit.Model(),GFX::PARAM_VISIBILITY);
   l_pAction->Add_Animation(l_pAnimation);
   SAFE_RELEASE(l_pAnimation);

   // Make the Hurt unit invisible
   l_pAnimation   = g_Joshua.Renderer()->Create_Animation_Static();
   l_pAnimation->Set_Value(0);
   l_pAnimation->Setup();
   l_pAnimation->Connect(m_HurtUnit.Model(),GFX::PARAM_VISIBILITY);
   l_pAction->Add_Animation(l_pAnimation);
   SAFE_RELEASE(l_pAnimation);

   // Build Attack ACtion
   m_AttackUnit.LoadAnimation(l_AnimationFilename + "Attack.ifl",m_pUnitActionTrack[UnitActionID::Attack]);
   l_pAction = m_pUnitActionTrack[UnitActionID::Attack]->Get_Action();

   // Make the attack unit visible
   l_pAnimation   = g_Joshua.Renderer()->Create_Animation_Static();
   l_pAnimation->Set_Value(1);
   l_pAnimation->Setup();
   l_pAnimation->Connect(m_AttackUnit.Model(),GFX::PARAM_VISIBILITY);
   l_pAction->Add_Animation(l_pAnimation);
   SAFE_RELEASE(l_pAnimation);
   
   //  Make Idle animation invisible
   l_pAnimation   = g_Joshua.Renderer()->Create_Animation_Static();
   l_pAnimation->Set_Value(0);
   l_pAnimation->Setup();
   l_pAnimation->Connect(m_IdleUnit.Model(),GFX::PARAM_VISIBILITY);
   l_pAction->Add_Animation(l_pAnimation);
   SAFE_RELEASE(l_pAnimation);

   // Make the Hurt unit invisible
   l_pAnimation   = g_Joshua.Renderer()->Create_Animation_Static();
   l_pAnimation->Set_Value(0);
   l_pAnimation->Setup();
   l_pAnimation->Connect(m_HurtUnit.Model(),GFX::PARAM_VISIBILITY);
   l_pAction->Add_Animation(l_pAnimation);
   SAFE_RELEASE(l_pAnimation);

   m_HurtUnit.LoadAnimation(l_AnimationFilename + "Idle.ifl",m_pUnitActionTrack[UnitActionID::Hurt]);
   // Build Explosion Action
   if(m_IdleUnit.Width() < 64)
      m_Explosion.LoadAnimation(GString("Data\\Combat_Overview\\") + in_UnitAnimationFolder + "\\Explosion\\Small\\HurtSmall.ifl",m_pUnitActionTrack[UnitActionID::Explosion]);
   else
   if(m_IdleUnit.Width() < 128)
      m_Explosion.LoadAnimation(GString("Data\\Combat_Overview\\") + in_UnitAnimationFolder + "\\Explosion\\Medium\\HurtMedium.ifl",m_pUnitActionTrack[UnitActionID::Explosion]);
   else
      m_Explosion.LoadAnimation(GString("Data\\Combat_Overview\\") + in_UnitAnimationFolder + "\\Explosion\\Large\\HurtLarge.ifl",m_pUnitActionTrack[UnitActionID::Explosion]);

   l_pAction = m_pUnitActionTrack[UnitActionID::Hurt]->Get_Action();

   // Reverse for Ennemy
   if(m_eUnitSide == EBattleOverviewUnitSide::Enemy)
   {
      GFX::GOverridePSCascade* l_pSelectedCursorOverride = g_Joshua.Renderer()->CreateOverridePSCascade();
      CGFX_Transform_2D_Itf* l_pTransform = g_Joshua.Renderer()->Create_Transform_2D();
      l_pTransform->Size().X = -1;
      l_pTransform->Size().Y = 1;
      l_pTransform->Position().X = 1;
      l_pTransform->Position().Y = 0;
      l_pSelectedCursorOverride->AddUVTransformOverride(0, l_pTransform);
      m_Explosion.Model()->AddOverride(l_pSelectedCursorOverride);
      SAFE_RELEASE(l_pTransform);
      SAFE_RELEASE(l_pSelectedCursorOverride);
   }

   // Make the Hurt unit visible
   l_pAnimation   = g_Joshua.Renderer()->Create_Animation_Static();
   l_pAnimation->Set_Value(1);
   l_pAnimation->Setup();
   l_pAnimation->Connect(m_HurtUnit.Model(),GFX::PARAM_VISIBILITY);
   l_pAction->Add_Animation(l_pAnimation);
   SAFE_RELEASE(l_pAnimation);

   // Make Explosion model visible until its last frame. 
   CGFX_Animation_Constant_Itf*  l_pAnimationConstant;
   l_pAnimationConstant   = g_Joshua.Renderer()->Create_Animation_Constant();
   CGFX_Key_Constant l_key;
   l_key.m_Time = 0;
   l_key.m_Value = 1;
   l_pAnimationConstant->Add_Key(l_key);
   l_key.m_Time = m_pUnitActionTrack[UnitActionID::Explosion]->Get_Action()->Length();
   l_key.m_Value = 0;
   l_pAnimationConstant->Add_Key(l_key);
   l_pAnimationConstant->Setup();
   l_pAnimationConstant->Connect(m_Explosion.Model(),GFX::PARAM_VISIBILITY);
   l_pAction->Add_Animation(l_pAnimationConstant);
   SAFE_RELEASE(l_pAnimationConstant);

   // Hide it
   m_Explosion.Model()->Visibility() = false;
      


   //  Make Idle animation invisible
   l_pAnimation   = g_Joshua.Renderer()->Create_Animation_Static();
   l_pAnimation->Set_Value(0);
   l_pAnimation->Setup(); 
   l_pAnimation->Connect(m_IdleUnit.Model(),GFX::PARAM_VISIBILITY);
   l_pAction->Add_Animation(l_pAnimation);
   SAFE_RELEASE(l_pAnimation);
 
   //  Make Attack animation invisible
   l_pAnimation   = g_Joshua.Renderer()->Create_Animation_Static();
   l_pAnimation->Set_Value(0);
   l_pAnimation->Setup();
   l_pAnimation->Connect(m_AttackUnit.Model(),GFX::PARAM_VISIBILITY);
   l_pAction->Add_Animation(l_pAnimation);
   SAFE_RELEASE(l_pAnimation);
   }


   // Build Hit Action
   l_pAction = m_pUnitActionTrack[UnitActionID::Hurt]->Get_Action();
   l_pAnimation   = g_Joshua.Renderer()->Create_Animation_Linear();
   l_key.m_Time   = 0 ;
   l_key.m_Value  = m_IdleUnit.Height()-10.0f;
   l_pAnimation->Add_Key(l_key);

   l_key.m_Time   = 1 ;
   l_key.m_Value  = m_IdleUnit.Height()- 32-10.0f;
   l_pAnimation->Add_Key(l_key);

   l_pAnimation->Setup();

   l_pAnimation->Connect(m_HitHP.Transform(),GFX::PARAM_POSITION_Y);
   l_pAction->Add_Animation(l_pAnimation);
   SAFE_RELEASE(l_pAnimation);

   // Build Hit Action Fade out
   l_pAnimation   = g_Joshua.Renderer()->Create_Animation_Linear();
   l_key.m_Time   = 0 ;
   l_key.m_Value  = 1 ;
   l_pAnimation->Add_Key(l_key);

   l_key.m_Time   = 1.0f ;
   l_key.m_Value  = 0 ;
   l_pAnimation->Add_Key(l_key);

   l_key.m_Time   = 2.0f ;
   l_key.m_Value  = 0 ;
   l_pAnimation->Add_Key(l_key);

   l_pAnimation->Setup();
   l_pAnimation->Connect(m_HitHP.TextPrimitive(),GFX::PARAM_COLOR_A);
   l_pAction->Add_Animation(l_pAnimation);
   l_pAction->Length(2.0f);
   SAFE_RELEASE(l_pAnimation);



   // Create Deactivate Track
   l_pAction      = g_Joshua.Renderer()->Create_Action();
   l_pAnimation   = g_Joshua.Renderer()->Create_Animation_Linear();
   l_key.m_Time   = 0 ;
   l_key.m_Value  = 1 ;
   l_pAnimation->Add_Key(l_key);

   l_key.m_Time   = 1 ;
   l_key.m_Value  = 0 ;
   l_pAnimation->Add_Key(l_key);

   l_pAnimation->Setup();

   // For multiple animation
   GFX::GAnimationMultiple*   l_pFadeOutAnimation =  g_Joshua.Renderer()->CreateAnimationMultiple();
   l_pFadeOutAnimation->Animation(l_pAnimation);
   l_pFadeOutAnimation->AddParamConnection(m_IdleUnit.m_pMaterial->PixelShader(0),GFX::PARAM_COLOR_FACTOR_A);
   l_pFadeOutAnimation->AddParamConnection(m_StatusIcon.m_pMaterial->PixelShader(0),GFX::PARAM_COLOR_FACTOR_A);
   l_pFadeOutAnimation->AddParamConnection(m_TotalUnitHP.TextPrimitive(),GFX::PARAM_COLOR_A);
   l_pFadeOutAnimation->AddParamConnection(m_UnitHP.TextPrimitive(),GFX::PARAM_COLOR_A);
   l_pFadeOutAnimation->AddParamConnection(m_UnitName.TextPrimitive(),GFX::PARAM_COLOR_A);

   
   l_pAction->Add_Animation(l_pFadeOutAnimation);
   l_pAction->Length(1.2f);
   m_pUnitActionTrack[UnitActionID::Deactivate]->Set_Action(l_pAction);

   SAFE_RELEASE(l_pAction);
   SAFE_RELEASE(l_pAnimation);
   SAFE_RELEASE(l_pFadeOutAnimation);
}


bool GBattleOverviewUnit::Activated(void)
{
   return m_bActivated;
}

UINT32 GBattleOverviewUnit::TotalUnitHP(void)
{
   return m_iTotalUnitHP;
}


UINT32 GBattleOverviewUnit::UnitHP(void)
{
   return m_iUnitHP;
}
void GBattleOverviewUnit::UnitHP(UINT32 in_iUnitHP)
{
   m_iUnitHP = in_iUnitHP;
   m_UnitHP.Text(GString(in_iUnitHP));
   SetInformationPosition();
}

void GBattleOverviewUnit::TotalUnitHP(UINT32 in_iTotalUnitHP)
{
   m_iTotalUnitHP = in_iTotalUnitHP;
   m_TotalUnitHP.Text(GString(in_iTotalUnitHP));
   SetInformationPosition();
}

void GBattleOverviewUnit::Attack(GBattleOverviewUnit* in_pTargetUnit)
{
   m_iTotalAttack++;
}


void GBattleOverviewUnit::Activate(void)
{ 

   m_pUnitActionTrack[UnitActionID::Deactivate]->ExtrapolationAfter(false);

   m_eCurrentAction = UnitActionID::Activate;
   m_eNextAction    = UnitActionID::Idle;

   // Start Activate Track
   m_pUnitActionTrack[m_eCurrentAction]->Playback_Count(1);
   m_pUnitActionTrack[m_eCurrentAction]->Set_Start_Time(g_Joshua.GUIManager()->Scene()->Get_Animation_System()->Get_Animation_Time() );
   m_fCurrentActionFinishTime =  m_pUnitActionTrack[m_eCurrentAction]->Get_Stop_Time();

   // Start Second Idle animation
   m_pUnitActionTrack[m_eNextAction]->Playback_Count(0);
   m_pUnitActionTrack[m_eNextAction]->Set_Start_Time(g_Joshua.GUIManager()->Scene()->Get_Animation_System()->Get_Animation_Time() );
   

   m_pUnitActionTrack[UnitActionID::Activate]->ExtrapolationAfter(true);
   // Make it visible
   m_pModel->Visibility() = true;

   m_bActivated = true;
   GBattleOverviewUnit::PlayActionSound(UnitActionID::Activate,0);
}

UINT32 GBattleOverviewUnit::TypeID(void)
{
   return m_iTypeID;
}

void GBattleOverviewUnit::Deactivate(void)
{
   m_pUnitActionTrack[UnitActionID::Activate]->ExtrapolationAfter(false);
   m_pUnitActionTrack[UnitActionID::Deactivate]->ExtrapolationAfter(true);
   m_bActivated = false;
}

void GBattleOverviewUnit::ReceivedHit(void)
{
   m_iTotalHit++;
}

void GBattleOverviewUnit::UpdateHP(void)
{
   m_iUnitHP -= m_iAccumulatedHit;
   gassert(m_iUnitHP >= 0,"should be higher than 0");

   if(m_iUnitHP < 0)
      m_iUnitHP = 0;

   m_iTotalUnitHP -= m_iAccumulatedHit;

   gassert(m_iTotalUnitHP >= 0, "Should be higher than 0");
   if(m_iTotalUnitHP < 0)
   {
      m_iTotalUnitHP =0;
   }
   
   m_UnitHP.Text(GString(m_iUnitHP));
   m_TotalUnitHP.Text(GString(m_iTotalUnitHP));

   m_iAccumulatedHit = 0;

   SetInformationPosition();
}

void GBattleOverviewUnit::AccumulatedHit(UINT32 in_iTotalAccumulatedHit)
{
   m_iAccumulatedHit = in_iTotalAccumulatedHit;
}

UINT32 GBattleOverviewUnit::AccumulatedHit(void)
{
   return m_iAccumulatedHit;
}


bool GBattleOverviewUnit::IsInsideMouseClick(GVector2D<REAL32> in_MousePosition)
{
   GVector2D<REAL32> l_MousePositionInSpriteSpace = in_MousePosition - Transform()->Position();
   return IsPointInside(in_MousePosition) || IsStatusIconClick(l_MousePositionInSpriteSpace);
}
bool GBattleOverviewUnit::IsStatusIconClick(GVector2D<REAL32> in_MousePositionInSpriteSpace)
{
   return m_StatusIcon.IsPointInside(in_MousePositionInSpriteSpace);
}

void GBattleOverviewUnit::Selected(bool in_bUnitSelected)
{
   if(in_bUnitSelected)
   {
      ((GFX::GCascadePixelShader*)m_IdleUnit.m_pMaterial->PixelShader(0))->ColorFactor() = GColorRGBReal(1.00f, 1.00f, 1.00f, 1);
      ((GFX::GCascadePixelShader*)m_AttackUnit.m_pMaterial->PixelShader(0))->ColorFactor() = GColorRGBReal(1.00f, 1.00f, 1.00f, 1);
      ((GFX::GCascadePixelShader*)m_HurtUnit.m_pMaterial->PixelShader(0))->ColorFactor() = GColorRGBReal(1.00f, 1.00f, 1.00f, 1);
      ((GFX::GCascadePixelShader*)m_Explosion.m_pMaterial->PixelShader(0))->ColorFactor() = GColorRGBReal(1.00f, 1.00f, 1.00f, 1);
   }
   else
   {
      ((GFX::GCascadePixelShader*)m_IdleUnit.m_pMaterial->PixelShader(0))->ColorFactor() = GColorRGBReal(0.5f, 0.5f, 0.5f, 1);
      ((GFX::GCascadePixelShader*)m_AttackUnit.m_pMaterial->PixelShader(0))->ColorFactor() = GColorRGBReal(0.5f, 0.5f, 0.5f, 1);
      ((GFX::GCascadePixelShader*)m_HurtUnit.m_pMaterial->PixelShader(0))->ColorFactor() = GColorRGBReal(0.5f, 0.5f, 0.5f, 1);
      ((GFX::GCascadePixelShader*)m_Explosion.m_pMaterial->PixelShader(0))->ColorFactor() = GColorRGBReal(0.5f, 0.5f, 0.5f, 1);
   }
}

void GBattleOverviewUnit::Status(UnitStatus in_eStatus)
{
   gassert(in_eStatus <TOTALSTATUS,"Invalid Status");
   m_eUnitStatus  = in_eStatus;

   if(in_eStatus ==ATTACKING)
   {
      m_StatusIcon.LoadTexture("data\\Combat_Overview\\UI\\Attack.tga");
   }
   else
   {
      m_StatusIcon.LoadTexture("data\\Combat_Overview\\UI\\Defend.tga");
   }
}

GBattleOverviewUnit::UnitStatus GBattleOverviewUnit::Status(void)
{
   return m_eUnitStatus;
}

void GBattleOverviewUnit::SetInformationPosition(void)
{

   if(m_eUnitSide == EBattleOverviewUnitSide::Ally)
   {
      m_StatusIcon.SetPosition(0,-10);
      m_UnitName.SetPosition(m_StatusIcon.Width() + 10,-10 );
      m_UnitHP.SetPosition(0,m_IdleUnit.Height());
      m_TotalUnitHP.SetPosition(0,m_IdleUnit.Height()+10);
      m_HitHP.SetPosition(0,m_IdleUnit.Height());
      
   }
   else
   {
      m_StatusIcon.SetPosition(m_IdleUnit.Width() - m_StatusIcon.Width(),-10);
      m_UnitName.SetPosition(m_IdleUnit.Width() - m_UnitName.Width() -m_StatusIcon.Width() -10,-10 );
      m_UnitHP.SetPosition(m_IdleUnit.Width() - m_UnitHP.Width() ,m_IdleUnit.Height());
      m_TotalUnitHP.SetPosition(m_IdleUnit.Width() - m_TotalUnitHP.Width(),m_IdleUnit.Height()+10);
      m_HitHP.SetPosition(m_IdleUnit.Width() - m_HitHP.Width(),m_IdleUnit.Height());
      
   }
}

EBattleOverviewUnitSide::Enum GBattleOverviewUnit::Side(void)
{
   return m_eUnitSide;
}


void GBattleOverviewUnit::OnProcess(float in_fCurrentTime)
{
   REAL32 l_fCurrentTime = g_Joshua.GUIManager()->Scene()->Get_Animation_System()->Get_Animation_Time();

   // Are we done with the current animation
   if(m_fCurrentActionFinishTime < l_fCurrentTime)
   {

      UnitActionID::Enum l_eNextActionToPlay;
      // Then for sure we are playing the next animation
      // So, lets decide what will be after the current animation
      if(m_iTotalUnitHP == 0)
      {
         // Play Dying Animation
         l_eNextActionToPlay = UnitActionID::Deactivate;
         Deactivate();
      }
      else

      // If we didn't attack,  didn't accumulated damage(or a Hurt animation is schedule) , play the Idle Animation
      if(m_iTotalAttack == 0 && (m_iAccumulatedHit == 0 || m_eNextAction == UnitActionID::Hurt))
      {
         // Play Idle
         l_eNextActionToPlay = UnitActionID::Idle;
      }
      else
      if(m_iTotalAttack > m_iTotalHit || m_iAccumulatedHit == 0)
      { 
         // PLay Attack
         l_eNextActionToPlay = UnitActionID::Attack;
         m_iTotalAttack = 0;
      }
      else
      {     
         // Play Getting Hit
         m_HitHP.Text(GString(m_iAccumulatedHit));
         SetInformationPosition();
         l_eNextActionToPlay = UnitActionID::Hurt;
         m_iTotalHit = 0;

      }

      if(m_eCurrentAction == UnitActionID::Deactivate)
      {
         // Hide this thing.
         m_pModel->Visibility() = 0;
      }
       
      m_eCurrentAction = m_eNextAction;
      m_eNextAction = l_eNextActionToPlay;

      if(m_eCurrentAction == UnitActionID::Hurt)
      {
         // This should be done, later.
         UpdateHP();
      }
      
      // If the next action is different
      if(m_eCurrentAction != m_eNextAction)
      {
         m_pUnitActionTrack[m_eCurrentAction]->Playback_Count(1);
         m_fCurrentActionFinishTime = m_pUnitActionTrack[m_eCurrentAction]->Get_Stop_Time();
         m_pUnitActionTrack[m_eNextAction]->Playback_Count(1);
         m_pUnitActionTrack[m_eNextAction]->Set_Start_Time(m_fCurrentActionFinishTime);
         PlayActionSound(m_eNextAction,m_fCurrentActionFinishTime);

         if(m_eNextAction == UnitActionID::Hurt)
         {
            m_pUnitActionTrack[UnitActionID::Explosion]->Playback_Count(1);
            m_pUnitActionTrack[UnitActionID::Explosion]->Set_Start_Time(m_fCurrentActionFinishTime);
         }
         else
         if(m_eNextAction == UnitActionID::Deactivate)
         {
            // Play an Idle animation when deactivating
         m_pUnitActionTrack[UnitActionID::Idle]->Playback_Count(0);
         if(m_eCurrentAction != UnitActionID::Idle)
            m_pUnitActionTrack[UnitActionID::Idle]->Set_Start_Time(m_fCurrentActionFinishTime);
         }
      }
      else
      // Just keep playing this one, but update next time variable
      {
         // We continue with the current one
         m_pUnitActionTrack[m_eCurrentAction]->Playback_Count(1);
         m_fCurrentActionFinishTime = m_pUnitActionTrack[m_eCurrentAction]->Get_Stop_Time();
         m_pUnitActionTrack[m_eCurrentAction]->Playback_Count(0);
         PlayActionSound(m_eNextAction,m_fCurrentActionFinishTime);
      }
   }
}


void     GBattleOverviewUnit::ResetHP(void)
{
   UpdateHP();
   m_iTotalAttack = 0;
   m_iTotalHit = 0;
}

void GBattleOverviewUnit::Reset(void)
{
   // Initialize variable
   m_iTotalUnitHP    = 0;
   m_iUnitHP         = 0;
   m_iAccumulatedHit = 0;
   m_iTotalAttack    = 0;
   m_iTotalHit       = 0;
   m_bActivated      = false;

   
   // Set Label
   m_UnitHP.Text("0");
   m_TotalUnitHP.Text("0");
}

void GBattleOverviewUnit::PlayHurtSound(REAL32 in_fDelay)
{
   ESoundContext::Enum l_contextToPlayIn;

   if(m_eUnitSide == EBattleOverviewUnitSide::Ally)
      l_contextToPlayIn = ESoundContext::Left;
   else
      l_contextToPlayIn = ESoundContext::Right;

switch(m_iTypeID + 1)
   {  
   // Boat
   case EUnitType::AttackSubmarine:
   case EUnitType::BallisticMissileSubmarine:
      g_SP2Client->PlaySoundWithDelay(ESoundEventID::CombatTorpedoHitA, in_fDelay,l_contextToPlayIn );
      break;
   case EUnitType::AircraftCarrier:
   case EUnitType::Corvette:
   case EUnitType::Destroyer:
         g_SP2Client->PlaySoundWithDelay((ESoundEventID::Enum)(ESoundEventID::CombatExplosionE), in_fDelay,l_contextToPlayIn);
         break;
   case EUnitType::Frigate:
   case EUnitType::PatrolCraft:
      g_SP2Client->PlaySoundWithDelay((ESoundEventID::Enum)(ESoundEventID::CombatExplosionF), in_fDelay,l_contextToPlayIn);
      break;
   // Air
   case EUnitType::AttackAircraft:
      break;
   case EUnitType::FighterAircraft:
         g_SP2Client->PlaySoundWithDelay((ESoundEventID::Enum)(ESoundEventID::CombatExplosionB), in_fDelay,l_contextToPlayIn);
         break;
   case EUnitType::Bomber:
         g_SP2Client->PlaySoundWithDelay((ESoundEventID::Enum)(ESoundEventID::CombatExplosionF), in_fDelay,l_contextToPlayIn);
         break;
   case EUnitType::ASWHelicopter:
      break;
   case EUnitType::AttackHelicopter:
      break;
   case EUnitType::TransportHelicopter:
         g_SP2Client->PlaySoundWithDelay((ESoundEventID::Enum)(ESoundEventID::CombatExplosionA), in_fDelay,l_contextToPlayIn);
         break;

   // Ground
   case EUnitType::AirDefense:
      break;
   case EUnitType::Tank:
         g_SP2Client->PlaySoundWithDelay((ESoundEventID::Enum)(ESoundEventID::CombatExplosionE), in_fDelay,l_contextToPlayIn);
         break;
   case EUnitType::ArtilleryGun:
         g_SP2Client->PlaySoundWithDelay((ESoundEventID::Enum)(ESoundEventID::CombatExplosionC), in_fDelay,l_contextToPlayIn);
         break;
   case EUnitType::MobileLauncher:
         g_SP2Client->PlaySoundWithDelay((ESoundEventID::Enum)(ESoundEventID::CombatExplosionD), in_fDelay,l_contextToPlayIn);
         break;

   case EUnitType::InfantryVehicle:
      g_SP2Client->PlaySoundWithDelay((ESoundEventID::Enum)((ESoundEventID::CombatExplosionA + rand()%6)), in_fDelay,l_contextToPlayIn);
      break;
   // Infantry
   case EUnitType::Infantry:
      g_SP2Client->PlaySoundWithDelay(ESoundEventID::CombatInfantryDyingA, in_fDelay,l_contextToPlayIn);
      break;
   default:
      gassert(false,"Not implemented type");
   }
}

void GBattleOverviewUnit::PlayIdleSound(REAL32 in_fDelay)
{
    ESoundContext::Enum l_contextToPlayIn;

   if(m_eUnitSide == EBattleOverviewUnitSide::Ally)
      l_contextToPlayIn = ESoundContext::Left;
   else
      l_contextToPlayIn = ESoundContext::Right;

   if(rand()%20)
   {
      return ;
   }

   switch(m_iTypeID + 1)
   {  
   // Boat
   case EUnitType::AircraftCarrier:
   case EUnitType::AttackSubmarine:
   case EUnitType::BallisticMissileSubmarine:
   case EUnitType::Corvette:
      g_SP2Client->PlaySoundWithDelay(ESoundEventID::CombatShipA, in_fDelay,l_contextToPlayIn);
      break;
   case EUnitType::Destroyer:
   case EUnitType::Frigate:
   case EUnitType::PatrolCraft:
      g_SP2Client->PlaySoundWithDelay(ESoundEventID::CombatShipB, in_fDelay,l_contextToPlayIn);
      break;
   // Air
   case EUnitType::AttackAircraft:
      g_SP2Client->PlaySoundWithDelay(ESoundEventID::CombatAircraftC, in_fDelay,l_contextToPlayIn);
      break;
   case EUnitType::FighterAircraft:
      g_SP2Client->PlaySoundWithDelay(ESoundEventID::CombatAircraftB, in_fDelay,l_contextToPlayIn);
      break;
   case EUnitType::Bomber:
      g_SP2Client->PlaySoundWithDelay(ESoundEventID::CombatAircraftA, in_fDelay,l_contextToPlayIn);
      break;
   case EUnitType::ASWHelicopter:
      g_SP2Client->PlaySoundWithDelay(ESoundEventID::CombatHelicopterA, in_fDelay,l_contextToPlayIn);
      break;
   case EUnitType::AttackHelicopter:
      g_SP2Client->PlaySoundWithDelay(ESoundEventID::CombatHelicopterB, in_fDelay,l_contextToPlayIn);
      break;
   case EUnitType::TransportHelicopter:
      g_SP2Client->PlaySoundWithDelay(ESoundEventID::CombatHelicopterC, in_fDelay,l_contextToPlayIn);
      break;
   // Ground
   case EUnitType::AirDefense:
      break;
   case EUnitType::Tank:
      //g_SP2Client->PlaySoundWithDelay(ESoundEventID::CombatMotorizedGroundUnitA, in_fDelay,l_contextToPlayIn);
      break;
   case EUnitType::ArtilleryGun:
      break;
   case EUnitType::InfantryVehicle:
      //g_SP2Client->PlaySoundWithDelay(ESoundEventID::CombatMotorizedGroundUnitB, in_fDelay,l_contextToPlayIn);
      break;
   case EUnitType::MobileLauncher:
      //g_SP2Client->PlaySoundWithDelay(ESoundEventID::CombatMotorizedGroundUnitC, in_fDelay,l_contextToPlayIn);
      break;
   // Infantry
   case EUnitType::Infantry:
      break;

   default:
      gassert(false,"Not implemented type");
   }
}


void GBattleOverviewUnit::PlayAttackSound(REAL32 in_fDelay)
{
    ESoundContext::Enum l_contextToPlayIn;

   if(m_eUnitSide == EBattleOverviewUnitSide::Ally)
      l_contextToPlayIn = ESoundContext::Left;
   else
      l_contextToPlayIn = ESoundContext::Right;
switch(m_iTypeID + 1)
   {  
   // Boat
   case EUnitType::AircraftCarrier:
      g_SP2Client->PlaySoundWithDelay(ESoundEventID::CombatAircraftCarrierTakeoffA, in_fDelay,l_contextToPlayIn);
      break;
   case EUnitType::AttackSubmarine:
      g_SP2Client->PlaySoundWithDelay(ESoundEventID::CombatTorpedoFireA, in_fDelay,l_contextToPlayIn);
      break;
   case EUnitType::BallisticMissileSubmarine:
      g_SP2Client->PlaySoundWithDelay(ESoundEventID::CombatTorpedoFireB, in_fDelay,l_contextToPlayIn);
      break;
   case EUnitType::Corvette:
      break;
   case EUnitType::Destroyer:
      g_SP2Client->PlaySoundWithDelay(ESoundEventID::CombatInfantryVehicleFireC, in_fDelay,l_contextToPlayIn);
      break;

   case EUnitType::Frigate:
      break;
   case EUnitType::PatrolCraft:
      g_SP2Client->PlaySoundWithDelay(ESoundEventID::CombatInfantryVehicleFireA, in_fDelay,l_contextToPlayIn);
      break;
   // Air
   case EUnitType::AttackAircraft:
      g_SP2Client->PlaySoundWithDelay(ESoundEventID::CombatMissileFireA, in_fDelay,l_contextToPlayIn);
      break;
   case EUnitType::FighterAircraft:
      g_SP2Client->PlaySoundWithDelay(ESoundEventID::CombatMissileFireF, in_fDelay,l_contextToPlayIn);
      break;
   case EUnitType::Bomber:
      g_SP2Client->PlaySoundWithDelay(ESoundEventID::CombatMissileFireB, in_fDelay,l_contextToPlayIn);
      break;
   case EUnitType::ASWHelicopter:
      g_SP2Client->PlaySoundWithDelay(ESoundEventID::CombatMissileFireC, in_fDelay,l_contextToPlayIn);
      break;
   case EUnitType::AttackHelicopter:
      g_SP2Client->PlaySoundWithDelay(ESoundEventID::CombatMissileFireD, in_fDelay,l_contextToPlayIn);
      break;
   case EUnitType::TransportHelicopter:
      break;
   // Ground
   case EUnitType::AirDefense:
      g_SP2Client->PlaySoundWithDelay(ESoundEventID::CombatMissileFireF, in_fDelay,l_contextToPlayIn);
      break;
   case EUnitType::Tank:
      g_SP2Client->PlaySoundWithDelay(ESoundEventID::CombatTankFireA, in_fDelay,l_contextToPlayIn);
      break;
   case EUnitType::ArtilleryGun:
      g_SP2Client->PlaySoundWithDelay(ESoundEventID::CombatTankFireB, in_fDelay,l_contextToPlayIn);
      break;
   case EUnitType::InfantryVehicle:
      g_SP2Client->PlaySoundWithDelay(ESoundEventID::CombatInfantryVehicleFireB, in_fDelay,l_contextToPlayIn);
      break;
   case EUnitType::MobileLauncher:
      g_SP2Client->PlaySoundWithDelay(ESoundEventID::CombatMissileFireE, in_fDelay,l_contextToPlayIn);
      break;
   // Infantry
   case EUnitType::Infantry:
      g_SP2Client->PlaySoundWithDelay(ESoundEventID::CombatInfantryFireA, in_fDelay,l_contextToPlayIn);
      break;
   default:
      gassert(false,"Not implemented type");
   }
}

void  GBattleOverviewUnit::PlayActionSound(UnitActionID::Enum in_eAction,REAL32 in_fTimeToPlay)
{

   // Calculate in how many seconds we are going to play that sound.
   REAL32 l_fTimeDelta = in_fTimeToPlay - g_Joshua.GUIManager()->Scene()->Get_Animation_System()->Get_Animation_Time();
   if(l_fTimeDelta < 0)
      l_fTimeDelta = 0;
 
   {  
      if(in_eAction == UnitActionID::Attack)
      {
         PlayAttackSound(l_fTimeDelta);
      } 
      else
      if(in_eAction == UnitActionID::Hurt)
      {
         PlayHurtSound(l_fTimeDelta);
      }
      else
      {
         // Play ambient sound !
         PlayIdleSound(l_fTimeDelta);

      }
   }
}
void     GBattleOverviewUnit::HideTotalHP(bool in_bHideTotalHP)
{
   m_TotalUnitHP.Model()->Visibility() = in_bHideTotalHP;
}