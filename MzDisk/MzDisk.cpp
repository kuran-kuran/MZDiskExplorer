#include <stdlib.h>
#include <stdexcept>
#include <iterator>
#include "Format.hpp"
#include "FileData.hpp"
#include "MzDisk.hpp"

// 漢字の1バイト目か
#define IsKanji(c) ((((0x81 <= c) && (c <= 0x9F)) || ((0xE0 <= c) && (c <= 0xFC))))
#define IsKanji2(c) ((((0x40 <= c) && (c <= 0x7E) || ((0x80 <= c) && (c <= 0xFC))))

//============================================================================
// コンストラクタ
//============================================================================
MzDisk::MzDisk()
:diskType(0)
,bitmap(NULL)
,fileType(0)
,clusterSize(0)
,dirSector(16)
{
	this->sectorSize = 256;
}

//============================================================================
// デストラクタ
//============================================================================
MzDisk::~MzDisk()
{
}

int MzDisk::DiskType(void)
{
	return Disk::MZ2000;
}

std::string MzDisk::DiskTypeText(void)
{
	return "MZ-80B/700/1500/2000/2200/2500ディスク";
}

//============================================================================
//  ディスクイメージをフォーマットする
//----------------------------------------------------------------------------
// In  : path = イメージファイル
//     : type
//     : DISKTYPE_MZ2500_2DD   // MZ-2500
//     : DISKTYPE_MZ2500_2DD40 // MZ-2500
//     : DISKTYPE_MZ2500_2DD35 // MZ-2500
//     : DISKTYPE_MZ80B_2D35   // MZ-80B (MZ-80BF)
//     : DISKTYPE_MZ2000_2D40  // MZ-2000 (MZ-1F07)
// Out : エラーコード ( 0 = 正常 )
//============================================================================
void MzDisk::Format(int type, int volumeNumber)
{
	if(type == DISKTYPE_MZ2500_2DD)
	{
		this->diskType = TYPE_2DD;
	}
	else if(type == DISKTYPE_MZ2500_2DD40)
	{
		this->diskType = TYPE_2DD;
	}
	else if(type == DISKTYPE_MZ2500_2DD35)
	{
		this->diskType = TYPE_2DD;
	}
	else if(type == DISKTYPE_MZ80B_2D35)
	{
		this->diskType = TYPE_2D;
	}
	else if(type == DISKTYPE_MZ2000_2D40)
	{
		this->diskType = TYPE_2D;
	}
	else
	{
		return;
	}
	// 物理フォーマット
	this->image.Format(type, 0);//0xBF ^ 0xFF);
	// 論理フォーマット
	if(type == DISKTYPE_MZ80B_2D35 || type == DISKTYPE_MZ2000_2D40)
	{
		// ディレクトリ
		std::vector<unsigned char> buffer;
		buffer.resize(static_cast<size_t>(this->sectorSize) * 8, 0);
		buffer[0] = 0x80;
		buffer[1] = static_cast<unsigned char>(volumeNumber);
		WriteSector(buffer, 16, 8);
	}
	else
	{
		// IPL
		std::vector<unsigned char> buffer;
		buffer.resize(this->sectorSize, 0);
		IPL* ipl = reinterpret_cast<IPL*>(&buffer[0]);
		ipl->machine = 0x04; // MZ-2500データディスク
		WriteSector(buffer, 0, 1);
		// ディレクトリ
		buffer.clear();
		buffer.resize(static_cast<size_t>(this->sectorSize) * 8, 0);
		for(int i = 0; i < 64; ++ i)
		{
			memset(&buffer[static_cast<size_t>(i) * 32], 0xD, 17);
			buffer[static_cast<size_t>(i) * 32] = 0x00;
		}
		buffer[0] = 0x80;
		WriteSector(buffer, 16, 8);
	}
	// ビットマップ
	this->bitmap.clear();
	this->bitmap.resize(this->sectorSize, 0);
	if(type == DISKTYPE_MZ2500_2DD)
	{
		this->bitmap[0] = 0x01;
		this->bitmap[1] = 0x18;
		this->bitmap[2] = 0x18;
		this->bitmap[3] = 0x00;
		this->bitmap[4] = 0x00;
		this->bitmap[5] = 0x05;
		this->bitmap[255] = 0x01;
	}
	else if(type == DISKTYPE_MZ2500_2DD40)
	{
		this->bitmap[0] = 0x01;
		this->bitmap[1] = 0x30;
		this->bitmap[2] = 0x30;
		this->bitmap[3] = 0x00;
		this->bitmap[4] = 0x00;
		this->bitmap[5] = 0x05;
		this->bitmap[255] = 0x00;
	}
	else if(type == DISKTYPE_MZ2500_2DD35)
	{
		this->bitmap[0] = 0x01;
		this->bitmap[1] = 0x30;
		this->bitmap[2] = 0x30;
		this->bitmap[3] = 0x00;
		this->bitmap[4] = 0x60;
		this->bitmap[5] = 0x04;
		this->bitmap[255] = 0x00;
	}
	else if(type == DISKTYPE_MZ80B_2D35)
	{
		this->bitmap[0] = static_cast<unsigned char>(volumeNumber);
		this->bitmap[1] = 0x30;
		this->bitmap[2] = 0x30;
		this->bitmap[3] = 0x00;
		this->bitmap[4] = 0x60;
		this->bitmap[5] = 0x04;
		this->bitmap[255] = 0x00;
	}
	else if(type == DISKTYPE_MZ2000_2D40)
	{
		this->bitmap[0] = static_cast<unsigned char>(volumeNumber);
		this->bitmap[1] = 0x30;
		this->bitmap[2] = 0x30;
		this->bitmap[3] = 0x00;
		this->bitmap[4] = 0x00;
		this->bitmap[5] = 0x05;
		this->bitmap[255] = 0x00;
	}
	WriteSector(this->bitmap, 15, 1);
	// ディスク情報格納
	this->clusterSize = this->sectorSize * (this->bitmap[255] + 1);
	ReadDirectory();
}

int MzDisk::Load(std::string path)
{
	dms::FileData fileData;
	fileData.Load(path.c_str());
	std::vector<unsigned char> buffer;
	fileData.GetBuffer(buffer);
	return Load(buffer);
}

