//============================================================================
//  mzdisk.cpp �uMZ �f�B�X�N�C���[�W����v
//----------------------------------------------------------------------------
//													Programmed by ����
//============================================================================
#include "stdafx.h"
#include "mzdisk.h"

#define IsKanji(c) ( (unsigned char)((int)((unsigned char)(c) ^ 0x20) - 0x0A1) < 0x3C )

//============================================================================
// �R���X�g���N�^
//============================================================================
cMzDisk::cMzDisk()
{
	DirSector = 0x10;
	DiskImage = 0;
}

//============================================================================
// �f�X�g���N�^
//============================================================================
cMzDisk::~cMzDisk()
{
	if ( DiskImage != 0 ) {
		free( DiskImage );
	}
}

//============================================================================
//  �f�B�X�N�C���[�W��ǂݍ���Ń������ɓW�J����
//----------------------------------------------------------------------------
// In  : path = �C���[�W�t�@�C��
//     : type = MZDISK_DISKTYPE_MZ2500_2DD
//     :        MZDISK_DISKTYPE_MZ2500_2D40
//     :        MZDISK_DISKTYPE_MZ2500_2D35
//     :        MZDISK_DISKTYPE_MZ2000_2D35
// Out : �G���[�R�[�h ( 0 = ���� )
//============================================================================
void cMzDisk::Format( U32 type )
{
	int i;
	if ( DiskImage != 0 ) {
		free( DiskImage );
		DiskImage = 0;
	}
	if ( MZDISK_DISKTYPE_MZ2500_2DD == type )
	{
		DiskType = MZDISK_TYPE_2DD;
		CylCount = 80;
	}
	else if ( MZDISK_DISKTYPE_MZ2500_2D40 == type )
	{
		DiskType = MZDISK_TYPE_2D;
		CylCount = 40;
	}
	else if ( MZDISK_DISKTYPE_MZ2500_2D35 == type )
	{
		DiskType = MZDISK_TYPE_2D;
		CylCount = 35;
	}
	else if ( MZDISK_DISKTYPE_MZ2000_2D35 == type )
	{
		DiskType = MZDISK_TYPE_2D;
		CylCount = 35;
	}
	else
	{
		return;
	}
	DiskImage = (unsigned char*)malloc( CylCount * 2 * 16 * 256 );
	// �����t�H�[�}�b�g
	memset( DiskImage, 0xBF, CylCount * 2 * 16 * 256 );
	// �_���t�H�[�}�b�g
	if ( MZDISK_DISKTYPE_MZ2000_2D35 != type )
	{
		// MZ-2500 IPL
		ZeroMemory( DiskImage, 15 * 256 );
		DiskImage[ 0 ] = 4;
		// MZ-2500 �f�B���N�g��
		for ( i = 0; i < 64; i ++ )
		{
			ZeroMemory( &DiskImage[ 256 * 16 + i * 32 ], 32 );
			memset( &DiskImage[ 256 * 16 + i * 32 ], 0xD, 17 );
			DiskImage[ 256 * 16 + i * 32 ] = 0;
		}
		DiskImage[ 256 * 16 ] = (char)0x80;
	}
	else
	{
		// MZ-2000 �f�B���N�g��
		for ( i = 0; i < 64; i ++ )
		{
			ZeroMemory( &DiskImage[ 256 * 16 + i * 32 ], 32 );
		}
		DiskImage[ 256 * 16 ] = (char)0x80;
		DiskImage[ 256 * 16 + 1 ] = 1;
	}
	// �r�b�g�}�b�v
	ZeroMemory( &DiskImage[ 256 * 15 ], 256 );
	if ( MZDISK_DISKTYPE_MZ2500_2DD == type )
	{
		DiskImage[ 256 * 15       ] = 0x1;
		DiskImage[ 256 * 15 + 1   ] = 0x18;
		DiskImage[ 256 * 15 + 2   ] = 0x18;
		DiskImage[ 256 * 15 + 3   ] = 0x0;
		DiskImage[ 256 * 15 + 4   ] = 0x0;
		DiskImage[ 256 * 15 + 5   ] = 0x5;
		DiskImage[ 256 * 15 + 255 ] = 0x1;
	}
	else if ( MZDISK_DISKTYPE_MZ2500_2D40 == type )
	{
		DiskImage[ 256 * 15       ] = 0x1;
		DiskImage[ 256 * 15 + 1   ] = 0x30;
		DiskImage[ 256 * 15 + 2   ] = 0x30;
		DiskImage[ 256 * 15 + 3   ] = 0x0;
		DiskImage[ 256 * 15 + 4   ] = 0x0;
		DiskImage[ 256 * 15 + 5   ] = 0x5;
		DiskImage[ 256 * 15 + 255 ] = 0x0;
	}
	else if ( MZDISK_DISKTYPE_MZ2500_2D35 == type )
	{
		DiskImage[ 256 * 15       ] = 0x1;
		DiskImage[ 256 * 15 + 1   ] = 0x30;
		DiskImage[ 256 * 15 + 2   ] = 0x30;
		DiskImage[ 256 * 15 + 3   ] = 0x0;
		DiskImage[ 256 * 15 + 4   ] = 0x60;
		DiskImage[ 256 * 15 + 5   ] = 0x04;
		DiskImage[ 256 * 15 + 255 ] = 0x0;
	}
	else if ( MZDISK_DISKTYPE_MZ2000_2D35 == type )
	{
		DiskImage[ 256 * 15       ] = 0x1;
		DiskImage[ 256 * 15 + 1   ] = 0x30;
		DiskImage[ 256 * 15 + 2   ] = 0x30;
		DiskImage[ 256 * 15 + 3   ] = 0x0;
		DiskImage[ 256 * 15 + 4   ] = 0x60;
		DiskImage[ 256 * 15 + 5   ] = 0x04;
		DiskImage[ 256 * 15 + 255 ] = 0x0;
	}
	// �f�B�X�N���i�[
	Bitmap = (U8*)&DiskImage[ 256 * 0xF ]; // 15 �Z�N�^�̓r�b�g�}�b�v
	ClusterSize = 256 * ( Bitmap[ 255 ] + 1 );
	MaxCluster = Bitmap[ 2 ] + Bitmap[ 3 ] * 256;
	DiskImageSize = ( (U32)Bitmap[ 4 ] + (U32)Bitmap[ 5 ] * 256 ) * 256 * ( (U32)Bitmap[ 255 ] + 1 );
}

