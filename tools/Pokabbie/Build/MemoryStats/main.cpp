#include <fstream>
#include <iostream>
#include <limits>
#include <string>
#include <sstream>
#include <thread>
#include <vector>

#include "StringUtils.h"


static inline void FatalExit()
{
    exit(1);
}

#define FATAL_ERROR(format)                \
do                                         \
{                                          \
	std::cout << format << '\n';           \
	FatalExit();                           \
} while (0)


const char *const cUsage = "Usage: memorystats [-F TARGET_PATH] FILE_PATH\n";

static void ProcessMapFile(std::string const& filePath);

int main(int argc, char **argv)
{
    // Comment this back in, if we want to skip this on Github
    //
    //const char* automationHost = std::getenv("AUTOMATION_HOST");
    //if (automationHost != nullptr)
    //{
    //    std::cout << "Skipping memorystats (AUTOMATION_HOST is defined). ";
    //    return 0;
    //}

    std::vector<std::string> filesToProcess;

    argc--;
    argv++;

    while (argc > 1)
    {
        std::string arg(argv[0]);
        if (arg.substr(0, 2) == "-F")
        {
            std::string filePath = arg.substr(2);
            if (filePath.empty())
            {
                argc--;
                argv++;
                filePath = std::string(argv[0]);
            }

            filesToProcess.push_back(filePath);
        }
        else
        {
            FATAL_ERROR(cUsage);
        }
        argc--;
        argv++;
    }

    if (argc != 0)
    {
        FATAL_ERROR(cUsage);
    }

    for (std::string const& filePath : filesToProcess)
    {
        std::cout << "Parsing Memory Stats for: '" << filePath << "'.. ";
        ProcessMapFile(filePath);
        std::cout << "Done!";
    }

    return 0;
}

struct MemoryStatsItem
{
    std::string name;
    size_t address = 0;
    size_t size = 0;
};

struct MemoryStatsBlock
{
    MemoryStatsItem block;
    std::vector<MemoryStatsItem> items;
};

struct MemoryStatsDomain
{
    size_t startAddress = std::numeric_limits<size_t>::max();
    size_t endAddress = std::numeric_limits<size_t>::min();
    std::vector<MemoryStatsBlock> blocks;
};

struct MemoryStats
{
    MemoryStatsDomain ewram;
    MemoryStatsDomain iwram;
    MemoryStatsDomain rom;
};

enum class ReadBlock
{
    None,
    EWRam,
    IWRam,
    Rom
};

struct ParsingStates
{
    ReadBlock currentReadBlock = ReadBlock::None;
    bool readingStatsBlock = false;
    MemoryStatsDomain* targetDomain = nullptr;
    MemoryStatsBlock* targetBlock = nullptr;
    MemoryStatsItem* previousItem = nullptr;
    bool hasReadBlockHeader = false;
};

static bool ReadEWRamData(std::string const& line, MemoryStats& outputStats, ParsingStates& states);
static bool ReadIWRamData(std::string const& line, MemoryStats& outputStats, ParsingStates& states);
static bool ReadRomData(std::string const& line, MemoryStats& outputStats, ParsingStates& states);

static void ReadStatsBlock(std::string const& line, MemoryStats& outputStats, ParsingStates& states);

static void DumpStatsFile(MemoryStats const& memStats, std::string const& filePath);

