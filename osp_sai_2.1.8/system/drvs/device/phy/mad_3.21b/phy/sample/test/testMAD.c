#include <madCopyright.h>
/********************************************************************************
* testMAD.c
*
* DESCRIPTION:
*       API test functions
*
* DEPENDENCIES:   Platform.
*
* FILE REVISION NUMBER:
*
*******************************************************************************/
#include "madSample.h"

/*
#define TEST_DEBUG
*/

extern void testMADDisplayStatus(MAD_STATUS status);

static void madWait (int waitTime)
{
	volatile int count=waitTime*100000;
    while (count--);
}

MAD_U32 testPTP(MAD_DEV *dev);


MAD_U32 testAutoNeg(MAD_DEV *dev)
{
    MAD_STATUS  retVal;
    MAD_LPORT   port;
    MAD_BOOL    en;
    int         testCase, nCases;
    MAD_U32     mode, testMode[11];

    MSG_PRINT(("Testing Forced Mode with various mode...\n"));

    nCases = 4;
    testMode[0] = MAD_PHY_100FDX;
    testMode[1] = MAD_PHY_100HDX;
    testMode[2] = MAD_PHY_10FDX;
    testMode[3] = MAD_PHY_10HDX;

    for(port=0; port<dev->numOfPorts; port++)
    {
        for(testCase=0; testCase<nCases; testCase++)
        {   
            retVal = mdCopperSetAutoNeg(dev,port,MAD_FALSE,testMode[testCase]);
            if (retVal != MAD_OK)
            {
		     	if((dev->deviceId==MAD_SW1680)&&(retVal==MAD_API_ERROR_OVER_PORT_RANGE)) break;
                MSG_PRINT(("Setting for (port %i, test case %i) returned ",(int)port,testCase));
                testMADDisplayStatus(retVal);
                return retVal;             
            }
    
            retVal = mdCopperGetAutoNeg(dev,port,&en,&mode);
            if (retVal != MAD_OK)
            {
		     	if((dev->deviceId==MAD_SW1680)&&(retVal==MAD_API_ERROR_OVER_PORT_RANGE)) break;
                MSG_PRINT(("Getting for (port %i, test case %i) returned ",(int)port,testCase));
                testMADDisplayStatus(retVal);
                return retVal;             
            }
            
            if (en != MAD_FALSE)
            {
                MSG_PRINT(("Unexpected return (port %i, test case %i, en %i)\n",(int)port,testCase,en));
                return MAD_FAIL;
            }
            
            if (mode != testMode[testCase])
            {
                MSG_PRINT(("Unexpected return (port %i, test case %i, mode %i:%i)\n",
                            (int)port,testCase,(int)mode,(int)testMode[testCase]));
                return MAD_FAIL;
            }

        }
    }

    MSG_PRINT(("Passed.\n\n"));

    MSG_PRINT(("Testing SetAutoNeg with various mode...\n"));


  if(dev->phyInfo.featureSet & MAD_PHY_30XX_MODE)
  { /* E3016/E3082 has no 1000M */
    nCases = 9;
    testMode[0] = MAD_AUTO_AD_ASYM_PAUSE |
               MAD_AUTO_AD_PAUSE |
               MAD_AUTO_AD_100FDX |
               MAD_AUTO_AD_100HDX |
               MAD_AUTO_AD_10FDX |
               MAD_AUTO_AD_10HDX;

    testMode[1] = MAD_AUTO_AD_ASYM_PAUSE |
               MAD_AUTO_AD_PAUSE |
               MAD_AUTO_AD_100FDX |
               MAD_AUTO_AD_100HDX |
               MAD_AUTO_AD_10FDX |
               MAD_AUTO_AD_10HDX;

    testMode[2] = MAD_AUTO_AD_ASYM_PAUSE |
               MAD_AUTO_AD_PAUSE |
               MAD_AUTO_AD_100FDX |
               MAD_AUTO_AD_10FDX;

    testMode[3] = 
               MAD_AUTO_AD_100FDX |
               MAD_AUTO_AD_100HDX |
               MAD_AUTO_AD_10FDX |
               MAD_AUTO_AD_10HDX;

    testMode[4] = 
               MAD_AUTO_AD_100HDX |
               MAD_AUTO_AD_10HDX;

    testMode[5] = MAD_AUTO_AD_100FDX;
    testMode[6] = MAD_AUTO_AD_100HDX;
    testMode[7] = MAD_AUTO_AD_10FDX;

    testMode[8] = MAD_AUTO_AD_ASYM_PAUSE |
               MAD_AUTO_AD_PAUSE |
               MAD_AUTO_AD_100FDX |
               MAD_AUTO_AD_100HDX |
               MAD_AUTO_AD_10FDX |
               MAD_AUTO_AD_10HDX;
  }
  else      
  {
    nCases = 11;
    testMode[0] = MAD_AUTO_AD_ASYM_PAUSE |
               MAD_AUTO_AD_PAUSE |
               MAD_AUTO_AD_100FDX |
               MAD_AUTO_AD_100HDX |
               MAD_AUTO_AD_10FDX |
               MAD_AUTO_AD_10HDX |
               MAD_AUTO_AD_1000FDX |
               MAD_AUTO_AD_1000HDX |
               MAD_AUTO_AD_MANUAL_CONF_MS |
               MAD_AUTO_AD_FORCE_MASTER |
               MAD_AUTO_AD_PREFER_MULTI_PORT;

    testMode[1] = MAD_AUTO_AD_ASYM_PAUSE |
               MAD_AUTO_AD_PAUSE |
               MAD_AUTO_AD_100FDX |
               MAD_AUTO_AD_100HDX |
               MAD_AUTO_AD_10FDX |
               MAD_AUTO_AD_10HDX |
               MAD_AUTO_AD_1000FDX |
               MAD_AUTO_AD_1000HDX;

    testMode[2] = MAD_AUTO_AD_ASYM_PAUSE |
               MAD_AUTO_AD_PAUSE |
               MAD_AUTO_AD_100FDX |
               MAD_AUTO_AD_10FDX |
               MAD_AUTO_AD_1000FDX;

    testMode[3] = 
               MAD_AUTO_AD_100FDX |
               MAD_AUTO_AD_100HDX |
               MAD_AUTO_AD_10FDX |
               MAD_AUTO_AD_10HDX |
               MAD_AUTO_AD_1000FDX |
               MAD_AUTO_AD_1000HDX;

    testMode[4] = 
               MAD_AUTO_AD_100HDX |
               MAD_AUTO_AD_10HDX |
               MAD_AUTO_AD_1000HDX;

    testMode[5] = MAD_AUTO_AD_100FDX;
    testMode[6] = MAD_AUTO_AD_1000FDX;
    testMode[7] = MAD_AUTO_AD_100HDX;
    testMode[8] = MAD_AUTO_AD_1000HDX;
    testMode[9] = MAD_AUTO_AD_10FDX;

    testMode[10] = MAD_AUTO_AD_ASYM_PAUSE |
               MAD_AUTO_AD_PAUSE |
               MAD_AUTO_AD_100FDX |
               MAD_AUTO_AD_100HDX |
               MAD_AUTO_AD_10FDX |
               MAD_AUTO_AD_10HDX |
               MAD_AUTO_AD_1000FDX |
               MAD_AUTO_AD_1000HDX |
               MAD_AUTO_AD_MANUAL_CONF_MS |
               MAD_AUTO_AD_FORCE_MASTER |
               MAD_AUTO_AD_PREFER_MULTI_PORT;
  }

    for(port=0; port<dev->numOfPorts; port++)
    {
        for(testCase=0; testCase<nCases; testCase++)
        {   
            retVal = mdCopperSetAutoNeg(dev,port,MAD_TRUE,testMode[testCase]);
            if (retVal != MAD_OK)
            {
		     	if((dev->deviceId==MAD_SW1680)&&(retVal==MAD_API_ERROR_OVER_PORT_RANGE)) break;
                MSG_PRINT(("Setting for (port %i, test case %i) returned ",(int)port,testCase));
                testMADDisplayStatus(retVal);
                return retVal;             
            }
    
            retVal = mdCopperGetAutoNeg(dev,port,&en,&mode);
            if (retVal != MAD_OK)
            {
		     	if((dev->deviceId==MAD_SW1680)&&(retVal==MAD_API_ERROR_OVER_PORT_RANGE)) break;
                MSG_PRINT(("Getting for (port %i, test case %i) returned ",(int)port,testCase));
                testMADDisplayStatus(retVal);
                return retVal;             
            }
            
            if (en != MAD_TRUE)
            {
                MSG_PRINT(("Unexpected return (port %i, test case %i, en %i)\n",(int)port,testCase,en));
               return MAD_FAIL;
            }
            
            if (mode != testMode[testCase])
            {
                MSG_PRINT(("Unexpected return (port %i, test case %i, mode %x:%x)\n",
                            (int)port,testCase,(unsigned int)mode,(unsigned int)testMode[testCase]));
                return MAD_FAIL;
            }

        }
    }

    MSG_PRINT(("Passed.\n"));
    return MAD_OK;
}   


MAD_U32 testMDIMode(MAD_DEV *dev)
{
    MAD_STATUS  retVal;
    MAD_LPORT   port;
    int         testCase, nCases;
    MAD_MDI_MODE     mode, testMode[10];

    MSG_PRINT(("Testing SetMDIMode with various mode...\n"));

    nCases = 3;
    testMode[0] = MAD_AUTO_MDI_MDIX;
    testMode[1] = MAD_FORCE_MDI;
    testMode[2] = MAD_FORCE_MDIX;

    for(port=0; port<dev->numOfPorts; port++)
    {
        for(testCase=0; testCase<nCases; testCase++)
        {   
            retVal = mdCopperSetMDIMode(dev,port,testMode[testCase]);
            if (retVal != MAD_OK)
            {
		     	if((dev->deviceId==MAD_SW1680)&&(retVal==MAD_API_ERROR_OVER_PORT_RANGE)) break;
                MSG_PRINT(("Setting for (port %i, test case %i) returned ",(int)port,testCase));
                testMADDisplayStatus(retVal);
                return retVal;             
            }
    
            retVal = mdCopperGetMDIMode(dev,port,&mode);
            if (retVal != MAD_OK)
            {
		     	if((dev->deviceId==MAD_SW1680)&&(retVal==MAD_API_ERROR_OVER_PORT_RANGE)) break;
                MSG_PRINT(("Getting for (port %i, test case %i) returned ",(int)port,testCase));
                testMADDisplayStatus(retVal);
                return retVal;             
            }
            
            if (mode != testMode[testCase])
            {
                MSG_PRINT(("Unexpected return (port %i, test case %i, mode %i:%i)\n",
                            (int)port,testCase,(int)mode,(int)testMode[testCase]));
                return MAD_FAIL;
            }

        }
    }

    MSG_PRINT(("Passed.\n\n"));
    return MAD_OK;
}



MAD_U32 testDownshift(MAD_DEV *dev)
{
    MAD_STATUS  retVal;
    MAD_LPORT   port;
    MAD_U32     testCase, nCases;
    MAD_BOOL    mode,testMode[10];
    MAD_16        count;
#define TestCount            3

    MSG_PRINT(("Testing Downshift Enable...\n"));

    nCases = 2;
    testMode[0] = MAD_TRUE;
    testMode[1] = MAD_FALSE;
    count = TestCount;;

    for(port=0; port<dev->numOfPorts; port++)
    {
        for(testCase=0; testCase<nCases; testCase++)
        {   
            retVal = mdCopperSetDownshiftEnable(dev,port,testMode[testCase], count);
            if (retVal != MAD_OK)
            {
		     	if((dev->deviceId==MAD_SW1680)&&(retVal==MAD_API_ERROR_OVER_PORT_RANGE)) break;
                MSG_PRINT(("Setting for (port %i, test case %i) returned ",(int)port,(int)testCase));
                testMADDisplayStatus(retVal);
                return retVal;             
            }
    
            retVal = mdCopperGetDownshiftEnable(dev,port,&mode, &count);
            if (retVal != MAD_OK)
            {
		     	if((dev->deviceId==MAD_SW1680)&&(retVal==MAD_API_ERROR_OVER_PORT_RANGE)) break;
                MSG_PRINT(("Getting for (port %i, test case %i) returned ",(int)port,(int)testCase));
                testMADDisplayStatus(retVal);
                return retVal;             
            }
            
            if ((mode != testMode[testCase]) || (count != TestCount))
            {
                MSG_PRINT(("Unexpected return (port %i, test case %i, mode %i:%i, count %i)\n",
                            (int)port,(int)testCase,(int)mode,(int)testMode[testCase], count));
                return MAD_FAIL;
            }

        }
    }

    MSG_PRINT(("Passed.\n\n"));
    return MAD_OK;
}


MAD_U32 testDTEDetect(MAD_DEV *dev)
{
    MAD_STATUS  retVal;
    MAD_LPORT   port;
    int         testCase, nCases;
    MAD_BOOL    mode,testMode[10];
    MAD_U16        dropHys;
#define TestDropHys        7

    MSG_PRINT(("Testing DTE Detect Enable...\n"));

    nCases = 2;
    testMode[0] = MAD_TRUE;
    testMode[1] = MAD_FALSE;
    dropHys = TestDropHys;

    for(port=0; port<dev->numOfPorts; port++)
    {
        for(testCase=0; testCase<nCases; testCase++)
        {   
            retVal = mdCopperSetDTEDetectEnable(dev,port,testMode[testCase], dropHys);
            if (retVal != MAD_OK)
            {
		     	if((dev->deviceId==MAD_SW1680)&&(retVal==MAD_API_ERROR_OVER_PORT_RANGE)) break;
                MSG_PRINT(("Setting for (port %i, test case %i) returned ",(int)port,testCase));
                testMADDisplayStatus(retVal);
                return retVal;             
            }
    
            retVal = mdCopperGetDTEDetectEnable(dev,port,&mode, &dropHys);
            if (retVal != MAD_OK)
            {
		     	if((dev->deviceId==MAD_SW1680)&&(retVal==MAD_API_ERROR_OVER_PORT_RANGE)) break;
                MSG_PRINT(("Getting for (port %i, test case %i) returned ",(int)port,testCase));
                testMADDisplayStatus(retVal);
                return retVal;             
            }
            
            if ((mode != testMode[testCase]) || (dropHys != TestDropHys))
            {
                MSG_PRINT(("Unexpected return (port %i, test case %i, mode %i:%i dropHys %i)\n",
                            (int)port,testCase,(int)mode,(int)testMode[testCase], dropHys));
                return MAD_FAIL;
            }

        }
    }

    MSG_PRINT(("Passed.\n\n"));
    return MAD_OK;
}


/* 

MAD_STATUS testLoopback(MAD_DEV *dev)
{
	MAD_STATUS retVal;
	MAD_LPORT port;
	MAD_SPEED_MODE speed;
	int option;
	MAD_BOOL en;
	MSG_PRINT(("Testing MAC loopback.\n"));
	printf("Enter port number:");
	scanf("%d", &port);
	printf("\nEnable loopback-0, Disable loopback-1:");
	scanf("%d", &option);
	if (option)
		en = MAD_FALSE;
	else
		en = MAD_TRUE;
	printf("\nSpeed: 1000M-0, 100M-1, 10M-2:");
	scanf("%d", &option);
	if (option == 0)
		speed = MAD_SPEED_1000M;
	else if(option == 1) {
		speed = MAD_SPEED_100M;
		madHwSetPagedPhyRegField(dev, port, 0, 0, 6, 1, 0);
		madHwSetPagedPhyRegField(dev, port, 0, 0, 13, 1, 1);
		madHwSetPagedPhyRegField(dev, port, 0, 0, 15, 1, 1);
	}
	else {
		speed = MAD_SPEED_10M;
		madHwSetPagedPhyRegField(dev, port, 0, 0, 6, 1, 0);
		madHwSetPagedPhyRegField(dev, port, 0, 0, 13, 1, 0);
		madHwSetPagedPhyRegField(dev, port, 0, 0, 15, 1, 1);
	}
	retVal = mdDiagSetMACIfLoopback(dev, port, en, MAD_MAC_LOOPBACK_MODE_SYS, speed);
	if (retVal != MAD_OK) {
		MSG_PRINT(("Testing MAC loopback failed.\n"));
		return retVal;
	}
	MSG_PRINT(("Testing MAC loopback succeed.\n"));
	return MAD_OK;
}

*/

MAD_STATUS testMADCtrl(MAD_DEV *dev)
{
	/*
	testLoopback(dev);
	*/	
    testAutoNeg(dev);  
    testMDIMode(dev); 
    testDownshift(dev);
    testDTEDetect(dev);
    return MAD_OK;
}

#define MAX_SAVED_ERROR    32
static int savedResult [MAX_SAVED_ERROR];
static char savedResultHead [MAX_SAVED_ERROR] [64];

MAD_STATUS testMADStatus(MAD_DEV *dev)
{
    MAD_STATUS  retVal;
    MAD_LPORT   port;
    MAD_BOOL    onOff;
    MAD_U32     data;
    MAD_32        i, count=0;

    MSG_PRINT(("\tLink\tSpeed\tDpx\tShift\tDTE\n"));

    for ( i=0; i<MAX_SAVED_ERROR; i++)
        savedResult [i]=0;


    for(port=0; port<dev->numOfPorts; port++)
    {
        MSG_PRINT(("port%1i\t",(int)port));
        retVal = mdGetLinkStatus(dev,port,&onOff);
        if (retVal != MAD_OK)
        {
	     	if((dev->deviceId==MAD_SW1680)&&(retVal==MAD_API_ERROR_OVER_PORT_RANGE)) break;
            MSG_PRINT(("%x\t",(int)retVal));  
            sprintf(savedResultHead[count], "GetLinkStatus Failed %x: ", retVal);
            savedResult[count++]=retVal;
        }
        else
        {
            MSG_PRINT(("%i\t",(int)onOff);
        }

        retVal = mdGetSpeedStatus(dev,port,(MAD_SPEED_MODE *)&data);
        if (retVal != MAD_OK)
        {
	     	if((dev->deviceId==MAD_SW1680)&&(retVal==MAD_API_ERROR_OVER_PORT_RANGE)) break;
            MSG_PRINT(("%x\t",(int)retVal)); 
            sprintf(savedResultHead[count], "GetSpeedStatus Failed %x: ", retVal));
            savedResult[count++]=retVal;
        }
        else
        {
            MSG_PRINT(("%i\t",(int)data);
        }
    
        retVal = mdGetDuplexStatus(dev,port,(MAD_DUPLEX_MODE *)&data);
        if (retVal != MAD_OK)
        {
	     	if((dev->deviceId==MAD_SW1680)&&(retVal==MAD_API_ERROR_OVER_PORT_RANGE)) break;
            MSG_PRINT(("%x\t",(int)retVal)); 
            sprintf(savedResultHead[count], "GetDuplexStatus Failed %x: ", retVal));
            savedResult[count++]=retVal;
        }
        else
        {
            MSG_PRINT(("%i\t",(int)data);
        }
    
        retVal = mdCopperGetDownshiftStatus(dev,port,&onOff); 
        if (retVal != MAD_OK)
        {
	     	if((dev->deviceId==MAD_SW1680)&&(retVal==MAD_API_ERROR_OVER_PORT_RANGE)) break;
            MSG_PRINT(("%x\t",(int)retVal)); 
            sprintf(savedResultHead[count], "GetDownShiftStatus Failed %x: ", retVal));
            savedResult[count++]=retVal;
        }
        else
        {
            MSG_PRINT(("%i\t",(int)onOff);
        }
    
        retVal = mdCopperGetDTEDetectStatus(dev,port,&onOff);  
        if (retVal != MAD_OK)
        {
	     	if((dev->deviceId==MAD_SW1680)&&(retVal==MAD_API_ERROR_OVER_PORT_RANGE)) break;
            MSG_PRINT(("%x\t",(int)retVal)); 
            sprintf(savedResultHead[count], "GetDTEDetectStatus Failed %x: ", retVal));
            savedResult[count++]=retVal;
        }
        else
        {
            MSG_PRINT(("%i\t",(int)onOff));
        }
        MSG_PRINT(("\n"));
   
    }

    if (count)
#ifdef SHOW_DEBUG_INFO
      MSG_PRINT_DBG (("\n\nReturned error: \n"));
#else
      MSG_PRINT (("\n\n$$$ Show all returned error:\n"));
#endif

    for (i=0; i<count; i++)
    {
      MSG_PRINT (("%s", savedResultHead[i]));
#ifdef SHOW_DEBUG_INFO
      testMADDisplayStatus(savedResult[i]);
#endif
    }

    return MAD_OK;
}

MAD_STATUS startPktCRC(MAD_DEV *dev, MAD_LPORT port)
{
    MAD_STATUS retVal;
    MAD_PG     pktInfo;
    MAD_U32    counter;

    MSG_PRINT(("Enabling CRC Counter and start generating Pkts\n"));

    pktInfo.payload = 0;
    pktInfo.length = 0;
    pktInfo.tx = 0;
    pktInfo.en_type = 0;

    retVal = mdStatsSetCRCCheckerEnable(dev,port,0, MAD_TRUE);
    if (retVal != MAD_OK)
    {
        MSG_PRINT(("enableCRCChecker return "));
        testMADDisplayStatus(retVal);
        return retVal;             
    }
    
    retVal = mdDiagSetPktGenEnable(dev,port,1,&pktInfo);
    if (retVal != MAD_OK)
    {
        MSG_PRINT(("mdDiagSetPktGenEnable return "));
        testMADDisplayStatus(retVal);
        return retVal;             
    }
    
    MSG_PRINT(("Press any key to stop Pkt Generator and read CRC Counter\n"));

    /* wait for seconds... */
/*    gets(str);  get an entire string from the keyboard. */
    
    /*
     *    Read CRC Error Counter
    */
    retVal = mdStatsGetCRCErrorCounter(dev,port,&counter);
    if (retVal != MAD_OK)
    {
        MSG_PRINT(("mdStatsGetCRCErrorCounter return "));
        testMADDisplayStatus(retVal);
        return retVal;             
    }

    MSG_PRINT(("CRC Error Counter: %i\n",(int)counter));

    /*
     *    Read Frame Counter
    */
    retVal = mdStatsGetFrameCounter(dev,port,&counter);
    if (retVal != MAD_OK)
    {
        MSG_PRINT(("mdStatsGetFrameCounter return "));
        testMADDisplayStatus(retVal);
        return retVal;             
    }

    MSG_PRINT(("Frame Counter:     %i\n\n",(int)counter));

    /*
     *  Stop Pkt Generator
    */
    retVal = mdDiagSetPktGenEnable(dev,port,0,NULL);
    if (retVal != MAD_OK)
    {
        MSG_PRINT(("mdDiagSetPktGenEnable return "));
        testMADDisplayStatus(retVal);
        return retVal;             
    }
    
    /*
     *  Disable CRC Counter
    */
    retVal = mdStatsSetCRCCheckerEnable(dev,port,0,MAD_FALSE);
    if (retVal != MAD_OK)
    {
        MSG_PRINT(("enableCRCChecker return "));
        testMADDisplayStatus(retVal);
        return retVal;             
    }

    MSG_PRINT(("Stopped generating Pkts and Disabled CRC Counter\n"));
    return MAD_OK;
}


MAD_U32 testCheckCounterEnable(MAD_DEV *dev)
{
    MAD_STATUS  retVal;
    MAD_LPORT   port;
    int         testCase, nCases;
    MAD_U8        mode,testMode[6];
    MAD_BOOL    en, enSet;

    MSG_PRINT(("Testing CRC Counter Enable Enable...\n"));

  if((dev->deviceId==MAD_88E1340)||(dev->deviceId==MAD_88E1340M)
    ||(dev->deviceId==MAD_88E1340S)||(dev->deviceId==MAD_88E1540)
	||(dev->deviceId==MAD_88E1545)||(dev->deviceId==MAD_88E1548)
	||(dev->deviceId==MAD_88E1680)||(dev->deviceId==MAD_88E1680M)
	||(dev->deviceId==MAD_SW1680)||(dev->deviceId==MAD_88E151x))
  {
    nCases = 5;
    testMode[0] = MAD_CHECKER_CTRL_COPPER;
    testMode[1] = MAD_CHECKER_CTRL_SGMII;
    testMode[2]= MAD_CHECKER_CTRL_MACSEC_TR;
    testMode[3]= MAD_CHECKER_CTRL_QSGMII;
    testMode[4] = MAD_CHECKER_CTRL_MACSEC_REC;
    testMode[5] = MAD_CHECKER_CTRL_DISABLE;    /* not test */
  }
  else
  {
    nCases = 2;
    testMode[0] = 0;
    testMode[1] = 1;
  }

  enSet = MAD_TRUE;

    for(port=0; port<dev->numOfPorts; port++)
    {
        for(testCase=0; testCase<nCases; testCase++)
        {   
            retVal = mdStatsSetCRCCheckerEnable(dev,port,testMode[testCase], enSet);
            if (retVal != MAD_OK)
            {
		     	if((dev->deviceId==MAD_SW1680)&&(retVal==MAD_API_ERROR_OVER_PORT_RANGE)) break;
                MSG_PRINT(("Setting for (port %i, test case %i) returned ",(int)port,testCase));
                testMADDisplayStatus(retVal);
                return retVal;             
            }
    
            retVal = mdStatsGetCRCCheckerEnable(dev,port,&mode, &en);
            if (retVal != MAD_OK)
            {
		     	if((dev->deviceId==MAD_SW1680)&&(retVal==MAD_API_ERROR_OVER_PORT_RANGE)) break;
                MSG_PRINT(("Getting for (port %i, test case %i) returned ",(int)port,testCase));
                testMADDisplayStatus(retVal);
                return retVal;             
            }
            
            if ((mode != testMode[testCase]) || (en != enSet))
            {
                MSG_PRINT(("Unexpected return (port %i, test case %i, mode %x:%x ebable %i:%i)\n",
                            (int)port,testCase,(int)mode,(int)testMode[testCase], enSet, en));
                return MAD_FAIL;
            }

        }
    }

    MSG_PRINT(("Passed.\n\n"));
    return MAD_OK;
}

