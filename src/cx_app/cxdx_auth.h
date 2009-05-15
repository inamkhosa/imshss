
#ifndef __CX_DX_AUTH_H__
#define	__CX_DX_AUTH_H__

#include "cxdx_app.h"

#define CX_K_SIZE       128     //Secret Key length in bits
#define CX_RAND_SIZE    128     //RAND length in bits
#define CX_SQN_SIZE      48     //SQN length in bits
#define CX_SEQ_SIZE      40     //SEQ length in bits
#define CX_IND_SIZE       8     //IND length in bits
#define CX_AMF_SIZE      16     //AMF length in bits
#define CX_MAC_SIZE      64     //MAC length in bits
#define CX_XRES_SIZE     64     //XRES length in bits
#define CX_CK_SIZE      128     //CK length in bits
#define CX_IK_SIZE      128     //IK length in bits
#define CX_AK_SIZE       48     //AK length in bits
#define CX_AUTN_SIZE    128     //AUTN length in bits



//////////////////////Code taken from SIP project///////////////////////////////
#define znew   (z=36969*(z&65535)+(z>>16))
#define wnew   (w=18000*(w&65535)+(w>>16))
#define MWC    ((znew<<16)+wnew )
#define SHR3  (jsr^=(jsr<<17), jsr^=(jsr>>13), jsr^=(jsr<<5))
#define CONG  (jcong=69069*jcong+1234567)
#define FIB   ((b=a+b),(a=b-a))
#define KISS  ((MWC^CONG)+SHR3)
#define LFIB4 (c++,t[c]=t[c]+t[UC(c+58)]+t[UC(c+119)]+t[UC(c+178)])
#define SWB   (c++,bro=(x<y),t[c]=(x=t[UC(c+34)])-(y=t[UC(c+19)]+bro))
#define UNI   (KISS*2.328306e-10)
#define VNI   ((long) KISS)*4.656613e-10
#define UC    (unsigned char)  /*a cast operation*/

typedef unsigned long UL;
typedef unsigned char u8;

/*  Global static variables: */
 static UL z=362436069, w=521288629, jsr=123456789, jcong=380116160;
 static UL a=224466889, b=7584631, t[256];
/* Use random seeds to reset z,w,jsr,jcong,a,b, and the table t[256]*/

 static UL x=0,y=0,bro; static unsigned char c=0;
 
 
#ifdef __cplusplus
extern "C" {
#endif
     
 void Cx_InitSeed(void* r_pvSIPContext);
 int Cx_GenerateRandomString(unsigned char* r_puchRandomString);
 t_CxDxAPIRetVal CX_GenerateSQN(HCXAPPCONTEXT hAppContext, 
         unsigned char* pucNewSQN, unsigned char* pucOldSQN);
 
 t_CxDxAPIRetVal CX_GenerateMAC(    HCXAPPCONTEXT hAppContext, 
                                    unsigned char* pucMAC, unsigned char* pucK, 
                                    unsigned char* pucAMF, unsigned char* pucSQN,
                                    unsigned char* pucRAND);
 
 t_CxDxAPIRetVal CX_GenerateAUTN(   HCXAPPCONTEXT hAppContext, 
                                    unsigned char* pucAUTN, unsigned char* pucSQN, 
                                    unsigned char* pucAK, unsigned char* pucAMF,
                                    unsigned char* pucMAC);
 
 t_CxDxAPIRetVal CX_GenerateXRES(HCXAPPCONTEXT hAppContext, unsigned char* pucXRES, 
                                    unsigned char* pucK, unsigned char* pucRAND);
 
 t_CxDxAPIRetVal CX_GenerateCK(HCXAPPCONTEXT hAppContext, unsigned char* pucCK, 
                                    unsigned char* pucK, unsigned char* pucRAND);
 
 t_CxDxAPIRetVal CX_GenerateIK(HCXAPPCONTEXT hAppContext, unsigned char* pucIK, 
                                    unsigned char* pucK, unsigned char* pucRAND);
 
 
 t_CxDxAPIRetVal CX_GenerateAK(HCXAPPCONTEXT hAppContext, unsigned char* pucAK, 
                                    unsigned char* pucK, unsigned char* pucRAND);
 

 void chartohex(unsigned char target[], unsigned char source[], int length);
 int getPower(int a, int b);
 void incSEQ( unsigned char seq[], int octetLenSeq, int amod8);
 void incIND(unsigned char ind[], int octetLenInd, int amod8);
 void reGenerateSQN(unsigned char seq[], unsigned char x[], unsigned char ind[], int octetLenSeq, int octetLenInd, int amod8);
 void getnext_sqn(unsigned char sqn[],unsigned char sqn_next[]);
 void f1    ( u8 k[16], u8 rand[16], u8 sqn[6], u8 amf[2],
                 u8 mac_a[8] );
 void f2345 ( u8 k[16], u8 rand[16],
                 u8 res[8], u8 ck[16], u8 ik[16], u8 ak[6] );
 void f1star( u8 k[16], u8 rand[16], u8 sqn[6], u8 amf[2], 
                 u8 mac_s[8] );
 void f5star( u8 k[16], u8 rand[16],
                 u8 ak[6] );
 void ComputeOPc( u8 op_c[16] );
 void RijndaelKeySchedule( u8 key[16] );
 void RijndaelEncrypt( u8 input[16], u8 output[16] );
 
#ifdef __cplusplus
}
#endif
 
#endif
