/**
 *  Copyright 2010-2016 Comcast Cable Communications Management, LLC
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */
#include <assert.h>
#include <errno.h>
#include <pthread.h>
#include <malloc.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include <nopoll.h>
#include <wrp-c.h>
#include <nanomsg/nn.h>

#include "../src/upstream.h"
#include "../src/config.h"
#include "../src/client_list.h"
#include "../src/ParodusInternal.h"
#include "../src/partners_check.h"
#include "../src/peer2peer.h"

/*----------------------------------------------------------------------------*/
/*                            File Scoped Variables                           */
/*----------------------------------------------------------------------------*/
static noPollConn conn;
static char *reconnect_reason = "webpa_process_starts";
static ParodusCfg parodusCfg;
extern size_t metaPackSize;
extern UpStreamMsg *UpStreamMsgQ;
int numLoops = 1;
wrp_msg_t *temp = NULL;
char *notification;
/*----------------------------------------------------------------------------*/
/*                                   Mocks                                    */
/*----------------------------------------------------------------------------*/

void addCRUDmsgToQueue(wrp_msg_t *crudMsg)
{
	UNUSED(crudMsg) ;
	return;
}

noPollConn *get_global_conn()
{
    return &conn;   
}

char *get_global_reconnect_reason()
{
    return reconnect_reason;
}

reg_list_item_t * get_global_node(void)
{
    function_called();
    return mock_ptr_type(reg_list_item_t *);
}

int get_numOfClients()
{
    function_called();
    return (int)mock();
}
void sendMessage(noPollConn *conn, void *msg, size_t len)
{
    (void) conn; (void) msg; (void) len;
    function_called();
}

ParodusCfg *get_parodus_cfg(void) 
{
    return &parodusCfg;
}

ssize_t wrp_pack_metadata( const data_t *packData, void **data )
{
    (void) packData; (void) data;
    function_called();
    
    return (ssize_t)mock();
}

size_t appendEncodedData( void **appendData, void *encodedBuffer, size_t encodedSize, void *metadataPack, size_t metadataSize )
{
    (void) encodedBuffer; (void) encodedSize; (void) metadataPack; (void) metadataSize;
    function_called();
    char *data = (char *) malloc (sizeof(char) * 100);
    parStrncpy(data, "AAAAAAAAYYYYIGkYTUYFJH", 100);
    *appendData = data;
    return (size_t)mock();
}

int sendAuthStatus(reg_list_item_t *new_node)
{
    (void) new_node;
    function_called();
    return (int)mock();
}

int addToList( wrp_msg_t **msg)
{
    (void) msg;
    function_called();
    return (int)mock();
}

int nn_socket (int domain, int protocol)
{
    (void) domain; (void) protocol;
    function_called();
    return (int)mock();
}

ssize_t wrp_to_struct( const void *bytes, const size_t length, const enum wrp_format fmt, wrp_msg_t **msg )
{
    UNUSED(bytes); UNUSED(length); UNUSED(fmt);
    function_called();
    *msg = temp;
    return (ssize_t)mock();
}

void wrp_free_struct( wrp_msg_t *msg )
{
    UNUSED(msg);
    function_called();
}

int nn_shutdown (int s, int how)
{
    UNUSED(s); UNUSED(how);
    function_called();
    return (int)mock();
}

int nn_setsockopt (int s, int level, int option, const void *optval, size_t optvallen)
{
    UNUSED(s); UNUSED(level); UNUSED(option); UNUSED(optval); UNUSED(optvallen);
    function_called();
    return (int)mock();
}

int nn_connect (int s, const char *addr)
{
    UNUSED(s); UNUSED(addr);
    function_called();
    return (int)mock();
}

int validate_partner_id(wrp_msg_t *msg, partners_t **partnerIds)
{
    UNUSED(msg); UNUSED(partnerIds);
    function_called();
    return (int) mock();
}

void filter_clients_and_send(wrp_msg_t *wrp_event_msg)
{
    UNUSED(wrp_event_msg);
    function_called();
}

ssize_t check_inbox(int sock, void **msg)
{
    UNUSED(sock);
    *msg = notification;
    function_called();
    return (ssize_t)mock();
}

void free_msg(void *msg)
{
    UNUSED(msg);
    function_called();
}

bool send_msg(int sock, const void *msg, size_t size)
{
    UNUSED(sock); UNUSED(msg); UNUSED(size);
    function_called();
    return (bool)mock();
}
void messageHandlerTask()
{
}

void CRUDHandlerTask()
{
}
/*----------------------------------------------------------------------------*/
/*                                   Tests                                    */
/*----------------------------------------------------------------------------*/


