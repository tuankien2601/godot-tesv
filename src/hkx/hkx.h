#ifndef GD_HKX_H
#define GD_HKX_H

#include <stdint.h>
#include <iostream>
#include <vector>
#include "havok.h"

struct LayoutRules
{
	uint8_t bytesInPointer;
	uint8_t littleEndian;
	uint8_t reusePaddingOptimization;
	uint8_t emptyBaseClassOptimization;
};

struct Header
{
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

struct SectionHeader
{
	char sectionTag[20];
	int32_t absoluteDataStart;
	int32_t localFixupsOffset;
	int32_t globalFixupsOffset;
	int32_t virtualFixupsOffset;
	int32_t exportsOffset;
	int32_t importsOffset;
	int32_t endOffset;
};

class HKX
{
public:
	Header *header;
	std::vector<SectionHeader *> sections;
	hkRootLevelContainer *rootContainer;

private:
	char *raw_data;

public:
	HKX() {}

	HKX(char *hkxData)
	{
		parse(hkxData);
	}

	void parse(char *hkxData)
	{
		raw_data = hkxData;

		header = reinterpret_cast<Header *>(raw_data);

		int sectionSize = 48;
		if (header->fileVersion == 11)
			sectionSize = 64;
		int sectionOffset = sizeof(Header);
		for (int i = 0; i < header->numSections; i++)
		{
			auto section = reinterpret_cast<SectionHeader *>(raw_data + sectionOffset);
			sections.push_back(section);
			sectionOffset += sectionSize;
		}

		for (int i = 0; i < header->numSections; i++)
		{
			SectionHeader *section = sections[i];
			auto localFixupsStart = raw_data + section->absoluteDataStart + section->localFixupsOffset;
			auto localFixupsEnd = localFixupsStart + section->globalFixupsOffset - section->localFixupsOffset;
			auto globalFixupsStart = raw_data + section->absoluteDataStart + section->globalFixupsOffset;
			auto globalFixupsEnd = globalFixupsStart + section->virtualFixupsOffset - section->globalFixupsOffset;
			auto virtualFixupsStart = raw_data + section->absoluteDataStart + section->virtualFixupsOffset;
			auto virtualFixupsEnd = virtualFixupsStart + section->exportsOffset - section->virtualFixupsOffset;

			if (localFixupsEnd - localFixupsStart > 0)
			{
				while (localFixupsStart < localFixupsEnd)
				{
					uint32_t offset = *reinterpret_cast<uint32_t *>(localFixupsStart);
					if (offset == 0xFFFFFFFF)
						break;
					uint32_t target = *reinterpret_cast<uint32_t *>(localFixupsStart + 4);
					offset += section->absoluteDataStart;
					target += section->absoluteDataStart;
					fixup(offset, target);
					localFixupsStart += 8;
				}
			}

			if (globalFixupsEnd - globalFixupsStart > 0)
			{
				while (globalFixupsStart < globalFixupsEnd)
				{
					uint32_t offset = *reinterpret_cast<uint32_t *>(globalFixupsStart);
					if (offset == 0xFFFFFFFF)
						break;
					uint32_t sectionIndex = *reinterpret_cast<uint32_t *>(globalFixupsStart + 4);
					uint32_t target = *reinterpret_cast<uint32_t *>(globalFixupsStart + 8);
					offset += section->absoluteDataStart;
					target += sections[sectionIndex]->absoluteDataStart;
					fixup(offset, target);
					globalFixupsStart += 12;
				}
			}

			if (virtualFixupsEnd - virtualFixupsStart > 0)
			{
				while (virtualFixupsStart < virtualFixupsEnd)
				{
					uint32_t offset = *reinterpret_cast<uint32_t *>(virtualFixupsStart);
					if (offset == 0xFFFFFFFF)
						break;
					uint32_t sectionIndex = *reinterpret_cast<uint32_t *>(virtualFixupsStart + 4);
					uint32_t target = *reinterpret_cast<uint32_t *>(virtualFixupsStart + 8);
					offset += section->absoluteDataStart;
					target += sections[sectionIndex]->absoluteDataStart;

					auto className = reinterpret_cast<const char *>(raw_data + target);
					if (has_vtable(className))
						fixup(offset, target);
					virtualFixupsStart += 12;
				}
			}
		}

		rootContainer = reinterpret_cast<hkRootLevelContainer *>(
			raw_data + sections[header->contentsSectionIndex]->absoluteDataStart +
			header->contentsSectionOffset);
	}

private:
	void fixup(size_t offset, size_t target)
	{
		if (header->layoutRules.bytesInPointer == 4)
		{
			*reinterpret_cast<uint32_t *>(raw_data + offset) = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(raw_data + target));
		}
		else if (header->layoutRules.bytesInPointer == 8)
		{
			*reinterpret_cast<uint64_t *>(raw_data + offset) = static_cast<uint64_t>(reinterpret_cast<uintptr_t>(raw_data + target));
		}
		else
		{
			std::cerr << "Unsupported pointer size: " << static_cast<int>(header->layoutRules.bytesInPointer) << std::endl;
			std::abort();
		}
	}
};

#endif