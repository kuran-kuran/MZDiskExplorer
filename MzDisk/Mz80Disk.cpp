#include <stdlib.h>
#include <stdexcept>
#include <iterator>
#include "Format.hpp"
#include "FileData.hpp"
#include "Mz80Disk.hpp"

#define IsKanji(c) ( (unsigned char)((int)((unsigned char)(c) ^ 0x20) - 0x0A1) < 0x3C )

//============================================================================
// コンストラクタ
//============================================================================
Mz80Disk::Mz80Disk()
:diskType(0)
,bitmap(NULL)
,fileType(0)
,sectorSize(128)
,dirSector(16)
{
}

//============================================================================
// デストラクタ
//============================================================================
Mz80Disk::~Mz80Disk()
{
}

int Mz80Disk::DiskType(void)
{
	return Disk::MZ80K_SP6010;
}

std::string Mz80Disk::DiskTypeText(void)
{
	return "MZ-80K SP-6010タイプディスク";
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
void Mz80Disk::Format(int type)
{
	if(type == DISKTYPE_MZ80_SP6010_2S)
	{
		this->diskType = TYPE_2S;
	}
	else if(type == DISKTYPE_MZ80_SP6110_2S)
	{
		this->diskType = TYPE_2S;
	}
	else
	{
		return;
	}
	// 物理フォーマット
	this->image.Format(type, 0x00);
	// 論理フォーマット
	if(type == DISKTYPE_MZ80_SP6010_2S)
	{
		// ディレクトリ
		std::vector<unsigned char> buffer;
		buffer.resize(static_cast<size_t>(this->sectorSize) * 14, 0);
		WriteSector(buffer, 16, 14);
	}
	else
	{
		// ディレクトリ
		std::vector<unsigned char> buffer;
		buffer.resize(static_cast<size_t>(this->sectorSize) * 14, 0);
		buffer[0] = 0x80;
		buffer[1] = 0x01;
		WriteSector(buffer, 16, 14);
	}
	// ビットマップ
	this->bitmap.clear();
	this->bitmap.resize(256, 0);
	if(type == DISKTYPE_MZ80_SP6010_2S)
	{
		this->bitmap[1] = 0x01;
	}
	else if(type == DISKTYPE_MZ80_SP6110_2S)
	{
		this->bitmap[1] = 0x01;
	}
	WriteSector(this->bitmap, 14, 2);
	// ディスク情報格納
	ReadDirectory();
}

int Mz80Disk::Load(std::string path)
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
int Mz80Disk::Load(const std::vector<unsigned char>& buffer)
{
	try
	{
		const void* image = &buffer[0];
		const D88Image::Header* header = reinterpret_cast<const D88Image::Header*>(image);
		this->image.Load(image, header->diskSize);
		std::vector<unsigned char> buffer;
		ReadSector(this->bitmap, 14, 2);
		// ディスク情報格納
		int trackMax = 0;
		for(int i = 0; i < D88Image::TRACK_MAX; ++ i)
		{
			if(header->trackTable[i] != 0)
			{
				++ trackMax;
			}
		}
		ReadDirectory();
		return 0;
	}
	catch(const std::exception& error)
	{
		return 1;
	}
}

int Mz80Disk::Save(std::string path)
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
int Mz80Disk::Save(std::vector<unsigned char>& buffer)
{
	try
	{
		FlushWrite();
		this->image.Save(buffer);
		return 0;
	}
	catch(const std::exception& error)
	{
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
int Mz80Disk::GetFile(int dirindex, std::string path, unsigned int mode)
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
			memcpy(mzthead.filename, this->directory[dirindex].filename, 16);
			mzthead.size = this->directory[dirindex].size;
			mzthead.loadAdr = this->directory[dirindex].loadAdr;
			mzthead.runAdr = this->directory[dirindex].runAdr;
			// ヘッダ情報書き込み
			fwrite(&mzthead, 1, 128, fp);
		}
		// データ書き込み
		int sector = this->directory[dirindex].startTrack * 16 + this->directory[dirindex].startSector - 1;
		int rest = this->directory[dirindex].size;
		while(sector != 0)
		{
			std::vector<unsigned char> buffer;
			ReadSector(buffer, sector, 1);
			if(rest > 126)
			{
				fwrite(&buffer[0], 1, 126, fp);
			}
			else
			{
				fwrite(&buffer[0], 1, rest, fp);
				break;
			}
			if((buffer[126] == 0) && (buffer[127] == 0))
			{
				// ファイル読み込み終了
				sector = 0;
			}
			else
			{
				// 次のセクタ
				sector = static_cast<int>(buffer[126]) * 16 + static_cast<int>(buffer[127]) - 1;
			}
			rest -= 126;
		}
		// 書き込み終了
		fclose(fp);
		return 0;
	}
	catch(const std::exception& error)
	{
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
int Mz80Disk::PutFile(std::string path, void* dirInfo, unsigned int mode, unsigned int type)
{
	try
	{
		DIRECTORY* dirinfo = reinterpret_cast<DIRECTORY*>(dirInfo);
		// ディレクトリ検索
		int select = -1;
		for(int i = 0; i < 28; ++ i)
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
			throw std::runtime_error(dms::Format("Can not open file"));
		}
		if(fp == NULL)
		{
			// MZT ファイルを読み込むことができない
			throw std::runtime_error(dms::Format("Can not read file"));
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
				throw std::runtime_error(dms::Format("Invalid file size"));
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
			memcpy(filename, dirinfo->filename, 16);
		}
		for(int i = 0; i < 17; ++ i)
		{
			if(filename[i] == 0x0D)
			{
				filename[i] = '\0';
			}
		}
		for(int i = 0; i < 28; ++ i)
		{
			if(this->directory[i].mode != 0)
			{
				// ファイルネーム
				if(strncmp(this->directory[i].filename, filename, strlen(filename)) == 0)
				{
					// 同じファイル名が存在する
					throw std::runtime_error(dms::Format("Already same file name"));
				}
			}
		}
		// ファイルデータを読み込む
		size_t readSize = (dataSize + this->sectorSize - 1) / this->sectorSize * this->sectorSize;
		std::vector<unsigned char> bufferTemp;
		bufferTemp.resize(readSize, 0);
		fread(&bufferTemp[0], 1, dataSize, fp);
		fclose(fp);
		if(PutFileData(dirinfo, mode, mzthead, select, dataSize, bufferTemp) == false)
		{
			throw std::runtime_error(dms::Format("Faild write file"));
		}
		// 管理情報をD88イメージに書き込み
		FlushWrite();
		return 0;
	}
	catch(const std::exception& error)
	{
		return 1;
	}
}

bool Mz80Disk::PutFileData(DIRECTORY *dirinfo, unsigned int mode, MZTHEAD& mzthead, int select, size_t dataSize, std::vector<unsigned char>& bufferTemp)
{
	if(GetBitmapSize() < dataSize)
	{
		return false;
	}
	// ファイル情報をディレクトリに登録
	int rest;
	if((FILEMODE_MZT == mode) && (dirinfo == NULL))
	{
		memset(&this->directory[select], 0, sizeof(DIRECTORY));
		this->directory[select].mode = mzthead.mode;
		memcpy(this->directory[select].filename, mzthead.filename, 16);
		this->directory[select].filename[15] = '\0';
		this->directory[select].attr = 0;
		this->directory[select].size = mzthead.size;
		this->directory[select].loadAdr = mzthead.loadAdr;
		this->directory[select].runAdr = mzthead.runAdr;
		rest = mzthead.size;
	}
	else
	{
		memcpy(&this->directory[select], dirinfo, sizeof(DIRECTORY));
		rest = dirinfo->size;
	}
	int temp = this->bitmap[2] + this->bitmap[3] * 256;
	int dataSectorSize = ((static_cast<int>(dataSize) + this->sectorSize - 1) / this->sectorSize);
	this->bitmap[2] = (temp + dataSectorSize) & 255;
	this->bitmap[3] = ((temp + dataSectorSize) / 256) & 255;
	// ファイルをディスクイメージに転送
	int before = -1;
	int freeSector = 64 + GetBitmap();
	this->directory[select].startTrack = freeSector / 16;
	this->directory[select].startSector = freeSector % 16 + 1;
	unsigned char* source = &bufferTemp[0];
	std::vector<unsigned char> writeBuffer;
	writeBuffer.resize(this->sectorSize, 0);
	while(rest > 0)
	{
		freeSector = GetBitmap();
		SetBitmap(freeSector, 1);
		int sector = 64 + freeSector;
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
			beforeBuffer[static_cast<size_t>(this->sectorSize) - 2] = sector / 16;		// 次のトラック
			beforeBuffer[static_cast<size_t>(this->sectorSize) - 1] = sector % 16 + 1;	// 次のセクタ
			WriteSector(beforeBuffer, before, 1);
		}
		rest -= write_size;
		if( rest <= 0 )
		{
			break;
		}
		source += (static_cast<size_t>(this->sectorSize) - 2);
		before = sector;
	}
	return true;
}

