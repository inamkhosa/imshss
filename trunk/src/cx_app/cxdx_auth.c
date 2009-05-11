
#include "AMPS_Defines.h"
#include "cxdx_auth.h"
#include "dmbase_ampswrappers.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "hss.h"


extern	int                     g_nDiamBaseTraceID ;

u8 OP[16] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
             0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
/* Example procedure to set the table, using KISS: */
 static void settable(UL i1,UL i2,UL i3,UL i4,UL i5, UL i6)
 {
        int i; z=i1;w=i2,jsr=i3; jcong=i4; a=i5; b=i6;
        for(i=0;i<256;i=i+1)  t[i]=KISS;
 }

void Cx_InitSeed(void* r_pvSIPContext)
{
	unsigned long seed=time(0);
	settable (seed, 2*seed, 3*seed, 4*seed, 5*seed, 6*seed);
}


int Cx_GenerateRandomString(unsigned char* r_puchRandomString)
{
	unsigned long nRandomVal =0;
	int nOffset = 0;
	unsigned char puchIntString[32];
	int nIntStringSize = 0;
	int nCount = 0;
        unsigned char *pcByte = (unsigned char*)&nRandomVal;
	for (nCount = 0; nCount <=3; nCount++)
	{
		//AMPS_Rand(r_poSIPContext->hAMPSHandle, (unsigned int*)&r_poSIPContext->oRandomNumberSeed.lMicroSecond);
		//nRandomVal = ((int)r_poSIPContext->oRandomNumberSeed.lMicroSecond);
		nRandomVal += (KISS + SWB);

		//memset(puchIntString, 0, sizeof(puchIntString));
		//nIntStringSize = SIP_IntToString(puchIntString, nRandomVal, 10);
		//memcpy(r_puchRandomString+nOffset, puchIntString, nIntStringSize);

		//nOffset += nIntStringSize;
		//r_puchRandomString[nOffset++] = '-';

                r_puchRandomString[nOffset++] = pcByte[0]%71 + 47;
                r_puchRandomString[nOffset++] = pcByte[1]%71 + 47;
                r_puchRandomString[nOffset++] = pcByte[2]%71 + 47;
                r_puchRandomString[nOffset++] = pcByte[3]%71 + 47;
	}
	return AMPS_SUCCESS;

}


////////////////////////////////////////////////////////////////////////////////

t_CxDxAPIRetVal CX_GenerateSQN(HCXAPPCONTEXT hAppContext, unsigned char* pucNewSQN,
                                    unsigned char* pucOldSQN)
{
    DMBASE_TRACE( g_nDiamBaseTraceID, AMPS_TRACE_LEVEL_INFO, " Entering.\n");
    DMBase_MemsetZero(pucNewSQN, CX_SQN_SIZE/8);
    DMBASE_TRACE( g_nDiamBaseTraceID, AMPS_TRACE_LEVEL_INFO, " Leaving.\n");
    return CxDx_Retval_Success;
}

t_CxDxAPIRetVal CX_GenerateMAC( HCXAPPCONTEXT hAppContext, unsigned char* pucMAC, unsigned char* pucK,
                                    unsigned char* pucAMF, unsigned char* pucSQN,
                                    unsigned char* pucRAND)
{
    DMBASE_TRACE( g_nDiamBaseTraceID, AMPS_TRACE_LEVEL_INFO, " Entering.\n");

    DMBase_MemsetZero(pucMAC, CX_MAC_SIZE/8);

    DMBASE_TRACE( g_nDiamBaseTraceID, AMPS_TRACE_LEVEL_INFO, " Leaving.\n");
    return CxDx_Retval_Success;
}

