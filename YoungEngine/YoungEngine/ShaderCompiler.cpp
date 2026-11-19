#include "ShaderCompiler.hpp"

ShaderCompiler::ShaderCompiler() {

	
	if (!std::filesystem::exists(glslcPath)) {
		throw std::runtime_error("glslc wasn't found at " + glslcPath);
	}

	assert(!active && "ShaderCompiler already exists!");
	active = this;
}

ShaderCompiler::~ShaderCompiler() {

	if (active == this) active = nullptr;
}

const std::vector<uint32_t> &ShaderCompiler::getCompiled(const std::string &sourcePath) {

	//The SPIR-V of the source, compiled on the first request.
	//Throws with glslc's output if it doesn't compile.

	auto found = trackedCodeMap.find(sourcePath);
	if (found != trackedCodeMap.end()) return found->second.code;

	return trackedCodeMap.emplace(sourcePath, compile(sourcePath)).first->second.code;
}

std::vector<std::string> ShaderCompiler::recompile() {

	//Recompiles every source compiled so far whose file changed since its last compile.
	//A source that fails keeps its previous code.
	//Returns the paths whose code was replaced.

	std::vector<std::string> changed;

	for (auto &[path, entry] : trackedCodeMap) {

		std::error_code error;
		std::filesystem::file_time_type sourceTime = std::filesystem::last_write_time(path, error);

		if (error) {
			std::cerr << "Can't read the shader source " << path << ": " << error.message() << std::endl;
			continue;
		}

		if (sourceTime == entry.sourceTime) continue;

		try {
			entry = compile(path);
			changed.push_back(path);
			std::cout << "Recompiled " << path << std::endl;
		} catch (const std::exception &e) {
			std::cerr << e.what() << std::endl;
		}
	}

	return changed;
}

std::vector<uint32_t> ShaderCompiler::readSPIRV(const std::string &path) {

	std::ifstream file(path, std::ios::ate | std::ios::binary);

	if (!file.is_open()) {
		throw std::runtime_error("Failed to open file: " + path);
	}

	size_t fileSize = static_cast<size_t>(file.tellg());

	//SPIR-V is a stream of 32-bit words
	if (fileSize == 0 || fileSize % sizeof(uint32_t) != 0) {
		throw std::runtime_error("Not a valid SPIR-V file: " + path);
	}

	std::vector<uint32_t> code(fileSize / sizeof(uint32_t));

	file.seekg(0);
	file.read(reinterpret_cast<char *>(code.data()), fileSize);

	if (code[0] != SPIRV_MAGIC) {
		throw std::runtime_error("Not a valid SPIR-V file (wrong magic number): " + path);
	}

	return code;
}

std::string ShaderCompiler::runCompiler(const std::string &sourcePath) const {

	//Compiles the source with glslc into shaders/spv/x.vert.spv next to it and returns the path of that file.
	//Throws with glslc's messages if it fails, prints them if it succeeds (they're warnings then).

	std::filesystem::path source(sourcePath);
	std::filesystem::path outputFolder = source.parent_path() / "spv";
	std::string output = (outputFolder / (source.filename().string() + ".spv")).string();

	//glslc doesn't create the folder it writes into
	std::filesystem::create_directories(outputFolder);

	//glslc writes errors and warnings to stderr, so it's redirected into what's collected
	std::string command = "\"" + glslcPath + "\" \"" + sourcePath + "\" -o \"" + output + "\" 2>&1";
	std::string messages;

	//_popen runs the command through cmd /c, which strips the first and the last quote of the line when there
	//are more than two of them. The whole command is wrapped in another pair of quotes for it to strip.
	FILE *pipe = _popen(("\"" + command + "\"").c_str(), "r");

	if (!pipe) {
		throw std::runtime_error("Failed to run: " + command);
	}

	char buffer[512];
	while (fgets(buffer, sizeof(buffer), pipe)) {
		messages += buffer;
	}

	//The exit code of glslc decides whether it worked, so an old file left at the output is never read by mistake
	if (_pclose(pipe) != 0) {
		throw std::runtime_error("Failed to compile shader " + sourcePath + ":\n" + messages);
	}

	//Warnings
	if (!messages.empty()) {
		std::cerr << messages;
	}

	return output;
}

ShaderCompiler::TrackedCode ShaderCompiler::compile(const std::string &sourcePath) const {

	//The time is taken before compiling, so an edit saved while glslc runs is seen by the next recompile

	std::error_code error;
	std::filesystem::file_time_type sourceTime = std::filesystem::last_write_time(sourcePath, error);

	if (error) {
		throw std::runtime_error("Can't read the shader source " + sourcePath + ": " + error.message());
	}

	return TrackedCode{ readSPIRV(runCompiler(sourcePath)), sourceTime };
}