//============================================================================
//  ディスクイメージのファイルを削除する
//----------------------------------------------------------------------------
// In  : dirindex = ディレクトリ番号
// Out : 0 = 正常終了
//============================================================================
int Mz80Disk::DelFile(int dirindex)
{
	if(this->directory[dirindex].mode == 0)
	{
		return 1;
	}
	int sector = this->directory[dirindex].startTrack * 16 + this->directory[dirindex].startSector - 1;
	while(sector > 0)
	{
		int start = sector - 64;
		DelBitmap(start, 1);
		std::vector<unsigned char> buffer;
		ReadSector(buffer, sector, 1);
		sector = static_cast<int>(buffer[this->sectorSize - 2]) * 16 + static_cast<int>(buffer[this->sectorSize - 1]) - 1;
	}
	this->directory[dirindex].mode = 0;
	// 管理情報をD88イメージに書き込み
	FlushWrite();
	return 0;
}

//============================================================================
//  ディスクイメージからブートプログラムを取り出す
//----------------------------------------------------------------------------
// In  : path = 保存位置
//     : mode = ファイルモード
// Out : 0 = 正常終了
//============================================================================
int Mz80Disk::GetBoot(std::string path, unsigned int mode)
{
	std::vector<unsigned char> iplBuffer;
	ReadSector(iplBuffer, 0, 1);
	if(iplBuffer[0] != 0xC3)
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
		memcpy(mzthead.filename, "IplBoot", 7);
		for(int i = 0; i < 17; ++ i)
		{
			if(mzthead.filename[i] == 0)
			{
				mzthead.filename[i] = '\xD';
				break;
			}
		}
		mzthead.size = this->sectorSize * 14;
		// ヘッダ情報書き込み
		fwrite(&mzthead, 1, 128, fp);
	}
	// データ書き込み
	int writesize = this->sectorSize * 14;
	std::vector<unsigned char> writeBuffer;
	int sectorSize = (writesize + this->sectorSize - 1) / this->sectorSize;
	ReadSector(writeBuffer, 0, sectorSize);
	fwrite(&writeBuffer[0], 1, writesize, fp);
	// 書き込み終了
	fclose( fp );
	return 0;
}