t_CxDxAPIRetVal CX_GenerateAUTN( HCXAPPCONTEXT hAppContext, unsigned char* pucAUTN, unsigned char* pucSQN,
                                    unsigned char* pucAK, unsigned char* pucAMF,
                                    unsigned char* pucMAC)
{
    DMBASE_TRACE( g_nDiamBaseTraceID, AMPS_TRACE_LEVEL_INFO, " Entering.\n");

    DMBase_MemsetZero(pucAUTN, CX_AUTN_SIZE/8);

    unsigned char ConcealedSQN[CX_SQN_SIZE];
    int i = 0;
    for(i = 0; i<(CX_SQN_SIZE/8); i++)
    {
        pucAUTN[i] = pucSQN[i] ^ pucAK[i];
    }

    int nOffset = CX_SQN_SIZE/8;
    memcpy((pucAUTN+nOffset), pucAMF, CX_AMF_SIZE/8);

    nOffset += CX_AMF_SIZE/8;
    memcpy((pucAUTN+nOffset), pucMAC, CX_MAC_SIZE/8);

    DMBASE_TRACE( g_nDiamBaseTraceID, AMPS_TRACE_LEVEL_INFO, " Leaving.\n");
    return CxDx_Retval_Success;

}

t_CxDxAPIRetVal CX_GenerateXRES(HCXAPPCONTEXT hAppContext, unsigned char* pucXRES,
                                    unsigned char* pucK, unsigned char* pucRAND)
{
    DMBASE_TRACE( g_nDiamBaseTraceID, AMPS_TRACE_LEVEL_INFO, " Entering.\n");
    DMBASE_MD5_HANDLE hMD5 = NULL;
    DMBase_MD5Init(hAppContext->hDiamStackContext,&hMD5);
    DMBase_MD5Update(hAppContext->hDiamStackContext,hMD5, pucK, CX_K_SIZE/8 );
    DMBase_MD5Update(hAppContext->hDiamStackContext,hMD5, pucRAND, CX_RAND_SIZE/8 );
    DMBase_MD5Final(hAppContext->hDiamStackContext, hMD5, pucXRES);
    DMBase_MD5Cleanup(hAppContext->hDiamStackContext, hMD5);

    DMBASE_TRACE( g_nDiamBaseTraceID, AMPS_TRACE_LEVEL_INFO, " Leaving.\n");
    return CxDx_Retval_Success;
}

t_CxDxAPIRetVal CX_GenerateCK(HCXAPPCONTEXT hAppContext, unsigned char* pucCK,
                                    unsigned char* pucK, unsigned char* pucRAND)
{
    DMBASE_TRACE( g_nDiamBaseTraceID, AMPS_TRACE_LEVEL_INFO, " Entering.\n");
    DMBASE_MD5_HANDLE hMD5 = NULL;
    DMBase_MD5Init(hAppContext->hDiamStackContext,&hMD5);
    DMBase_MD5Update(hAppContext->hDiamStackContext,hMD5, pucK, CX_K_SIZE/8 );
    DMBase_MD5Update(hAppContext->hDiamStackContext,hMD5, pucRAND, CX_RAND_SIZE/8 );
    DMBase_MD5Final(hAppContext->hDiamStackContext, hMD5, pucCK);
    DMBase_MD5Cleanup(hAppContext->hDiamStackContext, hMD5);


    DMBASE_TRACE( g_nDiamBaseTraceID, AMPS_TRACE_LEVEL_INFO, " Leaving.\n");
    return CxDx_Retval_Success;
}

t_CxDxAPIRetVal CX_GenerateIK(HCXAPPCONTEXT hAppContext, unsigned char* pucIK,
                                    unsigned char* pucK, unsigned char* pucRAND)
{
    DMBASE_TRACE( g_nDiamBaseTraceID, AMPS_TRACE_LEVEL_INFO, " Entering.\n");
    DMBASE_MD5_HANDLE hMD5 = NULL;
    DMBase_MD5Init(hAppContext->hDiamStackContext,&hMD5);
    DMBase_MD5Update(hAppContext->hDiamStackContext,hMD5, pucK, CX_K_SIZE/8 );
    DMBase_MD5Update(hAppContext->hDiamStackContext,hMD5, pucRAND, CX_RAND_SIZE/8 );
    DMBase_MD5Final(hAppContext->hDiamStackContext, hMD5, pucIK);
    DMBase_MD5Cleanup(hAppContext->hDiamStackContext, hMD5);

    DMBASE_TRACE( g_nDiamBaseTraceID, AMPS_TRACE_LEVEL_INFO, " Leaving.\n");
    return CxDx_Retval_Success;
}