//============================================================================
//  �f�B�X�N�C���[�W��ǂݍ���Ń������ɓW�J����
//----------------------------------------------------------------------------
// In  : path = �C���[�W�t�@�C��
//     : type = MZDISK_TYPE_D88
//     :        MZDISK_TYPE_BETA
// Out : �G���[�R�[�h ( 0 = ���� )
//============================================================================
int cMzDisk::Load( char *image, U32 type )
{
	D88HEADER d88header;
	U32 disksize;
	int cylcount;
	int imageindex;
	int i;
	int sectorlength_table[] =
	{
		128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768
	};
	if ( DiskImage != 0 )
	{
		free( DiskImage );
	}
	if ( MZDISK_TYPE_D88 == type )
	{
		char *source;
		// D88 �C���[�W��ǂݍ���
		FileType = MZDISK_TYPE_D88;
		// D88 �w�b�_��ǂݍ���
		memcpy( &d88header, image, sizeof( D88HEADER ) );
		DiskType = d88header.DiskType;
		source = image + sizeof( D88HEADER );
		// �f�B�X�N�T�C�Y���̃��������m�ۂ���
		disksize = 164 * 16 * 256;
		DiskImage = ( unsigned char* )malloc( disksize );
		// �f�[�^�ǂݍ���
		int track;
		int sector;
		int sector_max;
		int track_max;
		D88SECTOR *sectorinfo;
		track_max = 0;
		for( track = 0; track < 164; track ++ )
		{
			if( 0 != d88header.TrackTable[ track ] )
			{
				track_max ++;
			}
		}
		cylcount = track_max / 2;
		for( track = 0; track < 164; track ++ )
		{
			sector_max = 0;
			if (d88header.TrackTable[track] > 0)
			{
				source = image + d88header.TrackTable[track];
				sectorinfo = (D88SECTOR*)source;
				sector_max = (int)sectorinfo->SectorCount;
				source += 16;
			}
			for( sector = 0; sector < sector_max; sector ++ )
			{
				// �Z�N�^���擾
				if( 17 > sectorinfo->R )
				{
					// 1 �` 16 �Z�N�^�f�[�^�擾�A���̑��͖���
					int size;
					imageindex = sectorinfo->C * 256 * 16 * 2 + ( 1 - sectorinfo->H ) * 256 * 16 + ( sectorinfo->R - 1 ) * 256;
					size = sectorlength_table[ sectorinfo->N ];
					if( 256 < size ) 
					{
						size = 256;
					}
					memcpy( &DiskImage[ imageindex ], source, size );
					for ( i = 0; i < size; i ++ )
					{
						DiskImage[ imageindex + i ] ^= 0xFF;
					}
				}
				source += sectorlength_table[ sectorinfo->N ];
				// ���̃Z�N�^�̏��
				sectorinfo = ( D88SECTOR* )source;
				source += 16;
			}
		}
	}
	else
	{
		U8 *bitmap;
		// 2D �C���[�W
		FileType = MZDISK_TYPE_BETA;
		// �f�B�X�N�C���[�W�̃T�C�Y���擾����
		bitmap = ( U8* )&image[ 256 * 0xF ];
		disksize = ( ( U32 )bitmap[ 4 ] + ( U32 )bitmap[ 5 ] * 256 ) * 256 * ( ( U32 )bitmap[ 255 ] + 1 );
		if( disksize <= 286720 )
		{
			DiskType = MZDISK_TYPE_2D;
			cylcount = 35;
		}
		else if( disksize <= 327680 )
		{
			DiskType = MZDISK_TYPE_2D;
			cylcount = 40;
		}
		else if( disksize <= 655360 )
		{
			DiskType = MZDISK_TYPE_2DD;
			cylcount = 80;
		}
		else
		{
			DiskType = MZDISK_TYPE_2HD;
			cylcount = 80;
		}
		DiskImage = (unsigned char*)malloc( disksize );
		// �f�[�^�R�s�[
		memcpy( DiskImage, image, disksize );
	}
	CylCount = cylcount;
	// �f�B�X�N���i�[
	Bitmap = (U8*)&DiskImage[ 256 * 0xF ]; // 15 �Z�N�^�̓r�b�g�}�b�v
	ClusterSize = 256 * ( Bitmap[ 255 ] + 1 );
	MaxCluster = Bitmap[ 2 ] + Bitmap[ 3 ] * 256;
	DiskImageSize = ( ( U32 )Bitmap[ 4 ] + ( U32 )Bitmap[ 5 ] * 256 ) * 256 * ( ( U32 )Bitmap[ 255 ] + 1 );
/*
	// �C���[�W�e�X�g��������
	FILE* fp;
	fp = fopen( "test.img", "wb" );
	if( fp )
	{
		fwrite( DiskImage, 1, DiskImageSize, fp );
		fclose( fp );
	}
*/
	return 0;
}