//============================================================================
//  ディスクイメージを読み込んでメモリに展開する
//----------------------------------------------------------------------------
// In  : image = イメージファイルメモリイメージ
// Out : エラーコード ( 0 = 正常 )
//============================================================================
int MzDisk::Load(const std::vector<unsigned char>& buffer)
{
	try
	{
		const void* image = &buffer[0];
		const D88Image::Header* header = reinterpret_cast<const D88Image::Header*>(image);
		this->image.Load(image, header->diskSize);
		Update();
		return 0;
	}
	catch(const std::exception& error)
	{
		(void)error;
		return 1;
	}
}

void MzDisk::Update(void)
{
	D88Image::Header header;
	this->image.GetHeader(header);
	std::vector<unsigned char> buffer;
	this->bitmap.clear();
	ReadSector(this->bitmap, 15, 1);
	// ディスク情報格納
	int trackMax = 0;
	for(int i = 0; i < D88Image::TRACK_MAX; ++ i)
	{
		if(header.trackTable[i] != 0)
		{
			++ trackMax;
		}
	}
	this->clusterSize = this->sectorSize * (this->bitmap[255] + 1);
	ReadDirectory();
}

int MzDisk::Save(std::string path)
{
	std::vector<unsigned char> buffer;
	int result = Save(buffer);
	if(result != 0)
	{
		return result;
	}
	dms::FileData fileData;
	fileData.SetBuffer(&buffer[0], buffer.size());
	fileData.Save(path.c_str());
	return 0;
}

//============================================================================
//  ディスクイメージを作成する
//----------------------------------------------------------------------------
// In  : buffer = イメージファイルを格納するバッファ
// Out : エラーコード ( 0 = 正常 )
//============================================================================
int MzDisk::Save(std::vector<unsigned char>& buffer)
{
	try
	{
		FlushWrite();
		this->image.Save(buffer);
		return 0;
	}
	catch(const std::exception& error)
	{
		(void)error;
		return 1;
	}
}

//============================================================================
//  ディスクイメージからファイルを取り出す
//----------------------------------------------------------------------------
// In  : dirindex = 取り出すファイルのディレクトリ番号
//     : path = 保存位置
//     : mode = ファイルモード
// Out : 0 = 成功
//============================================================================
int MzDisk::GetFile(int dirindex, std::string path, unsigned int mode)
{
	try
	{
		FILE *fp;
		if(fopen_s(&fp, path.c_str(), "wb") != 0)
		{
			return 1;
		}
		if(fp == NULL)
		{
			return 1;
		}
		if((mode & FILEMODE_MASK) == FILEMODE_MZT)
		{
			MZTHEAD mzthead;
			// ヘッダ情報作成
			memset(&mzthead, 0, sizeof(MZTHEAD));
			mzthead.mode = this->directory[dirindex].mode;
			memcpy(mzthead.filename, this->directory[dirindex].filename, 17 );
			mzthead.size = this->directory[dirindex].size;
			mzthead.loadAdr = this->directory[dirindex].loadAdr;
			mzthead.runAdr = this->directory[dirindex].runAdr;
			// ヘッダ情報書き込み
			fwrite(&mzthead, 1, 128, fp);
		}
		// データ書き込み
		if(this->directory[dirindex].mode == FILETYPE_BSD)
		{
			int sector;
			int rest;
			sector = this->directory[dirindex].startSector;
			rest = this->directory[dirindex].size;
			while(sector != 0)
			{
				std::vector<unsigned char> buffer;
				ReadSector(buffer, sector, 1);
				if(rest > 254)
				{
					fwrite(&buffer[0], 1, 254, fp);
				}
				else
				{
					fwrite(&buffer[0], 1, rest, fp);
					break;
				}
				sector = static_cast<int>(buffer[254]) + static_cast<int>(buffer[255]) * 256;
				rest -= 254;
			}
		}
		else if(this->directory[dirindex].mode == FILETYPE_BRD)
		{
			int sector;
			int rest;
			int sectorIndex = 0;
			sector = this->directory[dirindex].startSector;
			rest = this->directory[dirindex].size * 32;
			std::vector<unsigned char> sectorListBuffer;
			ReadSector(sectorListBuffer, sector, 1);
			unsigned short* sectorList = reinterpret_cast<unsigned short*>(&sectorListBuffer[0]);
			sector = sectorList[sectorIndex];
			while(sector != 0)
			{
				std::vector<unsigned char> buffer;
				ReadSector(buffer, sector, 16);
				if(rest > 4096)
				{
					fwrite(&buffer[0], 1, 4096, fp);
				}
				else
				{
					fwrite(&buffer[0], 1, rest, fp);
					break;
				}
				++ sectorIndex;
				sector = sectorList[sectorIndex];
				rest -= 4096;
			}
		}
		else
		{
			std::vector<unsigned char> buffer;
			int sectorCount = (this->directory[dirindex].size + this->sectorSize - 1) / this->sectorSize;
			ReadSector(buffer, this->directory[dirindex].startSector, sectorCount);
			fwrite(&buffer[0], 1, this->directory[dirindex].size, fp);
		}
		// 書き込み終了
		fclose(fp);
		return 0;
	}
	catch(const std::exception& error)
	{
		(void)error;
		return 1;
	}
}

