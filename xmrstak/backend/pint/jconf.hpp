#pragma once

#include "xmrstak/params.hpp"

#include <stdlib.h>
#include <string>

namespace xmrstak
{
namespace ppu
{

class jconf
{
  public:
	static jconf* inst()
	{
		if(oInst == nullptr)
			oInst = new jconf;
		return oInst;
	};

	bool parse_config(const char* sFilename = params::inst().configFileCPU.c_str());

	struct thd_cfg
	{
		int iMultiway;
		bool bNoPrefetch;
		std::string asm_version_str;
		long long ppu_aff;
	};

	size_t GetPPUThreadCount();
	bool GetPPUThreadConfig(size_t id, thd_cfg& cfg);
	bool NeedsAutoconf();

  private:
	jconf();
	static jconf* oInst;

	struct opaque_private;
	opaque_private* prv;
};

} // namespace ppu
} // namespace xmrstak