//============================================================================
//  �f�B�X�N�C���[�W���쐬����
//----------------------------------------------------------------------------
// In  : path = �C���[�W�t�@�C��
//     : type = MZDISK_TYPE_D88
//     :        MZDISK_TYPE_BETA
// Out : �G���[�R�[�h ( 0 = ���� )
//============================================================================
int cMzDisk::Save( char *image, U32 type )
{
	D88HEADER d88header;
	D88SECTOR d88sector;
	int sec;
	int hed;
	int cyl;
	int imageindex;
	int i;
	char writetemp[ 256 ];
	if( MZDISK_TYPE_D88 == type )
	{
		// D88 �t�@�C�����쐬����
		char *dest;
		dest = image;
		// �w�b�_���쐬
		memset( &d88header, 0, sizeof( D88HEADER ) );
		d88header.DiskType = DiskType;
		d88header.DiskSize = sizeof( D88HEADER ) + ( 256 + 16 ) * 16 * 2 * CylCount;
		for( i = 0; i < ( CylCount * 2 ); i ++ )
		{
			d88header.TrackTable[ i ] = ( U32 )( sizeof( D88HEADER ) + ( 256 + 16 ) * 16 * ( U32 )i );
		}
		memcpy( dest, &d88header, sizeof( D88HEADER ) );
		dest += sizeof( D88HEADER );
		// �Z�N�^���쐬
		for( cyl = 0; cyl < CylCount; cyl ++ )
		{
			for( hed = 0; hed < 2; hed ++ )
			{
				for( sec = 0; sec < 16; sec ++ )
				{
					imageindex = cyl * 256 * 16 * 2 + ( 1 - hed ) * 256 * 16 + sec * 256;
					memcpy( writetemp, &DiskImage[ imageindex ], 256 );
					for( i = 0; i < 256; i ++ )
					{
						writetemp[ i ] ^= 0xFF;
					}
					for( i = 0; i < 16; i ++ )
					{
						memset( &d88sector, 0, sizeof( D88SECTOR ) );
						d88sector.C = cyl;
						d88sector.H = hed;
						d88sector.R = sec + 1;
						d88sector.N = 1;
						d88sector.SectorCount = 16;
						d88sector.Density = 0;
						d88sector.DeleteData = 0;
						d88sector.Status = 0;
						d88sector.SectorSize = 256;
					}
					memcpy( dest, &d88sector, sizeof( D88SECTOR ) );
					dest += sizeof( D88SECTOR );
					memcpy( dest, &writetemp, 256 );
					dest += 256;
				}
			}
		}
	}
	else
	{
		memcpy( image, DiskImage, DiskImageSize );
	}
	return 0;
}

//============================================================================
//  �f�B�X�N�C���[�W�̃T�C�Y���擾����
//----------------------------------------------------------------------------
// In  : type = MZDISK_TYPE_D88
//     :        MZDISK_TYPE_BETA
// Out : �f�B�X�N�C���[�W�̃T�C�Y
//============================================================================
int cMzDisk::GetSize( U32 type )
{
	int imagesize = 0;
	if ( MZDISK_TYPE_D88 == type ) {
		imagesize = sizeof( D88HEADER ) + CylCount * 2 * 16 * ( 256 + 16 );
	} else {
		imagesize = DiskImageSize;
	}
	return imagesize;
}

//============================================================================
//  �f�B�X�N�C���[�W����t�@�C�������o��
//----------------------------------------------------------------------------
// In  : dirindex = ���o���t�@�C���̃f�B���N�g���ԍ�
//     : path = �ۑ��ʒu
//     : mode = �t�@�C�����[�h
// Out : 0 = ����
//============================================================================
int cMzDisk::GetFile( int dirindex, char *path, U32 mode )
{
	DIRECTORY *dir;
	FILE *fp;
	MZTHEAD mzthead;
	dir = (DIRECTORY*)&DiskImage[ 256 * DirSector ];
	fp = fopen( path, "wb" );
	if ( NULL == fp ) {
		return 1;
	}
	if ( MZDISK_FILEMODE_MZT == ( mode & MZDISK_FILEMODE_MASK ) )
	{
		// �w�b�_���쐬
		memset( &mzthead, 0, sizeof( MZTHEAD ) );
		mzthead.Mode = dir[ dirindex ].Mode;
		memcpy( mzthead.Filename, dir[ dirindex ].Filename, 17 );
		mzthead.Size = dir[ dirindex ].Size;
		mzthead.LoadAdr = dir[ dirindex ].LoadAdr;
		mzthead.RunAdr = dir[ dirindex ].RunAdr;
		// �w�b�_��񏑂�����
		fwrite( &mzthead, 1, 128, fp );
	}
	// �f�[�^��������
	if ( MZDISK_FILETYPE_BSD == dir[ dirindex ].Mode )
	{
		int sector;
		int rest;
		sector = dir[ dirindex ].StartSector;
		rest = dir[ dirindex ].Size;
		while( sector != 0 )
		{
			if( rest > 254 )
			{
				fwrite( &DiskImage[ sector * 256 ], 1, 254, fp );
			}
			else
			{
				fwrite( &DiskImage[ sector * 256 ], 1, rest, fp );
				break;
			}
			sector = ( unsigned int )DiskImage[ sector * 256 + 254 ] + ( unsigned int )DiskImage[ sector * 256 + 255 ] * 256;
			rest -= 254;
		}
	}
	else if ( MZDISK_FILETYPE_BRD == dir[ dirindex ].Mode )
	{
		int sector;
		int rest;
		unsigned short* sector_list;
		int sector_index = 0;
		sector = dir[ dirindex ].StartSector;
		rest = dir[ dirindex ].Size * 32;
		sector_list = ( unsigned short* )&DiskImage[ sector * 256 ];	
		sector = sector_list[ sector_index ];
		while( sector != 0 )
		{
			if( rest > 4096 )
			{
				fwrite( &DiskImage[ sector * 256 ], 1, 4096, fp );
			}
			else
			{
				fwrite( &DiskImage[ sector * 256 ], 1, rest, fp );
				break;
			}
			sector_index ++;
			sector = sector_list[ sector_index ];
			rest -= 4096;
		}
	}
	else
	{
		fwrite( &DiskImage[ dir[ dirindex ].StartSector * 256 ], 1, dir[ dirindex ].Size, fp );
	}
	// �������ݏI��
	fclose( fp );
	return 0;
}

