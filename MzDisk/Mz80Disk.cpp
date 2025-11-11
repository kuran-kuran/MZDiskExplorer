#include <stdlib.h>
#include <stdexcept>
#include <iterator>
#include "Format.hpp"
#include "FileData.hpp"
#include "Mz80Disk.hpp"

#define IsKanji(c) ( (unsigned char)((int)((unsigned char)(c) ^ 0x20) - 0x0A1) < 0x3C )

/* MZ-80K/C,1200,700 */
const char Mz80Disk::asciiCodeAnk[] =
{
	" !\x22#$%&\x27()*+,-./"	/* 20 */
	"0123456789:;<=>?"		/* 30 */
	"@ABCDEFGHIJKLMNO"		/* 40 */
	"PQRSTUVWXYZ[.].."		/* 50 */
	"................"		/* 60 */
	".............\\.."		/* 70 */
	".｡｢｣､.ｦｧｨｩｪｫｬｭｮｯ"		/* 80 */
	"ｰｱｲｳｴｵｶｷｸｹｺｻｼｽｾｿ"		/* 90 */
	"ﾀﾁﾂﾃﾄﾅﾆﾇﾈﾉﾊﾋﾌﾍﾎﾏ"		/* A0 */
	"ﾐﾑﾒﾓﾔﾕﾖﾗﾘﾙﾚﾛﾜﾝﾞﾟ"		/* B0 */
	"................"		/* C0 */
	"................"		/* D0 */
	"................"		/* E0 */
	"................"		/* F0 */
};
const char Mz80Disk::asciiCodeSjis[] =
{
	"　！”＃＄％＆’（）＊＋，－．／"	/* 20 */
	"０１２３４５６７８９：；＜＝＞？"	/* 30 */
	"＠ＡＢＣＤＥＦＧＨＩＪＫＬＭＮＯ"	/* 40 */
	"ＰＱＲＳＴＵＶＷＸＹＺ［＼］↑←"	/* 50 */
	"※※※※※※※※※※※※※※※※"	/* 60 */
	"日月火水木金土生年時分秒円￥￡▼"	/* 70 */
	"↓。「」、．ヲァィゥェォャュョッ"	/* 80 */
	"ーアイウエオカキクケコサシスセソ"	/* 90 */
	"タチツテトナニヌネノハヒフヘホマ"	/* A0 */
	"ミムメモヤユヨラリルレロワン゛゜"	/* B0 */
	"→※※※※※※※※※※※※※※※"	/* C0 */
	"※※※※※※※※※※※※※※※※"	/* D0 */
	"※※※※※※※※※※※※※※※※"	/* E0 */
	"※※※※※※※※※※※※※※※π"	/* F0 */
};

//============================================================================
// コンストラクタ
//============================================================================
Mz80Disk::Mz80Disk()
:diskType(0)
,bitmap(NULL)
,fileType(0)
,dirSector(16)
{
	this->sectorSize = 128;
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
	return "2S MZ-80K SP-6010タイプディスク";
}

//============================================================================
//  ディスクイメージをフォーマットする
//----------------------------------------------------------------------------
// In  : path = イメージファイル
//     : type
//     : DISKTYPE_MZ80_SP6010_2S   // MZ-80K SP-6010
//     : DISKTYPE_MZ80_SP6110_2S   // MZ-80K SP-6110 非対応
// Out : エラーコード ( 0 = 正常 )
//============================================================================
void Mz80Disk::Format(int type, int volumeNumber)
{
	if(type != DISKTYPE_MZ80_SP6010_2S)
	{
		return;
	}
	this->diskType = D88Image::DISK_2S_35_128_16;
	// 物理フォーマット
	this->image.Format(type, 0x00);
	// 論理フォーマット
	// ディレクトリ
	std::vector<unsigned char> buffer;
	buffer.resize(static_cast<size_t>(this->sectorSize) * 14, 0);
	WriteSector(buffer, 16, 14);
	// ビットマップ
	this->bitmap.clear();
	this->bitmap.resize(static_cast<size_t>(this->sectorSize) * 2, 0);
	this->bitmap[1] = static_cast<unsigned char>(volumeNumber);
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
		Update();
		return 0;
	}
	catch(const std::exception& error)
	{
		(void)error;
		return 1;
	}
}

