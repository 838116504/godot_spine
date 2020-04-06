#include "spine_resource_format_loader.h"
#include "spine.h"

RES ResourceFormatLoaderSpineSkel::load(const String &p_path, const String& p_original_path, Error *p_err)
{
	auto extName = p_path.get_extension();
	if (extName == "json")
	{
		Ref<SpineJsonSkeleton> ref = memnew(SpineJsonSkeleton);
		//print_line("this is json");
		if (p_err)
			*p_err = OK;
		
		//String atlasPath = p_path.get_basename() + ".atlas";
		//res->atlas = load(atlasPath, p_original_path, p_err);
		ref->set_path(p_path);
		//res->load();

		return ref;
	}
	else
	{
		Ref<SpineBinarySkeleton> ref = memnew(SpineBinarySkeleton);
		//print_line("this is binary");
		if (p_err)
			*p_err = OK;
		//String atlasPath = p_path.get_basename() + ".atlas";
		//res->atlas = load(atlasPath, p_original_path, p_err);
		ref->set_path(p_path);
		//res->load();
		
		return ref;
	}
}

void ResourceFormatLoaderSpineSkel::get_recognized_extensions(List<String> *p_extensions) const
{
	if (!p_extensions->find("skel"))
		p_extensions->push_back("skel");
	if (!p_extensions->find("json"))
		p_extensions->push_back("json");
}

bool ResourceFormatLoaderSpineSkel::handles_type(const String& p_type) const
{
	return ClassDB::is_parent_class(p_type, "SpineSkeleton");
}

String ResourceFormatLoaderSpineSkel::get_resource_type(const String &p_path) const
{
	String extName = p_path.get_extension().to_lower();
	if (extName == "json")
		return "SpineJsonSkeleton";
	else if (extName == "skel")
		return "SpineBinarySkeleton";
	return "";
}


RES ResourceFormatLoaderSpineAtlas::load(const String &p_path, const String& p_original_path, Error *p_err)
{
	Ref<SpineAtlas> ref = memnew(SpineAtlas);
	//print_line("this is atlas");
	if (p_err)
		*p_err = OK;
	ref->set_path(p_path);
	ref->load();
	ERR_FAIL_COND_V(ref->data == NULL, RES());
	return ref;
}

void ResourceFormatLoaderSpineAtlas::get_recognized_extensions(List<String> *p_extensions) const
{
	if (!p_extensions->find("atlas"))
		p_extensions->push_back("atlas");
}

bool ResourceFormatLoaderSpineAtlas::handles_type(const String& p_type) const
{
	return p_type == "SpineAtlas";
}

String ResourceFormatLoaderSpineAtlas::get_resource_type(const String &p_path) const
{
	String extName = p_path.get_extension().to_lower();
	if (extName == "atlas")
		return "SpineAtlas";
	return "";
}
