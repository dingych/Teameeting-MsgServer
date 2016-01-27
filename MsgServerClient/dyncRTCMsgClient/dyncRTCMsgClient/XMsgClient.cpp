//
//  XMsgClient.cpp
//  dyncRTCMsgClient
//
//  Created by hp on 12/22/15.
//  Copyright (c) 2015 Dync. All rights reserved.
//

#include "XMsgClient.h"
#include "webrtc/base/logging.h"

#ifdef WEBRTC_ANDROID
#include <android/log.h>
#define  LOG_TAG    "XMsgClient"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#else
#include <iostream>
#include <string>
#endif

#define TIMEOUT_TS (60*1000)

XMsgClient::XMsgClient()
: XJSBuffer()
, m_pClientImpl(NULL)
, m_pMsgProcesser(NULL)
, m_lastUpdateTime(0)
, m_uid("")
, m_token("")
, m_nname("")
, m_server("")
, m_port(0)
, m_autoConnect(true)
, m_login(false)
, m_msState(MSNOT_CONNECTED)
{
    
}

XMsgClient::~XMsgClient()
{
    
}

int XMsgClient::Init(XMsgCallback& cb, const std::string& uid, const std::string& token, const std::string& nname, const std::string& server, int port, bool bAutoConnect)
{
    if (!m_pMsgProcesser) {
        m_pMsgProcesser = new XMsgProcesser(cb, *this);
    }
    if (!m_pMsgProcesser) {
        return -1;
    }
    
    if (!m_pClientImpl) {
        m_pClientImpl = new XTcpClientImpl(*this);
    }
    if (!m_pClientImpl) {
        if (m_pMsgProcesser) {
            delete m_pMsgProcesser;
            m_pMsgProcesser = NULL;
        }
        return -1;
    }
    m_uid = uid;
    m_token = token;
    m_nname = nname;
    m_server = server;
    m_port = port;
    m_autoConnect = bAutoConnect;
    m_pClientImpl->Connect(server, port, bAutoConnect);
    m_msState = MSCONNECTTING;
    m_pMsgProcesser->ServerState(MSCONNECTTING);
    
    return 0;
}

int XMsgClient::Unin()
{
    if (m_pClientImpl) {
        m_pClientImpl->Disconnect();
        if (m_pMsgProcesser) {
            delete m_pMsgProcesser;
            m_pMsgProcesser = NULL;
        }
        delete m_pClientImpl;
        m_pClientImpl = NULL;
    }
    
    return 0;
}

int XMsgClient::SndMsg(const std::string& roomid, const std::string& rname, const std::string& msg)
{
    std::string outstr;
    if (m_pMsgProcesser) {
        //outstr, userid, pass, roomid, to, msg, cmd, action, tags, type
        m_pMsgProcesser->EncodeSndMsg(outstr, m_uid, m_token, m_nname, roomid, rname, "a", msg, MEETCMD::dcomm, DCOMMACTION::msend, SENDTAGS::sendtags_talk, SENDTYPE::msg);
    } else {
        return -1;
    }
    if (outstr.length()==0) {
        return -1;
    }
    
    return SendEncodeMsg(outstr);
}

int XMsgClient::GetMsg(GETCMD cmd)
{
    std::string outstr;
    if (m_pMsgProcesser) {
        m_pMsgProcesser->EncodeGetMsg(outstr, m_uid, m_token, cmd);
    } else {
        return -1;
    }
    if (outstr.length()==0) {
        return -1;
    }
    
    return SendEncodeMsg(outstr);
}

int XMsgClient::OptRoom(MEETCMD cmd, const std::string& roomid, const std::string& rname, const std::string& remain)
{
    if (cmd<=0 || cmd>=MEETCMD::meetcmd_invalid) {
        return -1;
    }
    std::string outstr;
    if (m_pMsgProcesser) {
        //outstr, userid, pass, roomid, to, msg, cmd, action, tags, type
        m_pMsgProcesser->EncodeSndMsg(outstr, m_uid, m_token, m_nname, roomid, rname, "a", "", cmd, DCOMMACTION::dcommaction_invalid, SENDTAGS::sendtags_invalid, SENDTYPE::sendtype_invalid);
    } else {
        return -1;
    }
    if (outstr.length()==0) {
        return -1;
    }
    
    return SendEncodeMsg(outstr);
}

