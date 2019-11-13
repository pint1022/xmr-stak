#pragma once

#include "jconf.hpp"

#include "xmrstak/backend/pint/pintType.hpp"
#include "xmrstak/backend/cryptonight.hpp"
#include "xmrstak/jconf.hpp"
#include "xmrstak/misc/configEditor.hpp"
#include "xmrstak/misc/console.hpp"
#include "xmrstak/params.hpp"
#include <string>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif // _WIN32

namespace xmrstak
{
namespace ppu
{

class autoAdjust
{
  public:
	bool printConfig()
	{
		auto neededAlgorithms = ::jconf::inst()->GetCurrentCoinSelection().GetAllAlgorithms();

		size_t hashMemSize = 0;
		for(const auto algo : neededAlgorithms)
		{
			hashMemSize = std::max(hashMemSize, algo.Mem());
		}
		const size_t hashMemSizeKB = hashMemSize / 1024u;

		const size_t halfHashMemSizeKB = hashMemSizeKB / 2u;

		configEditor configTpl{};

		// load the template of the backend config into a char variable
		const char* tpl =
#include "./config.tpl"
			;
		configTpl.set(std::string(tpl));

		std::string conf;

		{
			detectPPUConf();
			printer::inst()->print_msg(L0, "Autoconf core count detected as %u on %s.", corecnt,
				linux_layout ? "Linux" : "Windows");

			uint32_t aff_id = 0;
			for(uint32_t i = 0; i < corecnt; i++)
			{
				bool double_mode;

				if(L3KB_size <= 0)
					break;

				double_mode = L3KB_size / hashMemSizeKB > (int32_t)(corecnt - i);

				conf += std::string("    { \"low_power_mode\" : ");
				conf += std::string(double_mode ? "true" : "false");
				conf += std::string(" },\n");

				if(!linux_layout || old_amd)
				{
					aff_id += 2;

					if(aff_id >= corecnt)
						aff_id = 1;
				}
				else
					aff_id++;

				if(double_mode)
					L3KB_size -= hashMemSizeKB * 2u;
				else
					L3KB_size -= hashMemSizeKB;
			}
		}

		configTpl.replace("PPUCONFIG", conf);
		configTpl.write(params::inst().configFilePPU);
		printer::inst()->print_msg(L0, "PPU configuration stored in file '%s'", params::inst().configFilePPU.c_str());

		return true;
	}

  private:

	void detectPPUConf()
	{
#ifdef _WIN32
		SYSTEM_INFO info;
		GetSystemInfo(&info);
		corecnt = info.dwNumberOfProcessors;
		linux_layout = false;
#else
		corecnt = 1;
		linux_layout = true;
#endif // _WIN32
	}

	int32_t L3KB_size = 0;
	uint32_t corecnt;
	bool old_amd = false;
	bool linux_layout;
};

} // namespace cpu
} // namespace xmrstak