//============================================================================
//  ディスクイメージにファイルを書き込む
//----------------------------------------------------------------------------
// In  : path = ファイル名
//     : dirinfo = ディレクトリ情報
//     : mode = ファイルモード
//     :   FILEMODE_MZT
//     :   FILEMODE_BIN
//     :   FILEMODE_NEWLINE_CONV
//     : type = ファイルタイプ
//     :   FILETYPE_OBJ = 1
//     :   FILETYPE_BTX = 2
//     :   FILETYPE_BSD = 3
//     :   FILETYPE_BRD = 4
//     :   FILETYPE_BSD_CONV = 5
// Out : 0 = 正常終了
//============================================================================
int MzDisk::PutFile(std::string path, void* dirInfo, unsigned int mode, unsigned int type)
{
	try
	{
		DIRECTORY* dirinfo = reinterpret_cast<DIRECTORY*>(dirInfo);
		// ディレクトリ検索
		int select = -1;
		for(int i = 0; i < 64; ++ i)
		{
			// ファイルネーム
			if(this->directory[i].mode == 0)
			{
				select = i;
				break;
			}
		}
		if(select == -1)
		{
			// ディレクトリに空きがありません
			return 1;
		}
		// ファイルを読み込む
		FILE* fp;
		if(fopen_s(&fp, path.c_str(), "rb") != 0)
		{
			// ファイルを読み込むことができない
			return 2;
		}
		if(fp == NULL)
		{
			// ファイルを読み込むことができない
			return 2;
		}
		// ファイルサイズ取得
		fseek(fp, 0, SEEK_END);
		size_t dataSize = ftell(fp);
		fseek(fp, 0, SEEK_SET);
		MZTHEAD mzthead;
		if(mode == FILEMODE_MZT)
		{
			if(dataSize <= 128)
			{
				// ファイルサイズが足りない
				return 3;
			}
			dataSize -= 128;	// ヘッダを除いたデータサイズ
			// ヘッダ情報作成
			memset(&mzthead, 0, sizeof(MZTHEAD));
			fread(&mzthead, 128, 1, fp);
		}
		// 同じファイル名が無いかチェックする
		char filename[18];
		memset(filename, 0, sizeof(filename));
		if((mode == FILEMODE_MZT) && (dirinfo == NULL))
		{
			memcpy(filename, mzthead.filename, 17);
		}
		else
		{
			memcpy(filename, dirinfo->filename, 17);
		}
		for(int i = 0; i < 17; ++ i)
		{
			if(filename[i] == 0x0D)
			{
				filename[i] = '\0';
			}
		}
		for(int i = 0; i < 64; ++ i)
		{
			if(this->directory[i].mode != 0)
			{
				// ファイルネーム
				if(strncmp(this->directory[i].filename, filename, strlen(filename)) == 0)
				{
					// 同じファイル名が存在する
					return 5;
				}
			}
		}
		// ファイルデータを読み込む
		size_t readSize = (dataSize + 255) / 256 * 256;
		std::vector<unsigned char> bufferTemp;
		bufferTemp.resize(readSize, 0);
		fread(&bufferTemp[0], 1, dataSize, fp);
		fclose(fp);
		if(type == FILETYPE_BSD_CONV)
		{
			ConvertBsdFile(dirinfo, mode, type, readSize, dataSize, bufferTemp);
		}
		if(type == FILETYPE_BSD)
		{
			if(PutBsdFile(dirinfo, mode, mzthead, select, dataSize, bufferTemp) == false)
			{
				// ビットマップの空き容量が無い
				return 4;
			}
		}
		else if(type == FILETYPE_BRD)
		{
			if(PutBrdFile(dirinfo, mode, mzthead, select, dataSize, bufferTemp) == false)
			{
				// ビットマップの空き容量が無い
				return 4;
			}
		}
		else
		{
			if(PutObjFile(dirinfo, mode, mzthead, select, dataSize, bufferTemp, type) == false)
			{
				// ビットマップの空き容量が無い
				return 4;
			}
		}
		// 管理情報をD88イメージに書き込み
		FlushWrite();
		return 0;
	}
	catch(const std::exception& error)
	{
		(void)error;
		return 1;
	}
}

void MzDisk::ConvertBsdFile(DIRECTORY *dirinfo, unsigned int mode, unsigned int& type, size_t readSize, size_t dataSize, std::vector<unsigned char>& bufferTemp)
{
	if(mode != FILEMODE_MZT)
	{
		int back = -1;
		std::vector<unsigned char> convTemp;
		convTemp.resize(dataSize, 0);
		int j = 0;
		for(int i = 0; i < dataSize; ++ i)
		{
			// CRLFの場合はLFを消す
			if((bufferTemp[i] == 0x0A) && (back == 0x0D))
			{
				back = bufferTemp[i];
				continue;
			}
			unsigned char data = bufferTemp[i];
			// LFの場合はCRにする
			if((bufferTemp[i] == 0x0A) && (back != 0x0D))
			{
				data = 0x0D;
			}
			convTemp[j] = data;
			++ j;
			back = bufferTemp[i];
		}
		dataSize = j;
		bufferTemp.clear();
		std::copy(convTemp.begin(), convTemp.end(), std::back_inserter(bufferTemp));
		if(dirinfo != 0)
		{
			dirinfo->size = static_cast<unsigned short>(dataSize);
		}
	}
	type = FILETYPE_BSD;
}

bool MzDisk::PutBsdFile(DIRECTORY *dirinfo, unsigned int mode, MZTHEAD& mzthead, int select, size_t dataSize, std::vector<unsigned char>& bufferTemp)
{
	if(GetBitmapSize() < dataSize)
	{
		return false;
	}
	// ファイル情報をディレクトリに登録
	int rest;
	if((FILEMODE_MZT == mode) && (dirinfo == NULL))
	{
		memset(&this->directory[select], 0, 32);
		this->directory[select].mode = mzthead.mode;
		memcpy(this->directory[select].filename, mzthead.filename, 17);
		this->directory[select].attr = 0;
		this->directory[select].size = mzthead.size;
		this->directory[select].loadAdr = mzthead.loadAdr;
		this->directory[select].runAdr = mzthead.runAdr;
		this->directory[select].date = 0;
		rest = mzthead.size;
	}
	else
	{
		memcpy(&this->directory[select], dirinfo, 32);
		rest = dirinfo->size;
	}
	int temp = this->bitmap[2] + this->bitmap[3] * 256;
	int datacluster = ((static_cast<int>(dataSize) + this->clusterSize - 1) / this->clusterSize);
	this->bitmap[2] = (temp + datacluster) & 255;
	this->bitmap[3] = ((temp + datacluster) / 256) & 255;
	// ファイルをディスクイメージに転送
	int before = -1;
	int cluster = GetBitmap();
	this->directory[select].startSector = (this->bitmap[1] + cluster) * this->clusterSize / this->sectorSize;
	unsigned char* source = &bufferTemp[0];
	std::vector<unsigned char> writeBuffer;
	writeBuffer.resize(this->sectorSize, 0);
	while(rest > 0)
	{
		cluster = GetBitmap();
		SetBitmap(cluster, 1);
		int sector = ((this->bitmap[1] + cluster) * this->clusterSize) / this->sectorSize;
		for(int i = 0; i < ( this->clusterSize / this->sectorSize ); ++ i)
		{
			int write_size;
			write_size = rest;
			if(rest > this->sectorSize - 2)
			{
				write_size = this->sectorSize - 2;
			}
			memset(&writeBuffer[0], 0, this->sectorSize);
			memcpy(&writeBuffer[0], source, write_size);
			WriteSector(writeBuffer, sector, 1);
			if(before != -1)
			{
				std::vector<unsigned char> beforeBuffer;
				ReadSector(beforeBuffer, before, 1);
				beforeBuffer[static_cast<size_t>(this->sectorSize) - 2] = sector % 256;
				beforeBuffer[static_cast<size_t>(this->sectorSize) - 1] = sector / 256;
				WriteSector(beforeBuffer, before, 1);
			}
			rest -= write_size;
			if( rest <= 0 )
			{
				break;
			}
			source += (static_cast<size_t>(this->sectorSize) - 2);
			before = sector;
			++ sector;
		}
	}
	return true;
}

