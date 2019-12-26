// WebRtcAudioTest.cpp : ¶¨Òå¿ØÖÆÌ¨Ó¦ÓÃ³ÌÐòµÄÈë¿Úµã¡£
//

#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>
#include <assert.h>

#include "../WebRtcMoudle/signal_processing_library.h"
#include "../WebRtcMoudle/noise_suppression_x.h"
#include "../WebRtcMoudle/noise_suppression.h"
#include "../WebRtcMoudle/gain_control.h"

void NoiseSuppression32(char *szFileIn,char *szFileOut,int nSample,int nMode)
{
	int nRet = 0;
	NsHandle *pNS_inst = NULL;

	FILE *fpIn = NULL;
	FILE *fpOut = NULL;

	char *pInBuffer =NULL;
	char *pOutBuffer = NULL;

	do
	{
		int i = 0;
		int nFileSize = 0;
		int nTime = 0;
		if (0 != WebRtcNs_Create(&pNS_inst))
		{
			printf("Noise_Suppression WebRtcNs_Create err! \n");
			break;
		}

		if (0 !=  WebRtcNs_Init(pNS_inst,nSample))
		{
			printf("Noise_Suppression WebRtcNs_Init err! \n");
			break;
		}

		if (0 !=  WebRtcNs_set_policy(pNS_inst,nMode))
		{
			printf("Noise_Suppression WebRtcNs_set_policy err! \n");
			break;
		}

		fpIn = fopen(szFileIn, "rb");
		if (NULL == fpIn)
		{
			printf("open src file err \n");
			break;
		}
		fseek(fpIn,0,SEEK_END);
		nFileSize = ftell(fpIn); 
		fseek(fpIn,0,SEEK_SET); 

		pInBuffer = (char*)malloc(nFileSize);
		memset(pInBuffer,0,nFileSize);
		fread(pInBuffer, sizeof(char), nFileSize, fpIn);

		pOutBuffer = (char*)malloc(nFileSize);
		memset(pOutBuffer,0,nFileSize);

		int  filter_state1[6],filter_state12[6];
		int  Synthesis_state1[6],Synthesis_state12[6];

		memset(filter_state1,0,sizeof(filter_state1));
		memset(filter_state12,0,sizeof(filter_state12));
		memset(Synthesis_state1,0,sizeof(Synthesis_state1));
		memset(Synthesis_state12,0,sizeof(Synthesis_state12));

		nTime = GetTickCount();
		for (i = 0;i < nFileSize;i+=640)
		{
			if (nFileSize - i >= 640)
			{
				short shBufferIn[320] = {0};

				short shInL[160],shInH[160];
				short shOutL[160] = {0},shOutH[160] = {0};

				memcpy(shBufferIn,(char*)(pInBuffer+i),320*sizeof(short));
				//Ê×ÏÈÐèÒªÊ¹ÓÃÂË²¨º¯Êý½«ÒôÆµÊý¾Ý·Ö¸ßµÍÆµ£¬ÒÔ¸ßÆµºÍµÍÆµµÄ·½Ê½´«Èë½µÔëº¯ÊýÄÚ²¿
				WebRtcSpl_AnalysisQMF(shBufferIn,320,shInL,shInH,filter_state1,filter_state12);

				//½«ÐèÒª½µÔëµÄÊý¾ÝÒÔ¸ßÆµºÍµÍÆµ´«Èë¶ÔÓ¦½Ó¿Ú£¬Í¬Ê±ÐèÒª×¢Òâ·µ»ØÊý¾ÝÒ²ÊÇ·Ö¸ßÆµºÍµÍÆµ
				if (0 == WebRtcNs_Process(pNS_inst ,shInL  ,shInH ,shOutL , shOutH))
				{
					short shBufferOut[320];
					//Èç¹û½µÔë³É¹¦£¬Ôò¸ù¾Ý½µÔëºó¸ßÆµºÍµÍÆµÊý¾Ý´«ÈëÂË²¨½Ó¿Ú£¬È»ºóÓÃ½«·µ»ØµÄÊý¾ÝÐ´ÈëÎÄ¼þ
					WebRtcSpl_SynthesisQMF(shOutL,shOutH,160,shBufferOut,Synthesis_state1,Synthesis_state12);
					memcpy(pOutBuffer+i,shBufferOut,320*sizeof(short));
				}
			}	
		}

		nTime = GetTickCount() - nTime;
		printf("n_s user time=%dms\n",nTime);
		fpOut = fopen(szFileOut, "wb");
		if (NULL == fpOut)
		{
			printf("open out file err! \n");
			break;
		}
		fwrite(pOutBuffer, sizeof(char), nFileSize, fpOut);
	} while (0);

	WebRtcNs_Free(pNS_inst);
	fclose(fpIn);
	fclose(fpOut);
	free(pInBuffer);
	free(pOutBuffer);
}

