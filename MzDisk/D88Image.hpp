#ifndef D88IMAGE_HPP
#define D88IMAGE_HPP

#include <string>
#include <vector>
#include "FileData.hpp"

class D88Image
{
public:
	enum
	{
		// �萔
		TRACK_MAX = 164,
		// DiskType
		DISK_2S = 0x0,
		DISK_2D = 0x0,
		DISK_2DD = 0x10,
		DISK_2HD = 0x20,
		DISK_1D = 0x30,
		DISK_1DD = 0x40,
		// MediaType
		DISK_2DD_80_256_16 = 0,	// MZ-2500
		DISK_2DD_40_256_16,		// MZ-2500
		DISK_2DD_35_256_16,		// MZ-2500
		DISK_2D_35_256_16,		// MZ-80B (MZ-80BF)
		DISK_2D_40_256_16,		// MZ-2000 (MZ-1F07)
		DISK_2S_35_128_16,		// MZ-80K
		DISK_2HD_77_1024_8,		// PC-9801
		DISK_2HD_80_512_15,		// PC/AT 5'
		DISK_2HD_80_512_18		// PC/AT 3.5'
	};
#ifdef _MSC_VER
	#pragma pack(push, 1)
#else
	#pragma pack(1)
#endif
	// D88�w�b�_���
	struct Header
	{
		char name[17]; // �f�B�X�N�̖��O(ASCII + '\0')
		char reserve[9];
		char writeProtect; // ���C�g�v���e�N�g  0x00 �Ȃ�, 0x10 ����
		char diskType; // �f�B�X�N�̎�� 0x00 2D, 0x10 2DD, 0x20 2HD, 0x30 1D, 0x40 1DD
		int diskSize; // �f�B�X�N�̃T�C�Y
		int trackTable[TRACK_MAX]; // �g���b�N���̃I�t�Z�b�g�e�[�u�� (0Track�`163Track, 4*164 = 656(0x290))
	};
	// D88�Z�N�^���
	struct SectorInfo
	{
		char c; // �V�����_�ԍ�
		char h; // �w�b�h�ԍ�
		char r; // �Z�N�^�ԍ� (1�`�Z�N�^����)
		char n; // �Z�N�^�T�C�Y 0 128bytes, 1 256bytes, 2 512bytes, 3 1024bytes, (128 << N)
		short numberOfSector; // 1�g���b�N�̃Z�N�^��
		char recordingDensity; // �L�^���x 0x00 �{���x, 0x40 �P���x
		char deletedMark; // �폜�t���O 0x00 �ʏ�, 0x10 �폜
		char status; // �X�e�[�^�X 0x00 ����, 0x10 ����(DELETED DATA), 0xA0 ID CRC �G���[, 0xB0 �f�[�^ CRC �G���[, 0xE0 �A�h���X�}�[�N�Ȃ�, 0xF0 �f�[�^�}�[�N�Ȃ�
		char reserve[5];
		short sizeOfData; // ���̃Z�N�^�̃f�[�^�T�C�Y
	};
#ifdef _MSC_VER
	#pragma pack(pop)
#else
	#pragma pack(0)
#endif
	// �Z�N�^�f�[�^
	struct SectorImage
	{
		SectorInfo sectorInfo = {};
		std::vector<unsigned char> sectorBuffer;
	};
	// �g���b�N�f�[�^
	struct TrackImage
	{
		std::vector<SectorImage> sectorImage;
	};
	// �f�B�X�N�f�[�^
	struct DiskImage
	{
		Header header = {};
		std::vector<TrackImage> trackData;
	};
	D88Image(void);
	~D88Image(void);
	void Load(std::string path);
	void Load(const void* buffer, size_t bufferSize);
	void Save(std::string path);
	void Save(std::vector<unsigned char>& buffer);
	void Format(int mediaType, unsigned char clearByte);
	void Format(int diskType, int track, int sectorCount, int sectorSize, unsigned char clearByte);
	void GetHeader(Header& header) const;
	int GetNumberOfSector(int track);
	void GetSectorInfo(SectorInfo& sectorInfo, int c, int h, int r, int index = 0) const;
	void ReadSector(SectorInfo& sectorInfo, std::vector<unsigned char>& buffer, int c, int h, int r, int index = 0) const;
	void WriteSector(const SectorInfo& sectorInfo, const std::vector<unsigned char>& buffer, int c, int h, int r);
	bool IsValid(void);
private:
	D88Image(D88Image&);
	D88Image& operator = (D88Image&);
	int GetTrack(int c, int h) const;
	void GetCH(int& c, int& h, int track) const;
	int GetNumberOfSector(int track, const void* diskImageBuffer) const;
	bool FindSectorData(SectorInfo& sectorInfo, size_t& sectorBufferOffset, const void* diskImageBuffer, int c, int h, int r, int index = 0) const;
	DiskImage diskImage;
};

#endif
