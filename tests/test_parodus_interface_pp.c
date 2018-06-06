/**
 *  Copyright 2018 Comcast Cable Communications Management, LLC
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
#include <stdarg.h>

#include <stdbool.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>

#include <CUnit/Basic.h>

#include "../src/ParodusInternal.h"
#include "../src/parodus_interface.h"

/*----------------------------------------------------------------------------*/
/*                                   Macros                                   */
/*----------------------------------------------------------------------------*/
#define PIPELINE "tcp://127.0.0.1:8888"

/*----------------------------------------------------------------------------*/
/*                               Data Structures                              */
/*----------------------------------------------------------------------------*/
typedef struct {
    char   *d;
    char   *n;
    ssize_t nsz;
} test_t;

/*----------------------------------------------------------------------------*/
/*                             Function Prototypes                            */
/*----------------------------------------------------------------------------*/
static void *check_hub();

/*----------------------------------------------------------------------------*/
/*                            File Scoped Variables                           */
/*----------------------------------------------------------------------------*/
static test_t tests[] = {
    {   // 0 
        .d = PIPELINE,
        .n = "Some binary",
        .nsz = 11,
    },
};

/*----------------------------------------------------------------------------*/
/*                                   Mocks                                    */
/*----------------------------------------------------------------------------*/
/* None */

/*----------------------------------------------------------------------------*/
/*                                   Tests                                    */
/*----------------------------------------------------------------------------*/
void test_push_pull()
{
    pthread_t t;
    int pipeline_sock;
    bool result;

    pthread_create(&t, NULL, check_hub, NULL);

    spoke_setup_pipeline( PIPELINE, &pipeline_sock );
    for( ;; ) {
        result = send_msg(pipeline_sock, tests[0].n, tests[0].nsz);
        if( true == result ) {
            break;
        }
        sleep(1);
    }
    cleanup_sock(&pipeline_sock);
}

/*----------------------------------------------------------------------------*/
/*                             External Functions                             */
/*----------------------------------------------------------------------------*/
int main(void)
{
    unsigned rv = 1;
    CU_pSuite suite = NULL;

    if( CUE_SUCCESS == CU_initialize_registry() ) {
        printf("--------Start of Test Cases Execution ---------\n");
        suite = CU_add_suite( "tests", NULL, NULL );
        CU_add_test( suite, "Test Push/Pull", test_push_pull );

        if( NULL != suite ) {
            CU_basic_set_mode( CU_BRM_VERBOSE );
            CU_basic_run_tests();
            printf( "\n" );
            CU_basic_show_failures( CU_get_failure_list() );
            printf( "\n\n" );
            rv = CU_get_number_of_tests_failed();
        }

        CU_cleanup_registry();

    }

    return rv;
}

/*----------------------------------------------------------------------------*/
/*                             Internal Functions                             */
/*----------------------------------------------------------------------------*/
static void *check_hub()
{
    int pipeline_sock;
    char *msg = NULL;
    ssize_t msg_sz = 0;

    hub_setup_pipeline( PIPELINE, &pipeline_sock );
    for( ;; ) {
        msg_sz = check_inbox(pipeline_sock, (void **)&msg);
        if( 0 < msg_sz ) {
            printf("check hub - msg_sz = %zd\n", msg_sz);
            CU_ASSERT_EQUAL( (tests[0].nsz), msg_sz );
            CU_ASSERT_STRING_EQUAL( tests[0].n, msg );
            free_msg(msg);
            break;
        }
        sleep(1);
    }
    cleanup_sock(&pipeline_sock);
    return NULL;
}
