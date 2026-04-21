#pragma once

#include <winsock2.h>
//#define		nfpProtocolVersion				"1.0.0.3" // +++ 2014.07.03 : ����� 2�� (���� �ʴ� ��� ����)
#define		nfpProtocolVersion				"1.0.0.4" // +++ 2015.12.29 : �α��� �Ϸ� �� ��ȣȭ �� ��й�ȣ�� ������ ���� �޴´�.

///////////  ��� ��������
#define		NMS_C_START						300
#define		NMS_C_START_OK					301
#define		NMS_C_AUTHENTICATE				321
#define		NMS_C_AUTHENTICATE_ERROR		322
#define		NMS_C_AUTHENTICATE_OK			323
#define		NMS_C_CLIENT_INFO				324
#define		NMS_C_INCOMING_TEST				325
#define		NMS_C_INCOMING_TEST_SKIP		326
#define		NMS_C_LISTEN_READY				327
#define		NMS_C_INCOMING_SUCCESS			328
#define		NMS_C_INCOMING_FAIL				329
#define		NMS_C_N2N_SERVER				330
#define		NMS_C_CTOS						341
#define		NMS_C_STOC						342
#define		NMS_C_CSTONMS					343
#define		NMS_C_SCTONMS					343
#define		NMS_C_SELF						344
#define		NMS_C_OK						345
#define		NMS_C_CTOS_NFTD					346
#define		NMS_C_STOC_NFTD					347
#define		NMS_C_CSTONMS_NFTD				348
#define		NMS_C_SCTONMS_NFTD				348
#define		KMS_C_CONNECTION_HISTORY		349
#define		KMS_C_CLIENT_INFO_FORCE_CONN	350
#define		KMS_C_N2N_CONNECTION			351
#define		NMS_C_EXCEED					352

///////////  ���� ��������
#define		NMS_LS_START					700
#define		NMS_LS_START_OK					701
#define		NMS_LS_START_IPDENIED_ERROR		702
#define		NMS_LS_LOGIN					721
#define		NMS_LS_LOGIN_ERROR				722
#define		NMS_LS_LOGIN_OK					723
#define		NMS_LS_LOGOUT					724
#define		NMS_LS_WEB_LOGIN				725
#define		NMS_LS_ALREADY_LOGIN			726
#define		NMS_LS_ALREADY_WEB_LOGIN		727
#define		NMS_LS_FORCE_LOGIN				728
#define		NMS_LS_FORCE_WEB_LOGIN			729
#define		NMS_LS_FORCE_SEND_MSG			730

#define		NMS_LS_CREATE_SESSION			731
#define		NMS_LS_SESSION_CREATED			732
#define		NMS_LS_DELETE_SESSION			733
#define		NMS_LS_SESSION_DELETED			734
#define		NMS_LS_CONNECT_SESSION			735
#define		NMS_LS_SESSION_CONNECTED		736
#define		NMS_LS_CREATE_COUPON_SESSION	737
#define		NMS_LS_COUPON_SESSION_CREATED	738
#define		NMS_LS_COUPON_NO_EXIST_ERROR	739
#define		NMS_LS_COUPON_MAX_TIME_USED_ERROR	740
#define		NMS_LS_COUPON_EXPIRED_ERROR		741
#define		NMS_LS_CREATE_SHORT_SESSION		742
#define		NMS_LS_SHORT_SESSION_CREATED	743
#define		NMS_LS_FORCE_SEND_MSG1			744 // �����ȵ� ���� -> ������ ����
#define		NMS_LS_FORCE_SEND_MSG1_Error	745 
#define		NMS_LS_FORCE_SEND_MSG2			746 // ������ ���� -> �����ȵ� ����
#define		NMS_LS_FORCE_SEND_MSG2_Error	747 
#define		NMS_LS_PROTOCOL_VERSION			748 
#define		NMS_LS_FORCE_SEND_MSG3			749 // ������ ���� -> �����ȵ� ����

#define		NMS_LS_UPDATE_SESSION_DATA		751
#define		NMS_LS_WRITE_LOG_REPORT_LEN		752
#define		NMS_LS_WRITE_LOG_FIXED_LEN_INFO	753
#define		NMS_LS_WRITE_LOG_REPORT			754
#define		NMS_LS_WRITE_LOG_ACK			755
#define		NMS_LS_FORCE_CHAT		        756
#define		NMS_LS_REQUEST_AGENTLIST        757
#define		NMS_LS_REQUEST_AGENTLIST_ACK    758	
#define		NMS_LS_INVITEAGENT              759
#define		NMS_LS_INVITEAGENT_ACK          760	
#define		NMS_LS_LOGIN_CHECK_IPADDRESS	761
#define		NMS_LS_LOGIN_CHECK_MACADDRESS	762
#define		NMS_LS_LOGIN_IPDENIED_ERROR		763
#define		NMS_LS_LOGIN_MACDENIED_ERROR	764
#define		NMS_LS_MOBILE_CS_CONNECT		765	 // [Mobile]
#define		NMS_LS_VIDEO_CS_CONNECT			766  // by rainmaker, for video edition
#define		NMS_LS_MOBILE_IOS_CS_CONNECT	767	 // [Mobile]

#define		NMS_LS_NO_MEANING				790

///////////  ���� ��������
#define		NMS_CS_TCP_KEEP_CONNECTION		421
#define		NMS_CS_TCP_VIEWER_CONNECT		431
#define		NMS_CS_TCP_PORT_INFO		 	432
#define		NMS_CS_TCP_CTOS					441
#define		NMS_CS_TCP_STOC					442
#define		NMS_CS_TCP_CSTONMS				443
#define		NMS_CS_TCP_SCTONMS				443
#define		NMS_CS_TCP_SELF					444
#define		NMS_CS_TCP_LOGOUT				462
#define		KMS_CS_TCP_VIEWER_CONNECT_NAT	470
#define		NMS_CS_TCP_SESSION_DELETED      472

#define		P2P_CS_SERVERNUM				610
#define		P2P_CS_PINGCHECK				611
#define		P2P_CS_PINGCHECK_ACK			612
#define		P2P_CS_PINGCHECK_ACK2			613
#define		P2P_CS_SERVERNUM2				614 
#define		P2P_C_SERVERNUM					620
#define		P2P_C_SERVERNUM2				621 

#define		NMS_CS_WEB_START				800
#define		NMS_CS_WEB_START_OK				801
#define		NMS_CS_WEB_END					802 
#define		NMS_CS_WEB_SESSION_RESTORE		803 
#define		NMS_CS_WEB_AUTHENTICATE			821
#define		NMS_CS_WEB_AUTHENTICATE_ERROR	822
#define		NMS_CS_WEB_AUTHENTICATE_OK		823
#define		NMS_CS_WEB_SERVER_IP_NAME		824
#define		NMS_CS_WEB_INCOMING_TEST		825
#define		NMS_CS_WEB_INCOMING_TEST_SKIP	826
#define		NMS_CS_WEB_LISTEN_READY			827
#define		NMS_CS_WEB_INCOMING_SUCCESS		828
#define		NMS_CS_WEB_INCOMING_FAIL		829
#define		NMS_CS_WEB_SERVER_FULL			831
#define		NMS_CS_WEB_CONNINFO				832
#define		NMS_CS_WEB_SERVER_NUM			835
#define		NMS_CS_WEB_N2N_LIST				841
#define		NMS_CS_WEB_N2N_SELECT			842
#define		NMS_CS_WEB_OK					851