static void ProcessMapFile(std::string const& filePath)
{
    std::ifstream stream(filePath, std::ios::in);

    bool isReadingEnabled = false;
    ParsingStates states;

    MemoryStats outputStats;

    for (std::string line; getline(stream, line);)
    {
        strutil::replace_all(line, "\r", "");

        if (!isReadingEnabled)
        {
            if (strutil::starts_with(line,"Linker script and memory map"))
            {
                isReadingEnabled = true;
            }
        }
        else
        {
            if (strutil::starts_with(line, "ewram "))
            {
                states.currentReadBlock = ReadBlock::EWRam;
                states.previousItem = nullptr;
                continue;
            }
            else if (strutil::starts_with(line, "iwram "))
            {
                states.currentReadBlock = ReadBlock::IWRam;
                states.previousItem = nullptr;
                continue;
            }
            else if (strutil::starts_with(line, ".text "))
            {
                states.currentReadBlock = ReadBlock::Rom;
                states.previousItem = nullptr;
                continue;
            }

            bool newBlock = false;

            switch (states.currentReadBlock)
            {
            case ReadBlock::EWRam:
                newBlock = ReadEWRamData(line, outputStats, states);
                break;
            case ReadBlock::IWRam:
                newBlock = ReadIWRamData(line, outputStats, states);
                break;
            case ReadBlock::Rom:
                newBlock = ReadRomData(line, outputStats, states);
                break;
            default:
                break;
            }

            if (!newBlock && states.readingStatsBlock && !line.empty())
            {
                ReadStatsBlock(line, outputStats, states);
            }
        }
    }

    
    std::string outputFile = filePath;
    strutil::replace_all(outputFile, ".map", "_memorystats.csv");

    std::cout << "Sizes:\n";
    std::cout << "\t-ROM: " << (outputStats.rom.endAddress - outputStats.rom.startAddress) / 8 << " bytes.\n";
    std::cout << "\t-EWRAM: " << (outputStats.ewram.endAddress - outputStats.ewram.startAddress) / 8 << " bytes.\n";
    std::cout << "\t-IWRAM: " << (outputStats.iwram.endAddress - outputStats.iwram.startAddress) / 8 << " bytes.\n";

    std::cout << "Conversion Reference:\n";
    std::cout << "\t- 8MB: " << (8 * 1024 * 1024) << " bytes.\n";
    std::cout << "\t-16MB: " << (16 * 1024 * 1024) << " bytes.\n";
    std::cout << "\t-32MB: " << (32 * 1024 * 1024) << " bytes.\n";
    std::cout << "\t-64MB: " << (64 * 1024 * 1024) << " bytes.\n";

    DumpStatsFile(outputStats, outputFile);
}

static void CloseExistingBlock(MemoryStats& outputStats, ParsingStates& states)
{
    states.readingStatsBlock = true;
    states.hasReadBlockHeader = false;

    if (states.targetBlock != nullptr)
    {
        if (states.targetBlock->block.size == 0)
        {
            // Manually calculate the size
            for (auto item : states.targetBlock->items)
            {
                states.targetBlock->block.size += item.size;
            }
        }
    }
}

static MemoryStatsBlock* FindOrCreateBlock(MemoryStatsDomain& domain, std::string const& blockName)
{
    for (MemoryStatsBlock& block : domain.blocks)
    {
        if (block.block.name == blockName)
        {
            // Found existing
            return &block;
        }
    }

    // Make new
    domain.blocks.emplace_back();

    MemoryStatsBlock& statsBlock = domain.blocks.back();
    statsBlock.block.name = blockName;
    return &statsBlock;
}

static bool ReadEWRamData(std::string const& line, MemoryStats& outputStats, ParsingStates& states)
{
    if (strutil::ends_with(line, "(ewram_data)"))
    {
        CloseExistingBlock(outputStats, states);

        // omit name
        std::string blockName = line.substr(0, line.size() - 12);
        strutil::trim(blockName);

        states.targetDomain = &outputStats.ewram;
        states.targetBlock = FindOrCreateBlock(outputStats.ewram, blockName);
        return true;
    }

    return false;
}

static bool ReadIWRamData(std::string const& line, MemoryStats& outputStats, ParsingStates& states)
{
    if (strutil::ends_with(line, "(.bss)"))
    {
        CloseExistingBlock(outputStats, states);

        // omit name
        std::string blockName = line.substr(0, line.size() - 6);
        strutil::trim(blockName);

        states.targetDomain = &outputStats.iwram;
        states.targetBlock = FindOrCreateBlock(outputStats.iwram, blockName);
        return true;
    }

    return false;
}

