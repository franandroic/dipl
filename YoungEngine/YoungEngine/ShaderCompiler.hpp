#pragma once

#include <filesystem>
#include <string>
#include <unordered_map>
#include <vector>
#include <cassert>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <system_error>

class ShaderCompiler {

public:

	//Construction
	ShaderCompiler();
	~ShaderCompiler();

	//Copy and Move constructors
	ShaderCompiler(const ShaderCompiler &) = delete;
	ShaderCompiler &operator=(const ShaderCompiler &) = delete;
	ShaderCompiler(ShaderCompiler &&) = delete;
	ShaderCompiler &operator=(ShaderCompiler &&) = delete;

	//Global access
	static inline ShaderCompiler *active = nullptr;

	const std::vector<uint32_t> &getCompiled(const std::string &sourcePath);
	std::vector<std::string> recompile();

private:

	//TODO: Currently hard-coded, to be settable later
	static constexpr const char *GLSLC_PATH = "C:/VulkanSDK/1.4.321.1/Bin/glslc.exe";

	//The first word of every SPIR-V module
	static constexpr uint32_t SPIRV_MAGIC = 0x07230203;

	//Represents a compiled shader source
	struct TrackedCode {

		std::vector<uint32_t> code;
		std::filesystem::file_time_type sourceTime;
	};

	std::string glslcPath = GLSLC_PATH;
	std::unordered_map<std::string, TrackedCode> trackedCodeMap;

	static std::vector<uint32_t> readSPIRV(const std::string &path);
	std::string runCompiler(const std::string &sourcePath) const;
	TrackedCode compile(const std::string &sourcePath) const;
};