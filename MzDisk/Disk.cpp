#include <stdlib.h>
#include <stdexcept>
#include <iterator>
#include "Format.hpp"
#include "FileData.hpp"
#include "Disk.hpp"

Disk::Disk()
:image()
,sectorSize(0)
{
}

Disk::~Disk()
{
}

int Disk::DiskType(std::string path)
{
	D88Image image;
	image.Load(path);
	return DiskType(image);
}

int Disk::DiskType(const std::vector<unsigned char>& buffer)
{
	D88Image image;
	image.Load(&buffer[0], buffer.size());
	return DiskType(image);
}

int Disk::DiskType(D88Image& image)
{
	int diskType = UNKNOWN;
	D88Image::Header header;
	image.GetHeader(header);
	D88Image::SectorInfo sectorInfo;
	std::vector<unsigned char> readBuffer;
	image.ReadSector(sectorInfo, readBuffer, 0, 0, 1);
	if(sectorInfo.sizeOfData == 128)
	{
		// セクタ長128は2SディスクなのでMZ-80K用のディスク
		image.ReadSector(sectorInfo, readBuffer, 0, 1, 1);
		if(readBuffer[0] == 0x80)
		{
			diskType = MZ80K_SP6110;
		}
		else
		{
			diskType = MZ80K_SP6010;
		}
	}
	else if(sectorInfo.sizeOfData == 256)
	{
		// MZ-80B/700/1500/2000/2200/2500用
		diskType = MZ2000;
	}
	return diskType;
}

void Disk::ExportBeta(std::string path)
{
	int diskType = DiskType();
	D88Image::Header header;
	this->image.GetHeader(header);
	int trackMax = -1;
	for(int i = 0; i < D88Image::TRACK_MAX; ++ i)
	{
		if(header.trackTable[i] == 0)
		{
			trackMax = i;
			break;
		}
	}
	if(trackMax <= 0)
	{
		return;
	}
	int cylinderMax = trackMax / 2;
	D88Image::SectorInfo sectorInfo;
	std::vector<unsigned char> sectorBuffer;
	_unlink(path.c_str());
	for(int c = 0; c < cylinderMax; ++ c)
	{
		for(int h = 0; h < 2; ++ h)
		{
			for(int r = 1; r <= 16; ++ r)
			{
				if(diskType == Disk::MZ2000)
				{
					this->image.ReadSector(sectorInfo, sectorBuffer, c, 1 - h, r);
					ReverseBuffer(sectorBuffer);
				}
				else
				{
					this->image.ReadSector(sectorInfo, sectorBuffer, c, h, r);
				}
				dms::FileData file;
				file.SetBuffer(&sectorBuffer[0], sectorBuffer.size());
				file.SaveAdd(path.c_str());
			}
		}
	}
}

void Disk::ImportBeta(std::string path)
{
	dms::FileData file;
	file.Load(path);
	size_t size = file.GetBufferSize();
	std::vector<unsigned char> betaBuffer;
	file.GetBuffer(betaBuffer);
	int diskType = DiskType();
	D88Image::Header header;
	this->image.GetHeader(header);
	int trackMax = -1;
	for(int i = 0; i < D88Image::TRACK_MAX; ++ i)
	{
		if(header.trackTable[i] == 0)
		{
			trackMax = i;
			break;
		}
	}
	if(trackMax <= 0)
	{
		return;
	}
	int cylinderMax = trackMax / 2;
	D88Image::SectorInfo sectorInfo;
	std::vector<unsigned char> sectorBuffer;
	size_t betaIndex = 0;
	for(int c = 0; c < cylinderMax; ++ c)
	{
		for(int h = 0; h < 2; ++ h)
		{
			for(int r = 1; r <= 16; ++ r)
			{
				sectorBuffer.clear();
				int rest = static_cast<int>(size) - static_cast<int>(betaIndex);
				if(rest > this->sectorSize)
				{
					rest = this->sectorSize;
				}
				unsigned char* source = reinterpret_cast<unsigned char*>(&betaBuffer[betaIndex]);
				std::copy(source, source + rest, std::back_inserter(sectorBuffer));
				if(sectorBuffer.size() < this->sectorSize)
				{
					sectorBuffer.resize(this->sectorSize, 0);
				}
				if(diskType == Disk::MZ2000)
				{
					this->image.GetSectorInfo(sectorInfo, c, 1 - h, r);
					ReverseBuffer(sectorBuffer);
					this->image.WriteSector(sectorInfo, sectorBuffer, c, 1 - h, r);
				}
				else
				{
					this->image.GetSectorInfo(sectorInfo, c, h, r);
					this->image.WriteSector(sectorInfo, sectorBuffer, c, h, r);
				}
				betaIndex += this->sectorSize;
			}
		}
	}
	Update();
}

void Disk::ReverseBuffer(std::vector<unsigned char>& buffer)
{
	for(size_t i = 0; i < buffer.size(); ++ i)
	{
		buffer[i] ^= 0xFF;
	}
}