int XMsgClient::SndMsgTo(const std::string& roomid, const std::string& rname, const std::string& msg, const std::list<std::string>& ulist)
{
    std::string outstr;
    if (m_pMsgProcesser) {
        //outstr, userid, pass, roomid, to, msg, cmd, action, tags, type
        std::string tousers;
        m_pMsgProcesser->GetMemberToJson(ulist, tousers);
        m_pMsgProcesser->EncodeSndMsg(outstr, m_uid, m_token, m_nname, roomid, rname, tousers, msg, MEETCMD::dcomm, DCOMMACTION::msend, SENDTAGS::sendtags_talk, SENDTYPE::msg);
    } else {
        return -1;
    }
    if (outstr.length()==0) {
        return -1;
    }
    
    return SendEncodeMsg(outstr);
}

int XMsgClient::NotifyMsg(const std::string& roomid, const std::string& rname, SENDTAGS tags, const std::string& msg)
{
    std::string outstr;
    if (m_pMsgProcesser) {
        //outstr, userid, pass, roomid, to, msg, cmd, action, tags, type
        m_pMsgProcesser->EncodeSndMsg(outstr, m_uid, m_token, m_nname, roomid, rname, "a", msg, MEETCMD::dcomm, DCOMMACTION::msend, tags, SENDTYPE::msg);
    } else {
        return -1;
    }
    if (outstr.length()==0) {
        return -1;
    }
    
    return SendEncodeMsg(outstr);
}

////////////////////////////////////////////
////////////////private/////////////////////
////////////////////////////////////////////

int XMsgClient::Login()
{
    std::string outstr;
    if (m_pMsgProcesser) {
        m_pMsgProcesser->EncodeLogin(outstr, m_uid, m_token, m_nname);
    } else {
        return -1;
    }
    if (outstr.length()==0) {
        return -1;
    }

#ifdef WEBRTC_ANDROID
    LOGI("XMsgClient::Login call SendEncodeMsg\n");
#else
    std::cout << "XMsgClient::Login call SendEncodeMsg" << std::endl;
#endif
    return SendEncodeMsg(outstr);
}

int XMsgClient::Logout()
{
    std::string outstr;
    if (m_pMsgProcesser) {
        m_pMsgProcesser->EncodeLogout(outstr, m_uid, m_token);
    } else {
        return -1;
    }
    if (outstr.length()==0) {
        return -1;
    }
    
    return SendEncodeMsg(outstr);
}

bool XMsgClient::RefreshTime()
{
    uint32 now = rtc::Time();
    if (m_lastUpdateTime <= now) {
        m_lastUpdateTime = now  + TIMEOUT_TS;
        KeepAlive();
        return true;
    } else {
        return false;
    }
}

int XMsgClient::KeepAlive()
{
    //if login or not
    if (!m_login) {
        return -1;
    }
    std::string outstr;
    if (m_pMsgProcesser) {
        //outstr, userid
        m_pMsgProcesser->EncodeKeepAlive(outstr, m_uid);
    } else {
        return -1;
    }
    if (outstr.length()==0) {
        return -1;
    }
    
    return SendEncodeMsg(outstr);
}

int XMsgClient::SendEncodeMsg(std::string& msg)
{
    if (TcpState::CONNECTED!=m_pClientImpl->Status()) {
#ifdef WEBRTC_ANDROID
        LOGI("XMsgClient::SendEncodeMsg NOT_CONNECTED\n");
#else
        std::cout << "XMsgClient::SendEncodeMsg NOT_CONNECTED" << std::endl;
#endif
        return -1;
    }
    char* ptr = new char[msg.length()+3];//sprintf will add 1 in the end
    if (ptr) {
        char* pptr = ptr;
        *pptr = '$';
        pptr++;
        writeShort(&pptr, (unsigned short)msg.length());
        memcpy((pptr), msg.c_str(), msg.length());
        
        if (m_pClientImpl) {
            int n = m_pClientImpl->SendMessageX(ptr, (int)strlen(ptr));
            delete ptr;
            ptr = NULL;
            return n;
        } else {
#ifdef WEBRTC_ANDROID
            LOGI("XMsgClient::SendEncodeMsg m_pClientImpl is NULL\n");
#else
            std::cout << "XMsgClient::SendEncodeMsg m_pClientImpl is NULL" << std::endl;
#endif
            delete ptr;
            ptr = NULL;
            LOG(LS_ERROR) << "SendEncodeMsg m_pClientImpl is NULL";
            return -1;
        }
    } else {
#ifdef WEBRTC_ANDROID
        LOGI("XMsgClient::SendEncodeMsg ptr is NULL\n");
#else
        std::cout << "XMsgClient::SendEncodeMsg ptr is NULL" << std::endl;
#endif
        LOG(LS_ERROR) << "SendEncodeMsg ptr is NULL";
        return -1;
    }
}