bool MzDisk::PutBrdFile(DIRECTORY *dirinfo, unsigned int mode, MZTHEAD& mzthead, int select, size_t dataSize, std::vector<unsigned char>& bufferTemp)
{
	if(GetBitmapSize() < dataSize)
	{
		return false;
	}
	// ファイル情報をディレクトリに登録
	int rest;
	if((mode == FILEMODE_MZT) && (dirinfo == NULL))
	{
		memset(&this->directory[select], 0, 32);
		this->directory[select].mode = mzthead.mode;
		memcpy(this->directory[select].filename, mzthead.filename, 17);
		this->directory[select].attr = 0;
		this->directory[select].size = ((mzthead.size + 4095) / 4096 * 4096) / 32;
		this->directory[select].loadAdr = mzthead.loadAdr;
		this->directory[select].runAdr = mzthead.runAdr;
		this->directory[select].date = 0;
		rest = mzthead.size;
	}
	else
	{
		memcpy(&this->directory[select], dirinfo, 32);
		rest = static_cast<int>(dirinfo->size) * 32;
	}
	int temp = this->bitmap[2] + this->bitmap[3] * 256;
	int datacluster = ((static_cast<int>(dataSize) + this->clusterSize - 1) / this->clusterSize);
	this->bitmap[ 2 ] = (temp + datacluster) & 255;
	this->bitmap[ 3 ] = ((temp + datacluster) / 256) & 255;
	// ファイルをディスクイメージに転送
	int cluster = GetBitmapSerial(256);
	this->directory[select].startSector = (this->bitmap[1] + cluster) * this->clusterSize / this->sectorSize;
	SetBitmap(cluster, 1);
	std::vector<unsigned short> sectorList;
	std::vector<unsigned char> writeBuffer;
	unsigned char* source = &bufferTemp[0];
	while(rest > 0)
	{
		unsigned short sector;
		int loop;
		// 4KByte単位で連続した領域を取得する
		cluster = GetBitmapSerial(4096);
		if(this->clusterSize <= 4096)
		{
			SetBitmap(cluster, 4096 / this->clusterSize);
			loop = 1;
		}
		else
		{
			SetBitmap(cluster, 1);
			loop = this->clusterSize / 4096;
		}
		sector = ((this->bitmap[1] + cluster) * this->clusterSize) / this->sectorSize;
		for(int i = 0; i < loop; ++ i)
		{
			int write_size;
			write_size = rest;
			if(rest > 4096)
			{
				write_size = 4096;
			}
			int writeSectorCount = (write_size + this->sectorSize - 1)/ this->sectorSize;
			writeBuffer.clear();
			std::copy(source, source + write_size, std::back_inserter(writeBuffer));
			WriteSector(writeBuffer, sector, writeSectorCount);
			sectorList.push_back(sector);
			rest -= write_size;
			if(rest <= 0)
			{
				break;
			}
			source += 4096;
			sector += (4096 / this->sectorSize);
		}
	}
	writeBuffer.clear();
	writeBuffer.resize(this->sectorSize, 0);
	unsigned char* copyBuffer = reinterpret_cast<unsigned char*>(&sectorList[0]);
	size_t coptBytes = sectorList.size() * sizeof(unsigned short);
	std::copy(copyBuffer, copyBuffer + coptBytes, writeBuffer.begin());
	WriteSector(writeBuffer, this->directory[select].startSector, 1);
	return true;
}

bool MzDisk::PutObjFile(DIRECTORY *dirinfo, unsigned int mode, MZTHEAD& mzthead, int select, size_t dataSize, std::vector<unsigned char>& bufferTemp, unsigned int type)
{
	// ビットマップ検索
	int freespace = GetBitmapSerial(static_cast<int>(dataSize));
	if(freespace == -1)
	{
		// ビットマップの空き容量が無い
		return false;
	}
	int size = 0;
	// ファイル情報をディレクトリに登録
	if((mode == FILEMODE_MZT) && (dirinfo == NULL))
	{
		memset(&this->directory[select], 0, 32);
		this->directory[select].mode = mzthead.mode;
		memcpy(this->directory[select].filename, mzthead.filename, 17);
		this->directory[select].attr = 0;
		this->directory[select].size = mzthead.size;
		this->directory[select].loadAdr = mzthead.loadAdr;
		this->directory[select].runAdr = mzthead.runAdr;
		this->directory[select].date = 0;
		size = mzthead.size;
	}
	else
	{
		memcpy(&this->directory[select], dirinfo, 32);
		size = dirinfo->size;
	}
	int temp = this->bitmap[2] + this->bitmap[3] * 256;
	int datacluster = ((static_cast<int>(dataSize) + this->clusterSize - 1) / this->clusterSize);
	this->bitmap[2] = (temp + datacluster) & 255;
	this->bitmap[3] = ((temp + datacluster) / 256) & 255;
	this->directory[select].startSector = (this->bitmap[1] + freespace) * (this->bitmap[255] + 1);
	// ビットマップ更新
	SetBitmap(freespace, datacluster);
	// ファイルをディスクイメージに転送
	std::vector<unsigned char> writeBuffer;
	int writeSector = (this->bitmap[1] + freespace) * this->clusterSize / this->sectorSize;
	int writeSectorCount = (size + this->sectorSize - 1) / this->sectorSize;
	writeBuffer.resize(static_cast<size_t>(writeSectorCount) * this->sectorSize, 0);
	std::copy(bufferTemp.begin(), bufferTemp.end(), writeBuffer.begin());
	WriteSector(writeBuffer, writeSector, writeSectorCount);
	return true;
}

