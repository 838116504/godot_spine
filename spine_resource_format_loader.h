#pragma once

#include "core/io/resource_loader.h"

class ResourceFormatLoaderSpineSkel : public ResourceFormatLoader
{
public:
	virtual RES load(const String& p_path, const String& p_original_path, Error* p_err);
	virtual void get_recognized_extensions(List<String>* p_extensions) const;
	virtual bool handles_type(const String& p_type) const;
	virtual String get_resource_type(const String& p_path) const;
};

class ResourceFormatLoaderSpineAtlas : public ResourceFormatLoader
{
public:
	virtual RES load(const String& p_path, const String& p_original_path, Error* p_err);
	virtual void get_recognized_extensions(List<String>* p_extensions) const;
	virtual bool handles_type(const String& p_type) const;
	virtual String get_resource_type(const String& p_path) const;
};
