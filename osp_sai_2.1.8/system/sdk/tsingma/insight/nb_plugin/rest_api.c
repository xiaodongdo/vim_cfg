/*****************************************************************************
  *
  * Copyright � 2016 Broadcom.  The term "Broadcom" refers
  * to Broadcom Limited and/or its subsidiaries.
  *
  * Licensed under the Apache License, Version 2.0 (the "License");
  * you may not use this file except in compliance with the License.
  *
  * You may obtain a copy of the License at
  * http://www.apache.org/licenses/LICENSE-2.0
  *
  * Unless required by applicable law or agreed to in writing, software
  * distributed under the License is distributed on an "AS IS" BASIS,
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ***************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <arpa/inet.h>


#include "rest.h"
#include "rest_http.h"
#include "rest_api.h"
#include "modulemgr.h"

static REST_CONTEXT_t rest;
const REST_JSON_ERROR_t rest_json_error_array[] = {
  {INSIGHT_STATUS_FAILURE, -32603, "Internal Error", rest_send_500_with_data},
  {INSIGHT_STATUS_OUTOFMEMORY, -32603, "Internal Error", rest_send_500_with_data},
  {INSIGHT_STATUS_RESOURCE_NOT_AVAILABLE, -32603, "Internal Error", rest_send_500_with_data},
  {INSIGHT_STATUS_INVALID_MEMORY, -32603, "Internal Error", rest_send_500_with_data},
  {INSIGHT_STATUS_INVALID_PARAMETER, -32602, "Invalid Params", rest_send_400_with_data},
  {INSIGHT_STATUS_INVALID_ID, -32602, "Invalid Params", rest_send_400_with_data},
  {INSIGHT_STATUS_UNSUPPORTED, -32601, "Method Not Found", rest_send_404_with_data},
  {INSIGHT_STATUS_INVALID_JSON, -32700, "Parse Error", rest_send_400_with_data},
  {INSIGHT_STATUS_FEATURE_NOT_ENABLED, -32001, "Feature not enabled", rest_send_403_with_data},
  {INSIGHT_STATUS_NOT_CONFIGURED, -32002, "Not configured", rest_send_403_with_data},
  {INSIGHT_STATUS_ALREADY_CONFIGURED, -32003, "Already configured", rest_send_403_with_data}
};
  const char json_error[] = "{     \
                             \"jsonrpc\": \"2.0\", \
                             \"error\": { \
                             \"code\": %d, \
                             \"message\": \"%s\" , \
                             \"version\": \"%d\" }, \
                             \"id\":  %d\
                              }";

const char json_error_async[] = "{     \
                                \"jsonrpc\": \"2.0\", \
                                \"error\": { \
                                \"code\": %d, \
                                \"message\": \"%s\" , \
                                \"version\": \"%d\" }, \
                                }";

#define REST_HTTP_JSON_MAX_ELEMENTS 7
#define REST_JSON_BUFF_LEN 512
#define REST_JSON_MSG_LEN 64

/******************************************************************
 * @brief  Initializes the REST component.
 *
 * @note     This function setsup the component and starts the webserver.
 *           It doesn't return.
 *********************************************************************/
INSIGHT_STATUS rest_init(void)
{
    INSIGHT_STATUS status;

    _REST_LOG(_REST_DEBUG_TRACE, "REST component initializing ..  \n");

    memset(&rest, 0, sizeof (rest));

    /* Read the configuration and configure the component */
    status = rest_config_init(&rest);
    _REST_ASSERT_ERROR( (status == INSIGHT_STATUS_SUCCESS), INSIGHT_STATUS_FAILURE);

    /* Initialize the session table */
    status = rest_sessions_init(&rest);
    _REST_ASSERT_ERROR( (status == INSIGHT_STATUS_SUCCESS), INSIGHT_STATUS_FAILURE);

    /* Initialize and Start the webserver */
    status = rest_http_server_run(&rest);
    _REST_ASSERT_ERROR( (status == INSIGHT_STATUS_SUCCESS), INSIGHT_STATUS_FAILURE);

    return status;
}
INSIGHT_STATUS rest_deinit(void)
{
  close(rest.listenFd);
  sal_printf("close rest!!\n");
  return 0;
}
/******************************************************************
 * @brief  Sends response to a client
 *
 * @note   The cookie is the 'session' and identifies the socket
 *         on which the data needs to be sent. This function adds
 *         HTTP header and sends it to client.
 *********************************************************************/
