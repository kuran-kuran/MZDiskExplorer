#include <stdlib.h>
#include <stdexcept>
#include <iterator>
#include "Format.hpp"
#include "FileData.hpp"
#include "Mz80SP6110Disk.hpp"

// ������1�o�C�g�ڂ�
#define IsKanji(c) ( (unsigned char)((int)((unsigned char)(c) ^ 0x20) - 0x0A1) < 0x3C )

/* MZ-80K/C,1200,700 */
const char Mz80SP6110Disk::asciiCodeAnk[] =
{
	" !\x22#$%&\x27()*+,-./"	/* 20 */
	"0123456789:;<=>?"		/* 30 */
	"@ABCDEFGHIJKLMNO"		/* 40 */
	"PQRSTUVWXYZ[.].."		/* 50 */
	"................"		/* 60 */
	".............\\.."		/* 70 */
	".����.����������"		/* 80 */
	"����������������"		/* 90 */
	"����������������"		/* A0 */
	"����������������"		/* B0 */
	"................"		/* C0 */
	"................"		/* D0 */
	"................"		/* E0 */
	"................"		/* F0 */
};
const char Mz80SP6110Disk::asciiCodeSjis[] =
{
	"�@�I�h���������f�i�j���{�C�|�D�^"	/* 20 */
	"�O�P�Q�R�S�T�U�V�W�X�F�G�������H"	/* 30 */
	"���`�a�b�c�d�e�f�g�h�i�j�k�l�m�n"	/* 40 */
	"�o�p�q�r�s�t�u�v�w�x�y�m�_�n����"	/* 50 */
	"��������������������������������"	/* 60 */
	"�����ΐ��؋��y���N�����b�~������"	/* 70 */
	"���B�u�v�A�D���@�B�D�F�H�������b"	/* 80 */
	"�[�A�C�E�G�I�J�L�N�P�R�T�V�X�Z�\"	/* 90 */
	"�^�`�c�e�g�i�j�k�l�m�n�q�t�w�z�}"	/* A0 */
	"�~���������������������������J�K"	/* B0 */
	"��������������������������������"	/* C0 */
	"��������������������������������"	/* D0 */
	"��������������������������������"	/* E0 */
	"��������������������������������"	/* F0 */
};

//============================================================================
// �R���X�g���N�^
//============================================================================
Mz80SP6110Disk::Mz80SP6110Disk()
:diskType(0)
,bitmap(NULL)
,fileType(0)
,clusterSize(0)
,dirSector(16)
{
	this->sectorSize = 128;
}

//============================================================================
// �f�X�g���N�^
//============================================================================
Mz80SP6110Disk::~Mz80SP6110Disk()
{
}

int Mz80SP6110Disk::DiskType(void)
{
	return Disk::MZ80K_SP6110;
}

std::string Mz80SP6110Disk::DiskTypeText(void)
{
	return "MZ-80K SP-6110�f�B�X�N";
}

//============================================================================
//  �f�B�X�N�C���[�W���t�H�[�}�b�g����
//----------------------------------------------------------------------------
// In  : path = �C���[�W�t�@�C��
//     : type
//     : DISKTYPE_MZ80_SP6110_2S   // MZ-80K SP-6110
// Out : �G���[�R�[�h ( 0 = ���� )
//============================================================================
void Mz80SP6110Disk::Format(int type, int volumeNumber)
{
	if(type != DISKTYPE_MZ80_SP6110_2S)
	{
		this->diskType = TYPE_2S;
	}
	this->diskType = TYPE_2S;
	// �����t�H�[�}�b�g
	this->image.Format(type, 0);//0xBF ^ 0xFF);
	// �_���t�H�[�}�b�g
	// �f�B���N�g��
	std::vector<unsigned char> buffer;
	buffer.resize(static_cast<size_t>(this->sectorSize) * 48, 0);
	WriteSector(buffer, 16, 48);
	// �r�b�g�}�b�v
	this->bitmap.clear();
	this->bitmap.resize(static_cast<size_t>(this->sectorSize) * 2, 0);
	this->bitmap[0] = static_cast<unsigned char>(volumeNumber);
	this->bitmap[1] = 0x30;
	this->bitmap[2] = 0x30;
	this->bitmap[3] = 0x00;
	this->bitmap[4] = 0x60;
	this->bitmap[5] = 0x04;
	this->bitmap[255] = 0x00;
	WriteSector(this->bitmap, 14, 2);
	// �f�B�X�N���i�[
	this->clusterSize = this->sectorSize * 2;
	ReadDirectory();
}