t_CxDxAPIRetVal CX_GenerateAK(HCXAPPCONTEXT hAppContext, unsigned char* pucAK,
                                    unsigned char* pucK, unsigned char* pucRAND)
{

    DMBASE_TRACE( g_nDiamBaseTraceID, AMPS_TRACE_LEVEL_INFO, " Entering.\n");

    DMBase_MemsetZero(pucAK, CX_AK_SIZE/8);

    DMBASE_TRACE( g_nDiamBaseTraceID, AMPS_TRACE_LEVEL_INFO, " Leaving.\n");
    return CxDx_Retval_Success;
}


void chartohex(unsigned char target[], unsigned char source[], int length)
{
	int i = 0, j = 0;
	char str[3];
	for(i=0;i<length; i++)
	{
		strncpy(str, source+j, 2);
		str[2] = '\0';
		target[i] = (unsigned char)strtol(str, (char**)NULL, 16);
		j += 2;
	}
}


int getPower(int a, int b)
{
        int res = 1, i;
        for ( i = 0; i < b; i++){
                res *= a;
        }
        return res;
}

 void incSEQ( unsigned char seq[], int octetLenSeq, int amod8)
{

        int i = octetLenSeq - 1;
        int increment = 1;

        while (i >= 0){
                if (i == octetLenSeq - 1 && (amod8 != 0)){
                        increment = getPower(2, amod8);
                }
                else{
                        increment = 1;
                }
                if (seq[i] == (unsigned char)(256 - increment)){
                        seq[i] = (unsigned char)0;
                        i--;
                }
                else{
                        seq[i] += increment;
                        break;
                }
        }

}
void reGenerateSQN(unsigned char seq[],unsigned char x[], unsigned char ind[], int octetLenSeq, int octetLenInd, int amod8){


        int k = 6 - 1, i;
        for (i = octetLenInd - 1; i >= 0; i--){
                x[k] = ind[i];
                k--;
        }

        for (i = octetLenSeq - 1; i >= 0; i--){
                if (i == octetLenSeq - 1 && amod8 != 0){
                        x[k+1] = (unsigned char) (x[k+1] + seq[i]);
                }
                else{
                        x[k] = seq[i];
                        k--;
                }
        }


}
void incIND(unsigned char ind[], int octetLenInd, int amod8)
{
        int i = octetLenInd - 1;

        while (i >= 0){
                if (i == 0 && (amod8 != 0 && ind[i] == (unsigned char)(getPower(2, amod8) - 1))){
                        ind[i]=(unsigned char)0;
                        break;
                }
                else if (ind[i] == (unsigned char)255){
                        ind[i] = (unsigned char)0;
                        i--;
                }
                else{
                        ind[i]++;
                        break;
                }
        }

}


void getnext_sqn(unsigned char sqn[],unsigned char sqn_next[])
{
    int a = 5;

    int startFlag = 1;
    int i;
    for (i = 0; i < 6; i++)
    {
            if (sqn[i] != 0)
            {
                    startFlag = 0;
                    break;
            }
    }

    unsigned char ind[6];
    unsigned char seq[6];

    int octetLenInd = 0;
    int octetLenSeq = 0;

    unsigned char amod8 = (unsigned char)(a % 8);

    octetLenInd = a / 8;

    if (amod8 != 0)
    {
            octetLenInd++;
    }

    octetLenSeq = (48 - a) / 8;
    if (amod8 != 0)
    {
            octetLenSeq++;
    }

    int k = octetLenInd - 1;
    int j;
    for (i = 6 - 1; i > 6 - 1 - octetLenInd; i--)
    {
            ind[k] = sqn[i];
            k--;
    }

    if (amod8 != 0){
            ind[k+1] = (unsigned char) (ind[k+1] & (getPower(2, amod8) - 1));
            sqn[i+1] = (unsigned char)(sqn[i+1] - ind[k+1]);
            i++;
    }

    k = octetLenSeq - 1;
    for (j = i; j >= 0; j--){
            seq[k] = sqn[j];
            k--;
    }

    if (startFlag){
            incSEQ(seq, octetLenSeq, amod8);
    }
    else{
            incIND(ind, octetLenInd, amod8);
            incSEQ(seq, octetLenSeq, amod8);
    }

    reGenerateSQN( seq, sqn_next, ind, octetLenSeq, octetLenInd, amod8);

}

