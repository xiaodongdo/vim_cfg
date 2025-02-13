
#include <pthread.h>
#include <string.h>
#include "modulemgr.h"
#include "insight.h"
#include "json.h"
#include "cJSON.h"
#include "log_api.h"


/* Module info structure - this contains modules information
 * that are registered with module manager                  */
typedef struct _module_info_ {
    bool                     isInUse;
    insight_module_feature_info_t  moduleData;
} INSIGHT_MODULE_INFO_t;

/* Module managers local data used to keep different modules information */
INSIGHT_MODULE_INFO_t   moduleData[INSIGHT_MAX_FEATURES];
/* Read-Write lock for protection */
pthread_rwlock_t            moduleMgrRWLock;
/* Module managers debug flag */
int moduleMgrDebugFlag = 1;

/* Macro used for printing the log messages*/
#define MODULE_MANAGER_DEBUG_PRINT(severity,format, args...)         \
                              if (moduleMgrDebugFlag)                \
                              {                                      \
                                  LOG_POST(severity,format, ##args); \
                              }


/* Macro to acquire read lock */
#define MODULE_MANAGER_RWLOCK_RD_LOCK(lock)                          \
           if (pthread_rwlock_rdlock(&lock) != 0)                    \
           {                                                         \
               MODULE_MANAGER_DEBUG_PRINT(INSIGHT_LOG_ERROR,           \
			   "(%s:%d) Failed to take Module manager"               \
               "read write lock for read\n", __FILE__, __LINE__);    \
               return INSIGHT_STATUS_FAILURE;                          \
           }


/* Macro to acquire write lock */
#define MODULE_MANAGER_RWLOCK_WR_LOCK(lock)                          \
           if (pthread_rwlock_wrlock(&lock) != 0)                    \
           {                                                         \
               MODULE_MANAGER_DEBUG_PRINT(INSIGHT_LOG_ERROR,           \
			   "(%s:%d) Failed to take Module manager"               \
               "read write lock for write\n", __FILE__, __LINE__);   \
               return INSIGHT_STATUS_FAILURE;                          \
           }

/* Macro to release RW lock */
#define MODULE_MANAGER_RWLOCK_UNLOCK(lock)                           \
           if (pthread_rwlock_unlock(&lock) != 0)                    \
           {                                                         \
               MODULE_MANAGER_DEBUG_PRINT(INSIGHT_LOG_ERROR,           \
			   "(%s:%d) Failed to release Module manager"            \
               "read write lock          \n", __FILE__, __LINE__);   \
               return INSIGHT_STATUS_FAILURE;                          \
           }


/*********************************************************************
* @brief       Set debug flag for Module Manager
*
* @param[in]  val      Debug flag
*
* @retval   none
*
*
* @note    none
*
*********************************************************************/
void module_mgr_debug_set(int val)
{
    moduleMgrDebugFlag = val;
}

/*********************************************************************
* @brief       Get api string from Json buffer
*
* @param[in]  jsonBuffer       Json Buffer
* @param[out]  apiString       api String
*
* @retval   INSIGHT_STATUS_FAILURE     Not able to get api string from
*                                    Json buffer
* @retval   INSIGHT_STATUS_SUCCESS     When api string is successfully
*                                    decoded from Json buffer
*
* @retval   INSIGHT_STATUS_INVALID_JSON    JSON is malformatted, or doesn't
*                                         have necessary data.
* @retval   INSIGHT_STATUS_INVALID_PARAMETER  Invalid input parameter
*
*
* @note    none
*
*********************************************************************/
static INSIGHT_STATUS json_buf_to_api_string_get(char *jsonBuffer, char *apiString)
{
    cJSON *json_method;
    cJSON *root;
    INSIGHT_STATUS status = INSIGHT_STATUS_SUCCESS;

    /* Validating 'jsonBuffer' */
    JSON_VALIDATE_POINTER(jsonBuffer,"jsonBuffer",INSIGHT_STATUS_INVALID_PARAMETER);

    /* Parse JSON to a C-JSON root */
    root = cJSON_Parse(jsonBuffer);
    JSON_VALIDATE_JSON_POINTER(root,"root",INSIGHT_STATUS_INVALID_JSON);

    /* Parsing and Validating 'method' from JSON buffer */
    json_method = cJSON_GetObjectItem(root,"method");
    JSON_VALIDATE_JSON_POINTER(json_method,"method",INSIGHT_STATUS_INVALID_JSON);
    JSON_VALIDATE_JSON_AS_STRING(json_method,"method",INSIGHT_STATUS_INVALID_JSON);

    /* Copy the string, with a limit on max characters */
    strncpy (&apiString[0], json_method->valuestring, JSON_MAX_NODE_LENGTH-1);
    return status;
}
/*********************************************************************
* @brief       Initialize module manager data with default values
*
* @retval   INSIGHT_STATUS_FAILURE   if failed to initialize data
* @retval   INSIGHT_STATUS_SUCCESS   Module manager is successfully initialized
*
*
* @note    none
*
*********************************************************************/
INSIGHT_STATUS modulemgr_init()
{
    INSIGHT_STATUS rv = INSIGHT_STATUS_SUCCESS;

    /* Set local data to 0x00 */
    memset(moduleData, 0x00, sizeof(moduleData));
    /* Initialize Read Write lock with default attributes */
    if (pthread_rwlock_init(&moduleMgrRWLock, NULL) != 0)
    {
          MODULE_MANAGER_DEBUG_PRINT(INSIGHT_LOG_ERROR,
                          "(%s:%d) Failed to create Module manager read write"
                          "lock with deafault attributes\n",
                                                           __FILE__, __LINE__);
          rv = INSIGHT_STATUS_FAILURE;
    }
    return rv;
}

/*********************************************************************
* @brief       Register a feature with Module manager, all the modules
*              such as BSt and packet trace need to register api handlers
*              with module manager.
*
* @param[in]  featureInfo        Feature information
*
* @retval   INSIGHT_STATUS_FAILURE     If failed to acquire lock
*
* @retval   INSIGHT_STATUS_TABLE_FULL  INSIGHT_MAX_FEATURES number of modules
*                                    already registered
*
* @retval   INSIGHT_STATUS_DUPLICATE   If a module is already registered
*                                    with same feature ID
*
* @retval   INSIGHT_STATUS_SUCCESS     Upon registering successfully with
*                                    Module Manager
*
*
* @note    none
*
*********************************************************************/
INSIGHT_STATUS modulemgr_register(insight_module_feature_info_t * featureInfoPtr)
{
    int   moduleIndex = 0;
	int   freeEntryIndex = 0;
    bool  freeEntryFound = false;
    bool  entryAlreadyPresent = false;
    INSIGHT_STATUS rv = INSIGHT_STATUS_SUCCESS;
    insight_module_feature_info_t featureInfo = *featureInfoPtr;

    /* Acquire write lock */
    MODULE_MANAGER_RWLOCK_WR_LOCK(moduleMgrRWLock);

    /* Loop through module list to find out the free/entry to be replaced. */
    /* This will loop till moduleIndex = INSIGHT_MAX_FEATURES   or            */
    /*  a module entry with the same feature is already  present.          */
    for (moduleIndex = 0; moduleIndex < INSIGHT_MAX_FEATURES; moduleIndex++)
    {
       /* Check for free entry, if it is not yet found */
       if ((moduleData[moduleIndex].isInUse == false) &&
		                          (freeEntryFound == false))
       {
           freeEntryFound = true;
           freeEntryIndex = moduleIndex;
       } /* Check whether a module is already registered with this feature ID*/
       else if (moduleData[moduleIndex].moduleData.featureId
                                                 == featureInfo.featureId)
       {
          MODULE_MANAGER_DEBUG_PRINT(INSIGHT_LOG_ERROR,
                             "(%s:%d) Module manager has already a module",
                             "registered with feature ID = %d\n", __FILE__,
                                             __LINE__, featureInfo.featureId);
          entryAlreadyPresent = true;
          break;
       }
    }

    /* First check for duplicate entry */
    if (entryAlreadyPresent == true)
    {
        rv = INSIGHT_STATUS_DUPLICATE;
    }
    else if ((moduleIndex == INSIGHT_MAX_FEATURES) && (freeEntryFound == false))
    {
     /* To know whether a table is full or not, need to check freeEntryFound flag
        also as the "above for loop" goes through all entries in search of
        a duplicate entry. so moduleIndex does not alone provide table full/not */
        MODULE_MANAGER_DEBUG_PRINT(INSIGHT_LOG_ERROR,
                                  "(%s:%d) Module manager has already a module",
                                  "registered with feature ID = %d\n",
                                     __FILE__, __LINE__, featureInfo.featureId);
        rv = INSIGHT_STATUS_TABLE_FULL;
    }
    else
    {
        MODULE_MANAGER_DEBUG_PRINT(INSIGHT_LOG_INFO,
                 "(%s:%d) Found free entry at index %d, placing modules info\n",
                                            __FILE__, __LINE__, freeEntryIndex);
        moduleData[freeEntryIndex].isInUse = true;
        moduleData[freeEntryIndex].moduleData = featureInfo;
        rv = INSIGHT_STATUS_SUCCESS;

	}
    /* Release RW lock */
    MODULE_MANAGER_RWLOCK_UNLOCK(moduleMgrRWLock);
    return rv;
}


/*********************************************************************
* @brief     When a REST API is received, the web server thread obtains
*            the associated handler using this API
*
* @param[in]  jsonBuffer       Json Buffer
* @param[in]  bufLength        Length of the Buffer
* @param[out]  handler          Function handler
*
* @retval   INSIGHT_STATUS_FAILURE     Unable to find function handler
*                                     for the api string in Json buffer
* @retval   INSIGHT_STATUS_SUCCESS     Function handler is found
*                                     for the api string in Json buffer
*
*
* @retval   INSIGHT_STATUS_INVALID_JSON    JSON is malformatted, or doesn't
*                                         have necessary data.
* @retval   INSIGHT_STATUS_INVALID_PARAMETER  Invalid input parameter
*
*
*
* @note    none
*
*********************************************************************/
INSIGHT_STATUS modulemgr_rest_api_handler_get(char * jsonBuffer,
                                            int bufLength,
                                            INSIGHT_REST_API_HANDLER_t *handler)
{
    INSIGHT_STATUS rv;
    int   moduleIndex = 0;
    int   apiMapIndex = 0;
    bool  entryFound = false;
    INSIGHT_REST_API_t  *moduleApiListPtr;
    char  apiString[JSON_MAX_NODE_LENGTH] = {0};

    /* Get api string from json buffer */
    if ((rv = json_buf_to_api_string_get(jsonBuffer, apiString))
                                                 != INSIGHT_STATUS_SUCCESS)
    {
        MODULE_MANAGER_DEBUG_PRINT(INSIGHT_LOG_ERROR,
                      "(%s:%d) Failed to get api string from json buffer, rv = %d\n",
                                                       __FILE__, __LINE__, rv);
        return rv;
    }

    MODULE_MANAGER_DEBUG_PRINT(INSIGHT_LOG_INFO,
                   "(%s:%d) Api string in json buffer is %s\n",
                                            __FILE__, __LINE__, apiString);

    /* Acquire read lock */
    MODULE_MANAGER_RWLOCK_RD_LOCK(moduleMgrRWLock);

    /* Loop through the module list to find out correct module */
    for (moduleIndex = 0; (moduleIndex < INSIGHT_MAX_FEATURES); moduleIndex++)
    {
        moduleApiListPtr = moduleData[moduleIndex].moduleData.restApiList;

        /* Loop through the API list to find out the correct handler */
        for (apiMapIndex = 0; apiMapIndex < INSIGHT_MAX_API_CMDS_PER_FEATURE;
                                                                 apiMapIndex++)
        {
            /* Validate apiString in module api list */
            if (moduleApiListPtr[apiMapIndex].apiString == NULL)
            {
                continue;
            }

            /* Check with the API string */
            if (strcmp(moduleApiListPtr[apiMapIndex].apiString, apiString) == 0)
            { /* api string is matched */
                if (moduleApiListPtr[apiMapIndex].handler != NULL)
                {
                    entryFound = true;
                    *handler = moduleApiListPtr[apiMapIndex].handler;
                }
                else
                {
                    MODULE_MANAGER_DEBUG_PRINT(INSIGHT_LOG_ERROR,
                      "(%s:%d) Handler for api string %s is not present/NULL\n",
                                                 __FILE__, __LINE__, apiString);
                    rv = INSIGHT_STATUS_FAILURE;
                }
                break;
            }
        }
        /* If entry found then break out of main for loop */
        if (entryFound == true)
        {
            MODULE_MANAGER_DEBUG_PRINT(INSIGHT_LOG_INFO,
                                "(%s:%d) Handler for api string %s is found\n",
                                __FILE__, __LINE__, apiString);

            break;
        }
    }


    if (entryFound == false)
    {
        MODULE_MANAGER_DEBUG_PRINT(INSIGHT_LOG_ERROR,
                          "(%s:%d) Failed to find handler for api string %s\n",
                                                __FILE__, __LINE__, apiString);
        rv = INSIGHT_STATUS_FAILURE;
    }

    /* Release read lock */
    MODULE_MANAGER_RWLOCK_UNLOCK(moduleMgrRWLock);
    return rv;
}

/*********************************************************************
* @brief     Utility api to get the feature name for the
*            rest  API  method
*
* @param[in]  apiString       api method string
* @param[out]  handler          Function handler
*
* @retval   INSIGHT_STATUS_FAILURE     Unable to find feature name
*                                     for the api string
* @retval   INSIGHT_STATUS_SUCCESS     Feature name is found
*                                     for the api string
*
*
* @retval   INSIGHT_STATUS_INVALID_PARAMETER  Invalid input parameter
*
*
*
* @note    none
*
*********************************************************************/
INSIGHT_STATUS modulemgr_rest_api_feature_name_get(char * apiString,
                                            char *featureName)
{
    INSIGHT_STATUS rv = INSIGHT_STATUS_SUCCESS;
    int   moduleIndex = 0;
    int   apiMapIndex = 0;
    bool  entryFound = false;
    INSIGHT_REST_API_t  *moduleApiListPtr;


    MODULE_MANAGER_DEBUG_PRINT(INSIGHT_LOG_INFO,
                   "(%s:%d) Api string in json buffer is %s\n",
                                            __FILE__, __LINE__, apiString);

    /* Acquire read lock */
    MODULE_MANAGER_RWLOCK_RD_LOCK(moduleMgrRWLock);

    /* Loop through the module list to find out correct module */
    for (moduleIndex = 0; (moduleIndex < INSIGHT_MAX_FEATURES); moduleIndex++)
    {
        moduleApiListPtr = moduleData[moduleIndex].moduleData.restApiList;

        /* Loop through the API list to find out the correct handler */
        for (apiMapIndex = 0; apiMapIndex < INSIGHT_MAX_API_CMDS_PER_FEATURE;
                                                                 apiMapIndex++)
        {
            /* Validate apiString in module api list */
            if (moduleApiListPtr[apiMapIndex].apiString == NULL)
            {
                continue;
            }

            /* Check with the API string */
            if (strcmp(moduleApiListPtr[apiMapIndex].apiString, apiString) == 0)
            { /* api string is matched */
                    entryFound = true;
                if (strlen(moduleData[moduleIndex].moduleData.featureName) != 0)
                {
                    strncpy(featureName, &moduleData[moduleIndex].moduleData.featureName[0], strlen(moduleData[moduleIndex].moduleData.featureName));
                }
                else
                {
                    MODULE_MANAGER_DEBUG_PRINT(INSIGHT_LOG_INFO,
                      "(%s:%d) Feature name for api string %s is not present/NULL\n",
                                                 __FILE__, __LINE__, apiString);
                      featureName = NULL;
                }
                break;
            }
        }
        /* If entry found then break out of main for loop */
        if (entryFound == true)
        {
            MODULE_MANAGER_DEBUG_PRINT(INSIGHT_LOG_INFO,
                                "(%s:%d) Feature name for api string %s is found\n",
                                __FILE__, __LINE__, apiString);
            break;
        }
    }


    if (entryFound == false)
    {
        MODULE_MANAGER_DEBUG_PRINT(INSIGHT_LOG_ERROR,
                          "(%s:%d) Failed to find handler for api string %s\n",
                                                __FILE__, __LINE__, apiString);
        rv = INSIGHT_STATUS_FAILURE;
    }

    /* Release read lock */
    MODULE_MANAGER_RWLOCK_UNLOCK(moduleMgrRWLock);
    return rv;
}

