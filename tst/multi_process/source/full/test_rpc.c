/* ----------------------------------------------------------------------------
 * File:          test_rpc.c
 *
 * Copyright:     2006-2014 Delphi Technologies, Inc., All Rights Reserved
 *                Information Contained Herein Is Proprietary and Confidential
 *
 * Author:        Dariusz Kubiczek
 *
 * Description:   Unit tests of synchronous communication.
 * 
 * --------------------------------------------------------------------------*/

#include "pbc_trace.h"
#include "xsal.h"

EM_FILENUM(TEST_MODULE2_ID, 0);

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/timeb.h>


const SAL_App_Id_T APP_ID = 2;
const size_t NUMBER_OF_THREADS = 2;

const SAL_Thread_Id_T SERVER_THREAD_ID = 1;
const SAL_Thread_Id_T CLIENT_THREAD_ID = 2;

ssize_t Max_Pulse_Num = 256;
size_t Max_Send_Num  = 256;


const SAL_Thread_Attr_T* Init_Thread_Attr(
   const char_t* name, 
   SAL_Thread_Id_T id, 
   SAL_Priority_T prior, 
   SAL_Thread_Attr_T* attr)
{
   SAL_Init_Thread_Attr(attr);

   attr->name = name;
   attr->id = id;
   attr->priority = prior;

   return attr;
}


/**  \file
 *  
 *   Test of send-receive-reply communication (<B><I>XSAL only</I></B>).
 *  
 *   \section T Tests:
 *   <OL>
 *   <LI> \ref STP - test of SAL_Send_Pulse and SAL_Sync_Receive functions. </LI>
 *   <LI> \ref STS - test of SAL_Sync_Send, SAL_SYNC_Receive and SAL_Sync_Reply functions. </LI>
 *   <LI> \ref STSV - test of SAL_Sync_Send_V, SAL_SYNC_Receive and SAL_Sync_Reply_V functions. </LI>
 *   <LI> \ref STR - test of SAL_Sync_Read function. </LI>
 *   </OL>
 *
 *   \section STP Server_Test_Pulse
 *
 *   \subsection TP1 Test procedure:
 *     
 *   <OL>
 *   <LI> CLIENT_THREAD_ID sends the pulse.</LI>
 *   <LI> SERVER_THREAD_ID receives the pulse and checks if all contents of the message are valid.</LI>
 *   </OL>
 *
 *   \section STS Server_Test_Send
 *
 *   \subsection TP2 Test procedure:
 *     
 *   <OL>
 *   <LI> CLIENT_THREAD_ID sends the message. </LI>
 *   <LI> SERVER_THREAD_ID receives the message and checks if all contents of the message are valid. </LI>
 *   <LI> SERVER_THREAD_ID sends the reply. </LI>
 *   <LI> CLIENT_THREAD_ID receives and checks the reply. </LI>
 *
 *   \section STSV Server_Test_SendV
 *
 *   \subsection TP3 Test procedure:
 *
 *   <OL>
 *   <LI> CLIENT_THREAD_ID sends the message. </LI>
 *   <LI> SERVER_THREAD_ID receives two buffers in the message and checks if all contents of the message are valid. </LI>
 *   <LI> SERVER_THREAD_ID sends the reply. </LI>
 *   <LI> CLIENT_THREAD_ID receives and checks the reply. </LI>
 *   </OL>
 *
 *   \section STR Server_Test_Read
 *
 *   \subsection TP4 Test procedure:
 *
 *   <OL>
 *   <LI> CLIENT_THREAD_ID sends the message. </LI>
 *   <LI> SERVER_THREAD_ID receives the message and checks if all contents of the message are valid. </LI>
 *   <LI> SERVER_THREAD_ID reads the rest of the message. </LI>
 *   <LI> SERVER_THREAD_ID sends the reply. </LI>
 *   <LI> CLIENT_THREAD_ID receives and checks the reply. </LI>
 *   </OL>
 *
 */


