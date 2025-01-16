#pragma once
#include <string>

class Resource {
public:
	Resource() = default;
	virtual ~Resource() = default;

	const std::string &GetName() const
	{
		return m_name;
	}
	void SetName(const std::string &name)
	{
		m_name = name;
	}

protected:
	std::string m_name;
};