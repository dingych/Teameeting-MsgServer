#include <stdio.h>
#include "SocketUtils.h"
#include "MRTHttpSvrConn.h"
#include "RTConnHttp.h"
#include "MRTRoomManager.h"

MRTHttpSvrConn::MRTHttpSvrConn(void)
: RTHttpSvrConn()
, m_httpHost("")
, m_httpIp("")
, m_httpPort(0)
{
    AddObserver(this);
}

MRTHttpSvrConn::~MRTHttpSvrConn(void)
{
    DelObserver(this);
}

////////////////////////////////////////////////////
///////////////  write to http  ////////////////////
////////////////////////////////////////////////////

//* HTTP_POST

void MRTHttpSvrConn::HttpInsertMeetingMsg(const char* sign, const char* meetingid, const char* messagetype, const char* sessionid, const char* strMsg, const char* userid)
{
    if (!sign || !meetingid || !messagetype || !sessionid || !strMsg || !userid || strlen(strMsg)>1024) {
        LE("HttpInsertMeetingMsg params error\n");
        return;
    }

    int outLen = 0;
    char data[1216] = {0};//1024+128+64:msg len + other value len + attr name len
    sprintf(data, "sign=%s&meetingid=%s&messagetype=%s&sessionid=%s&strMsg=%s&userid=%s", sign, meetingid, messagetype, sessionid, strMsg, userid);
    const char* msg = GenerateRequest(HTTP_POST, "meeting/insertMeetingMsg", data, outLen);
    if (msg && outLen>0) {
        SendData(msg, outLen);
        free((void*)msg);
        msg = NULL;
    }
}

void MRTHttpSvrConn::HttpInsertSessionMeetingInfo(const char* sign, const char* meetingid, const char* sessionid, const char* sessionstatus, const char* sessiontype, const char* sessionnumber)
{
    if (!sign || !meetingid || !sessionid || !sessionstatus || !sessiontype || !sessionnumber) {
        LE("HttpInsertSessionMeetingInfo params error\n");
        return;
    }
    int outLen = 0;
    char data[512] = {0};
    sprintf(data, "sign=%s&meetingid=%s&sessionid=%s&sessionstatus=%s&sessiontype=%s&sessionnumber=%s", sign, meetingid, sessionid, sessionstatus, sessiontype, sessionnumber);
    const char* msg = GenerateRequest(HTTP_POST, "meeting/insertSessionMeetingInfo", data, outLen);
    if (msg && outLen>0) {
        SendData(msg, outLen);
        free((void*)msg);
        msg = NULL;
    }
}

void MRTHttpSvrConn::HttpUpdateSessionMeetingEndtime(const char* sign, const char* sessionid)
{
    if (!sign || !sessionid) {
        LE("HttpUpdateSessionMeetingEndtime params error\n");
        return;
    }
    int outLen = 0;
    char data[512] = {0};
    sprintf(data, "sign=%s&sessionid=%s", sign, sessionid);
    const char* msg = GenerateRequest(HTTP_POST, "meeting/updateSessionMeetingEndtime", data, outLen);
    if (msg && outLen>0) {
        SendData(msg, outLen);
        free((void*)msg);
        msg = NULL;
    }
}

void MRTHttpSvrConn::HttpUpdateSessionMeetingNumber(const char* sign, const char* sessionid, const char* sessionnumber, const char* meetingid)
{
    if (!sign || !sessionid || !meetingid || !sessionnumber) {
        LE("HttpUpdateSessionMeetingNumber params error\n");
        return;
    }
    int outLen = 0;
    char data[512] = {0};
    sprintf(data, "sign=%s&sessionid=%s&sessionnumber=%s&meetingid=%s", sign, sessionid, sessionnumber, meetingid);
    const char* msg = GenerateRequest(HTTP_POST, "meeting/updateSessionMeetingNumber", data, outLen);
    if (msg && outLen>0) {
        SendData(msg, outLen);
        free((void*)msg);
        msg = NULL;
    }
}

void MRTHttpSvrConn::HttpUpdateUserMeetingJointime(const char* sign, const char* meetingid)
{
    if (!sign || !meetingid) {
        LE("HttpUpdateUserMeetingJointime params error\n");
        return;
    }
    int outLen = 0;
    char data[512] = {0};
    sprintf(data, "sign=%s&meetingid=%s", sign, meetingid);
    const char* msg = GenerateRequest(HTTP_POST, "meeting/updateUserMeetingJointime", data, outLen);
    if (msg && outLen>0) {
        SendData(msg, outLen);
        free((void*)msg);
        msg = NULL;
    }
}

