#include "Engine/ResourceManager.h"
#include <stdexcept>

ResourceManager &ResourceManager::GetInstance()
{
	static ResourceManager instance;
	return instance;
}

//template<IsResource T>
//void ResourceManager::Add(const std::string &name, std::shared_ptr<T> resource)
//{
//	auto typeIndex = std::type_index(typeid(T));
//	auto &resourceMap = m_resources[typeIndex];
//
//	if (resourceMap.find(name) != resourceMap.end())
//	{
//		throw std::runtime_error("Resource '" + name + "' already exists!");
//	}
//
//	resourceMap[name] = resource;
//}

//template<IsResource T, typename... Args>
//	requires std::constructible_from<T, Args...>
//std::shared_ptr<T> ResourceManager::Create(const std::string &name, Args&&... args)
//{
//	auto resource = std::make_shared<T>(std::forward<Args>(args)...);
//	resource->SetName(name);
//	Add<T>(name, resource);
//	return resource;
//}

//template<IsResource T>
//std::shared_ptr<T> ResourceManager::Get(const std::string &name)
//{
//	auto typeIndex = std::type_index(typeid(T));
//	auto mapIt = m_resources.find(typeIndex);
//
//	if (mapIt == m_resources.end())
//	{
//		throw std::runtime_error("No resources of type " + std::string(typeid(T).name()));
//	}
//
//	auto &resourceMap = mapIt->second;
//	auto resourceIt = resourceMap.find(name);
//
//	if (resourceIt == resourceMap.end())
//	{
//		throw std::runtime_error("Resource '" + name + "' not found!");
//	}
//
//	return std::static_pointer_cast<T>(resourceIt->second);
//}

//template<IsResource T>
//std::shared_ptr<T> ResourceManager::TryGet(const std::string &name) noexcept
//{
//	try
//	{
//		return Get<T>(name);
//	}
//	catch (...)
//	{
//		return nullptr;
//	}
//}

//template<IsResource T>
//bool ResourceManager::Exists(const std::string &name) const noexcept
//{
//	auto typeIndex = std::type_index(typeid(T));
//	auto mapIt = m_resources.find(typeIndex);
//
//	if (mapIt == m_resources.end())
//	{
//		return false;
//	}
//
//	return mapIt->second.find(name) != mapIt->second.end();
//}

//template<IsResource T>
//bool ResourceManager::Remove(const std::string &name) noexcept
//{
//	auto typeIndex = std::type_index(typeid(T));
//	auto mapIt = m_resources.find(typeIndex);
//
//	if (mapIt != m_resources.end())
//	{
//		return mapIt->second.erase(name) > 0;
//	}
//	return false;
//}

//template<IsResource T>
//void ResourceManager::ClearType() noexcept
//{
//	auto typeIndex = std::type_index(typeid(T));
//	m_resources.erase(typeIndex);
//}

//void ResourceManager::ClearAll() noexcept
//{
//	m_resources.clear();
//}