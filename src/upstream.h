/**
 * Copyright 2015 Comcast Cable Communications Management, LLC
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
/**
 * @file upstream.h
 *
 * @description This header defines functions required to manage upstream messages.
 *
 */
 
#ifndef _UPSTREAM_H_
#define _UPSTREAM_H_

#include <pthread.h>

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------*/
/*                               Data Structures                              */
/*----------------------------------------------------------------------------*/
typedef struct UpStreamMsg__
{
	void *msg;
	size_t len;
	struct UpStreamMsg__ *next;
} UpStreamMsg;

typedef struct _sub_list {
	char *service_name;
	char *regex;
} _sub_list_t;

/*----------------------------------------------------------------------------*/
/*                             Function Prototypes                            */
/*----------------------------------------------------------------------------*/

void packMetaData();
void handle_upstream(void *args);
void processUpstreamMessage();

void sendUpstreamMsgToServer(void **resp_bytes, size_t resp_size);
void sendToAllRegisteredClients(void **resp_bytes, size_t resp_size);
void set_global_UpStreamMsgQ(UpStreamMsg * UpStreamQ);
UpStreamMsg * get_global_UpStreamMsgQ(void);
pthread_cond_t *get_global_nano_con(void);
pthread_mutex_t *get_global_nano_mut(void);
int sendMsgtoRegisteredClients(char *dest,const char **Msg,size_t msgSize);

#ifdef __cplusplus
}
#endif


#endif /* _UPSTREAM_H_ */