void NoiseSuppressionX32(char *szFileIn,char *szFileOut,int nSample,int nMode)
{
	int nRet = 0;
	NsxHandle *pNS_inst = NULL;

	FILE *fpIn = NULL;
	FILE *fpOut = NULL;

	char *pInBuffer =NULL;
	char *pOutBuffer = NULL;

	do
	{
		int i = 0;
		int nFileSize = 0;
		int nTime = 0;
		if (0 != WebRtcNsx_Create(&pNS_inst))
		{
			printf("Noise_Suppression WebRtcNs_Create err! \n");
			break;
		}

		if (0 !=  WebRtcNsx_Init(pNS_inst,nSample))
		{
			printf("Noise_Suppression WebRtcNs_Init err! \n");
			break;
		}

		if (0 !=  WebRtcNsx_set_policy(pNS_inst,nMode))
		{
			printf("Noise_Suppression WebRtcNs_set_policy err! \n");
			break;
		}

		fpIn = fopen(szFileIn, "rb");
		if (NULL == fpIn)
		{
			printf("open src file err \n");
			break;
		}
		fseek(fpIn,0,SEEK_END);
		nFileSize = ftell(fpIn); 
		fseek(fpIn,0,SEEK_SET); 

		pInBuffer = (char*)malloc(nFileSize);
		memset(pInBuffer,0,nFileSize);
		fread(pInBuffer, sizeof(char), nFileSize, fpIn);

		pOutBuffer = (char*)malloc(nFileSize);
		memset(pOutBuffer,0,nFileSize);

		int  filter_state1[6],filter_state12[6];
		int  Synthesis_state1[6],Synthesis_state12[6];

		memset(filter_state1,0,sizeof(filter_state1));
		memset(filter_state12,0,sizeof(filter_state12));
		memset(Synthesis_state1,0,sizeof(Synthesis_state1));
		memset(Synthesis_state12,0,sizeof(Synthesis_state12));

		nTime = GetTickCount();
		for (i = 0;i < nFileSize;i+=640)
		{
			if (nFileSize - i >= 640)
			{
				short shBufferIn[320] = {0};

				short shInL[160],shInH[160];
				short shOutL[160] = {0},shOutH[160] = {0};

				memcpy(shBufferIn,(char*)(pInBuffer+i),320*sizeof(short));
				//Ê×ÏÈÐèÒªÊ¹ÓÃÂË²¨º¯Êý½«ÒôÆµÊý¾Ý·Ö¸ßµÍÆµ£¬ÒÔ¸ßÆµºÍµÍÆµµÄ·½Ê½´«Èë½µÔëº¯ÊýÄÚ²¿
				WebRtcSpl_AnalysisQMF(shBufferIn,320,shInL,shInH,filter_state1,filter_state12);

				//½«ÐèÒª½µÔëµÄÊý¾ÝÒÔ¸ßÆµºÍµÍÆµ´«Èë¶ÔÓ¦½Ó¿Ú£¬Í¬Ê±ÐèÒª×¢Òâ·µ»ØÊý¾ÝÒ²ÊÇ·Ö¸ßÆµºÍµÍÆµ
				if (0 == WebRtcNsx_Process(pNS_inst ,shInL  ,shInH ,shOutL , shOutH))
				{
					short shBufferOut[320];
					//Èç¹û½µÔë³É¹¦£¬Ôò¸ù¾Ý½µÔëºó¸ßÆµºÍµÍÆµÊý¾Ý´«ÈëÂË²¨½Ó¿Ú£¬È»ºóÓÃ½«·µ»ØµÄÊý¾ÝÐ´ÈëÎÄ¼þ
					WebRtcSpl_SynthesisQMF(shOutL,shOutH,160,shBufferOut,Synthesis_state1,Synthesis_state12);
					memcpy(pOutBuffer+i,shBufferOut,320*sizeof(short));
				}
			}	
		}

		nTime = GetTickCount() - nTime;
		printf("n_s user time=%dms\n",nTime);
		fpOut = fopen(szFileOut, "wb");
		if (NULL == fpOut)
		{
			printf("open out file err! \n");
			break;
		}
		fwrite(pOutBuffer, sizeof(char), nFileSize, fpOut);
	} while (0);

	WebRtcNsx_Free(pNS_inst);
	fclose(fpIn);
	fclose(fpOut);
	free(pInBuffer);
	free(pOutBuffer);
}