MAD_U32 testTempInt(MAD_DEV *dev)
{
    MAD_STATUS  retVal;
    MAD_LPORT   port;
    MAD_U32     testCase, nCases;
    MAD_BOOL    mode,testMode[4];

    MSG_PRINT(("Testing Temperature sensor interrupt Enable...\n"));

    nCases = 2;
    testMode[0] = MAD_TRUE;
    testMode[1] = MAD_FALSE;

    for(port=0; port<dev->numOfPorts; port++)
    {
        for(testCase=0; testCase<nCases; testCase++)
        {   
            retVal = mdTempIntSetEnable(dev,port,testMode[testCase]);
            if (retVal != MAD_OK)
            {
		     	if((dev->deviceId==MAD_SW1680)&&(retVal==MAD_API_ERROR_OVER_PORT_RANGE)) break;
                MSG_PRINT(("Setting for (port %i, test case %i) returned ",(int)port,(int)testCase));
                testMADDisplayStatus(retVal);
                return retVal;             
            }
    
            retVal = mdTempIntGetEnable(dev,port,&mode);
            if (retVal != MAD_OK)
            {
		     	if((dev->deviceId==MAD_SW1680)&&(retVal==MAD_API_ERROR_OVER_PORT_RANGE)) break;
                MSG_PRINT(("Getting for (port %i, test case %i) returned ",(int)port,(int)testCase));
                testMADDisplayStatus(retVal);
                return retVal;             
            }
            
            if (mode != testMode[testCase])
            {
                MSG_PRINT(("Unexpected return (port %i, test case %i, mode %i:%i)\n",
                            (int)port,(int)testCase,(int)mode,(int)testMode[testCase]));
                return MAD_FAIL;
            }

        }
    }

    MSG_PRINT(("Passed.\n\n"));
    return MAD_OK;
}

MAD_U32 testQSGMIIPwD(MAD_DEV *dev)
{
    MAD_STATUS  retVal;
    MAD_LPORT   port;
    MAD_U32     testCase, nCases;
    MAD_BOOL    mode,testMode[4];

    MSG_PRINT(("Testing System QSGMII Power down Enable...\n"));

    nCases = 2;
    testMode[0] = MAD_TRUE;
    testMode[1] = MAD_FALSE;

    for(port=0; port<dev->numOfPorts; port++)
    {
        for(testCase=0; testCase<nCases; testCase++)
        {   
            retVal = mdSysSetQSGMIIPwDown(dev,port,testMode[testCase]);
            if (retVal != MAD_OK)
            {
		     	if((dev->deviceId==MAD_SW1680)&&(retVal==MAD_API_ERROR_OVER_PORT_RANGE)) break;
                MSG_PRINT(("Setting for (port %i, test case %i) returned ",(int)port,(int)testCase));
                testMADDisplayStatus(retVal);
                return retVal;             
            }
    
            retVal = mdSysGetQSGMIIPwDown(dev,port,&mode);
            if (retVal != MAD_OK)
            {
		     	if((dev->deviceId==MAD_SW1680)&&(retVal==MAD_API_ERROR_OVER_PORT_RANGE)) break;
                MSG_PRINT(("Getting for (port %i, test case %i) returned ",(int)port,(int)testCase));
                testMADDisplayStatus(retVal);
                return retVal;             
            }
            
            if (mode != testMode[testCase])
            {
                MSG_PRINT(("Unexpected return (port %i, test case %i, mode %i:%i)\n",
                            (int)port,(int)testCase,(int)mode,(int)testMode[testCase]));
                return MAD_FAIL;
            }

        }
    }

    MSG_PRINT(("Passed.\n\n"));
    return MAD_OK;
}

MAD_U32 testSnoop(MAD_DEV *dev)
{
    MAD_STATUS  retVal;
    MAD_LPORT   port;
    MAD_U32     testCase, nCases;
    MAD_U16    mode,testMode[4];

    MSG_PRINT(("Testing System Snooping control...\n"));

    nCases = 3;
    testMode[0] = MAD_SNOOPING_OFF;
    testMode[1] = MAD_SNOOPING_FROM_NET;
    testMode[2] = MAD_SNOOPING_FROM_MAC;

    for(port=0; port<dev->numOfPorts; port++)
    {
        for(testCase=0; testCase<nCases; testCase++)
        {   
            retVal = mdSysSetSnoopCtrl(dev,port,testMode[testCase]);
            if (retVal != MAD_OK)
            {
		     	if((dev->deviceId==MAD_SW1680)&&(retVal==MAD_API_ERROR_OVER_PORT_RANGE)) break;
                MSG_PRINT(("Setting for (port %i, test case %i) returned ",(int)port,(int)testCase));
                testMADDisplayStatus(retVal);
                return retVal;             
            }
    
            retVal = mdSysGetSnoopCtrl(dev,port,&mode);
            if (retVal != MAD_OK)
            {
		     	if((dev->deviceId==MAD_SW1680)&&(retVal==MAD_API_ERROR_OVER_PORT_RANGE)) break;
                MSG_PRINT(("Getting for (port %i, test case %i) returned ",(int)port,(int)testCase));
                testMADDisplayStatus(retVal);
                return retVal;             
            }
            
            if (mode != testMode[testCase])
            {
                MSG_PRINT(("Unexpected return (port %i, test case %i, mode %i:%i)\n",
                            (int)port,(int)testCase,(int)mode,(int)testMode[testCase]));
                return MAD_FAIL;
            }

        }
    }

    MSG_PRINT(("Passed.\n\n"));
    return MAD_OK;
}

MAD_U32 testAutoSelect(MAD_DEV *dev)
{
    MAD_STATUS  retVal;
    MAD_LPORT   port;
    MAD_U32     testCase, nCases;
    MAD_U16    mode,testMode[4];

    MSG_PRINT(("Testing Auto selection media...\n"));

    nCases = 3;
    testMode[0] = MAD_AUTO_MEDIA_COPPER_SGMII;
    testMode[1] = MAD_AUTO_MEDIA_COPPER_1000BX;
    testMode[2] = MAD_AUTO_MEDIA_COPPER_100B_FX;   

    for(port=0; port<dev->numOfPorts; port++)
    {
        for(testCase=0; testCase<nCases; testCase++)
        {   
            retVal = mdSysSetAutoSelect(dev,port,testMode[testCase]);
            if (retVal != MAD_OK)
            {
		     	if((dev->deviceId==MAD_SW1680)&&(retVal==MAD_API_ERROR_OVER_PORT_RANGE)) break;
                MSG_PRINT(("Setting for (port %i, test case %i) returned ",(int)port,(int)testCase));
                testMADDisplayStatus(retVal);
                return retVal;             
            }
    
            retVal = mdSysGetAutoSelect(dev,port,&mode);
            if (retVal != MAD_OK)
            {
		     	if((dev->deviceId==MAD_SW1680)&&(retVal==MAD_API_ERROR_OVER_PORT_RANGE)) break;
                MSG_PRINT(("Getting for (port %i, test case %i) returned ",(int)port,(int)testCase));
                testMADDisplayStatus(retVal);
                return retVal;             
            }
            
            if (mode != testMode[testCase])
            {
                MSG_PRINT(("Unexpected return (port %i, test case %i, mode %i:%i)\n",
                            (int)port,(int)testCase,(int)mode,(int)testMode[testCase]));
                return MAD_FAIL;
            }

        }
    }

    MSG_PRINT(("Passed.\n\n"));
    return MAD_OK;
}

MAD_U32 testPreferredMedis(MAD_DEV *dev)
{
    MAD_STATUS  retVal;
    MAD_LPORT   port;
    MAD_U32     testCase, nCases;
    MAD_U16    mode,testMode[4];

    MSG_PRINT(("Testing Set Preferred media...\n"));

    nCases = 3;
    testMode[0] = MAD_PREFERRED_MEDIA_FIRST;
    testMode[1] = MAD_PREFERRED_MEDIA_COPPER;
    testMode[2] = MAD_PREFERRED_MEDIA_FIBER;

    for(port=0; port<dev->numOfPorts; port++)
    {
        for(testCase=0; testCase<nCases; testCase++)
        {   
            retVal = mdSysSetPreferredMedia(dev,port,testMode[testCase]);
            if (retVal != MAD_OK)
            {
		     	if((dev->deviceId==MAD_SW1680)&&(retVal==MAD_API_ERROR_OVER_PORT_RANGE)) break;
                MSG_PRINT(("Setting for (port %i, test case %i) returned ",(int)port,(int)testCase));
                testMADDisplayStatus(retVal);
                return retVal;             
            }
    
            retVal = mdSysGetPreferredMedia(dev,port,&mode);
            if (retVal != MAD_OK)
            {
		     	if((dev->deviceId==MAD_SW1680)&&(retVal==MAD_API_ERROR_OVER_PORT_RANGE)) break;
                MSG_PRINT(("Getting for (port %i, test case %i) returned ",(int)port,(int)testCase));
                testMADDisplayStatus(retVal);
                return retVal;             
            }
            
            if (mode != testMode[testCase])
            {
                MSG_PRINT(("Unexpected return (port %i, test case %i, mode %i:%i)\n",
                            (int)port,(int)testCase,(int)mode,(int)testMode[testCase]));
                return MAD_FAIL;
            }

        }
    }

    MSG_PRINT(("Passed.\n\n"));
    return MAD_OK;
}

MAD_U32 testMediaMode(MAD_DEV *dev)
{
    MAD_STATUS  retVal;
    MAD_LPORT   port;
    MAD_U32     testCase, nCases;
    MAD_U16    mode,testMode[6];

    MSG_PRINT(("Testing system media mode ...\n"));

    nCases = 6;
    testMode[0] = MAD_AUTO_MEDIA_COPPER_SGMII;     
    testMode[1] = MAD_MEDIA_QSGMII_2_COPPER;     
    testMode[2] = MAD_MEDIA_SGMII_2_COPPER;     
    testMode[3] = MAD_MEDIA_QSGMII_2_1000X;     
    testMode[4] = MAD_MEDIA_QSGMII_2_1000FX;     
    testMode[5] = MAD_MEDIA_QSGMII_2_SGMII;     

    for(port=0; port<dev->numOfPorts; port++)
    {
        for(testCase=0; testCase<nCases; testCase++)
        {   
            retVal = mdSysSetMediaMode(dev,port,testMode[testCase]);
            if (retVal != MAD_OK)
            {
		     	if((dev->deviceId==MAD_SW1680)&&(retVal==MAD_API_ERROR_OVER_PORT_RANGE)) break;
                MSG_PRINT(("Setting for (port %i, test case %i) returned ",(int)port,(int)testCase));
                testMADDisplayStatus(retVal);
                return retVal;             
            }
    
            retVal = mdSysGetMediaMode(dev,port,&mode);
            if (retVal != MAD_OK)
            {
		     	if((dev->deviceId==MAD_SW1680)&&(retVal==MAD_API_ERROR_OVER_PORT_RANGE)) break;
                MSG_PRINT(("Getting for (port %i, test case %i) returned ",(int)port,(int)testCase));
                testMADDisplayStatus(retVal);
                return retVal;             
            }
            
            if (mode != testMode[testCase])
            {
                MSG_PRINT(("Unexpected return (port %i, test case %i, mode %i:%i)\n",
                            (int)port,(int)testCase,(int)mode,(int)testMode[testCase]));
                return MAD_FAIL;
            }

        }
    }

    MSG_PRINT(("Passed.\n\n"));
    return MAD_OK;
}













MAD_U32 testLinkDownDelay(MAD_DEV *dev)
{
    MAD_STATUS  retVal;
    MAD_LPORT   port;
    MAD_U32     testCase, nCases;
    MAD_U16    mode,testMode[4];

    MSG_PRINT(("Testing Copper link down delay...\n"));

    nCases = 4;
    testMode[0] = MAD_1G_LINKDOWN_DELAY_0;
    testMode[1] = MAD_1G_LINKDOWN_DELAY_10;
    testMode[2] = MAD_1G_LINKDOWN_DELAY_20;
    testMode[3] = MAD_1G_LINKDOWN_DELAY_40;

    for(port=0; port<dev->numOfPorts; port++)
    {
        for(testCase=0; testCase<nCases; testCase++)
        {   
            retVal = mdCopperSetLinkDownDelay(dev,port,testMode[testCase]);
            if (retVal != MAD_OK)
            {
		     	if((dev->deviceId==MAD_SW1680)&&(retVal==MAD_API_ERROR_OVER_PORT_RANGE)) break;
                MSG_PRINT(("Setting for (port %i, test case %i) returned ",(int)port,(int)testCase));
                testMADDisplayStatus(retVal);
                return retVal;             
            }
    
            retVal = mdCopperGetLinkDownDelay(dev,port,&mode);
            if (retVal != MAD_OK)
            {
		     	if((dev->deviceId==MAD_SW1680)&&(retVal==MAD_API_ERROR_OVER_PORT_RANGE)) break;
                MSG_PRINT(("Getting for (port %i, test case %i) returned ",(int)port,(int)testCase));
                testMADDisplayStatus(retVal);
                return retVal;             
            }
            
            if (mode != testMode[testCase])
            {
                MSG_PRINT(("Unexpected return (port %i, test case %i, mode %i:%i)\n",
                            (int)port,(int)testCase,(int)mode,(int)testMode[testCase]));
                return MAD_FAIL;
            }

        }
    }

    MSG_PRINT(("Passed.\n\n"));
    return MAD_OK;
}

MAD_U32 testLinkDownDelayEn(MAD_DEV *dev)
{
    MAD_STATUS  retVal;
    MAD_LPORT   port;
    MAD_U32     testCase, nCases;
    MAD_BOOL    mode,testMode[4];

    MSG_PRINT(("Testing Copper link down delay enable...\n"));

    nCases = 2;
    testMode[0] = MAD_TRUE;
    testMode[1] = MAD_FALSE;

    for(port=0; port<dev->numOfPorts; port++)
    {
        for(testCase=0; testCase<nCases; testCase++)
        {   
            retVal = mdCopperSetLinkDownDelayEnable(dev,port,testMode[testCase]);
            if (retVal != MAD_OK)
            {
		     	if((dev->deviceId==MAD_SW1680)&&(retVal==MAD_API_ERROR_OVER_PORT_RANGE)) break;
                MSG_PRINT(("Setting for (port %i, test case %i) returned ",(int)port,(int)testCase));
                testMADDisplayStatus(retVal);
                return retVal;             
            }
    
            retVal = mdCopperGetLinkDownDelayEnable(dev,port,&mode);
            if (retVal != MAD_OK)
            {
		     	if((dev->deviceId==MAD_SW1680)&&(retVal==MAD_API_ERROR_OVER_PORT_RANGE)) break;
                MSG_PRINT(("Getting for (port %i, test case %i) returned ",(int)port,(int)testCase));
                testMADDisplayStatus(retVal);
                return retVal;             
            }
            
            if (mode != testMode[testCase])
            {
                MSG_PRINT(("Unexpected return (port %i, test case %i, mode %i:%i)\n",
                            (int)port,(int)testCase,(int)mode,(int)testMode[testCase]));
                return MAD_FAIL;
            }

        }
    }

    MSG_PRINT(("Passed.\n\n"));
    return MAD_OK;
}

MAD_U32 testMadLED(MAD_DEV *dev)
{
    MAD_STATUS  retVal;
    MAD_LPORT   port=0;
    MAD_LED_CTRL_DATA   ledData, testLedData, kpLedData;

    MSG_PRINT(("Testing LED Control data...\n"));
      retVal = mdSysGetLEDCtrl(dev,port,&kpLedData);
      if (retVal != MAD_OK)
      {
        MSG_PRINT(("Getting for (port %i) returned ",(int)port));
        testMADDisplayStatus(retVal);
        return retVal;             
      }

    testLedData.ledCtrl[0] = 1;
    testLedData.ledCtrl[1] = 2;
    testLedData.ledCtrl[2] = 3;
    testLedData.ledCtrl[3] = 4;
    testLedData.ledCtrl[4] = 5;
    testLedData.ledCtrl[5] = 6;
    testLedData.ledPolarity[0] = 1;
    testLedData.ledPolarity[1] = 2;
    testLedData.ledPolarity[2] = 3;
    testLedData.ledPolarity[3] = 3;
    testLedData.ledPolarity[4] = 2;
    testLedData.ledPolarity[5] = 1;
    testLedData.ledMixPercen0 = 0x5;
    testLedData.ledMixPercen1 = 0xa;
    testLedData.ledForceInt = 1;
    testLedData.ledPulseDuration = 0x5;
    testLedData.ledBlinkRate = 0x6;
    testLedData.ledSppedOffPulse_perio = 0x1;
    testLedData.ledSppedOnPulse_perio = 0x1;
    testLedData.ledFuncMap3 = 0x1;
    testLedData.ledFuncMap2 = 0x1;

    for(port=0; port<dev->numOfPorts; port++)
    {
      retVal = mdSysSetLEDCtrl(dev,port,&testLedData);
      if (retVal != MAD_OK)
      {
     	if((dev->deviceId==MAD_SW1680)&&(retVal==MAD_API_ERROR_OVER_PORT_RANGE)) break;
        MSG_PRINT(("Setting for (port %i) returned ",(int)port));
        testMADDisplayStatus(retVal);
        return retVal;             
      }
      retVal = mdSysGetLEDCtrl(dev,port,&ledData);
      if (retVal != MAD_OK)
      {
     	if((dev->deviceId==MAD_SW1680)&&(retVal==MAD_API_ERROR_OVER_PORT_RANGE)) break;
        MSG_PRINT(("Getting for (port %i) returned ",(int)port));
        testMADDisplayStatus(retVal);
        return retVal;             
      }
      if ((testLedData.ledCtrl[0] != ledData.ledCtrl[0]) ||
        (testLedData.ledCtrl[1] != ledData.ledCtrl[1]) ||
        (testLedData.ledCtrl[2] != ledData.ledCtrl[2]) ||
        (testLedData.ledCtrl[3] != ledData.ledCtrl[3]) ||
        (testLedData.ledCtrl[4] != ledData.ledCtrl[4]) ||
        (testLedData.ledCtrl[5] != ledData.ledCtrl[5]) ||
        (testLedData.ledPolarity[0] != ledData.ledPolarity[0]) ||
        (testLedData.ledPolarity[1] != ledData.ledPolarity[1]) ||
        (testLedData.ledPolarity[2] != ledData.ledPolarity[2]) ||
        (testLedData.ledPolarity[3] != ledData.ledPolarity[3]) ||
        (testLedData.ledPolarity[4] != ledData.ledPolarity[4]) ||
        (testLedData.ledPolarity[5] != ledData.ledPolarity[5]) ||
        (testLedData.ledMixPercen0 != ledData.ledMixPercen0) ||
        (testLedData.ledMixPercen1 != ledData.ledMixPercen1) ||
        (testLedData.ledForceInt != ledData.ledForceInt) ||
        (testLedData.ledPulseDuration != ledData.ledPulseDuration) ||
        (testLedData.ledBlinkRate != ledData.ledBlinkRate) ||
        (testLedData.ledSppedOffPulse_perio != ledData.ledSppedOffPulse_perio) ||
        (testLedData.ledSppedOnPulse_perio != ledData.ledSppedOnPulse_perio) ||
        (testLedData.ledFuncMap3 != ledData.ledFuncMap3) ||
        (testLedData.ledFuncMap2 != ledData.ledFuncMap2))
      {
        MSG_PRINT(("Unexpected return (port %i)\n", (int)port));
        MSG_PRINT((" ledCtrl %x:%x  %x:%x  %x:%x  %x:%x  %x:%x,  %x:%x \n", 
           testLedData.ledCtrl[0], ledData.ledCtrl[0],
           testLedData.ledCtrl[1], ledData.ledCtrl[1],
           testLedData.ledCtrl[2], ledData.ledCtrl[2],
           testLedData.ledCtrl[3], ledData.ledCtrl[3],
           testLedData.ledCtrl[4], ledData.ledCtrl[4],
           testLedData.ledCtrl[5], ledData.ledCtrl[5]));
        MSG_PRINT((" ledPolarity %x:%x  %x:%x  %x:%x  %x:%x  %x:%x,  %x:%x \n", 
           testLedData.ledPolarity[0], ledData.ledPolarity[0],
           testLedData.ledPolarity[1], ledData.ledPolarity[1],
           testLedData.ledPolarity[2], ledData.ledPolarity[2],
           testLedData.ledPolarity[3], ledData.ledPolarity[3],
           testLedData.ledPolarity[4], ledData.ledPolarity[4],
           testLedData.ledPolarity[5], ledData.ledPolarity[5]));
        MSG_PRINT((" Mix percentige 0_2_4: %x:%x  1_3_5: %x:%x \n", 
           testLedData.ledMixPercen0, ledData.ledMixPercen0,
           testLedData.ledMixPercen1, ledData.ledMixPercen1));
        MSG_PRINT((" Force INT: %x:%x  \n", 
           testLedData.ledForceInt, ledData.ledForceInt));
        MSG_PRINT((" led Pulse Duration: %x:%x  \n", 
           testLedData.ledPulseDuration, ledData.ledPulseDuration));
        MSG_PRINT((" Blink rate: %x:%x  \n", 
           testLedData.ledBlinkRate, ledData.ledBlinkRate));
        MSG_PRINT((" Speed off Pulse Period %x:%x  \n", 
           testLedData.ledSppedOffPulse_perio, ledData.ledSppedOffPulse_perio));
        MSG_PRINT((" Speed on Pulse Period %x:%x  \n", 
           testLedData.ledSppedOnPulse_perio, ledData.ledSppedOnPulse_perio));
        MSG_PRINT((" Function pin mapping LED3 : %x:%x  LED2: %x:%x \n", 
           testLedData.ledFuncMap3, ledData.ledFuncMap3,
           testLedData.ledFuncMap2, ledData.ledFuncMap2));
        return MAD_FAIL;
      }

    }
    port = 0;
      retVal = mdSysSetLEDCtrl(dev,port, &kpLedData);
      if (retVal != MAD_OK)
      {
        MSG_PRINT(("Getting for (port %i) returned ",(int)port));
        testMADDisplayStatus(retVal);
        return retVal;             
      }

    MSG_PRINT(("Passed.\n\n"));
    return MAD_OK;
}

