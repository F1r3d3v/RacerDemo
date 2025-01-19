#pragma once
#include <concepts>
#include <memory>
#include <string>
#include "Engine/Resource/Resource.h"

template<typename T>
concept IsResource = std::is_base_of_v<Resource, T> &&
std::is_destructible_v<T> &&
	requires(T t)
{
	{
		t.GetName()
	} -> std::convertible_to<const std::string &>;
	{
		t.SetName(std::string())
	} -> std::same_as<void>;
};

template<typename T>
concept IsResourceHandle = std::is_same_v<T, std::shared_ptr<typename T::element_type>> &&
IsResource<typename T::element_type>;