static bool ReadRomData(std::string const& line, MemoryStats& outputStats, ParsingStates& states)
{
    if (strutil::ends_with(line, "(.text)"))
    {
        CloseExistingBlock(outputStats, states);

        // omit name
        std::string blockName = line.substr(0, line.size() - 7);
        strutil::trim(blockName);

        states.targetDomain = &outputStats.rom;
        states.targetBlock = FindOrCreateBlock(outputStats.rom, blockName);
        return true;
    }
    else if (strutil::ends_with(line, "(.rodata)"))
    {
        CloseExistingBlock(outputStats, states);

        // omit name
        std::string blockName = line.substr(0, line.size() - 9);
        strutil::trim(blockName);

        states.targetDomain = &outputStats.rom;
        outputStats.rom.blocks.emplace_back();

        MemoryStatsBlock& statsBlock = outputStats.rom.blocks.back();
        statsBlock.block.name = blockName;
        states.targetBlock = &statsBlock;
        return true;
    }

    return false;
}

static void ReadStatsBlock(std::string const& line, MemoryStats& outputStats, ParsingStates& states)
{
    std::string trimmedLine = line;
    strutil::trim(trimmedLine);

    // Skip these lines, as they don't add anything extra to memstats
    if (strutil::ends_with(trimmedLine, "(.data)"))
        return;

    // Remove spaces so it's easier to handle below
    // e.g. . = ALIGN (0x4) or . = (. + 0xc)
    auto idx = trimmedLine.find(". = ");
    if (idx != std::string::npos)
    {
        //return; // skip these for now

        // Remove spaces and reattach to end of line
        std::string suffix = trimmedLine.substr(idx);
        while (strutil::replace_all(suffix, " ", "")) {}
        
        trimmedLine = trimmedLine.substr(0, idx) + suffix;
    }

    // Trim white spaces down to a single space between each word
    while(strutil::replace_all(trimmedLine, "  ", " ")) {}

    std::vector<std::string> parts = strutil::split(trimmedLine, ' ');

    // We can have just .=ALIGN(0x4) instead of headers sometimes
    if (!states.hasReadBlockHeader && parts.size() == 4)
    {
        states.hasReadBlockHeader = true;

        // Header Format
        // domain address size source
        // e.g.  ewram_data     0x0000000002020630     0x1691 gflib/sprite.o
        states.targetBlock->block.address = strutil::parse_string_hex<size_t>(parts[1]);
        states.targetBlock->block.size = strutil::parse_string_hex<size_t>(parts[2]);
    }
    else
    {
        MemoryStatsItem item;

        if (parts.size() == 1)
        {
            // Skip
            return;
        }
        if (parts.size() == 2)
        {
            // Simple Item Format
            // address source
            // e.g.  0x0000000002020630                gSprites
            item.name = parts[1];
            item.address = strutil::parse_string_hex<size_t>(parts[0]);
        }
        else if (parts.size() == 3)
        {
            // Fill Item Format
            // fill address source
            // e.g.   *fill*         0x0000000002021cc1        0x3 
            item.name = parts[2];
            item.address = strutil::parse_string_hex<size_t>(parts[1]);
        }
        else if (parts.size() == 4)
        {
            // Full Item Format
            // type address size source
            // e.g.   .bss           0x0000000003000010      0x802 gflib/dma3_manager.o
            item.name = parts[3];
            item.address = strutil::parse_string_hex<size_t>(parts[1]);
        }
        else
        {
            FATAL_ERROR(("Unexpected iten format @ " + line).c_str());
        }

        if (states.previousItem != nullptr && item.address > states.previousItem->address)
        {
            states.previousItem->size = item.address - states.previousItem->address;
        }

        if (item.address != 0)
        {
            states.targetDomain->startAddress = std::min(states.targetDomain->startAddress, item.address);
            states.targetDomain->endAddress = std::max(states.targetDomain->endAddress, item.address);
        }

        states.targetBlock->items.emplace_back();
        states.targetBlock->items.back() = item;

        states.previousItem = &states.targetBlock->items.back();

        if (!states.hasReadBlockHeader)
        {
            states.hasReadBlockHeader = true;
            states.targetBlock->block.address = item.address;
            states.targetBlock->block.size = 0;
        }
    }


    return;
}