MAD_U32 testCollAdj(MAD_DEV *dev)
{
    MAD_STATUS  retVal, retVal1=MAD_OK;
    MAD_LPORT   port;
    MAD_U32     testCase, nCases;
    MAD_U8    mode,testMode[8];
    MAD_U32     savedMode;
    MAD_BOOL    savedEn;

    MSG_PRINT(("Testing Late Collision window adjust..\n"));

    nCases = 8;
    testMode[0] = 1;
    testMode[1] = 2;
    testMode[2] = 4;
    testMode[3] = 8;
    testMode[4] = 0x10;
    testMode[5] = 0x15;
    testMode[6] = 0xa;
    testMode[7] = 0x1f;

    for(port=0; port<dev->numOfPorts; port++)
    {
      retVal = mdCopperGetAutoNeg(dev,port,&savedEn,&savedMode);
      if (retVal != MAD_OK)
      {
		if((dev->deviceId==MAD_SW1680)&&(retVal==MAD_API_ERROR_OVER_PORT_RANGE)) break;
        MSG_PRINT(("Getting Link mode or (port %i) returned error",(int)port));
        testMADDisplayStatus(retVal);
        return retVal;             
      }
   
      /* Foced to set Half dulpex mode */
      retVal = mdCopperSetAutoNeg(dev,port,MAD_FALSE,MAD_PHY_100HDX);
      if (retVal != MAD_OK)
      {
     	if((dev->deviceId==MAD_SW1680)&&(retVal==MAD_API_ERROR_OVER_PORT_RANGE)) break;
        MSG_PRINT(("Setting Link mode MAD_PHY_100HDX for (port %i) returned error",(int)port));
        testMADDisplayStatus(retVal);
        return retVal;             
      }
        for(testCase=0; testCase<nCases; testCase++)
        {   
    
            retVal1 = mdSysSetLateCollision_winAdj(dev,port,testMode[testCase]);
            if (retVal1 != MAD_OK)
            {
		     	if((dev->deviceId==MAD_SW1680)&&(retVal==MAD_API_ERROR_OVER_PORT_RANGE)) break;
                MSG_PRINT(("Setting for (port %i, test case %i) returned ",(int)port,(int)testCase));
                testMADDisplayStatus(retVal1);
                break;;             
            }
    
            retVal1 = mdSysGetLateCollision_winAdj(dev,port,&mode);
            if (retVal1 != MAD_OK)
            {
		     	if((dev->deviceId==MAD_SW1680)&&(retVal==MAD_API_ERROR_OVER_PORT_RANGE)) break;
                MSG_PRINT(("Getting for (port %i, test case %i) returned ",(int)port,(int)testCase));
                testMADDisplayStatus(retVal1);
                break;             
            }
            
            if (mode != testMode[testCase])
            {
                MSG_PRINT(("Unexpected return (port %i, test case %i, mode %i:%i)\n",
                            (int)port,(int)testCase,(int)mode,(int)testMode[testCase]));
                retVal1 = MAD_FAIL;
                break;
            }


        }
        /* Set back link mode */
        retVal = mdCopperSetAutoNeg(dev,port,savedEn,savedMode);
        if (retVal != MAD_OK)
        {
       	  if((dev->deviceId==MAD_SW1680)&&(retVal==MAD_API_ERROR_OVER_PORT_RANGE)) break;
          MSG_PRINT(("Setting Link mode  for (port %i) returned error",(int)port));
          testMADDisplayStatus(retVal);
          return retVal;             
        }
        if(retVal1!=MAD_OK)
            break;
        
    }

    if(retVal1!=MAD_OK)
    {
      /* MSG_PRINT(("Failed.\n\n")); */
      return retVal1;
    }

    MSG_PRINT(("Passed.\n\n"));

    return MAD_OK;
}

MAD_U32 testSyncFifo(MAD_DEV *dev)
{
    MAD_STATUS  retVal;
    MAD_LPORT   port;
    MAD_U32     testCase, nCases;
    MAD_U8    mode,mode1,testMode[4];

    MSG_PRINT(("Testing Synchronize FIFO depth..\n"));

    nCases = 4;
    testMode[0] = MAD_SYNC_TRANS_FIFO_00;
    testMode[1] = MAD_SYNC_TRANS_FIFO_01;
    testMode[2] = MAD_SYNC_TRANS_FIFO_02;
    testMode[3] = MAD_SYNC_TRANS_FIFO_03;

    for(port=0; port<dev->numOfPorts; port++)
    {
        for(testCase=0; testCase<nCases; testCase++)
        { 
            retVal = mdMacSetFifoDepth(dev,port,testMode[testCase],testMode[testCase]);
            if (retVal != MAD_OK)
            {
		     	if((dev->deviceId==MAD_SW1680)&&(retVal==MAD_API_ERROR_OVER_PORT_RANGE)) break;
                MSG_PRINT(("Setting for (port %i, test case %i) returned ",(int)port,(int)testCase));
                testMADDisplayStatus(retVal);
                return retVal;             
            }
    
            retVal = mdMacGetFifoDepth(dev,port,&mode, &mode1);
            if (retVal != MAD_OK)
            {
		     	if((dev->deviceId==MAD_SW1680)&&(retVal==MAD_API_ERROR_OVER_PORT_RANGE)) break;
                MSG_PRINT(("Getting for (port %i, test case %i) returned ",(int)port,(int)testCase));
                testMADDisplayStatus(retVal);
                return retVal;             
            }
            
            if ( (mode != testMode[testCase]) || (mode1 != testMode[testCase]))
            {
                MSG_PRINT(("Unexpected return (port %i, test case %i, mode %i+++%i:%i)\n",
                            (int)port,(int)testCase,(int)mode,(int)mode1,(int)testMode[testCase]));
                return MAD_FAIL;
            }

        }
    }

    MSG_PRINT(("Passed.\n\n"));
    return MAD_OK;
}

MAD_U32 testPowerDownEn(MAD_DEV *dev)
{
    MAD_STATUS  retVal;
    MAD_LPORT   port;
    MAD_U32     testCase, nCases;
    MAD_BOOL    mode,testMode[2];

    MSG_PRINT(("Testing Power Down enable...\n"));

    nCases = 2;
    testMode[0] = MAD_FALSE;
    testMode[1] = MAD_TRUE;

    for(port=0; port<dev->numOfPorts; port++)
    {
        for(testCase=0; testCase<nCases; testCase++)
        {   
            retVal = mdSysSetPhyEnable(dev,port,testMode[testCase]);
            if (retVal != MAD_OK)
            {
		     	if((dev->deviceId==MAD_SW1680)&&(retVal==MAD_API_ERROR_OVER_PORT_RANGE)) break;
                MSG_PRINT(("Setting for (port %i, test case %i) returned ",(int)port,(int)testCase));
                testMADDisplayStatus(retVal);
                return retVal;             
            }
    
            retVal = mdSysGetPhyEnable(dev,port,&mode);
            if (retVal != MAD_OK)
            {
		     	if((dev->deviceId==MAD_SW1680)&&(retVal==MAD_API_ERROR_OVER_PORT_RANGE)) break;
                MSG_PRINT(("Getting for (port %i, test case %i) returned ",(int)port,(int)testCase));
                testMADDisplayStatus(retVal);
                return retVal;             
            }
            
            if (mode != testMode[testCase])
            {
                MSG_PRINT(("Unexpected return (port %i, test case %i, mode %i:%i)\n",
                            (int)port,(int)testCase,(int)mode,(int)testMode[testCase]));
                return MAD_FAIL;
            }

        }
    }

    MSG_PRINT(("Passed.\n\n"));
    return MAD_OK;
}

MAD_U32 testQSGMIIPowerDownEn(MAD_DEV *dev)
{
    MAD_STATUS  retVal;
    MAD_LPORT   port;
    MAD_U32     testCase, nCases;
    MAD_BOOL    mode,testMode[2];

    MSG_PRINT(("Testing QSGMII Power Down enable...\n"));

    nCases = 2;
    testMode[0] = MAD_TRUE;
    testMode[1] = MAD_FALSE;

    for(port=0; port<dev->numOfPorts; port++)
    {
        for(testCase=0; testCase<nCases; testCase++)
        {   
            retVal = mdSysSetQSGMIIPwDown(dev,port,testMode[testCase]);
            if (retVal != MAD_OK)
            {
		     	if((dev->deviceId==MAD_SW1680)&&(retVal==MAD_API_ERROR_OVER_PORT_RANGE)) break;
                MSG_PRINT(("Setting for (port %i, test case %i) returned ",(int)port,(int)testCase));
                testMADDisplayStatus(retVal);
                return retVal;             
            }
    
            retVal = mdSysGetQSGMIIPwDown(dev,port,&mode);
            if (retVal != MAD_OK)
            {
		     	if((dev->deviceId==MAD_SW1680)&&(retVal==MAD_API_ERROR_OVER_PORT_RANGE)) break;
                MSG_PRINT(("Getting for (port %i, test case %i) returned ",(int)port,(int)testCase));
                testMADDisplayStatus(retVal);
                return retVal;             
            }
            
            if (mode != testMode[testCase])
            {
                MSG_PRINT(("Unexpected return (port %i, test case %i, mode %i:%i)\n",
                            (int)port,(int)testCase,(int)mode,(int)testMode[testCase]));
                return MAD_FAIL;
            }

        }
    }

    MSG_PRINT(("Passed.\n\n"));
    return MAD_OK;
}

MAD_U32 testMACPowerDownEn(MAD_DEV *dev)
{
    MAD_STATUS  retVal;
    MAD_LPORT   port;
    MAD_U32     testCase, nCases;
    MAD_BOOL    mode,testMode[2];

    MSG_PRINT(("Testing MAC Power Down enable...\n"));

    nCases = 2;
    testMode[0] = MAD_TRUE;
    testMode[1] = MAD_FALSE;

    for(port=0; port<dev->numOfPorts; port++)
    {
        for(testCase=0; testCase<nCases; testCase++)
        {   
            retVal = mdSysSetMacPowerDown(dev,port,testMode[testCase]);
            if (retVal != MAD_OK)
            {
		     	if((dev->deviceId==MAD_SW1680)&&(retVal==MAD_API_ERROR_OVER_PORT_RANGE)) break;
                MSG_PRINT(("Setting for (port %i, test case %i) returned ",(int)port,(int)testCase));
                testMADDisplayStatus(retVal);
                return retVal;             
            }
    
            retVal = mdSysGetMacPowerDown(dev,port,&mode);
            if (retVal != MAD_OK)
            {
		     	if((dev->deviceId==MAD_SW1680)&&(retVal==MAD_API_ERROR_OVER_PORT_RANGE)) break;
                MSG_PRINT(("Getting for (port %i, test case %i) returned ",(int)port,(int)testCase));
                testMADDisplayStatus(retVal);
                return retVal;             
            }
            
            if (mode != testMode[testCase])
            {
                MSG_PRINT(("Unexpected return (port %i, test case %i, mode %i:%i)\n",
                            (int)port,(int)testCase,(int)mode,(int)testMode[testCase]));
                return MAD_FAIL;
            }

        }
    }

    MSG_PRINT(("Passed.\n\n"));
    return MAD_OK;
}

MAD_U32 testDetectPowerDownMode(MAD_DEV *dev)
{
    MAD_STATUS  retVal;
    MAD_LPORT   port;
    MAD_U32     testCase, nCases;
    MAD_U16    mode,testMode[3];

    MSG_PRINT(("Testing Detect Power Down mode...\n"));

    if(dev->phyInfo.featureSet & MAD_PHY_30XX_MODE)
    {
      nCases = 2;
      testMode[0] = MAD_ENERGE_DETECT_MODE_YES;
      testMode[1] = MAD_ENERGE_DETECT_MODE_NO;
    }
    else
    {
      nCases = 3;
      testMode[0] = MAD_ENERGE_DETECT_MODE_1;
      testMode[1] = MAD_ENERGE_DETECT_MODE_2;
      testMode[2] = MAD_ENERGE_DETECT_MODE_NO;
    }

    for(port=0; port<dev->numOfPorts; port++)
    {
        for(testCase=0; testCase<nCases; testCase++)
        {   
            retVal = mdSysSetDetectPowerDownMode(dev,port,testMode[testCase]);
            if (retVal != MAD_OK)
            {
		     	if((dev->deviceId==MAD_SW1680)&&(retVal==MAD_API_ERROR_OVER_PORT_RANGE)) break;
                MSG_PRINT(("Setting for (port %i, test case %i) returned ",(int)port,(int)testCase));
                testMADDisplayStatus(retVal);
                return retVal;             
            }
    
            retVal = mdSysGetDetectPowerDownMode(dev,port,&mode);
            if (retVal != MAD_OK)
            {
		     	if((dev->deviceId==MAD_SW1680)&&(retVal==MAD_API_ERROR_OVER_PORT_RANGE)) break;
                MSG_PRINT(("Getting for (port %i, test case %i) returned ",(int)port,(int)testCase));
                testMADDisplayStatus(retVal);
                return retVal;             
            }
            
            if (mode != testMode[testCase])
            {
                MSG_PRINT(("Unexpected return (port %i, test case %i, mode %i:%i)\n",
                            (int)port,(int)testCase,(int)mode,(int)testMode[testCase]));
                return MAD_FAIL;
            }

        }
    }

    MSG_PRINT(("Passed.\n\n"));
    return MAD_OK;
}

MAD_U32 testEEELPIMode(MAD_DEV *dev)
{
    MAD_STATUS  retVal;
    MAD_LPORT   port;
    MAD_U32     testCase, nCases;
    MAD_EEE_LPI_TIMER      lpiTimer, testLpiTimer[2];
    MAD_EEE_LPI_CONFIG     lpiCfg, testLpiCfg[2];

    MSG_PRINT(("Testing  Energe efficiant ethernet low power mode...\n"));

    nCases = 2;
    testLpiCfg[0].cfg_master_lpi = 1;
    testLpiCfg[0].cfg_slave_lpi = 0;
    testLpiTimer[0].fastExitTime = 0x55;
    testLpiTimer[0].slowExitTime = 0xaa;
    testLpiTimer[0].fastEnterTime = 0x11;
    testLpiTimer[0].slowEnterTime = 0xff;

    testLpiCfg[1].cfg_master_lpi = 0;
    testLpiCfg[1].cfg_slave_lpi = 1;
    testLpiTimer[1].fastExitTime = 0xaa;
    testLpiTimer[1].slowExitTime = 0x55;
    testLpiTimer[1].fastEnterTime = 0x33;
    testLpiTimer[1].slowEnterTime = 0x77;


    for(port=0; port<dev->numOfPorts; port++)
    {
        for(testCase=0; testCase<nCases; testCase++)
        {   
            retVal = mdSysSetEEELPIMode(dev,port,&testLpiCfg[testCase],&testLpiTimer[testCase]);
            if (retVal != MAD_OK)
            {
		     	if((dev->deviceId==MAD_SW1680)&&(retVal==MAD_API_ERROR_OVER_PORT_RANGE)) break;
                MSG_PRINT(("Setting for (port %i, test case %i) returned ",(int)port,(int)testCase));
                testMADDisplayStatus(retVal);
                return retVal;             
            }
    
            retVal = mdSysGetEEELPIMode(dev,port,&lpiCfg, &lpiTimer);
            if (retVal != MAD_OK)
            {
		     	if((dev->deviceId==MAD_SW1680)&&(retVal==MAD_API_ERROR_OVER_PORT_RANGE)) break;
                MSG_PRINT(("Getting for (port %i, test case %i) returned ",(int)port,(int)testCase));
                testMADDisplayStatus(retVal);
                return retVal;             
            }
            
            if ((lpiCfg.cfg_master_lpi != testLpiCfg[testCase].cfg_master_lpi) ||
               (lpiCfg.cfg_slave_lpi != testLpiCfg[testCase].cfg_slave_lpi) ||
               (lpiTimer.fastExitTime != testLpiTimer[testCase].fastExitTime) ||
               (lpiTimer.slowExitTime != testLpiTimer[testCase].slowExitTime) ||
               (lpiTimer.fastEnterTime != testLpiTimer[testCase].fastEnterTime) ||
               (lpiTimer.slowEnterTime != testLpiTimer[testCase].slowEnterTime))
            {
                MSG_PRINT(("Unexpected return (port %i, test case %i, lpi cfg master %i:%i)\n",
                            (int)port,(int)testCase,lpiCfg.cfg_master_lpi, 
							testLpiCfg[testCase].cfg_master_lpi));
                MSG_PRINT(("test lpi cfg slave %i:%i)\n",
                            (int)port,(int)testCase,lpiCfg.cfg_master_lpi, 
							testLpiCfg[testCase].cfg_master_lpi,
							lpiCfg.cfg_slave_lpi, 
							testLpiCfg[testCase].cfg_slave_lpi));
			    MSG_PRINT(("lpiTimer.fastExitTime %i:%i, lpiTimer.slowExitTime %i:%i,lpiTimer.fastEnterTime %i:%i, lpiTimer.slowEnterTime %i:%i \n",
                     lpiTimer.fastExitTime, testLpiTimer[testCase].fastExitTime,
                     lpiTimer.slowExitTime, testLpiTimer[testCase].slowExitTime,
                     lpiTimer.fastEnterTime, testLpiTimer[testCase].fastEnterTime,
                     lpiTimer.slowEnterTime, testLpiTimer[testCase].slowEnterTime));
                return MAD_FAIL;
            }

        }
    }

    MSG_PRINT(("Passed.\n\n"));
    return MAD_OK;
}

MAD_U32 testEEELPIInterrupt(MAD_DEV *dev)
{
    MAD_STATUS  retVal;
    MAD_LPORT   port;
    MAD_U32     testCase, nCases;
    MAD_BOOL     rxEn, testRxEn[2];
    MAD_BOOL     txEn, testTxEn[2];

    MSG_PRINT(("Testing  Energe efficiant ethernet Interrupt...\n"));

    nCases = 1;
#if 0
    testRxEn[0] = 1;
    testTxEn[0] = 1;
#endif
    testRxEn[0] = 0;
    testTxEn[0] = 0;


    for(port=0; port<dev->numOfPorts; port++)
    {
        for(testCase=0; testCase<nCases; testCase++)
        {   
            retVal = mdSetEEELPIInterrupt(dev,port,testRxEn[testCase],testTxEn[testCase]);
            if (retVal != MAD_OK)
            {
		     	if((dev->deviceId==MAD_SW1680)&&(retVal==MAD_API_ERROR_OVER_PORT_RANGE)) break;
                MSG_PRINT(("Setting for (port %i, test case %i) returned ",(int)port,(int)testCase));
                testMADDisplayStatus(retVal);
                return retVal;             
            }
    
            retVal = mdGetEEELPIInterrupt(dev,port,&rxEn, &txEn);
            if (retVal != MAD_OK)
            {
		     	if((dev->deviceId==MAD_SW1680)&&(retVal==MAD_API_ERROR_OVER_PORT_RANGE)) break;
                MSG_PRINT(("Getting for (port %i, test case %i) returned ",(int)port,(int)testCase));
                testMADDisplayStatus(retVal);
                return retVal;             
            }
            
            if ((rxEn != testRxEn[testCase]) || (txEn != testTxEn[testCase]))
            {
                MSG_PRINT(("Unexpected return (port %i, test case %i, rxEn %i:%i, txEn %i:%i)\n",
                            (int)port,(int)testCase,rxEn,testRxEn[testCase],txEn,testTxEn[testCase]));
                return MAD_FAIL;
            }

        }
    }

    MSG_PRINT(("Passed.\n\n"));
    return MAD_OK;
}


MAD_U32 testEEELPIInterruptMask(MAD_DEV *dev)
{
    MAD_STATUS  retVal;
    MAD_LPORT   port;
    MAD_U32     testCase, nCases;
    MAD_BOOL     rxEn, testRxEn[2];
    MAD_BOOL     txEn, testTxEn[2];

    MSG_PRINT(("Testing  Energe efficiant ethernet Interrupt Mask...\n"));

    nCases = 2;
    testRxEn[0] = 1;
    testTxEn[0] = 1;

    testRxEn[1] = 0;
    testTxEn[1] = 0;


    for(port=0; port<dev->numOfPorts; port++)
    {
        for(testCase=0; testCase<nCases; testCase++)
        {   
            retVal = mdSetEEELPIInterruptMask(dev,port,testRxEn[testCase],testTxEn[testCase]);
            if (retVal != MAD_OK)
            {
		     	if((dev->deviceId==MAD_SW1680)&&(retVal==MAD_API_ERROR_OVER_PORT_RANGE)) break;
                MSG_PRINT(("Setting for (port %i, test case %i) returned ",(int)port,(int)testCase));
                testMADDisplayStatus(retVal);
                return retVal;             
            }
    
            retVal = mdGetEEELPIInterruptMask(dev,port,&rxEn, &txEn);
            if (retVal != MAD_OK)
            {
		     	if((dev->deviceId==MAD_SW1680)&&(retVal==MAD_API_ERROR_OVER_PORT_RANGE)) break;
                MSG_PRINT(("Getting for (port %i, test case %i) returned ",(int)port,(int)testCase));
                testMADDisplayStatus(retVal);
                return retVal;             
            }
            
            if ((rxEn != testRxEn[testCase]) || (txEn != testTxEn[testCase]))
            {
                MSG_PRINT(("Unexpected return (port %i, test case %i, rxEn %i:%i, txEn %i:%i)\n",
                            (int)port,(int)testCase,rxEn,testRxEn[testCase],txEn,testTxEn[testCase]));
                return MAD_FAIL;
            }

        }
    }

    MSG_PRINT(("Passed.\n\n"));
    return MAD_OK;
}

MAD_U32 testEEELPIIgrBufThresh(MAD_DEV *dev)
{
    MAD_STATUS  retVal;
    MAD_LPORT   port;
    MAD_U32     testCase, nCases;
    MAD_U8     igr_buf_on_thresh, testIgr_buf_on_thresh[2];
    MAD_U8     igr_buf_off_thresh, testIgr_buf_off_thresh[2];

    MSG_PRINT(("Testing  Energe efficiant ethernet Igr Buffer Threshold...\n"));

    nCases = 2;
    testIgr_buf_on_thresh[0] = 0x55;
    testIgr_buf_off_thresh[0] = 0xaa;

    testIgr_buf_on_thresh[1] = 0xaa;
    testIgr_buf_off_thresh[1] = 0x55;


    for(port=0; port<dev->numOfPorts; port++)
    {
        for(testCase=0; testCase<nCases; testCase++)
        {   
            retVal = mdSetEEELPIIgrBuffThresh(dev,port,testIgr_buf_on_thresh[testCase],testIgr_buf_off_thresh[testCase]);
            if (retVal != MAD_OK)
            {
		     	if((dev->deviceId==MAD_SW1680)&&(retVal==MAD_API_ERROR_OVER_PORT_RANGE)) break;
                MSG_PRINT(("Setting for (port %i, test case %i) returned ",(int)port,(int)testCase));
                testMADDisplayStatus(retVal);
                return retVal;             
            }
    
            retVal = mdGetEEELPIIgrBuffThresh(dev,port,&igr_buf_on_thresh, &igr_buf_off_thresh);
            if (retVal != MAD_OK)
            {
		     	if((dev->deviceId==MAD_SW1680)&&(retVal==MAD_API_ERROR_OVER_PORT_RANGE)) break;
                MSG_PRINT(("Getting for (port %i, test case %i) returned ",(int)port,(int)testCase));
                testMADDisplayStatus(retVal);
                return retVal;             
            }
            
            if ((igr_buf_on_thresh != testIgr_buf_on_thresh[testCase]) || (igr_buf_off_thresh != testIgr_buf_off_thresh[testCase]))
            {
                MSG_PRINT(("Unexpected return (port %i, test case %i, igr_buf_on_thresh %i:%i, igr_buf_off_thresh %i:%i)\n",
                        (int)port,(int)testCase,
						igr_buf_on_thresh,testIgr_buf_on_thresh[testCase],
						igr_buf_off_thresh,testIgr_buf_off_thresh[testCase]));
                return MAD_FAIL;
            }

        }
    }

    MSG_PRINT(("Passed.\n\n"));
    return MAD_OK;
}


MAD_U32 testEEELPIEEEBufThresh(MAD_DEV *dev)
{
    MAD_STATUS  retVal;
    MAD_LPORT   port;
    MAD_U32     testCase, nCases;
    MAD_U8     eee_buf_on_thresh, testEEE_buf_on_thresh[2];
    MAD_U8     eee_buf_off_thresh, testEEE_buf_off_thresh[2];

    MSG_PRINT(("Testing  Energe efficiant ethernet EEE Buffer Threshold...\n"));

    nCases = 2;
    testEEE_buf_on_thresh[0] = 0x55;
    testEEE_buf_off_thresh[0] = 0xaa;

    testEEE_buf_on_thresh[1] = 0xaa;
    testEEE_buf_off_thresh[1] = 0x55;


    for(port=0; port<dev->numOfPorts; port++)
    {
        for(testCase=0; testCase<nCases; testCase++)
        {   
            retVal = mdSetEEELPIEEEBuffThresh(dev,port,testEEE_buf_on_thresh[testCase],testEEE_buf_off_thresh[testCase]);
            if (retVal != MAD_OK)
            {
		     	if((dev->deviceId==MAD_SW1680)&&(retVal==MAD_API_ERROR_OVER_PORT_RANGE)) break;
                MSG_PRINT(("Setting for (port %i, test case %i) returned ",(int)port,(int)testCase));
                testMADDisplayStatus(retVal);
                return retVal;             
            }
    
            retVal = mdGetEEELPIEEEBuffThresh(dev,port,&eee_buf_on_thresh, &eee_buf_off_thresh);
            if (retVal != MAD_OK)
            {
		     	if((dev->deviceId==MAD_SW1680)&&(retVal==MAD_API_ERROR_OVER_PORT_RANGE)) break;
                MSG_PRINT(("Getting for (port %i, test case %i) returned ",(int)port,(int)testCase));
                testMADDisplayStatus(retVal);
                return retVal;             
            }
            
            if ((eee_buf_on_thresh != testEEE_buf_on_thresh[testCase]) || (eee_buf_off_thresh != testEEE_buf_off_thresh[testCase]))
            {
                MSG_PRINT(("Unexpected return (port %i, test case %i, eee_buf_on_thresh %i:%i, eee_buf_off_thresh %i:%i)\n",
                        (int)port,(int)testCase,
						eee_buf_on_thresh,testEEE_buf_on_thresh[testCase],
						eee_buf_off_thresh,testEEE_buf_off_thresh[testCase]));
                return MAD_FAIL;
            }

        }
    }

    MSG_PRINT(("Passed.\n\n"));
    return MAD_OK;
}


