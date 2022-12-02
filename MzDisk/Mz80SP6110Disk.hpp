#ifndef MZ80SP6110DISK_HPP
#define MZ80SP6110DISK_HPP

#include <vector>
#include <string>
#include "D88Image.hpp"
#include "Disk.hpp"

class Mz80SP6110Disk : public Disk
{
public:
	// 定数定義
	static const unsigned int MODE_FILE   = 0;
	static const unsigned int MODE_MEMORY = 1;
	static const unsigned char TYPE_2S  = 0x00;
	static const unsigned int FILETYPE_OBJ          = 0x00000001;
	static const unsigned int FILETYPE_BTX          = 0x00000002;
	static const unsigned int FILETYPE_BSD          = 0x00000003;
	static const unsigned int FILETYPE_BSD_CONV     = 0x00000005;
	static const unsigned int DISKTYPE_MZ80_SP6110_2S = D88Image::DISK_2S_35_128_16;
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
	Mz80SP6110Disk();
	~Mz80SP6110Disk();
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
//	void DisplayDir(void);
private:
	unsigned char diskType;
	std::vector<unsigned char> bitmap;
	std::vector<DIRECTORY> directory;
	int fileType;
	int clusterSize;
	int dirSector;
	Mz80SP6110Disk(Mz80SP6110Disk&);
	Mz80SP6110Disk& operator = (Mz80SP6110Disk&);
	void GetExtFilename(std::string path, std::string& extfilename);
	void DeleteFileExtname(std::string path);
	void WriteUseSize(void);
	void FlushWrite(void);
	void ReadDirectory(void);
	void WriteDirectory(void);
	void ConvertBsdFile(DIRECTORY *dirinfo, unsigned int mode, unsigned int& type, size_t readSize, size_t dataSize, std::vector<unsigned char>& bufferTemp);
	bool PutBsdFile(DIRECTORY *dirinfo, unsigned int mode, MZTHEAD& mzthead, int select, size_t dataSize, std::vector<unsigned char>& bufferTemp);
	bool PutObjFile(DIRECTORY *dirinfo, unsigned int mode, MZTHEAD& mzthead, int select, size_t dataSize, std::vector<unsigned char>& bufferTemp, unsigned int type);
	void DelBsdFile(int dirindex);
	void DelObjFile(int dirindex);
};

#endif