/*//////////////////////////////////////////////////////////////////////////////////*/

SAL_Channel_T     Channel;

static void Test_Error(const char_t* expr, const char_t* file, int line)
{
   Tr_Fault_3("Assertion failed: '%s', file: '%s', line: %d", expr, file, (int)line);
}

#define CHECK(expr) if (!(expr)) { Test_Error(#expr, __FILE__, __LINE__); SAL_Exit(1); }

/*//////////////////////////////////////////////////////////////////////////////////*/

typedef void (*server_test_t) (SAL_Channel_T* channel);
typedef void (*client_test_t) (SAL_Connection_T* connection);

typedef struct
{
   const char* name;
   server_test_t server_test;
   client_test_t client_test;
} Test;


/**  \name Functions of Server_Test_Pulse
 *
 *   \{
 */


/**  Function of SERVER_THREAD_ID.
 *
 *   - receives pulses
 *   - checks if all contents of pulses are valid
 */

void Server_Test_Pulse(SAL_Channel_T* channel)
{
   size_t msg_len;
   uint8_t is_pulse;
   ssize_t  pulse_no;
   SAL_Pulse_T pulse;
   SAL_Receive_T receive_id;

   Tr_Info_Hi( "Server_Test_Pulse START");

   for(pulse_no=0; pulse_no < Max_Pulse_Num; pulse_no++)
   {
      CHECK(SAL_Sync_Receive(channel, &pulse, sizeof(pulse), &msg_len, &is_pulse, &receive_id));
      Tr_Info_Lo_3("Server_Test_Pulse: is_pulse=%d, code=%d, value=%d",
                   (int)is_pulse, (int)pulse.code, (int)pulse.value.sival_int);

      CHECK(is_pulse);
      CHECK((pulse.code % 128) == (pulse_no % 128));
      CHECK(pulse.value.sival_int == pulse_no);
   }

   Tr_Info_Hi( "Server_Test_Pulse STOP");
}


/**  Function of CLIENT_THREAD_ID.
 *
 *   - sends pulse
 */
void Client_Test_Pulse(SAL_Connection_T* connection)
{
   ssize_t pulse_no;
   struct timeb t1, t2;
   uint32_t dt;

   Tr_Info_Hi( "Client_Test_Pulse START");
   ftime(&t1);
   for(pulse_no=0; pulse_no < Max_Pulse_Num; pulse_no++)
   {
      CHECK(SAL_Send_Pulse(connection, pulse_no % 128, pulse_no));
      Tr_Info_Lo_2("Client_Test_Pulse: code=%d, value=%d", (int)(pulse_no % 128), (int)pulse_no);
   }
   ftime(&t2);
   Tr_Info_Hi( "Client_Test_Pulse STOP");
   dt = (uint32_t)((t2.time - t1.time)*1000 + t2.millitm - t1.millitm);
   Tr_Info_Mid_2("Client_Test_Pulse(%d) time: %d", (int)Max_Pulse_Num, (int)dt);
}

/**  \}
 */
 

/**
 * Test_Send
 */

const char_t Req_Msg_1[] = "Req_Msg_1 3000";
const char_t Rep_Msg_1[] = "reply 1";

void Server_Test_Send(SAL_Channel_T* channel)
{
   size_t test_no;
   int rep_status;
   char_t msg[sizeof(Req_Msg_1)];
   size_t msg_len;
   uint8_t is_pulse;
   SAL_Receive_T receive_id;

   Tr_Info_Hi( "Server_Test_Send START");

   for(test_no=0; test_no < Max_Send_Num; test_no++)
   {
      CHECK(SAL_Sync_Receive(channel, msg, sizeof(msg), &msg_len, &is_pulse, &receive_id));
      CHECK(!is_pulse);
      CHECK(msg_len == sizeof(Req_Msg_1));
      CHECK(memcmp(msg, Req_Msg_1, sizeof(msg)) == 0);

      sscanf(msg, "%*s%d", &rep_status);
      CHECK(SAL_Sync_Reply(&receive_id, rep_status, Rep_Msg_1, sizeof(Rep_Msg_1)));
   }

   Tr_Info_Hi( "Server_Test_Send STOP");
}