MAD_U32 testEEELPIIgrDscThresh(MAD_DEV *dev)
{
    MAD_STATUS  retVal;
    MAD_LPORT   port;
    MAD_U32     testCase, nCases;
    MAD_U8     igr_dsc_on_thresh, testIgr_dsc_on_thresh[2];
    MAD_U8     igr_dsc_off_thresh, testIgr_dsc_off_thresh[2];

    MSG_PRINT(("Testing  Energe efficiant ethernet IGR Descriptor Buffer Threshold...\n"));

    nCases = 2;
    testIgr_dsc_on_thresh[0] = 0x55;
    testIgr_dsc_off_thresh[0] = 0xaa;

    testIgr_dsc_on_thresh[1] = 0xaa;
    testIgr_dsc_off_thresh[1] = 0x55;


    for(port=0; port<dev->numOfPorts; port++)
    {
        for(testCase=0; testCase<nCases; testCase++)
        {   
            retVal = mdSetEEELPIIgrDscThresh(dev,port,testIgr_dsc_on_thresh[testCase],testIgr_dsc_off_thresh[testCase]);
            if (retVal != MAD_OK)
            {
		     	if((dev->deviceId==MAD_SW1680)&&(retVal==MAD_API_ERROR_OVER_PORT_RANGE)) break;
                MSG_PRINT(("Setting for (port %i, test case %i) returned ",(int)port,(int)testCase));
                testMADDisplayStatus(retVal);
                return retVal;             
            }
    
            retVal = mdGetEEELPIIgrDscThresh(dev,port,&igr_dsc_on_thresh, &igr_dsc_off_thresh);
            if (retVal != MAD_OK)
            {
		     	if((dev->deviceId==MAD_SW1680)&&(retVal==MAD_API_ERROR_OVER_PORT_RANGE)) break;
                MSG_PRINT(("Getting for (port %i, test case %i) returned ",(int)port,(int)testCase));
                testMADDisplayStatus(retVal);
                return retVal;             
            }
            
            if ((igr_dsc_on_thresh != testIgr_dsc_on_thresh[testCase]) || (igr_dsc_off_thresh != testIgr_dsc_off_thresh[testCase]))
            {
                MSG_PRINT(("Unexpected return (port %i, test case %i, igr_buf_on_thresh %i:%i, igr_buf_off_thresh %i:%i)\n",
                        (int)port,(int)testCase,
						igr_dsc_on_thresh,testIgr_dsc_on_thresh[testCase],
						igr_dsc_off_thresh,testIgr_dsc_off_thresh[testCase]));
                return MAD_FAIL;
            }

        }
    }

    MSG_PRINT(("Passed.\n\n"));
    return MAD_OK;
}

MAD_U32 testUniDirTransEn(MAD_DEV *dev)
{
    MAD_STATUS  retVal;
    MAD_LPORT   port;
    MAD_U32     testCase, nCases;
    MAD_BOOL    mode,testMode[2];

    MSG_PRINT(("Testing Unidirectional transmit enable...\n"));

    nCases = 2;
    testMode[0] = MAD_TRUE;
    testMode[1] = MAD_FALSE;

    for(port=0; port<dev->numOfPorts; port++)
    {
        for(testCase=0; testCase<nCases; testCase++)
        {   
            retVal = mdSysSetUniDirTrans(dev,port,testMode[testCase]);
            if (retVal != MAD_OK)
            {
		     	if((dev->deviceId==MAD_SW1680)&&(retVal==MAD_API_ERROR_OVER_PORT_RANGE)) break;
                MSG_PRINT(("Setting for (port %i, test case %i) returned ",(int)port,(int)testCase));
                testMADDisplayStatus(retVal);
                return retVal;             
            }
    
            retVal = mdSysGetUniDirTrans(dev,port,&mode);
            if (retVal != MAD_OK)
            {
		     	if((dev->deviceId==MAD_SW1680)&&(retVal==MAD_API_ERROR_OVER_PORT_RANGE)) break;
                MSG_PRINT(("Getting for (port %i, test case %i) returned ",(int)port,(int)testCase));
                testMADDisplayStatus(retVal);
                return retVal;             
            }
            
            if (mode != testMode[testCase])
            {
                MSG_PRINT(("Unexpected return (port %i, test case %i, mode %i:%i)\n",
                            (int)port,(int)testCase,(int)mode,(int)testMode[testCase]));
                return MAD_FAIL;
            }

        }
    }

    MSG_PRINT(("Passed.\n\n"));
    return MAD_OK;
}

MAD_STATUS testPatPRBS(MAD_DEV *dev)
{
    MAD_STATUS retVal;
    MAD_LPORT   port;
    MAD_U32   testCase, nCases;
    MAD_PRBS_GEN_CONFIG    mode,testMode[2];

    MSG_PRINT(("Test setting Pattern generator PRBS \n"));

    nCases = 2;
    testMode[0].genEn = 1;    
    testMode[0].countEn = 1;    
    testMode[0].polarCheckInv = 1;
    testMode[0].polarGenInv = 1;
    testMode[0].counterLock = 1;
    testMode[0].patternSel = 3;
    testMode[0].errCounter = 0;

    testMode[1].genEn = 0;    
    testMode[1].countEn = 0;    
    testMode[1].polarCheckInv = 0;
    testMode[1].polarGenInv = 0;
    testMode[1].counterLock = 0;
    testMode[1].patternSel = 0;
    testMode[1].errCounter = 0;


    for(port=0; port<dev->numOfPorts; port++)
    {
        for(testCase=0; testCase<nCases; testCase++)
        {   
            retVal = madPatCtrlSetPRBS(dev,port,&testMode[testCase]);
            if (retVal != MAD_OK)
            {
		     	if((dev->deviceId==MAD_SW1680)&&(retVal==MAD_API_ERROR_OVER_PORT_RANGE)) break;
                MSG_PRINT(("Setting for (port %i, test case %i) returned ",(int)port,(int)testCase));
                testMADDisplayStatus(retVal);
                return retVal;             
            }
    
            retVal = madPatCtrlGetPRBS(dev,port,&mode);
            if (retVal != MAD_OK)
            {
		     	if((dev->deviceId==MAD_SW1680)&&(retVal==MAD_API_ERROR_OVER_PORT_RANGE)) break;
                MSG_PRINT(("Getting for (port %i, test case %i) returned ",(int)port,(int)testCase));
                testMADDisplayStatus(retVal);
                return retVal;             
            }
            
             if((mode.genEn != testMode[testCase].genEn) ||    
                (mode.countEn != testMode[testCase].countEn) ||    
                (mode.polarCheckInv != testMode[testCase].polarCheckInv) ||
                (mode.polarGenInv != testMode[testCase].polarGenInv) ||
                (mode.counterLock != testMode[testCase].counterLock) ||
                (mode.patternSel != testMode[testCase].patternSel) )
            {
                MSG_PRINT(("Unexpected return (port %i, test case %i, genEn %i:%i)\n",
                            (int)port,(int)testCase,(int)mode.genEn,(int)testMode[testCase].genEn));
                MSG_PRINT(("        countEn %i:%i)\n", (int)mode.countEn,(int)testMode[testCase].countEn));
                MSG_PRINT(("        polarCheckInv %i:%i)\n", (int)mode.polarCheckInv,(int)testMode[testCase].polarCheckInv));
                MSG_PRINT(("        polarGenInv %i:%i)\n", (int)mode.polarGenInv,(int)testMode[testCase].polarGenInv));
                MSG_PRINT(("        counterLock %i:%i)\n", (int)mode.counterLock,(int)testMode[testCase].counterLock));
                MSG_PRINT(("        patternSel %i:%i)\n", (int)mode.patternSel,(int)testMode[testCase].patternSel));
                return MAD_FAIL;
            }
        }
    }

    MSG_PRINT(("Passed.\n\n"));
    
    return MAD_OK;
}

MAD_STATUS testPat125PRBS(MAD_DEV *dev)
{
    MAD_STATUS retVal;
    MAD_LPORT   port;
    MAD_U32   testCase, nCases;
    MAD_PRBS_GEN_CONFIG    mode,testMode[2];

    MSG_PRINT(("Test setting Pattern generator 1.25G PRBS \n"));

    nCases = 2;
    testMode[0].genEn = 1;    
    testMode[0].countEn = 1;    
    testMode[0].polarCheckInv = 1;
    testMode[0].polarGenInv = 1;
    testMode[0].counterLock = 1;
    testMode[0].errCounter = 0;

    testMode[1].genEn = 0;    
    testMode[1].countEn = 0;    
    testMode[1].polarCheckInv = 0;
    testMode[1].polarGenInv = 0;
    testMode[1].counterLock = 0;
    testMode[1].errCounter = 0;


    for(port=0; port<dev->numOfPorts; port++)
    {
        for(testCase=0; testCase<nCases; testCase++)
        {   
            retVal = madPatCtrlSet125GPRBS(dev,port,&testMode[testCase]);
            if (retVal != MAD_OK)
            {
		     	if((dev->deviceId==MAD_SW1680)&&(retVal==MAD_API_ERROR_OVER_PORT_RANGE)) break;
                MSG_PRINT(("Setting for (port %i, test case %i) returned %x\n",(int)port,(int)testCase, retVal));
                testMADDisplayStatus(retVal);
                return retVal;             
            }
    
            retVal = madPatCtrlGet125GPRBS(dev,port,&mode);
            if (retVal != MAD_OK)
            {
		     	if((dev->deviceId==MAD_SW1680)&&(retVal==MAD_API_ERROR_OVER_PORT_RANGE)) break;
                MSG_PRINT(("Getting for (port %i, test case %i) returned %x",(int)port,(int)testCase, retVal));
                testMADDisplayStatus(retVal);
                return retVal;             
            }
            
             if((mode.genEn != testMode[testCase].genEn) ||    
                (mode.countEn != testMode[testCase].countEn) ||    
                (mode.polarCheckInv != testMode[testCase].polarCheckInv) ||
                (mode.polarGenInv != testMode[testCase].polarGenInv) ||
                (mode.counterLock != testMode[testCase].counterLock) )
            {
                MSG_PRINT(("Unexpected return (port %i, test case %i, genEn %i:%i)\n",
                            (int)port,(int)testCase,(int)mode.genEn,(int)testMode[testCase].genEn));
                MSG_PRINT(("        countEn %i:%i)\n", (int)mode.countEn,(int)testMode[testCase].countEn));
                MSG_PRINT(("        polarCheckInv %i:%i)\n", (int)mode.polarCheckInv,(int)testMode[testCase].polarCheckInv));
                MSG_PRINT(("        polarGenInv %i:%i)\n", (int)mode.polarGenInv,(int)testMode[testCase].polarGenInv));
                MSG_PRINT(("        counterLock %i:%i)\n", (int)mode.counterLock,(int)testMode[testCase].counterLock));
                MSG_PRINT(("        patternSel %i:%i)\n", (int)mode.patternSel,(int)testMode[testCase].patternSel));
                return MAD_FAIL;
            }
        }
    }

    MSG_PRINT(("Passed.\n\n"));
    
    return MAD_OK;
}

MAD_STATUS testPtpEnable(MAD_DEV *dev)
{
    MAD_STATUS retVal;
    MAD_U32   testCase, nCases;
    MAD_BOOL    mode,testMode[2];

    MSG_PRINT(("Test setting PTP Enable \n"));

    nCases = 2;
    testMode[0] = MAD_TRUE;    

    testMode[1] = MAD_FALSE;    


    for(testCase=0; testCase<nCases; testCase++)
    {   
        retVal = mdPtpSetEnable(dev,testMode[testCase]);
        if (retVal != MAD_OK)
        {
            MSG_PRINT(("Setting for (port %i, test case %i) returned %x\n",(int)0,(int)testCase, retVal));
            testMADDisplayStatus(retVal);
            return retVal;             
        }
    
        retVal = mdPtpGetEnable(dev,&mode);
        if (retVal != MAD_OK)
        {
            MSG_PRINT(("Getting for (port %i, test case %i) returned %x",(int)0,(int)testCase, retVal));
            testMADDisplayStatus(retVal);
            return retVal;             
        }
          
        if (mode != testMode[testCase]) 
        {
            MSG_PRINT(("Unexpected return (port %i, test case %i, en %i:%i)\n",
                        (int)0,(int)testCase,(int)mode,(int)testMode[testCase]));
            return MAD_FAIL;
        }
    }

    MSG_PRINT(("Passed.\n\n"));
    
    return MAD_OK;
}

MAD_STATUS testPtpRefClkSrc(MAD_DEV *dev)
{
    MAD_STATUS retVal;
    MAD_U32   testCase, nCases;
    MAD_BOOL    mode,testMode[2];

    MSG_PRINT(("Test setting PTP reference clock source \n"));

    nCases = 2;
    testMode[0] = MAD_TRUE;    

    testMode[1] = MAD_FALSE;    

    for(testCase=0; testCase<nCases; testCase++)
    {   
        retVal = mdPtpSetRefClkSrc(dev,testMode[testCase]);
        if (retVal != MAD_OK)
        {
            MSG_PRINT(("Setting for (port %i, test case %i) returned %x\n",(int)0,(int)testCase, retVal));
            testMADDisplayStatus(retVal);
            return retVal;             
        }
    
        retVal = mdPtpGetRefClkSrc(dev,&mode);
        if (retVal != MAD_OK)
        {
            MSG_PRINT(("Getting for (port %i, test case %i) returned %x",(int)0,(int)testCase, retVal));
            testMADDisplayStatus(retVal);
            return retVal;             
        }
          
        if (mode != testMode[testCase]) 
        {
            MSG_PRINT(("Unexpected return (port %i, test case %i, RefClkSrc %i:%i)\n",
                        (int)0,(int)testCase,(int)mode,(int)testMode[testCase]));
            return MAD_FAIL;
        }
    }

    MSG_PRINT(("Passed.\n\n"));
    
    return MAD_OK;
}

MAD_STATUS testPtpInputSrc(MAD_DEV *dev)
{
    MAD_STATUS retVal;
    MAD_U32   testCase, nCases;
    MAD_BOOL    mode,testMode[2];

    MSG_PRINT(("Test setting PTP input source \n"));

    nCases = 2;
    testMode[0] = MAD_TRUE;    

    testMode[1] = MAD_FALSE;    


    for(testCase=0; testCase<nCases; testCase++)
    {   
        retVal = mdPtpSetInputSrc(dev,testMode[testCase]);
        if (retVal != MAD_OK)
        {
            MSG_PRINT(("Setting for (port %i, test case %i) returned %x\n",(int)0,(int)testCase, retVal));
            testMADDisplayStatus(retVal);
            return retVal;             
        }
    
        retVal = mdPtpGetInputSrc(dev,&mode);
        if (retVal != MAD_OK)
        {
            MSG_PRINT(("Getting for (port %i, test case %i) returned %x",(int)0,(int)testCase, retVal));
            testMADDisplayStatus(retVal);
            return retVal;             
        }
          
        if (mode != testMode[testCase]) 
        {
            MSG_PRINT(("Unexpected return (port %i, test case %i, InputSrc %i:%i)\n",
                        (int)0,(int)testCase,(int)mode,(int)testMode[testCase]));
            return MAD_FAIL;
        }
    }

    MSG_PRINT(("Passed.\n\n"));
    
    return MAD_OK;
}

MAD_STATUS testPtpIntEnable(MAD_DEV *dev)
{
  MAD_STATUS retVal;
  MAD_U32   testCase, nCases;
  MAD_BOOL    mode,testMode[2];
  MAD_BOOL en, kpEn;

  MSG_PRINT(("Test setting PTP Interrupt Enable \n"));

  nCases = 2;
  testMode[0] = MAD_TRUE;    
  testMode[1] = MAD_FALSE;    

#if 1
    retVal = mdPtpGetEnable(dev,&kpEn);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }

    retVal = mdPtpSetEnable(dev, MAD_TRUE);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }

    retVal = mdPtpGetEnable(dev,&en);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }

    if (en!=MAD_TRUE)
    {
      MSG_PRINT(("Can not enable PTP \n"));
      return MAD_FAIL;
    }
    else
      MSG_PRINT(("!!!!!!!! Enable PTP !!!!!!\n"));
#endif
    

    for(testCase=0; testCase<nCases; testCase++)
    {   
        retVal = mdPtpSetIntEnable(dev,testMode[testCase]);
        if (retVal != MAD_OK)
        {
            MSG_PRINT(("Setting for (port %i, test case %i) returned %x\n",(int)0,(int)testCase, retVal));
            testMADDisplayStatus(retVal);
            return retVal;             
        }
    
        retVal = mdPtpGetIntEnable(dev,&mode);
        if (retVal != MAD_OK)
        {
            MSG_PRINT(("Getting for (port %i, test case %i) returned %x",(int)0,(int)testCase, retVal));
            testMADDisplayStatus(retVal);
            return retVal;             
        }
          
        if (mode != testMode[testCase]) 
        {
            MSG_PRINT(("Unexpected return (port %i, test case %i, en %i:%i)\n",
                        (int)0,(int)testCase,(int)mode,(int)testMode[testCase]));
            return MAD_FAIL;
        }
    }

    MSG_PRINT(("Passed.\n\n"));
    
    return MAD_OK;
}

MAD_STATUS testPtpConfig(MAD_DEV *dev)
{
    MAD_STATUS retVal;
    MAD_U32   testCase, nCases;
    MAD_PTP_CONFIG    mode,testMode[2];
    MAD_BOOL en, kpEn;
    int port;

    MSG_PRINT(("Test setting PTP Config \n"));

#if 1
    retVal = mdPtpGetEnable(dev,&kpEn);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }

    retVal = mdPtpSetEnable(dev, MAD_TRUE);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }

    retVal = mdPtpGetEnable(dev,&en);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }

    if (en!=MAD_TRUE)
    {
      MSG_PRINT(("Can not enable PTP \n"));
      return MAD_FAIL;
    }
    else
      MSG_PRINT(("!!!!!!!! Enable PTP !!!!!!\n"));
#endif
    
    nCases = 2;
    testMode[0].ptpEType = 1;    
    testMode[0].msgIdTSEn = 1;    
    testMode[0].tsArrPtr = 1;
    testMode[0].ptpArrIntEn = 1;
    testMode[0].ptpDepIntEn = 1;
    testMode[0].transSpec = 1;
/*    testMode[0].msgIdStartBit = 1; */
    testMode[0].disTSOverwrite = 1;

    testMode[1].ptpEType = 0;    
    testMode[1].msgIdTSEn = 0;    
    testMode[1].tsArrPtr = 0;
    testMode[1].ptpArrIntEn = 0;
    testMode[1].ptpDepIntEn = 0;
    testMode[1].transSpec = 0;
/*    testMode[1].msgIdStartBit = 0; */
    testMode[1].disTSOverwrite = 0;

/*  for(port=0; port<4; port++) */
	for(port=0; port<dev->numOfPorts; port++)
  {
            testMode[0].ptpArrIntEn = (1 << port);
            testMode[0].ptpDepIntEn = (1 << port);
    for(testCase=0; testCase<nCases; testCase++)
    {   
      retVal = mdPtpSetConfig_port(dev,port,&testMode[testCase]);
      if (retVal != MAD_OK)
      {
     	if((dev->deviceId==MAD_SW1680)&&(retVal==MAD_API_ERROR_OVER_PORT_RANGE)) break;
        MSG_PRINT(("Setting for (port %i, test case %i) returned %x\n",(int)0,(int)testCase, retVal));
        testMADDisplayStatus(retVal);
        return retVal;             
      }
    
      retVal = mdPtpGetConfig_port(dev,port,&mode);
      if (retVal != MAD_OK)
      {
     	if((dev->deviceId==MAD_SW1680)&&(retVal==MAD_API_ERROR_OVER_PORT_RANGE)) break;
        MSG_PRINT(("Getting for (port %i, test case %i) returned %x",(int)0,(int)testCase, retVal));
        testMADDisplayStatus(retVal);
        return retVal;             
      }
            
      if((mode.ptpEType != testMode[testCase].ptpEType) ||    
        (mode.msgIdTSEn != testMode[testCase].msgIdTSEn) ||    
        (mode.tsArrPtr != testMode[testCase].tsArrPtr) ||
        (mode.ptpArrIntEn != testMode[testCase].ptpArrIntEn) ||
        (mode.ptpDepIntEn != testMode[testCase].ptpDepIntEn) ||
        (mode.transSpec != testMode[testCase].transSpec) ||
/*        (mode.msgIdStartBit != testMode[testCase].msgIdStartBit) || */
        (mode.disTSOverwrite != testMode[testCase].disTSOverwrite) )
      {
        MSG_PRINT(("Unexpected return (port %i, test case %i, ptpEType %i:%i)\n",
                    (int)0,(int)testCase,(int)mode.ptpEType,(int)testMode[testCase].ptpEType));
        MSG_PRINT(("        msgIdTSEn %i:%i)\n", (int)mode.msgIdTSEn,(int)testMode[testCase].msgIdTSEn));
        MSG_PRINT(("        tsArrPtr %i:%i)\n", (int)mode.tsArrPtr,(int)testMode[testCase].tsArrPtr));
        MSG_PRINT(("        ptpArrIntEn %i:%i)\n", (int)mode.ptpArrIntEn,(int)testMode[testCase].ptpArrIntEn));
        MSG_PRINT(("        ptpDepIntEn %i:%i)\n", (int)mode.ptpDepIntEn,(int)testMode[testCase].ptpDepIntEn));
        MSG_PRINT(("        transSpec %i:%i)\n", (int)mode.transSpec,(int)testMode[testCase].transSpec));
        /* MSG_PRINT(("        msgIdStartBit %i:%i)\n", (int)mode.msgIdStartBit,(int)testMode[testCase].msgIdStartBit)); */
        MSG_PRINT(("        disTSOverwrite %i:%i)\n", (int)mode.disTSOverwrite,(int)testMode[testCase].disTSOverwrite));
        return MAD_FAIL;
      }
#if 0
      MSG_PRINT((" (port %i, test case %i, ptpEType %i:%i)\n",
                      (int)0,(int)testCase,(int)mode.ptpEType,(int)testMode[testCase].ptpEType));
      MSG_PRINT(("        msgIdTSEn %i:%i)\n", (int)mode.msgIdTSEn,(int)testMode[testCase].msgIdTSEn));
      MSG_PRINT(("        tsArrPtr %i:%i)\n", (int)mode.tsArrPtr,(int)testMode[testCase].tsArrPtr));
      MSG_PRINT(("        ptpArrIntEn %i:%i)\n", (int)mode.ptpArrIntEn,(int)testMode[testCase].ptpArrIntEn));
      MSG_PRINT(("        ptpDepIntEn %i:%i)\n", (int)mode.ptpDepIntEn,(int)testMode[testCase].ptpDepIntEn));
      MSG_PRINT(("        transSpec %i:%i)\n", (int)mode.transSpec,(int)testMode[testCase].transSpec));
      MSG_PRINT(("        msgIdStartBit %i:%i)\n", (int)mode.msgIdStartBit,(int)testMode[testCase].msgIdStartBit));
      MSG_PRINT(("        disTSOverwrite %i:%i)\n", (int)mode.disTSOverwrite,(int)testMode[testCase].disTSOverwrite));
#endif
    }
  }

    MSG_PRINT(("Passed.\n\n"));
#if 1
    retVal = mdPtpSetEnable(dev,kpEn);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }
#endif
        
    return MAD_OK;
}


MAD_STATUS testPtpGlobalConfig(MAD_DEV *dev)
{
    MAD_STATUS retVal;
    MAD_U32   testCase, nCases;
    MAD_PTP_GLOBAL_CONFIG    mode,testMode[2];
    MAD_BOOL en, kpEn;

    MSG_PRINT(("Test setting PTP Global Config \n"));

#if 1
    retVal = mdPtpGetEnable(dev,&kpEn);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }

    retVal = mdPtpSetEnable(dev, MAD_TRUE);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }

    retVal = mdPtpGetEnable(dev,&en);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }

    if (en!=MAD_TRUE)
    {
      MSG_PRINT(("Can not enable PTP \n"));
      return MAD_FAIL;
    }
    else
      MSG_PRINT(("!!!!!!!! Enable PTP !!!!!!\n"));
