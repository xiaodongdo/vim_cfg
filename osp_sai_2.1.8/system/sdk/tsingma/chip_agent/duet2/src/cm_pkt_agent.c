/****************************************************************************
* $Id$
*  Packet agent file for support process based env
*
* (C) Copyright Centec Networks Inc.  All rights reserved.
*
* Modify History:
* Revision      : R0.01
* Author        : Alexander
* Date          : 2012-10-16 11:19
* Reason        : First Create.
****************************************************************************/

/****************************************************************************
 *
* Header Files
*
****************************************************************************/

/****************************************************************************
 *
* Defines and Macros
*
****************************************************************************/

/****************************************************************************
 *
* Global and Declaration
*
****************************************************************************/
static int g_bIsProcessBasedEnv = 0;

/****************************************************************************
 *
* Function
*
****************************************************************************/

int isProcessBaseEnv(void)
{
    return g_bIsProcessBasedEnv;
}

void set_process_based_env(int bEnable)
{
    g_bIsProcessBasedEnv = bEnable;
    return;
}
