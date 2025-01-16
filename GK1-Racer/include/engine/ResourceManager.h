#pragma once
#include <unordered_map>
#include <memory>
#include <string>
#include <stdexcept>
#include <typeindex>

#include "engine/resource/Resource.h"
#include "engine/Concepts.h"

class ResourceManager {
public:
	static ResourceManager &GetInstance();

	template<IsResource T>
	void Add(const std::string &name, std::shared_ptr<T> resource);

	template<IsResource T, typename... Args>
		requires std::constructible_from<T, Args...>
	std::shared_ptr<T> Create(const std::string &name, Args&&... args);

	template<IsResource T>
	std::shared_ptr<T> Get(const std::string &name);

	template<IsResource T>
	std::shared_ptr<T> TryGet(const std::string &name) noexcept;

	template<IsResource T>
	bool Exists(const std::string &name) const noexcept;

	template<IsResource T>
	bool Remove(const std::string &name) noexcept;

	template<IsResource T>
	void ClearType() noexcept;

	void ClearAll() noexcept;

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