#endif
    
    nCases = 2;
    testMode[0].ptpEType = 1;    
    testMode[0].msgIdTSEn = 1;    
    testMode[0].tsArrPtr = 1;

    testMode[1].ptpEType = 0;    
    testMode[1].msgIdTSEn = 0;    
    testMode[1].tsArrPtr = 0;


    for(testCase=0; testCase<nCases; testCase++)
    {   
      retVal = mdPtpSetGlobalConfig(dev,&testMode[testCase]);
      if (retVal != MAD_OK)
      {
        MSG_PRINT(("Setting for (port %i, test case %i) returned %x\n",(int)0,(int)testCase, retVal));
        testMADDisplayStatus(retVal);
        return retVal;             
      }
    
      retVal = mdPtpGetGlobalConfig(dev,&mode);
      if (retVal != MAD_OK)
      {
        MSG_PRINT(("Getting for (port %i, test case %i) returned %x",(int)0,(int)testCase, retVal));
        testMADDisplayStatus(retVal);
        return retVal;             
      }
            
      if((mode.ptpEType != testMode[testCase].ptpEType) ||    
        (mode.msgIdTSEn != testMode[testCase].msgIdTSEn) ||    
        (mode.tsArrPtr != testMode[testCase].tsArrPtr) )
      {
        MSG_PRINT(("Unexpected return (port %i, test case %i, ptpEType %i:%i)\n",
                    (int)0,(int)testCase,(int)mode.ptpEType,(int)testMode[testCase].ptpEType));
        MSG_PRINT(("        msgIdTSEn %i:%i)\n", (int)mode.msgIdTSEn,(int)testMode[testCase].msgIdTSEn));
        MSG_PRINT(("        tsArrPtr %i:%i)\n", (int)mode.tsArrPtr,(int)testMode[testCase].tsArrPtr));
        return MAD_FAIL;
      }
#if 1
      MSG_PRINT((" (port %i, test case %i, ptpEType %i:%i)\n",
                      (int)0,(int)testCase,(int)mode.ptpEType,(int)testMode[testCase].ptpEType));
      MSG_PRINT(("        msgIdTSEn %i:%i)\n", (int)mode.msgIdTSEn,(int)testMode[testCase].msgIdTSEn));
      MSG_PRINT(("        tsArrPtr %i:%i)\n", (int)mode.tsArrPtr,(int)testMode[testCase].tsArrPtr));
#endif
    }

    MSG_PRINT(("Passed.\n\n"));
#if 1
    retVal = mdPtpSetEnable(dev,kpEn);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }
#endif
        
    return MAD_OK;
}


MAD_STATUS testPtpPortConfig(MAD_DEV *dev)
{
    MAD_STATUS retVal;
    MAD_LPORT   port;
    MAD_U32   testCase, nCases;
    MAD_PTP_PORT_CONFIG    mode,testMode[2];
    MAD_BOOL en, kpEn;

    MSG_PRINT(("Test setting PTP Port Config \n"));

#if 1
    retVal = mdPtpGetEnable(dev,&kpEn);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }

    retVal = mdPtpSetEnable(dev, MAD_TRUE);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }

    retVal = mdPtpGetEnable(dev,&en);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }

    if (en!=MAD_TRUE)
    {
      MSG_PRINT(("Can not enable PTP \n"));
      return MAD_FAIL;
    }
    else
      MSG_PRINT(("!!!!!!!! Enable PTP !!!!!!\n"));
#endif
    
    nCases = 2;
    testMode[0].transSpec = 1;    
    testMode[0].disTSpec = MAD_TRUE;    
    testMode[0].etJump = 1;
    testMode[0].ipJump = 1;    
    testMode[0].ptpArrIntEn = MAD_TRUE;    
    testMode[0].ptpDepIntEn = MAD_TRUE;    
    testMode[0].disTSOverwrite = MAD_TRUE;
	/* 151x port config */
	testMode[0].arrTSMode = 1;
	testMode[0].filterAct = MAD_TRUE;
	testMode[0].hwAccPtp = MAD_TRUE;
	testMode[0].kpFramwSa = MAD_TRUE;
/*	testMode[0].fullCheck = MAD_TRUE; */
	testMode[0].noCorrupt = MAD_TRUE;
	testMode[0].extHwAcc = MAD_TRUE;
	testMode[0].arrLedCtrl = 0xff;
	testMode[0].depLedCtrl = 0xff;

    testMode[1].transSpec = 0;    
    testMode[1].disTSpec = MAD_FALSE;    
    testMode[1].etJump = 0;
    testMode[1].ipJump = 0;    
    testMode[1].ptpArrIntEn = MAD_FALSE;    
    testMode[1].ptpDepIntEn = MAD_FALSE;    
    testMode[1].disTSOverwrite = MAD_FALSE;
		/* 151x port config */
	testMode[1].arrTSMode = 0;
	testMode[1].filterAct = MAD_FALSE;
	testMode[1].hwAccPtp = MAD_FALSE;
	testMode[1].kpFramwSa = MAD_FALSE;
/*	testMode[1].fullCheck = MAD_FALSE; */
	testMode[1].noCorrupt = MAD_FALSE;
	testMode[1].extHwAcc = MAD_FALSE;
	testMode[1].arrLedCtrl = 0;
	testMode[1].depLedCtrl = 0;

  for(port=0; port<dev->numOfPorts; port++)
  {
    for(testCase=0; testCase<nCases; testCase++)
    {   
      retVal = mdPtpSetPortConfig(dev, port, &testMode[testCase]);
      if (retVal != MAD_OK)
      {
     	if((dev->deviceId==MAD_SW1680)&&(retVal==MAD_API_ERROR_OVER_PORT_RANGE)) break;
        MSG_PRINT(("Setting for (port %i, test case %i) returned %x\n",(int)port,(int)testCase, retVal));
        testMADDisplayStatus(retVal);
        return retVal;             
      }
    
      retVal = mdPtpGetPortConfig(dev, port, &mode);
      if (retVal != MAD_OK)
      {
     	if((dev->deviceId==MAD_SW1680)&&(retVal==MAD_API_ERROR_OVER_PORT_RANGE)) break;
        MSG_PRINT(("Getting for (port %i, test case %i) returned %x",(int)port,(int)testCase, retVal));
        testMADDisplayStatus(retVal);
        return retVal;             
      }
            
      if((mode.transSpec != testMode[testCase].transSpec) ||    
        (mode.disTSpec != testMode[testCase].disTSpec) ||    
        (mode.etJump != testMode[testCase].etJump) ||    
        (mode.ipJump != testMode[testCase].ipJump) ||    
        (mode.ptpArrIntEn != testMode[testCase].ptpArrIntEn) ||    
        (mode.ptpDepIntEn != testMode[testCase].ptpDepIntEn) ||    
        (mode.disTSOverwrite != testMode[testCase].disTSOverwrite) )
      {
        MSG_PRINT(("Unexpected return (port %i, test case %i, ptpEType %i:%i)\n",
                    (int)port,(int)testCase,(int)mode.transSpec,(int)testMode[testCase].transSpec));
        MSG_PRINT(("        disTSpec %i:%i)\n", (int)mode.disTSpec,(int)testMode[testCase].disTSpec));
        MSG_PRINT(("        etJump %i:%i)\n", (int)mode.etJump,(int)testMode[testCase].etJump));
        MSG_PRINT(("        ipJump %i:%i)\n", (int)mode.ipJump,(int)testMode[testCase].ipJump));
        MSG_PRINT(("        ptpArrIntEn %i:%i)\n", (int)mode.ptpArrIntEn,(int)testMode[testCase].ptpArrIntEn));
        MSG_PRINT(("        ptpDepIntEn %i:%i)\n", (int)mode.ptpDepIntEn,(int)testMode[testCase].ptpDepIntEn));
        MSG_PRINT(("        disTSOverwrite %i:%i)\n", (int)mode.disTSOverwrite,(int)testMode[testCase].disTSOverwrite));
        return MAD_FAIL;
      }
	  if (dev->deviceId == MAD_88E151x)
	  {
		if ((mode.arrTSMode != testMode[testCase].arrTSMode) ||
			(mode.hwAccPtp != testMode[testCase].hwAccPtp) ||
			(mode.kpFramwSa != testMode[testCase].kpFramwSa) ||
			/* (mode.fullCheck != testMode[testCase].fullCheck) || */
			(mode.noCorrupt != testMode[testCase].noCorrupt) ||
			(mode.extHwAcc != testMode[testCase].extHwAcc) ||
			(mode.arrLedCtrl != testMode[testCase].arrLedCtrl) ||
			(mode.depLedCtrl != testMode[testCase].depLedCtrl))
		{
			MSG_PRINT(("Unexpected return (port %i, test case %i, arrTSMode %i:%i)\n",
                    (int)port,(int)testCase,(int)mode.arrTSMode,(int)testMode[testCase].arrTSMode));
			MSG_PRINT(("        hwAccPtp %i:%i)\n", (int)mode.hwAccPtp,(int)testMode[testCase].hwAccPtp));
			MSG_PRINT(("        kpFramwSa %i:%i)\n", (int)mode.kpFramwSa,(int)testMode[testCase].kpFramwSa));
			/* MSG_PRINT(("        fullCheck %i:%i)\n", (int)mode.fullCheck,(int)testMode[testCase].fullCheck)); */
			MSG_PRINT(("        noCorrupt %i:%i)\n", (int)mode.noCorrupt,(int)testMode[testCase].noCorrupt));
			MSG_PRINT(("        extHwAcc %i:%i)\n", (int)mode.extHwAcc,(int)testMode[testCase].extHwAcc));
			MSG_PRINT(("        arrLedCtrl %i:%i)\n", (int)mode.arrLedCtrl,(int)testMode[testCase].arrLedCtrl));
			MSG_PRINT(("        depLedCtrl %i:%i)\n", (int)mode.depLedCtrl,(int)testMode[testCase].depLedCtrl));
			return MAD_FAIL;
		}
	  }
#if 0
    MSG_PRINT(("  (port %i, test case %i, ptpEType %i:%i)\n",
                (int)port,(int)testCase,(int)mode.transSpec,(int)testMode[testCase].transSpec));
    MSG_PRINT(("        disTSpec %i:%i)\n", (int)mode.disTSpec,(int)testMode[testCase].disTSpec));
    MSG_PRINT(("        etJump %i:%i)\n", (int)mode.etJump,(int)testMode[testCase].etJump));
    MSG_PRINT(("        ipJump %i:%i)\n", (int)mode.ipJump,(int)testMode[testCase].ipJump));
    MSG_PRINT(("        ptpArrIntEn %i:%i)\n", (int)mode.ptpArrIntEn,(int)testMode[testCase].ptpArrIntEn));
    MSG_PRINT(("        ptpDepIntEn %i:%i)\n", (int)mode.ptpDepIntEn,(int)testMode[testCase].ptpDepIntEn));
    MSG_PRINT(("        disTSOverwrite %i:%i)\n", (int)mode.disTSOverwrite,(int)testMode[testCase].disTSOverwrite));
#endif
    }
  }

    MSG_PRINT(("Passed.\n\n"));
#if 1
    retVal = mdPtpSetEnable(dev,kpEn);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }
#endif
        
    return MAD_OK;
}


MAD_STATUS testPtpPTPEn(MAD_DEV *dev)
{
    MAD_STATUS retVal;
    MAD_U32   testCase, nCases;
    MAD_BOOL    mode,testMode[2];
    MAD_BOOL en, kpEn;

    MSG_PRINT(("Test PTP setting PTP enable \n"));

    nCases = 2;
    testMode[0] = MAD_TRUE;    

    testMode[1] = MAD_FALSE;    

#if 1
    retVal = mdPtpGetEnable(dev,&kpEn);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }

    retVal = mdPtpSetEnable(dev, MAD_TRUE);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }

    retVal = mdPtpGetEnable(dev,&en);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }

    if (en!=MAD_TRUE)
    {
      MSG_PRINT(("Can not enable PTP \n"));
      return MAD_FAIL;
    }
    else
      MSG_PRINT(("!!!!!!!! Enable PTP !!!!!!\n"));
#endif


    for(testCase=0; testCase<nCases; testCase++)
    {   
        retVal = mdPtpSetPTPEn(dev,testMode[testCase]);
        if (retVal != MAD_OK)
        {
            MSG_PRINT(("Setting for (port %i, test case %i) returned %x\n",(int)0,(int)testCase, retVal));
            testMADDisplayStatus(retVal);
            return retVal;             
        }
    
        retVal = mdPtpGetPTPEn(dev,&mode);
        if (retVal != MAD_OK)
        {
            MSG_PRINT(("Getting for (port %i, test case %i) returned %x",(int)0,(int)testCase, retVal));
            testMADDisplayStatus(retVal);
            return retVal;             
        }
          
        if (mode != testMode[testCase]) 
        {
            MSG_PRINT(("Unexpected return (port %i, test case %i, en %i:%i)\n",
                        (int)0,(int)testCase,(int)mode,(int)testMode[testCase]));
            return MAD_FAIL;
        }
    }

    MSG_PRINT(("Passed.\n\n"));
#if 1
    retVal = mdPtpSetEnable(dev,kpEn);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }
#endif
        
    
    return MAD_OK;
}

MAD_STATUS testTaiEventConfig(MAD_DEV *dev)
{
    MAD_STATUS retVal;
    MAD_U32   testCase, nCases;
    MAD_TAI_EVENT_CONFIG    mode,testMode[2];
    MAD_BOOL en, kpEn;

    MSG_PRINT(("Test TAI Event Config \n"));

    nCases = 2;
    testMode[0].eventOverwrite = MAD_TRUE;    
    testMode[0].eventCtrStart = MAD_TRUE;    
    testMode[0].intEn = MAD_TRUE;   
	testMode[0].captTrigEvent = MAD_TRUE;
    testMode[0].eventPhase = MAD_TRUE;

    testMode[1].eventOverwrite = MAD_FALSE;    
    testMode[1].eventCtrStart = MAD_FALSE;    
    testMode[1].intEn = MAD_FALSE; 
	testMode[1].captTrigEvent = MAD_FALSE;
    testMode[1].eventPhase = MAD_FALSE;

#if 1
    retVal = mdPtpGetEnable(dev,&kpEn);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }

    retVal = mdPtpSetEnable(dev, MAD_TRUE);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }

    retVal = mdPtpGetEnable(dev,&en);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }

    if (en!=MAD_TRUE)
    {
      MSG_PRINT(("Can not enable PTP \n"));
      return MAD_FAIL;
    }
    else
      MSG_PRINT(("!!!!!!!! Enable PTP !!!!!!\n"));
#endif


    for(testCase=0; testCase<nCases; testCase++)
    {   
        retVal = madTaiSetEventConfig(dev,&testMode[testCase]);
        if (retVal != MAD_OK)
        {
            MSG_PRINT(("Setting for (port %i, test case %i) returned %x\n",(int)0,(int)testCase, retVal));
            testMADDisplayStatus(retVal);
            return retVal;             
        }
    
        retVal = madTaiGetEventConfig(dev,&mode);
        if (retVal != MAD_OK)
        {
            MSG_PRINT(("Getting for (port %i, test case %i) returned %x",(int)0,(int)testCase, retVal));
            testMADDisplayStatus(retVal);
            return retVal;             
        }
          
      if((mode.eventOverwrite != testMode[testCase].eventOverwrite) ||    
        (mode.eventCtrStart != testMode[testCase].eventCtrStart) ||    
        (mode.intEn != testMode[testCase].intEn) )
      {
        MSG_PRINT(("Unexpected return (port %i, test case %i, eventOverwrite %i:%i)\n",
                    (int)0,(int)testCase,(int)mode.eventOverwrite,(int)testMode[testCase].eventOverwrite));
        MSG_PRINT(("        eventCtrStart %i:%i)\n", (int)mode.eventCtrStart,(int)testMode[testCase].eventCtrStart));
        MSG_PRINT(("        intEn %i:%i)\n", (int)mode.intEn,(int)testMode[testCase].intEn));
        return MAD_FAIL;
      }

	  if (dev->deviceId == MAD_88E151x)
	  {
		if((mode.captTrigEvent != testMode[testCase].captTrigEvent) ||    
        (mode.eventPhase != testMode[testCase].eventPhase))
      {
        MSG_PRINT(("Unexpected return (port %i, test case %i, captTrigEvent %i:%i)\n",
                    (int)0,(int)testCase,(int)mode.captTrigEvent,(int)testMode[testCase].captTrigEvent));
        MSG_PRINT(("        eventPhase %i:%i)\n", (int)mode.eventPhase,(int)testMode[testCase].eventPhase));
        return MAD_FAIL;
      }
	  }
#if 0
      MSG_PRINT((" (port %i, test case %i, eventOverwrite %i:%i)\n",
                  (int)0,(int)testCase,(int)mode.eventOverwrite,(int)testMode[testCase].eventOverwrite));
      MSG_PRINT(("        eventCtrStart %i:%i)\n", (int)mode.eventCtrStart,(int)testMode[testCase].eventCtrStart));
      MSG_PRINT(("        intEn %i:%i)\n", (int)mode.intEn,(int)testMode[testCase].intEn));
#endif
    }

    MSG_PRINT(("Passed.\n\n"));
#if 1
    retVal = mdPtpSetEnable(dev,kpEn);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }
#endif
        
    
    return MAD_OK;
}


MAD_STATUS testTaiTrigConfig(MAD_DEV *dev)
{
    MAD_STATUS retVal;
    MAD_U32   testCase, nCases;
    MAD_TAI_TRIGGER_CONFIG    mode,testMode[4];
    MAD_BOOL trigEn, testTrigEn[4];
    MAD_BOOL en, kpEn;

    MSG_PRINT(("Test setting TAI Trig Config \n"));

#if 1
    retVal = mdPtpGetEnable(dev,&kpEn);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }

    retVal = mdPtpSetEnable(dev, MAD_TRUE);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }

    retVal = mdPtpGetEnable(dev,&en);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }

    if (en!=MAD_TRUE)
    {
      MSG_PRINT(("Can not enable PTP \n"));
      return MAD_FAIL;
    }
    else
      MSG_PRINT(("!!!!!!!! Enable PTP !!!!!!\n"));
#endif
    
    nCases = 4;
    testTrigEn[0] = MAD_TRUE;
    testMode[0].mode = MAD_TAI_TRIG_ON_GIVEN_TIME;    
    testMode[0].trigGenAmt = 11;    
    testMode[0].pulseWidth = 13;
	testMode[0].intEn = 1;
    testMode[0].trigClkComp = 0x5555;
    testMode[0].trigGenTime = 0x5555aaaa;
    testMode[0].trigGenDelay = 0x5555;
	testMode[0].trigWindow = 1;
	testMode[0].trig2Signal = 1;
	testMode[0].irlGenTime = 0x5555aaaa;

    testTrigEn[1] = MAD_TRUE;
    testMode[1].mode = MAD_TAI_TRIG_ON_GIVEN_TIME;    
    testMode[1].trigGenAmt = 0;    
    testMode[1].pulseWidth = 0;
	testMode[1].intEn = 0;
    testMode[1].trigClkComp = 0xaaaa;
    testMode[1].trigGenTime = 0xaaaa5555;
    testMode[1].trigGenDelay = 0xaaaa;
	testMode[1].trigWindow = 0;
	testMode[1].trig2Signal = 0;
	testMode[1].irlGenTime = 0;

    testTrigEn[2] = MAD_TRUE;
    testMode[2].mode = MAD_TAI_TRIG_PERIODIC_PULSE;    
    testMode[2].trigGenAmt = 11;    
    testMode[2].pulseWidth = 0;
	testMode[2].intEn = 1;
    testMode[2].trigClkComp = 0x5555;
    testMode[2].trigGenTime = 0x5555aaaa;
    testMode[2].trigGenDelay = 0x5555;
	testMode[2].trigWindow = 1;
	testMode[2].trig2Signal = 1;
	testMode[2].irlGenTime = 0xaaaa5555;

    testTrigEn[3] = MAD_TRUE;
    testMode[3].mode = MAD_TAI_TRIG_PERIODIC_PULSE;    
    testMode[3].trigGenAmt = 0;    
    testMode[3].pulseWidth = 0;
    testMode[3].trigClkComp =55;
	testMode[3].intEn = 0;
    testMode[3].trigClkComp = 0xaaaa;
    testMode[3].trigGenTime = 0xaaaa5555;
    testMode[3].trigGenDelay = 0xaaaa;
	testMode[3].trigWindow = 0;
	testMode[3].trig2Signal = 0;
	testMode[3].irlGenTime = 0;

    for(testCase=0; testCase<nCases; testCase++)
    {   
      retVal = madTaiSetTrigConfig(dev,testTrigEn[testCase], &testMode[testCase]);
      if (retVal != MAD_OK)
      {
        MSG_PRINT(("Setting for (port %i, test case %i) returned %x\n",(int)0,(int)testCase, retVal));
        testMADDisplayStatus(retVal);
        return retVal;             
      }
    
      retVal = madTaiGetTrigConfig(dev, &trigEn, &mode);
      if (retVal != MAD_OK)
      {
        MSG_PRINT(("Getting for (port %i, test case %i) returned %x",(int)0,(int)testCase, retVal));
        testMADDisplayStatus(retVal);
        return retVal;             
      }
            
      if (trigEn != testTrigEn[testCase]) 
        {
            MSG_PRINT(("Unexpected return (port %i, test case %i, en %i:%i)\n",
                        (int)0,(int)testCase,(int)trigEn,(int)testTrigEn[testCase]));
            return MAD_FAIL;
        }

      if((mode.mode != testMode[testCase].mode) ||    
        (mode.trigGenAmt != testMode[testCase].trigGenAmt) ||    
        (mode.pulseWidth != testMode[testCase].pulseWidth) ||
        (mode.trigClkComp != testMode[testCase].trigClkComp) )
      {
        MSG_PRINT(("Unexpected return (port %i, test case %i, mode %i:%i)\n",
                    (int)0,(int)testCase,(int)mode.mode,(int)testMode[testCase].mode));
        MSG_PRINT(("        trigGenAmt %i:%i)\n", (int)mode.trigGenAmt,(int)testMode[testCase].trigGenAmt));
        MSG_PRINT(("        pulseWidth %i:%i)\n", (int)mode.pulseWidth,(int)testMode[testCase].pulseWidth));
        MSG_PRINT(("        trigClkComp %i:%i)\n", (int)mode.trigClkComp,(int)testMode[testCase].trigClkComp));
        return MAD_FAIL;
      }

	  if (dev->deviceId == MAD_88E151x)
	  {
		  if((mode.intEn != testMode[testCase].intEn) ||    
			(mode.trigGenTime != testMode[testCase].trigGenTime) ||    
			(mode.trigGenDelay != testMode[testCase].trigGenDelay) ||   
			(mode.trigWindow != testMode[testCase].trigWindow) ||
			(mode.trig2Signal != testMode[testCase].trig2Signal) ||
			(mode.irlGenTime != testMode[testCase].irlGenTime))
		  {
				MSG_PRINT(("Unexpected return (port %i, test case %i, intEn %i:%i)\n",
                    (int)0,(int)testCase,(int)mode.intEn,(int)testMode[testCase].intEn));
				MSG_PRINT(("        trigGenTime %i:%i)\n", (int)mode.trigGenTime,(int)testMode[testCase].trigGenTime));
				MSG_PRINT(("        trigGenDelay %i:%i)\n", (int)mode.trigGenDelay,(int)testMode[testCase].trigGenDelay));
				MSG_PRINT(("        trigWindow %i:%i)\n", (int)mode.trigWindow,(int)testMode[testCase].trigWindow));
				MSG_PRINT(("        trig2Signal %i:%i)\n", (int)mode.trig2Signal,(int)testMode[testCase].trig2Signal));
				MSG_PRINT(("        irlGenTime %i:%i)\n", (int)mode.irlGenTime,(int)testMode[testCase].irlGenTime));
				return MAD_FAIL;
		  }
	  }

#if 0
        MSG_PRINT((" (port %i, test case %i, mode %i:%i)\n",
                    (int)0,(int)testCase,(int)mode.mode,(int)testMode[testCase].mode));
        MSG_PRINT(("        trigGenAmt %i:%i)\n", (int)mode.trigGenAmt,(int)testMode[testCase].trigGenAmt));
        MSG_PRINT(("        pulseWidth %i:%i)\n", (int)mode.pulseWidth,(int)testMode[testCase].pulseWidth));
        MSG_PRINT(("        trigClkComp %i:%i)\n", (int)mode.trigClkComp,(int)testMode[testCase].trigClkComp));
#endif
    }

    MSG_PRINT(("Passed.\n\n"));
#if 1
    retVal = mdPtpSetEnable(dev,kpEn);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }
#endif
        
    return MAD_OK;
}


MAD_STATUS testTaiTSClkPer(MAD_DEV *dev)
{
    MAD_STATUS retVal;
    MAD_U32   testCase, nCases;
    MAD_U32    mode,testMode[2];
    MAD_BOOL en, kpEn;

    MSG_PRINT(("Test setting TAI TS Clock Per \n"));

#if 1
    retVal = mdPtpGetEnable(dev,&kpEn);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }

    retVal = mdPtpSetEnable(dev, MAD_TRUE);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }

    retVal = mdPtpGetEnable(dev,&en);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }

    if (en!=MAD_TRUE)
    {
      MSG_PRINT(("Can not enable PTP \n"));
      return MAD_FAIL;
    }
    else
      MSG_PRINT(("!!!!!!!! Enable PTP !!!!!!\n"));