//============================================================================
//  ディスクイメージのファイルを削除する
//----------------------------------------------------------------------------
// In  : dirindex = ディレクトリ番号
// Out : 0 = 正常終了
//============================================================================
int MzDisk::DelFile(int dirindex)
{
	if(this->directory[dirindex].mode == 0)
	{
		return 1;
	}
	if(this->directory[dirindex].mode == FILETYPE_BSD)
	{
		DelBsdFile(dirindex);
	}
	else if(this->directory[dirindex].mode == FILETYPE_BRD)
	{
		DelBrdFile(dirindex);
	}
	else
	{
		DelObjFile(dirindex);
	}
	this->directory[dirindex].mode = 0;
	// 管理情報をD88イメージに書き込み
	FlushWrite();
	return 0;
}

// BSDファイル削除
void MzDisk::DelBsdFile(int dirindex)
{
	int sector = this->directory[dirindex].startSector;
	while(sector > 0)
	{
		int start = sector / (this->bitmap[255] + 1) - this->bitmap[1];
		DelBitmap(start, 1);
		std::vector<unsigned char> buffer;
		ReadSector(buffer, sector, 1);
		sector = static_cast<int>(buffer[254]) + static_cast<int>(buffer[255]) * 256;
	}
}

// BRDファイル削除
void MzDisk::DelBrdFile(int dirindex)
{
	int sector = this->directory[dirindex].startSector;
	std::vector<unsigned char> sectorListBuffer;
	ReadSector(sectorListBuffer, sector, 1);
	int start = sector / (this->bitmap[255] + 1) - this->bitmap[1];
	DelBitmap(start, 1);
	unsigned short* sectorList = reinterpret_cast<unsigned short*>(&sectorListBuffer[0]);
	int sectorIndex = 0;
	int size = 4096 / this->clusterSize;
	while(sectorList[sectorIndex] > 0)
	{
		int start = sectorList[sectorIndex] / (this->bitmap[255] + 1) - this->bitmap[1];
		DelBitmap(start, size);
		++ sectorIndex;
	}
}

// BTX or OBJファイル削除
void MzDisk::DelObjFile(int dirindex)
{
	int start = this->directory[dirindex].startSector / (this->bitmap[255] + 1) - this->bitmap[1];
	int size = (this->directory[dirindex].size + this->clusterSize - 1) / this->clusterSize;
	DelBitmap(start, size);
}

//============================================================================
//  ディスクイメージからブートプログラムを取り出す
//----------------------------------------------------------------------------
// In  : path = 保存位置
//     : mode = ファイルモード
// Out : 0 = 正常終了
//============================================================================
int MzDisk::GetBoot(std::string path, unsigned int mode)
{
	std::vector<unsigned char> iplBuffer;
	ReadSector(iplBuffer, 0, 1);
	IPL* ipl = reinterpret_cast<IPL*>(&iplBuffer[0]);
	if(strncmp(ipl->signature, "IPLPRO", 6) != 0)
	{
		// 起動ディスクではありません
		return 1;
	}
	FILE *fp;
	if(fopen_s(&fp, path.c_str(), "wb") != 0)
	{
		return 1;
	}
	if(fp == NULL)
	{
		return 1;
	}
	if((mode & FILEMODE_MASK) == FILEMODE_MZT)
	{
		// ヘッダ情報作成
		MZTHEAD mzthead;
		memset(&mzthead, 0, sizeof(MZTHEAD));
		mzthead.mode = 1;
		memcpy(mzthead.filename, ipl->bootname, 11);
		for(int i = 0; i < 17; ++ i)
		{
			if(mzthead.filename[i] == 0)
			{
				mzthead.filename[i] = '\xD';
				break;
			}
		}
		mzthead.size = ipl->size;
		// ヘッダ情報書き込み
		fwrite(&mzthead, 1, 128, fp);
	}
	// データ書き込み
	int writesize = ipl->size;
	if(writesize == 0)
	{
		writesize = 65536;
	}
	std::vector<unsigned char> writeBuffer;
	int sectorSize = (writesize + this->sectorSize - 1) / this->sectorSize;
	ReadSector(writeBuffer, ipl->startSector, sectorSize);
	fwrite(&writeBuffer[0], 1, writesize, fp);
	// 書き込み終了
	fclose( fp );
	return 0;
}