//============================================================================
//  ディスクイメージにブートプログラムを書き込む
//----------------------------------------------------------------------------
// In  : path = ファイル名
//     : mode = ファイルモード
//     :   FILEMODE_MZT
//     :   FILEMODE_BIN
//     : machine = 機種
//     :   01h MZ-80B/2000/2200
//     :   02h MZ-80A
//     :   03h MZ-700/1500
// Out : 0 = 正常終了
//============================================================================
int Mz80Disk::PutBoot(std::string path, void* iplInfo, unsigned int mode, unsigned char machine)
{
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
	WriteSector(bufferFile, 0, fileSectorCount);
	FlushWrite();
	return 0;
}

//============================================================================
//  ビットマップから空き領域を検索する
//----------------------------------------------------------------------------
// Out : 空きサイズ
//============================================================================
int Mz80Disk::GetBitmapSize(void)
{
	int freesize = 0;
	unsigned char bit;
	int end = 35 * 2 * 16 - 64; // 35シリンダ2ヘッド16セクタ(1120) - データ開始セクタ(64) = 1056
	for(int i = 48; i < end; ++ i)
	{
		bit = (this->bitmap[i / 8] >> (i % 8)) & 1;
		if(bit == 0)
		{
			freesize ++;
		}
	}
	return freesize * this->sectorSize;
}