/*-------------------------------------------------------------------
 *                            Algorithm f1
 *-------------------------------------------------------------------
 *
 *  Computes network authentication code MAC-A from key K, random
 *  challenge RAND, sequence number SQN and authentication management
 *  field AMF.
 *
 *-----------------------------------------------------------------*/

void f1    ( u8 k[16], u8 rand[16], u8 sqn[6], u8 amf[2],
             u8 mac_a[8] )
{
  u8 op_c[16];
  u8 temp[16];
  u8 in1[16];
  u8 out1[16];
  u8 rijndaelInput[16];
  u8 i;

  RijndaelKeySchedule( k );

  ComputeOPc( op_c );

  for (i=0; i<16; i++)
    rijndaelInput[i] = rand[i] ^ op_c[i];
  RijndaelEncrypt( rijndaelInput, temp );

  for (i=0; i<6; i++)
  {
    in1[i]    = sqn[i];
    in1[i+8]  = sqn[i];
  }
  for (i=0; i<2; i++)
  {
    in1[i+6]  = amf[i];
    in1[i+14] = amf[i];
  }

  /* XOR op_c and in1, rotate by r1=64, and XOR *
   * on the constant c1 (which is all zeroes)   */

  for (i=0; i<16; i++)
    rijndaelInput[(i+8) % 16] = in1[i] ^ op_c[i];

  /* XOR on the value temp computed before */

  for (i=0; i<16; i++)
    rijndaelInput[i] ^= temp[i];

  RijndaelEncrypt( rijndaelInput, out1 );
  for (i=0; i<16; i++)
    out1[i] ^= op_c[i];

  for (i=0; i<8; i++)
    mac_a[i] = out1[i];

  return;
} /* end of function f1 */



/*-------------------------------------------------------------------
 *                            Algorithms f2-f5
 *-------------------------------------------------------------------
 *
 *  Takes key K and random challenge RAND, and returns response RES,
 *  confidentiality key CK, integrity key IK and anonymity key AK.
 *
 *-----------------------------------------------------------------*/

void f2345 ( u8 k[16], u8 rand[16],
             u8 res[8], u8 ck[16], u8 ik[16], u8 ak[6] )
{
  u8 op_c[16];
  u8 temp[16];
  u8 out[16];
  u8 rijndaelInput[16];
  u8 i;

  RijndaelKeySchedule( k );

  ComputeOPc( op_c );

  for (i=0; i<16; i++)
    rijndaelInput[i] = rand[i] ^ op_c[i];
  RijndaelEncrypt( rijndaelInput, temp );

  /* To obtain output block OUT2: XOR OPc and TEMP,    *
   * rotate by r2=0, and XOR on the constant c2 (which *
   * is all zeroes except that the last bit is 1).     */

  for (i=0; i<16; i++)
    rijndaelInput[i] = temp[i] ^ op_c[i];
  rijndaelInput[15] ^= 1;

  RijndaelEncrypt( rijndaelInput, out );
  for (i=0; i<16; i++)
    out[i] ^= op_c[i];

  for (i=0; i<8; i++)
    res[i] = out[i+8];
  for (i=0; i<6; i++)
    ak[i]  = out[i];

  /* To obtain output block OUT3: XOR OPc and TEMP,        *
   * rotate by r3=32, and XOR on the constant c3 (which    *
   * is all zeroes except that the next to last bit is 1). */

  for (i=0; i<16; i++)
    rijndaelInput[(i+12) % 16] = temp[i] ^ op_c[i];
  rijndaelInput[15] ^= 2;

  RijndaelEncrypt( rijndaelInput, out );
  for (i=0; i<16; i++)
    out[i] ^= op_c[i];

  for (i=0; i<16; i++)
    ck[i] = out[i];

  /* To obtain output block OUT4: XOR OPc and TEMP,         *
   * rotate by r4=64, and XOR on the constant c4 (which     *
   * is all zeroes except that the 2nd from last bit is 1). */

  for (i=0; i<16; i++)
    rijndaelInput[(i+8) % 16] = temp[i] ^ op_c[i];
  rijndaelInput[15] ^= 4;

  RijndaelEncrypt( rijndaelInput, out );
  for (i=0; i<16; i++)
    out[i] ^= op_c[i];

  for (i=0; i<16; i++)
    ik[i] = out[i];

  return;
} /* end of function f2345 */


