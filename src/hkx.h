#include <memory>
#include <stdint.h>
#include <iostream>
#include <fstream>
#include <filesystem>

struct LayoutRules {
  uint8_t bytesInPointer;
  uint8_t littleEndian;
  uint8_t reusePaddingOptimization;
  uint8_t emptyBaseClassOptimization;
};

struct Header {
  int32_t magic0;
  int32_t magic1;
  int32_t userTag;
  int32_t fileVersion;
  LayoutRules layoutRules;
  int32_t numSections;
  int32_t contentsSectionIndex;
  int32_t contentsSectionOffset;
  int32_t contentsClassNameSectionIndex;
  int32_t contentsClassNameSectionOffset;
  char contentsVersion[16];
  int32_t flags;
  int32_t pad;
};

struct SectionHeader {
  char sectionTag[20];
  int32_t absoluteDataStart;
  int32_t localFixupsOffset;
  int32_t globalFixupsOffset;
  int32_t virtualFixupsOffset;
  int32_t exportsOffset;
  int32_t importsOffset;
  int32_t endOffset;
};

class HKX {
public:
  Header *header;
  SectionHeader **sections;

private:
  char *raw_data;

public:
  void parse(const char *hkxPath) {
    std::filesystem::path filepath(hkxPath);
    std::uintmax_t filesize = std::filesystem::file_size(filepath);
    raw_data = new char[filesize];
    std::ifstream fin(filepath, std::ios::binary);
    fin.read(raw_data, filesize);
    if (!fin) {
      std::cerr << "Error reading file, could only read " << fin.gcount()
                << " bytes" << std::endl;
    }

	fin.close();

    header = reinterpret_cast<Header *>(raw_data);
    
	int sectionSize = 48;
	if (header->fileVersion == 11) sectionSize = 64;
	int sectionOffset = sizeof(Header);
	for (int i = 0; i < header->numSections; i++) {
		sections[i] = reinterpret_cast<SectionHeader*>(raw_data + sectionOffset);
		sectionOffset += sectionSize;
	}

	for (int i = 0; i < header->numSections; i++) {
		SectionHeader *section = sections[i];
		auto localFixup = section->absoluteDataStart + section->localFixupsOffset;
		auto localFixupSize = section->globalFixupsOffset - section->localFixupsOffset;
		auto globalFixupsOffset
	}

  }

  private:
  void fixup() {

  }
};