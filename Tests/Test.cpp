#include "stdafx.h"
#include "Test.h"


void APIENTRY glDebugCallbackTest(GLenum source, GLenum mType, GLuint id, GLenum severity, GLsizei length,
	const GLchar* message, const void* userParam);

TestFailException::TestFailException(const fs::path& framesFolder, const std::string& error) :
	framesFolder(framesFolder), error(error), runtime_error(error) {}

Test::Test(const fs::path& testType, const fs::path& testCategory, const fs::path& testName) 
try : nes(testType / testCategory / (testName.string() + ".nes")) {
} catch(const std::runtime_error& e) {
	throw TestFailException(testType / testCategory / testName.string(), e.what());
}

void Test::makeTest() {
	glDebugMessageCallback(glDebugCallbackTest, &this->nes);
	fs::remove_all(this->nes.framesFolder);
	fs::create_directory(this->nes.framesFolder);
	
	try {
		while(glfwGetKey(this->nes.ppu.window.window, GLFW_KEY_ESCAPE) != GLFW_PRESS) {
			this->nes.tick();
			if(glfwGetKey(this->nes.ppu.window.window, GLFW_KEY_F) == GLFW_PRESS)
				throw std::runtime_error("Test Failed!");
		}
	} catch(const std::runtime_error& e) {
		throw TestFailException(this->nes.framesFolder, e.what());
	}
}

void Test::passed() {
	fs::path failFile = this->nes.framesFolder / "FAIL";

	if(fs::exists(failFile)) {
		std::ifstream file(failFile);
		std::stringstream error;
		error << file.rdbuf();
		throw std::runtime_error(error.str());
	}
}

void APIENTRY glDebugCallbackTest(GLenum source, GLenum mType, GLuint id, GLenum severity, GLsizei length,
	const GLchar* message, const void* userParam) {
	if(id == 0xABC) {
		reinterpret_cast<const NESTest*>(userParam)->dumpScreen();
		return;
	}
	std::string src;
	switch(source) {
	case GL_DEBUG_SOURCE_API:
		src = "Windows";
		break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER:
		src = "Shader Compiler";
		break;
	case GL_DEBUG_SOURCE_THIRD_PARTY:
		src = "Third Party";
		break;
	case GL_DEBUG_SOURCE_APPLICATION:
		src = "Application";
		break;
	case GL_DEBUG_SOURCE_OTHER:
		src = "Other";
		break;
	}

	std::string type;
	switch(mType) {
	case GL_DEBUG_TYPE_ERROR:
		type = "Error";
		break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
		type = "Deprecated Behavior";
		break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
		type = "Undefined Behavior";
		break;
	case GL_DEBUG_TYPE_PORTABILITY:
		type = "Portability Error";
		break;
	case GL_DEBUG_TYPE_PERFORMANCE:
		type = "Performance Error";
		break;
	case GL_DEBUG_TYPE_OTHER:
		type = "Other Error";
		break;
	}

	switch(severity) {
	case GL_DEBUG_SEVERITY_LOW:
		cout << "GL LOW - " + src + " " + type + ": " + message << endl;
		break;
	case GL_DEBUG_SEVERITY_MEDIUM:
		cout << "GL MEDIUM - " + src + " " + type + ": " + message << endl;
		break;
	case GL_DEBUG_SEVERITY_HIGH:
		throw std::runtime_error("GL HIGH - " + src + " " + type + ": " + message);
		break;
	}
}
