#include <iostream>
#include <filesystem>
#include <direct.h>
#include <vector>
#include <string>
#include <fstream>


std::vector<std::string> foundComponentPaths;
std::vector<std::string> cppPaths;
std::vector<std::string> componentNames;
std::string applicationPath;
const std::string dotComponent = ".component";
const std::string dotCpp = ".cpp";
std::string componentsDirectory;
std::string intermediateDirectory;
std::string devCommandPromptDir;

void FindComponentFiles()
{
	foundComponentPaths = std::vector<std::string>();
	applicationPath = (std::string)_getcwd(NULL, 0);
	componentsDirectory = applicationPath + "\\components";
	intermediateDirectory = componentsDirectory + "\\intermediate";
	for (const auto & entry : std::filesystem::recursive_directory_iterator("components"))
	{
		const std::string tempPath = entry.path().u8string();
		if (tempPath.compare(tempPath.size() - dotComponent.size(), dotComponent.size(), dotComponent) == 0)
		{
			foundComponentPaths.push_back(applicationPath + "\\" + entry.path().u8string());
		}
	}
	for (const std::string & path : foundComponentPaths)
	{
		std::cout << "Found component file : " << path << std::endl;
	}
}

void ConvertCononentToCpp()
{
	cppPaths = std::vector<std::string>();
	componentNames = std::vector<std::string>();
	for (const std::string & path : foundComponentPaths)
	{
		componentNames.push_back(path.substr(componentsDirectory.size() + 1, (path.size() - componentsDirectory.size() - dotComponent.size() - 1)));
	}
	for (int i = 0; i < componentNames.size(); i++)
	{
		std::string name = componentNames[i];
		std::string path = foundComponentPaths[i];

		std::ifstream input(path.c_str());
		if (!input)
		{
			std::cout << "Could not locate component file at '" << path << "'." << std::endl;			
		}
		else
		{
			std::vector<std::string> tempHeader;
			std::vector<std::string> tempImplem;
			std::vector<std::string> header;
			std::vector<std::string> implem;
			bool writingHeader = false;
			bool writingImplem = false;
			std::string tempLine;
			bool headerIncluded = false;
			while (std::getline(input, tempLine))
			{
				if (tempLine.size() > 0)
				{
					if (strcmp(tempLine.c_str(), "# IMPLEMENTATION #") == 0) { writingHeader = false; writingImplem = true; }
					else if (strcmp(tempLine.c_str(), "# HEADER #") == 0) { writingHeader = true; writingImplem = false; }
					else
					{
						if (writingHeader)
						{
							tempHeader.push_back(tempLine);
						}
						if (writingImplem)
						{
							if (strcmp(tempLine.c_str(), ("#include \"" + name + ".h\"").c_str()) == 0) { headerIncluded = true; }

							tempImplem.push_back(tempLine);
						}
					}
				}
			}
			if (headerIncluded == false)
			{
				implem.push_back("#include \"" + name + ".h\"");
			}
			for (const std::string & line : tempHeader)
			{
				header.push_back(line);
			}
			for (const std::string & line : tempImplem)
			{
				implem.push_back(line);
			}

			std::filesystem::create_directory(intermediateDirectory);
			std::ofstream output;
			output.open(intermediateDirectory + "/" + name + ".h");
			for (const std::string & line : header)
			{
				output << line + "\n";
			}
			output.close();
			output.open(intermediateDirectory + "/" + name + ".cpp");
			for (const std::string & line : implem)
			{
				output << line + "\n";
			}
			output.close();

			cppPaths.push_back(intermediateDirectory + "/" + name + ".cpp");
		}
	}
}

void CreateComponentsClass()
{
	std::ofstream output;
	output.open(intermediateDirectory + "\\components.h");
	output << "#include <string>\n";
	for (const std::string & name : componentNames)
	{
		output << "#include \"" + name + ".h\"\n";
	}
	output << "int GetComponentByName(std::string _componentName);\n";
	output.close();

	output.open(intermediateDirectory + "\\components.cpp");
	output << "#include \"components.h\"\n";
	output << "int GetComponentByName(std::string _componentName)\n";
	output << "{\n";
	for (int i = 0; i < componentNames.size(); i++)
	{
		output << "if (_componentName == \"" + componentNames[i] + "\") { return " + std::to_string(i) + "; }\n";
	}
	output << "return -1;\n";
	output << "}\n";
	output.close();

	cppPaths.push_back(intermediateDirectory + "\\components.cpp");
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
	std::string cppNames = "";
	for (const std::string & path : cppPaths)
	{
		cppNames += (" " + path.substr(intermediateDirectory.size() + 1));
	}
	if (cppPaths.size() >= 1)
	{
		std::string cPath = cppPaths[0];
		std::string actionString = ("c: && cd " + devCommandPromptDir + " && vcvars64 && ");
		actionString = (actionString + cPath.substr(0, 2) + " && cd " + intermediateDirectory + " && cl /EHsc" + cppNames + " /link /out:../Program.exe");
		system(actionString.c_str());
	}
}

int main()
{
	FindComponentFiles();
	FindDevCommandPrmopt();
	ConvertCononentToCpp();
	CreateComponentsClass();
	CompileFilesToApplication();

	while (1 != 0) {}
}