INSIGHT_STATUS rest_response_send(void *cookie, char *pBuf, int size)
{
    REST_SESSION_t *session = (REST_SESSION_t *) cookie;
    INSIGHT_STATUS status;

    /* if input is not valid, we still need to clean up session, if valid */
    if ((pBuf == NULL) && (session != NULL))
    {
        if (rest_session_validate(&rest, session) == INSIGHT_STATUS_SUCCESS)
        {
            close(session->connectionFd);
            session->inUse = false;
        }

        return INSIGHT_STATUS_INVALID_PARAMETER;
    }
    else if (NULL == pBuf)
    {
      return INSIGHT_STATUS_INVALID_PARAMETER;
    }

    /* session == NULL indicates an asynchronous send.
     * It needs to be handled differently.
     */
    if (session != NULL)
    {
        status = rest_session_validate(&rest, session);
        if (status == INSIGHT_STATUS_SUCCESS)
        {
            status = rest_send_200_with_data(session->connectionFd, pBuf, size);
        }

        close(session->connectionFd);
        session->inUse = false;
        return status;
    }

    /* asynchronous data sending */
    status = rest_send_async_report(&rest, pBuf, size);
    return status;
}


/******************************************************************
 * @brief  Sends successful response to a client
 *
 * @note   The cookie is the 'session' and identifies the socket
 *         on which the data needs to be sent and invokes the
 *         function to send the successful response.
 *********************************************************************/
INSIGHT_STATUS rest_response_send_ok (void *cookie)
{
  INSIGHT_STATUS ret;
  int fd = 0;
  REST_SESSION_t *session = (REST_SESSION_t *) cookie;


  if (NULL == cookie)
  {
      return INSIGHT_STATUS_INVALID_PARAMETER;
  }

  ret = rest_session_fd_get(cookie, &fd);
  if (INSIGHT_STATUS_SUCCESS != ret)
  {
      return ret;
  }

  ret = rest_send_200(fd);
  close(session->connectionFd);
  session->inUse = false;
  return ret;

}

/******************************************************************
 * @brief  Sends error response to a client
 *
 * @note   The cookie is the 'session' and identifies the socket
 *         on which the data needs to be sent. The error value which
 *         is returned in the value is used when the report is asyncronous
 *         and the return value and id are used for the syncronous responses.
 *********************************************************************/

INSIGHT_STATUS rest_response_send_error(void *cookie, INSIGHT_STATUS rv, int id)
{
  INSIGHT_STATUS ret;


  if (NULL == cookie)
  {
    /* Send the error reporting for the asyncronous
       reports */
    ret = rest_send_json_error_async(rv);
  }
  else
  {
    ret = rest_send_json_error(cookie, rv, id);
  }

  return ret;
}

/******************************************************************
 * @brief  Sends error response to a client
 *
 * @note   The cookie is the 'session' and identifies the socket
 *         on which the data needs to be sent.The cookie, returned error code
 *         and the id are used for the syncronous error responses.
 *********************************************************************/

INSIGHT_STATUS rest_send_json_error(void *cookie,  INSIGHT_STATUS rv, int id)
{
  INSIGHT_STATUS ret;
  int fd = 0;
  REST_SESSION_t *session = (REST_SESSION_t *) cookie;

  /* get the fd */

  if (NULL == cookie)
  {
      return INSIGHT_STATUS_INVALID_PARAMETER;
  }

  ret = rest_session_fd_get(cookie, &fd);
  if (INSIGHT_STATUS_SUCCESS != ret)
  {
      return ret;
  }

  /* call the api to prepare the json info and send */
    ret = rest_json_error_fn_invoke(fd, rv, id);
    close(session->connectionFd);
    session->inUse = false;
    return ret;
}


/******************************************************************
 * @brief  invokes the json error function for the given error code
 *
 * @note   The return value is used to retrieved the json error code,
 *         json error string and the rest api which is used to send
 *         the error respnse to the client.
 *********************************************************************/
