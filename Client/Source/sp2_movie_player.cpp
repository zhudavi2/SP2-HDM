/**************************************************************
*
* sp2_movie_player.cpp
*
* Description
* ===========
*  See corresponding .h file
*
* Owner
* =====
*  Frederic O'Reilly
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#include "golem_pch.hpp"

#include <dshow.h>

#include "../include/sp2_movie_player.h"

// Taken from : 
//-----------------------------------------------------------------------------
// File: DShowTextures.h
//
// Desc: DirectShow sample code - adds support for DirectShow videos playing 
//       on a DirectX 8.0 texture surface. Turns the D3D texture tutorial into 
//       a recreation of the VideoTex sample from previous versions of DirectX.
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//-----------------------------------------------------------------------------

#include <atlbase.h>
#include <streams.h>


//-----------------------------------------------------------------------------
// Define GUID for Texture Renderer
// {71771540-2017-11cf-AE26-0020AFD79767}
//-----------------------------------------------------------------------------
struct __declspec(uuid("{71771540-2017-11cf-ae26-0020afd79767}")) CLSID_TextureRenderer;

//-----------------------------------------------------------------------------
// CTextureRenderer Class Declarations
//-----------------------------------------------------------------------------
#pragma pack()
class CTextureRenderer : public CBaseVideoRenderer
{
public:
   CTextureRenderer(void* in_pContainer, LPUNKNOWN pUnk,HRESULT *phr);
   ~CTextureRenderer();

public:
   HRESULT CheckMediaType(const CMediaType *pmt );     // Format acceptable?
   HRESULT SetMediaType(const CMediaType *pmt );       // Video format notification
   HRESULT DoRenderSample(IMediaSample *pMediaSample); // New video sample

   struct GDShowComponents* m_pContainer;
   GImage2D* m_pImage;
   LONG m_lVidWidth;   // Video width
   LONG m_lVidHeight;  // Video Height
   LONG m_lVidPitch;   // Video Pitch
   EPixelFormat m_eTextureFormat; // Texture format
};

//-----------------------------------------------------------------------------
// File: DShowTextures.cpp
//
// Desc: DirectShow sample code - adds support for DirectShow videos playing 
//       on a DirectX 9.0 texture surface. Turns the D3D texture tutorial into 
//       a recreation of the VideoTex sample from previous versions of DirectX.
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Msg: Display an error message box if needed
//-----------------------------------------------------------------------------
void Msg(TCHAR *szFormat, ...)
{
   TCHAR szBuffer[1024];  // Large buffer for long filenames or URLs
   const size_t NUMCHARS = sizeof(szBuffer) / sizeof(szBuffer[0]);
   const int LASTCHAR = NUMCHARS - 1;

   // Format the input string
   va_list pArgs;
   va_start(pArgs, szFormat);

   // Use a bounded buffer size to prevent buffer overruns.  Limit count to
   // character size minus one to allow for a NULL terminating character.
   _vsntprintf(szBuffer, NUMCHARS - 1, szFormat, pArgs);
   va_end(pArgs);

   // Ensure that the formatted string is NULL-terminated
   szBuffer[LASTCHAR] = TEXT('\0');

#ifdef GOLEM_DEBUG
   g_Joshua.Log(szBuffer);
#endif
}

//-----------------------------------------------------------------------------
// Global DirectShow pointers
//-----------------------------------------------------------------------------
struct GDShowComponents
{
   GString                 m_sFileName;
   vector<CGFX_Scene_Itf*> m_vLastScenes;

   CGFX_Scene_Itf*         m_pScene;
   GFX::GWindow*           m_pWindow;
   CGFX_Texture_Itf*       m_pTxtr;

   CComPtr<IGraphBuilder>  m_pGB;          // GraphBuilder
   CComPtr<IMediaControl>  m_pMC;          // Media Control
   CComPtr<IMediaPosition> m_pMP;          // Media Position
   CComPtr<IMediaEvent>    m_pME;          // Media Event
   CComPtr<IBaseFilter>    m_pRenderer;    // our custom renderer

   //-----------------------------------------------------------------------------
   // InitDShowTextureRenderer : Create DirectShow filter graph and run the graph
   //-----------------------------------------------------------------------------
   HRESULT InitDShowTextureRenderer()
   {
      m_pScene = NULL;
      m_pWindow = NULL;
      m_pTxtr = NULL;

      HRESULT hr = S_OK;
      CComPtr<IBaseFilter>    pFSrc;          // Source Filter
      CComPtr<IPin>           pFSrcPinOut;    // Source Filter Output Pin   
      CTextureRenderer        *pCTR=0;        // DirectShow Texture renderer

      // Create the filter graph
      if (FAILED(m_pGB.CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC)))
         return E_FAIL;

      // Create the Texture Renderer object
      pCTR = new CTextureRenderer(this, NULL, &hr);
      if (FAILED(hr) || !pCTR)
      {
         Msg(TEXT("Could not create texture renderer object!  hr=0x%x"), hr);
         return E_FAIL;
      }

      // Get a pointer to the IBaseFilter on the TextureRenderer, add it to graph
      m_pRenderer = pCTR;
      if (FAILED(hr = m_pGB->AddFilter(m_pRenderer, L"TEXTURERENDERER")))
      {
         Msg(TEXT("Could not add renderer filter to graph!  hr=0x%x"), hr);
         return hr;
      }

      // Add the source filter to the graph.
      hr = m_pGB->AddSourceFilter (m_sFileName.c_str(), L"SOURCE", &pFSrc);

      // If the media file was not found, inform the user.
      if (hr == VFW_E_NOT_FOUND)
      {
         Msg(TEXT("Could not add source filter to graph!  (hr==VFW_E_NOT_FOUND)\r\n\r\n")
            TEXT("This sample reads a media file from the DirectX SDK's media path.\r\n")
            TEXT("Please install the DirectX 9 SDK on this machine."));
         return hr;
      }
      else if(FAILED(hr))
      {
         Msg(TEXT("Could not add source filter to graph!  hr=0x%x"), hr);
         return hr;
      }

      if (FAILED(hr = pFSrc->FindPin(L"Output", &pFSrcPinOut)))
      {
         Msg(TEXT("Could not find output pin!  hr=0x%x"), hr);
         return hr;
      }

      // Render the source filter's output pin.  The Filter Graph Manager
      // will connect the video stream to the loaded CTextureRenderer
      // and will load and connect an audio renderer (if needed).

      if (FAILED(hr = m_pGB->Render(pFSrcPinOut)))
      {
         Msg(TEXT("Could not render source output pin!  hr=0x%x"), hr);
         return hr;
      }

      m_pWindow = g_Joshua.Renderer()->CreateWindow();
      CGFX_Texture_Itf* l_pTxtr = g_Joshua.Renderer()->TextureManager()->CreateTexture(PIXELFMT_X8R8G8B8, 512, 512, false, GString("CR;S") );
      m_pWindow->SetImage(l_pTxtr);
      SAFE_RELEASE(l_pTxtr);
      m_pWindow->ImageMagFilter(GFX::FILTER_LINEAR);
      m_pWindow->ImageMinFilter(GFX::FILTER_LINEAR);

      // Get the graph's media control, event & position interfaces
      m_pGB.QueryInterface(&m_pMC);
      m_pGB.QueryInterface(&m_pMP);
      m_pGB.QueryInterface(&m_pME);

      // Start the graph running;
      if (FAILED(hr = m_pMC->Run()))
      {
         Msg(TEXT("Could not run the DirectShow graph!  hr=0x%x"), hr);
         return hr;
      }

      // Create the scene
      m_pScene = g_Joshua.Renderer()->Create_Scene();
      m_pScene->ClearFrameBuffer(true);
      m_pScene->ClearFrameColor() = GColorRGBReal(0, 0, 0, 0);

      // Create a camera
      CGFX_Camera_Itf* l_Camera = g_Joshua.Renderer()->Create_Camera();
      m_pScene->Set_Active_Camera(l_Camera);
      m_pScene->Get_Root_Model()->Set_Primitive(l_Camera);
      l_Camera->View_Type(GFX::VIEW_ORTHOGONAL);
      l_Camera->Near_Plane() = -1;
      l_Camera->Far_Plane() = 1;
      l_Camera->Release();

      // Create a full screen window
      CGFX_Renderer_Init_Mode mode;
      g_Joshua.Renderer()->Get_Current_Mode(&mode);

      CGFX_Model_Itf* l_Model = g_Joshua.Renderer()->Create_Model();
      m_pScene->Get_Root_Model()->Add_Child(l_Model);

      CGFX_Transform_2D_Itf* l_Trans = g_Joshua.Renderer()->Create_Transform_2D();
      l_Model->Set_Transform(l_Trans);

      // Make sure to respect movie aspect ratio:
      REAL32 l_fMovieAspectRatio = (REAL32) m_pTxtr->PartialWidth() / (REAL32) m_pTxtr->PartialHeight();
      REAL32 l_fResolutionAspectRatio = (REAL32) mode.m_Resolution.x / (REAL32) mode.m_Resolution.y;

      if(l_fMovieAspectRatio == l_fResolutionAspectRatio)
      {
         // Same aspect ratio, use a fullscreen window
         l_Trans->Position() = GVector2D<REAL32>(0, 0);
         l_Trans->Size() = GVector2D<REAL32>(mode.m_Resolution);
      }
      else if(l_fMovieAspectRatio > l_fResolutionAspectRatio)
      {
         // Movie has a higher width to height than screen, center movie vertically
         UINT32 l_iWidth = mode.m_Resolution.x;
         UINT32 l_iHeight = m_pTxtr->PartialHeight() * mode.m_Resolution.x / m_pTxtr->PartialWidth();

         UINT32 l_iPosX = 0;
         UINT32 l_iPosY = (mode.m_Resolution.y - l_iHeight) / 2;

         l_Trans->Position().X = (REAL32) l_iPosX;
         l_Trans->Position().Y = (REAL32) l_iPosY;
         l_Trans->Size().X = (REAL32) l_iWidth;
         l_Trans->Size().Y = (REAL32) l_iHeight;
      }
      else
      {
         // Movie has a lesser width to height than screen, center movie horizontally
         UINT32 l_iWidth = m_pTxtr->PartialWidth() * mode.m_Resolution.y / m_pTxtr->PartialHeight();
         UINT32 l_iHeight = mode.m_Resolution.y;

         UINT32 l_iPosX = (mode.m_Resolution.x - l_iWidth) / 2;
         UINT32 l_iPosY = 0;

         l_Trans->Position().X = (REAL32) l_iPosX;
         l_Trans->Position().Y = (REAL32) l_iPosY;
         l_Trans->Size().X = (REAL32) l_iWidth;
         l_Trans->Size().Y = (REAL32) l_iHeight;
      }

      l_Model->Set_Primitive(m_pWindow);
      SAFE_RELEASE(l_Model);
      SAFE_RELEASE(l_Trans);

      // Backup current scenes
      for(UINT32 i = 0;i < g_Joshua.Renderer()->RenderTree()->NbScenes();i ++)
      {
         g_Joshua.Renderer()->RenderTree()->Scene(i)->Inc_Ref();
         m_vLastScenes.push_back(g_Joshua.Renderer()->RenderTree()->Scene(i) );
      }
      g_Joshua.Renderer()->RenderTree()->ClearScenes();

      g_Joshua.Renderer()->RenderTree()->AddScene(m_pScene);

      return S_OK;
   }


   //-----------------------------------------------------------------------------
   // CheckMovieStatus: If the movie has ended, rewind to beginning
   //-----------------------------------------------------------------------------
   bool CheckMovieStatus(void)
   {
      long lEventCode;
      LONG_PTR lParam1, lParam2;
      HRESULT hr;

      if (!m_pME)
         return true;

      m_pWindow->SetImage(m_pTxtr);
      bool l_bComplete = false;

      // Check for completion events
      hr = m_pME->GetEvent(&lEventCode, &lParam1, &lParam2, 0);
      if (SUCCEEDED(hr))
      {
         if (EC_COMPLETE == lEventCode) 
         {
            l_bComplete = true;
//            hr = m_pMP->put_CurrentPosition(0);
         }

         // Free any memory associated with this event
         hr = m_pME->FreeEventParams(lEventCode, lParam1, lParam2);
      }

      return l_bComplete;
   }


   //-----------------------------------------------------------------------------
   // CleanupDShow
   //-----------------------------------------------------------------------------
   void CleanupDShow(void)
   {
      // Shut down the graph
      if (!(!m_pMC)) m_pMC->Stop();

      if (!(!m_pMC)) m_pMC.Release();
      if (!(!m_pME)) m_pME.Release();
      if (!(!m_pMP)) m_pMP.Release();
      if (!(!m_pGB)) m_pGB.Release();
      if (!(!m_pRenderer)) m_pRenderer.Release();

      g_Joshua.Renderer()->RenderTree()->RemoveScene(m_pScene);

      SAFE_RELEASE(m_pScene);
      SAFE_RELEASE(m_pWindow);
      SAFE_RELEASE(m_pTxtr);

      // Recover last scenes
      for(UINT32 i = 0;i < m_vLastScenes.size();i ++)
      {
         g_Joshua.Renderer()->RenderTree()->AddScene(m_vLastScenes[i] );
         m_vLastScenes[i]->Release();
      }

      m_vLastScenes.clear();
   }
};


//-----------------------------------------------------------------------------
// CTextureRenderer constructor
//-----------------------------------------------------------------------------
CTextureRenderer::CTextureRenderer( void* in_pContainer,  LPUNKNOWN pUnk, HRESULT *phr )
   : CBaseVideoRenderer(__uuidof(CLSID_TextureRenderer), NAME("Texture Renderer"), pUnk, phr)
   , m_pContainer( (GDShowComponents*) in_pContainer)
   , m_pImage(NULL)
{
   // Store and AddRef the texture for our use.
   gassert(phr, "");
   if (phr)
      *phr = S_OK;
}


//-----------------------------------------------------------------------------
// CTextureRenderer destructor
//-----------------------------------------------------------------------------
CTextureRenderer::~CTextureRenderer()
{
   if(m_pImage)
   {
      GImage2D::FreeObject(m_pImage);
   }
}


//-----------------------------------------------------------------------------
// CheckMediaType: This method forces the graph to give us an R8G8B8 video
// type, making our copy to texture memory trivial.
//-----------------------------------------------------------------------------
HRESULT CTextureRenderer::CheckMediaType(const CMediaType *pmt)
{
   HRESULT   hr = E_FAIL;
   VIDEOINFO *pvi=0;

   CheckPointer(pmt,E_POINTER);

   // Reject the connection if this is not a video type
   if( *pmt->FormatType() != FORMAT_VideoInfo ) {
      return E_INVALIDARG;
   }

   // Only accept RGB24 video
   pvi = (VIDEOINFO *)pmt->Format();

   if(IsEqualGUID( *pmt->Type(),    MEDIATYPE_Video)  &&
      IsEqualGUID( *pmt->Subtype(), MEDIASUBTYPE_RGB24) )
   {
      hr = S_OK;
   }

   return hr;
}

//-----------------------------------------------------------------------------
// SetMediaType: Graph connection has been made. 
//-----------------------------------------------------------------------------
HRESULT CTextureRenderer::SetMediaType(const CMediaType *pmt)
{
   HRESULT hr;

   // Retrive the size of this media type
   VIDEOINFO *pviBmp;                      // Bitmap info header
   pviBmp = (VIDEOINFO *)pmt->Format();

   m_lVidWidth  = pviBmp->bmiHeader.biWidth;
   m_lVidHeight = abs(pviBmp->bmiHeader.biHeight);
   m_lVidPitch  = (m_lVidWidth * 3 + 3) & ~(3); // We are forcing RGB24

   // Create the texture that maps to this media type
   hr = E_UNEXPECTED;
   m_pContainer->m_pTxtr = g_Joshua.Renderer()->TextureManager()->CreateTexture(PIXELFMT_X8R8G8B8, m_lVidWidth, m_lVidHeight, true, GString("LMovieTxtr") + GString(g_Joshua.TimeCounter().GetPrecisionTime() ));
   m_pImage = GImage2D::AllocObject();

   // CreateTexture can silently change the parameters on us
   m_eTextureFormat = m_pContainer->m_pTxtr->ColorFormat();

   if (m_eTextureFormat != PIXELFMT_X8R8G8B8 &&
       m_eTextureFormat != PIXELFMT_A1R5G5B5)
   {
      return VFW_E_TYPE_NOT_ACCEPTED;
   }

   return S_OK;
}


//-----------------------------------------------------------------------------
// DoRenderSample: A sample has been delivered. Copy it to the texture.
//-----------------------------------------------------------------------------
HRESULT CTextureRenderer::DoRenderSample( IMediaSample * pSample )
{
   BYTE  *pBmpBuffer, *pTxtBuffer; // Bitmap buffer, texture buffer
   LONG  lTxtPitch;                // Pitch of bitmap, texture

   BYTE  * pbS = NULL;
   DWORD * pdwS = NULL;
   DWORD * pdwD = NULL;
   UINT row, col, dwordWidth;

   CheckPointer(pSample,E_POINTER);
   CheckPointer(m_pContainer->m_pTxtr,E_UNEXPECTED);

   // Get the video bitmap buffer
   pSample->GetPointer( &pBmpBuffer );
   long l_iBufferSize = pSample->GetSize();
   long l_iBufferLength = pSample->GetActualDataLength();

   // Lock the Texture
   m_pContainer->m_pTxtr->LockImage(m_pImage);

   // Get the texture buffer & pitch
   pTxtBuffer = (byte *) m_pImage->Pixels();
   lTxtPitch = m_pImage->Pitch();

   pTxtBuffer = pTxtBuffer + (lTxtPitch * ( (UINT) m_lVidHeight - 1) );


   // Copy the bits    
   if (m_eTextureFormat == PIXELFMT_X8R8G8B8) 
   {
      // Instead of copying data bytewise, we use DWORD alignment here.
      // We also unroll loop by copying 4 pixels at once.
      //
      // original BYTE array is [b0][g0][r0][b1][g1][r1][b2][g2][r2][b3][g3][r3]
      //
      // aligned DWORD array is     [b1 r0 g0 b0][g2 b2 r1 g1][r3 g3 b3 r2]
      //
      // We want to transform it to [ff r0 g0 b0][ff r1 g1 b1][ff r2 g2 b2][ff r3 b3 g3]
      // below, bitwise operations do exactly this.

      dwordWidth = m_lVidWidth / 4; // aligned width of the row, in DWORDS
      // (pixel by 3 bytes over sizeof(DWORD))

      for( row = 0; row< (UINT)m_lVidHeight; row++)
      {
         pdwS = ( DWORD*)pBmpBuffer;
         pdwD = ( DWORD*)pTxtBuffer;

         for( col = 0; col < dwordWidth; col ++ )
         {
            pdwD[0] =  pdwS[0] | 0xFF000000;
            pdwD[1] = ((pdwS[1]<<8)  | 0xFF000000) | (pdwS[0]>>24);
            pdwD[2] = ((pdwS[2]<<16) | 0xFF000000) | (pdwS[1]>>16);
            pdwD[3] = 0xFF000000 | (pdwS[2]>>8);
            pdwD +=4;
            pdwS +=3;
         }

         // we might have remaining (misaligned) bytes here
         pbS = (BYTE*) pdwS;
         for( col = 0; col < (UINT)m_lVidWidth % 4; col++)
         {
            *pdwD = 0xFF000000     |
               (pbS[2] << 16) |
               (pbS[1] <<  8) |
               (pbS[0]);
            pdwD++;
            pbS += 3;           
         }

         pBmpBuffer  += m_lVidPitch;
         pTxtBuffer -= lTxtPitch;
      }// for rows
   }

   if (m_eTextureFormat == PIXELFMT_A1R5G5B5) 
   {
      for(int y = 0; y < m_lVidHeight; y++ ) 
      {
         BYTE *pBmpBufferOld = pBmpBuffer;
         BYTE *pTxtBufferOld = pTxtBuffer;   

         for (int x = 0; x < m_lVidWidth; x++) 
         {
            *(WORD *)pTxtBuffer = (WORD)
               (0x8000 +
               ((pBmpBuffer[2] & 0xF8) << 7) +
               ((pBmpBuffer[1] & 0xF8) << 2) +
               (pBmpBuffer[0] >> 3));

            pTxtBuffer += 2;
            pBmpBuffer += 3;
         }

         pBmpBuffer = pBmpBufferOld + m_lVidPitch;
         pTxtBuffer = pTxtBufferOld - lTxtPitch;
      }
   }

   // Unlock the Texture
   m_pContainer->m_pTxtr->Unlock();

   return S_OK;
}

#define m_DShow (*( (GDShowComponents*) m_pYouDontWantToKnowWhatThisIs) )

GMoviePlayer::GMoviePlayer()
{
   m_pYouDontWantToKnowWhatThisIs = NULL;
}

GMoviePlayer::~GMoviePlayer()
{
   Stop();
}

void GMoviePlayer::PlayFile(const GString& in_sFileName)
{
   Stop();

   
   // Find real file location
   GFile l_MovieFile;
   g_Joshua.FileManager()->File(in_sFileName,l_MovieFile);

   // Extract the file to find ist real location
   GString l_FilePath;
   l_MovieFile.Extract(l_FilePath);

   m_pYouDontWantToKnowWhatThisIs = (void*) (new GDShowComponents);
   m_DShow.m_sFileName = l_FilePath;
   m_DShow.InitDShowTextureRenderer();
}

void GMoviePlayer::Stop()
{
   if(&m_DShow)
   {
      m_DShow.CleanupDShow();

      delete &m_DShow;

      m_pYouDontWantToKnowWhatThisIs = NULL;
   }
}

bool GMoviePlayer::Iterate()
{
   if( m_DShow.CheckMovieStatus() )
   {
      return true;
   }

   Sleep(10);

   return false;
}
