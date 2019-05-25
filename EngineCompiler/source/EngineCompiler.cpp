#include <iostream>
#include <filesystem>
#include <direct.h>
#include <vector>
#include <string>


std::vector<std::string> foundComponentPaths;

void FindComponentFiles()
{
	foundComponentPaths = std::vector<std::string>();
	const std::string c = ".component";
	for (const auto & entry : std::filesystem::recursive_directory_iterator("components"))
	{
		const std::string tempPath = entry.path().u8string();
		if (tempPath.compare(tempPath.size() - c.size(), c.size(), c) == 0)
		{
			foundComponentPaths.push_back((std::string)_getcwd(NULL, 0) + "\\" + entry.path().u8string());
		}
	}
	for (const std::string & path : foundComponentPaths)
	{
		std::cout << path << std::endl;
	}
}

int main()
{
	FindComponentFiles();

	while (1 != 0) {}
}