#ifndef FILEDATA_HPP
#define FILEDATA_HPP

#include <vector>
#include <string>

namespace dms
{
	class FileData
	{
	public:
		FileData(void);
		~FileData(void);
		void Load(const std::string& path);
		void Save(const std::string& path);
		void SaveAdd(const std::string& path);
		void Clear(void);
		const void* GetBuffer(void) const;
		void GetBuffer(std::vector<unsigned char>& buffer) const;
		size_t GetBufferSize(void) const;
		void SetBuffer(const void* buffer, size_t size);
		void RewriteBuffer(void* buffer, size_t size, size_t bufferAddress);
		bool Empty(void) const;
	private:
		std::vector<unsigned char> buffer;
	};
};

#endif
