/*
  * This program is free software: you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation, either version 3 of the License, or
  * any later version.
  *
  * This program is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  * GNU General Public License for more details.
  *
  * You should have received a copy of the GNU General Public License
  * along with this program.  If not, see <http://www.gnu.org/licenses/>.
  *
  * Additional permission under GNU GPL version 3 section 7
  *
  * If you modify this Program, or any covered work, by linking or combining
  * it with OpenSSL (or a modified version of that library), containing parts
  * covered by the terms of OpenSSL License and SSLeay License, the licensors
  * of this Program grant you additional permission to convey the resulting work.
  *
  */

#include "backendConnector.hpp"
#include "globalStates.hpp"
#include "iBackend.hpp"
#include "miner_work.hpp"
#include "plugin.hpp"
#include "xmrstak/misc/console.hpp"
#include "xmrstak/misc/environment.hpp"
#include "xmrstak/params.hpp"

#include "cpu/minethd.hpp"
#ifndef CONF_NO_CUDA
#include "nvidia/minethd.hpp"
#endif
#ifndef CONF_NO_OPENCL
#include "amd/minethd.hpp"
#endif
//#ifndef CONF_NO_PPU
#include "pint/minethd.hpp"
//#endif

#include <assert.h>
#include <bitset>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <thread>

namespace xmrstak
{

bool BackendConnector::self_test()
{
	bool ret = cpu::minethd::self_test();
	if (!ret) {
		printf("%s %d: cpu self test fails!!!!!!!!!!!!\n", __FILE__, __LINE__);
		return ret;
	}
	return (ret && ppu::minethd::self_test());
}

std::vector<iBackend*>* BackendConnector::thread_starter(miner_work& pWork)
{

	std::vector<iBackend*>* pvThreads = new std::vector<iBackend*>;

#ifndef CONF_NO_OPENCL
	if(params::inst().useAMD)
	{
		const std::string backendName = xmrstak::params::inst().openCLVendor;
		plugin amdplugin;
		amdplugin.load(backendName, "xmrstak_opencl_backend");
		std::vector<iBackend*>* amdThreads = amdplugin.startBackend(static_cast<uint32_t>(pvThreads->size()), pWork, environment::inst());
		size_t numWorkers = 0u;
		if(amdThreads != nullptr)
		{
			pvThreads->insert(std::end(*pvThreads), std::begin(*amdThreads), std::end(*amdThreads));
			numWorkers = amdThreads->size();
			delete amdThreads;
		}
		if(numWorkers == 0)
			printer::inst()->print_msg(L0, "WARNING: backend %s (OpenCL) disabled.", backendName.c_str());
	}
#endif

#ifndef CONF_NO_CUDA
	if(params::inst().useNVIDIA)
	{
		bool disableNvidia = false;

		plugin nvidiaplugin;
#ifdef XMRSTAK_DEV_RELEASE
		std::vector<std::string> libNames = {"xmrstak_cuda_backend_cuda10_0", "xmrstak_cuda_backend"};
#	ifndef _WIN32
		auto neededAlgorithms = ::jconf::inst()->GetCurrentCoinSelection().GetAllAlgorithms();
		bool cn_r_derivate =
			std::find(neededAlgorithms.begin(), neededAlgorithms.end(), cryptonight_r) != neededAlgorithms.end() ||
			std::find(neededAlgorithms.begin(), neededAlgorithms.end(), cryptonight_r_wow) != neededAlgorithms.end();

		if(cn_r_derivate)
		{
			disableNvidia = true;
			printer::inst()->print_msg(L0, "WARNING: The linux release binaries not support cryptonight_r derived coins for NVIDIA.");		
		}
#	endif
#else
		std::vector<std::string> libNames = {"xmrstak_cuda_backend"};
#endif
		size_t numWorkers = 0u;

		if(!disableNvidia)
		{
			for(const auto& name : libNames)
			{
				printer::inst()->print_msg(L0, "NVIDIA: try to load library '%s'", name.c_str());
				nvidiaplugin.load("NVIDIA", name);
				std::vector<iBackend*>* nvidiaThreads = nvidiaplugin.startBackend(static_cast<uint32_t>(pvThreads->size()), pWork, environment::inst());
				if(nvidiaThreads != nullptr)
				{
					pvThreads->insert(std::end(*pvThreads), std::begin(*nvidiaThreads), std::end(*nvidiaThreads));
					numWorkers = nvidiaThreads->size();
					delete nvidiaThreads;
				}
				else
				{
					// remove the plugin if we have found no GPUs
					nvidiaplugin.unload();
				}
				// we found at leat one working GPU
				if(numWorkers != 0)
				{
					printer::inst()->print_msg(L0, "NVIDIA: use library '%s'", name.c_str());
					break;
				}
			}
		}
		if(numWorkers == 0)
			printer::inst()->print_msg(L0, "WARNING: backend NVIDIA disabled.");
	}
#endif

#ifndef CONF_NO_CPU
	if(params::inst().useCPU)
	{
		auto cpuThreads = cpu::minethd::thread_starter(static_cast<uint32_t>(pvThreads->size()), pWork);
		pvThreads->insert(std::end(*pvThreads), std::begin(cpuThreads), std::end(cpuThreads));
		if(cpuThreads.size() == 0)
			printer::inst()->print_msg(L0, "WARNING: backend CPU disabled.");
	}
#endif
#ifndef CONF_NO_PPU
	std::string libname = "xmrstak_ppu_backend";
	if(params::inst().usePPU)
	{
#if 1
		auto ppuThreads = ppu::minethd::thread_starter(static_cast<uint32_t>(pvThreads->size()), pWork);
		pvThreads->insert(std::end(*pvThreads), std::begin(ppuThreads), std::end(ppuThreads));
//		printf("%s %d: 1000 -------------------threadsize %d \n", __FILE__, __LINE__, ppuThreads.size());
		if(ppuThreads.size() == 0)
			printer::inst()->print_msg(L0, "WARNING: backend PPU disabled.");
#else
		printer::inst()->print_msg(L0, "PPU: try to load library '%s'", libname.c_str());
//		plugin ppuplugin;
//		ppuplugin.load("ppu", libname);
//		printf("%s %d: 2000 -------------------\n", __FILE__, __LINE__);

		std::vector<iBackend*>* ppuThreads = ppu::minethd::thread_starter(static_cast<uint32_t>(pvThreads->size()), pWork);
//		printf("%s %d: 2000 -------------------\n", __FILE__, __LINE__);
//		auto ppuThreads = ppu::minethd::thread_starter(static_cast<uint32_t>(pvThreads->size()), pWork);
		size_t numWorkers = 0u;
		if(ppuThreads != nullptr)
		{
			pvThreads->insert(std::end(*pvThreads), std::begin(*ppuThreads), std::end(*ppuThreads));
			numWorkers = ppuThreads->size();
			delete ppuThreads;
		}
//		else
//		{
//			// remove the plugin if we have found no GPUs
//			ppuplugin.unload();
//		}
		if(numWorkers == 0)
			printer::inst()->print_msg(L0, "WARNING: backend %s (PPU) disabled.", libname.c_str());
#endif
	}
#endif
	globalStates::inst().iThreadCount = pvThreads->size();
	return pvThreads;
}

} // namespace xmrstak
