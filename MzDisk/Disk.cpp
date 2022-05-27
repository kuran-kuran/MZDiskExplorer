#include <stdlib.h>
#include <stdexcept>
#include <iterator>
#include "Format.hpp"
#include "FileData.hpp"
#include "Disk.hpp"

Disk::Disk()
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

void Disk::ReverseBuffer(std::vector<unsigned char>& buffer)
{
	for(size_t i = 0; i < buffer.size(); ++ i)
	{
		buffer[i] ^= 0xFF;
	}
}