void Client_Test_Send(SAL_Connection_T* connection)
{
   size_t test_no;
   int32_t status;
   int status2;
   char_t Rep_Buff[sizeof(Rep_Msg_1)] = {0};

   Tr_Info_Hi( "Client_Test_Send START");

   for(test_no=0; test_no < Max_Send_Num; test_no++)
   {
      CHECK(SAL_Sync_Send(connection, Req_Msg_1, sizeof(Req_Msg_1), Rep_Buff, sizeof(Rep_Buff), &status));
      CHECK(memcmp(Rep_Buff, Rep_Msg_1, sizeof(Rep_Buff)) == 0);

      sscanf(Req_Msg_1, "%*s%d", &status2);
      CHECK(status == status2);
   }

   Tr_Info_Hi( "Client_Test_Send STOP");
}


/**
 * Test_Send_VS
 */

const char_t Req_VS_Msg_1[] = "Req_VS_Msg_1";
const char_t Req_VS_Msg_2[] = " 3000 ";
const char_t Req_VS_Msg_3[] = "Req_VS_Msg_3";

const char_t Rep_VS_Msg_1[] = "Rep_VS_Msg_1";


void Server_Test_Send_VS(SAL_Channel_T* channel)
{
   size_t test_no;
   int status;
   char_t msg[sizeof(Rep_VS_Msg_1)+sizeof(Req_VS_Msg_2)+sizeof(Req_VS_Msg_3)-2];
   char_t msg1[sizeof(Rep_VS_Msg_1)+sizeof(Req_VS_Msg_2)+sizeof(Req_VS_Msg_3)-2];
   size_t msg_len;
   uint8_t is_pulse;
   SAL_Receive_T receive_id;

   snprintf(msg1, sizeof(msg1), "%s%s%s", Req_VS_Msg_1, Req_VS_Msg_2, Req_VS_Msg_3);

   Tr_Info_Hi( "Server_Test_Send_VS START");

   for(test_no=0; test_no < Max_Send_Num; test_no++)
   {
      CHECK(SAL_Sync_Receive(channel, msg, sizeof(msg), &msg_len, &is_pulse, &receive_id));
      CHECK(!is_pulse);
      CHECK(msg_len == sizeof(Rep_VS_Msg_1)+sizeof(Req_VS_Msg_2)+sizeof(Req_VS_Msg_3)-2);
      CHECK(memcmp(msg, msg1, sizeof(msg1)) == 0);

      CHECK(sscanf(msg, "%*s%d%*s", &status) == 1);
      CHECK(SAL_Sync_Reply(&receive_id, status, Rep_VS_Msg_1, sizeof(Rep_VS_Msg_1)));
   }

   Tr_Info_Hi( "Server_Test_Send_VS STOP");
}


void Client_Test_Send_VS(SAL_Connection_T* connection)
{
   size_t test_no;
   int32_t status;
   char_t Rep_Buff[sizeof(Rep_VS_Msg_1)] = {0};
   SAL_IOV_T req_iov[3];

   Tr_Info_Hi( "Client_Test_Send_VS START");

   req_iov[0].iov_base = (void*)Req_VS_Msg_1;
   req_iov[0].iov_len = sizeof(Req_VS_Msg_1)-1;

   req_iov[1].iov_base = (void*)Req_VS_Msg_2;
   req_iov[1].iov_len = sizeof(Req_VS_Msg_2)-1;

   req_iov[2].iov_base = (void*)Req_VS_Msg_3;
   req_iov[2].iov_len = sizeof(Req_VS_Msg_3);   /* Add '\0' char */

   for(test_no=0; test_no < Max_Send_Num; test_no++)
   {
      int status2;

      CHECK(SAL_Sync_Send_VS(connection, req_iov, 3, Rep_Buff, sizeof(Rep_Buff), &status));
      CHECK(memcmp(Rep_Buff, Rep_VS_Msg_1, sizeof(Rep_Buff)) == 0);

      CHECK(sscanf(Req_VS_Msg_2, "%d", &status2) == 1);
      CHECK(status == status2);
   }

   Tr_Info_Hi( "Client_Test_Send_VS STOP");
}