#endif
    
    nCases = 2;
    testMode[0] = 0x5555;    

    testMode[1] = 0;    

    for(testCase=0; testCase<nCases; testCase++)
    {   
      retVal = madTaiSetTSClkPer(dev, testMode[testCase]);
      if (retVal != MAD_OK)
      {
        MSG_PRINT(("Setting for (port %i, test case %i) returned %x\n",(int)0,(int)testCase, retVal));
        testMADDisplayStatus(retVal);
        return retVal;             
      }
    
      retVal = madTaiGetTSClkPer(dev, &mode);
      if (retVal != MAD_OK)
      {
        MSG_PRINT(("Getting for (port %i, test case %i) returned %x",(int)0,(int)testCase, retVal));
        testMADDisplayStatus(retVal);
        return retVal;             
      }
            
        if (mode != testMode[testCase]) 
        {
            MSG_PRINT(("Unexpected return (port %i, test case %i, en %i:%i)\n",
                        (int)0,(int)testCase,(int)mode,(int)testMode[testCase]));
            return MAD_FAIL;
        }

#if 1
        MSG_PRINT((" (port %i, test case %i, mode %i:%i)\n",
                    (int)0,(int)testCase,(int)mode,(int)testMode[testCase]));
#endif
    }

    MSG_PRINT(("Passed.\n\n"));
#if 0
    retVal = mdPtpSetEnable(dev,kpEn);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }
#endif
        
    return MAD_OK;
}


MAD_STATUS testTaiMultiPTPSync(MAD_DEV *dev)
{
    MAD_STATUS retVal;
    MAD_U32   testCase, nCases;
    MAD_32    mode,testMode[3];
    MAD_BOOL multiEn, testMultiEn[3];
    MAD_BOOL en, kpEn;

    MSG_PRINT(("Test setting TAI Multi PTP Sync \n"));

#if 1
    retVal = mdPtpGetEnable(dev,&kpEn);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }

    retVal = mdPtpSetEnable(dev, MAD_TRUE);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }

    retVal = mdPtpGetEnable(dev,&en);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }

    if (en!=MAD_TRUE)
    {
      MSG_PRINT(("Can not enable PTP \n"));
      return MAD_FAIL;
    }
    else
      MSG_PRINT(("!!!!!!!! Enable PTP !!!!!!\n"));
#endif
    
    nCases = 3;
    testMultiEn[0] = MAD_TRUE;
    testMode[0] = 0x12345;    

    testMultiEn[1] = MAD_TRUE;
    testMode[1] = 0;    

    testMultiEn[2] = MAD_FALSE;
    testMode[2] = 0;    

    for(testCase=0; testCase<nCases; testCase++)
    {   
      retVal = madTaiSetMultiPTPSync(dev,testMultiEn[testCase], testMode[testCase]);
      if (retVal != MAD_OK)
      {
        MSG_PRINT(("Setting for (port %i, test case %i) returned %x\n",(int)0,(int)testCase, retVal));
        testMADDisplayStatus(retVal);
        return retVal;             
      }
    
      retVal = madTaiGetMultiPTPSync(dev, &multiEn, &mode);
      if (retVal != MAD_OK)
      {
        MSG_PRINT(("Getting for (port %i, test case %i) returned %x",(int)0,(int)testCase, retVal));
        testMADDisplayStatus(retVal);
        return retVal;             
      }
            
        if (multiEn != testMultiEn[testCase]) 
        {
            MSG_PRINT(("Unexpected return (port %i, test case %i, en %x:%x)\n",
                        (int)0,(int)testCase,(int)multiEn,(int)testMultiEn[testCase]));
            return MAD_FAIL;
        }

        if(mode != testMode[testCase])
      {
        MSG_PRINT(("Unexpected return (port %i, test case %i, syncTime %x:%x)\n",
                    (int)0,(int)testCase,(int)mode,(int)testMode[testCase]));
        return MAD_FAIL;
      }
#if 0
        MSG_PRINT((" (port %i, test case %i, syncTime %x:%x)\n",
                    (int)0,(int)testCase,(int)mode,(int)testMode[testCase]));
#endif
    }

    MSG_PRINT(("Passed.\n\n"));
#if 1
    retVal = mdPtpSetEnable(dev,kpEn);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }
#endif
        
    return MAD_OK;
}


MAD_STATUS testTaiTimeIncDec(MAD_DEV *dev)
{
    MAD_STATUS retVal;
    MAD_U32   testCase, nCases;
    MAD_32    mode,testMode[4];
    MAD_BOOL isInc, testIsInc[4], expired;
    MAD_BOOL en, kpEn;

    MSG_PRINT(("Test setting TAI Time IncDec \n"));

#if 1
    retVal = mdPtpGetEnable(dev,&kpEn);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }

    retVal = mdPtpSetEnable(dev, MAD_TRUE);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }

    retVal = mdPtpGetEnable(dev,&en);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }

    if (en!=MAD_TRUE)
    {
      MSG_PRINT(("Can not enable PTP \n"));
      return MAD_FAIL;
    }
    else
      MSG_PRINT(("!!!!!!!! Enable PTP !!!!!!\n"));
#endif
    
    nCases = 4;
    testIsInc[0] = MAD_TRUE;
    testMode[0] = 0x33;    

    testIsInc[1] = MAD_TRUE;
    testMode[1] = 0;    

    testIsInc[2] = MAD_FALSE;
    testMode[2] = 0x5a;    

    testIsInc[3] = MAD_FALSE;
    testMode[3] = 0;    

    for(testCase=0; testCase<nCases; testCase++)
    {   
      if (testIsInc[testCase]==MAD_TRUE) 
        retVal = madTaiSetTimeInc(dev, testMode[testCase]);
      else
        retVal = madTaiSetTimeDec(dev, testMode[testCase]);
      if (retVal != MAD_OK)
      {
        MSG_PRINT(("Setting for (port %i, test case %i) returned %x\n",(int)0,(int)testCase, retVal));
        testMADDisplayStatus(retVal);
        return retVal;             
      }
    
      retVal = madTaiGetTimeIncDec(dev, &expired, &isInc, &mode);
      if (retVal != MAD_OK)
      {
        MSG_PRINT(("Getting for (port %i, test case %i) returned %x",(int)0,(int)testCase, retVal));
        testMADDisplayStatus(retVal);
        return retVal;             
      }
            
        if (isInc != testIsInc[testCase]) 
        {
            MSG_PRINT(("Unexpected return (port %i, test case %i, en %x:%x)\n",
                        (int)0,(int)testCase,(int)isInc,(int)testIsInc[testCase]));
            return MAD_FAIL;
        }

        if(mode != testMode[testCase])
      {
        MSG_PRINT(("Unexpected return (port %i, test case %i, expired %x amount %x:%x)\n",
                    (int)0,(int)testCase,(int)expired, (int)mode,(int)testMode[testCase]));
        return MAD_FAIL;
      }
#if 0
      MSG_PRINT((" (port %i, test case %i, isInc %x:%x)\n",
                        (int)0,(int)testCase,(int)isInc,(int)testIsInc[testCase]));
      MSG_PRINT((" (port %i, test case %i, expired %x amount %x:%x)\n",
                    (int)0,(int)testCase,(int)expired,(int)mode,(int)testMode[testCase]));
#endif
    }

    MSG_PRINT(("Passed.\n\n"));
#if 1
    retVal = mdPtpSetEnable(dev,kpEn);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }
#endif
        
    return MAD_OK;
}

MAD_STATUS testPtpOutputSrc(MAD_DEV *dev)
{
    MAD_STATUS retVal;
    MAD_U32   testCase, nCases;
    MAD_BOOL    mode,testMode[2];

    MSG_PRINT(("Test setting PTP output source \n"));

    nCases = 2;
    testMode[0] = MAD_TRUE;    

    testMode[1] = MAD_FALSE;    


    for(testCase=0; testCase<nCases; testCase++)
    {   
        retVal = mdPtpSetOutputSrc(dev,testMode[testCase]);
        if (retVal != MAD_OK)
        {
            MSG_PRINT(("Setting for (port %i, test case %i) returned %x\n",(int)0,(int)testCase, retVal));
            testMADDisplayStatus(retVal);
            return retVal;             
        }
    
        retVal = mdPtpGetOutputSrc(dev,&mode);
        if (retVal != MAD_OK)
        {
            MSG_PRINT(("Getting for (port %i, test case %i) returned %x",(int)0,(int)testCase, retVal));
            testMADDisplayStatus(retVal);
            return retVal;             
        }
          
        if (mode != testMode[testCase]) 
        {
            MSG_PRINT(("Unexpected return (port %i, test case %i, OutputSrc %i:%i)\n",
                        (int)0,(int)testCase,(int)mode,(int)testMode[testCase]));
            return MAD_FAIL;
        }
    }

    MSG_PRINT(("Passed.\n\n"));
    
    return MAD_OK;
}

MAD_STATUS testPtpTsMode(MAD_DEV *dev)
{
    MAD_STATUS retVal;
    MAD_U32   testCase, nCases;
    MAD_PTP_TS_MODE    mode,testMode[3];
    MAD_BOOL en, kpEn;

    MSG_PRINT(("Test setting TS mode \n"));

#if 1
    retVal = mdPtpGetEnable(dev,&kpEn);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }

    retVal = mdPtpSetEnable(dev, MAD_TRUE);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }

    retVal = mdPtpGetEnable(dev,&en);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }

    if (en!=MAD_TRUE)
    {
      MSG_PRINT(("Can not enable PTP \n"));
      return MAD_FAIL;
    }
    else
      MSG_PRINT(("!!!!!!!! Enable PTP !!!!!!\n"));
#endif
    
    nCases = 3;
    testMode[0] = MAD_PTP_TS_MODE_IN_REG;    
    testMode[1] = MAD_PTP_TS_MODE_IN_RESERVED_2;    
    testMode[2] = MAD_PTP_TS_MODE_IN_FRAME_END;     

    for(testCase=0; testCase<nCases; testCase++)
    {   

      retVal = mdPtpSetTsMode(dev, testMode[testCase]);
      if (retVal != MAD_OK)
      {
        MSG_PRINT(("Setting for (port %i, test case %i) returned %x\n",(int)0,(int)testCase, retVal));
        testMADDisplayStatus(retVal);
        return retVal;             
      }
    
      retVal = mdPtpGetTsMode(dev, &mode);
      if (retVal != MAD_OK)
      {
        MSG_PRINT(("Getting for (port %i, test case %i) returned %x",(int)0,(int)testCase, retVal));
        testMADDisplayStatus(retVal);
        return retVal;             
      }
            
      if(mode != testMode[testCase])
      {
        MSG_PRINT(("Unexpected return (port %i, test case %i, amount %x:%x)\n",
                    (int)0,(int)testCase,(int)mode,(int)testMode[testCase]));
        return MAD_FAIL;
      }
    }

    MSG_PRINT(("Passed.\n\n"));
#if 1
    retVal = mdPtpSetEnable(dev,kpEn);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }
#endif
        
    return MAD_OK;
}

MAD_STATUS testPtpMeanPathDelay(MAD_DEV *dev)
{
    MAD_STATUS retVal;
    MAD_U32   testCase, nCases;
    MAD_U16    mode,testMode[2];
    MAD_BOOL en, kpEn;

    MSG_PRINT(("Test setting mean path delay \n"));

#if 1
    retVal = mdPtpGetEnable(dev,&kpEn);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }

    retVal = mdPtpSetEnable(dev, MAD_TRUE);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }

    retVal = mdPtpGetEnable(dev,&en);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }

    if (en!=MAD_TRUE)
    {
      MSG_PRINT(("Can not enable PTP \n"));
      return MAD_FAIL;
    }
    else
      MSG_PRINT(("!!!!!!!! Enable PTP !!!!!!\n"));
#endif
    
    nCases = 2;
    testMode[0] = 0xffff;    
    testMode[1] = 0;         

    for(testCase=0; testCase<nCases; testCase++)
    {   
      retVal = mdPtpSetMeanPathDelay(dev, testMode[testCase]);
      if (retVal != MAD_OK)
      {
        MSG_PRINT(("Setting for (port %i, test case %i) returned %x\n",(int)0,(int)testCase, retVal));
        testMADDisplayStatus(retVal);
        return retVal;             
      }
    
      retVal = mdPtpGetMeanPathDelay(dev, &mode);
      if (retVal != MAD_OK)
      {
        MSG_PRINT(("Getting for (port %i, test case %i) returned %x",(int)0,(int)testCase, retVal));
        testMADDisplayStatus(retVal);
        return retVal;             
      }
            
      if(mode != testMode[testCase])
      {
        MSG_PRINT(("Unexpected return (port %i, test case %i, amount %x:%x)\n",
                    (int)0,(int)testCase,(int)mode,(int)testMode[testCase]));
        return MAD_FAIL;
      }
    }

    MSG_PRINT(("Passed.\n\n"));
#if 1
    retVal = mdPtpSetEnable(dev,kpEn);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }
#endif
        
    return MAD_OK;
}

MAD_STATUS testPtpIngPathDelayAsym(MAD_DEV *dev)
{
    MAD_STATUS retVal;
    MAD_U32   testCase, nCases;
    MAD_U16    mode,testMode[3];
    MAD_BOOL en, kpEn;

    MSG_PRINT(("Test setting ingress path delay asymmetry. \n"));

#if 1
    retVal = mdPtpGetEnable(dev,&kpEn);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }

    retVal = mdPtpSetEnable(dev, MAD_TRUE);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }

    retVal = mdPtpGetEnable(dev,&en);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }

    if (en!=MAD_TRUE)
    {
      MSG_PRINT(("Can not enable PTP \n"));
      return MAD_FAIL;
    }
    else
      MSG_PRINT(("!!!!!!!! Enable PTP !!!!!!\n"));
#endif
    
    nCases = 3;
    testMode[0] = 0xffff;    
    testMode[1] = 0x7fff;
	testMode[2] = 0;

    for(testCase=0; testCase<nCases; testCase++)
    {   
      retVal = mdPtpSetIngPathDelayAsym(dev, testMode[testCase]);
      if (retVal != MAD_OK)
      {
        MSG_PRINT(("Setting for (port %i, test case %i) returned %x\n",(int)0,(int)testCase, retVal));
        testMADDisplayStatus(retVal);
        return retVal;             
      }
    
      retVal = mdPtpGetIngPathDelayAsym(dev, &mode);
      if (retVal != MAD_OK)
      {
        MSG_PRINT(("Getting for (port %i, test case %i) returned %x",(int)0,(int)testCase, retVal));
        testMADDisplayStatus(retVal);
        return retVal;             
      }
            
      if(mode != testMode[testCase])
      {
        MSG_PRINT(("Unexpected return (port %i, test case %i, amount %x:%x)\n",
                    (int)0,(int)testCase,(int)mode,(int)testMode[testCase]));
        return MAD_FAIL;
      }
    }

    MSG_PRINT(("Passed.\n\n"));
#if 1
    retVal = mdPtpSetEnable(dev,kpEn);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }
#endif
        
    return MAD_OK;
}

MAD_STATUS testPtpEgPathDelayAsym(MAD_DEV *dev)
{
    MAD_STATUS retVal;
    MAD_U32   testCase, nCases;
    MAD_U16    mode,testMode[3];
    MAD_BOOL en, kpEn;

    MSG_PRINT(("Test setting egress path delay asymmetry. \n"));

#if 1
    retVal = mdPtpGetEnable(dev,&kpEn);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }

    retVal = mdPtpSetEnable(dev, MAD_TRUE);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }

    retVal = mdPtpGetEnable(dev,&en);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }

    if (en!=MAD_TRUE)
    {
      MSG_PRINT(("Can not enable PTP \n"));
      return MAD_FAIL;
    }
    else
      MSG_PRINT(("!!!!!!!! Enable PTP !!!!!!\n"));
#endif
    
    nCases = 3;
    testMode[0] = 0xffff;    
    testMode[1] = 0x7fff;
	testMode[2] = 0;

    for(testCase=0; testCase<nCases; testCase++)
    {   
      retVal = mdPtpSetEgPathDelayAsym(dev, testMode[testCase]);
      if (retVal != MAD_OK)
      {
        MSG_PRINT(("Setting for (port %i, test case %i) returned %x\n",(int)0,(int)testCase, retVal));
        testMADDisplayStatus(retVal);
        return retVal;             
      }
    
      retVal = mdPtpGetEgPathDelayAsym(dev, &mode);
      if (retVal != MAD_OK)
      {
        MSG_PRINT(("Getting for (port %i, test case %i) returned %x",(int)0,(int)testCase, retVal));
        testMADDisplayStatus(retVal);
        return retVal;             
      }
            
      if(mode != testMode[testCase])
      {
        MSG_PRINT(("Unexpected return (port %i, test case %i, amount %x:%x)\n",
                    (int)0,(int)testCase,(int)mode,(int)testMode[testCase]));
        return MAD_FAIL;
      }
    }

    MSG_PRINT(("Passed.\n\n"));
#if 1
    retVal = mdPtpSetEnable(dev,kpEn);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }
#endif
        
    return MAD_OK;
}

MAD_STATUS testTaiClockSelect(MAD_DEV *dev)
{
    MAD_STATUS retVal;
    MAD_U32   testCase, nCases;
    MAD_TAI_CLOCK_SELECT    mode,testMode[2];
    MAD_BOOL en, kpEn;

    MSG_PRINT(("Test setting clock select. \n"));

#if 1
    retVal = mdPtpGetEnable(dev,&kpEn);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }

    retVal = mdPtpSetEnable(dev, MAD_TRUE);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }

    retVal = mdPtpGetEnable(dev,&en);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }

    if (en!=MAD_TRUE)
    {
      MSG_PRINT(("Can not enable PTP \n"));
      return MAD_FAIL;
    }
    else
      MSG_PRINT(("!!!!!!!! Enable PTP !!!!!!\n"));
#endif
    
    nCases = 2;
	testMode[0].priRecClkSel = 5;
	testMode[0].syncRecClkSel = 5;
	testMode[0].ptpExtClk = 1;

	testMode[1].priRecClkSel = 2;
	testMode[1].syncRecClkSel = 2;
	testMode[1].ptpExtClk = 0;
   

    for(testCase=0; testCase<nCases; testCase++)
    {   
      retVal = madTaiSetClockSelect(dev, &testMode[testCase]);
      if (retVal != MAD_OK)
      {
        MSG_PRINT(("Setting for (port %i, test case %i) returned %x\n",(int)0,(int)testCase, retVal));
        testMADDisplayStatus(retVal);
        return retVal;             
      }
    
      retVal = madTaiGetClockSelect(dev, &mode);
      if (retVal != MAD_OK)
      {
        MSG_PRINT(("Getting for (port %i, test case %i) returned %x",(int)0,(int)testCase, retVal));
        testMADDisplayStatus(retVal);
        return retVal;             
      }
	  if((mode.priRecClkSel != testMode[testCase].priRecClkSel) ||    
        (mode.syncRecClkSel != testMode[testCase].syncRecClkSel) ||    
        (mode.ptpExtClk != testMode[testCase].ptpExtClk) )
      {
        MSG_PRINT(("Unexpected return (port %i, test case %i, priRecClkSel %i:%i)\n",
                    (int)0,(int)testCase,(int)mode.priRecClkSel,(int)testMode[testCase].priRecClkSel));
        MSG_PRINT(("        syncRecClkSel %i:%i)\n", (int)mode.syncRecClkSel,(int)testMode[testCase].syncRecClkSel));
        MSG_PRINT(("        ptpExtClk %i:%i)\n", (int)mode.ptpExtClk,(int)testMode[testCase].ptpExtClk));
        return MAD_FAIL;
      }
        
    }

    MSG_PRINT(("Passed.\n\n"));
#if 1
    retVal = mdPtpSetEnable(dev,kpEn);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }
#endif
        
    return MAD_OK;
}

MAD_STATUS testTaiClockConfig(MAD_DEV *dev)
{
    MAD_STATUS retVal;
    MAD_U32   testCase, nCases;
    MAD_TAI_CLOCK_CONFIG    mode,testMode[2];
    MAD_BOOL en, kpEn;

    MSG_PRINT(("Test setting clock config. \n"));

#if 1
    retVal = mdPtpGetEnable(dev,&kpEn);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }

    retVal = mdPtpSetEnable(dev, MAD_TRUE);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }

    retVal = mdPtpGetEnable(dev,&en);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }

    if (en!=MAD_TRUE)
    {
      MSG_PRINT(("Can not enable PTP \n"));
      return MAD_FAIL;
    }
    else
      MSG_PRINT(("!!!!!!!! Enable PTP !!!!!!\n"));
#endif
    
    nCases = 2;
	testMode[0].irlClkEn = 1;
	testMode[0].compDir = 1;
	testMode[0].irlClkGenAmt = 0xffff;
	testMode[0].irlClkComp = 0x7fff;
	testMode[0].irlClkCompSubPs = 0xff;

	testMode[1].irlClkEn = 0;
	testMode[1].compDir = 0;
	testMode[1].irlClkGenAmt = 0;
	testMode[1].irlClkComp = 0;
	testMode[1].irlClkCompSubPs = 0;

    for(testCase=0; testCase<nCases; testCase++)
    {   
      retVal = madTaiSetClockConfig(dev, &testMode[testCase]);
      if (retVal != MAD_OK)
      {
        MSG_PRINT(("Setting for (port %i, test case %i) returned %x\n",(int)0,(int)testCase, retVal));
        testMADDisplayStatus(retVal);
        return retVal;             
      }
    
      retVal = madTaiGetClockConfig(dev, &mode);
      if (retVal != MAD_OK)
      {
        MSG_PRINT(("Getting for (port %i, test case %i) returned %x",(int)0,(int)testCase, retVal));
        testMADDisplayStatus(retVal);
        return retVal;             
      }
	  if((mode.irlClkEn != testMode[testCase].irlClkEn) ||
		(mode.compDir != testMode[testCase].compDir) ||    
        (mode.irlClkGenAmt != testMode[testCase].irlClkGenAmt) ||    
        (mode.irlClkComp != testMode[testCase].irlClkComp) ||
		(mode.irlClkCompSubPs != testMode[testCase].irlClkCompSubPs))
      {
        MSG_PRINT(("Unexpected return (port %i, test case %i, compDir %i:%i)\n",
                    (int)0,(int)testCase,(int)mode.compDir,(int)testMode[testCase].compDir));
		MSG_PRINT(("        irlClkEn %i:%i)\n", (int)mode.irlClkEn,(int)testMode[testCase].irlClkEn));
        MSG_PRINT(("        irlClkGenAmt %i:%i)\n", (int)mode.irlClkGenAmt,(int)testMode[testCase].irlClkGenAmt));
        MSG_PRINT(("        irlClkComp %i:%i)\n", (int)mode.irlClkComp,(int)testMode[testCase].irlClkComp));
		MSG_PRINT(("        irlClkCompSubPs %i:%i)\n", (int)mode.irlClkCompSubPs,(int)testMode[testCase].irlClkCompSubPs));
        return MAD_FAIL;
      }
    }

    MSG_PRINT(("Passed.\n\n"));
#if 1
    retVal = mdPtpSetEnable(dev,kpEn);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }
#endif
        
    return MAD_OK;
}

MAD_STATUS testTaiTrigLock(MAD_DEV *dev)
{
    MAD_STATUS retVal;
    MAD_U32   testCase, nCases;
    MAD_TAI_TRIG_LOCK_CONFIG    mode,testMode[2];
    MAD_BOOL en, kpEn;

    MSG_PRINT(("Test setting trigger lock. \n"));

#if 1
    retVal = mdPtpGetEnable(dev,&kpEn);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }

    retVal = mdPtpSetEnable(dev, MAD_TRUE);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }

    retVal = mdPtpGetEnable(dev,&en);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }

    if (en!=MAD_TRUE)
    {
      MSG_PRINT(("Can not enable PTP \n"));
      return MAD_FAIL;
    }
    else
      MSG_PRINT(("!!!!!!!! Enable PTP !!!!!!\n"));
#endif
    
    nCases = 2;
	testMode[0].trigLock = 0;
	testMode[0].trigLockRange = 5;

	testMode[1].trigLock = 1;
	testMode[1].trigLockRange = 5;

    for(testCase=0; testCase<nCases; testCase++)
    {   
      retVal = madTaiSetTrigLock(dev, &testMode[testCase]);
      if (retVal != MAD_OK)
      {
        MSG_PRINT(("Setting for (port %i, test case %i) returned %x\n",(int)0,(int)testCase, retVal));
        testMADDisplayStatus(retVal);
        return retVal;             
      }
    
      retVal = madTaiGetTrigLock(dev, &mode);
      if (retVal != MAD_OK)
      {
        MSG_PRINT(("Getting for (port %i, test case %i) returned %x",(int)0,(int)testCase, retVal));
        testMADDisplayStatus(retVal);
        return retVal;             
      }

//	  if((mode.trigLock != testMode[testCase].trigLock) ||    
	  if(mode.trigLockRange != testMode[testCase].trigLockRange)
      {
        MSG_PRINT(("Unexpected return (port %i, test case %i, trigLockRange %i:%i)\n",
                    (int)0,(int)testCase,(int)mode.trigLockRange,(int)testMode[testCase].trigLockRange));
        return MAD_FAIL;
      }
    }

    MSG_PRINT(("Passed.\n\n"));
