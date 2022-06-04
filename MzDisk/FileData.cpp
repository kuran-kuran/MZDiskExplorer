#include <stdio.h>
#include <iterator>
#include <algorithm>
#include <stdexcept>
#include "Format.hpp"
#include "FileData.hpp"

namespace dms
{
	FileData::FileData(void)
	:buffer()
	{
	}

	FileData::~FileData(void)
	{
	}

	void FileData::Load(const std::string& path)
	{
		FILE* file = NULL;
		if(fopen_s(&file, path.c_str(), "rb") != 0)
		{
			throw std::runtime_error(dms::Format("File not found: %s", path.c_str()));
		}
		if(file == NULL)
		{
			throw std::runtime_error(dms::Format("File not found: %s", path.c_str()));
		}
		long position;
		fseek(file, 0, SEEK_END);
		position = ftell(file);
		if(position == -1)
		{
			throw std::runtime_error(dms::Format("File size error: %s", path.c_str()));
		}
		size_t filesize = static_cast<size_t>(position);
		fseek(file, 0, SEEK_SET);
		this->buffer.resize(filesize);
		fread(&this->buffer[0], 1, filesize, file);
		fclose(file);
	}

	void FileData::Save(const std::string& path)
	{
		FILE* file = NULL;
		if(fopen_s(&file, path.c_str(), "wb") != 0)
		{
			throw std::runtime_error(dms::Format("Save file open error: %s", path.c_str()));
		}
		if(file == NULL)
		{
			throw std::runtime_error(dms::Format("Save file open error: %s", path.c_str()));
		}
		fwrite(&this->buffer[0], 1, this->buffer.size(), file);
		fclose(file);
	}

	void FileData::SaveAdd(const std::string& path)
	{
		FILE* file = NULL;
		if(fopen_s(&file, path.c_str(), "rb+") != 0)
		{
			if(fopen_s(&file, path.c_str(), "wb+") != 0)
			{
				throw std::runtime_error(dms::Format("SaveAdd file open error: %s", path.c_str()));
			}
		}
		if(file == NULL)
		{
			throw std::runtime_error(dms::Format("SaveAdd file open error: %s", path.c_str()));
		}
		fseek(file, 0, SEEK_END);
		fwrite(&this->buffer[0], 1, this->buffer.size(), file);
		fclose(file);
	}

	void FileData::Clear(void)
	{
		this->buffer.clear();
	}

	const void* FileData::GetBuffer(void) const
	{
		if(this->buffer.empty() == true)
		{
			return NULL;
		}
		return reinterpret_cast<const void*>(&(this->buffer[0]));
	}

	void FileData::GetBuffer(std::vector<unsigned char>& buffer) const
	{
		buffer.clear();
		std::copy(this->buffer.begin(), this->buffer.end(), std::back_inserter(buffer));
	}

	size_t FileData::GetBufferSize(void) const
	{
		return this->buffer.size();
	}

	void FileData::SetBuffer(const void* buffer, size_t size)
	{
		this->buffer.clear();
		const unsigned char* writeBuffer = reinterpret_cast<const unsigned char*>(buffer);
		std::copy(writeBuffer, writeBuffer + size, std::back_inserter(this->buffer));
	}

	void FileData::RewriteBuffer(void* buffer, size_t size, size_t bufferOffset)
	{
		if(bufferOffset + size > this->buffer.size())
		{
			return;
		}
		unsigned char* writeBuffer = reinterpret_cast<unsigned char*>(buffer);
		std::copy(writeBuffer, writeBuffer + size, &this->buffer[bufferOffset]);
	}

	bool FileData::Empty(void) const
	{
		return this->buffer.empty();
	}
};