/**
 * Test_Send_SV
 */

const char_t Req_SV_Msg_1[] = "Req_VS_Msg_1 1000000 ";

const char_t Rep_SV_Msg_1[] = "Rep_SV_Msg_1";
const char_t Rep_SV_Msg_2[] = "Rep_SV_Msg_2";
const char_t Rep_SV_Msg_3[] = "Rep_SV_Msg_3";


void Server_Test_Send_SV(SAL_Channel_T* channel)
{
   size_t test_no;
   int status;
   char_t msg[sizeof(Req_SV_Msg_1)];
   size_t msg_len;
   uint8_t is_pulse;
   SAL_Receive_T receive_id;
   SAL_IOV_T rep_iov[3];

   Tr_Info_Hi( "Server_Test_Send_SV START");

   rep_iov[0].iov_base = (void*)Rep_SV_Msg_1;
   rep_iov[0].iov_len = sizeof(Rep_SV_Msg_1);

   rep_iov[1].iov_base = (void*)Rep_SV_Msg_2;
   rep_iov[1].iov_len = sizeof(Rep_SV_Msg_2);

   rep_iov[2].iov_base = (void*)Rep_SV_Msg_3;
   rep_iov[2].iov_len = sizeof(Rep_SV_Msg_3);

   for(test_no=0; test_no < Max_Send_Num; test_no++)
   {
      CHECK(SAL_Sync_Receive(channel, msg, sizeof(msg), &msg_len, &is_pulse, &receive_id));
      CHECK(!is_pulse);
      CHECK(msg_len == sizeof(Req_SV_Msg_1));
      CHECK(memcmp(msg, Req_SV_Msg_1, sizeof(msg)) == 0);
      CHECK(sscanf(msg, "%*s %d", &status) == 1);
      CHECK(SAL_Sync_Reply_V(&receive_id, status, rep_iov, sizeof(rep_iov)/sizeof(rep_iov[0])));
   }

   Tr_Info_Hi( "Server_Test_Send_SV STOP");
}


void Client_Test_Send_SV(SAL_Connection_T* connection)
{
   size_t test_no;
   int32_t status;
   int status2;
   char_t rep1[sizeof(Rep_SV_Msg_1)];
   char_t rep2[sizeof(Rep_SV_Msg_2)];
   char_t rep3[sizeof(Rep_SV_Msg_3)];
   SAL_IOV_T rep_iov[3];

   Tr_Info_Hi( "Client_Test_Send_SV START");

   rep_iov[0].iov_base = (void*)rep1;
   rep_iov[0].iov_len = sizeof(Rep_SV_Msg_1);

   rep_iov[1].iov_base = (void*)rep2;
   rep_iov[1].iov_len = sizeof(Rep_SV_Msg_2);

   rep_iov[2].iov_base = (void*)rep3;
   rep_iov[2].iov_len = sizeof(Rep_SV_Msg_3);

   for(test_no=0; test_no < Max_Send_Num; test_no++)
   {
      CHECK(SAL_Sync_Send_SV(connection, Req_SV_Msg_1, sizeof(Req_SV_Msg_1), rep_iov, 3, &status));
      /*CHECK(memcmp(Rep_Buff, Rep_VS_Msg_1, sizeof(Rep_Buff)) == 0);*/

      sscanf(Req_SV_Msg_1, "%*s %d", &status2);
      CHECK(status == status2);

      CHECK(memcmp(rep1, Rep_SV_Msg_1, sizeof(rep1)) == 0);
      CHECK(memcmp(rep2, Rep_SV_Msg_2, sizeof(rep2)) == 0);
      CHECK(memcmp(rep3, Rep_SV_Msg_3, sizeof(rep3)) == 0);
   }
   Tr_Info_Hi( "Client_Test_Send_SV STOP");
}