/////////// LMMAgent + CS매니저 프로토콜
#define		NMS_CS_START					200
#define		NMS_CS_START_OK					201
#define		NMS_CS_RSA_PUBLIC_KEY			211
#define		NMS_CS_AES_SECRET_KEY			212
#define		NMS_CS_LOGIN					221
#define		NMS_CS_AUTO_LOGIN				230
#define		NMS_CS_LOGIN_ERROR				222
#define		NMS_CS_LOGIN_OK					223
#define		NMS_CS_SERVER_IP_NAME			224
#define		NMS_CS_INCOMING_TEST			225
#define		NMS_CS_INCOMING_TEST_SKIP		226
#define		NMS_CS_LISTEN_READY				227
#define		NMS_CS_INCOMING_SUCCESS			228
#define		NMS_CS_INCOMING_FAIL			229
#define		NMS_CS_SERVER_FULL				231
#define		NMS_CS_SERVER_OK				232
#define		NMS_CS_SERVER_LIST				233
#define		NMS_CS_SERVER_SELECT			234
#define		NMS_CS_SERVER_NUM				235
#define		NMS_CS_N2N_LIST					241
#define		NMS_CS_N2N_SELECT				242
#define		NMS_CS_OK						251
#define		NMS_CS_EXCEED					260
#define		NMS_CS_LOGIN_AUTH_EXPIRED		270
#define		NMS_CS_LOGIN_ERROR_NOT_FOUND	272

/////////// LMM 프로토콜
#define		NMS_LMM_START					900
#define		NMS_LMM_START_OK				901
#define		NMS_LMM_LOGIN					902
#define		NMS_LMM_LOGIN_ERROR				903
#define		NMS_LMM_ALREADY_LOGIN			904
#define		NMS_LMM_LOGIN_OK				905
#define		NMS_LMM_FORCE_SEND_MSG			906
#define		NMS_LMM_FORCE_CHAT				907
#define		NMS_LMM_FORCE_LOGIN				908
#define		NMS_LMM_FORCE_LOGIN_OK			909
#define		NMS_LMM_REQUEST_INFO			910
#define		NMS_LMM_REQUEST_INFO_ACK		911
#define		NMS_LMM_LOGOFF					912
#define		NMS_LMM_UPDATE_HOST				913
#define		NMS_LMM_COMMAND					914
#define		NMS_LMM_COMMAND_WEB				915
#define		NMS_LMM_ADD_INFO				917
#define		NMS_LMM_ADD_INFO_OK				918

///////////  ���� �������� (defined by drumpet)
#define		NMS_LS_LOGOFF					2101
#define		NMS_LS_REQUEST_URL				2200
#define		NMS_LS_REQUEST_URL_ACK			2201
#define		NMS_LS_REQUEST_TROUBLES			2202
#define		NMS_LS_REQUEST_TROUBLES_ACK		2203
#define		NMS_LS_TROUBLE_CONTENT			2204
#define		NMS_LS_RECV_TROUBLES_OK			2205
#define		NMS_LS_TERMINATE_SESSION_DATA	2206

#define		NMS_LS_PASSWORD					2207

const int MAX_ID_LEN = 12; // 128 -> 12
const int MAX_PW_LEN = 16; // 48 -> 16 

#pragma pack (push, 1)

typedef struct _msg0 { // 2 byte
	short command;
} msg0;

typedef struct _msg66 { // 68 byte 
	short command;
	char temp[66];
} msg66;

typedef struct _msg268 { // 268 byte
	short command;
	char data[266];
} msg268;

typedef struct _msg_login {
	short command;
	char id[13];
	char passwd[13];
	char temp[24];
} msg_login;

typedef struct _msg_authenticate { // 52 byte
	short command;
	char titlekey[50];
} msg_authenticate;

// by hchkim(2009.02.05)
// �̹� �α��� �� ������ ���� ��� ���� ������ ������ ������� �Ѵ�.
typedef struct _msg_ls_login {
	short command;
	char id[13];
	char passwd[13];
	char agent_priip[16];
	char com_name[20]; 
	char temp[24];
} msg_ls_login;