//============================================================================
//  �f�B�X�N�C���[�W�Ƀt�@�C������������
//----------------------------------------------------------------------------
// In  : path = �t�@�C����
//     : dirinfo = �f�B���N�g�����
//     : mode = �t�@�C�����[�h
//     :   MZDISK_FILEMODE_MZT
//     :   MZDISK_FILEMODE_BIN
//     :   MZDISK_FILEMODE_NEWLINE_CONV
//     : type = �t�@�C���^�C�v
//     :   MZDISK_FILETYPE_OBJ = 1
//     :   MZDISK_FILETYPE_BTX = 2
//     :   MZDISK_FILETYPE_BSD = 3
//     :   MZDISK_FILETYPE_BRD = 4
//     :   MZDISK_FILETYPE_BSD_CONV = 5
// Out : 0 = ����I��
//============================================================================
int cMzDisk::PutFile( char *path, DIRECTORY *dirinfo, U32 mode, U32 type )
{
	DIRECTORY *dir;
	int i;
	int j;
	FILE *fp;
	MZTHEAD mzthead;
	unsigned char *buffertemp;
	unsigned char *writetemp;
	int result;
	int datasize;
	int readsize;
	int freespace;
	int temp;
	int datacluster;
	int filesize = 0;
	int select = -1;
	// �f�B���N�g������
	dir = (DIRECTORY*)&DiskImage[ 256 * DirSector ];
	for( i = 0; i < 64; i++ )
	{
		// �t�@�C���l�[��
		if( 0 == dir[ i ].Mode )
		{
			select = i;
			break;
		}
	}
	if( -1 == select )
	{
		// �f�B���N�g���ɋ󂫂�����܂���
		return 1;
	}
	// �t�@�C����ǂݍ���
	fp = fopen( path, "rb" );
	if( NULL == fp )
	{
		// MZT �t�@�C����ǂݍ��ނ��Ƃ��ł��Ȃ�
		return 2;
	}
	// �t�@�C���T�C�Y�擾
	fseek( fp, 0, SEEK_END );
	datasize = ftell( fp );
	fseek( fp, 0, SEEK_SET );
	if( MZDISK_FILEMODE_MZT == mode )
	{
		if( datasize <= 128 )
		{
			// �t�@�C���T�C�Y������Ȃ�
			return 3;
		}
		datasize -= 128;	// �w�b�_���������f�[�^�T�C�Y
		// �w�b�_���쐬
		memset( &mzthead, 0, sizeof( MZTHEAD ) );
		result = fread( &mzthead, 128, 1, fp );
	}
	// �����t�@�C�������������`�F�b�N����
	char filename[ 18 ];
	if( ( MZDISK_FILEMODE_MZT == mode ) && ( 0 == dirinfo ) )
	{
		memcpy( filename, mzthead.Filename, 17 );
	}
	else
	{
		memcpy( filename, dirinfo->Filename, 17 );
	}
	for( i = 0; i < 17; i ++ )
	{
		if( 0xD == filename[ i ] )
		{
			filename[ i ] = '\0';
		}
	}
	for( i = 0; i < 64; i++ )
	{
		if( 0 != dir[ i ].Mode )
		{
			// �t�@�C���l�[��
			if( 0 == strncmp( dir[ i ].Filename, filename, strlen( filename ) ) )
			{
				// �����t�@�C���������݂���
				return 5;
			}
		}
	}
	// �t�@�C���f�[�^��ǂݍ���
	readsize = ( datasize + 255 ) / 256 * 256;
	buffertemp = (unsigned char*)malloc( readsize );
	memset( buffertemp, 0, readsize );
	fread( buffertemp, 1, datasize, fp );
	if( MZDISK_FILETYPE_BSD_CONV == type )
	{
		if( mode != MZDISK_FILEMODE_MZT )
		{
			char* conv_temp;
			int back = -1;
			conv_temp = (char*)malloc( readsize );
			j = 0;
			for( i = 0; i < datasize; i ++ )
			{
				if( 0x0A == buffertemp[ i ] )
				{
					if( !IsKanji( back ) )
					{
						back = buffertemp[ i ];
						continue;
					}
				}
				conv_temp[ j ] = buffertemp[ i ];
				j ++;
				back = buffertemp[ i ];
			}
			datasize = j;
			memcpy( buffertemp, conv_temp, datasize );
			free( conv_temp );
			if( dirinfo != 0 )
			{
				dirinfo->Size = datasize;
			}
		}
		type = MZDISK_FILETYPE_BSD;
	}
	fclose( fp );
	if ( MZDISK_FILETYPE_BSD == type )
	{
		if( GetBitmapSize() < datasize )
		{
			// �r�b�g�}�b�v�̋󂫗e�ʂ�����
			free( buffertemp );
			return 4;
		}
		// �t�@�C�������f�B���N�g���ɓo�^
		int rest;
		if( ( MZDISK_FILEMODE_MZT == mode ) && ( 0 == dirinfo ) )
		{
			memset( &dir[ select ], 0, 32 );
			dir[ select ].Mode = mzthead.Mode;
			memcpy( dir[ select ].Filename, mzthead.Filename, 17 );
			dir[ select ].Attr = 0;
			dir[ select ].Size = mzthead.Size;
			dir[ select ].LoadAdr = mzthead.LoadAdr;
			dir[ select ].RunAdr = mzthead.RunAdr;
			dir[ select ].Date = 0;
			rest = mzthead.Size;
		}
		else
		{
			memcpy( &dir[ select ], dirinfo, 32 );
			rest = static_cast<int>(dirinfo->Size) * 32;
		}
		temp = Bitmap[ 2 ] + Bitmap[ 3 ] * 256;
		datacluster = ( datasize / ClusterSize );
		if( ( datasize % ClusterSize ) != 0 ) {
			datacluster++;
		}
		Bitmap[ 2 ] = ( temp + datacluster ) & 255;
		Bitmap[ 3 ] = ( ( temp + datacluster ) / 256 ) & 255;
		// �t�@�C�����f�B�X�N�C���[�W�ɓ]��
		unsigned char *source;
		int cluster;
		int before = -1;
		cluster = GetBitmap();
		dir[ select ].StartSector = ( Bitmap[ 1 ] + cluster ) * ClusterSize / 256;
		source = buffertemp;
		while( 0 < rest )
		{
			int sector;
			cluster = GetBitmap();
			SetBitmap( cluster, 1 );
			sector = ( ( Bitmap[ 1 ] + cluster ) * ClusterSize ) / 256;
			writetemp = &DiskImage[ sector * 256 ];
			for( i = 0; i < ( ClusterSize / 256 ); i ++ )
			{
				int write_size;
				write_size = rest;
				if( rest > 254 )
				{
					write_size = 254;
				}
				memset( writetemp, 0, 256 );
				memcpy( writetemp, source, write_size );
				if( before != -1 )
				{
					DiskImage[ before * 256 + 254 ] = sector % 256;
					DiskImage[ before * 256 + 255 ] = sector / 256;
				}
				rest -= write_size;
				if( rest <= 0 )
				{
					break;
				}
				writetemp += 256;
				source += 254;
				before = sector;
				sector ++;
			}
		}
	}
	else if ( MZDISK_FILETYPE_BRD == type )
	{
		if( GetBitmapSize() < datasize )
		{
			// �r�b�g�}�b�v�̋󂫗e�ʂ�����
			free( buffertemp );
			return 4;
		}
		// �t�@�C�������f�B���N�g���ɓo�^
		int rest;
		if( ( MZDISK_FILEMODE_MZT == mode ) && ( 0 == dirinfo ) )
		{
			memset( &dir[ select ], 0, 32 );
			dir[ select ].Mode = mzthead.Mode;
			memcpy( dir[ select ].Filename, mzthead.Filename, 17 );
			dir[ select ].Attr = 0;
			dir[ select ].Size = ( ( mzthead.Size + 4095 ) / 4096 * 4096 ) / 32;
			dir[ select ].LoadAdr = mzthead.LoadAdr;
			dir[ select ].RunAdr = mzthead.RunAdr;
			dir[ select ].Date = 0;
			rest = mzthead.Size;
		}
		else
		{
			memcpy( &dir[ select ], dirinfo, 32 );
			rest = dirinfo->Size * 32;
		}
		temp = Bitmap[ 2 ] + Bitmap[ 3 ] * 256;
		datacluster = ( datasize / ClusterSize );
		if( ( datasize % ClusterSize ) != 0 ) {
			datacluster++;
		}
		Bitmap[ 2 ] = ( temp + datacluster ) & 255;
		Bitmap[ 3 ] = ( ( temp + datacluster ) / 256 ) & 255;
		// �t�@�C�����f�B�X�N�C���[�W�ɓ]��
		unsigned char* source;
		int cluster;
		int before = -1;
		int index = 0;
		unsigned short* SectorList;

		cluster = GetBitmapSerial( 256 );
		dir[ select ].StartSector = ( Bitmap[ 1 ] + cluster ) * ClusterSize / 256;
		SetBitmap( cluster, 256 );
		SectorList = ( unsigned short* )&DiskImage[ dir[ select ].StartSector * 256 ];
		memset( SectorList, 0, ClusterSize );

		source = buffertemp;
		while( rest > 0 )
		{
			int sector;
			int loop;
			cluster = GetBitmapSerial( 4096 );
			if( ClusterSize <= 4096 )
			{
				SetBitmap( cluster, 4096 );
				loop = 1;
			}
			else
			{
				SetBitmap( cluster, 256 );
				loop = ClusterSize / 4096;
			}
			sector = ( ( Bitmap[ 1 ] + cluster ) * ClusterSize ) / 256;
			writetemp = &DiskImage[ sector * 256 ];
			for( i = 0; i < loop; i ++ )
			{
				int write_size;
				write_size = rest;
				if( rest > 4096 )
				{
					write_size = 4096;
				}
				memset( writetemp, 0, 4096 );
				memcpy( writetemp, source, write_size );

				SectorList[ index ] = sector;
				index ++;

				rest -= write_size;
				if( rest <= 0 )
				{
					break;
				}
				writetemp += 4096;
				source += 4096;
				sector += ( 4096 / 256 );
			}
		}
	}
	else
	{
		// �r�b�g�}�b�v����
		freespace = GetBitmapSerial( datasize );
		if( -1 == freespace )
		{
			// �r�b�g�}�b�v�̋󂫗e�ʂ�����
			free( buffertemp );
			return 4;
		}
		// �t�@�C�������f�B���N�g���ɓo�^
		if( ( MZDISK_FILEMODE_MZT == mode ) && ( 0 == dirinfo ) )
		{
			memset( &dir[ select ], 0, 32 );
			dir[ select ].Mode = mzthead.Mode;
			memcpy( dir[ select ].Filename, mzthead.Filename, 17 );
			dir[ select ].Attr = 0;
			dir[ select ].Size = mzthead.Size;
			dir[ select ].LoadAdr = mzthead.LoadAdr;
			dir[ select ].RunAdr = mzthead.RunAdr;
			dir[ select ].Date = 0;
		}
		else
		{
			memcpy( &dir[ select ], dirinfo, 32 );
		}
		temp = Bitmap[ 2 ] + Bitmap[ 3 ] * 256;
		datacluster = ( datasize / ClusterSize );
		if( ( datasize % ClusterSize ) != 0 ) {
			datacluster++;
		}
		Bitmap[ 2 ] = ( temp + datacluster ) & 255;
		Bitmap[ 3 ] = ( ( temp + datacluster ) / 256 ) & 255;
		dir[ select ].StartSector = ( Bitmap[ 1 ] + freespace ) * ( Bitmap[ 255 ] + 1 );
		// �r�b�g�}�b�v�X�V
		SetBitmap( freespace, datasize );
		// �t�@�C�����f�B�X�N�C���[�W�ɓ]��
		if( MZDISK_FILETYPE_OBJ == type )
		{
			writetemp = &DiskImage[ ( Bitmap[ 1 ] + freespace ) * ClusterSize ];
			memcpy( writetemp, buffertemp, dirinfo->Size );
		}
		else if( MZDISK_FILETYPE_BTX == type )
		{
			writetemp = &DiskImage[ ( Bitmap[ 1 ] + freespace ) * ClusterSize ];
			memcpy( writetemp, buffertemp, dirinfo->Size );
		}
	}
	// �������J��
	free( buffertemp );
	return 0;
}