void WebRtcAgcTest(char *filename, char *outfilename,int fs)
{
	FILE *infp      = NULL;
	FILE *outfp     = NULL;

	short *pData    = NULL;
	short *pOutData = NULL;
	void *agcHandle = NULL;	

	do 
	{
		WebRtcAgc_Create(&agcHandle);

		int minLevel = 0;
		int maxLevel = 255;
		int agcMode  = kAgcModeFixedDigital;
		WebRtcAgc_Init(agcHandle, minLevel, maxLevel, agcMode, fs);

		WebRtcAgc_config_t agcConfig;
		agcConfig.compressionGaindB = 20;
		agcConfig.limiterEnable     = 1;
		agcConfig.targetLevelDbfs   = 3;
		WebRtcAgc_set_config(agcHandle, agcConfig);

		infp = fopen(filename,"rb");
		int frameSize = 80;
		pData    = (short*)malloc(frameSize*sizeof(short));
		pOutData = (short*)malloc(frameSize*sizeof(short));

		outfp = fopen(outfilename,"wb");
		int len = frameSize*sizeof(short);
		int micLevelIn = 0;
		int micLevelOut = 0;
		while(TRUE)
		{
			memset(pData, 0, len);
			len = fread(pData, 1, len, infp);
			if (len > 0)
			{
				int inMicLevel  = micLevelOut;
				int outMicLevel = 0;
				uint8_t saturationWarning;
				int nAgcRet = WebRtcAgc_Process(agcHandle, pData, NULL, frameSize, pOutData,NULL, inMicLevel, &outMicLevel, 0, &saturationWarning);
				if (nAgcRet != 0)
				{
					printf("failed in WebRtcAgc_Process\n");
					break;
				}
				micLevelIn = outMicLevel;
				fwrite(pOutData, 1, len, outfp);
			}
			else
			{
				break;
			}
		}
	} while (0);

	fclose(infp);
	fclose(outfp);
	free(pData);
	free(pOutData);
	WebRtcAgc_Free(agcHandle);
}

int InConvertFormat(char* pszFile)
{
	char* pszPostfix = strrchr(pszFile, '.');

	char szExecFile[512] = "../ffmpeg.exe -hide_banner";
	strcat(szExecFile, " -y -i ");
	strcat(szExecFile, pszFile);
	strcat(szExecFile, " -acodec pcm_s16le -ar 32000 -ac 1 -f s16le ");
	strncat(szExecFile, pszFile, pszPostfix - pszFile);
	strcat(szExecFile, "_converted.pcm");

	DWORD dwExitCode = DWORD(-1);
	PROCESS_INFORMATION pi = { 0 };
	STARTUPINFOA si = { 0 };
	si.cb = sizeof(si);
	si.dwFlags = STARTF_USESHOWWINDOW;	//Ö¸¶¨wShowWindow³ÉÔ±ÓÐÐ§
	si.wShowWindow = FALSE;				//´Ë³ÉÔ±ÉèÎªTRUEµÄ»°ÔòÏÔÊ¾ÐÂ½¨½ø³ÌµÄÖ÷´°¿Ú
	if (CreateProcessA(nullptr, szExecFile, nullptr, nullptr, FALSE, CREATE_SUSPENDED, nullptr, nullptr, &si, &pi))
	{
		::ResumeThread(pi.hThread);
		WaitForSingleObject(pi.hProcess, INFINITE);
		GetExitCodeProcess(pi.hProcess, &dwExitCode);
		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);
	}
	else
	{
		dwExitCode = GetLastError();
	}
	return (int)dwExitCode;
}