typedef struct _msg_login_info {
	short command;
	char id[13];
	char agent_priip[16];
	char agent_pubip[16];
	char com_name[20]; 
	int function;
	int setting;
	char temp[13];
} msg_login_info;

typedef struct _msg_ls_authenticate {
	short command;
	char titlekey[50];
	char agent_priip[16];
	char com_name[20]; 
} msg_ls_authenticate;

// 2019.08.06 fix : ���� OS ���� ����
/*typedef struct _msg_ls_protocol_ver { // 88 bit
	short command;
	char protocolVer[15];
	char macAddress[20];
	char temp[51];
} msg_ls_protocol_ver;*/

typedef struct _msg_ls_protocol_ver { // 88 bit
	short command;
	char protocolVer[15];
	char macAddress[20];
	short ostype;
	char temp[49];
} msg_ls_protocol_ver;
/***/

typedef struct _msg_ls_forcelogin_msg {
	short command;
	char cmsg[512];
}msg_ls_forcelogin_msg;

typedef struct _msg_ls_loginchat_header_68{ // 68 bit [�ߺ� �α��� �޽��� ������]
	short command;
	int protocolVer;
	int packetSize;
	char temp[58];
}msg_ls_loginchat_header_68;

typedef struct _msg_ls_loginchat_header_88{ // 88 bit
	short command;
	int protocolVer;
	int packetSize;
	char temp[78];
}msg_ls_loginchat_header_88;

typedef struct _msg_ls_loginchat_msg{ // 1024 bit
	short command;
	char loginwaitID[128];
	char sender_pubip[16];
	char sender_priip[16];
	char cmsg[512];
	char temp[350];
}msg_ls_loginchat_msg;

typedef struct _msg_ls_info { 
	short command;
	char id[13];
	int servernum;
	short oneuse;
	short freeuser;
	int function;
	int setting;
	int mobile_setting; // for mobile
	char temp[53];
} msg_ls_info;

typedef struct _msg_session_info {
	short command;
	unsigned int accesscode;
	short status;
	char ip[16];
	char time[20];
	char couponcode[11];
	//char temp[13];
	int reportnum; // 2022.12.11 [����û] : reportnum�� �����ش�.
	char temp[9];
} msg_session_info;

/*
typedef struct _msg_session_key {
	short command;
	unsigned int accesscode;
	short status;
	//char key[40];
	char key[36];
	int reportnum; // 2022.12.11 [����û] : reportnum�� �����ش�. 
	unsigned int ostype; // by rainmaker, for MAC
	char ip[16];
} msg_session_key;
*/
typedef struct _msg_session_key {
	short command;
	unsigned int accesscode;
	short status;
	char key[40];
	//char key[44];
	unsigned int ostype; // by rainmaker, for MAC
	char ip[16];
} msg_session_key;


typedef struct _msg_session_key_info{
	char name[30];
	char email[40];
	char tel[30];
	char time[20];
} msg_session_key_info;

typedef struct _msg_log {
	short command;
	int reportnum;
	unsigned int accesscode;
	char name[30];
	char email[40];
	char tel[30];
	char ip[16];
	char starttime[20];
	char endtime[20];
	int trbnum;
	short operationlog;
} msg_log;

typedef struct _msg_log_ack {
	short command;
	int reportnum;
	unsigned int accesscode;
	char temp[58];
} msg_log_ack;

typedef struct _msg_conninfo_ack {
	short command;
	char company[50];
	char personnel[30];
	char phone[15];
	char time[20];
} msg_conninfo_ack;

typedef struct _msg50 {
	short command;
	char data[50];
} msg50;

typedef struct _msg_trouble_num {
	short command;
	int trouble_num;
} msg_trouble_num;

typedef struct _msg_trouble_content {
	short command;
	char data[50];
	int trbnum;
} msg_trouble_content;

typedef struct _msg_ls_request_agent_list {
	short command;
	unsigned int accesscode;
	char temp[62];
} msg_ls_request_agent_list;