//============================================================================
//  ディスクイメージにブートプログラムを書き込む
//----------------------------------------------------------------------------
// In  : path = ファイル名
//     : iplinfo = ディレクトリ情報
//     : mode = ファイルモード
//     :   FILEMODE_MZT
//     :   FILEMODE_BIN
//     : machine = 機種
//     :   01h MZ-80B/2000/2200
//     :   02h MZ-80A
//     :   03h MZ-700/1500
// Out : 0 = 正常終了
//============================================================================
int MzDisk::PutBoot(std::string path, void* iplInfo, unsigned int mode, unsigned char machine)
{
	IPL* iplinfo = reinterpret_cast<IPL*>(iplInfo);
	std::vector<unsigned char> iplBuffer;
	ReadSector(iplBuffer, 0, 1);
	IPL* ipl = reinterpret_cast<IPL*>(&iplBuffer[0]);
	// ファイルを読み込む
	FILE *fp;
	if(fopen_s(&fp, path.c_str(), "rb") != 0)
	{
		return 1;
	}
	if(fp == NULL)
	{
		// ファイルを読み込むことができない
		return 1;
	}
	// ファイルサイズ取得
	fseek(fp, 0, SEEK_END);
	int datasize = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	MZTHEAD mzthead;
	if(mode == FILEMODE_MZT)
	{
		if(datasize <= 128)
		{
			// ファイルサイズが足りない
			return 2;
		}
		datasize -= 128;	// ヘッダを除いたデータサイズ
		// ヘッダ情報作成
		memset(&mzthead, 0, sizeof(MZTHEAD));
		fread(&mzthead, 128, 1, fp);
	}
	// ファイルデータを読み込む
	int fileSectorCount = (datasize + this->sectorSize - 1) / this->sectorSize;
	int fileBufferSize = fileSectorCount * this->sectorSize;
	std::vector<unsigned char> bufferFile;
	bufferFile.resize(fileBufferSize, 0);
	fread(&bufferFile[0], 1, datasize, fp);
	fclose(fp);
	// ビットマップ検索
	int freeSpace = GetBitmapSerial(datasize);
	if(freeSpace == -1)
	{
		// ビットマップの空き容量が無い
		return 4;
	}
	// ファイル情報をディレクトリに登録
	if((mode == FILEMODE_MZT) && (iplinfo == NULL))
	{
		memset(ipl, 0, 32);
		ipl->machine = machine;
		memcpy(ipl->bootname, mzthead.filename, 10);
		ipl->bootname[10] = '\xD';
		ipl->size = mzthead.size;
		ipl->runAdr = mzthead.runAdr;
	}
	else
	{
		memcpy(ipl, iplinfo, 32);
		ipl->size = datasize;
	}
	ipl->signature[0] = 'I';
	ipl->signature[1] = 'P';
	ipl->signature[2] = 'L';
	ipl->signature[3] = 'P';
	ipl->signature[4] = 'R';
	ipl->signature[5] = 'O';
	int usedCluster = this->bitmap[2] + this->bitmap[3] * 256;
	int dataCluster = (datasize + this->clusterSize - 1) / this->clusterSize;
	this->bitmap[2] = (usedCluster + dataCluster) & 255;
	this->bitmap[3] = ((usedCluster + dataCluster) / 256) & 255;
	ipl->startSector = (this->bitmap[1] + freeSpace) * (this->bitmap[255] + 1);
	ipl->master = 0xFF; // マスターディスク
	// ビットマップ更新
	SetBitmap(freeSpace, dataCluster);
	// IPLをディスクイメージに転送
	WriteSector(iplBuffer, 0, 1);
	// ファイルをディスクイメージに転送
	WriteSector(bufferFile, ipl->startSector, fileSectorCount);
	FlushWrite();
	return 0;
}

//============================================================================
//  ディスクイメージからしててむプログラムを取り出す(MZ-80K専用)
//----------------------------------------------------------------------------
// In  : path = 保存位置
//     : mode = ファイルモード
// Out : 0 = 正常終了
//============================================================================
int MzDisk::GetSystem(std::string path, unsigned int mode)
{
	return 1;
}

//============================================================================
//  ディスクイメージにシステムプログラムを書き込む(MZ-80K専用)
//----------------------------------------------------------------------------
// In  : path = ファイル名
//     : mode = ファイルモード
//     :   FILEMODE_MZT
//     :   FILEMODE_BIN
// Out : 0 = 正常終了
//============================================================================
int MzDisk::PutSystem(std::string path, void* iplInfo, unsigned int mode)
{
	return 1;
}

//============================================================================
//  ビットマップから空き領域を検索する
//----------------------------------------------------------------------------
// Out : 空きサイズ
//============================================================================
int MzDisk::GetBitmapSize(void)
{
	int freesize = 0;
	unsigned char bit;
	int end;
	end = this->bitmap[4] + this->bitmap[5] * 256;
	for(int i = 48; i < end; ++ i)
	{
		bit = (this->bitmap[i / 8] >> (i % 8)) & 1;
		if(bit == 0)
		{
			freesize ++;
		}
	}
	return freesize * this->clusterSize;
}

//============================================================================
//  ビットマップから空き領域を検索する
//----------------------------------------------------------------------------
// Out : 空きサイズ
//============================================================================
int MzDisk::GetBitmap(void)
{
	unsigned char bit;
	int end;
	end = this->bitmap[4] + this->bitmap[5] * 256;
	for(int i = 48; i < end; ++ i)
	{
		bit = (this->bitmap[i / 8] >> (i % 8)) & 1;
		if(bit == 0)
		{
			return i - 48;
		}
	}
	return -1;
}

//============================================================================
//  ビットマップから連続空き領域を検索する
//----------------------------------------------------------------------------
// In  : length = 書き込み長 (Byte)
// Out : 空き位置格納領域
//============================================================================
int MzDisk::GetBitmapSerial(int length)
{
	int lengthtemp = 0;
	unsigned char bit;
	int index = -1;
	int end;
	int length_cluster;
	length_cluster = (length + this->clusterSize - 1) / this->clusterSize;
	end = this->bitmap[4] + this->bitmap[5] * 256;
	for(int i = 48; i < end; ++ i)
	{
		bit = (this->bitmap[i / 8] >> (i % 8)) & 1;
		if(bit == 0)
		{
			if(index == -1)
			{
				index = i;
			}
			lengthtemp ++;
			if(lengthtemp >= length_cluster)
			{
				index -= 48;
				break;
			}
		}
		else
		{
			lengthtemp = 0;
			index = -1;
		}
	}
	return index;
}

//============================================================================
//  ビットマップを設定する
//----------------------------------------------------------------------------
// In  : start = 位置
//     : length = 長さ (length ビット)
// Out : なし
//============================================================================
void MzDisk::SetBitmap(int start, int length)
{
	unsigned char* data;
	start += 48;
	for(int i = start; i < (start + length); ++ i)
	{
		data = &this->bitmap[i / 8];
		*data |= (1 << (i % 8));
	}
	WriteUseSize();
}

//============================================================================
//  ビットマップを開放する
//----------------------------------------------------------------------------
// In  : start = 位置
//     : length = 長さ (length ビット)
// Out : なし
//============================================================================
void MzDisk::DelBitmap(int start, int length)
{
	unsigned char* data;
	start += 48;
	for(int i = start; i < (start + length); ++ i)
	{
		data = &this->bitmap[i / 8];
		*data &= (~(1 << (i % 8)));
	}
	WriteUseSize();
}