/*//////////////////////////////////////////////////////////////////////////////////*/

const char SERVER_MESSAGE [] = "OK";
const char CLIENT_MESSAGE [] = "The Larch";
enum { SERVER_BUFFER_LEN = sizeof (CLIENT_MESSAGE) };
enum { CLIENT_BUFFER_LEN = sizeof (SERVER_MESSAGE) };
const int SERVER_STATUS = 3;




const char_t Request_Message_1[] = "Request1";
const char_t Request_Message_2[] = "55";
const char_t Request_Message_3[] = "Request-3";

const char_t Reply_Message_1[] = "Reply1";
const char_t Reply_Message_2[] = "_Reply_2_";
const char_t Reply_Message_3[] = "reply-3";

/**  \name Functions of Server_Test_Send
 *
 *   \{
 */

/**  Function of SERVER_THREAD_ID.
 *
 *   - receives the message
 *   - checks if all contents of the message are valid
 *   - sends the reply 
 */

void Server_Test_Send_V(SAL_Channel_T* channel)
{
   char_t msg[SERVER_BUFFER_LEN] = {0};
   size_t msg_len;
   uint8_t is_pulse;
   SAL_Receive_T receive_id;

   CHECK(SAL_Sync_Receive(channel, msg, SERVER_BUFFER_LEN, &msg_len, &is_pulse, &receive_id));
   CHECK(!is_pulse);
   CHECK(strcmp(msg, CLIENT_MESSAGE) == 0);
   CHECK(msg_len == strlen(CLIENT_MESSAGE));
   CHECK(SAL_Sync_Reply(&receive_id, SERVER_STATUS, (uint8_t*)SERVER_MESSAGE, strlen(SERVER_MESSAGE)));
}


void Client_Test_Send_V(SAL_Connection_T* connection)
{
   SAL_IOV_T req_iov[3];
   SAL_IOV_T rep_iov[3];
   int32_t status;

   req_iov[0].iov_base = (void*)Request_Message_1;
   req_iov[0].iov_len  = strlen(Request_Message_1);

   req_iov[1].iov_base = (void*)Request_Message_2;
   req_iov[1].iov_len  = strlen(Request_Message_2);

   req_iov[2].iov_base = (void*)Request_Message_3;
   req_iov[2].iov_len  = strlen(Request_Message_3);

/*   char reply_msg [CLIENT_BUFFER_LEN] = {0}; */

   CHECK(SAL_Sync_Send_V(connection, req_iov, sizeof(req_iov)/sizeof(req_iov[0]), req_iov, sizeof(rep_iov)/sizeof(rep_iov[0]), &status));
   CHECK(status == atoi(Request_Message_1));
   /*CHECK(strcmp(reply_msg, SERVER_MESSAGE) == 0);*/
}


/*
void Server_Test_Send ( SAL_Channel_T* channel )
{
   char msg [ SERVER_BUFFER_LEN ] = {0};
   size_t msg_len;
   uint8_t is_pulse;
   SAL_Receive_T receive_id;

   CHECK(SAL_Sync_Receive ( channel, msg, SERVER_BUFFER_LEN, &msg_len, &is_pulse, &receive_id ) );
   CHECK ( ! is_pulse );
   CHECK ( strcmp ( msg, CLIENT_MESSAGE ) == 0 );
   CHECK ( msg_len == strlen ( CLIENT_MESSAGE ) );
   CHECK ( SAL_Sync_Reply ( &receive_id, SERVER_STATUS, (uint8_t*) SERVER_MESSAGE, strlen (SERVER_MESSAGE) ) );
}
*/