int OutConvertFormat(char* pszFile)
{
	char* pszPostfix = strrchr(pszFile, '.');

	char szExecFile[512] = "../ffmpeg.exe -hide_banner";
	strcat(szExecFile, " -acodec pcm_s16le -ar 32000 -ac 1 -f s16le ");
	strcat(szExecFile, " -y -i ");
	strcat(szExecFile, pszFile);
	strcat(szExecFile, " ");
	strncat(szExecFile, pszFile, pszPostfix - pszFile);
	strcat(szExecFile, "_converted.aac");

	DWORD dwExitCode = DWORD(-1);
	PROCESS_INFORMATION pi = { 0 };
	STARTUPINFOA si = { 0 };
	si.cb = sizeof(si);
	si.dwFlags = STARTF_USESHOWWINDOW;	
	si.wShowWindow = FALSE;				
	if (CreateProcessA(nullptr, szExecFile, nullptr, nullptr, FALSE, CREATE_SUSPENDED, nullptr, nullptr, &si, &pi))
	{
		::ResumeThread(pi.hThread);
		WaitForSingleObject(pi.hProcess, INFINITE);
		GetExitCodeProcess(pi.hProcess, &dwExitCode);
		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);
	}
	else
	{
		dwExitCode = GetLastError();
	}
	return (int)dwExitCode;
}

int main(int argc, char* argv[])
{
	printf("Usage: WebRtcAudioTest.exe -i <32khz_1channel.pcm> <-m mode> -o <32khz_1channel_output.pcm>\n\n");

	char szInputFile[256] = { 0 };
	char szOutputFile[256] = { 0 };
	char szMode[256] = { 0 };
	int imode = 0;;
	for (int i = 0; i < argc; i++)
	{
		if (strcmp("-i", argv[i]) == 0)
		{
			strcpy(szInputFile, argv[++i]);
		}
		else if (strcmp("-o", argv[i]) == 0)
		{
			strcpy(szOutputFile, argv[++i]);
		}
		else if (strcmp("-m", argv[i]) == 0)
		{
			strcpy(szMode, argv[++i]);
			imode = atoi(szMode);
		}
	}
	if (strlen(szInputFile) > 0)
	{
		char* pszPostfix = strrchr(szInputFile, '.');
		if (pszPostfix != NULL && strcmp(".pcm", pszPostfix) != 0)
		{
			int i = InConvertFormat(szInputFile);
			assert(i == 0);
			strcpy(pszPostfix, "_converted.pcm");
		}
		if (strlen(szOutputFile) == 0)
		{
			pszPostfix = strrchr(szInputFile, '.');
			strncpy(szOutputFile, szInputFile, (pszPostfix - szInputFile));
			strcpy(szOutputFile + (pszPostfix - szInputFile), szMode);
			strcpy(szOutputFile + strlen(szOutputFile), "_out.pcm");
		}
		NoiseSuppression32(szInputFile, szOutputFile, 32000, imode);
	}
	else
	{
		NoiseSuppression32("0163_32khz_1ch_s16le.pcm", "0163_32khz_1ch_s16le_out_1.pcm", 32000, 1);
	}
	//WebRtcAgcTest("byby_8K_1C_16bit.pcm","byby_8K_1C_16bit_agc.pcm",8000);

	//NoiseSuppression32("lhydd_1C_16bit_32K.PCM","lhydd_1C_16bit_32K_ns.pcm",32000,1);

	//NoiseSuppressionX32("lhydd_1C_16bit_32K.PCM","lhydd_1C_16bit_32K_nsx.pcm",32000,1);

	OutConvertFormat(szOutputFile);

	printf("denoise over ...\n");
	getchar();
	return 0;
}