// MZ-80B/2000の文字をWindowsで使える文字に変換する
// MZ-2500のディスクの時に誤変換してしまうので一旦使わないようにした
std::string MzDisk::ConvertText(std::string text)
{
	/* MZ-80B,2000,2200 */
	static const char asciiCodeAnk[] =
	{
		" !\x22#$%&\x27()*+,-./"	/* 20 */
		"0123456789:;<=>?"		/* 30 */
		"@ABCDEFGHIJKLMNO"		/* 40 */
		"PQRSTUVWXYZ[\\]^*"		/* 50 */
		"*abcdefghijklmno"		/* 60 */
		"pqrstuvwxyz{|}~."		/* 70 */
		"................"		/* 80 */
		".\\.............."		/* 90 */
		".｡｢｣WXｦｧｨｩｪｫﾔﾕﾖｯ"		/* A0 */
		"*ｱｲｳｴｵｶｷｸｹｺｻｼｽｾｿ"		/* B0 */
		"ﾀﾁﾂﾃﾄﾅﾆﾇﾈﾉﾊﾋﾌﾍﾎﾏ"		/* C0 */
		"ﾐﾑﾒﾓﾔﾕﾖﾗﾘﾙﾚﾛﾜﾝﾞﾟ"		/* D0 */
		"ZABCDEFGHIJKLMNO"		/* E0 */
		"0123456789PQRST."		/* F0 */
	};
	static const char asciiCodeSjis[] =
	{
		"　！”＃＄％＆’（）＊＋，－．／"	/* 20 */
		"０１２３４５６７８９：；＜＝＞？"	/* 30 */
		"＠ＡＢＣＤＥＦＧＨＩＪＫＬＭＮＯ"	/* 40 */
		"ＰＱＲＳＴＵＶＷＸＹＺ［＼］＾￣"	/* 50 */
		"´ａｂｃｄｅｆｇｈｉｊｋｌｍｎｏ"	/* 60 */
		"ｐｑｒｓｔｕｖｗｘｙｚ｛｜｝～※"	/* 70 */
		"※↓↑→←※※※※※※※※※※※"	/* 80 */
		"※￥※●○※※※※※※※※※※※"	/* 90 */
		"※。「」ＷＸヲァィゥェォャュョッ"	/* A0 */
		"※アイウエオカキクケコサシスセソ"	/* B0 */
		"タチツテトナニヌネノハヒフヘホマ"	/* C0 */
		"ミムメモヤユヨラリルレロワン゛゜"	/* D0 */
		"ＺＡＢＣＤＥＦＧＨＩＪＫＬＭＮＯ"	/* E0 */
		"０１２３４５６７８９ＰＱＲＳＴπ"	/* F0 */
	};
	std::string result;
	bool kanji = false;
	for(size_t i = 0; i < text.size(); ++ i)
	{
		if(kanji == true)
		{
			// 漢字の2バイト目
			result += text[i];
			kanji = false;
		}
		else if(IsKanji(static_cast<unsigned char>(text[i])))
		{
			// 漢字の1バイト目
			result += text[i];
			kanji = true;
		}
		else if(IsNotAvailableFileCharacter(text[i]) == true)
		{
			// ファイル名に使えない文字
			result += "_";
		}
		else
		{
			std::string ascii;
			std::string sjis;
			int asciiIndex = static_cast<unsigned char>(text[i]) - 0x20;
			ascii.push_back(asciiCodeAnk[asciiIndex]);
			int index = asciiIndex * 2;
			sjis.push_back(asciiCodeSjis[index]);
			sjis.push_back(asciiCodeSjis[index + 1]);
			if((ascii == ".") && (sjis == "※"))
			{
				result += "_";
			}
			else if((ascii == ".") && (sjis != "※"))
			{
				result += sjis;
			}
			else
			{
				result += ascii;
			}
		}
	}
	return result;
}

//============================================================================
//  ディレクトリ位置を設定する
//----------------------------------------------------------------------------
// In  : ディレクトリのあるセクタ番号 (-1=ルートディレクトリ)
// Out : なし
//============================================================================
void MzDisk::SetDirSector( int sector )
{
	if( -1 == sector )
	{
		this->dirSector = 0x10;
	}
	else
	{
		this->dirSector = sector;
	}
	ReadDirectory();
}

//============================================================================
//  ディレクトリ位置を取得する
//----------------------------------------------------------------------------
// In  : なし
// Out : ディレクトリのあるセクタ番号
//============================================================================
int MzDisk::GetDirSector( void )
{
	return this->dirSector;
}

int MzDisk::GetDirCount(void)
{
	return static_cast<int>(this->directory.size());
}

//============================================================================
//  ディレクトリ情報を取得する
//----------------------------------------------------------------------------
// In  : ディレクトリ番号 (0～63);
// Out : なし
//============================================================================
void MzDisk::GetDir(void* dirData, int dirindex)
{
	memcpy(dirData, &this->directory[dirindex], sizeof(DIRECTORY));
}

//============================================================================
//  ディレクトリ情報を書き込む
//----------------------------------------------------------------------------
// In  : ディレクトリ番号 (0～63);
// Out : なし
//============================================================================
void MzDisk::SetDir(void* dirData, int dirindex)
{
	memcpy(&this->directory[dirindex], dirData, sizeof(DIRECTORY));
	// 管理情報をD88イメージに書き込み
	FlushWrite();
}

//============================================================================
//  ディレクトリを表示する (ルートのみ)
//----------------------------------------------------------------------------
// In  : なし
// Out : なし
//============================================================================
#if false
void MzDisk::DisplayDir(void)
{
	char fileName[ 18 ];
	int mode;
	struct {
		unsigned char mode;
		char modeStr[ 6 ];
	} ModeTbl[] = {
		0x01, "OBJ  ",
		0x02, "BTX  ",
		0x03, "BSD  ",
		0x04, "BRD  ",
		0x05, "RB   ",
		0x07, "LIB  ",
		0x0A, "SYS  ",
		0x0B, "GR   ",
		0x0F, "DIR  ",
		0x80, "NSWAP",
		0x81, "SWAP "
	};
	// インデックス
	printf("Mode  Filename             Size Load  Run  Sec\n");
	printf("+---+ +-----------------+ +---+ +--+ +--+ +--+\n");
	// ファイル情報表示
	for(int i = 0; i < 64; ++ i)
	{
		if(this->directory[i].mode != 0)
		{
			mode = -1;
			for(int j = 0; j < 10; ++ j)
			{
				if(this->directory[i].mode == ModeTbl[j].mode)
				{
					mode = j;
					break;
				}
			}
			if((mode >= 0) && (mode < 9))
			{
				// モード
				printf("%02d %5s ", i, ModeTbl[mode].modeStr);
				// ファイルネーム
				memset(&fileName, 0, 18);
				memcpy(&fileName, this->directory[i].filename, 17);
				for(int j = 0; j < 17; ++ j)
				{
					if(fileName[j] == 0x0D)
					{
						fileName[j] = 0;
					}
				}
				printf("\"%s\" ", fileName);
				for(int j = 0; j < (17 - (int)strlen(fileName)); ++ j)
				{
					printf(" ");
				}
				// ファイルサイズ
				printf("%5d ", this->directory[i].size);
				// ロードアドレス
				printf("%04X ", this->directory[i].loadAdr);
				// 実行アドレス
				printf("%04X ", this->directory[i].runAdr);
				// 格納セクタ
				printf("%04X\n", this->directory[i].startSector);
			}
		}
	}
}
#endif