static void WriteSizeHeaders(std::ofstream& stream)
{
    stream << "Group, Name, MB, KB, Bytes\n";
}

static void WriteSize(std::ofstream& stream, std::string const& group, std::string const& name, size_t size)
{
    size_t byteSize = size / 8;

    stream << group << ", " << name << ", " << (byteSize / 1024 / 1024) << ", " << (byteSize / 1024) << ", " << (byteSize) << "\n";
}

static void WriteBlock(std::ofstream& stream, std::string const& domainName, MemoryStatsBlock const& block)
{
    std::vector<MemoryStatsItem> items = block.items;

    std::sort(items.begin(), items.end(),
        [](const MemoryStatsItem& itemA, const MemoryStatsItem& itemB) -> bool
        {
            return itemA.size > itemB.size;
        });

    for (auto item : items)
    {
        if(item.size != 0)
            WriteSize(stream, domainName + "::" + block.block.name, item.name, item.size);
    }
}

static void WriteDomainGroups(std::ofstream& stream, std::string const& name, MemoryStatsDomain const& domain)
{
    std::vector<MemoryStatsBlock> blocks = domain.blocks;

    std::sort(blocks.begin(), blocks.end(),
        [](const MemoryStatsBlock& blockA, const MemoryStatsBlock& blockB) -> bool
        {
            return blockA.block.size > blockB.block.size;
        });

    stream << name + " GROUPS" << "\n\n";
    WriteSizeHeaders(stream);
    WriteSize(stream, name, "TOTAL", domain.endAddress - domain.startAddress);

    for (auto block : blocks)
    {
        WriteSize(stream, name, block.block.name, block.block.size);
    }
    stream << "\n\n";

    return;
}

static void WriteDomainObjects(std::ofstream& stream, std::string const& name, MemoryStatsDomain const& domain)
{
    std::vector<MemoryStatsBlock> blocks = domain.blocks;

    std::sort(blocks.begin(), blocks.end(),
        [](const MemoryStatsBlock& blockA, const MemoryStatsBlock& blockB) -> bool
        {
            return blockA.block.size > blockB.block.size;
        });

    stream << name + " OBJECTS" << "\n\n";
    WriteSizeHeaders(stream);

    for (auto block : blocks)
    {
        WriteSize(stream, name + "::" + block.block.name, "TOTAL", block.block.size);
        WriteBlock(stream, name, block);
        stream << "\n";
    }
    stream << "\n\n";

    return;
}

static void DumpStatsFile(MemoryStats const& memStats, std::string const& filePath)
{
    std::cout << "Writing to: '" << filePath << "'.. ";

    std::ofstream stream;
    stream.open(filePath, std::ios::out);

    stream << "Memory Stats\n";
    stream << "\n";

    WriteSizeHeaders(stream);
    WriteSize(stream, "", "ROM", memStats.rom.endAddress - memStats.rom.startAddress);
    WriteSize(stream, "", "EWRAM", memStats.ewram.endAddress - memStats.ewram.startAddress);
    WriteSize(stream, "", "IWRAM", memStats.iwram.endAddress - memStats.iwram.startAddress);
    stream << "\n\n";

    WriteDomainGroups(stream, "ROM", memStats.rom);
    WriteDomainGroups(stream, "EWRAM", memStats.ewram);
    WriteDomainGroups(stream, "IWRAM", memStats.iwram);

    WriteDomainObjects(stream, "ROM", memStats.rom);
    WriteDomainObjects(stream, "EWRAM", memStats.ewram);
    WriteDomainObjects(stream, "IWRAM", memStats.iwram);
    stream << "\n\n";
}