//============================================================================
//  �f�B�X�N�C���[�W�̃t�@�C�����폜����
//----------------------------------------------------------------------------
// In  : dirindex = �f�B���N�g���ԍ�
// Out : 0 = ����I��
//============================================================================
int cMzDisk::DelFile( int dirindex )
{
	DIRECTORY *dir;
	dir = (DIRECTORY*)&DiskImage[ 256 * DirSector ];
	if ( 0 == dir[ dirindex ].Mode )
	{
		return 1;
	}
	int start;
	int size;
	int temp;
	start = dir[ dirindex ].StartSector / ( Bitmap[ 255 ] + 1 ) - Bitmap[ 1 ];
	size = ( dir[ dirindex ].Size + ClusterSize - 1 ) / ClusterSize;
	DelBitmap( start, size );
	dir[ dirindex ].Mode = 0;
	temp = Bitmap[ 2 ] + Bitmap[ 3 ] * 256;
	Bitmap[ 2 ] = ( temp - size ) & 255;
	Bitmap[ 3 ] = ( ( temp - size ) / 256 ) & 255;
	return 0;
}

//============================================================================
//  �f�B�X�N�C���[�W����u�[�g�v���O���������o��
//----------------------------------------------------------------------------
// In  : path = �ۑ��ʒu
//     : mode = �t�@�C�����[�h
// Out : 0 = ����I��
//============================================================================
int cMzDisk::GetBoot( char *path, U32 mode )
{
	IPL *ipl;
	FILE *fp;
	MZTHEAD mzthead;
	int writesize;
	int i;
	ipl = (IPL*)DiskImage;
	if ( strncmp( ipl->Signature, "IPLPRO", 6 ) != 0 ) {
		// �N���f�B�X�N�ł͂���܂���
		return 1;
	}
	fp = fopen( path, "wb" );
	if ( NULL == fp ) {
		return 1;
	}
	if ( MZDISK_FILEMODE_MZT == ( mode & MZDISK_FILEMODE_MASK ) )
	{
		// �w�b�_���쐬
		memset( &mzthead, 0, sizeof( MZTHEAD ) );
		mzthead.Mode = 1;
		memcpy( mzthead.Filename, ipl->Bootname, 11 );
		for ( i=0; i<17; i++ ) {
			if ( mzthead.Filename[ i ] == 0 ) {
				mzthead.Filename[ i ] = '\xD';
				break;
			}
		}
		mzthead.Size = ipl->Size;
		// �w�b�_��񏑂�����
		fwrite( &mzthead, 1, 128, fp );
	}
	// �f�[�^��������
	writesize = ipl->Size;
	if ( writesize == 0 ) {
		writesize = 65536;
	}
	fwrite( &DiskImage[ ipl->StartSector * 256 ], 1, writesize, fp );
	// �������ݏI��
	fclose( fp );
	return 0;
}