/**  Function of CLIENT_THREAD_ID.
 *
 *   - sends the message
 *   - receives and checks the reply
 */
/*
void Client_Test_Send (SAL_Connection_T* connection)
{
   char reply_msg [CLIENT_BUFFER_LEN] = {0};
   int status;

   CHECK(SAL_Sync_Send (connection, (const uint8_t*)CLIENT_MESSAGE, strlen(CLIENT_MESSAGE), (uint8_t*)reply_msg, CLIENT_BUFFER_LEN, &status));
   CHECK(status == SERVER_STATUS);
   CHECK(strcmp(reply_msg, SERVER_MESSAGE) == 0);
}
*/
/**  \}
 */

/*//////////////////////////////////////////////////////////////////////////////////*/

const char CLIENT_MESSAGE_1 [] = "Number One. ";
const char CLIENT_MESSAGE_2 [] = "The Larch.";
enum { CLIENT_BUFFERS_NUMBER = 2 };
enum { SERVER_BUFFER_LEN_V = sizeof (CLIENT_MESSAGE_1) + sizeof (CLIENT_MESSAGE_2) };
const char SERVER_MESSAGE_1 [] = "OK. ";
const char SERVER_MESSAGE_2 [] = "OK";
enum { SERVER_BUFFERS_NUMBER = 2 };
enum { CLIENT_BUFFER_LEN_V = sizeof (SERVER_MESSAGE_1) + sizeof (SERVER_MESSAGE_2) };
const int SERVER_STATUS_V = 5;

/**  \name Functions of Server_Test_SendV
 *
 *   \{
 */

/**  Function of SERVER_THREAD_ID.
 *
 *   - receives the message
 *   - checks if contents of the message are valid
 *   - sends the reply
 */

void Server_Test_Sendv ( SAL_Channel_T* channel )
{
   size_t msg_len;
   uint8_t is_pulse;
   SAL_Receive_T receive_id;
   char msg [ SERVER_BUFFER_LEN_V ] = {0}; 
   SAL_IOV_T reply_buffers [SERVER_BUFFERS_NUMBER] =
   {
      { (void*) SERVER_MESSAGE_1, strlen ( SERVER_MESSAGE_1 ) },
      { (void*) SERVER_MESSAGE_2, strlen ( SERVER_MESSAGE_2 ) },
   };

   CHECK ( SAL_Sync_Receive ( channel, msg, SERVER_BUFFER_LEN_V, &msg_len, &is_pulse, &receive_id ) );
   CHECK ( ! is_pulse );
   CHECK ( strncmp ( msg, CLIENT_MESSAGE_1, strlen ( CLIENT_MESSAGE_1 ) ) == 0 );
   CHECK ( strncmp ( msg + strlen ( CLIENT_MESSAGE_1 ), CLIENT_MESSAGE_2, strlen ( CLIENT_MESSAGE_2 ) ) == 0 );
   CHECK ( msg_len == strlen ( CLIENT_MESSAGE_1 ) + strlen ( CLIENT_MESSAGE_2 ) );
   CHECK ( SAL_Sync_Reply_V ( &receive_id, SERVER_STATUS_V, reply_buffers, SERVER_BUFFERS_NUMBER ) );
}

/**  Function of CLIENT_THREAD_ID.
 *
 *   - sends two buffers in the message
 *   - receives and checks the reply
 */

