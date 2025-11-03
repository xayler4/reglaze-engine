#include "pch.h"
#include "core/attribute.h"

namespace rglz {
	Attribute::Attribute() 
		: m_name("")
		, m_value(nullptr)
	{
	}

	Attribute::Attribute(std::string_view name, bool value)
		: m_name(name)
		, m_value(value)

	{
		RGLZ_ASSERT_MSG(is_string_valid(name), "Attribute string must contain only alphanumeric characters or spaces!");
	}

	Attribute::Attribute(std::string_view name, double value)
		: m_name(name)
		, m_value(value)

	{
		RGLZ_ASSERT_MSG(is_string_valid(name), "Attribute string must contain only alphanumeric characters or spaces!");
	}

	Attribute::Attribute(std::string_view name, const char* value)
		: m_name(name)
		, m_value(value)
	{
		RGLZ_ASSERT_MSG(is_string_valid(name) && is_string_valid(value), "Attribute string must contain only alphanumeric characters or spaces!");
	}

	Attribute::Attribute(std::string_view name, std::string_view value)
		: m_name(name)
		, m_value(value)
	{
		RGLZ_ASSERT_MSG(is_string_valid(name) && is_string_valid(value), "Attribute string must contain only alphanumeric characters or spaces");
	}

	Attribute::Attribute(std::string_view name, Attribute::Null)
		: m_name(name)
		, m_value(nullptr)
	{
		RGLZ_ASSERT_MSG(is_string_valid(name), "Attribute string must contain only alphanumeric characters or spaces!");
	}

	Attribute::Attribute(std::string_view name) 
		: m_name(name)
		, m_value(nullptr)
	{
		RGLZ_ASSERT_MSG(is_string_valid(name), "Attribute string must contain only alphanumeric characters or spaces!");
	}
}
