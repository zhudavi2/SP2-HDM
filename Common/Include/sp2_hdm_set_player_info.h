/**************************************************************
*
* sp2_hdm_set_player_info.h
*
* Description
* ===========
*  HDM event version of player info.
*
* Copyright  (C) 2018, D. Z. (BlasterMillennia)
***************************************************************/
#ifndef _GOLEM_SP2_HDM_SET_PLAYER_INFO_H_
#define _GOLEM_SP2_HDM_SET_PLAYER_INFO_H_

namespace SP2
{
    namespace Event
    {
        class GHdmSetPlayerInfo : public SDK::Event::GGameEvent
        {
            friend class GClient;
            friend class GServer;

            static SDK::GGameEventSPtr New();

        public:
            SP2::Event::GSetPlayerInfo::PlayerInfo m_PlayerInfo;
            UINT32 m_iPassword;

            virtual bool Serialize(GIBuffer& io_Buffer) const;
            virtual bool Unserialize(GOBuffer& io_Buffer);

            GHdmSetPlayerInfo();
        };
    }
}

#endif //_GOLEM_SP2_HDM_SET_PLAYER_INFO_H_