void Client_Test_Sendv (SAL_Connection_T* connection)
{
   int32_t status;
   char reply_msg [ CLIENT_BUFFER_LEN_V ] = {0};
   SAL_IOV_T buffers [CLIENT_BUFFERS_NUMBER] =
   {
      { (void*) CLIENT_MESSAGE_1, strlen ( CLIENT_MESSAGE_1 ) },
      { (void*) CLIENT_MESSAGE_2, strlen ( CLIENT_MESSAGE_2 ) },
   };

   CHECK ( SAL_Sync_Send_VS( connection, buffers, CLIENT_BUFFERS_NUMBER, (uint8_t*) reply_msg, CLIENT_BUFFER_LEN_V, &status ) );
   CHECK ( strncmp ( reply_msg, SERVER_MESSAGE_1, strlen ( SERVER_MESSAGE_1 ) ) == 0 );
   CHECK ( strncmp ( reply_msg + strlen ( SERVER_MESSAGE_1 ), SERVER_MESSAGE_2, strlen ( SERVER_MESSAGE_2 ) ) == 0 );
   CHECK ( status == SERVER_STATUS_V );
}

/** \}
 */

/*//////////////////////////////////////////////////////////////////////////////////*/

const char CLIENT_MESSAGE_R [] = "I see trees of green, red roses too"
                                 "I see them bloom, for me and you."
                                 "And I think to myself..."
                                 "What a wonderful world.";

enum { SERVER_BUFFER_LEN_R = 10 };
const int SERVER_STATUS_R = 7;

static int Get_Min ( int x, int y )
{
   return x < y ? x : y;
}

/**  \name Functions of Server_Test_Read
 *
 *   \{
 */

/**  Function of SERVER_THREAD_ID.
 *
 *   - receives the message
 *   - checks if contents of message are valid
 *   - reads the rest of the message
 *   - sends the reply
 */

void Server_Test_Read ( SAL_Channel_T* channel )
{
   char msg [ SERVER_BUFFER_LEN_R ] = {0};
   size_t msg_len;
   uint8_t is_pulse;
   SAL_Receive_T receive_id;
   size_t part_msg_len;
   int to_read;

   CHECK ( SAL_Sync_Receive ( channel, msg, SERVER_BUFFER_LEN_R, &msg_len, &is_pulse, &receive_id ) );
   CHECK ( ! is_pulse );
   CHECK ( msg_len == strlen ( CLIENT_MESSAGE_R ) );
   CHECK ( strncmp ( msg, CLIENT_MESSAGE_R, Get_Min ( msg_len, SERVER_BUFFER_LEN_R ) ) == 0 );

   for ( to_read = msg_len - SERVER_BUFFER_LEN_R; to_read > 0; to_read -= part_msg_len )
   {
      CHECK ( SAL_Sync_Read ( &receive_id, msg, SERVER_BUFFER_LEN_R, &part_msg_len ) )
      CHECK ( strncmp ( msg, CLIENT_MESSAGE_R + msg_len - to_read, part_msg_len ) == 0 );
   }

   CHECK ( SAL_Sync_Reply ( &receive_id, SERVER_STATUS_R, 0, 0 ) );
}

/**  Function of CLIENT_THREAD_ID.
 *
 *   - sends the message
 *   - checks the reply status
 */

void Client_Test_Read ( SAL_Connection_T* connection )
{
   int32_t status;
   
   CHECK ( SAL_Sync_Send ( connection, (const uint8_t*) CLIENT_MESSAGE_R, strlen ( CLIENT_MESSAGE_R ), 0, 0, &status ) );
   CHECK ( status == SERVER_STATUS_R );
}


/**  \}
 */

/*//////////////////////////////////////////////////////////////////////////////////*/


/**  \name Functions of server
 *
 *   \{
 */




/**  Function of server thread.
 *
 *   - creates the channel
 *   - runs the test of server
 *   - destroys the channel
 */
void Server_Thread (void* param)
{
   CHECK(SAL_Create_Channel(&Channel));
   SAL_Signal_Ready ();

   ((Test*)param)->server_test(&Channel);

   CHECK(SAL_Destroy_Channel(&Channel));
}

/** \}
 */

/*//////////////////////////////////////////////////////////////////////////////////*/

/**  \name Functions of client
 *
 *   \{
 */


/*//////////////////////////////////////////////////////////////////////////////////*/

