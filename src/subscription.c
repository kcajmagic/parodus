/**
 * @file subscription.c
 *
 * @description This describes functions required 
 * to manage parodus local client subscriptions.
 *
 * Copyright (c) 2018  Comcast
 */

#include "subscription.h"
#include "upstream.h"
/*----------------------------------------------------------------------------*/
/*                            File Scoped Variables                           */
/*----------------------------------------------------------------------------*/
rebar_ll_list_t *g_sub_list = NULL;
/*----------------------------------------------------------------------------*/
/*                             Function Prototypes                            */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/*                             External Functions                             */
/*----------------------------------------------------------------------------*/

rebar_ll_list_t *get_global_subscription_list(void)
{
    return g_sub_list;
}

void init_subscription_list()
{
    ParodusPrint("****** %s *******\n",__FUNCTION__);
    g_sub_list = (rebar_ll_list_t *) malloc(sizeof(rebar_ll_list_t));
    memset(g_sub_list, 0, sizeof(rebar_ll_list_t));
    rebar_ll_init( g_sub_list );
}

bool add_Client_Subscription(char *service_name, char *regex)
{
    ParodusPrint("****** %s *******\n",__FUNCTION__);
    if(service_name != NULL && regex != NULL)
    {
        ParodusPrint("service_name : %s regex : %s\n",service_name,regex);
        Subscription *sub = (Subscription *) malloc(sizeof(Subscription));
        sub->service_name = strdup(service_name);
        sub->regex = strdup(regex);
        rebar_ll_append( g_sub_list, &sub->sub_node );
        return true;
    }
    return false;
}

cJSON* get_Client_Subscriptions(char *service_name)
{
    rebar_ll_node_t *node = NULL;
    Subscription *sub = NULL;
    cJSON * jsonArray = NULL;
    int match_found = 0;
    
    ParodusPrint("****** %s *******\n",__FUNCTION__);
    if(service_name != NULL)
    {
        jsonArray = cJSON_CreateArray();
        node = rebar_ll_get_first( g_sub_list );
        while( NULL != node ) 
        {
            sub = rebar_ll_get_data(Subscription, sub_node, node);
            ParodusPrint("sub->service_name = %s sub->regex = %s\n",sub->service_name,sub->regex);
            if(strcmp(sub->service_name, service_name) == 0)
            {
                cJSON_AddItemToArray(jsonArray, cJSON_CreateString(sub->regex));
                match_found = 1;
            }
            node = node->next;
        }
    }
    if(match_found == 0)
    {
        jsonArray = NULL;
    }
    else
    {
        ParodusPrint("jsonArray = %s\n",cJSON_Print(jsonArray));
    }
    return jsonArray;
}

void filter_clients_and_send(wrp_msg_t *wrp_event_msg)
{
    rebar_ll_node_t *node = NULL;
    Subscription *sub = NULL;
    char *tempStr;
    void *bytes;

    ParodusPrint("****** %s *******\n",__FUNCTION__);
    if(wrp_event_msg != NULL)
    {
        node = rebar_ll_get_first( g_sub_list );
        while( NULL != node ) 
        {
            sub = rebar_ll_get_data(Subscription, sub_node, node);
            ParodusPrint("sub->service_name = %s sub->regex = %s\n",sub->service_name,sub->regex);
            if(wrp_event_msg->u.event.dest != NULL)
            {
                tempStr = strdup(wrp_event_msg->u.event.dest);
                if(strstr(tempStr, sub->regex) != NULL)
                {
                    ParodusInfo("%s registered for this event\n",sub->service_name);
                    int size = wrp_struct_to( wrp_event_msg, WRP_BYTES, &bytes );
                    sendMsgtoRegisteredClients(sub->service_name, (const char **)&bytes, size);
                }
                free(tempStr);
            }
            node = node->next;
        }
    }
}

bool delete_client_subscriptions(char *service_name)
{
    rebar_ll_node_t *node = NULL;
    Subscription *sub = NULL;
    bool match_found = false;
    ParodusPrint("****** %s *******\n",__FUNCTION__);
    node = rebar_ll_get_first( g_sub_list );
    while( NULL != node ) 
    {
        sub = rebar_ll_get_data(Subscription, sub_node, node);
        ParodusPrint("sub->service_name = %s sub->regex = %s\n",sub->service_name,sub->regex);
        if(strcmp(sub->service_name, service_name) == 0)
        {
            rebar_ll_remove( g_sub_list, node );
            match_found = true;
        }
        node = node->next;
    }
    return match_found;
}