void test_packMetaData()
{
    will_return(wrp_pack_metadata, 100);
    expect_function_call(wrp_pack_metadata);
    packMetaData();
}

void err_packMetaData()
{
    will_return(wrp_pack_metadata, 0);
    expect_function_call(wrp_pack_metadata);
    packMetaData();
}

void test_handleUpstreamNull()
{
    numLoops = 1;
    handle_upstream(NULL);
}

void test_handle_upstream()
{
    numLoops = 1;
    socket_handles_t sock;
    sock.pipeline = 1;
    sock.pubsub = 0;
    sock.local = 2;
    UpStreamMsgQ = (UpStreamMsg *) malloc(sizeof(UpStreamMsg));
    UpStreamMsgQ->msg = "First Message";
    UpStreamMsgQ->len = 13;
    UpStreamMsgQ->next = (UpStreamMsg *) malloc(sizeof(UpStreamMsg));
    UpStreamMsgQ->next->msg = "Second Message";
    UpStreamMsgQ->next->len = 15;
    UpStreamMsgQ->next->next = NULL;
    notification = "Hello";
    expect_function_call(check_inbox);
    will_return(check_inbox, 6);
    handle_upstream((void *)&sock);
    free(UpStreamMsgQ->next);
    free(UpStreamMsgQ);
}

void err_handleUpstream()
{
    socket_handles_t sock;
    sock.pipeline = 1;
    sock.pubsub = 0;
    sock.local = 2;
    notification = NULL;
    expect_function_call(check_inbox);
    will_return(check_inbox, -1);
    handle_upstream((void *)&sock);
}

void test_processUpstreamMessage()
{
    numLoops = 1;
    metaPackSize = 20;
    UpStreamMsgQ = (UpStreamMsg *) malloc(sizeof(UpStreamMsg));
    UpStreamMsgQ->msg = "First Message";
    UpStreamMsgQ->len = 13;
    UpStreamMsgQ->next = (UpStreamMsg *) malloc(sizeof(UpStreamMsg));
    UpStreamMsgQ->next->msg = "Second Message";
    UpStreamMsgQ->next->len = 15;
    UpStreamMsgQ->next->next = NULL;

    temp = (wrp_msg_t *) malloc(sizeof(wrp_msg_t));
    memset(temp,0,sizeof(wrp_msg_t));
    temp->msg_type = 4;

    will_return(wrp_to_struct, 12);
    expect_function_call(wrp_to_struct);

    will_return(validate_partner_id, 1);
    expect_function_call(validate_partner_id);

    will_return(appendEncodedData, 100);
    expect_function_call(appendEncodedData);

    expect_function_call(sendMessage);
    
    will_return(wrp_to_struct, 10);
    expect_function_call(wrp_to_struct);
    
    expect_function_call(filter_clients_and_send);
    expect_function_call(wrp_free_struct);
    expect_function_call(wrp_free_struct);

    processUpstreamMessage();
    free(temp);
    free(UpStreamMsgQ->next);
    free(UpStreamMsgQ);
}

void test_processUpstreamMessageInvalidPartner()
{
    numLoops = 1;
    metaPackSize = 20;
    UpStreamMsgQ = (UpStreamMsg *) malloc(sizeof(UpStreamMsg));
    UpStreamMsgQ->msg = "First Message";
    UpStreamMsgQ->len = 13;
    UpStreamMsgQ->next = (UpStreamMsg *) malloc(sizeof(UpStreamMsg));
    UpStreamMsgQ->next->msg = "Second Message";
    UpStreamMsgQ->next->len = 15;
    UpStreamMsgQ->next->next = NULL;

    temp = (wrp_msg_t *) malloc(sizeof(wrp_msg_t));
    memset(temp,0,sizeof(wrp_msg_t));
    temp->msg_type = 4;

    will_return(wrp_to_struct, 12);
    expect_function_call(wrp_to_struct);

    will_return(validate_partner_id, 0);
    expect_function_call(validate_partner_id);

    will_return(appendEncodedData, 100);
    expect_function_call(appendEncodedData);

    expect_function_call(sendMessage);
    will_return(wrp_to_struct, 10);
    expect_function_call(wrp_to_struct);
    
    expect_function_call(filter_clients_and_send);
    expect_function_call(wrp_free_struct);

    expect_function_call(wrp_free_struct);
    processUpstreamMessage();
    free(temp);
    free(UpStreamMsgQ->next);
    free(UpStreamMsgQ);
}

