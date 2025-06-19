#ifndef DISK_HPP
#define DISK_HPP

#include <string>
#include <vector>
#include "D88Image.hpp"

class Disk
{
public:
	enum
	{
		UNKNOWN = 0,
		MZ80K_SP6010,
		MZ80K_SP6110,
		MZ2000,
	};
	static const unsigned int FILEMODE_MZT          = 0x00000000;
	static const unsigned int FILEMODE_BIN          = 0x00000001;
	static const unsigned int FILEMODE_MASK         = 0x0000000F;
	static const unsigned int FILEMODE_NEWLINE_CONV = 0x00000010;
	Disk();
	~Disk();
	static int DiskType(std::string path);
	static int DiskType(const std::vector<unsigned char>& buffer);
	void ExportBeta(std::string path);
	void ImportBeta(std::string path);
	void ImportBetaBuffer(std::vector<unsigned char>& betaBuffer);
	void ExportBetaBuffer(std::vector<unsigned char>& betaBuffer);
	int GetTrackCount(void);
	bool IsRom(void);
	virtual int DiskType(void) = 0;
	virtual std::string DiskTypeText(void) = 0;
	virtual void Format(int type, int volumeNumber) = 0;
	virtual int Load(std::string path) = 0;
	virtual int Load(const std::vector<unsigned char>& buffer) = 0;
	virtual void Update(void) = 0;
	virtual int Save(std::string path) = 0;
	virtual int Save(std::vector<unsigned char>& buffer) = 0;
	virtual int GetFile(int dirindex, std::string path, unsigned int mode) = 0;
	virtual int PutFile(std::string path, void *dirInfo, unsigned int mode, unsigned int type) = 0;
	virtual int DelFile(int dirindex) = 0;
	virtual int GetBoot(std::string path, unsigned int mode) = 0;
	virtual int PutBoot(std::string path, void *iplInfo, unsigned int mode, unsigned char machine = 0x01) = 0;
	virtual int GetSystem(std::string path, unsigned int mode) = 0;
	virtual int PutSystem(std::string path, void* iplInfo, unsigned int mode) = 0;
	virtual void SetDirSector(int sector) = 0;
	virtual int GetDirSector(void) = 0;
	virtual int GetDirCount(void) = 0;
	virtual void GetDir(void* dirData, int dirindex) = 0;
	virtual void SetDir(void* dirData, int dirindex) = 0;
	virtual int GetUseBlockSize(void) = 0;
	virtual int GetClusterSize(void) = 0;
	virtual int GetAllBlockSize(void) = 0;
	virtual void ReadSector(std::vector<unsigned char>& buffer, int sector, int numOfSector) = 0;
	virtual void WriteSector(std::vector<unsigned char>& buffer, int sector, int numOfSector) = 0;
	virtual int GetBitmapSize(void) = 0;
	virtual int GetBitmap(void) = 0;
	virtual int GetBitmapSerial(int length) = 0;
	virtual void SetBitmap(int start, int length) = 0;
	virtual void DelBitmap(int start, int length) = 0;
	virtual std::string ConvertText(std::string text) = 0;
	virtual std::string ConvertMzText(std::string text) = 0;
	virtual int FindFile(std::string filename, int ignoreIndex) = 0;
	virtual int GetType(void) = 0;;
	virtual void ChangeType(int type) = 0;
protected:
	void ReverseBuffer(std::vector<unsigned char>& buffer);
	bool IsNotAvailableFileCharacter(char character);
	D88Image image;
	int sectorSize;
	size_t betaSize;
	int cylinderCount;
	int hedCount;
	bool isRom;
private:
	static int DiskType(D88Image& image);
};

#endif