void XMsgClient::writeShort(char** pptr, unsigned short anInt)
{
    **pptr = (char)(anInt / 256);
    (*pptr)++;
    **pptr = (char)(anInt % 256);
    (*pptr)++;
}

unsigned short XMsgClient::readShort(char** pptr)
{
    char* ptr = *pptr;
    unsigned short len = 256 * ((unsigned char)(*ptr)) + (unsigned char)(*(ptr + 1));
    *pptr += 2;
    return len;
}

//////////////////////////////////////////////////
//////////////XTcpClientCallback//////////////////
//////////////////////////////////////////////////

void XMsgClient::OnServerConnected()
{
    //LOG(INFO) << __FUNCTION__ << " was called";
    Login();
}

void XMsgClient::OnServerDisconnect()
{
    //LOG(INFO) << __FUNCTION__ << " was called";
    if (m_pMsgProcesser) {
        m_login = false;
        m_msState = MSNOT_CONNECTED;
        m_pMsgProcesser->ServerState(MSNOT_CONNECTED);
        m_pMsgProcesser->ServerDisconnect();
    }
}

void XMsgClient::OnServerConnectionFailure()
{
    //LOG(INFO) << __FUNCTION__ << " was called";
    if (m_pMsgProcesser) {
        m_login = false;
        m_msState = MSNOT_CONNECTED;
        m_pMsgProcesser->ServerState(MSNOT_CONNECTED);
        m_pMsgProcesser->ServerConnectionFailure();
    }
}

void XMsgClient::OnTick()
{
    //LOG(INFO) << __FUNCTION__ << " was called";
    if (m_msState == MSCONNECTED) {
        RefreshTime();
    }
    
}

void XMsgClient::OnMessageSent(int err)
{
    //LOG(INFO) << __FUNCTION__ << " was called err:" << err;
}

void XMsgClient::OnMessageRecv(const char*pData, int nLen)
{
    //LOG(INFO) << __FUNCTION__ << " was called nLen:" << nLen << ",pData:" << pData;
    if (!pData) {
        return;
    }
    XJSBuffer::RecvData(pData, nLen);
}

//////////////////////////////////////////////////
//////////////XMsgClientHelper////////////////////
//////////////////////////////////////////////////

void XMsgClient::OnLogin(int code, const std::string& userid)
{
#ifdef WEBRTC_ANDROID
    LOGI("XMsgClient::OnLogin code:%d, userid:%s\n", code, userid.c_str());
#else
    std::cout << "XMsgClient::OnLogin code:" << code << ", userid:" << userid << std::endl;
#endif
    if (code == 0) {
        m_login = true;
        m_msState = MSCONNECTED;
        m_pMsgProcesser->ServerState(MSCONNECTED);
        m_pMsgProcesser->ServerConnected();
    } else {
        Login();
    }
    
}

void XMsgClient::OnLogout(int code, const std::string& userid)
{
#ifdef WEBRTC_ANDROID
    LOGI("XMsgClient::OnLogout code:%d, userid:%s\n", code, userid.c_str());
#else
    std::cout << "XMsgClient::OnLogout code:" << code << ", userid:" << userid << std::endl;
#endif
    m_login = false;
}

//////////////////////////////////////////////////
//////////////XJSBuffer///////////////////////////
//////////////////////////////////////////////////

void XMsgClient::OnRecvMessage(const char*message, int nLen)
{
    if (m_pMsgProcesser) {
#ifdef WEBRTC_ANDROID
        LOGI("XMsgClient::OnRecvMessage len:%d, message:%s\n", nLen, message);
#else
        std::cout << "XMsgClient::OnRecvMessage len:" << nLen << ", message:" << message << std::endl;
#endif
        m_pMsgProcesser->DecodeRecvData((char *)message, nLen);
    }
}