#if 1
    retVal = mdPtpSetEnable(dev,kpEn);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }
#endif
        
    return MAD_OK;
}

MAD_STATUS testPtpMode(MAD_DEV *dev)
{
	MAD_STATUS retVal;
    MAD_U32   testCase, nCases;
    MAD_PTP_MODE_CONFIG    mode,testMode[2];
    MAD_BOOL en, kpEn;

    MSG_PRINT(("Test setting PTP mode. \n"));

#if 1
    retVal = mdPtpGetEnable(dev,&kpEn);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }

    retVal = mdPtpSetEnable(dev, MAD_TRUE);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }

    retVal = mdPtpGetEnable(dev,&en);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }

    if (en!=MAD_TRUE)
    {
      MSG_PRINT(("Can not enable PTP \n"));
      return MAD_FAIL;
    }
    else
      MSG_PRINT(("!!!!!!!! Enable PTP !!!!!!\n"));
#endif
    
    nCases = 2;
	testMode[0].tsAtSFD = 1;
	testMode[0].disPtp2Switch = 1;
	testMode[0].accelNewPTP = 1;
	testMode[0].altScheme = 1;
	testMode[0].grandMstr = 1;
	testMode[0].oneStep = 1;
	testMode[0].PTPMode = 1;

	testMode[1].tsAtSFD = 0;
	testMode[1].disPtp2Switch = 0;
	testMode[1].accelNewPTP = 0;
	testMode[1].altScheme = 0;
	testMode[1].grandMstr = 0;
	testMode[1].oneStep = 0;
	testMode[1].PTPMode = 0;	

    for(testCase=0; testCase<nCases; testCase++)
    {   
      retVal = mdPtpModeSet(dev, &testMode[testCase]);
      if (retVal != MAD_OK)
      {
        MSG_PRINT(("Setting for (port %i, test case %i) returned %x\n",(int)0,(int)testCase, retVal));
        testMADDisplayStatus(retVal);
        return retVal;             
      }
    
      retVal = mdPtpModeGet(dev, &mode);
      if (retVal != MAD_OK)
      {
        MSG_PRINT(("Getting for (port %i, test case %i) returned %x",(int)0,(int)testCase, retVal));
        testMADDisplayStatus(retVal);
        return retVal;             
      }
	  if((mode.tsAtSFD != testMode[testCase].tsAtSFD) ||    
		  (mode.disPtp2Switch != testMode[testCase].disPtp2Switch) ||
		  (mode.accelNewPTP != testMode[testCase].accelNewPTP) ||    
		  (mode.altScheme != testMode[testCase].altScheme) ||
		  (mode.grandMstr != testMode[testCase].grandMstr) ||
		  (mode.oneStep != testMode[testCase].oneStep) ||    
		  (mode.PTPMode != testMode[testCase].PTPMode))
      {
        MSG_PRINT(("Unexpected return (port %i, test case %i, tsAtSFD %i:%i)\n",
                    (int)0,(int)testCase,(int)mode.tsAtSFD,(int)testMode[testCase].tsAtSFD));
        MSG_PRINT(("        disPtp2Switch %i:%i)\n", (int)mode.disPtp2Switch,(int)testMode[testCase].disPtp2Switch));
		MSG_PRINT(("        accelNewPTP %i:%i)\n", (int)mode.accelNewPTP,(int)testMode[testCase].accelNewPTP));
		MSG_PRINT(("        AltScheme %i:%i)\n", (int)mode.altScheme,(int)testMode[testCase].altScheme));
		MSG_PRINT(("        GrandMstr %i:%i)\n", (int)mode.grandMstr,(int)testMode[testCase].grandMstr));
		MSG_PRINT(("        OneStep %i:%i)\n", (int)mode.oneStep,(int)testMode[testCase].oneStep));
		MSG_PRINT(("        PTPMode %i:%i)\n", (int)mode.PTPMode,(int)testMode[testCase].PTPMode));
        return MAD_FAIL;
      }
    }

    MSG_PRINT(("Passed.\n\n"));
#if 1
    retVal = mdPtpSetEnable(dev,kpEn);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }
#endif
        
    return MAD_OK;
}

MAD_STATUS testPtpExtTimeFormat(MAD_DEV *dev)
{
    MAD_STATUS retVal;
    MAD_U32   testCase, nCases;
    MAD_PTP_EXTERNAL_TIME_FORM    mode,testMode[2];
    MAD_BOOL en, kpEn;

    MSG_PRINT(("Test setting external time format. \n"));

#if 1
    retVal = mdPtpGetEnable(dev,&kpEn);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }

    retVal = mdPtpSetEnable(dev, MAD_TRUE);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }

    retVal = mdPtpGetEnable(dev,&en);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }

    if (en!=MAD_TRUE)
    {
      MSG_PRINT(("Can not enable PTP \n"));
      return MAD_FAIL;
    }
    else
      MSG_PRINT(("!!!!!!!! Enable PTP !!!!!!\n"));
#endif
    
    nCases = 2;
	testMode[0].intPTPTime = 1;
	testMode[0].extMask = 3;
	testMode[0].ext1722Time = 1;
	testMode[0].extTASel = 3;

	testMode[1].intPTPTime = 0;
	testMode[1].extMask = 0;
	testMode[1].ext1722Time = 0;
	testMode[1].extTASel = 0;

    for(testCase=0; testCase<nCases; testCase++)
    {   
      retVal = mdPtpSetExtTimeFormat(dev, &testMode[testCase]);
      if (retVal != MAD_OK)
      {
        MSG_PRINT(("Setting for (port %i, test case %i) returned %x\n",(int)0,(int)testCase, retVal));
        testMADDisplayStatus(retVal);
        return retVal;             
      }
    
      retVal = mdPtpGetExtTimeFormat(dev, &mode);
      if (retVal != MAD_OK)
      {
        MSG_PRINT(("Getting for (port %i, test case %i) returned %x",(int)0,(int)testCase, retVal));
        testMADDisplayStatus(retVal);
        return retVal;             
      }
	  if((mode.intPTPTime != testMode[testCase].intPTPTime) ||    
        (mode.extMask != testMode[testCase].extMask) ||
		(mode.ext1722Time != testMode[testCase].ext1722Time) ||    
        (mode.extTASel != testMode[testCase].extTASel))
      {
        MSG_PRINT(("Unexpected return (port %i, test case %i, intPTPTime %i:%i)\n",
                    (int)0,(int)testCase,(int)mode.intPTPTime,(int)testMode[testCase].intPTPTime));
        MSG_PRINT(("        extMask %i:%i)\n", (int)mode.extMask,(int)testMode[testCase].extMask));
		MSG_PRINT(("        ext1722Time %i:%i)\n", (int)mode.ext1722Time,(int)testMode[testCase].ext1722Time));
		MSG_PRINT(("        extTASel %i:%i)\n", (int)mode.extTASel,(int)testMode[testCase].extTASel));
        return MAD_FAIL;
      }
    }

    MSG_PRINT(("Passed.\n\n"));
#if 1
    retVal = mdPtpSetEnable(dev,kpEn);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }
#endif
        
    return MAD_OK;
}

MAD_STATUS testPtpIngActVec(MAD_DEV *dev)
{
    MAD_STATUS retVal;
    MAD_U32   testCase, testCase1, nCases;
    MAD_U32    mode,testMode[2];
	MAD_PTP_SPEC spec[2];
    MAD_BOOL en, kpEn;

    MSG_PRINT(("Test setting ingress action vectors. \n"));

#if 1
    retVal = mdPtpGetEnable(dev,&kpEn);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }

    retVal = mdPtpSetEnable(dev, MAD_TRUE);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }

    retVal = mdPtpGetEnable(dev,&en);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }

    if (en!=MAD_TRUE)
    {
      MSG_PRINT(("Can not enable PTP \n"));
      return MAD_FAIL;
    }
    else
      MSG_PRINT(("!!!!!!!! Enable PTP !!!!!!\n"));
#endif
    
    nCases = 2;
	testMode[0] = 0xffffff;
	testMode[1] = 0;

	spec[0] = MAD_PTP_IEEE_1588;
	spec[1] = MAD_PTP_IEEE_802_1AS;

    for(testCase=0; testCase<nCases; testCase++)
    {
		for(testCase1=0; testCase1<nCases; testCase1++)
		{
			retVal = mdPtpSetIngActVec(dev, spec[testCase1], testMode[testCase]);
			if (retVal != MAD_OK)
			{
				MSG_PRINT(("Setting for (port %i, spec %i, test case %i) returned %x\n",
					(int)0,(int)spec[testCase1],(int)testCase, retVal));
				testMADDisplayStatus(retVal);
				return retVal;             
			}
    
			retVal = mdPtpGetIngActVec(dev, spec[testCase1], &mode);
			if (retVal != MAD_OK)
			{
				MSG_PRINT(("Getting for (port %i, spec %i, test case %i) returned %x",
					(int)0,(int)spec[testCase1],(int)testCase, retVal));
				testMADDisplayStatus(retVal);
				return retVal;             
			}
            
			if(mode != testMode[testCase])
			{
				MSG_PRINT(("Unexpected return (port %i, spec %i, test case %i, amount %x:%x)\n",
                    (int)0,(int)spec[testCase1],(int)testCase,(int)mode,(int)testMode[testCase]));
				return MAD_FAIL;
			}
		}
    }

    MSG_PRINT(("Passed.\n\n"));
#if 1
    retVal = mdPtpSetEnable(dev,kpEn);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }
#endif
        
    return MAD_OK;
}

MAD_STATUS testPtpEgActVec(MAD_DEV *dev)
{
    MAD_STATUS retVal;
    MAD_U32   testCase, testCase1, nCases;
    MAD_PTP_EG_ACT_VEC    mode,testMode[2];
	MAD_PTP_SPEC spec[2];
    MAD_BOOL en, kpEn;

    MSG_PRINT(("Test setting egress action vectors. \n"));

#if 1
    retVal = mdPtpGetEnable(dev,&kpEn);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }

    retVal = mdPtpSetEnable(dev, MAD_TRUE);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }

    retVal = mdPtpGetEnable(dev,&en);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }

    if (en!=MAD_TRUE)
    {
      MSG_PRINT(("Can not enable PTP \n"));
      return MAD_FAIL;
    }
    else
      MSG_PRINT(("!!!!!!!! Enable PTP !!!!!!\n"));
#endif
    
    nCases = 2;
	testMode[0] = 0x7007eb67;
	testMode[1] = 0;

	spec[0] = MAD_PTP_IEEE_1588;
	spec[1] = MAD_PTP_IEEE_802_1AS;

    for(testCase=0; testCase<nCases; testCase++)
    {
		for(testCase1=0; testCase1<nCases; testCase1++)
		{
			retVal = mdPtpSetEgActVec(dev, spec[testCase1], testMode[testCase]);
			if (retVal != MAD_OK)
			{
				MSG_PRINT(("Setting for (port %i, spec %i, test case %i) returned %x\n",
					(int)0,(int)spec[testCase1],(int)testCase, retVal));
				testMADDisplayStatus(retVal);
				return retVal;             
			}
    
			retVal = mdPtpGetEgActVec(dev, spec[testCase1], &mode);
			if (retVal != MAD_OK)
			{
				MSG_PRINT(("Getting for (port %i, spec %i, test case %i) returned %x",
					(int)0,(int)spec[testCase1],(int)testCase, retVal));
				testMADDisplayStatus(retVal);
				return retVal;             
			}
            
			if(mode != testMode[testCase])
			{
				MSG_PRINT(("Unexpected return (port %i, spec %i, test case %i, amount %x:%x)\n",
                    (int)0,(int)spec[testCase1],(int)testCase,(int)mode,(int)testMode[testCase]));
			return MAD_FAIL;
			}
		}
    }

    MSG_PRINT(("Passed.\n\n"));
#if 1
    retVal = mdPtpSetEnable(dev,kpEn);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }
#endif
        
    return MAD_OK;
}

MAD_STATUS testPtpTODLoadPoint(MAD_DEV *dev)
{
    MAD_STATUS retVal;
    MAD_U32   testCase, nCases;
    MAD_U32   mode,testMode[2];
    MAD_BOOL en, kpEn;

    MSG_PRINT(("Test setting TOD load point. \n"));

#if 1
    retVal = mdPtpGetEnable(dev,&kpEn);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }

    retVal = mdPtpSetEnable(dev, MAD_TRUE);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }

    retVal = mdPtpGetEnable(dev,&en);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }

    if (en!=MAD_TRUE)
    {
      MSG_PRINT(("Can not enable PTP \n"));
      return MAD_FAIL;
    }
    else
      MSG_PRINT(("!!!!!!!! Enable PTP !!!!!!\n"));
#endif
    
    nCases = 2;
	testMode[0] = 0xffffffff;
	testMode[1] = 0;

    for(testCase=0; testCase<nCases; testCase++)
    {   
      retVal = mdPtpTODLoadPointSet(dev, testMode[testCase]);
      if (retVal != MAD_OK)
      {
        MSG_PRINT(("Setting for (port %i, test case %i) returned %x\n",(int)0,(int)testCase, retVal));
        testMADDisplayStatus(retVal);
        return retVal;             
      }
    
      retVal = mdPtpTODLoadPointGet(dev, &mode);
      if (retVal != MAD_OK)
      {
        MSG_PRINT(("Getting for (port %i, test case %i) returned %x",(int)0,(int)testCase, retVal));
        testMADDisplayStatus(retVal);
        return retVal;             
      }
            
      if(mode != testMode[testCase])
      {
        MSG_PRINT(("Unexpected return (port %i, test case %i, amount %x:%x)\n",
                    (int)0,(int)testCase,(int)mode,(int)testMode[testCase]));
        return MAD_FAIL;
      }
    }

    MSG_PRINT(("Passed.\n\n"));
#if 1
    retVal = mdPtpSetEnable(dev,kpEn);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }
#endif
        
    return MAD_OK;
}

MAD_STATUS testPtpTODOpCtrl(MAD_DEV *dev)
{
    MAD_STATUS retVal;
    MAD_U32   testCase, nCases;
    MAD_PTP_TOD_OP_CTRL   mode,testMode[2];
    MAD_BOOL en, kpEn;

    MSG_PRINT(("Test setting TOD opeartion control. \n"));

#if 1
    retVal = mdPtpGetEnable(dev,&kpEn);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }

    retVal = mdPtpSetEnable(dev, MAD_TRUE);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }

    retVal = mdPtpGetEnable(dev,&en);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }

    if (en!=MAD_TRUE)
    {
      MSG_PRINT(("Can not enable PTP \n"));
      return MAD_FAIL;
    }
    else
      MSG_PRINT(("!!!!!!!! Enable PTP !!!!!!\n"));
#endif
    
    nCases = 2;
	testMode[0].ptpOpCtrlTodOp = 0x1;
	testMode[0].ptpOpCtrlTimeArray = 0x3;
	testMode[0].ptpOpCtrlClkValid = 1;
	testMode[0].ptpOpCtrlDomainNum = 0xff;

	testMode[1].ptpOpCtrlTodOp = 0;
	testMode[1].ptpOpCtrlTimeArray = 0;
	testMode[1].ptpOpCtrlClkValid = 0;
	testMode[1].ptpOpCtrlDomainNum = 0;

    for(testCase=0; testCase<nCases; testCase++)
    {   
      retVal = mdPtpTODOpCtrlSet(dev, &testMode[testCase]);
      if (retVal != MAD_OK)
      {
        MSG_PRINT(("Setting for (port %i, test case %i) returned %x\n",(int)0,(int)testCase, retVal));
        testMADDisplayStatus(retVal);
        return retVal;             
      }
    
      retVal = mdPtpTODOpCtrlGet(dev, &mode);
      if (retVal != MAD_OK)
      {
        MSG_PRINT(("Getting for (port %i, test case %i) returned %x",(int)0,(int)testCase, retVal));
        testMADDisplayStatus(retVal);
        return retVal;             
      }

	  if((mode.ptpOpCtrlTodOp != testMode[testCase].ptpOpCtrlTodOp) ||    
        (mode.ptpOpCtrlTimeArray != testMode[testCase].ptpOpCtrlTimeArray) ||
		(mode.ptpOpCtrlClkValid != testMode[testCase].ptpOpCtrlClkValid) ||    
        (mode.ptpOpCtrlDomainNum != testMode[testCase].ptpOpCtrlDomainNum))
      {
        MSG_PRINT(("Unexpected return (port %i, test case %i, ptpOpCtrlTodOp %i:%i)\n",
                    (int)0,(int)testCase,(int)mode.ptpOpCtrlTodOp,(int)testMode[testCase].ptpOpCtrlTodOp));
        MSG_PRINT(("        ptpOpCtrlTimeArray %i:%i)\n", (int)mode.ptpOpCtrlTimeArray,(int)testMode[testCase].ptpOpCtrlTimeArray));
		MSG_PRINT(("        ptpOpCtrlClkValid %i:%i)\n", (int)mode.ptpOpCtrlClkValid,(int)testMode[testCase].ptpOpCtrlClkValid));
		MSG_PRINT(("        ptpOpCtrlDomainNum %i:%i)\n", (int)mode.ptpOpCtrlDomainNum,(int)testMode[testCase].ptpOpCtrlDomainNum));
        return MAD_FAIL;
      }
    }

    MSG_PRINT(("Passed.\n\n"));
#if 1
    retVal = mdPtpSetEnable(dev,kpEn);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }
#endif
        
    return MAD_OK;
}

MAD_STATUS testPtpPulse(MAD_DEV *dev)
{
    MAD_STATUS retVal;
    MAD_U32   testCase, nCases;
    MAD_PTP_PULSE_STRUCT   mode,testMode[2];
    MAD_BOOL en, kpEn;

    MSG_PRINT(("Test setting pulse parameters. \n"));

#if 1
    retVal = mdPtpGetEnable(dev,&kpEn);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }

    retVal = mdPtpSetEnable(dev, MAD_TRUE);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }

    retVal = mdPtpGetEnable(dev,&en);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }

    if (en!=MAD_TRUE)
    {
      MSG_PRINT(("Can not enable PTP \n"));
      return MAD_FAIL;
    }
    else
      MSG_PRINT(("!!!!!!!! Enable PTP !!!!!!\n"));
#endif
    
    nCases = 2;
	testMode[0].ptpPulseWidth = 0xf;
	testMode[0].ptp1ppsWidthRange = 7;
	testMode[0].ptp1ppsPhase = 1;
	testMode[0].ptp1ppsSelect = 3;

	testMode[1].ptpPulseWidth = 0;
	testMode[1].ptp1ppsWidthRange = 0;
	testMode[1].ptp1ppsPhase = 0;
	testMode[1].ptp1ppsSelect = 0;

    for(testCase=0; testCase<nCases; testCase++)
    {   
      retVal = mdPtpPulseSet(dev, &testMode[testCase]);
      if (retVal != MAD_OK)
      {
        MSG_PRINT(("Setting for (port %i, test case %i) returned %x\n",(int)0,(int)testCase, retVal));
        testMADDisplayStatus(retVal);
        return retVal;             
      }
    
      retVal = mdPtpPulseGet(dev, &mode);
      if (retVal != MAD_OK)
      {
        MSG_PRINT(("Getting for (port %i, test case %i) returned %x",(int)0,(int)testCase, retVal));
        testMADDisplayStatus(retVal);
        return retVal;             
      }
	  if((mode.ptpPulseWidth != testMode[testCase].ptpPulseWidth) ||    
        (mode.ptp1ppsWidthRange != testMode[testCase].ptp1ppsWidthRange) ||
		(mode.ptp1ppsPhase != testMode[testCase].ptp1ppsPhase) ||    
        (mode.ptp1ppsSelect != testMode[testCase].ptp1ppsSelect))
      {
        MSG_PRINT(("Unexpected return (port %i, test case %i, ptpPulseWidth %i:%i)\n",
                    (int)0,(int)testCase,(int)mode.ptpPulseWidth,(int)testMode[testCase].ptpPulseWidth));
        MSG_PRINT(("        ptp1ppsWidthRange %i:%i)\n", (int)mode.ptp1ppsWidthRange,(int)testMode[testCase].ptp1ppsWidthRange));
		MSG_PRINT(("        ptp1ppsPhase %i:%i)\n", (int)mode.ptp1ppsPhase,(int)testMode[testCase].ptp1ppsPhase));
		MSG_PRINT(("        ptp1ppsSelect %i:%i)\n", (int)mode.ptp1ppsSelect,(int)testMode[testCase].ptp1ppsSelect));
        return MAD_FAIL;
      }
    }

    MSG_PRINT(("Passed.\n\n"));
#if 1
    retVal = mdPtpSetEnable(dev,kpEn);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }
#endif
        
    return MAD_OK;
}

MAD_STATUS testPtpTODTime(MAD_DEV *dev)
{
    MAD_STATUS retVal;
    MAD_U32   testCase, nCases;
    MAD_PTP_TS_STRUCT   mode,testMode[2];
    MAD_BOOL en, kpEn;

    MSG_PRINT(("Test setting TOD time. \n"));

#if 1
    retVal = mdPtpGetEnable(dev,&kpEn);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }

    retVal = mdPtpSetEnable(dev, MAD_TRUE);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }

    retVal = mdPtpGetEnable(dev,&en);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }

    if (en!=MAD_TRUE)
    {
      MSG_PRINT(("Can not enable PTP \n"));
      return MAD_FAIL;
    }
    else
      MSG_PRINT(("!!!!!!!! Enable PTP !!!!!!\n"));
#endif
    
    nCases = 2;
	testMode[0].nanoseconds = 0xffffffff;
	testMode[0].seconds = 0xffffffff;
	testMode[0].topseconds = 0xffff;

	testMode[1].nanoseconds = 0;
	testMode[1].seconds = 0;
	testMode[1].topseconds = 0;

    for(testCase=0; testCase<nCases; testCase++)
    {   
      retVal = mdPtpTODTimeSet(dev, &testMode[testCase]);
      if (retVal != MAD_OK)
      {
        MSG_PRINT(("Setting for (port %i, test case %i) returned %x\n",(int)0,(int)testCase, retVal));
        testMADDisplayStatus(retVal);
        return retVal;             
      }
    
      retVal = mdPtpTODTimeGet(dev, &mode);
      if (retVal != MAD_OK)
      {
        MSG_PRINT(("Getting for (port %i, test case %i) returned %x",(int)0,(int)testCase, retVal));
        testMADDisplayStatus(retVal);
        return retVal;             
      }
	  if((mode.nanoseconds != testMode[testCase].nanoseconds) ||    
        (mode.seconds != testMode[testCase].seconds) ||
		(mode.topseconds != testMode[testCase].topseconds))
      {
        MSG_PRINT(("Unexpected return (port %i, test case %i, nanoseconds %i:%i)\n",
                    (int)0,(int)testCase,(int)mode.nanoseconds,(int)testMode[testCase].nanoseconds));
        MSG_PRINT(("        seconds %i:%i)\n", (int)mode.seconds,(int)testMode[testCase].seconds));
		MSG_PRINT(("        topseconds %i:%i)\n", (int)mode.topseconds,(int)testMode[testCase].topseconds));
        return MAD_FAIL;
      }
    }

    MSG_PRINT(("Passed.\n\n"));
#if 1
    retVal = mdPtpSetEnable(dev,kpEn);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }
#endif
        
    return MAD_OK;
}

MAD_STATUS testPtp1722Time(MAD_DEV *dev)
{
    MAD_STATUS retVal;
    MAD_U32   testCase, nCases;
    unsigned long long   mode,testMode[2];
    MAD_BOOL en, kpEn;

    MSG_PRINT(("Test setting 1722 TOD time. \n"));

#if 1
    retVal = mdPtpGetEnable(dev,&kpEn);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }

    retVal = mdPtpSetEnable(dev, MAD_TRUE);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }

    retVal = mdPtpGetEnable(dev,&en);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }

    if (en!=MAD_TRUE)
    {
      MSG_PRINT(("Can not enable PTP \n"));
      return MAD_FAIL;
    }
    else
      MSG_PRINT(("!!!!!!!! Enable PTP !!!!!!\n"));