/*-------------------------------------------------------------------
 *                            Algorithm f1*
 *-------------------------------------------------------------------
 *
 *  Computes resynch authentication code MAC-S from key K, random
 *  challenge RAND, sequence number SQN and authentication management
 *  field AMF.
 *
 *-----------------------------------------------------------------*/

void f1star( u8 k[16], u8 rand[16], u8 sqn[6], u8 amf[2],
             u8 mac_s[8] )
{
  u8 op_c[16];
  u8 temp[16];
  u8 in1[16];
  u8 out1[16];
  u8 rijndaelInput[16];
  u8 i;

  RijndaelKeySchedule( k );

  ComputeOPc( op_c );

  for (i=0; i<16; i++)
    rijndaelInput[i] = rand[i] ^ op_c[i];
  RijndaelEncrypt( rijndaelInput, temp );

  for (i=0; i<6; i++)
  {
    in1[i]    = sqn[i];
    in1[i+8]  = sqn[i];
  }
  for (i=0; i<2; i++)
  {
    in1[i+6]  = amf[i];
    in1[i+14] = amf[i];
  }

  /* XOR op_c and in1, rotate by r1=64, and XOR *
   * on the constant c1 (which is all zeroes)   */

  for (i=0; i<16; i++)
    rijndaelInput[(i+8) % 16] = in1[i] ^ op_c[i];

  /* XOR on the value temp computed before */

  for (i=0; i<16; i++)
    rijndaelInput[i] ^= temp[i];

  RijndaelEncrypt( rijndaelInput, out1 );
  for (i=0; i<16; i++)
    out1[i] ^= op_c[i];

  for (i=0; i<8; i++)
    mac_s[i] = out1[i+8];

  return;
} /* end of function f1star */


/*-------------------------------------------------------------------
 *                            Algorithm f5*
 *-------------------------------------------------------------------
 *
 *  Takes key K and random challenge RAND, and returns resynch
 *  anonymity key AK.
 *
 *-----------------------------------------------------------------*/

void f5star( u8 k[16], u8 rand[16],
             u8 ak[6] )
{
  u8 op_c[16];
  u8 temp[16];
  u8 out[16];
  u8 rijndaelInput[16];
  u8 i;

  RijndaelKeySchedule( k );

  ComputeOPc( op_c );

  for (i=0; i<16; i++)
    rijndaelInput[i] = rand[i] ^ op_c[i];
  RijndaelEncrypt( rijndaelInput, temp );

  /* To obtain output block OUT5: XOR OPc and TEMP,         *
   * rotate by r5=96, and XOR on the constant c5 (which     *
   * is all zeroes except that the 3rd from last bit is 1). */

  for (i=0; i<16; i++)
    rijndaelInput[(i+4) % 16] = temp[i] ^ op_c[i];
  rijndaelInput[15] ^= 8;

  RijndaelEncrypt( rijndaelInput, out );
  for (i=0; i<16; i++)
    out[i] ^= op_c[i];

  for (i=0; i<6; i++)
    ak[i] = out[i];

  return;
} /* end of function f5star */


/*-------------------------------------------------------------------
 *  Function to compute OPc from OP and K.  Assumes key schedule has
    already been performed.
 *-----------------------------------------------------------------*/

void ComputeOPc( u8 op_c[16] )
{
  u8 i;

  RijndaelEncrypt( OP, op_c );
  for (i=0; i<16; i++)
    op_c[i] ^= OP[i];

  return;
} /* end of function ComputeOPc */