void test_processUpstreamMessageRegMsg()
{
    numLoops = 1;
    UpStreamMsgQ = (UpStreamMsg *) malloc(sizeof(UpStreamMsg));
    UpStreamMsgQ->msg = "First Message";
    UpStreamMsgQ->len = 13;
    UpStreamMsgQ->next = (UpStreamMsg *) malloc(sizeof(UpStreamMsg));
    UpStreamMsgQ->next->msg = "Second Message";
    UpStreamMsgQ->next->len = 15;
    UpStreamMsgQ->next->next = NULL;

    reg_list_item_t *head = (reg_list_item_t *) malloc(sizeof(reg_list_item_t));
    memset(head, 0, sizeof(reg_list_item_t));
    parStrncpy(head->service_name, "iot", sizeof(head->service_name));
    parStrncpy(head->url, "tcp://10.0.0.1:6600", sizeof(head->url));

    temp = (wrp_msg_t *) malloc(sizeof(wrp_msg_t));
    memset(temp,0,sizeof(wrp_msg_t));
    temp->msg_type = 9;
    temp->u.reg.service_name = head->service_name;
    temp->u.reg.url = head->url;

    will_return(wrp_to_struct, 12);
    expect_function_call(wrp_to_struct);

    will_return(get_numOfClients, 1);
    expect_function_call(get_numOfClients);

    will_return(get_global_node, (intptr_t)head);
    expect_function_call(get_global_node);

    will_return(nn_shutdown, 1);
    expect_function_call(nn_shutdown);

    will_return(nn_socket, 1);
    expect_function_call(nn_socket);

    will_return(nn_setsockopt, 1);
    expect_function_call(nn_setsockopt);

    will_return(nn_connect, 1);
    expect_function_call(nn_connect);

    will_return(sendAuthStatus, 0);
    expect_function_call(sendAuthStatus);

    will_return(get_numOfClients, 1);
    expect_function_call(get_numOfClients);

    expect_function_call(wrp_free_struct);

    processUpstreamMessage();
    free(temp);
    free(head);
    free(UpStreamMsgQ->next);
    free(UpStreamMsgQ);
}

void test_processUpstreamMessageRegMsgNoClients()
{
    numLoops = 1;
    UpStreamMsgQ = (UpStreamMsg *) malloc(sizeof(UpStreamMsg));
    UpStreamMsgQ->msg = "First Message";
    UpStreamMsgQ->len = 13;
    UpStreamMsgQ->next = (UpStreamMsg *) malloc(sizeof(UpStreamMsg));
    UpStreamMsgQ->next->msg = "Second Message";
    UpStreamMsgQ->next->len = 15;
    UpStreamMsgQ->next->next = NULL;

    reg_list_item_t *head = (reg_list_item_t *) malloc(sizeof(reg_list_item_t));
    memset(head, 0, sizeof(reg_list_item_t));
    parStrncpy(head->service_name, "iot", sizeof(head->service_name));
    parStrncpy(head->url, "tcp://10.0.0.1:6600", sizeof(head->url));

    temp = (wrp_msg_t *) malloc(sizeof(wrp_msg_t));
    memset(temp,0,sizeof(wrp_msg_t));
    temp->msg_type = 9;
    temp->u.reg.service_name = head->service_name;
    temp->u.reg.url = head->url;

    will_return(wrp_to_struct, 12);
    expect_function_call(wrp_to_struct);

    will_return(get_numOfClients, 0);
    expect_function_call(get_numOfClients);

    will_return(addToList, 0);
    expect_function_call(addToList);

    expect_function_call(wrp_free_struct);

    processUpstreamMessage();
    free(temp);
    free(head);
    free(UpStreamMsgQ->next);
    free(UpStreamMsgQ);
}

void err_processUpstreamMessage()
{
    numLoops = 1;
    UpStreamMsgQ = NULL;
    processUpstreamMessage();
}

void err_processUpstreamMessageDecodeErr()
{
    numLoops = 1;
    UpStreamMsgQ = (UpStreamMsg *) malloc(sizeof(UpStreamMsg));
    UpStreamMsgQ->msg = "First Message";
    UpStreamMsgQ->len = 13;
    UpStreamMsgQ->next = NULL;

    temp = (wrp_msg_t *) malloc(sizeof(wrp_msg_t));
    memset(temp,0,sizeof(wrp_msg_t));
    temp->msg_type = 3;

    will_return(wrp_to_struct, -1);
    expect_function_call(wrp_to_struct);
    expect_function_call(wrp_free_struct);
    processUpstreamMessage();
    free(temp);
    free(UpStreamMsgQ);
}

