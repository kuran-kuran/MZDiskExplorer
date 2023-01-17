#include <stdexcept>
#include <iterator>
#include "Format.hpp"
#include "D88Image.hpp"

const int D88Image::diskTypeTable[] = {DISK_2DD, DISK_2DD, DISK_2DD, DISK_2D, DISK_2D, DISK_2S, DISK_2HD, DISK_2HD, DISK_2HD};
const unsigned char D88Image::recordingDensityTable[] = {0x0, 0x0, 0x0, 0x40, 0x40, 0x40, 0x01, 0x01, 0x01};
const int D88Image::trackTable[] = {160, 80, 70, 70, 80, 70, 154, 160, 160};
const int D88Image::sectorCountTable[] = {16, 16, 16, 16, 16, 16, 8, 15, 18};
const int D88Image::sectorSizeTable[] = {256, 256, 256, 256, 256, 128, 1024, 512, 512};

D88Image::D88Image(void)
:diskImage()
{
}

D88Image::~D88Image(void)
{
}

// D88ファイル読み込み
void D88Image::Load(std::string path)
{
	dms::FileData fileData;
	fileData.Load(path);
	Load(fileData.GetBuffer(), fileData.GetBufferSize());
}

// メモリからのD88ファイル読み込み
void D88Image::Load(const void* buffer, size_t bufferSize)
{
	memcpy_s(&this->diskImage.header, sizeof(this->diskImage.header), buffer, sizeof(diskImage.header));
	this->diskImage.trackData.resize(TRACK_COUNT);
	for(int track = 0; track < TRACK_COUNT; ++ track)
	{
		int numberOfSector = GetNumberOfSector(track, buffer);
		if(numberOfSector == 0)
		{
			break;
		}
		for(int sectorIndex = 0; sectorIndex < numberOfSector; ++ sectorIndex)
		{
			int c = 0;
			int h = 0;
			GetCH(c, h, track);
			SectorImage sectorImage;
			size_t sectorBufferOffset;
			FindSectorData(sectorImage.sectorInfo, sectorBufferOffset, buffer, c, h, -1, sectorIndex);
			const unsigned char* imageBuffer = reinterpret_cast<const unsigned char*>(buffer);
			std::copy(&imageBuffer[sectorBufferOffset], &imageBuffer[sectorBufferOffset] + sectorImage.sectorInfo.sizeOfData, std::back_inserter(sectorImage.sectorBuffer));
			this->diskImage.trackData[track].sectorImage.push_back(sectorImage);
		}
	}
}

// D88ファイル書き込み
void D88Image::Save(std::string path)
{
	std::vector<unsigned char> buffer;
	Save(buffer);
	dms::FileData fileData;
	fileData.SetBuffer(&buffer[0], buffer.size());
	fileData.Save(path);
}

// D88ファイルをbufferに展開
void D88Image::Save(std::vector<unsigned char>& buffer)
{
	buffer.clear();
	// ヘッダ
	unsigned char* source = reinterpret_cast<unsigned char*>(&this->diskImage.header);
	std::copy(source, source + sizeof(this->diskImage.header), std::back_inserter(buffer));
	// トラック
	for(int track = 0; track < TRACK_COUNT; ++ track)
	{
		int numberOfSector = static_cast<int>(this->diskImage.trackData[track].sectorImage.size());
		for(int sectorIndex = 0; sectorIndex < numberOfSector; ++ sectorIndex)
		{
			SectorImage& sectorImage = this->diskImage.trackData[track].sectorImage[sectorIndex];
			unsigned char* source = reinterpret_cast<unsigned char*>(&sectorImage.sectorInfo);
			// セクタヘッダ
			std::copy(source, source + sizeof(sectorImage.sectorInfo), std::back_inserter(buffer));
			// セクタデータ
			std::copy(sectorImage.sectorBuffer.begin(), sectorImage.sectorBuffer.end(), std::back_inserter(buffer));
		}
	}
}

// mediaType: DISK_2DD_80_256_16, DISK_2DD_40_256_16, DISK_2DD_35_256_16, DISK_2D_35_256_16,
//			  DISK_2D_40_256_16, DISK_2S_35_128_16, DISK_2HD_77_1024_8, DISK_2HD_80_512_15, DISK_2HD_80_512_18
void D88Image::Format(int mediaType, unsigned char clearByte)
{
	std::vector<SectorInfo> sectorInfoList;
	Format(diskTypeTable[mediaType], recordingDensityTable[mediaType], trackTable[mediaType], sectorCountTable[mediaType], sectorSizeTable[mediaType], clearByte);
}