//============================================================================
//  �f�B�X�N�C���[�W�Ƀu�[�g�v���O��������������
//----------------------------------------------------------------------------
// In  : path = �t�@�C����
//     : iplinfo = �f�B���N�g�����
//     : mode = �t�@�C�����[�h
//     :   MZDISK_FILEMODE_MZT
//     :   MZDISK_FILEMODE_BIN
// Out : 0 = ����I��
//============================================================================
int cMzDisk::PutBoot( char *path, IPL *iplinfo, U32 mode )
{
	IPL *ipl;
	FILE *fp;
	MZTHEAD mzthead;
	unsigned char *buffertemp;
	unsigned char *writetemp;
	int result;
	int datasize;
	int readsize;
	int freespace;
	int temp;
	int datacluster;
	int filesize = 0;
	int select = -1;
	ipl = (IPL*)DiskImage;
	// �t�@�C����ǂݍ���
	fp = fopen( path, "rb" );
	if ( NULL == fp ) {
		// �t�@�C����ǂݍ��ނ��Ƃ��ł��Ȃ�
		return 1;
	}
	// �t�@�C���T�C�Y�擾
	fseek( fp, 0, SEEK_END );
	datasize = ftell( fp );
	fseek( fp, 0, SEEK_SET );
	if ( MZDISK_FILEMODE_MZT == mode ) {
		if ( datasize <= 128 ) {
			// �t�@�C���T�C�Y������Ȃ�
			return 2;
		}
		datasize -= 128;	// �w�b�_���������f�[�^�T�C�Y
		// �w�b�_���쐬
		memset( &mzthead, 0, sizeof( MZTHEAD ) );
		result = fread( &mzthead, 128, 1, fp );
	}
	// �t�@�C���f�[�^��ǂݍ���
	readsize = ( datasize + 255 ) / 256 * 256;
	buffertemp = (unsigned char*)malloc( readsize );
	memset( buffertemp, 0, readsize );
	if ( datasize < 65536 )
	{
		fread( buffertemp, 1, datasize, fp );
	}
	else
	{
		int rest = datasize;
		while( 1 )
		{
			fread( buffertemp, 32768, 1, fp );
			rest -= 32768;
			if( rest < 32768 )
			{
				if( rest != 0 )
				{
					fread( buffertemp, rest, 1, fp );
				}
				break;
			}
		}
	}
	// �r�b�g�}�b�v����
	freespace = GetBitmapSerial( datasize );
	if( -1 == freespace )
	{
		// �r�b�g�}�b�v�̋󂫗e�ʂ�����
		free( buffertemp );
		return 4;
	}
	// �t�@�C�������f�B���N�g���ɓo�^
	if( ( MZDISK_FILEMODE_MZT == mode ) && ( 0 == iplinfo ) )
	{
		memset( ipl, 0, 32 );
		ipl->Machine = 1;
		memcpy( ipl->Bootname, mzthead.Filename, 10 );
		ipl->Bootname[ 10 ] = '\xD';
		ipl->Size = mzthead.Size;
		ipl->RunAdr = mzthead.RunAdr;
	}
	else
	{
		memcpy( ipl, iplinfo, 32 );
		ipl->Size = datasize;
	}
	ipl->Signature[ 0 ] = 'I';
	ipl->Signature[ 1 ] = 'P';
	ipl->Signature[ 2 ] = 'L';
	ipl->Signature[ 3 ] = 'P';
	ipl->Signature[ 4 ] = 'R';
	ipl->Signature[ 5 ] = 'O';
	temp = Bitmap[ 2 ] + Bitmap[ 3 ] * 256;
	datacluster = ( datasize / ClusterSize );
	if ( ( datasize % ClusterSize ) != 0 )
	{
		datacluster++;
	}
	Bitmap[ 2 ] = ( temp + datacluster ) & 255;
	Bitmap[ 3 ] = ( ( temp + datacluster ) / 256 ) & 255;
	ipl->StartSector = ( Bitmap[ 1 ] + freespace ) * ( Bitmap[ 255 ] + 1 );
	// �r�b�g�}�b�v�X�V
	SetBitmap( freespace, datasize );
	// �t�@�C�����f�B�X�N�C���[�W�ɓ]��
	writetemp = &DiskImage[ ( Bitmap[ 1 ] + freespace ) * ClusterSize ];
	memcpy( writetemp, buffertemp, ipl->Size );
	// �������J��
	free( buffertemp );
	return 0;
}

