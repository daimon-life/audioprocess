#include "webrtc_ns/legacy_ns/noise_suppression.h"
#include "webrtc_ns/legacy_ns/noise_suppression_x.h"
#include "webrtc_ns/legacy_ns/signal_processing_library.h"

#include <cassert>
#include <fstream>
#include <algorithm>>

static __inline int16_t _FloatS16ToS16(float v) {
	v = std::min(v, 32767.f);
	v = std::max(v, -32768.f);
	return static_cast<int16_t>(v + std::copysign(0.5f, v));
}

void FloatS16ToS16(const float* src, int size, short* dst)
{
	for (size_t i = 0; i < size; ++i)
		dst[i] = _FloatS16ToS16(src[i]);
}

void S16ToFloatS16(const short* src, int size, float* dst)
{
	for (size_t i = 0; i < size; ++i)
		dst[i] = src[i];
}

void NoiseSuppression32khz(const char* pszFileIn, const char* pszFileOut, int nMode)
{
	if (pszFileIn == nullptr || pszFileOut == nullptr) return;

	NsHandle* pNsHandle = WebRtcNs_Create();
	assert(pNsHandle != nullptr);
	WebRtcNs_Init(pNsHandle, 32000);
	WebRtcNs_set_policy(pNsHandle, nMode);

	std::fstream fileI, fileO;
	fileI.open(pszFileIn, std::fstream::in | std::fstream::binary);
	fileO.open(pszFileOut, std::fstream::out | std::fstream::binary);
	assert(fileI.is_open() && fileO.is_open());

	short pcms16leBuf[320]    = { 0 };

	short shInL[160], shInH[160];
	short shOutL[160] = { 0 };
	short shOutH[160] = { 0 };

	int  filter_state1[6], filter_state12[6];
	int  Synthesis_state1[6], Synthesis_state12[6];

	memset(filter_state1, 0, sizeof(filter_state1));
	memset(filter_state12, 0, sizeof(filter_state12));
	memset(Synthesis_state1, 0, sizeof(Synthesis_state1));
	memset(Synthesis_state12, 0, sizeof(Synthesis_state12));

	float fltInL[160], fltInH[160];
	float fltOutL[160] = { 0 };
	float fltOutH[160] = { 0 };

	float* inspFrame[2] = { fltInL, fltInH };
	float* outspFrame[2] = { fltOutL, fltOutH };

	const float kSpeechProbabilityReference = 0.67999554f;
	const float kNoiseEstimateReference[] = { 2149.780518f, 7076.936035f,
		14939.945312f };
	const float kOutputReference[] = { 0.001221f, 0.001984f, 0.002228f };

	do
	{
		// read to buffer from file
		fileI.read((char*)pcms16leBuf, 320*sizeof(short));
		int iReadSize = fileI.gcount();
		assert(iReadSize%sizeof(short) == 0);
		int iActualPrcessSamples = iReadSize / sizeof(short);
		if (iActualPrcessSamples < 320)
		{
			//memset(pcms16leBuf + iReadSize, 0, 320 * sizeof(short) - iReadSize);
			fileO.write((char*)pcms16leBuf, iReadSize);
			break;
		}
		// process
		WebRtcSpl_AnalysisQMF(pcms16leBuf, 320, shInL, shInH, filter_state1, Synthesis_state12);
		S16ToFloatS16(shInL, 160, fltInL);
		S16ToFloatS16(shInH, 160, fltInH);
		WebRtcNs_Analyze(pNsHandle, fltInL);
		WebRtcNs_Process(pNsHandle, inspFrame, 2, outspFrame);
		FloatS16ToS16(fltOutL, 160, shOutL);
		FloatS16ToS16(fltOutH, 160, shOutH);
		short shBufferOut[320];
		WebRtcSpl_SynthesisQMF(shOutL, shOutH, 160, shBufferOut, Synthesis_state1, Synthesis_state12);
		// wirte to file from buffer
		fileO.write((char*)shBufferOut, iReadSize);
	} while (!fileI.eof());

	fileO.close();
	fileI.close();

	WebRtcNs_Free(pNsHandle);
}


void NoiseSuppression16khz(const char* pszFileIn, const char* pszFileOut, int nMode)
{
	if (pszFileIn == nullptr || pszFileOut == nullptr) return;

	NsHandle* pNsHandle = WebRtcNs_Create();
	assert(pNsHandle != nullptr);
	WebRtcNs_Init(pNsHandle, 16000);
	WebRtcNs_set_policy(pNsHandle, nMode);

	std::fstream fileI, fileO;
	fileI.open(pszFileIn, std::fstream::in | std::fstream::binary);
	fileO.open(pszFileOut, std::fstream::out | std::fstream::binary);
	assert(fileI.is_open() && fileO.is_open());

	short pcms16leBuf[320] = { 0 };

	int  filter_state1[6], filter_state12[6];
	int  Synthesis_state1[6], Synthesis_state12[6];

	memset(filter_state1, 0, sizeof(filter_state1));
	memset(filter_state12, 0, sizeof(filter_state12));
	memset(Synthesis_state1, 0, sizeof(Synthesis_state1));
	memset(Synthesis_state12, 0, sizeof(Synthesis_state12));

	float fltIn[160];
	float fltOut[160] = { 0 };

	const float kSpeechProbabilityReference = 0.67999554f;
	const float kNoiseEstimateReference[] = { 2149.780518f, 7076.936035f,
		14939.945312f };
	const float kOutputReference[] = { 0.001221f, 0.001984f, 0.002228f };

	do
	{
		float* inspFrame[1] = { fltIn };
		float* outspFrame[1] = { fltOut };

		// read to buffer from file
		fileI.read((char*)pcms16leBuf, 160 * sizeof(short));
		int iReadSize = fileI.gcount();
		assert(iReadSize%sizeof(short) == 0);
		int iActualPrcessSamples = iReadSize / sizeof(short);
		if (iActualPrcessSamples < 160)
		{
			//memset(pcms16leBuf + iReadSize, 0, 320 * sizeof(short) - iReadSize);
			fileO.write((char*)pcms16leBuf, iReadSize);
			break;
		}
		// process
		S16ToFloatS16(pcms16leBuf, 160, fltIn);
		WebRtcNs_Analyze(pNsHandle, fltIn);
		WebRtcNs_Process(pNsHandle, inspFrame, 1, outspFrame);
		FloatS16ToS16(fltOut, 160, pcms16leBuf);
		// wirte to file from buffer
		fileO.write((char*)pcms16leBuf, iReadSize);
	} while (!fileI.eof());

	fileO.close();
	fileI.close();

	WebRtcNs_Free(pNsHandle);
}
