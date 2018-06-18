#pragma once
#include "targetver.h"
#include <Windows.h>
#include <memory>

struct sc_handle_deleter
{
	typedef SC_HANDLE pointer;
	void operator()(SC_HANDLE handle) const noexcept
	{ 
		if (handle != nullptr)
		{
			::CloseServiceHandle(handle);
		}
	}
};

typedef std::unique_ptr<SC_HANDLE, sc_handle_deleter> unique_sc_handle;

struct hkey_deleter
{
	typedef HKEY pointer;
	void operator()(HKEY handle) const noexcept
	{
		if (handle != nullptr)
		{
			::RegCloseKey(handle);
		}
	}
};

typedef std::unique_ptr<HKEY, hkey_deleter> unique_hkey;

struct event_source_handle_deleter
{
	typedef HANDLE pointer;
	void operator()(HANDLE handle) const noexcept
	{
		if (handle != nullptr)
		{
			::DeregisterEventSource(handle);
		}
	}
};

typedef std::unique_ptr<HANDLE, event_source_handle_deleter> unique_event_source_handle;

struct handle_deleter
{
	typedef HANDLE pointer;
	void operator()(HANDLE handle) const noexcept
	{
		if (handle != nullptr)
		{
			::CloseHandle(handle);
		}
	}
};

typedef std::unique_ptr<HANDLE, handle_deleter> unique_handle;

struct hlocal_deleter
{
	typedef HLOCAL pointer;
	void operator()(HLOCAL handle) const noexcept
	{
		if (handle != nullptr)
		{
			::LocalFree(handle);
		}
	}
};

typedef std::unique_ptr<HLOCAL, hlocal_deleter> unique_hlocal;
