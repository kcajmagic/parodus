/**
 * Copyright 2018 Comcast Cable Communications Management, LLC
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
 * @file parodus_interface.h
 *
 * @description Declares parodus-to-parodus API.
 *
 */
 
#ifndef _PARODUS_INTERFACE_H_
#define _PARODUS_INTERFACE_H_

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------*/
/*                               Data Structures                              */
/*----------------------------------------------------------------------------*/
/* None */

/*----------------------------------------------------------------------------*/
/*                             Function Prototypes                            */
/*----------------------------------------------------------------------------*/
/**
 *  @note Call this with valid parameter before any spoke pubsub sock operation
 *
 *  @param pubsub_url    [in]  URL
 *  @param pubsub_sock   [out] sock
 *
 *  @return true if success, false if failure
 */
bool spoke_setup_pubsub(const char *pubsub_url, int *pubsub_sock);

/**
 *  @note Call this with valid parameter before any spoke pipeline sock operation
 *
 *  @param pipeline_url  [in]  URL
 *  @param pipeline_sock [out] sock
 *
 *  @return true if success, false if failure
 */
bool spoke_setup_pipeline(const char *pipeline_url, int *pipeline_sock);

/**
 *  @note Call this with valid parameter before any hub pipeline sock operation
 *
 *  @param pipeline_url  [in]  URL
 *  @param pipeline_sock [out] sock
 *
 *  @return true if success, false if failure
 */
bool hub_setup_pipeline(const char *pipeline_url, int *pipeline_sock);

/**
 *  @note Call this with valid parameter before any hub pubsub sock operation
 *
 *  @param pubsub_url    [in]  URL
 *  @param pubsub_sock   [out] sock
 *
 *  @return true if success, false if failure
 */
bool hub_setup_pubsub(const char *pubsub_url, int *pubsub_sock);

/**
 *  @note Call this after any setup function to cleanup
 *
 *  @param sock handle
 */
void cleanup_sock(int *sock);

/**
 *  Send message.
 *
 *  @param sock socket handle 
 *  @param msg  notification
 *  @param size size of notification
 *
 *  @return whether operation succeeded, or not.
 */
bool send_msg(int sock, const void *msg, size_t size);

/**
 *  Check for message.
 *
 *  @note msg needs to be cleaned up by the caller using free_msg() only.
 *
 *  @param sock   socket handle
 *  @param msg    address of message buffer
 *
 *  @return size of msg
 */
ssize_t check_inbox(int sock, void **msg);

/**
 *  Free message return by check_inbox().
 *
 *  @param msg    address of message buffer
 */
void free_msg(void *msg);

#ifdef __cplusplus
}
#endif

#endif /* _PARODUS_INTERFACE_H_ */

