#ifndef MZ80DISK_HPP
#define MZ80DISK_HPP

#include <vector>
#include <string>
#include "D88Image.hpp"
#include "Disk.hpp"

class Mz80Disk : public Disk
{
public:
	// 定数定義
	static const unsigned int MODE_FILE   = 0;
	static const unsigned int MODE_MEMORY = 1;
	static const unsigned char TYPE_2S  = 0x50;
	static const unsigned int FILETYPE_OBJ          = 0x00000001;
	static const unsigned int FILETYPE_BTX          = 0x00000002;
	static const unsigned int DISKTYPE_MZ80_SP6010_2S = D88Image::DISK_2S_35_128_16;
#ifdef _MSC_VER
#pragma pack(push, 1)
#else
#pragma pack(1)
#endif
	// ディレクトリ構造
	struct DIRECTORY
	{
		unsigned char mode; // 01h OBJ, 02h BTX
		char filename[16];
		unsigned char attr; // 00h 通常, 01h プロテクト
		unsigned short size;
		unsigned short loadAdr;
		unsigned short runAdr;
		unsigned char reserve[38];
		unsigned char startTrack;
		unsigned char startSector;
	};
	// テープヘッダ構造
	struct MZTHEAD
	{
		unsigned char mode;
		char filename[17];
		unsigned short size;
		unsigned short loadAdr;
		unsigned short runAdr;
		unsigned char reserve[104];
	};
#ifdef _MSC_VER
	#pragma pack(pop)
#else
	#pragma pack(0)
#endif
	Mz80Disk();
	~Mz80Disk();
	int DiskType(void);
	std::string DiskTypeText(void);
	void Format(int type, int volumeNumber);
	int Load(std::string path);
	int Load(const std::vector<unsigned char>& buffer);
	void Update(void);
	int Save(std::string path);
	int Save(std::vector<unsigned char>& buffer);
	int GetFile(int dirindex, std::string path, unsigned int mode);
	int PutFile(std::string path, void* dirInfo, unsigned int mode, unsigned int type = FILETYPE_OBJ);
	int DelFile(int dirindex);
	int GetBoot(std::string path, unsigned int mode);
	int PutBoot(std::string path, void* iplInfo, unsigned int mode, unsigned char machine = 0x01);
	int GetSystem(std::string path, unsigned int mode);
	int PutSystem(std::string path, void* iplInfo, unsigned int mode);
	int GetBitmapSize(void);
	int GetBitmap(void);
	int GetBitmapSerial(int length);
	void SetBitmap(int start, int length);
	void DelBitmap(int start, int length);
	std::string ConvertText(std::string text);
	std::string ConvertMzText(std::string text);
	int GetType(void);
	void ChangeType(int type);
	int FindFile(std::string filename, int ignoreIndex);
	void SetDirSector(int sector);
	int GetDirSector(void);
	int GetDirCount(void);
	void GetDir(void* dirData, int dirindex);
	void SetDir(void* dirData, int dirindex);
	int GetUseBlockSize(void);
	int GetAllBlockSize(void);
	int GetClusterSize(void);
	void DisplayDir(void);
private:
	unsigned char diskType;
	std::vector<unsigned char> bitmap;
	std::vector<DIRECTORY> directory;
	int fileType;
	int dirSector;
	static const char asciiCodeAnk[];
	static const char asciiCodeSjis[];
	Mz80Disk(Mz80Disk&);
	Mz80Disk& operator = (Mz80Disk&);
	void GetExtFilename(std::string path, std::string& extfilename);
	void DeleteFileExtname(std::string path);
	void WriteUseSize(void);
	void FlushWrite(void);
	void ReadDirectory(void);
	void WriteDirectory(void);
	void ReadSector(std::vector<unsigned char>& buffer, int sector, int numOfSector);
	void WriteSector(std::vector<unsigned char>& buffer, int sector, int numOfSector);
	bool PutFileData(DIRECTORY *dirinfo, unsigned int mode, MZTHEAD& mzthead, int select, size_t dataSize, std::vector<unsigned char>& bufferTemp);
};

#endif
