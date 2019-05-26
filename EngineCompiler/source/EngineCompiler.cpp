#include <iostream>
#include <filesystem>
#include <direct.h>
#include <vector>
#include <string>
#include <fstream>


std::vector<std::string> foundComponentPaths;
std::string applicationPath;
const std::string dotComponent = ".component";
std::string componentsDirectory;
std::string devCommandPromptDir;

void FindComponentFiles()
{
	foundComponentPaths = std::vector<std::string>();
	applicationPath = (std::string)_getcwd(NULL, 0);
	componentsDirectory = applicationPath + "\\components";
	for (const auto & entry : std::filesystem::recursive_directory_iterator("components"))
	{
		const std::string tempPath = entry.path().u8string();
			foundComponentPaths.push_back(applicationPath + "\\" + entry.path().u8string());
		if (tempPath.compare(tempPath.size() - dotComponent.size(), dotComponent.size(), dotComponent) == 0)
		{
		}
	}
	for (const std::string & path : foundComponentPaths)
	{
		std::cout << "Found component file : " << path << std::endl;
	}
}

void FindDevCommandPrmopt()
{
	if (std::ifstream ("C:\\Program Files (x86)\\Microsoft Visual Studio\\2019\\Community\\VC\\Auxiliary\\Build\\vcvars64.bat"))
	{
		devCommandPromptDir = "C:\\Program Files (x86)\\Microsoft Visual Studio\2019\Community\\VC\\Auxiliary\\Build";
	}
	else if (std::ifstream ("C:\\Program Files (x86)\\Microsoft Visual Studio\\2017\\Community\\VC\\Auxiliary\\Build\\vcvars64.bat"))
	{
		devCommandPromptDir = "C:\\Program Files (x86)\\Microsoft Visual Studio\\2017\\Community\\VC\\Auxiliary\\Build";
	}
	else
	{
		std::cout << "Visual Studio 2017 or later is required." << std::endl;
		return;
	}
	system(("c: && cd " + devCommandPromptDir + " && vcvars64").c_str());
}

void CompileFilesToApplication()
{
	if (devCommandPromptDir == "") return;
	for (const std::string & path : foundComponentPaths)
	{
		std::string actionString = ("c: && cd " + devCommandPromptDir + " && vcvars64 && ");
		std::string componentFileName = path.substr(componentsDirectory.size() + 1);
		std::cout << "Compiling component file : " << componentFileName << std::endl;
		actionString = (actionString + path.substr(0, 2) + " && cd " + componentsDirectory + " && cl /EHsc " + componentFileName);
		system(actionString.c_str());
	}
}

int main()
{
	FindComponentFiles();
	FindDevCommandPrmopt();
	CompileFilesToApplication();

	while (1 != 0) {}
}