/**  Function runs client tests.
 */
/*
void Run_Client_Test(Test* test, SAL_Connection_T* connection)
{
   test->client_test(connection);
}
*/

/*//////////////////////////////////////////////////////////////////////////////////*/

/**  Function of client thread.
 *
 *   - creates connection
 *   - runs client test
 *   - destroys connection
 */

void Client_Thread(void* param)
{
   SAL_Connection_T connection;
   const SAL_Thread_Id_T thread_id_list [] = { SERVER_THREAD_ID };

   SAL_Wait_Ready(thread_id_list, sizeof(thread_id_list)/sizeof(thread_id_list[0]));

   CHECK(SAL_Create_Connection(&Channel, &connection));

   ((Test*)param)->client_test(&connection);

   CHECK(SAL_Disconnect(&connection));
}

/** \}
 */

Test Tests [] =
{
   { "pulse", Server_Test_Pulse, Client_Test_Pulse },
   { "send & reply", Server_Test_Send, Client_Test_Send },
   { "send_vs & reply", Server_Test_Send_VS, Client_Test_Send_VS },
   { "send_sv & reply_v", Server_Test_Send_SV, Client_Test_Send_SV },
   /*{ "send_v & reply_v", Server_Test_Send_V, Client_Test_Send_V },
   *{ "send_sv & reply", Server_Test_Send, Client_Test_Send },
   *{ "sendv & replyv", Server_Test_Sendv, Client_Test_Sendv },
   *{ "read", Server_Test_Read, Client_Test_Read },
   */
};


bool_t Set_Up()
{
   bool_t status;
   SAL_Config_T config;

   SAL_Get_Config(&config);
   
   config.app_id = APP_ID;
   config.max_number_of_threads = NUMBER_OF_THREADS;

   status = SAL_Init(&config);
   if (!status)
   {
      Tr_Fault("SAL_Init error");
   }

   return status;
}


int32_t RPC_Test_Suite()
{
   size_t i;
   int32_t status = 0;
   SAL_Priority_T priority;
   SAL_Thread_Attr_T thread_attr;

   SAL_Get_Thread_Priority(SAL_Get_Thread_Id(), &priority);

   for (i=0; status == 0 && (i < sizeof(Tests)/sizeof(Tests[0])); i++)
   {
      SAL_Thread_Id_T wait_list[] = { SERVER_THREAD_ID, CLIENT_THREAD_ID };

      CHECK(SAL_Create_Thread(
         Server_Thread, 
         &Tests[i], 
         Init_Thread_Attr("Server", SERVER_THREAD_ID, priority, &thread_attr)));

      CHECK(SAL_Create_Thread(
         Client_Thread, 
         &Tests[i], 
         Init_Thread_Attr("Client", CLIENT_THREAD_ID, priority, &thread_attr)));

      status = SAL_Run();
      if (status == 0)
      {
         SAL_Wait_Destroyed(wait_list, sizeof(wait_list)/sizeof(wait_list[0]));
      }
   }
   return status;
}


void Tear_Down()
{
}


int main(int argc, char_t* argv[])
{
   int32_t ret = 0;

   if (Set_Up())
   {
      ret = RPC_Test_Suite();

      Tear_Down();
   }
   else
   {
      Tr_Fault("Set_Up failed");
      ret = 1;
   }

   Tr_Info_Hi_1("Test %s", ((ret)?("FAILED"):("PASSED")));

   return ret;
}


/* --------------------------------------------------------------------------
 * Revision history:
 *
 * 25 Jul 2006, Dariusz Kubiczek (dariusz.kubiczek@delphiauto.com)
 *   Initial revision
 *
 * 26-Sep-2013 Larry Piekarski Rev
 *   Task 1334: Fixed compiler warnings
 *
 * - 30-Aug-2014 Kirk Bailey
 *   - Fixed Klocwork defects.
 * ------------------------------------------------------------------------*/