//============================================================================
//  パス名から拡張子を取得する
//----------------------------------------------------------------------------
// In  : path = ファイル
//     : extfilename = 拡張子格納領域
// Out : なし
//============================================================================
void MzDisk::GetExtFilename(std::string path, std::string& extfilename)
{
	size_t loop = 4;
	size_t extpos;
	extfilename.resize(4, 0);
	size_t length = path.size();
	if(length > 2)
	{
		if(loop > length)
		{
			loop = length;
		}
		extpos = -1;
		for(size_t i = 0; i < loop; ++ i) 
		{
			if(path[length - i - 1] == '.')
			{
				extpos = length - i - 1 + 1;
				break;
			}
		}
		if(extpos != -1)
		{
			for(size_t i = 0; i < 3; ++ i)
			{
				if(path[extpos] == '\0')
				{
					break;
				}
				extfilename[i] = path[extpos];
				extpos++;
			}
		}
	}
}

//============================================================================
//	パス名から拡張子を削除する
//----------------------------------------------------------------------------
// In  : path = パス名
// Out : なし
//============================================================================
void MzDisk::DeleteFileExtname(std::string path)
{
	size_t loop = 4;
	size_t length = path.size();
	if(length > 2)
	{
		if(loop > length)
		{
			loop = length;
		}
		for(size_t i = 0; i < loop; ++ i)
		{
			if(path[length - i - 1] == '.')
			{
				path[length - i - 1] = '\0';
				break;
			}
		}
	}
}

void MzDisk::WriteUseSize(void)
{
	int size = 0;
	int end = this->bitmap[4] + this->bitmap[5] * 256;
	for(int i = 48; i < end; ++ i)
	{
		unsigned char bit = (this->bitmap[i / 8] >> (i % 8)) & 1;
		if(bit == 1)
		{
			++ size;
		}
	}
	size += this->bitmap[1];
	this->bitmap[2] = size % 256;
	this->bitmap[3] = size / 256;
}

// ビットマップ、ディレクトリをD88に書き込む
void MzDisk::FlushWrite(void)
{
	std::vector<unsigned char> writeBuffer;
	// ビットマップ書き込み
	std::copy(this->bitmap.begin(), this->bitmap.end(), std::back_inserter(writeBuffer));
	WriteSector(writeBuffer, 15, 1);
	// ディレクトリ書き込み
	WriteDirectory();
}

int MzDisk::GetUseBlockSize(void)
{
	if(this->image.IsValid() == false)
	{
		return 0;
	}
	return this->bitmap[2] + this->bitmap[3] * 256;
}

int MzDisk::GetAllBlockSize(void)
{
	if(this->image.IsValid() == false)
	{
		return 0;
	}
	return this->bitmap[4] + this->bitmap[5] * 256;
}

int MzDisk::GetClusterSize(void)
{
	if(this->image.IsValid() == false)
	{
		return 0;
	}
	return this->clusterSize;
}

// D88イメージからディレクトリデータをthis->directryに読み込む
void MzDisk::ReadDirectory(void)
{
	if(this->image.IsValid() == false)
	{
		return;
	}
	this->directory.clear();
	std::vector<unsigned char> buffer;
	ReadSector(buffer, this->dirSector, 8);
	size_t directoryCount = this->sectorSize * 8 / 32;
	for(size_t i = 0; i < directoryCount; ++ i)
	{
		DIRECTORY* directory = reinterpret_cast<DIRECTORY*>(&buffer[i * 32]);
		this->directory.push_back(*directory);
	}
}

// ディレクトリデータをthis->directryからD88イメージに書き込む
void MzDisk::WriteDirectory(void)
{
	std::vector<unsigned char> writeBuffer;
	writeBuffer.clear();
	unsigned char* copyBuffer = reinterpret_cast<unsigned char*>(&this->directory[0]);
	std::copy(copyBuffer, copyBuffer + static_cast<size_t>(this->sectorSize) * 8, std::back_inserter(writeBuffer));
	WriteSector(writeBuffer, this->dirSector, 8);
}

void MzDisk::ReadSector(std::vector<unsigned char>& buffer, int sector, int numOfSector)
{
	for(int i = 0; i < numOfSector; ++ i)
	{
		int readSector = sector + i;
		int track = readSector / 16;
		int c = track / 2;
		int h = 1 - (track % 2);
		int r = readSector % 16 + 1;
		D88Image::SectorInfo sectorInfo;
		std::vector<unsigned char> readBuffer;
		this->image.ReadSector(sectorInfo, readBuffer, c, h, r);
		ReverseBuffer(readBuffer);
		std::copy(readBuffer.begin(), readBuffer.end(), std::back_inserter(buffer));
	}
}

void MzDisk::WriteSector(std::vector<unsigned char>& buffer, int sector, int numOfSector)
{
	for(int i = 0; i < numOfSector; ++ i)
	{
		int writeSector = sector + i;
		int track = writeSector / 16;
		int c = track / 2;
		int h = 1 - (track % 2);
		int r = writeSector % 16 + 1;
		D88Image::SectorInfo sectorInfo;
		this->image.GetSectorInfo(sectorInfo, c, h, r);
		std::vector<unsigned char> writeBuffer;
		std::copy(&buffer[static_cast<size_t>(this->sectorSize) * i], &buffer[static_cast<size_t>(this->sectorSize) * i] + this->sectorSize, std::back_inserter(writeBuffer));
		ReverseBuffer(writeBuffer);
		this->image.WriteSector(sectorInfo, writeBuffer, c, h, r);
	}
}