void err_processUpstreamMessageMetapackFailure()
{
    numLoops = 1;
    metaPackSize = 0;
    UpStreamMsgQ = (UpStreamMsg *) malloc(sizeof(UpStreamMsg));
    UpStreamMsgQ->msg = "First Message";
    UpStreamMsgQ->len = 13;
    UpStreamMsgQ->next = NULL;

    temp = (wrp_msg_t *) malloc(sizeof(wrp_msg_t));
    memset(temp,0,sizeof(wrp_msg_t));
    temp->msg_type = 5;

    will_return(wrp_to_struct, 15);
    expect_function_call(wrp_to_struct);

    expect_function_call(wrp_free_struct);
    processUpstreamMessage();
    free(temp);
    free(UpStreamMsgQ);
}

void err_processUpstreamMessageRegMsg()
{
    numLoops = 1;
    UpStreamMsgQ = (UpStreamMsg *) malloc(sizeof(UpStreamMsg));
    UpStreamMsgQ->msg = "First Message";
    UpStreamMsgQ->len = 13;
    UpStreamMsgQ->next = (UpStreamMsg *) malloc(sizeof(UpStreamMsg));
    UpStreamMsgQ->next->msg = "Second Message";
    UpStreamMsgQ->next->len = 15;
    UpStreamMsgQ->next->next = NULL;

    reg_list_item_t *head = (reg_list_item_t *) malloc(sizeof(reg_list_item_t));
    parStrncpy(head->service_name, "iot", sizeof(head->service_name));
    parStrncpy(head->url, "tcp://10.0.0.1:6600", sizeof(head->url));
    head->next = (reg_list_item_t *) malloc(sizeof(reg_list_item_t));
    parStrncpy(head->next->service_name, "iot", sizeof(head->service_name));
    parStrncpy(head->next->url, "tcp://10.0.0.1:6600", sizeof(head->url));
    head->next->next = NULL;

    temp = (wrp_msg_t *) malloc(sizeof(wrp_msg_t));
    memset(temp,0,sizeof(wrp_msg_t));
    temp->msg_type = 9;
    temp->u.reg.service_name = head->service_name;
    temp->u.reg.url = head->url;

    will_return(wrp_to_struct, 12);
    expect_function_call(wrp_to_struct);

    will_return(get_numOfClients, 1);
    expect_function_call(get_numOfClients);

    will_return(get_global_node, (intptr_t)head);
    expect_function_call(get_global_node);

    will_return(nn_shutdown, -1);
    expect_function_call(nn_shutdown);

    will_return(nn_socket, -1);
    expect_function_call(nn_socket);

    will_return(nn_shutdown, 1);
    expect_function_call(nn_shutdown);

    will_return(nn_socket, 1);
    expect_function_call(nn_socket);

    will_return(nn_setsockopt, -1);
    expect_function_call(nn_setsockopt);

    will_return(nn_connect, -1);
    expect_function_call(nn_connect);

    will_return(addToList, -1);
    expect_function_call(addToList);

    expect_function_call(wrp_free_struct);

    processUpstreamMessage();
    free(temp);
    free(head->next);
    free(head);
    free(UpStreamMsgQ->next);
    free(UpStreamMsgQ);
}

void test_sendUpstreamMsgToServer()
{
    void *bytes = NULL;
    wrp_msg_t msg;
    memset(&msg, 0, sizeof(wrp_msg_t));
    
    msg.msg_type = WRP_MSG_TYPE__EVENT;
    wrp_struct_to( &msg, WRP_BYTES, &bytes );
    metaPackSize = 10;
    
    will_return(appendEncodedData, 100);
    expect_function_call(appendEncodedData);
    expect_function_call(sendMessage);
    sendUpstreamMsgToServer(&bytes, 110);
    free(bytes);
}

void err_sendUpstreamMsgToServer()
{
    metaPackSize = 0;
    sendUpstreamMsgToServer(NULL, 110);
}

/*----------------------------------------------------------------------------*/
/*                             External Functions                             */
/*----------------------------------------------------------------------------*/

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_packMetaData),
        cmocka_unit_test(err_packMetaData),
        cmocka_unit_test(test_handleUpstreamNull),
        cmocka_unit_test(test_handle_upstream),
        cmocka_unit_test(err_handleUpstream),
        cmocka_unit_test(test_processUpstreamMessage),
        cmocka_unit_test(test_processUpstreamMessageInvalidPartner),
        cmocka_unit_test(test_processUpstreamMessageRegMsg),
        cmocka_unit_test(test_processUpstreamMessageRegMsgNoClients),
        cmocka_unit_test(err_processUpstreamMessage),
        cmocka_unit_test(err_processUpstreamMessageDecodeErr),
        cmocka_unit_test(err_processUpstreamMessageMetapackFailure),
        cmocka_unit_test(err_processUpstreamMessageRegMsg),
        cmocka_unit_test(test_sendUpstreamMsgToServer),
        cmocka_unit_test(err_sendUpstreamMsgToServer),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
