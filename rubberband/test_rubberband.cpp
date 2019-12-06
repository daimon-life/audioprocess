#include "RubberBandStretcher.h"

#include <iostream>
#include <fstream>
#include <cassert>
//#include <cstring>

using namespace RubberBand;

#define SAMPLE_RATE    44100
#define CHANNEL_COUNT  2

// 
int main(int argc, char** argv)
{
	// 打开文件
	std::fstream fIPcmAudio, fOPcmAudio;
	fIPcmAudio.open("./input2.pcm", std::fstream::in  | std::fstream::binary);
	fOPcmAudio.open("./output2.pcm", std::fstream::out | std::fstream::binary);
	assert(fIPcmAudio.is_open() && fOPcmAudio.is_open());
	// 逐步伸缩器
	double dTimeRatio = 1 / 1.889996;
	RubberBand::RubberBandStretcher* pStretcher = nullptr;
	pStretcher = new RubberBandStretcher(SAMPLE_RATE, CHANNEL_COUNT,
		RubberBandStretcher::OptionProcessRealTime |
		RubberBandStretcher::OptionPitchHighConsistency |
		RubberBandStretcher::OptionStretchPrecise);
	pStretcher->reset();
	pStretcher->setTimeRatio(dTimeRatio);
	pStretcher->setPitchOption(RubberBandStretcher::OptionPitchHighConsistency);

	// 数据存取
	float* pData = nullptr;
	int    iSize = 0;
	float* pOutData = nullptr;
	int    iOutSize = 0;
	float* input[CHANNEL_COUNT] = { 0 };
	float* output[CHANNEL_COUNT] = { 0 };
	char* pReadData = nullptr;
	int   iReadSize = 0;
	char* pWriteData = nullptr;
	int   iWriteSize = 0;
	bool  bEnd = false;
	// 统计数据
	int   iIndex = 0;
	int   iISize = 0;
	int   iOSize = 0;
	
	// 处理
	do 
	{
	int iRequiredSize = pStretcher->getSamplesRequired();
	// 分配输入数据存取
	if (iSize < iRequiredSize*CHANNEL_COUNT)
	{
		if (pData) delete[] pData;
		iSize = iRequiredSize*CHANNEL_COUNT;
		pData = new float[iSize];
	}
	for (int i = 0; i < CHANNEL_COUNT; i++)
	{
		input[i] = pData + i * iSize / CHANNEL_COUNT;
	}
	// 分配从文件中读取到的数据存取
	if (pReadData == nullptr || iReadSize < iRequiredSize*CHANNEL_COUNT*sizeof(float))
	{
		if (pReadData) delete[] pReadData;
		iReadSize = iSize*sizeof(float);
		pReadData = new char[iReadSize];
	}
	// 从文件中读取数据
	fIPcmAudio.read((char*)pReadData, iRequiredSize*CHANNEL_COUNT*sizeof(float));
	int iActualReadSize = fIPcmAudio.gcount();
	assert(iActualReadSize % (CHANNEL_COUNT*sizeof(float)) == 0);
	if (iActualReadSize == 0) break;
	if (iActualReadSize < iRequiredSize*CHANNEL_COUNT*sizeof(float))
	{
		bEnd = true;
		//std::memset(pReadData + iActualReadSize, 0, iRequiredSize*CHANNEL_COUNT*sizeof(float) - iActualReadSize);
		//iActualReadSize = iRequiredSize*CHANNEL_COUNT*sizeof(float);
	}
	int iSampleCount = iActualReadSize / (CHANNEL_COUNT*sizeof(float));
	// 将文件中数据转换到输入数据
	for (int i = 0; i < CHANNEL_COUNT; i++)
	{
		float* pReadDataTmp = (float*)pReadData;
		for (int j = 0; j < iSampleCount; j++)
		{
			input[i][j] = *(pReadDataTmp + CHANNEL_COUNT*j + i);
		}
	}
	// 分配输出数据存取
	if (dTimeRatio*iRequiredSize*CHANNEL_COUNT > iOutSize)
	{
		if (pOutData) delete[] pOutData;
		iOutSize = dTimeRatio*iRequiredSize*CHANNEL_COUNT + 10 * CHANNEL_COUNT;
		pOutData = new float[iOutSize];
	}
	for (int i = 0; i < CHANNEL_COUNT; i++)
	{
		output[i] = pOutData + i * iOutSize / CHANNEL_COUNT;
	}
	pStretcher->process(input, iActualReadSize / (CHANNEL_COUNT*sizeof(float)), bEnd);
	int iAvail = pStretcher->available();
	pStretcher->retrieve(output, iAvail);
	// 分配从文件中写到的数据存取
	if (pWriteData == nullptr || iWriteSize < iAvail*CHANNEL_COUNT*sizeof(float))
	{
		iWriteSize = iAvail*CHANNEL_COUNT*sizeof(float);
		pWriteData = new char[iWriteSize];
	}
	// 将输出数据转换到文件中数据
	for (int i = 0; i < CHANNEL_COUNT; i++)
	{
		float* pWriteDataTmp = (float*)pWriteData;
		for (int j = 0; j < iAvail; j++)
		{
			*(pWriteDataTmp + CHANNEL_COUNT*j + i) = output[i][j];
		}
	}
	fOPcmAudio.write(pWriteData, iAvail*CHANNEL_COUNT*sizeof(float));


	if (iIndex % 4 == 0)
	{
		std::cout << std::endl;
	}
	iIndex++;
	std::cout << "in: " << iRequiredSize << " - out: " << iAvail << "         ";
	iISize += !bEnd ? iRequiredSize : iActualReadSize / (CHANNEL_COUNT*sizeof(float));
	iOSize += iAvail;
	} while (!fIPcmAudio.eof());

	if (iIndex % 4 != 0)
	{
		std::cout << std::endl;
	}
	std::cout << iISize*CHANNEL_COUNT*sizeof(float) << " bytes - " << iOSize*CHANNEL_COUNT*sizeof(float) << " bytes   ";
	std::cout << "I: " << iISize << " - O: " << iOSize << std::endl;

	// 
	if (pWriteData)
	{
		delete[] pWriteData;
		pWriteData = nullptr;
	}
	if (pReadData)
	{
		delete[] pReadData;
		pReadData = nullptr;
	}
	if (pData != nullptr)
	{
		delete[] pData;
		pData = nullptr;
		iSize = 0;
	}
	fIPcmAudio.close();
	fOPcmAudio.close();

	return 0;
}
