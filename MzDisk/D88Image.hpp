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
		// 定数
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
	// D88ヘッダ情報
	struct Header
	{
		char name[17]; // ディスクの名前(ASCII + '\0')
		char reserve[9];
		char writeProtect; // ライトプロテクト  0x00 なし, 0x10 あり
		char diskType; // ディスクの種類 0x00 2D, 0x10 2DD, 0x20 2HD, 0x30 1D, 0x40 1DD
		int diskSize; // ディスクのサイズ
		int trackTable[TRACK_MAX]; // トラック部のオフセットテーブル (0Track～163Track, 4*164 = 656(0x290))
	};
	// D88セクタ情報
	struct SectorInfo
	{
		char c; // シリンダ番号
		char h; // ヘッド番号
		char r; // セクタ番号 (1～セクタ数分)
		char n; // セクタサイズ 0 128bytes, 1 256bytes, 2 512bytes, 3 1024bytes, (128 << N)
		short numberOfSector; // 1トラックのセクタ数
		char recordingDensity; // 記録密度 0x00 倍密度, 0x40 単密度
		char deletedMark; // 削除フラグ 0x00 通常, 0x10 削除
		char status; // ステータス 0x00 正常, 0x10 正常(DELETED DATA), 0xA0 ID CRC エラー, 0xB0 データ CRC エラー, 0xE0 アドレスマークなし, 0xF0 データマークなし
		char reserve[5];
		short sizeOfData; // このセクタのデータサイズ
	};
#ifdef _MSC_VER
	#pragma pack(pop)
#else
	#pragma pack(0)
#endif
	// セクタデータ
	struct SectorImage
	{
		SectorInfo sectorInfo = {};
		std::vector<unsigned char> sectorBuffer;
	};
	// トラックデータ
	struct TrackImage
	{
		std::vector<SectorImage> sectorImage;
	};
	// ディスクデータ
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
