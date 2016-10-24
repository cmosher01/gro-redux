#pragma once

typedef
	enum reg_tag
	{
		not_registered,
		registered_good,
		registered_bad
	}
	reg_t;

inline reg_t GetRegistration()
{
	return registered_good;
}