void MRTHttpSvrConn::HttpUpdateSessionMeetingStatus(const char* sign, const char* sessionid, const char* sessionstatus)
{
    if (!sign || !sessionid || !sessionstatus) {
        LE("HttpUpdateSessionMeetingStatus params error\n");
        return;
    }
    int outLen = 0;
    char data[512] = {0};
    sprintf(data, "sign=%s&sessionid=%s&sessionstatus=%s", sign, sessionid, sessionstatus);
    const char* msg = GenerateRequest(HTTP_POST, "meeting/updateSessionMeetingStatus", data, outLen);
    if (msg && outLen>0) {
        SendData(msg, outLen);
        free((void*)msg);
        msg = NULL;
    }
}


//* HTTP_GET
void MRTHttpSvrConn::HttpGetMeetingInfo(pms::RelayMsg& rmsg, pms::Entity& msg)
{
    int outLen = 0;
    char data[512] = {0};
    const char* meetingid = msg.rom_id().c_str();
    sprintf(data, "meeting/getMeetingInfo/%s", meetingid);
    const char* pmsg = GenerateRequest(HTTP_GET, data, "", outLen);
    if (pmsg && outLen>0) {
        MSender *sender = new MSender(M_HTTP_CMD_GET_MEETING_INFO, rmsg, msg);
        sender->ConnHttpHost(m_httpIp, m_httpPort, m_httpHost);
        sender->SendRequest(pmsg, outLen);
        free((void*)pmsg);
        pmsg = NULL;
    }
}

void MRTHttpSvrConn::HttpGetMeetingInfo(pms::RelayMsg& rmsg, pms::Entity& msg) const
{
    int outLen = 0;
    char data[512] = {0};
    const char* meetingid = msg.rom_id().c_str();
    sprintf(data, "meeting/getMeetingInfo/%s", meetingid);
    const char* pmsg = GenerateRequest(HTTP_GET, data, "", outLen);
    if (pmsg && outLen>0) {
        MSender *sender = new MSender(M_HTTP_CMD_GET_MEETING_INFO, rmsg, msg);
        sender->ConnHttpHost(m_httpIp, m_httpPort, m_httpHost);
        sender->SendRequest(pmsg, outLen);
        free((void*)pmsg);
        pmsg = NULL;
    }
}

void MRTHttpSvrConn::HttpGetMeetingMemberList(pms::RelayMsg& rmsg, pms::Entity& msg)
{
    int outLen = 0;
    char data[512] = {0};
    const char* meetingid = msg.rom_id().c_str();
    sprintf(data, "meeting/getMeetingMemberList/%s", meetingid);
    const char* pmsg = GenerateRequest(HTTP_GET, data, "", outLen);
    if (pmsg && outLen>0) {
        MSender *sender = new MSender(M_HTTP_CMD_GET_MEMBER_LIST, rmsg, msg);
        sender->ConnHttpHost(m_httpIp, m_httpPort, m_httpHost);
        sender->SendRequest(pmsg, outLen);
        free((void*)pmsg);
        pmsg = NULL;
    }
}

void MRTHttpSvrConn::HttpGetMeetingMemberList(pms::RelayMsg& rmsg, pms::Entity& msg) const
{
    int outLen = 0;
    char data[512] = {0};
    const char* meetingid = msg.rom_id().c_str();
    sprintf(data, "meeting/getMeetingMemberList/%s", meetingid);
    const char* pmsg = GenerateRequest(HTTP_GET, data, "", outLen);
    if (pmsg && outLen>0) {
        MSender *sender = new MSender(M_HTTP_CMD_GET_MEMBER_LIST, rmsg, msg);
        sender->ConnHttpHost(m_httpIp, m_httpPort, m_httpHost);
        sender->SendRequest(pmsg, outLen);
        free((void*)pmsg);
        pmsg = NULL;
    }
}


////////////////////////////////////////////////////


int MRTHttpSvrConn::OnWriteEvent(const char*pData, int nLen, int* nOutLen)
{
    MSender *sender = new MSender();
    sender->ConnHttpHost(m_httpIp, m_httpPort, m_httpHost);
    sender->SendRequest(pData, nLen);
    return 0;
}

void MRTHttpSvrConn::ConnectionDisconnected()
{
    LI("%s was called\n", __FUNCTION__);
}

void MRTHttpSvrConn::MSender::OnResponse(const char* pData, int nLen)
{
    if (!pData || nLen<=0) {
        LE("RTHttpSender::OnResponse pData nLen error\n");
        return;
    }
    if (GetMethod() == HTTP_GET) {
        std::string data(pData, nLen);
        MRTRoomManager::Instance().HandleOptRoomWithData(GetCmd(), GetTransmsg(), GetMeetmsg(), data);
    } else {
        this->Signal(kKillEvent);
    }
}

