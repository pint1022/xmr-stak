#pragma once

#include "uap/cryptonight.hpp"
#include "xmrstak/backend/iBackend.hpp"
#include "xmrstak/backend/miner_work.hpp"
#include "xmrstak/jconf.hpp"

#include <atomic>
#include <future>
#include <iostream>
#include <thread>
#include <vector>

namespace xmrstak
{
namespace ppu
{

class minethd : public iBackend
{
  public:
	static std::vector<iBackend*> thread_starter(uint32_t threadOffset, miner_work& pWork);
	static bool self_test();

	typedef void (*cn_on_new_job)(const miner_work&, cryptonight_ctx**);

	static void func_selector(cryptonight_ctx**, bool bHaveAes, bool bNoPrefetch, const xmrstak_algo& algo);

//	static cryptonight_ctx* minethd_alloc_ctx();

	template <size_t N>
	static void func_multi_selector(cryptonight_ctx**, minethd::cn_on_new_job& on_new_job,
		bool bHaveAes, bool bNoPrefetch, const xmrstak_algo& algo, const std::string& asm_version_str = "off");

  private:
//	minethd(miner_work& pWork, size_t iNo, const jconf::thd_cfg& cfg, const std::string& kernel_path);
//
//	std::promise<void> numa_promise;
//	std::promise<void> thread_work_promise;
//
//	// block thread until all NVIDIA GPUs are initialized
//	std::future<void> thread_work_guard;
	void ppu_work_main();

	minethd(miner_work& pWork, size_t iNo, int iMultiway, bool no_prefetch, int64_t affinity, const std::string& asm_version, const std::string& kernel_path);

	template <uint32_t N>
	void m_work_main();

	template <size_t N>
	void p_multiway_work(uint8_t* bWorkBlob, uint32_t** piNonce);

	void work_main();


	uint64_t iJobNo;

	miner_work oWork;

	std::promise<void> order_fix;
	std::mutex thd_aff_set;
	pint_ctx ctx;

	std::thread oWorkThd;
	int64_t affinity;

	bool bQuit;
	bool bNoPrefetch;
	std::string asm_version_str = "off";
	std::string ppu_kernel_path = "./";
};

} // namespace ppu
} // namespace xmrstak