typedef struct _msg_ls_request_agent_list_ack {
	short command;
	unsigned int agentnum;
	unsigned int accesscode;
	char temp[58];
} msg_ls_request_agent_list_ack;

typedef struct _msg_ls_agent_info{
	char id[13];
	char name[30];
} msg_ls_agent_info;

typedef struct _msg_ls_invite_agent{
	short command;
	unsigned int accesscode;
	char FromAgentID[13];
	char ToAgentID[13];
	short isMobile; // [Mobile]
	char temp[34];
} msg_ls_invite_agent;

typedef struct _msg_ls_invite_agent_ack{
	short command;
	unsigned int result; /* 0: ����, 1: ����, 2: ���� �α׾ƿ� 3: ȣ��Ʈ ���� */
	unsigned int accesscode;
	char FromAgentID[13];
	char ToAgentID[13];
	char temp[32];
} msg_ls_invite_agent_ack;

typedef struct _msg_ls_invite_agent_sesssionkey{
	char key[44];
} msg_ls_invite_agent_sesssionkey;



typedef struct _trouble_content {
	int trbnum;
	char data[50];
} trouble_content , *pTrouble_content;

typedef struct _supporter_password {
	short command;
	char password[256];
}msg_ls_password;

typedef struct _msg_upload_screenshot {  // 2019.01.22 add - ��ũ���� ����
	TCHAR serverUrl[1024];
	unsigned int uploadInterval;
	TCHAR sptid[256];
	int reportnum;
	int accesscode;
	TCHAR starttime[256];
} msg_upload_screenshot;

typedef struct _msg_update_filehistory {  // 2019.10.28 [�ϳ��������� - �������۱��]
	TCHAR webIP[256];
	int webPort;
	int useHttps;
	TCHAR fullUrl[1024];
	TCHAR subUrl[1024];
	TCHAR sptid[256];
	int reportnum;
	int accesscode;
	TCHAR starttime[256];
} msg_upload_filehistory;

typedef struct _msg_login_ip {
	short command;
	char ip[16];
	char com_name[20];
	short ostype;
	char temp[12];;
} msg_login_ip;

typedef struct _msg_login_ip_v2 {
	short command;
	char ip[16];
	char com_name[20];
	short ostype;
	int opt;
	char temp[8];
} msg_login_ip_v2;

typedef struct _msg_server_info {
	short command;
	char server_name[20];
	char server_ip[16];
	short server_port;
	char server_pubip[16];
	char server_priip[16];
} msg_server_info;

typedef struct _msg_server_num {
	short command;
	int server_num;
	short n2nserver_num;
	char temp[44];
} msg_server_num;

typedef struct _msg_ls_conninfo {
	short command;
	char company[30];
	char personnel[30];
	char phone[15];
	char time[20];
	char email[50];
} msg_ls_conninfo;

typedef struct _msg_pingcheck {
	short command;
	DWORD timestamp;
} msg_pingcheck;

typedef struct _msg_keep_connection {
	short command;
	int server_num;
	char id[MAX_ID_LEN + 1];
	char passwd[MAX_ID_LEN + 1];
	char ip[16];
	char computer_name[20];
} msg_keep_connection;

typedef struct _msg_listen_ready {
	short command;
	short port_info;
	int port1;
	int port2;
	char temp[56];
} msg_listen_ready;

/////////// LMM Agent 전용 구조체
typedef struct _msg_login_lmm {
	short command;
	char id[128];
	char passwd[48];
	char token[31];
	char version[24];
} msg_login_lmm;

typedef struct _msg_login_ip_lmm {
	short command;
	char ip[16];
	char com_name[48];
	short ostype;
	char mac_addr[20];
	char device_id[48];
} msg_login_ip_lmm;

typedef struct _msg_lmm_add_info {
	short command;
	char wol_mac_addr[1024];
	char temp1[128];
	char temp2[128];
	int temp3;
	int temp4;
} msg_lmm_add_info;

#pragma pack (pop)