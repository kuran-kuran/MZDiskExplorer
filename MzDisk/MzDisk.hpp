#ifndef MZDISK_HPP
#define MZDISK_HPP

#include <vector>
#include <string>
#include "D88Image.hpp"
#include "Disk.hpp"

class MzDisk : public Disk
{
public:
	// 定数定義
	static const unsigned int MODE_FILE   = 0;
	static const unsigned int MODE_MEMORY = 1;
	static const unsigned char TYPE_2D  = 0x00;
	static const unsigned char TYPE_2DD = 0x10;
	static const unsigned char TYPE_2HD = 0x20;
	static const unsigned char TYPE_1D  = 0x30;
	static const unsigned char TYPE_1DD = 0x40;
	static const unsigned int FILETYPE_OBJ          = 0x00000001;
	static const unsigned int FILETYPE_BTX          = 0x00000002;
	static const unsigned int FILETYPE_BSD          = 0x00000003;
	static const unsigned int FILETYPE_BRD          = 0x00000004;
	static const unsigned int FILETYPE_BSD_CONV     = 0x00000005;
	static const unsigned int DISKTYPE_MZ2500_2DD   = D88Image::DISK_2DD_80_256_16;
	static const unsigned int DISKTYPE_MZ2500_2DD40 = D88Image::DISK_2DD_40_256_16;
	static const unsigned int DISKTYPE_MZ2500_2DD35 = D88Image::DISK_2DD_35_256_16;
	static const unsigned int DISKTYPE_MZ80B_2D35   = D88Image::DISK_2D_35_256_16;
	static const unsigned int DISKTYPE_MZ2000_2D40  = D88Image::DISK_2D_40_256_16;
#ifdef _MSC_VER
#pragma pack(push, 1)
#else
#pragma pack(1)
#endif
	// ディレクトリ構造
	struct DIRECTORY
	{
		unsigned char mode;
		char filename[17];
		unsigned char attr;
		unsigned char reserve;
		unsigned short size;
		unsigned short loadAdr;
		unsigned short runAdr;
		unsigned int date;
		unsigned short startSector;
	};
	// IPL 構造
	struct IPL
	{
		unsigned char machine;
		char signature[6];
		char bootname[11];
		unsigned char reserve[2];
		unsigned short size;
		unsigned short reserve2;
		unsigned short runAdr;
		unsigned char reserve3[3];
		unsigned char master;
		unsigned short startSector;
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
	MzDisk();
	~MzDisk();
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
	void SetDirSector(int sector);
	int GetDirSector(void);
	int GetDirCount(void);
	void GetDir(void* dirData, int dirindex);
	void SetDir(void* dirData, int dirindex);
	int GetUseBlockSize(void);
	int GetClusterSize(void);
	int GetAllBlockSize(void);
	void ReadSector(std::vector<unsigned char>& buffer, int sector, int numOfSector);
	void WriteSector(std::vector<unsigned char>& buffer, int sector, int numOfSector);
	int GetBitmapSize(void);
	int GetBitmap(void);
	int GetBitmapSerial(int length);
	void SetBitmap(int start, int length);
	void DelBitmap(int start, int length);
	std::string ConvertText(std::string text);
	std::string ConvertMzText(std::string text);
	int FindFile(std::string filename, int ignoreIndex);
	//	void DisplayDir(void);
private:
	unsigned char diskType;
	std::vector<unsigned char> bitmap;
	std::vector<DIRECTORY> directory;
	int fileType;
	int clusterSize;
	int dirSector;
	static const char asciiCodeAnk[];
	static const char asciiCodeSjis[];
	MzDisk(MzDisk&);
	MzDisk& operator = (MzDisk&);
	void GetExtFilename(std::string path, std::string& extfilename);
	void DeleteFileExtname(std::string path);
	void WriteUseSize(void);
	void FlushWrite(void);
	void ReadDirectory(void);
	void WriteDirectory(void);
	void ConvertBsdFile(DIRECTORY *dirinfo, unsigned int mode, unsigned int& type, size_t readSize, size_t dataSize, std::vector<unsigned char>& bufferTemp);
	bool PutBsdFile(DIRECTORY *dirinfo, unsigned int mode, MZTHEAD& mzthead, int select, size_t dataSize, std::vector<unsigned char>& bufferTemp);
	bool PutBrdFile(DIRECTORY *dirinfo, unsigned int mode, MZTHEAD& mzthead, int select, size_t dataSize, std::vector<unsigned char>& bufferTemp);
	bool PutObjFile(DIRECTORY *dirinfo, unsigned int mode, MZTHEAD& mzthead, int select, size_t dataSize, std::vector<unsigned char>& bufferTemp, unsigned int type);
	void DelBsdFile(int dirindex);
	void DelBrdFile(int dirindex);
	void DelObjFile(int dirindex);
};

#endif
