#include "PPSound.h"

CPPSound::CPPSound()
{
	DirectSoundCreate(NULL, &pDs, NULL);
	pDs->SetCooperativeLevel(GetDesktopWindow(), DSSCL_PRIORITY);
}

LPDIRECTSOUNDBUFFER  CPPSound::LoadSound(const wchar_t* strFilePath)
{
	HMMIO hMmio;
	MMRESULT mmResult;
	hMmio = mmioOpen(const_cast<wchar_t*>(strFilePath), NULL, MMIO_ALLOCBUF | MMIO_READ);
	if (!hMmio)
		return NULL;

	MMCKINFO mmCkRiff;//WAVE�ļ���Ϣ�ṹ
	mmCkRiff.fccType = mmioFOURCC('W', 'A', 'V', 'E');
	mmResult = mmioDescend(hMmio, &mmCkRiff, NULL, MMIO_FINDRIFF);
	if (mmResult != MMSYSERR_NOERROR)
		return NULL;

	MMCKINFO mmCkInfo;
	mmCkInfo.ckid = mmioFOURCC('f', 'm', 't', ' ');//�趨��������
	mmResult = mmioDescend(hMmio, &mmCkInfo, &mmCkRiff, MMIO_FINDCHUNK);//�˷����˳�����
	if (mmResult != MMSYSERR_NOERROR)
		return NULL;

	//��ȡ��Ƶ��ʽ
	WAVEFORMATEX oFormat;
	if (mmioRead(hMmio, (HPSTR)&oFormat, sizeof(oFormat)) == -1)
		return NULL;

	mmioAscend(hMmio, &mmCkInfo, 0);
	mmCkInfo.ckid = mmioFOURCC('d', 'a', 't', 'a');
	mmResult = mmioDescend(hMmio, &mmCkInfo, &mmCkRiff, MMIO_FINDCHUNK);
	if (mmResult != MMSYSERR_NOERROR)
		return NULL;

	//������Ƶ�λ�����
	LPDIRECTSOUNDBUFFER pTempBuf;
	DSBUFFERDESC desc;//���������������ṹ
	memset(&desc, 0, sizeof(desc));
	desc.dwSize = sizeof(desc);
	desc.lpwfxFormat = &oFormat;
	desc.dwFlags = DSBCAPS_STATIC;//static��ʾ�ɶ�β��ţ���Ȼ������ָ��������ʹ��| ����
	desc.dwBufferBytes = mmCkInfo.cksize;
	HRESULT hRes = pDs->CreateSoundBuffer(&desc, &pTempBuf, NULL);
	if (hRes != DS_OK)
		return NULL;

	// ���ļ���ȡ��Ƶ���ݴ���λ�����
	LPVOID pAudio;
	DWORD BytesAudio;
	pTempBuf->Lock(0, mmCkInfo.cksize, &pAudio, &BytesAudio, NULL, NULL, NULL);
	if (mmioRead(hMmio, (HPSTR)pAudio, BytesAudio) == -1)
		return NULL;
	pTempBuf->Unlock(pAudio, BytesAudio, NULL, NULL);

	//�ر��ļ�
	mmioClose(hMmio, 0);

	return pTempBuf;
}

BOOL CPPSound::PlaySond(CPPString& strFilePath)
{
	return PlaySond(strFilePath.GetString());
}

BOOL CPPSound::PlaySond(const wchar_t* strFilePath)
{
	pDSB = LoadSound(strFilePath);
	return pDSB->Play(0, 0, 1);
}