int Mz80SP6110Disk::Load(std::string path)
{
	dms::FileData fileData;
	fileData.Load(path.c_str());
	std::vector<unsigned char> buffer;
	fileData.GetBuffer(buffer);
	return Load(buffer);
}

//============================================================================
//  �f�B�X�N�C���[�W��ǂݍ���Ń������ɓW�J����
//----------------------------------------------------------------------------
// In  : image = �C���[�W�t�@�C���������C���[�W
// Out : �G���[�R�[�h ( 0 = ���� )
//============================================================================
int Mz80SP6110Disk::Load(const std::vector<unsigned char>& buffer)
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

void Mz80SP6110Disk::Update(void)
{
	D88Image::Header header;
	this->image.GetHeader(header);
	std::vector<unsigned char> buffer;
	this->bitmap.clear();
	ReadSector(this->bitmap, 14, 2);
	// �f�B�X�N���i�[
	int trackMax = 0;
	for(int i = 0; i < D88Image::TRACK_MAX; ++ i)
	{
		if(header.trackTable[i] != 0)
		{
			++ trackMax;
		}
	}
	this->clusterSize = this->sectorSize * 2;
	ReadDirectory();
}

int Mz80SP6110Disk::Save(std::string path)
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
//  �f�B�X�N�C���[�W���쐬����
//----------------------------------------------------------------------------
// In  : buffer = �C���[�W�t�@�C�����i�[����o�b�t�@
// Out : �G���[�R�[�h ( 0 = ���� )
//============================================================================
int Mz80SP6110Disk::Save(std::vector<unsigned char>& buffer)
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
//  �f�B�X�N�C���[�W����t�@�C�������o��
//----------------------------------------------------------------------------
// In  : dirindex = ���o���t�@�C���̃f�B���N�g���ԍ�
//     : path = �ۑ��ʒu
//     : mode = �t�@�C�����[�h
// Out : 0 = ����
//============================================================================
int Mz80SP6110Disk::GetFile(int dirindex, std::string path, unsigned int mode)
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
			// �w�b�_���쐬
			memset(&mzthead, 0, sizeof(MZTHEAD));
			mzthead.mode = this->directory[dirindex].mode;
			memcpy(mzthead.filename, this->directory[dirindex].filename, 17 );
			mzthead.size = this->directory[dirindex].size;
			mzthead.loadAdr = this->directory[dirindex].loadAdr;
			mzthead.runAdr = this->directory[dirindex].runAdr;
			// �w�b�_��񏑂�����
			fwrite(&mzthead, 1, 128, fp);
		}
		// �f�[�^��������
		if(this->directory[dirindex].mode == FILETYPE_BSD)
		{
			int sector;
			int rest;
			sector = this->directory[dirindex].startSector * 2;
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
		else
		{
			std::vector<unsigned char> buffer;
			int sectorCount = (this->directory[dirindex].size + this->sectorSize - 1) / this->sectorSize;
			ReadSector(buffer, this->directory[dirindex].startSector * 2, sectorCount);
			fwrite(&buffer[0], 1, this->directory[dirindex].size, fp);
		}
		// �������ݏI��
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
//  �f�B�X�N�C���[�W�Ƀt�@�C������������
//----------------------------------------------------------------------------
// In  : path = �t�@�C����
//     : dirinfo = �f�B���N�g�����
//     : mode = �t�@�C�����[�h
//     :   FILEMODE_MZT
//     :   FILEMODE_BIN
//     :   FILEMODE_NEWLINE_CONV
//     : type = �t�@�C���^�C�v
//     :   FILETYPE_OBJ = 1
//     :   FILETYPE_BTX = 2
//     :   FILETYPE_BSD = 3
//     :   FILETYPE_BRD = 4
//     :   FILETYPE_BSD_CONV = 5
// Out : 0 = ����I��
//============================================================================
int Mz80SP6110Disk::PutFile(std::string path, void* dirInfo, unsigned int mode, unsigned int type)
{
	try
	{
		DIRECTORY* dirinfo = reinterpret_cast<DIRECTORY*>(dirInfo);
		// �f�B���N�g������
		int select = -1;
		for(int i = 0; i < 64; ++ i)
		{
			// �t�@�C���l�[��
			if(this->directory[i].mode == 0)
			{
				select = i;
				break;
			}
		}
		if(select == -1)
		{
			// �f�B���N�g���ɋ󂫂�����܂���
			return 1;
		}
		// �t�@�C����ǂݍ���
		FILE* fp;
		if(fopen_s(&fp, path.c_str(), "rb") != 0)
		{
			// �t�@�C����ǂݍ��ނ��Ƃ��ł��Ȃ�
			return 2;
		}
		if(fp == NULL)
		{
			// �t�@�C����ǂݍ��ނ��Ƃ��ł��Ȃ�
			return 2;
		}
		// �t�@�C���T�C�Y�擾
		fseek(fp, 0, SEEK_END);
		size_t dataSize = ftell(fp);
		fseek(fp, 0, SEEK_SET);
		MZTHEAD mzthead;
		if(mode == FILEMODE_MZT)
		{
			if(dataSize <= 128)
			{
				// �t�@�C���T�C�Y������Ȃ�
				return 3;
			}
			dataSize -= 128;	// �w�b�_���������f�[�^�T�C�Y
			// �w�b�_���쐬
			memset(&mzthead, 0, sizeof(MZTHEAD));
			fread(&mzthead, 128, 1, fp);
		}
		// �����t�@�C�������������`�F�b�N����
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
				// �t�@�C���l�[��
				if(strncmp(this->directory[i].filename, filename, strlen(filename)) == 0)
				{
					// �����t�@�C���������݂���
					return 5;
				}
			}
		}
		// �t�@�C���f�[�^��ǂݍ���
		size_t readSize = (dataSize + this->sectorSize - 1) / this->sectorSize * this->sectorSize;
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
				// �r�b�g�}�b�v�̋󂫗e�ʂ�����
				return 4;
			}
		}
		else
		{
			if(PutObjFile(dirinfo, mode, mzthead, select, dataSize, bufferTemp, type) == false)
			{
				// �r�b�g�}�b�v�̋󂫗e�ʂ�����
				return 4;
			}
		}
		// �Ǘ�����D88�C���[�W�ɏ�������
		FlushWrite();
		return 0;
	}
	catch(const std::exception& error)
	{
		(void)error;
		return 1;
	}
}