void Mz80Disk::Update(void)
{
	// ディスク情報格納
	D88Image::Header header;
	this->image.GetHeader(header);
	int trackMax = 0;
	for(int i = 0; i < D88Image::TRACK_COUNT; ++ i)
	{
		if(header.trackTable[i] != 0)
		{
			++ trackMax;
		}
	}
	ReadDirectory();
	this->diskType = TYPE_2S;

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
int Mz80Disk::PutFile(std::string path, void* dirInfo, unsigned int mode, unsigned int type)
{
	try
	{
		DIRECTORY* dirinfo = reinterpret_cast<DIRECTORY*>(dirInfo);
		// ディレクトリ検索
		int select = -1;
		for(int i = 0; i < 28; ++ i)
		{
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
				// ファイル名の長さが同じ?
				if(strlen(this->directory[i].filename) == strlen(filename))
				{
					// ファイルネームが同じ?
					if(strncmp(this->directory[i].filename, filename, strlen(filename)) == 0)
					{
						// 同じファイル名が存在する
						return 5;
					}
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
			// ビットマップの空き容量が無い
			return 4;
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
		sector = static_cast<int>(buffer[static_cast<size_t>(this->sectorSize) - 2]) * 16 + static_cast<int>(buffer[static_cast<size_t>(this->sectorSize) - 1]) - 1;
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
		memcpy(mzthead.filename, "BOOT", 5);
		for(int i = 0; i < 17; ++ i)
		{
			if(mzthead.filename[i] == 0)
			{
				mzthead.filename[i] = '\xD';
				break;
			}
		}
		mzthead.size = this->sectorSize * 14;
		mzthead.loadAdr = 0x9800;
		mzthead.runAdr = 0x9800;
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
//  ディスクイメージからシステムプログラムを取り出す
//----------------------------------------------------------------------------
// In  : path = 保存位置
//     : mode = ファイルモード
// Out : 0 = 正常終了
//============================================================================
int Mz80Disk::GetSystem(std::string path, unsigned int mode)
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
	int writesize = this->sectorSize * 184;
	if((mode & FILEMODE_MASK) == FILEMODE_MZT)
	{
		// ヘッダ情報作成
		MZTHEAD mzthead;
		memset(&mzthead, 0, sizeof(MZTHEAD));
		mzthead.mode = 1;
		memcpy(mzthead.filename, "SYSTEM", 7);
		for(int i = 0; i < 17; ++ i)
		{
			if(mzthead.filename[i] == 0)
			{
				mzthead.filename[i] = '\xD';
				break;
			}
		}
		mzthead.size = writesize;
		mzthead.loadAdr = 0x1200;
		mzthead.runAdr = 0x21FA;
		// ヘッダ情報書き込み
		fwrite(&mzthead, 1, 128, fp);
	}
	// データ書き込み
	std::vector<unsigned char> writeBuffer;
	int sectorSize = (writesize + this->sectorSize - 1) / this->sectorSize;
	ReadSector(writeBuffer, 64, sectorSize);
	fwrite(&writeBuffer[0], 1, writesize, fp);
	// 書き込み終了
	fclose( fp );
	return 0;
}

//============================================================================
//  ディスクイメージにシステムプログラムを書き込む
//----------------------------------------------------------------------------
// In  : path = ファイル名
//     : mode = ファイルモード
//     :   FILEMODE_MZT
//     :   FILEMODE_BIN
// Out : 0 = 正常終了
//============================================================================
int Mz80Disk::PutSystem(std::string path, void* iplInfo, unsigned int mode)
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
	WriteSector(bufferFile, 64, fileSectorCount);
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

// MZ-80Kの文字をWindowsで使える文字に変換する
std::string Mz80Disk::ConvertText(std::string text)
{
	std::string result;
	for(size_t i = 0; i < text.size(); ++ i)
	{
		if(IsNotAvailableFileCharacter(text[i]) == true)
		{
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

// Windowsの文字をで使える文字に変換する
std::string Mz80Disk::ConvertMzText(std::string text)
{
	std::string result = "";
	bool kanji = false;
	std::string word = "";
	for(size_t i = 0; i < text.size(); ++ i)
	{
		if(IsKanji(text[i]))
		{
			word.push_back(text[i]);
			kanji = true;
			continue;
		}
		else
		{
			word.push_back(text[i]);
		}
		if(kanji == true)
		{
			size_t findIndex = std::string(asciiCodeSjis).find(word);
			std::string a;
			size_t l = strlen(asciiCodeSjis);
			a.push_back(asciiCodeSjis[findIndex]);
			a.push_back(asciiCodeSjis[findIndex + 1]);
			if(findIndex != std::string::npos)
			{
				// 全角文字が見つかった
				word = "";
				word.push_back(static_cast<char>(findIndex / 2 + 0x20));
			}
			else
			{
				word = "";
			}
		}
		else
		{
			size_t findIndex = std::string(asciiCodeAnk).find(word);
			if(findIndex != std::string::npos)
			{
				// 半角文字が見つかった
				word = "";
				word.push_back(static_cast<char>(findIndex + 0x20));
			}
			else
			{
				word = "";
			}
		}
		result += word;
		word = "";
		kanji = false;
	}
	return result;
}

//============================================================================
//  ファイルを検索する
//----------------------------------------------------------------------------
// In  : path: ファイル名
// Out : -1: なし, 0～: ファイルがあるディレクトリのインデックス
//============================================================================
int Mz80Disk::FindFile(std::string filename, int ignoreIndex)
{
	for(int i = 0; i < 64; ++ i)
	{
		if(this->directory[i].mode != 0)
		{
			size_t directryFileLength = 0;
			for(size_t j = 0; j < 17; ++ j)
			{
				if(this->directory[i].filename[j] == 0x0D)
				{
					break;
				}
				++ directryFileLength;
			}
			size_t fileLength = filename.size();
			// ファイル名の長さが同じ
			if(directryFileLength == fileLength)
			{
				// ファイルネーム
				if((strncmp(this->directory[i].filename, &filename[0], filename.size()) == 0) && (i != ignoreIndex))
				{
					// 同じファイル名が存在する
					return i;
				}
			}
		}
	}
	return -1;
}

//============================================================================
//  ディスクのタイプを取得する
//----------------------------------------------------------------------------
// In  : なし
// Out : TYPE_2D: 2Dに変更する, TYPE_2DD: 2DDに変更する
//============================================================================
int Mz80Disk::GetType(void)
{
	return this->diskType;
}

//============================================================================
//  ディスクのタイプを変更する
//----------------------------------------------------------------------------
// MZ-80Kでは未使用
//============================================================================
void Mz80Disk::ChangeType(int type)
{
}

//============================================================================
//  IPLセレクタを登録する
//----------------------------------------------------------------------------
// MZ-80Kでは未使用
//============================================================================
void Mz80Disk::PutIplSelector(std::string path)
{
	dms::FileData fileData;
	std::vector<unsigned char> buffer;
	try
	{
		fileData.Load(path.c_str());
		fileData.GetBuffer(buffer);
	}
	catch (const std::exception& error)
	{
		(void)error;
		static const unsigned char iplSelectorImage[] =
		{
			0xC3, 0x03, 0x98, 0x3E, 0x0A, 0x32, 0x07, 0x10, 0x3E, 0xC3, 0x32, 0x0B, 0x10, 0x21, 0x63, 0x9A,
			0x22, 0x0C, 0x10, 0xCD, 0x06, 0x00, 0xCD, 0x06, 0x00, 0x11, 0x82, 0x9A, 0xCD, 0x15, 0x00, 0xCD,
			0x06, 0x00, 0xCD, 0x06, 0x00, 0x3E, 0x20, 0x21, 0x00, 0x88, 0x77, 0x11, 0x01, 0x88, 0x01, 0x44,
			0x02, 0xED, 0xB0, 0xAF, 0x32, 0x00, 0x8C, 0x32, 0x01, 0x8C, 0x32, 0x0A, 0x8C, 0x32, 0x00, 0x8B,
			0x21, 0x00, 0x8B, 0x77, 0x11, 0x01, 0x8B, 0x01, 0xE8, 0x00, 0xED, 0xB0, 0x21, 0x00, 0x80, 0x22,
			0x02, 0x8C, 0x21, 0x00, 0x88, 0x22, 0x04, 0x8C, 0x21, 0x00, 0x8B, 0x22, 0x08, 0x8C, 0x3A, 0x11,
			0x10, 0x32, 0x2E, 0x9B, 0xDD, 0x21, 0x2E, 0x9B, 0xCD, 0x3B, 0xF1, 0xCD, 0xA7, 0xF0, 0x2A, 0x02,
			0x8C, 0x7E, 0xFE, 0x80, 0xCA, 0x03, 0x99, 0x2A, 0x02, 0x8C, 0x7E, 0xFE, 0x01, 0x20, 0x51, 0x3A,
			0x00, 0x8C, 0x3C, 0x32, 0x00, 0x8C, 0xC6, 0x40, 0xED, 0x5B, 0x04, 0x8C, 0x12, 0x13, 0x3E, 0x3A,
			0x12, 0x13, 0x23, 0x06, 0x10, 0x7E, 0xFE, 0x0D, 0x28, 0x06, 0x12, 0x23, 0x13, 0x05, 0x20, 0xF5,
			0x2A, 0x02, 0x8C, 0x01, 0x3E, 0x00, 0x09, 0xED, 0x5B, 0x08, 0x8C, 0x01, 0x02, 0x00, 0xED, 0xB0,
			0x2A, 0x02, 0x8C, 0x01, 0x12, 0x00, 0x09, 0x01, 0x06, 0x00, 0xED, 0xB0, 0x2A, 0x04, 0x8C, 0x01,
			0x14, 0x00, 0x09, 0x22, 0x04, 0x8C, 0x2A, 0x08, 0x8C, 0x11, 0x08, 0x00, 0x19, 0x22, 0x08, 0x8C,
			0x2A, 0x02, 0x8C, 0x11, 0x40, 0x00, 0x19, 0x22, 0x02, 0x8C, 0x3A, 0x01, 0x8C, 0x3C, 0x32, 0x01,
			0x8C, 0xFE, 0x1A, 0x20, 0x92, 0x3A, 0x00, 0x8C, 0xFE, 0x00, 0x20, 0x1C, 0x11, 0xD1, 0x9A, 0xCD,
			0x15, 0x00, 0xCD, 0x06, 0x00, 0x3E, 0x05, 0xCD, 0x41, 0x00, 0x11, 0x1F, 0x9B, 0xCD, 0x30, 0x00,
			0xC3, 0x82, 0x00, 0x11, 0xFC, 0x9A, 0x18, 0xE7, 0x2A, 0x04, 0x8C, 0x2B, 0x3E, 0x0D, 0x77, 0x11,
			0x00, 0x88, 0xCD, 0x18, 0x00, 0xCD, 0x06, 0x00, 0xCD, 0x06, 0x00, 0x11, 0xDD, 0x9A, 0xCD, 0x15,
			0x00, 0x11, 0x00, 0x8F, 0xCD, 0x03, 0x00, 0x21, 0x1E, 0x00, 0x19, 0x7E, 0xFE, 0x21, 0xCA, 0x82,
			0x00, 0xFE, 0x23, 0x28, 0xDA, 0xD6, 0x41, 0x38, 0xE2, 0x32, 0x0B, 0x8C, 0x47, 0x3A, 0x00, 0x8C,
			0x3D, 0xB8, 0x38, 0xD7, 0x3A, 0x0B, 0x8C, 0x01, 0x14, 0x00, 0x21, 0x00, 0x88, 0xFE, 0x00, 0x28,
			0x04, 0x09, 0x3D, 0x20, 0xFC, 0x11, 0xE8, 0x8B, 0x01, 0x14, 0x00, 0xED, 0xB0, 0x3E, 0x0D, 0x32,
			0xFC, 0x8B, 0xCD, 0x06, 0x00, 0x11, 0x25, 0x9B, 0xCD, 0x15, 0x00, 0x11, 0xEA, 0x8B, 0xCD, 0x15,
			0x00, 0xCD, 0x06, 0x00, 0x01, 0x08, 0x00, 0x21, 0x00, 0x8B, 0x3A, 0x0B, 0x8C, 0xFE, 0x00, 0x28,
			0x04, 0x09, 0x3D, 0x20, 0xFC, 0x22, 0x0C, 0x8C, 0xCD, 0x59, 0x9A, 0xE5, 0x21, 0x2F, 0x9B, 0xCD,
			0x5E, 0x9A, 0xE1, 0x23, 0x23, 0xCD, 0x59, 0x9A, 0xE5, 0x21, 0x31, 0x9B, 0xCD, 0x5E, 0x9A, 0xE1,
			0x23, 0x23, 0xCD, 0x59, 0x9A, 0xE5, 0x21, 0x33, 0x9B, 0xCD, 0x5E, 0x9A, 0xE1, 0xDD, 0x21, 0x2E,
			0x9B, 0xCD, 0xC6, 0x99, 0xCD, 0xA7, 0xF0, 0x31, 0xF0, 0x10, 0x2A, 0x0C, 0x8C, 0x01, 0x06, 0x00,
			0x09, 0x5E, 0x23, 0x56, 0xEB, 0xE9, 0x3E, 0x0A, 0x32, 0x07, 0x10, 0xCD, 0xFF, 0xF0, 0x3A, 0x01,
			0x10, 0x47, 0x0E, 0xF8, 0xD9, 0x0E, 0xFB, 0x1E, 0x03, 0xDD, 0x6E, 0x05, 0xDD, 0x66, 0x06, 0xCD,
			0xBD, 0xF0, 0xAF, 0xDD, 0x7E, 0x01, 0x1F, 0xD3, 0xF9, 0xDD, 0x7E, 0x02, 0x30, 0x02, 0xF6, 0x80,
			0xD3, 0xF8, 0xCD, 0xA6, 0xF1, 0x3E, 0x70, 0x32, 0x00, 0x10, 0xF3, 0xD3, 0xFA, 0x06, 0x80, 0xDB,
			0xF9, 0xA3, 0x28, 0xFB, 0x0F, 0x30, 0x2D, 0xED, 0xA2, 0xC2, 0xFF, 0x99, 0xD9, 0xED, 0x78, 0xDD,
			0x6E, 0x05, 0xDD, 0x66, 0x06, 0x01, 0x7E, 0x00, 0x09, 0xDD, 0x75, 0x05, 0xDD, 0x74, 0x06, 0x7E,
			0xDD, 0x77, 0x01, 0x23, 0x7E, 0xDD, 0x77, 0x02, 0xDD, 0x7E, 0x01, 0xB7, 0x20, 0xA6, 0xDD, 0x7E,
			0x02, 0xB7, 0x20, 0xA0, 0xCD, 0xD9, 0xF0, 0xD0, 0x3A, 0x07, 0x10, 0x3D, 0x32, 0x07, 0x10, 0xCA,
			0x0B, 0x10, 0xCD, 0xB2, 0xF0, 0xC3, 0xC6, 0x99, 0xCD, 0x15, 0x00, 0x7E, 0x5F, 0x23, 0x7E, 0x57,
			0x23, 0xE5, 0xD5, 0xE1, 0xCD, 0xBA, 0x03, 0xE1, 0xC9, 0x5E, 0x23, 0x56, 0x2B, 0xC9, 0x73, 0x23,
			0x72, 0x2B, 0xC9, 0x31, 0xF0, 0x10, 0xCD, 0x06, 0x00, 0x11, 0x75, 0x9A, 0xCD, 0x15, 0x00, 0xCD,
			0xA7, 0xF0, 0xC3, 0x82, 0x00, 0x45, 0x52, 0x52, 0x4F, 0x52, 0x20, 0x28, 0x48, 0x4F, 0x4F, 0x4B,
			0x29, 0x0D, 0x3D, 0x3D, 0x3D, 0x20, 0x49, 0x50, 0x4C, 0x20, 0x53, 0x45, 0x4C, 0x45, 0x43, 0x54,
			0x45, 0x52, 0x20, 0x46, 0x4F, 0x52, 0x20, 0x4D, 0x5A, 0x2D, 0x38, 0x30, 0x4B, 0x2F, 0x43, 0x2C,
			0x31, 0x32, 0x30, 0x30, 0x20, 0x3D, 0x3D, 0x3D, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
			0x20, 0x56, 0x45, 0x52, 0x2E, 0x30, 0x2E, 0x33, 0x20, 0x20, 0x32, 0x30, 0x32, 0x31, 0x2E, 0x30,
			0x32, 0x2E, 0x32, 0x32, 0x20, 0x48, 0x49, 0x44, 0x45, 0x4B, 0x49, 0x20, 0x53, 0x55, 0x47, 0x41,
			0x0D, 0x4E, 0x4F, 0x20, 0x4F, 0x42, 0x4A, 0x20, 0x46, 0x49, 0x4C, 0x45, 0x0D, 0x42, 0x4F, 0x4F,
			0x54, 0x20, 0x53, 0x45, 0x4C, 0x45, 0x43, 0x54, 0x20, 0x5B, 0x21, 0x3A, 0x4D, 0x4F, 0x4E, 0x20,
			0x23, 0x3A, 0x4C, 0x49, 0x53, 0x54, 0x5D, 0x20, 0x3D, 0x3E, 0x20, 0x0D, 0x45, 0x52, 0x52, 0x3A,
			0x53, 0x4F, 0x52, 0x52, 0x59, 0x20, 0x53, 0x50, 0x2D, 0x36, 0x31, 0x31, 0x30, 0x20, 0x49, 0x53,
			0x20, 0x4E, 0x4F, 0x54, 0x20, 0x53, 0x55, 0x50, 0x50, 0x4F, 0x52, 0x54, 0x45, 0x44, 0x0D, 0xD7,
			0x43, 0x31, 0x43, 0x35, 0x0D, 0x4C, 0x4F, 0x41, 0x44, 0x49, 0x4E, 0x47, 0x20, 0x0D, 0x00, 0x01,
			0x01, 0x00, 0x07, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
		};
		buffer.resize(sizeof(iplSelectorImage));
		memcpy_s(&buffer[0], buffer.size(), iplSelectorImage, sizeof(iplSelectorImage));
	}
	WriteSector(buffer, 0, static_cast<int>(buffer.size()) / 128);
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

int Mz80Disk::GetDirCount(void)
{
	return static_cast<int>(this->directory.size());
}

//============================================================================
//  ディレクトリ情報を取得する
//----------------------------------------------------------------------------
// In  : ディレクトリ番号 (0～63);
// Out : なし
//============================================================================
void Mz80Disk::GetDir(void* dirData, int dirindex)
{
	memcpy(dirData, &this->directory[dirindex], sizeof(DIRECTORY));
}

//============================================================================
//  ディレクトリ情報を取得する
//----------------------------------------------------------------------------
// In  : ディレクトリ番号 (0～63);
// Out : なし
//============================================================================
void Mz80Disk::SetDir(void* dirData, int dirindex)
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
	ReadSector(buffer, this->dirSector, 48);
	size_t directoryCount = this->sectorSize * 48 / 64;
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
