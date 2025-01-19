#pragma once
#include <unordered_map>
#include <string>
#include <memory>
#include <sstream>
#include <functional>

template<typename T>
class LoaderFactory {
public:
	using LoaderCreator = std::function<std::shared_ptr<T>()>;

	static void RegisterLoader(const std::string &extension, LoaderCreator creator)
	{
		// The extension is in the format ".ext1;.ext2;.ext3" so we need to split it
		std::istringstream iss(extension);
		std::string ext;
		while (std::getline(iss, ext, ';'))
		{
			GetRegistry()[ext] = creator;
		}
	}

	static std::shared_ptr<T> CreateLoader(const std::string &extension)
	{
		auto &registry = GetRegistry();
		auto it = registry.find(extension);
		if (it != registry.end())
		{
			return it->second();
		}
		return nullptr;
	}

private:

	static std::unordered_map<std::string, LoaderCreator> &GetRegistry()
	{
		static std::unordered_map<std::string, LoaderCreator> registry;
		return registry;
	}
};