void Mz80SP6110Disk::ConvertBsdFile(DIRECTORY *dirinfo, unsigned int mode, unsigned int& type, size_t readSize, size_t dataSize, std::vector<unsigned char>& bufferTemp)
{
	if(mode != FILEMODE_MZT)
	{
		int back = -1;
		std::vector<unsigned char> convTemp;
		convTemp.resize(dataSize, 0);
		int j = 0;
		for(int i = 0; i < dataSize; ++ i)
		{
			// CRLF�̏ꍇ��LF������
			if((bufferTemp[i] == 0x0A) && (back == 0x0D))
			{
				back = bufferTemp[i];
				continue;
			}
			unsigned char data = bufferTemp[i];
			// LF�̏ꍇ��CR�ɂ���
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

bool Mz80SP6110Disk::PutBsdFile(DIRECTORY *dirinfo, unsigned int mode, MZTHEAD& mzthead, int select, size_t dataSize, std::vector<unsigned char>& bufferTemp)
{
	if(GetBitmapSize() < dataSize)
	{
		return false;
	}
	// �t�@�C�������f�B���N�g���ɓo�^
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
	// �t�@�C�����f�B�X�N�C���[�W�ɓ]��
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

bool Mz80SP6110Disk::PutObjFile(DIRECTORY *dirinfo, unsigned int mode, MZTHEAD& mzthead, int select, size_t dataSize, std::vector<unsigned char>& bufferTemp, unsigned int type)
{
	// �r�b�g�}�b�v����
	int freespace = GetBitmapSerial(static_cast<int>(dataSize));
	if(freespace == -1)
	{
		// �r�b�g�}�b�v�̋󂫗e�ʂ�����
		return false;
	}
	int size = 0;
	// �t�@�C�������f�B���N�g���ɓo�^
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
	// �r�b�g�}�b�v�X�V
	SetBitmap(freespace, datacluster);
	// �t�@�C�����f�B�X�N�C���[�W�ɓ]��
	std::vector<unsigned char> writeBuffer;
	int writeSector = (this->bitmap[1] + freespace) * this->clusterSize / this->sectorSize;
	int writeSectorCount = (size + this->sectorSize - 1) / this->sectorSize;
	writeBuffer.resize(static_cast<size_t>(writeSectorCount) * this->sectorSize, 0);
	std::copy(bufferTemp.begin(), bufferTemp.end(), writeBuffer.begin());
	WriteSector(writeBuffer, writeSector, writeSectorCount);
	return true;
}

//============================================================================
//  �f�B�X�N�C���[�W�̃t�@�C�����폜����
//----------------------------------------------------------------------------
// In  : dirindex = �f�B���N�g���ԍ�
// Out : 0 = ����I��
//============================================================================
int Mz80SP6110Disk::DelFile(int dirindex)
{
	if(this->directory[dirindex].mode == 0)
	{
		return 1;
	}
	if(this->directory[dirindex].mode == FILETYPE_BSD)
	{
		DelBsdFile(dirindex);
	}
	else
	{
		DelObjFile(dirindex);
	}
	this->directory[dirindex].mode = 0;
	// �Ǘ�����D88�C���[�W�ɏ�������
	FlushWrite();
	return 0;
}

// BSD�t�@�C���폜
void Mz80SP6110Disk::DelBsdFile(int dirindex)
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

// BTX or OBJ�t�@�C���폜
void Mz80SP6110Disk::DelObjFile(int dirindex)
{
	int start = this->directory[dirindex].startSector / (this->bitmap[255] + 1) - this->bitmap[1];
	int size = (this->directory[dirindex].size + this->clusterSize - 1) / this->clusterSize;
	DelBitmap(start, size);
}

//============================================================================
//  �f�B�X�N�C���[�W����u�[�g�v���O���������o��
//----------------------------------------------------------------------------
// In  : path = �ۑ��ʒu
//     : mode = �t�@�C�����[�h
// Out : 0 = ����I��
//============================================================================
int Mz80SP6110Disk::GetBoot(std::string path, unsigned int mode)
{
	std::vector<unsigned char> iplBuffer;
	ReadSector(iplBuffer, 0, 1);
	if(iplBuffer[0] != 0xC3)
	{
		// �N���f�B�X�N�ł͂���܂���
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
		// �w�b�_���쐬
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
		// �w�b�_��񏑂�����
		fwrite(&mzthead, 1, 128, fp);
	}
	// �f�[�^��������
	int writesize = this->sectorSize * 14;
	std::vector<unsigned char> writeBuffer;
	int sectorSize = (writesize + this->sectorSize - 1) / this->sectorSize;
	ReadSector(writeBuffer, 0, sectorSize);
	fwrite(&writeBuffer[0], 1, writesize, fp);
	// �������ݏI��
	fclose( fp );
	return 0;
}

//============================================================================
//  �f�B�X�N�C���[�W�Ƀu�[�g�v���O��������������
//----------------------------------------------------------------------------
// In  : path = �t�@�C����
//     : mode = �t�@�C�����[�h
//     :   FILEMODE_MZT
//     :   FILEMODE_BIN
//     : machine = �@��
//     :   01h MZ-80B/2000/2200
//     :   02h MZ-80A
//     :   03h MZ-700/1500
// Out : 0 = ����I��
//============================================================================
int Mz80SP6110Disk::PutBoot(std::string path, void* iplInfo, unsigned int mode, unsigned char machine)
{
	// �t�@�C����ǂݍ���
	FILE *fp;
	if(fopen_s(&fp, path.c_str(), "rb") != 0)
	{
		return 1;
	}
	if(fp == NULL)
	{
		// �t�@�C����ǂݍ��ނ��Ƃ��ł��Ȃ�
		return 1;
	}
	// �t�@�C���T�C�Y�擾
	fseek(fp, 0, SEEK_END);
	int datasize = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	MZTHEAD mzthead;
	if(mode == FILEMODE_MZT)
	{
		if(datasize <= 128)
		{
			// �t�@�C���T�C�Y������Ȃ�
			return 2;
		}
		datasize -= 128;	// �w�b�_���������f�[�^�T�C�Y
							// �w�b�_���쐬
		memset(&mzthead, 0, sizeof(MZTHEAD));
		fread(&mzthead, 128, 1, fp);
	}
	// �t�@�C���f�[�^��ǂݍ���
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
//  �f�B�X�N�C���[�W����V�X�e���v���O���������o��
//----------------------------------------------------------------------------
// In  : path = �ۑ��ʒu
//     : mode = �t�@�C�����[�h
// Out : 0 = ����I��
//============================================================================
int Mz80SP6110Disk::GetSystem(std::string path, unsigned int mode)
{
	std::vector<unsigned char> iplBuffer;
	ReadSector(iplBuffer, 0, 1);
	if(iplBuffer[0] != 0xC3)
	{
		// �N���f�B�X�N�ł͂���܂���
		return 1;
	}
	if(this->directory[0].mode != 0x80)
	{
		// SP-6110�f�B�X�N�ł͂���܂���
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
	int offset = static_cast<int>(this->bitmap[1]) * 2;
	int writeSectorSize = offset - 64;
	int writesize = this->sectorSize * writeSectorSize;
	if((mode & FILEMODE_MASK) == FILEMODE_MZT)
	{
		// �w�b�_���쐬
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
		// �w�b�_��񏑂�����
		fwrite(&mzthead, 1, 128, fp);
	}
	// �f�[�^��������
	std::vector<unsigned char> writeBuffer;
	int sectorSize = (writesize + this->sectorSize - 1) / this->sectorSize;
	ReadSector(writeBuffer, 64, sectorSize);
	fwrite(&writeBuffer[0], 1, writesize, fp);
	// �������ݏI��
	fclose( fp );
	return 0;
}

//============================================================================
//  �f�B�X�N�C���[�W�ɃV�X�e���v���O��������������
//----------------------------------------------------------------------------
// In  : path = �t�@�C����
//     : mode = �t�@�C�����[�h
//     :   FILEMODE_MZT
//     :   FILEMODE_BIN
// Out : 0 = ����I��
//============================================================================
int Mz80SP6110Disk::PutSystem(std::string path, void* iplInfo, unsigned int mode)
{
	// �t�@�C����ǂݍ���
	FILE *fp;
	if(fopen_s(&fp, path.c_str(), "rb") != 0)
	{
		return 1;
	}
	if(fp == NULL)
	{
		// �t�@�C����ǂݍ��ނ��Ƃ��ł��Ȃ�
		return 1;
	}
	// �t�@�C���T�C�Y�擾
	fseek(fp, 0, SEEK_END);
	int datasize = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	MZTHEAD mzthead;
	if(mode == FILEMODE_MZT)
	{
		if(datasize <= 128)
		{
			// �t�@�C���T�C�Y������Ȃ�
			return 2;
		}
		datasize -= 128;	// �w�b�_���������f�[�^�T�C�Y
							// �w�b�_���쐬
		memset(&mzthead, 0, sizeof(MZTHEAD));
		fread(&mzthead, 128, 1, fp);
	}
	// �t�@�C���f�[�^��ǂݍ���
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
//  �r�b�g�}�b�v����󂫗̈����������
//----------------------------------------------------------------------------
// Out : �󂫃T�C�Y
//============================================================================
int Mz80SP6110Disk::GetBitmapSize(void)
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
//  �r�b�g�}�b�v����󂫗̈����������
//----------------------------------------------------------------------------
// Out : �󂫃T�C�Y
//============================================================================
int Mz80SP6110Disk::GetBitmap(void)
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
//  �r�b�g�}�b�v����A���󂫗̈����������
//----------------------------------------------------------------------------
// In  : length = �������ݒ� (Byte)
// Out : �󂫈ʒu�i�[�̈�
//============================================================================
int Mz80SP6110Disk::GetBitmapSerial(int length)
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
//  �r�b�g�}�b�v��ݒ肷��
//----------------------------------------------------------------------------
// In  : start = �ʒu
//     : length = ���� (length �r�b�g)
// Out : �Ȃ�
//============================================================================
void Mz80SP6110Disk::SetBitmap(int start, int length)
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
//  �r�b�g�}�b�v���J������
//----------------------------------------------------------------------------
// In  : start = �ʒu
//     : length = ���� (length �r�b�g)
// Out : �Ȃ�
//============================================================================
void Mz80SP6110Disk::DelBitmap(int start, int length)
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

// MZ-80K�̕�����Windows�Ŏg���镶���ɕϊ�����
std::string Mz80SP6110Disk::ConvertText(std::string text)
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
			if((ascii == ".") && (sjis == "��"))
			{
				result += "_";
			}
			else if((ascii == ".") && (sjis != "��"))
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

// Windows�̕������Ŏg���镶���ɕϊ�����
std::string Mz80SP6110Disk::ConvertMzText(std::string text)
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
				// �S�p��������������
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
				// ���p��������������
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
//  �f�B���N�g���ʒu��ݒ肷��
//----------------------------------------------------------------------------
// In  : �f�B���N�g���̂���Z�N�^�ԍ� (-1=���[�g�f�B���N�g��)
// Out : �Ȃ�
//============================================================================
void Mz80SP6110Disk::SetDirSector( int sector )
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
//  �f�B���N�g���ʒu���擾����
//----------------------------------------------------------------------------
// In  : �Ȃ�
// Out : �f�B���N�g���̂���Z�N�^�ԍ�
//============================================================================
int Mz80SP6110Disk::GetDirSector( void )
{
	return this->dirSector;
}

int Mz80SP6110Disk::GetDirCount(void)
{
	return static_cast<int>(this->directory.size());
}

//============================================================================
//  �f�B���N�g�������擾����
//----------------------------------------------------------------------------
// In  : �f�B���N�g���ԍ� (0�`63);
// Out : �Ȃ�
//============================================================================
void Mz80SP6110Disk::GetDir(void* dirData, int dirindex)
{
	memcpy(dirData, &this->directory[dirindex], sizeof(DIRECTORY));
}

//============================================================================
//  �f�B���N�g��������������
//----------------------------------------------------------------------------
// In  : �f�B���N�g���ԍ� (0�`63);
// Out : �Ȃ�
//============================================================================
void Mz80SP6110Disk::SetDir(void* dirData, int dirindex)
{
	memcpy(&this->directory[dirindex], dirData, sizeof(DIRECTORY));
	// �Ǘ�����D88�C���[�W�ɏ�������
	FlushWrite();
}

//============================================================================
//  �f�B���N�g����\������ (���[�g�̂�)
//----------------------------------------------------------------------------
// In  : �Ȃ�
// Out : �Ȃ�
//============================================================================
#if false
void Mz80SP6110Disk::DisplayDir(void)
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
	// �C���f�b�N�X
	printf("Mode  Filename             Size Load  Run  Sec\n");
	printf("+---+ +-----------------+ +---+ +--+ +--+ +--+\n");
	// �t�@�C�����\��
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
				// ���[�h
				printf("%02d %5s ", i, ModeTbl[mode].modeStr);
				// �t�@�C���l�[��
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
				// �t�@�C���T�C�Y
				printf("%5d ", this->directory[i].size);
				// ���[�h�A�h���X
				printf("%04X ", this->directory[i].loadAdr);
				// ���s�A�h���X
				printf("%04X ", this->directory[i].runAdr);
				// �i�[�Z�N�^
				printf("%04X\n", this->directory[i].startSector);
			}
		}
	}
}
#endif

//============================================================================
//  �p�X������g���q���擾����
//----------------------------------------------------------------------------
// In  : path = �t�@�C��
//     : extfilename = �g���q�i�[�̈�
// Out : �Ȃ�
//============================================================================
void Mz80SP6110Disk::GetExtFilename(std::string path, std::string& extfilename)
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
//	�p�X������g���q���폜����
//----------------------------------------------------------------------------
// In  : path = �p�X��
// Out : �Ȃ�
//============================================================================
void Mz80SP6110Disk::DeleteFileExtname(std::string path)
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

void Mz80SP6110Disk::WriteUseSize(void)
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

// �r�b�g�}�b�v�A�f�B���N�g����D88�ɏ�������
void Mz80SP6110Disk::FlushWrite(void)
{
	std::vector<unsigned char> writeBuffer;
	// �r�b�g�}�b�v��������
	std::copy(this->bitmap.begin(), this->bitmap.end(), std::back_inserter(writeBuffer));
	WriteSector(writeBuffer, 14, 2);
	// �f�B���N�g����������
	WriteDirectory();
}

int Mz80SP6110Disk::GetUseBlockSize(void)
{
	if(this->image.IsValid() == false)
	{
		return 0;
	}
	return this->bitmap[2] + this->bitmap[3] * 256;
}

int Mz80SP6110Disk::GetAllBlockSize(void)
{
	if(this->image.IsValid() == false)
	{
		return 0;
	}
	return this->bitmap[4] + this->bitmap[5] * 256;
}

int Mz80SP6110Disk::GetClusterSize(void)
{
	if(this->image.IsValid() == false)
	{
		return 0;
	}
	return this->clusterSize;
}

// D88�C���[�W����f�B���N�g���f�[�^��this->directry�ɓǂݍ���
void Mz80SP6110Disk::ReadDirectory(void)
{
	if(this->image.IsValid() == false)
	{
		return;
	}
	this->directory.clear();
	std::vector<unsigned char> buffer;
	ReadSector(buffer, this->dirSector, 16);
	size_t directoryCount = this->sectorSize * 16 / 32;
	for(size_t i = 0; i < directoryCount; ++ i)
	{
		DIRECTORY* directory = reinterpret_cast<DIRECTORY*>(&buffer[i * 32]);
		this->directory.push_back(*directory);
	}
}

// �f�B���N�g���f�[�^��this->directry����D88�C���[�W�ɏ�������
void Mz80SP6110Disk::WriteDirectory(void)
{
	std::vector<unsigned char> writeBuffer;
	writeBuffer.clear();
	unsigned char* copyBuffer = reinterpret_cast<unsigned char*>(&this->directory[0]);
	std::copy(copyBuffer, copyBuffer + static_cast<size_t>(this->sectorSize) * 16, std::back_inserter(writeBuffer));
	WriteSector(writeBuffer, this->dirSector, 16);
}

void Mz80SP6110Disk::ReadSector(std::vector<unsigned char>& buffer, int sector, int numOfSector)
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

void Mz80SP6110Disk::WriteSector(std::vector<unsigned char>& buffer, int sector, int numOfSector)
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
