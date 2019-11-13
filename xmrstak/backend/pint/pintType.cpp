
#include "pintType.hpp"

#include <cstdio>
#include <cstring>
#include <inttypes.h>


namespace xmrstak
{
namespace ppu
{
void ppuid(uint32_t eax, int32_t ecx, int32_t val[4])
{
	std::memset(val, 0, sizeof(int32_t) * 4);
}

int32_t get_masked(int32_t val, int32_t h, int32_t l)
{
	val &= (0x7FFFFFFF >> (31 - (h - l))) << l;
	return val >> l;
}

bool has_feature(int32_t val, int32_t bit)
{
	int32_t mask = 1 << bit;
	return (val & mask) != 0u;
}

//
// FIXME
//
Model getModel()
{
	//
	// under construciton
	//
	int32_t ppu_info[4];
	char ppustr[13] = {0};

	ppuid(0, 0, ppu_info);
	std::memcpy(ppustr, &ppu_info[1], 4);
	std::memcpy(ppustr + 4, &ppu_info[3], 4);
	std::memcpy(ppustr + 8, &ppu_info[2], 4);

	Model result;

//	cpuid(1, 0, cpu_info);
//
//	result.family = get_masked(cpu_info[0], 12, 8);
//	result.model = get_masked(cpu_info[0], 8, 4) | get_masked(cpu_info[0], 20, 16) << 4;
//	result.type_name = cpustr;
//
//	// feature bits https://en.wikipedia.org/wiki/CPUID
//	// sse2
//	result.sse2 = has_feature(cpu_info[3], 26);
//	// aes-ni
//	result.aes = has_feature(cpu_info[2], 25);
//	// avx - 27 is the check if the OS overwrote cpu features
//	result.avx = has_feature(cpu_info[2], 28) && has_feature(cpu_info[2], 27);
//
//	if(strcmp(cpustr, "AuthenticAMD") == 0)
//	{
//		if(result.family == 0xF)
//			result.family += get_masked(cpu_info[0], 28, 20);
//	}

	return result;
}

} // namespace ppu
} // namespace xmrstak
