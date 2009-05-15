#ifndef SH_DIAMRETCODES_H
#define	SH_DIAMRETCODES_H

//Permanent Failure return codes , to be returned in Experimental Result AVP
#define	SH_PERMFAIL_EXPRES_DIAMETER_ERROR_USER_DATA_NOT_RECOGNIZED		5100
#define	SH_PERMFAIL_EXPRES_DIAMETER_ERROR_OPERATION_NOT_ALLOWED			5101
#define	SH_PERMFAIL_EXPRES_DIAMETER_ERROR_USER_DATA_CANNOT_BE_READ		5102	
#define	SH_PERMFAIL_EXPRES_DIAMETER_ERROR_USER_DATA_CANNOT_BE_MODIFIED	5103	
#define	SH_PERMFAIL_EXPRES_DIAMETER_ERROR_USER_DATA_CANNOT_BE_NOTIFIED	5104	
#define	SH_PERMFAIL_EXPRES_DIAMETER_ERROR_TOO_MUCH_DATA					5008
#define	SH_PERMFAIL_EXPRES_DIAMETER_ERROR_TRANSPARENT_DATA_OUT_OF_SYNC	5105
#define	SH_PERMFAIL_EXPRES_DIAMETER_ERROR_FEATURE_UNSUPPORTED			5011
#define	SH_PERMFAIL_EXPRES_DIAMETER_ERROR_NO_SUBSCRIPTION_TO_DATA		5107
#define	SH_PERMFAIL_EXPRES_DIAMETER_ERROR_DSAI_NOT_AVAILABLE			5108

//Transient Failure return codes , to be returned in Experimental Result AVP
#define	SH_TRANSFAIL_EXPRES_DIAMETER_USER_DATA_NOT_AVAILABLE			4100
#define	SH_TRANSFAIL_EXPRES_DIAMETER_PRIOR_UPDATE_IN_PROGRESS			4101


//Values that can come in the Data Reference AVP
#define	SH_DATA_REF_AVP_VAL_REPOSITORYDATA								0
#define	SH_DATA_REF_AVP_VAL_IMSPUBLICIDENTITY							10
#define	SH_DATA_REF_AVP_VAL_IMSUSERSTATE								11
#define	SH_DATA_REF_AVP_VAL_S_CSCFNAME									12
#define	SH_DATA_REF_AVP_VAL_INITIALFILTERCRITERIA						13
#define	SH_DATA_REF_AVP_VAL_LOCATIONINFORMATION							14
#define	SH_DATA_REF_AVP_VAL_USERSTATE									15
#define	SH_DATA_REF_AVP_VAL_CHARGINGINFORMATION							16
#define	SH_DATA_REF_AVP_VAL_MSISDN										17
#define	SH_DATA_REF_AVP_VAL_PSIACTIVATION								18
#define	SH_DATA_REF_AVP_VAL_DSAI										19
#define	SH_DATA_REF_AVP_VAL_ALIASESREPOSITORYDATA						20



//Values that that come in the Subs-Req-Type AVP
#define	SH_SUBS_REQ_TYPE_AVP_VAL_SUBSCRIBE								0
#define	SH_SUBS_REQ_TYPE_AVP_VAL_UNSUBSCRIBE							1

//Values that can come in the Requested-Domain AVP
#define	SH_REQUESTED_DOMAIN_AVP_VAL_CS									0
#define	SH_REQUESTED_DOMAIN_AVP_VAL_PS									1


//Values that can come in the Current-Location AVP
#define	SH_CURRENT_LOCATION_AVP_VAL_NO_NEED								0
#define	SH_CURRENT_LOCATION_AVP_VAL_INITIATE							1


//Values that can come in the value of Identity-Set AVP
#define	SH_IDENTITY_SET_AVP_VAL_ALL_IDENTITIES							0
#define	SH_IDENTITY_SET_AVP_VAL_REGISTERED_IDENTITIES					1
#define	SH_IDENTITY_SET_AVP_VAL_IMPLICIT_IDENTITIES						2
#define	SH_IDENTITY_SET_AVP_VAL_ALIAS_IDENTITIES						3


//values that can come in the Send-Data-Indication AVP
#define	SH_SEND_DATA_INDIC_AVP_VAL_USER_DATA_NOT_REQUESTED				0
#define	SH_SEND_DATA_INDIC_AVP_VAL_USER_DATA_REQUESTED					1



#endif //#ifndef SH_DIAMRETCODES_H