//============================================================================
//  ビットマップから空き領域を検索する
//----------------------------------------------------------------------------
// Out : 空きサイズ
//============================================================================
int Mz80Disk::GetBitmap(void)
{
	unsigned char bit;
	int end = 35 * 2 * 16 - 64; // 35シリンダ2ヘッド16セクタ(1120) - データ開始セクタ(64) = 1056
	for(int i = 32; i < end; ++ i)
	{
		bit = (this->bitmap[i / 8] >> (i % 8)) & 1;
		if(bit == 0)
		{
			return i - 32;
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
int Mz80Disk::GetBitmapSerial(int length)
{
	int lengthtemp = 0;
	unsigned char bit;
	int index = -1;
	int lengthSector = (length + this->sectorSize - 1) / this->sectorSize;
	int end = 35 * 2 * 16 - 64; // 35シリンダ2ヘッド16セクタ(1120) - データ開始セクタ(64) = 1056
	for(int i = 32; i < end; ++ i)
	{
		bit = (this->bitmap[i / 8] >> (i % 8)) & 1;
		if(bit == 0)
		{
			if(index == -1)
			{
				index = i;
			}
			lengthtemp ++;
			if(lengthtemp >= lengthSector)
			{
				index -= 32;
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
void Mz80Disk::SetBitmap(int start, int length)
{
	unsigned char* data;
	start += 32;
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
void Mz80Disk::DelBitmap(int start, int length)
{
	start += 32;
	for(int i = start; i < (start + length); ++ i)
	{
		unsigned char* data = &this->bitmap[i / 8];
		unsigned char tmp = (~(1 << (i % 8)));
		*data &= (~(1 << (i % 8)));
	}
	WriteUseSize();
}

//============================================================================
//  ディレクトリ位置を設定する
//----------------------------------------------------------------------------
// In  : ディレクトリのあるセクタ番号 (-1=ルートディレクトリ)
// Out : なし
//============================================================================
void Mz80Disk::SetDirSector( int sector )
{
	if( -1 == sector )
	{
		this->dirSector = 0x10;
	}
	else
	{
		this->dirSector = sector;
	}
}

//============================================================================
//  ディレクトリ位置を取得する
//----------------------------------------------------------------------------
// In  : なし
// Out : ディレクトリのあるセクタ番号
//============================================================================
int Mz80Disk::GetDirSector( void )
{
	return this->dirSector;
}

//============================================================================
//  ディレクトリ情報を取得する
//----------------------------------------------------------------------------
// In  : ディレクトリ番号 (0～63);
// Out : なし
//============================================================================
void Mz80Disk::GetDir(void* dirData, int dirindex)
{
	DIRECTORY* dirdata = reinterpret_cast<DIRECTORY*>(dirData);
	*dirdata = this->directory[dirindex];
}

//============================================================================
//  ディレクトリを表示する (ルートのみ)
//----------------------------------------------------------------------------
// In  : なし
// Out : なし
//============================================================================
#if true
void Mz80Disk::DisplayDir(void)
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
	for(int i = 0; i < 28; ++ i)
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
				memcpy(&fileName, this->directory[i].filename, 16);
				for(int j = 0; j < 16; ++ j)
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
				// 格納トラック
				printf("%02X\n", this->directory[i].startTrack);
				// 格納セクタ
				printf("%02X\n", this->directory[i].startSector);
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
void Mz80Disk::GetExtFilename(std::string path, std::string& extfilename)
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
void Mz80Disk::DeleteFileExtname(std::string path)
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

void Mz80Disk::WriteUseSize(void)
{
	int size = 0;
	int end = 35 * 2 * 16 - 64; // 35シリンダ2ヘッド16セクタ(1120) - データ開始セクタ(64) = 1056
	for(int i = 32; i < end; ++ i)
	{
		unsigned char bit = (this->bitmap[i / 8] >> (i % 8)) & 1;
		if(bit == 1)
		{
			++ size;
		}
	}
	this->bitmap[2] = size % 256;
	this->bitmap[3] = size / 256;
}

// ビットマップ、ディレクトリをD88に書き込む
void Mz80Disk::FlushWrite(void)
{
	std::vector<unsigned char> writeBuffer;
	// ビットマップ書き込み
	std::copy(this->bitmap.begin(), this->bitmap.end(), std::back_inserter(writeBuffer));
	WriteSector(writeBuffer, 14, 2);
	// ディレクトリ書き込み
	WriteDirectory();
}

int Mz80Disk::GetUseBlockSize(void)
{
	if(this->image.IsValid() == false)
	{
		return 0;
	}
	return this->bitmap[2] + this->bitmap[3] * 256;
}

int Mz80Disk::GetAllBlockSize(void)
{
	if(this->image.IsValid() == false)
	{
		return 0;
	}
	return 35 * 2 * 16 - 64;
}

int Mz80Disk::GetClusterSize(void)
{
	if(this->image.IsValid() == false)
	{
		return 0;
	}
	return this->sectorSize;
}

// D88イメージからディレクトリデータをthis->directryに読み込む
void Mz80Disk::ReadDirectory(void)
{
	if(this->image.IsValid() == false)
	{
		return;
	}
	this->directory.clear();
	std::vector<unsigned char> buffer;
	ReadSector(buffer, this->dirSector, 14);
	size_t directoryCount = this->sectorSize * 14 / 64;
	for(size_t i = 0; i < directoryCount; ++ i)
	{
		DIRECTORY* directory = reinterpret_cast<DIRECTORY*>(&buffer[i * 64]);
		this->directory.push_back(*directory);
	}
}

// ディレクトリデータをthis->directryからD88イメージに書き込む
void Mz80Disk::WriteDirectory(void)
{
	std::vector<unsigned char> writeBuffer;
	writeBuffer.clear();
	unsigned char* copyBuffer = reinterpret_cast<unsigned char*>(&this->directory[0]);
	std::copy(copyBuffer, copyBuffer + static_cast<size_t>(this->sectorSize) * 14, std::back_inserter(writeBuffer));
	WriteSector(writeBuffer, this->dirSector, 14);
}

void Mz80Disk::ReadSector(std::vector<unsigned char>& buffer, int sector, int numOfSector)
{
	for(int i = 0; i < numOfSector; ++ i)
	{
		int readSector = sector + i;
		int track = readSector / 16;
		int c = track / 2;
		int h = track % 2;
		int r = readSector % 16 + 1;
		D88Image::SectorInfo sectorInfo;
		std::vector<unsigned char> readBuffer;
		this->image.ReadSector(sectorInfo, readBuffer, c, h, r);
		std::copy(readBuffer.begin(), readBuffer.end(), std::back_inserter(buffer));
	}
}

void Mz80Disk::WriteSector(std::vector<unsigned char>& buffer, int sector, int numOfSector)
{
	for(int i = 0; i < numOfSector; ++ i)
	{
		int writeSector = sector + i;
		int track = writeSector / 16;
		int c = track / 2;
		int h = track % 2;
		int r = writeSector % 16 + 1;
		D88Image::SectorInfo sectorInfo;
		this->image.GetSectorInfo(sectorInfo, c, h, r);
		std::vector<unsigned char> writeBuffer;
		std::copy(&buffer[static_cast<size_t>(this->sectorSize) * i], &buffer[static_cast<size_t>(this->sectorSize) * i] + this->sectorSize, std::back_inserter(writeBuffer));
		this->image.WriteSector(sectorInfo, writeBuffer, c, h, r);
	}
}