// D88イメージを初期化する
// diskType: DISK_2S, DISK_2D, DISK_2DD, DISK_2HD, DISK_1D, DISK_1DD
void D88Image::Format(int diskType, unsigned char recordingDensity, int track, int sectorCount, int sectorSize, unsigned char clearByte)
{
	memset(this->diskImage.header.name, 0, sizeof(this->diskImage.header.name));
	memset(this->diskImage.header.reserve, 0, sizeof(this->diskImage.header.reserve));
	this->diskImage.header.writeProtect = 0x00;
	this->diskImage.header.diskType = diskType;
	this->diskImage.header.diskSize = sizeof(diskImage.header) + (sectorSize + sizeof(SectorInfo))* sectorCount * track;
	this->diskImage.trackData.resize(TRACK_COUNT);
	int offset = sizeof(diskImage.header);
	int sector = sectorSize;
	int n = 0;
	while(sector > 128)
	{
		sector >>= 1;
		++ n;
	}
	for(int trackIndex = 0; trackIndex < TRACK_COUNT; ++ trackIndex)
	{
		TrackImage trackImage;
		this->diskImage.trackData[trackIndex].sectorImage.clear();
		if(trackIndex < track)
		{
			for(int sectorIndex = 0; sectorIndex < sectorCount; ++ sectorIndex)
			{
				SectorImage sectorImage;
				sectorImage.sectorInfo.c = trackIndex / 2;
				sectorImage.sectorInfo.h = trackIndex % 2;
				sectorImage.sectorInfo.r = sectorIndex + 1;
				sectorImage.sectorInfo.n = n;
				sectorImage.sectorInfo.numberOfSector = sectorCount;
				sectorImage.sectorInfo.recordingDensity = recordingDensity;
				sectorImage.sectorInfo.deletedMark = 0;
				sectorImage.sectorInfo.status = 0;
				memset(sectorImage.sectorInfo.reserve, 0, sizeof(sectorImage.sectorInfo.reserve));
				sectorImage.sectorInfo.sizeOfData = sectorSize;
				sectorImage.sectorBuffer.clear();
				sectorImage.sectorBuffer.resize(sectorSize, clearByte);
				this->diskImage.trackData[trackIndex].sectorImage.push_back(sectorImage);
			}
			this->diskImage.header.trackTable[trackIndex] = offset;
			offset += ((sectorSize + sizeof(SectorInfo))* sectorCount);
		}
		else
		{
			this->diskImage.header.trackTable[trackIndex] = 0;
		}
	}
}

void D88Image::GetHeader(D88Image::Header& header) const
{
	header = this->diskImage.header;
}

// type: DISK_2D, DISK_2DD
void D88Image::SetDiskType(int type)
{
	this->diskImage.header.diskType = type;
	int recordingDensity = 0x00; // 2D
	if(type == DISK_2DD)
	{
		recordingDensity = 0x40; // 2DD
	}
	int cylinderMax = TRACK_COUNT / 2;
	for(int c = 0; c < cylinderMax; ++ c)
	{
		for(int h = 0; h < 2; ++ h)
		{
			for(int r = 1; r <= 16; ++ r)
			{
				SectorInfo sectorInfo = {};
				GetSectorInfo(sectorInfo, c, h, r);
				if(sectorInfo.sizeOfData == 0)
				{
					continue;
				}
				sectorInfo.recordingDensity = recordingDensity;
				SetSectorInfo(sectorInfo, c, h, r);
			}
		}
	}
}

// r: セクタ番号(1～n) -1の場合はindex番のセクタ
// index: セクタ位置(0～n) rが-1の時に有効
void D88Image::GetSectorInfo(SectorInfo& sectorInfo, int c, int h, int r, int index) const
{
	int track = GetTrack(c, h);
	int findIndex = 0;
	for(const SectorImage& sectorImage: this->diskImage.trackData[track].sectorImage)
	{
		if(((r == -1) && (findIndex == index)) || (sectorImage.sectorInfo.r == r))
		{
			sectorInfo = sectorImage.sectorInfo;
			break;
		}
		++ findIndex;
	}
}

// r: セクタ番号(1～n) -1の場合はindex番のセクタ
// index: セクタ位置(0～n) rが-1の時に有効
void D88Image::SetSectorInfo(SectorInfo& sectorInfo, int c, int h, int r, int index)
{
	int track = GetTrack(c, h);
	int findIndex = 0;
	for(SectorImage& sectorImage: this->diskImage.trackData[track].sectorImage)
	{
		if(((r == -1) && (findIndex == index)) || (sectorImage.sectorInfo.r == r))
		{
			sectorImage.sectorInfo = sectorInfo;
			break;
		}
		++ findIndex;
	}
}

