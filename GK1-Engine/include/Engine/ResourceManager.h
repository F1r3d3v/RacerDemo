#pragma once
#include <unordered_map>
#include <memory>
#include <string>
#include <stdexcept>
#include <typeindex>

#include "Engine/Resource/Resource.h"
#include "Concepts.h"

class ResourceManager {
public:
	static ResourceManager &Get()
	{
		static ResourceManager instance;
		return instance;
	}

	template<IsResource T>
	void Add(const std::string &name, std::shared_ptr<T> resource)
	{
		auto typeIndex = std::type_index(typeid(T));
		auto &resourceMap = m_resources[typeIndex];

		if (resourceMap.find(name) != resourceMap.end())
		{
			throw std::runtime_error("Resource '" + name + "' already exists!");
		}

		resourceMap[name] = resource;
	}

	template<IsResource T, typename... Args>
		requires std::constructible_from<T, Args...>
	std::shared_ptr<T> Create(const std::string &name, Args&&... args)
	{
		auto resource = std::make_shared<T>(std::forward<Args>(args)...);
		resource->SetName(name);
		Add<T>(name, resource);
		return resource;
	}

	template<IsResource T>
	std::shared_ptr<T> Load(const std::string &name)
	{
		auto typeIndex = std::type_index(typeid(T));
		auto mapIt = m_resources.find(typeIndex);

		if (mapIt == m_resources.end())
		{
			throw std::runtime_error("No resources of type " + std::string(typeid(T).name()));
		}

		auto &resourceMap = mapIt->second;
		auto resourceIt = resourceMap.find(name);

		if (resourceIt == resourceMap.end())
		{
			throw std::runtime_error("Resource '" + name + "' not found!");
		}

		return std::static_pointer_cast<T>(resourceIt->second);
	}

	template<IsResource T>
	std::shared_ptr<T> TryLoad(const std::string &name) noexcept
	{
		try
		{
			return Load<T>(name);
		}
		catch (...)
		{
			return nullptr;
		}
	}

	template<IsResource T>
	bool Exists(const std::string &name) const noexcept
	{
		auto typeIndex = std::type_index(typeid(T));
		auto mapIt = m_resources.find(typeIndex);

		if (mapIt == m_resources.end())
		{
			return false;
		}

		return mapIt->second.find(name) != mapIt->second.end();
	}

	template<IsResource T>
	bool Remove(const std::string &name) noexcept
	{
		auto typeIndex = std::type_index(typeid(T));
		auto mapIt = m_resources.find(typeIndex);

		if (mapIt != m_resources.end())
		{
			return mapIt->second.erase(name) > 0;
		}
		return false;
	}

	template<IsResource T>
	void ClearType() noexcept
	{
		auto typeIndex = std::type_index(typeid(T));
		m_resources.erase(typeIndex);
	}

	void ClearAll() noexcept
	{
		m_resources.clear();
	}

private:
	ResourceManager() = default;
	~ResourceManager() = default;

	ResourceManager(const ResourceManager &) = delete;
	ResourceManager &operator=(const ResourceManager &) = delete;

	std::unordered_map<
		std::type_index,
		std::unordered_map<std::string, std::shared_ptr<Resource>>
	> m_resources;
};