INSIGHT_STATUS rest_json_error_fn_invoke(int fd, INSIGHT_STATUS rv, int id)
{
  INSIGHT_STATUS ret_json;
  char json[REST_JSON_BUFF_LEN];
  INSIGHT_REST_ERROR_HANDLER_t handler;
  int json_val =0;
  char str[REST_JSON_MSG_LEN];

  memset (str, 0, REST_JSON_MSG_LEN);



  ret_json = rest_get_json_error_data(rv, &json_val, str, &handler);

  if (INSIGHT_STATUS_SUCCESS != ret_json)
    return ret_json;

  memset (json, 0, REST_JSON_BUFF_LEN);
  snprintf(json, REST_JSON_BUFF_LEN, json_error, json_val, str, INSIGHT_JSON_VERSION, id);

  /* call the function to send the json error */

  ret_json = handler(fd, json, strlen(json));
  return ret_json;
}



/******************************************************************
 * @brief  invokes the json error function for the given error code
 *
 * @note   The return value is used to retrieved the json error code,
 *         json error string and the rest api which is used to send
 *         the error respnse to the client.This api is used for error
 *         reporting incase of failures in async report genetation.
 *********************************************************************/
INSIGHT_STATUS rest_send_json_error_async(INSIGHT_STATUS rv)
{
  INSIGHT_STATUS ret_json;
  char json[REST_JSON_BUFF_LEN];
  INSIGHT_REST_ERROR_HANDLER_t handler;
  int json_val =0;
  char str[REST_JSON_MSG_LEN];

  memset (str, 0, REST_JSON_MSG_LEN);



  ret_json = rest_get_json_error_data(rv, &json_val, str, &handler);

  if (INSIGHT_STATUS_SUCCESS != ret_json)
    return ret_json;

  memset (json, 0, REST_JSON_BUFF_LEN);
  snprintf(json, REST_JSON_BUFF_LEN, json_error_async, json_val, str, INSIGHT_JSON_VERSION);

  /* call the function to send the json error */
  ret_json = rest_send_async_report(&rest, json, strlen(json));
  return ret_json;
}

/******************************************************************
 * @brief  utility api to get the json error code, rest handler and
 *         json error string for the failure code.
 *
 * @note   The return value is used to retrieved the json error code,
 *         json error string and the rest api.
 *********************************************************************/
INSIGHT_STATUS rest_get_json_error_data(INSIGHT_STATUS rv, int *json_val,
                                      char *ptr, INSIGHT_REST_ERROR_HANDLER_t *handler)
{
  unsigned int i;

  for (i = 0; i < (sizeof(rest_json_error_array)/sizeof(rest_json_error_array[0])); i++)
  {
    if (rv == rest_json_error_array[i].return_code)
    {
      *json_val = rest_json_error_array[i].json_code;
      strncpy(ptr, rest_json_error_array[i].errorstr, strlen(rest_json_error_array[i].errorstr));
      *handler = rest_json_error_array[i].handler;
      return INSIGHT_STATUS_SUCCESS;
    }
  }

  return INSIGHT_STATUS_INVALID_PARAMETER;
}

/******************************************************************
 * @brief  utility api to get the fd for the given session.
 *
 * @note   The session id is used to retrieve the corresponding
 *          fd for the given session.
 *********************************************************************/
INSIGHT_STATUS rest_session_fd_get (void *cookie, int *fd)
{
    REST_SESSION_t *session = (REST_SESSION_t *) cookie;
    INSIGHT_STATUS status = INSIGHT_STATUS_INVALID_PARAMETER;


    /* session == NULL indicates an asynchronous send.
     * It needs to be handled differently.
     */
    if (session != NULL)
    {
        status = rest_session_validate(&rest, session);
        if (status == INSIGHT_STATUS_SUCCESS)
        {
          *fd = session->connectionFd;
           return status;
        }
    }

    return status;

}


/******************************************************************
 * @brief  Initializes configuration, reads it from file or assumes defaults.
 *
 * @param[in]   *ipaddr     pointer to client ip addr string
 * @param[in]   clientPort  client port
 *
 * @retval   0  when configuration is initialized successfully
 *
 * @note
 *********************************************************************/
int rest_agent_config_params_modify(char *ipaddr, unsigned int clientPort)
{
  REST_CONTEXT_t *ptr;

  ptr = &rest;

     REST_LOCK_TAKE(ptr);

     /* setup default client IP */
     strncpy(&ptr->config.clientIp[0], ipaddr, REST_MAX_IP_ADDR_LENGTH);

     /* setup default client port */
     ptr->config.clientPort = clientPort;

     REST_LOCK_GIVE(ptr);

     return 0;
}