#endif
    
    nCases = 2;
	testMode[0] = 0xffffffff;
	testMode[1] = 0;

    for(testCase=0; testCase<nCases; testCase++)
    {   
      retVal = mdPtp1722TimeSet(dev, testMode[testCase]);
      if (retVal != MAD_OK)
      {
        MSG_PRINT(("Setting for (port %i, test case %i) returned %x\n",(int)0,(int)testCase, retVal));
        testMADDisplayStatus(retVal);
        return retVal;             
      }
    
      retVal = mdPtp1722TimeGet(dev, &mode);
      if (retVal != MAD_OK)
      {
        MSG_PRINT(("Getting for (port %i, test case %i) returned %x",(int)0,(int)testCase, retVal));
        testMADDisplayStatus(retVal);
        return retVal;             
      }
            
      if(mode != testMode[testCase])
      {
        MSG_PRINT(("Unexpected return (port %i, test case %i, amount %x:%x)\n",
                    (int)0,(int)testCase,(int)mode,(int)testMode[testCase]));
        return MAD_FAIL;
      }
    }

    MSG_PRINT(("Passed.\n\n"));
#if 1
    retVal = mdPtpSetEnable(dev,kpEn);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }
#endif
        
    return MAD_OK;
}

MAD_STATUS testPtpTODComp(MAD_DEV *dev)
{
    MAD_STATUS retVal;
    MAD_U32   testCase, nCases;
    MAD_U32   mode,testMode[2];
    MAD_BOOL en, kpEn;

    MSG_PRINT(("Test setting TOD compensation. \n"));

#if 1
    retVal = mdPtpGetEnable(dev,&kpEn);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }

    retVal = mdPtpSetEnable(dev, MAD_TRUE);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }

    retVal = mdPtpGetEnable(dev,&en);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }

    if (en!=MAD_TRUE)
    {
      MSG_PRINT(("Can not enable PTP \n"));
      return MAD_FAIL;
    }
    else
      MSG_PRINT(("!!!!!!!! Enable PTP !!!!!!\n"));
#endif
    
    nCases = 2;
	testMode[0] = 0xffffffff;
	testMode[1] = 0;

    for(testCase=0; testCase<nCases; testCase++)
    {   
      retVal = mdPtpTODCompSet(dev, testMode[testCase]);
      if (retVal != MAD_OK)
      {
        MSG_PRINT(("Setting for (port %i, test case %i) returned %x\n",(int)0,(int)testCase, retVal));
        testMADDisplayStatus(retVal);
        return retVal;             
      }
    
      retVal = mdPtpTODCompGet(dev, &mode);
      if (retVal != MAD_OK)
      {
        MSG_PRINT(("Getting for (port %i, test case %i) returned %x",(int)0,(int)testCase, retVal));
        testMADDisplayStatus(retVal);
        return retVal;             
      }
            
      if(mode != testMode[testCase])
      {
        MSG_PRINT(("Unexpected return (port %i, test case %i, amount %x:%x)\n",
                    (int)0,(int)testCase,(int)mode,(int)testMode[testCase]));
        return MAD_FAIL;
      }
    }

    MSG_PRINT(("Passed.\n\n"));
#if 1
    retVal = mdPtpSetEnable(dev,kpEn);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }
#endif
        
    return MAD_OK;
}

MAD_U32 testSyncClkSetClkSelect(MAD_DEV *dev)
{
    MAD_STATUS  retVal;
    MAD_LPORT   port;
    MAD_U32     testCase, testCase1, nCases, nCases1;
    MAD_U8    refClk,testRefClk[2];
    MAD_U8    testInterf[3];

    MSG_PRINT(("Testing Sync clocking set reference clock...\n"));

      nCases = 2;
      testRefClk[0] = MAD_SYNC_CLK_REF_CLK_XTAL;
      testRefClk[1] = MAD_SYNC_CLK_REF_CLK_SCLK;
      nCases1 = 3;
      testInterf[0] = MAD_SYNC_CLK_MODE_PORT_COPPER;
      testInterf[1] = MAD_SYNC_CLK_MODE_PORT_125SERDES;
      testInterf[2] = MAD_SYNC_CLK_MODE_PORT_QSGMII;

    for(port=0; port<dev->numOfPorts; port++)
    {
      for(testCase1=0; testCase1<nCases1; testCase1++)
      {   
        for(testCase=0; testCase<nCases; testCase++)
        {   
            retVal = mdSyncClkSetClkSelect(dev,port,testInterf[testCase1],testRefClk[testCase]);
            if (retVal != MAD_OK)
            {
		     	if((dev->deviceId==MAD_SW1680)&&(retVal==MAD_API_ERROR_OVER_PORT_RANGE)) break;
                MSG_PRINT(("Setting for (port %i, test case %i) returned ",(int)port,(int)testCase));
                testMADDisplayStatus(retVal);
                return retVal;             
            }
    
            retVal = mdSyncClkGetClkSelect(dev,port,testInterf[testCase1],&refClk);
            if (retVal != MAD_OK)
            {
		     	if((dev->deviceId==MAD_SW1680)&&(retVal==MAD_API_ERROR_OVER_PORT_RANGE)) break;
                MSG_PRINT(("Getting for (port %i, test case %i:%i) returned ",(int)port,(int)testCase1,(int)testCase));
                testMADDisplayStatus(retVal);
                return retVal;             
            }
           
            if (refClk != testRefClk[testCase]) 
            {
                MSG_PRINT(("Unexpected return (port %i, test case %i:%i, interface %i, refClk %i:%i)\n",
                            (int)port,(int)testCase1,(int)testCase,(int)testInterf[testCase1],
                            (int)refClk,(int)testRefClk[testCase]));
                return MAD_FAIL;
            }
        }
      }
    }

    MSG_PRINT(("Passed.\n\n"));
    return MAD_OK;
}

MAD_U32 testSyncClkSetRClk(MAD_DEV *dev)
{
    MAD_STATUS  retVal = MAD_OK;
    MAD_LPORT   port;
    MAD_U32     testCase, testCase1, nCases, nCases1;
    MAD_U8    rclk,testRclk[2];
    MAD_U8    freq,testFreq[2];

    MSG_PRINT(("Testing Sync Clocking Set Recoverd clock...\n"));

      nCases = 4;
      testRclk[0] = MAD_SYNC_CLK_RCLK1;
      testRclk[1] = MAD_SYNC_CLK_RCLK2;
      testRclk[2] = MAD_SYNC_CLK_RCLK1_2;
      testRclk[3] = MAD_SYNC_CLK_RCLK_NO;
     nCases1 = 2;
      testFreq[0] = MAD_SYNC_CLK_FREQ_25M;
      testFreq[1] = MAD_SYNC_CLK_FREQ_125M;

    for(port=0; port<dev->numOfPorts; port++)
    {
      for(testCase=0; testCase<nCases; testCase++)
      {   
        for(testCase1=0; testCase1<nCases1; testCase1++)
        {   
            retVal = mdSyncClkSetRClk(dev,port,testRclk[testCase],testFreq[testCase1]);
            if (retVal != MAD_OK)
            {
		     	if((dev->deviceId==MAD_SW1680)&&(retVal==MAD_API_ERROR_OVER_PORT_RANGE)) break;
                MSG_PRINT(("Setting for (port %i, test case %i) returned ",(int)port,(int)testCase));
                testMADDisplayStatus(retVal);
                return retVal;             
            }
    
            retVal = mdSyncClkGetRClk(dev,port,&rclk,&freq);
            if (retVal != MAD_OK)
            {
		     	if((dev->deviceId==MAD_SW1680)&&(retVal==MAD_API_ERROR_OVER_PORT_RANGE)) break;
                MSG_PRINT(("Getting for (port %i, test case %i) returned ",(int)port,(int)testCase));
                testMADDisplayStatus(retVal);
                return retVal;             
            }
            
            if ((rclk != testRclk[testCase]) || (freq != testFreq[testCase1]))
            {
                MSG_PRINT(("Unexpected return (port %i, test case %i:%i, rclk %i:%i, freq %i:%i)\n",
                            (int)port,(int)testCase,(int)testCase1,(int)rclk,(int)testRclk[testCase],
                            (int)freq,(int)testFreq[testCase1]));
                return MAD_FAIL;
            }
        }
      }
    }

    MSG_PRINT(("Passed.\n\n"));
    return MAD_OK;
}

MAD_STATUS testXmdioSetClkStoppable(MAD_DEV *dev)
{
    MAD_STATUS  retVal = MAD_OK;
    MAD_U32   testCase, nCases;
    MAD_BOOL    mode,testMode[2];
    MAD_LPORT   port;

    MSG_PRINT(("Test XMDIO Clock stoppable enable \n"));

    nCases = 2;
    testMode[0] = MAD_TRUE;    

    testMode[1] = MAD_FALSE;    

  for(port=0; port<dev->numOfPorts; port++)
  {
    for(testCase=0; testCase<nCases; testCase++)
    {   
        retVal = madXmdioSetClkStoppable(dev, port, testMode[testCase]);
        if (retVal != MAD_OK)
        {
		     	if((dev->deviceId==MAD_SW1680)&&(retVal==MAD_API_ERROR_OVER_PORT_RANGE)) break;
            MSG_PRINT(("Setting for (port %i, test case %i) returned %x\n",(int)0,(int)testCase, retVal));
            testMADDisplayStatus(retVal);
            return retVal;             
        }
    
        retVal = madXmdioGetClkStoppable(dev, port, &mode);
        if (retVal != MAD_OK)
        {
		     	if((dev->deviceId==MAD_SW1680)&&(retVal==MAD_API_ERROR_OVER_PORT_RANGE)) break;
            MSG_PRINT(("Getting for (port %i, test case %i) returned %x",(int)0,(int)testCase, retVal));
            testMADDisplayStatus(retVal);
            return retVal;             
        }
          
        if (mode != testMode[testCase]) 
        {
            MSG_PRINT(("Unexpected return (port %i, test case %i, en %i:%i)\n",
                        (int)0,(int)testCase,(int)mode,(int)testMode[testCase]));
            return MAD_FAIL;
        }
    }
  }

  MSG_PRINT(("Passed.\n\n"));    
    
  return MAD_OK;
}

/* test WOL */
MAD_U32 testWolEnable(MAD_DEV *dev)
{
  MAD_STATUS  retVal;
  MAD_LPORT   port;
  MAD_U32     testCase, nCases;
  MAD_U16    wolType,testWolType[8];

  MSG_PRINT(("Testing WOL type enable...\n"));

  nCases = 8;
  testWolType[0] = MAD_PHY_WOL_TYPE_PKT;
  testWolType[1] = 0;
  testWolType[2] = MAD_PHY_WOL_TYPE_MAGIC;
  testWolType[3] = 0;
  testWolType[4] = MAD_PHY_WOL_TYPE_LINKUP;
  testWolType[5] = 0;
  testWolType[6] = MAD_PHY_WOL_TYPE_PKT | MAD_PHY_WOL_TYPE_MAGIC;
  testWolType[7] = 0;

  for(port=0; port<dev->numOfPorts; port++)
  {
    for(testCase=0; testCase<nCases; testCase++)
    {   
      retVal = madWolEnable(dev,port,testWolType[testCase]);
      if (retVal != MAD_OK)
      {
        MSG_PRINT(("Setting for (port %i, test case %i) returned ",(int)port,(int)testCase));
        testMADDisplayStatus(retVal);
        return retVal;             
      }
    
      retVal = madWolGetEnable(dev,port,&wolType);
      if (retVal != MAD_OK)
      {
        MSG_PRINT(("Getting for (port %i, test case %i) returned ",(int)port,(int)testCase));
        testMADDisplayStatus(retVal);
        return retVal;             
      }
            
      if ((wolType != testWolType[testCase]))
      {
        MSG_PRINT(("Unexpected return (port %i, test case %i, wolType %i:%i)\n",
                   (int)port,(int)testCase,(int)wolType,(int)testWolType[testCase]));
        return MAD_FAIL;
      }
    }
  }

  MSG_PRINT(("Passed.\n\n"));
  return MAD_OK;
}

static void displayPktConfig(MAD_WOL_PKT_MATCH_STRUCT *pktConfig)
{
  int i;
  /* Sram packet number(0-7); */
  MSG_PRINT(("pktConfig->sramPktNum %d\n",pktConfig->sramPktNum));
  /* Enable; */
  MSG_PRINT(("pktConfig->enable %d\n",pktConfig->enable));
  /* tpktConfig1->matchStatus;   Real time enable status; */
  /* Sram Packet Match length; */
  MSG_PRINT(("pktConfig->sramPktLength %d\n",pktConfig->sramPktLength));
/*  pktConfig1->sramPacket[128][2];  Sram packet data: 128 X [enable][byte] */
  for(i=0; i<128; i++)
  {
    MSG_PRINT(("pktConfig->sramPacket[i].enable %01x,",pktConfig->sramPacket[i].enable));
    MSG_PRINT(("pktConfig->sramPacket[i].byte %01x  \n",pktConfig->sramPacket[i].byte));
  }
  MSG_PRINT(("\n"));
}

static MAD_BOOL checkPktConfig(MAD_WOL_PKT_MATCH_STRUCT *pktConfig1, MAD_WOL_PKT_MATCH_STRUCT *pktConfig2)
{
  int i;
  /* Sram packet number(0-7); */
  if(pktConfig1->sramPktNum != pktConfig2->sramPktNum) return MAD_FALSE;
  /* Enable; */
  if(pktConfig1->enable != pktConfig2->enable) return MAD_FALSE;      
  /* tpktConfig1->matchStatus;   Real time enable status; */
  /* Sram Packet Match length; */
  if(pktConfig1->sramPktLength != pktConfig2->sramPktLength) return MAD_FALSE; 
/*  pktConfig1->sramPacket[128][2];  Sram packet data: 128 X [enable][byte] */
  for(i=0; i<128; i++)
  {
    if(pktConfig1->sramPacket[i].enable != pktConfig2->sramPacket[i].enable) return MAD_FALSE;
    if(pktConfig1->sramPacket[i].byte != pktConfig2->sramPacket[i].byte) return MAD_FALSE;
  }
  return MAD_TRUE;
}

MAD_U32 testWolPktConfig(MAD_DEV *dev)
{
  MAD_STATUS  retVal;
  MAD_LPORT   port;
  MAD_U32     testCase, nCases;
  MAD_WOL_PKT_MATCH_STRUCT    pktConfig, testPktConfig[4];
  int i;

  MSG_PRINT(("Testing WOL packet configuration...\n"));

  nCases = 4;
  testPktConfig[0].sramPktNum = 0;   /* Sram packet number(0-7); */
  testPktConfig[0].enable     = 1;       /* Enable; */
  /* testPktConfig[0].matchStatus;   Real time enable status; */
  testPktConfig[0].sramPktLength = 64; /* Sram Packet Match length; */
/*  testPktConfig[0].sramPacket[128][2];  Sram packet data: 128 X [enable][byte] */
  for(i=0; i<128; i++)
  {
    testPktConfig[0].sramPacket[i].enable = 1;
    testPktConfig[0].sramPacket[i].byte = i+0x10;
  }

  testPktConfig[1].sramPktNum = 1;   /* Sram packet number(0-7); */
  testPktConfig[1].enable     = 1;       /* Enable; */
  /* testPktConfig[1].matchStatus;   Real time enable status; */
  testPktConfig[1].sramPktLength = 32; /* Sram Packet Match length; */
/*  testPktConfig[1].sramPacket[128][2];  Sram packet data: 128 X [enable][byte] */
  for(i=0; i<128; i++)
  {
    testPktConfig[1].sramPacket[i].enable = 1;
    testPktConfig[1].sramPacket[i].byte = i+128;
  }

  testPktConfig[2].sramPktNum = 3;   /* Sram packet number(0-7); */
  testPktConfig[2].enable     = 1;       /* Enable; */
  /* testPktConfig[2].matchStatus;   Real time enable status; */
  testPktConfig[2].sramPktLength = 16; /* Sram Packet Match length; */
/*  testPktConfig[2].sramPacket[128][2];  Sram packet data: 128 X [enable][byte] */
  for(i=0; i<128; i++)
  {
    testPktConfig[2].sramPacket[i].enable = 1;
    testPktConfig[2].sramPacket[i].byte = (i%2)?0x55:0xaa;
  }

  testPktConfig[3].sramPktNum = 7;   /* Sram packet number(0-7); */
  testPktConfig[3].enable     = 1;       /* Enable; */
  /* testPktConfig[3].matchStatus;   Real time enable status; */
  testPktConfig[3].sramPktLength = 127; /* Sram Packet Match length; */
/*  testPktConfig[3].sramPacket[128][2];  Sram packet data: 128 X [enable][byte] */
  for(i=0; i<128; i++)
  {
    testPktConfig[3].sramPacket[i].enable = 1;
    testPktConfig[3].sramPacket[i].byte = (i%2)?0xaa:0x55;
  }

  for(port=0; port<dev->numOfPorts; port++)
  {
    for(testCase=0; testCase<nCases; testCase++)
    {   
      retVal = madSetWolPktConfig(dev,port,&testPktConfig[testCase]);
      if (retVal != MAD_OK)
      {
        MSG_PRINT(("Setting for (port %i, test case %i) returned ",(int)port,(int)testCase));
        testMADDisplayStatus(retVal);
        return retVal;             
      }
      pktConfig.sramPktNum = testPktConfig[testCase].sramPktNum;   /* Sram packet number(0-7); */
      retVal = madGetWolPktConfig(dev,port,&pktConfig);
      if (retVal != MAD_OK)
      {
        MSG_PRINT(("Getting for (port %i, test case %i) returned ",(int)port,(int)testCase));
        testMADDisplayStatus(retVal);
        return retVal;             
      }
            
      if (checkPktConfig(&pktConfig, &testPktConfig[testCase]) != MAD_TRUE)
      {
        MSG_PRINT(("Unexpected return (port %i, test case %i)\n", (int)port,(int)testCase));
        displayPktConfig(&testPktConfig[testCase]);
        displayPktConfig(&pktConfig);
        return MAD_FAIL;
      }
	  else
	  {
        MSG_PRINT(("test passed (port %i, test case %i)\n", (int)port,(int)testCase));
	  }
    }
  }

  MSG_PRINT(("Passed.\n\n"));
  return MAD_OK;
}

MAD_U32 testWolMagicPktConfig(MAD_DEV *dev)
{
  MAD_STATUS  retVal;
  MAD_LPORT   port;
  MAD_U32     testCase, nCases;
  MAD_U8    destAddr[6], testDestAddr[4][6];
  int i;

  MSG_PRINT(("Testing WOL packet configuration...\n"));

  nCases = 4;
  for(i=0; i<6; i++)
  {
    testDestAddr[0][i]= i;
  }
  for(i=0; i<6; i++)
  {
    testDestAddr[1][i]= i*0x10;
  }
  for(i=0; i<6; i++)
  {
    testDestAddr[2][i]= i+0x50;
  }
  for(i=0; i<6; i++)
  {
    testDestAddr[3][i]= i+0xa0;
  }

  for(port=0; port<dev->numOfPorts; port++)
  {
    for(testCase=0; testCase<nCases; testCase++)
    {   
      retVal = madSetWolMagicPktConfig(dev,port,testDestAddr[testCase]);
      if (retVal != MAD_OK)
      {
        MSG_PRINT(("Setting for (port %i, test case %i) returned ",(int)port,(int)testCase));
        testMADDisplayStatus(retVal);
        return retVal;             
      }
    
      retVal = madGetWolMagicPktConfig(dev,port,destAddr);
      if (retVal != MAD_OK)
      {
        MSG_PRINT(("Getting for (port %i, test case %i) returned ",(int)port,(int)testCase));
        testMADDisplayStatus(retVal);
        return retVal;             
      }
            
      if (memcmp(destAddr, testDestAddr[testCase], 6))
      {
        MSG_PRINT(("Unexpected return (port %i, test case %i)\n", (int)port,(int)testCase));
        MSG_PRINT(("got destaddr %02x:%02x:%02x:%02x:%02x:%02x\n", 
			destAddr[0],
			destAddr[1],
			destAddr[2],
			destAddr[3],
			destAddr[4],
			destAddr[5]));
        MSG_PRINT(("test destaddr %02x:%02x:%02x:%02x:%02x:%02x\n", 
			testDestAddr[testCase][0],
			testDestAddr[testCase][1],
			testDestAddr[testCase][2],
			testDestAddr[testCase][3],
			testDestAddr[testCase][4],
			testDestAddr[testCase][5]));
        return MAD_FAIL;
      }
    }
  }

  MSG_PRINT(("Passed.\n\n"));
  return MAD_OK;
}

MAD_STATUS testWol10BLowPwSel(MAD_DEV *dev)
{
    MAD_STATUS retVal;
    MAD_U32   testCase, nCases;
    MAD_BOOL    select,testSelect[2];
    MAD_LPORT   port;

    MSG_PRINT(("Test XMDIO Clock stoppable enable \n"));

    nCases = 2;
    testSelect[0] = MAD_TRUE;    

    testSelect[1] = MAD_FALSE;    

  for(port=0; port<dev->numOfPorts; port++)
  {
    for(testCase=0; testCase<nCases; testCase++)
    {   
        retVal = madWol10BLowPwSel(dev, port, testSelect[testCase]);
        if (retVal != MAD_OK)
        {
            MSG_PRINT(("Setting for (port %i, test case %i) returned %x\n",(int)0,(int)testCase, retVal));
            testMADDisplayStatus(retVal);
            return retVal;             
        }
    
        retVal = madWol10BLowPwGet(dev, port, &select);
        if (retVal != MAD_OK)
        {
            MSG_PRINT(("Getting for (port %i, test case %i) returned %x",(int)0,(int)testCase, retVal));
            testMADDisplayStatus(retVal);
            return retVal;             
        }
          
        if (select != testSelect[testCase]) 
        {
            MSG_PRINT(("Unexpected return (port %i, test case %i, en %i:%i)\n",
                        (int)0,(int)testCase,(int)select,(int)testSelect[testCase]));
            return MAD_FAIL;
        }
    }
  }

  MSG_PRINT(("Passed.\n\n"));    
    
  return MAD_OK;
}


MAD_STATUS testMAD(MAD_DEV *dev)
{
  testMADCtrl(dev);
  testTempInt(dev);
  testQSGMIIPwD(dev);
  testLinkDownDelay(dev);
  testLinkDownDelayEn(dev);
  testMadLED(dev);
  testCollAdj(dev);
  testSyncFifo(dev);
  testPowerDownEn(dev);
  testQSGMIIPowerDownEn(dev);
  testMACPowerDownEn(dev);
  testDetectPowerDownMode(dev);
  testEEELPIMode(dev);
  testEEELPIInterrupt(dev);
  testEEELPIInterruptMask(dev);
  testEEELPIIgrBufThresh(dev);
  testEEELPIEEEBufThresh(dev);
  testEEELPIIgrDscThresh(dev);
  testUniDirTransEn(dev);
  testPatPRBS(dev);
  testPat125PRBS(dev);
  if((dev->deviceId==MAD_88E1340M)
    ||(dev->deviceId==MAD_88E1540)
	||(dev->deviceId==MAD_88E151x))
  {
    testPtpEnable(dev);
	testPtpRefClkSrc(dev);
	testPtpInputSrc(dev);
    testPtpIntEnable(dev);
    testPtpConfig(dev);
    testPtpGlobalConfig(dev);
    testPtpPortConfig(dev);
    testPtpPTPEn(dev);
    testTaiEventConfig(dev);
    testTaiTrigConfig(dev);
    testTaiTSClkPer(dev);
    testTaiMultiPTPSync(dev);
    testTaiTimeIncDec(dev);
  }
#if 1

  if (dev->deviceId==MAD_88E151x)
  {
	testPtpOutputSrc(dev);
	testPtpTsMode(dev);
	testPtpMeanPathDelay(dev);
	testPtpIngPathDelayAsym(dev);
	testPtpEgPathDelayAsym(dev);
	testTaiClockSelect(dev);
	testTaiClockConfig(dev);
	testTaiTrigLock(dev);
	testPtpMode(dev);
	testPtpExtTimeFormat(dev);
	/* testPtpIngActVec(dev); */ 
	testPtpEgActVec(dev);
	testPtpTODLoadPoint(dev);
	testPtpTODOpCtrl(dev);
	testPtpPulse(dev);
	testPtpTODTime(dev);
	testPtp1722Time(dev);
	testPtpTODComp(dev);
  }
 
  testSyncClkSetRClk(dev);
  testSyncClkSetClkSelect(dev);
  if((dev->deviceId==MAD_88E1680)||(dev->deviceId==MAD_88E1680M))
  {
    testXmdioSetClkStoppable(dev);
  }
  testSnoop(dev);
  testAutoSelect(dev);
  testPreferredMedis(dev);
  testMediaMode(dev);

  testWolPktConfig(dev); 
  testWolMagicPktConfig(dev);
  testWol10BLowPwSel(dev);
  testWolEnable(dev);
#endif

  testPTP(dev);  /* latest switch PTP */
  testTAI(dev);  /* latest switch PTP */
	
  return MAD_OK;
}