/*-------------------- Rijndael round subkeys ---------------------*/
u8 roundKeys[11][4][4];

/*--------------------- Rijndael S box table ----------------------*/
u8 S[256] = {
 99,124,119,123,242,107,111,197, 48,  1,103, 43,254,215,171,118,
202,130,201,125,250, 89, 71,240,173,212,162,175,156,164,114,192,
183,253,147, 38, 54, 63,247,204, 52,165,229,241,113,216, 49, 21,
  4,199, 35,195, 24,150,  5,154,  7, 18,128,226,235, 39,178,117,
  9,131, 44, 26, 27,110, 90,160, 82, 59,214,179, 41,227, 47,132,
 83,209,  0,237, 32,252,177, 91,106,203,190, 57, 74, 76, 88,207,
208,239,170,251, 67, 77, 51,133, 69,249,  2,127, 80, 60,159,168,
 81,163, 64,143,146,157, 56,245,188,182,218, 33, 16,255,243,210,
205, 12, 19,236, 95,151, 68, 23,196,167,126, 61,100, 93, 25,115,
 96,129, 79,220, 34, 42,144,136, 70,238,184, 20,222, 94, 11,219,
224, 50, 58, 10, 73,  6, 36, 92,194,211,172, 98,145,149,228,121,
231,200, 55,109,141,213, 78,169,108, 86,244,234,101,122,174,  8,
186,120, 37, 46, 28,166,180,198,232,221,116, 31, 75,189,139,138,
112, 62,181,102, 72,  3,246, 14, 97, 53, 87,185,134,193, 29,158,
225,248,152, 17,105,217,142,148,155, 30,135,233,206, 85, 40,223,
140,161,137, 13,191,230, 66,104, 65,153, 45, 15,176, 84,187, 22,
};

/*------- This array does the multiplication by x in GF(2^8) ------*/
u8 Xtime[256] = {
  0,  2,  4,  6,  8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30,
 32, 34, 36, 38, 40, 42, 44, 46, 48, 50, 52, 54, 56, 58, 60, 62,
 64, 66, 68, 70, 72, 74, 76, 78, 80, 82, 84, 86, 88, 90, 92, 94,
 96, 98,100,102,104,106,108,110,112,114,116,118,120,122,124,126,
128,130,132,134,136,138,140,142,144,146,148,150,152,154,156,158,
160,162,164,166,168,170,172,174,176,178,180,182,184,186,188,190,
192,194,196,198,200,202,204,206,208,210,212,214,216,218,220,222,
224,226,228,230,232,234,236,238,240,242,244,246,248,250,252,254,
 27, 25, 31, 29, 19, 17, 23, 21, 11,  9, 15, 13,  3,  1,  7,  5,
 59, 57, 63, 61, 51, 49, 55, 53, 43, 41, 47, 45, 35, 33, 39, 37,
 91, 89, 95, 93, 83, 81, 87, 85, 75, 73, 79, 77, 67, 65, 71, 69,
123,121,127,125,115,113,119,117,107,105,111,109, 99, 97,103,101,
155,153,159,157,147,145,151,149,139,137,143,141,131,129,135,133,
187,185,191,189,179,177,183,181,171,169,175,173,163,161,167,165,
219,217,223,221,211,209,215,213,203,201,207,205,195,193,199,197,
251,249,255,253,243,241,247,245,235,233,239,237,227,225,231,229
};


/*-------------------------------------------------------------------
 *  Rijndael key schedule function.  Takes 16-byte key and creates
 *  all Rijndael's internal subkeys ready for encryption.
 *-----------------------------------------------------------------*/