// r: セクタ番号(1～n) -1の場合はindex番のセクタ
// index: セクタ位置(0～n) rが-1の時に有効
void D88Image::ReadSector(SectorInfo& sectorInfo, std::vector<unsigned char>& buffer, int c, int h, int r, int index) const
{
	int track = GetTrack(c, h);
	int findIndex = 0;
	for(const SectorImage& sectorImage: this->diskImage.trackData[track].sectorImage)
	{
		if(((r == -1) && (findIndex == index)) || (sectorImage.sectorInfo.r == r))
		{
			buffer.clear();
			sectorInfo = sectorImage.sectorInfo;
			std::copy(sectorImage.sectorBuffer.begin(), sectorImage.sectorBuffer.end(), std::back_inserter(buffer));
			break;
		}
		++ findIndex;
	}
}

void D88Image::WriteSector(const SectorInfo& sectorInfo, const std::vector<unsigned char>& buffer, int c, int h, int r)
{
	int track = GetTrack(c, h);
	for(SectorImage& sectorImage: this->diskImage.trackData[track].sectorImage)
	{
		if(sectorImage.sectorInfo.r == r)
		{
			sectorImage.sectorInfo = sectorInfo;
			sectorImage.sectorBuffer.clear();
			std::copy(buffer.begin(), buffer.end(), std::back_inserter(sectorImage.sectorBuffer));
			break;
		}
	}
}

// chからトラック番号を取得する
int D88Image::GetTrack(int c, int h) const
{
	int track = c * 2 + h;
	return track;
}

// トラック番号からCHを取得
void D88Image::GetCH(int& c, int& h, int track) const
{
	c = track / 2;
	h = track % 2;
}

// トラック中のセクタ数取得
int D88Image::GetNumberOfSector(int track, const void* diskImageBuffer) const
{
	int c = 0;
	int h = 0;
	GetCH(c, h, track);
	SectorInfo sectorInfo;
	size_t sectorBufferOffset;
	if(FindSectorData(sectorInfo, sectorBufferOffset, diskImageBuffer, c, h, -1, 0) == false)
	{
		return 0;
	}
	return sectorInfo.numberOfSector;
}

// r: セクタ番号(1～n) -1の場合はindex番のセクタ
// index: セクタ位置(0～n) rが-1の時に有効
bool D88Image::FindSectorData(SectorInfo& sectorInfo, size_t& sectorBufferOffset, const void* diskImageBuffer, int c, int h, int r, int index) const
{
	sectorBufferOffset = 0;
	D88Image::Header header;
	GetHeader(header);
	int track = GetTrack(c, h);
	int trackOffset = header.trackTable[track];
	if(trackOffset == 0)
	{
		return false;
	}
	const unsigned char* imageBuffer = reinterpret_cast<const unsigned char*>(diskImageBuffer);
	if(imageBuffer == NULL)
	{
		throw std::runtime_error(dms::Format("Image buffer is NULL"));
	}
	const SectorInfo* seekSectorData = reinterpret_cast<const SectorInfo*>(&imageBuffer[trackOffset]);
	sectorBufferOffset += (trackOffset + sizeof(SectorInfo));
	int numberOfSector = seekSectorData->numberOfSector;
	bool findSector = false;
	size_t sectorOffset = 0;
	int findIndex = 0;
	for(int i = 0; i < numberOfSector; ++ i)
	{
		// 探しているセクタか、-1の場合は最初に見つかったセクタ
		if(((r == -1) && (findIndex == index)) || (seekSectorData->r == r))
		{
			// 見つかったので検索中断
			findSector = true;
			break;
		}
		// 次のセクタ
		sectorOffset += (sizeof(SectorInfo) + seekSectorData->sizeOfData);
		seekSectorData = reinterpret_cast<const SectorInfo*>(&imageBuffer[trackOffset + sectorOffset]);
		sectorBufferOffset += sizeof(SectorInfo) + seekSectorData->sizeOfData;
		++ findIndex;
	}
	if(findSector == true)
	{
		if(memcpy_s(&sectorInfo, sizeof(SectorInfo), seekSectorData, sizeof(SectorInfo)) != 0)
		{
			throw std::runtime_error(dms::Format("Can not memcpy sectorInfo"));
		}
	}
	return true;
}

bool D88Image::IsValid(void)
{
	if(this->diskImage.trackData.size() == TRACK_COUNT)
	{
		return true;
	}
	return false;
}