//============================================================================
//  �r�b�g�}�b�v����󂫗̈����������
//----------------------------------------------------------------------------
// Out : �󂫃T�C�Y
//============================================================================
int cMzDisk::GetBitmapSize( void )
{
	int freesize = 0;
	U8 bit;
	int end;
	int i;
	end = Bitmap[ 4 ] + Bitmap[ 5 ] * 256;
	for( i = 48; i < end; i ++ )
	{
		bit = ( Bitmap[ i / 8 ] >> ( i % 8 ) ) & 1;
		if( bit == 0 )
		{
			freesize ++;
		}
	}
	return freesize * ClusterSize;
}

//============================================================================
//  �r�b�g�}�b�v����󂫗̈����������
//----------------------------------------------------------------------------
// Out : �󂫃T�C�Y
//============================================================================
int cMzDisk::GetBitmap( void )
{
	U8 bit;
	int end;
	int i;
	end = Bitmap[ 4 ] + Bitmap[ 5 ] * 256;
	for( i = 48; i < end; i ++ )
	{
		bit = ( Bitmap[ i / 8 ] >> ( i % 8 ) ) & 1;
		if( bit == 0 )
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
int cMzDisk::GetBitmapSerial( int length )
{
	int lengthtemp = 0;
	U8 bit;
	int index = -1;
	int end;
	int i;
	int length_cluster;
	length_cluster = ( length + ClusterSize - 1 ) / ClusterSize;
	end = Bitmap[ 4 ] + Bitmap[ 5 ] * 256;
	for( i = 48; i < end; i ++ )
	{
		bit = ( Bitmap[ i / 8 ] >> ( i % 8 ) ) & 1;
		if( bit == 0 )
		{
			if( index == -1 )
			{
				index = i;
			}
			lengthtemp ++;
			if( lengthtemp >= length_cluster )
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
void cMzDisk::SetBitmap( int start, int length )
{
	int loop;
	int i;
	BYTE *data;
	loop = length / ClusterSize;
	if( ( length % ClusterSize ) != 0 )
	{
		loop += 1;
	}
	start += 48;
	for( i = start; i < ( start + loop ); i ++ )
	{
		data = &Bitmap[ i / 8 ];
		*data |= ( 1 << ( i % 8 ) );
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
void cMzDisk::DelBitmap( int start, int length )
{
	int loop;
	int i;
	BYTE *data;
	loop = length / ClusterSize;
	if( ( length % ClusterSize ) != 0 )
	{
		loop += 1;
	}
	start += 48;
	for( i = start; i < ( start + loop ); i ++ )
	{
		data = &Bitmap[ i / 8 ];
		*data &= ( ~( 1 << ( i % 8 ) ) );
	}
	WriteUseSize();
}

//============================================================================
//  �f�B���N�g���ʒu��ݒ肷��
//----------------------------------------------------------------------------
// In  : �f�B���N�g���̂���Z�N�^�ԍ� (-1=���[�g�f�B���N�g��)
// Out : �Ȃ�
//============================================================================
void cMzDisk::SetDirSector( int sector )
{
	if( -1 == sector )
	{
		DirSector = 0x10;
	}
	else
	{
		DirSector = sector;
	}
}

//============================================================================
//  �f�B���N�g���ʒu���擾����
//----------------------------------------------------------------------------
// In  : �Ȃ�
// Out : �f�B���N�g���̂���Z�N�^�ԍ�
//============================================================================
int cMzDisk::GetDirSector( void )
{
	return DirSector;
}

//============================================================================
//  �f�B���N�g�������擾����
//----------------------------------------------------------------------------
// In  : �f�B���N�g���ԍ� (0�`63);
// Out : �Ȃ�
//============================================================================
void cMzDisk::GetDir( DIRECTORY *dirdata, int dirindex )
{
	DIRECTORY *dir;
	dir = (DIRECTORY*)&DiskImage[ 256 * DirSector ];
	memcpy( dirdata, &dir[ dirindex ], sizeof( DIRECTORY ) );
}

//============================================================================
//  �Z�N�^�f�[�^�擾
//----------------------------------------------------------------------------
// In  : buffer = �Z�N�^�f�[�^�i�[�̈� (256 �o�C�g)
// Out : �Ȃ�
//============================================================================
void cMzDisk::GetSector( char *buffer, int sector )
{
	memcpy( buffer, &DiskImage[ 256 * sector ], 256 );
}

//============================================================================
//  �f�B���N�g����\������ (���[�g�̂�)
//----------------------------------------------------------------------------
// In  : pcDiskImage = �f�B�X�N�C���[�W�i�[�̈�
// Out : �Ȃ�
//============================================================================
/*
void DisplayDir( char *pcDiskImage )
{
	DIRECTORY *dir;
	char pcFileName[ 18 ];
	int nMode;
	int i;
	int j;
	struct {
		BYTE byMode;
		char pcModeStr[ 6 ];
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
	dir = (DIRECTORY*)&DiskImage[ 256 * DirSector ];
	// �C���f�b�N�X
	printf( "Mode  Filename             Size Load  Run  Sec\n" );
	printf( "+---+ +-----------------+ +---+ +--+ +--+ +--+\n" );
	// �t�@�C�����\��
	for ( i=0; i<64; i++ ) {
		if ( dir[ i ].byMode != 0 ) {
			nMode = -1;
			for ( j=0; j<10; j++ ) {
				if ( dir[ i ].byMode == ModeTbl[ j ].byMode ) {
					nMode = j;
					break;
				}
			}
			if ( ( nMode >= 0 ) && ( nMode < 9 ) ) {
				// ���[�h
				printf( "%5s ", ModeTbl[ j ].pcModeStr );
				// �t�@�C���l�[��
				memset( &pcFileName, 0, 18 );
				memcpy( &pcFileName, dir[ i ].pcFilename, 17 );
				for ( j=0; j<17; j++ ) {
					if ( pcFileName[ j ] == 0x0D ) {
						pcFileName[ j ] = 0;
					}
				}
				printf( "\"%s\" ", pcFileName );
				for ( j=0; j<( 17 - (int)strlen( pcFileName ) ); j++ ) {
					printf( " " );
				}
				// �t�@�C���T�C�Y
				printf( "%5d ", dir[ i ].wSize );
				// ���[�h�A�h���X
				printf( "%04X ", dir[ i ].wLoadAdr );
				// ���s�A�h���X
				printf( "%04X ", dir[ i ].wRunAdr );
				// �i�[�Z�N�^
				printf( "%04X\n", dir[ i ].wStartSector );
			}
		}
	}
}
*/

//============================================================================
//  �p�X������g���q���擾����
//----------------------------------------------------------------------------
// In  : path = �t�@�C��
//     : extfilename = �g���q�i�[�̈� ( char[ 4 ] )
// Out : �Ȃ�
//============================================================================
void cMzDisk::GetExtFilename( char *path, char *extfilename )
{
	int i;
	int length;
	int loop = 4;
	int extpos;
	memset( extfilename, 0, 4 );
	length = strlen( path );
	if( length > 2 )
	{
		if( loop > length )
		{
			loop = length;
		}
		extpos = -1;
		for( i=0; i<loop; i++ ) 
		{
			if( path[ length - i - 1 ] == '.' )
			{
				extpos = length - i - 1 + 1;
				break;
			}
		}
		if( extpos != -1 )
		{
			for( i=0; i<3; i++ )
			{
				if( path[ extpos ] == '\0' )
				{
					break;
				}
				extfilename[ i ] = path[ extpos ];
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
void cMzDisk::DeleteFileExtname( char *path )
{
	int i;
	int length;
	int loop = 4;
	length = strlen( path );
	if( length > 2 )
	{
		if( loop > length )
		{
			loop = length;
		}
		for( i=0; i<loop; i++ )
		{
			if( path[ length - i - 1 ] == '.' )
			{
				path[ length - i - 1 ] = '\0';
				break;
			}
		}
	}
}

void cMzDisk::WriteUseSize( void )
{
	int size = 0;
	U8 bit;
	int end;
	int i;
	end = Bitmap[ 4 ] + Bitmap[ 5 ] * 256;
	for( i = 48; i < end; i ++ )
	{
		bit = ( Bitmap[ i / 8 ] >> ( i % 8 ) ) & 1;
		if( bit == 1 )
		{
			size ++;
		}
	}
	Bitmap[ 2 ] = size % 256;
	Bitmap[ 3 ] = size / 256;
}

int cMzDisk::GetUseBlockSize( void )
{
	if( 0 == DiskImage )
	{
		return 0;
	}
	return ( int )Bitmap[ 2 ] + ( int )Bitmap[ 3 ] * 256;
}

int cMzDisk::GetAllBlockSize( void )
{
	if( 0 == DiskImage )
	{
		return 0;
	}
	return ( int )Bitmap[ 4 ] + ( int )Bitmap[ 5 ] * 256;
}

int cMzDisk::GetClusterSize( void )
{
	if( 0 == DiskImage )
	{
		return 0;
	}
	return ClusterSize;
}
