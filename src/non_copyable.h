#pragma once

#define _NON_COPYABLE(TypeName) \
	TypeName(const TypeName&) = delete; \
	TypeName& operator=(const TypeName&) = delete; \
	TypeName(TypeName&&) = delete; \
	TypeName& operator=(TypeName&&) = delete;