void RijndaelKeySchedule( u8 key[16] )
{
  u8 roundConst;
  int i, j;

  /* first round key equals key */
  for (i=0; i<16; i++)
    roundKeys[0][i & 0x03][i>>2] = key[i];

  roundConst = 1;

  /* now calculate round keys */
  for (i=1; i<11; i++)
  {
    roundKeys[i][0][0] = S[roundKeys[i-1][1][3]]
                         ^ roundKeys[i-1][0][0] ^ roundConst;
    roundKeys[i][1][0] = S[roundKeys[i-1][2][3]]
                         ^ roundKeys[i-1][1][0];
    roundKeys[i][2][0] = S[roundKeys[i-1][3][3]]
                         ^ roundKeys[i-1][2][0];
    roundKeys[i][3][0] = S[roundKeys[i-1][0][3]]
                         ^ roundKeys[i-1][3][0];

    for (j=0; j<4; j++)
    {
      roundKeys[i][j][1] = roundKeys[i-1][j][1] ^ roundKeys[i][j][0];
      roundKeys[i][j][2] = roundKeys[i-1][j][2] ^ roundKeys[i][j][1];
      roundKeys[i][j][3] = roundKeys[i-1][j][3] ^ roundKeys[i][j][2];
    }

    /* update round constant */
    roundConst = Xtime[roundConst];
  }

  return;
} /* end of function RijndaelKeySchedule */


/* Round key addition function */
void KeyAdd(u8 state[4][4], u8 roundKeys[11][4][4], int round)
{
  int i, j;

  for (i=0; i<4; i++)
    for (j=0; j<4; j++)
      state[i][j] ^= roundKeys[round][i][j];

  return;
}


/* Byte substitution transformation */
int ByteSub(u8 state[4][4])
{
  int i, j;

  for (i=0; i<4; i++)
    for (j=0; j<4; j++)
      state[i][j] = S[state[i][j]];

  return 0;
}


/* Row shift transformation */
void ShiftRow(u8 state[4][4])
{
  u8 temp;

  /* left rotate row 1 by 1 */
  temp = state[1][0];
  state[1][0] = state[1][1];
  state[1][1] = state[1][2];
  state[1][2] = state[1][3];
  state[1][3] = temp;

  /* left rotate row 2 by 2 */
  temp = state[2][0];
  state[2][0] = state[2][2];
  state[2][2] = temp;
  temp = state[2][1];
  state[2][1] = state[2][3];
  state[2][3] = temp;

  /* left rotate row 3 by 3 */
  temp = state[3][0];
  state[3][0] = state[3][3];
  state[3][3] = state[3][2];
  state[3][2] = state[3][1];
  state[3][1] = temp;

  return;
}


/* MixColumn transformation*/
void MixColumn(u8 state[4][4])
{
  u8 temp, tmp, tmp0;
  int i;

  /* do one column at a time */
  for (i=0; i<4;i++)
  {
    temp = state[0][i] ^ state[1][i] ^ state[2][i] ^ state[3][i];
    tmp0 = state[0][i];

    /* Xtime array does multiply by x in GF2^8 */
    tmp = Xtime[state[0][i] ^ state[1][i]];
    state[0][i] ^= temp ^ tmp;

    tmp = Xtime[state[1][i] ^ state[2][i]];
    state[1][i] ^= temp ^ tmp;

    tmp = Xtime[state[2][i] ^ state[3][i]];
    state[2][i] ^= temp ^ tmp;

    tmp = Xtime[state[3][i] ^ tmp0];
    state[3][i] ^= temp ^ tmp;
  }

  return;
}


/*-------------------------------------------------------------------
 *  Rijndael encryption function.  Takes 16-byte input and creates
 *  16-byte output (using round keys already derived from 16-byte
 *  key).
 *-----------------------------------------------------------------*/

void RijndaelEncrypt( u8 input[16], u8 output[16] )
{
  u8 state[4][4];
  int i, r;

  /* initialise state array from input byte string */
  for (i=0; i<16; i++)
    state[i & 0x3][i>>2] = input[i];

  /* add first round_key */
  KeyAdd(state, roundKeys, 0);

  /* do lots of full rounds */
  for (r=1; r<=9; r++)
  {
    ByteSub(state);
    ShiftRow(state);
    MixColumn(state);
    KeyAdd(state, roundKeys, r);
  }

  /* final round */
  ByteSub(state);
  ShiftRow(state);
  KeyAdd(state, roundKeys, r);

  /* produce output byte string from state array */
  for (i=0; i<16; i++)
  {
    output[i] = state[i & 0x3][i>>2];
  }

  return;
} /* end of function RijndaelEncrypt */

