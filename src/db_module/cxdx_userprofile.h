
#ifndef __CX_DX_USERPROFILE_H__
#define	__CX_DX_USERPROFILE_H__

#include "dbmodule.h"
#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>


#define TAG_IMS_SUBSCRIPTION            BAD_CAST"IMSSubscription"
#define TAG_PRIVATE_ID                  BAD_CAST"PrivateID"
#define TAG_SERVICE_PROFILE             BAD_CAST"ServiceProfile"
#define TAG_PUBLIC_IDENTITY             BAD_CAST"PublicIdentity"
#define TAG_IFC                         BAD_CAST"InitialFilterCriteria"
#define TAG_CORE_NETWORK_SERVICE_AUTH   BAD_CAST"CoreNetworkServicesAuthorization"
#define TAG_EXTENSION                   BAD_CAST"Extension"
#define TAG_SHARED_IFC_SET_ID           BAD_CAST"SharedIFCSetID"
#define TAG_BARING_INDICATION           BAD_CAST"BarringIndication"
#define TAG_IDENTITY                    BAD_CAST"Identity"
#define TAG_PRIORITY                    BAD_CAST"Priority"
#define TAG_TRIGGER_POINT               BAD_CAST"TriggerPoint"
#define TAG_APPLICATION_SERVER          BAD_CAST"ApplicationServer"
#define TAG_PROFILE_PART_INDICATOR      BAD_CAST"ProfilePartIndicator"
#define TAG_CONDITION_TYPE_CNF          BAD_CAST"ConditionTypeCNF"
#define TAG_SPT                         BAD_CAST"SPT"
#define TAG_CONDITION_NEGATED           BAD_CAST"ConditionNegated"
#define TAG_GROUP                       BAD_CAST"Group"
#define TAG_REQUEST_URI                 BAD_CAST"RequestURI"
#define TAG_METHOD                      BAD_CAST"Method"
#define TAG_SIP_HEADER                  BAD_CAST"SIPHeader"
#define TAG_SESSION_CASE                BAD_CAST"SessionCase"
#define TAG_SESSION_DESCRIPTION         BAD_CAST"SessionDescription"
#define TAG_REGISTRATION_TYPE           BAD_CAST"RegistrationType"
#define TAG_HEADER                      BAD_CAST"Header"
#define TAG_CONTENT                     BAD_CAST"Content"
#define TAG_LINE                        BAD_CAST"Line"
#define TAG_SERVER_NAME                 BAD_CAST"ServerName"
#define TAG_DEFAULT_HANDLING            BAD_CAST"DefaultHandling"
#define TAG_SERVICE_INFO                BAD_CAST"ServiceInfo"
#define TAG_IDENTITY_TYPE               BAD_CAST"IdentityType"
#define TAG_WILDCARDED_PSI              BAD_CAST"WildcardedPSI"
#define TAG_WILDCAEDED_IMPU             BAD_CAST"WildcardedIMPU"
#define TAG_DISPLAY_NAME                BAD_CAST"DisplayName"
#define TAG_ALIAS_IDENTITY_GROUP_ID     BAD_CAST"AliasIdentityGroupID"
#define TAG_LIST_OF_SERVICE_IDS         BAD_CAST"ListOfServiceIds"
#define TAG_SERVICE_ID                  BAD_CAST"ServiceId"
#define TAG_SUBSCRIBED_MEDIA_PRO_ID     BAD_CAST"SubscribedMediaProfileId"

#define USER_PROFILE_ENCODING           "ISO-8859-1"


#ifdef __cplusplus
extern "C" {
#endif


t_Db_HSSReturnVal CxDx_GetUserProfile(HDBHSSTHREADCONTEXT hdbHSSThreadCtxt_io,
                                      unsigned char *pucUserID,
                                      unsigned char **ppucUserProfile);

t_Db_HSSReturnVal CxDx_AddServiceProfileTags(HDBHSSTHREADCONTEXT hdbHSSThreadCtxt_io,
                                             xmlTextWriterPtr pWriter,
                                             unsigned char    *pucUserID);

t_Db_HSSReturnVal CxDx_AddPublicIdTags(HDBHSSTHREADCONTEXT hdbHSSThreadCtxt_io,
                                       xmlTextWriterPtr pWriter,
                                       unsigned int     unServiceProfile_ID);

t_Db_HSSReturnVal CxDx_AddIFCTags(HDBHSSTHREADCONTEXT hdbHSSThreadCtxt_io,
                                  xmlTextWriterPtr pWriter,
                                  unsigned int     unServiceProfile_ID);

t_Db_HSSReturnVal CxDx_AddTriggerPointTag(HDBHSSTHREADCONTEXT hdbHSSThreadCtxt_io,
                                          xmlTextWriterPtr pWriter,
                                          unsigned int     unTriggerPoint_ID);

t_Db_HSSReturnVal CxDx_AddCoreNetworkServiceAuthTag(HDBHSSTHREADCONTEXT hdbHSSThreadCtxt_io,
                                                    xmlTextWriterPtr pWriter,
                                                    unsigned int     unCNSA_ID);

t_Db_HSSReturnVal CxDx_AddExtensionTag(HDBHSSTHREADCONTEXT hdbHSSThreadCtxt_io,
                                       xmlTextWriterPtr pWriter,
                                       unsigned int     unExt_ID);


#ifdef __cplusplus
}
#endif

#endif //__CX_DX_USERPROFILE_H__

