/******************************************************************************
 * Spine Runtimes License Agreement
 * Last updated January 1, 2020. Replaces all prior versions.
 *
 * Copyright (c) 2013-2020, Esoteric Software LLC
 *
 * Integration of the Spine Runtimes into software or otherwise creating
 * derivative works of the Spine Runtimes is permitted under the terms and
 * conditions of Section 2 of the Spine Editor License Agreement:
 * http://esotericsoftware.com/spine-editor-license
 *
 * Otherwise, it is permitted to integrate the Spine Runtimes into software
 * or otherwise create derivative works of the Spine Runtimes (collectively,
 * "Products"), provided that each user of the Products must obtain their own
 * Spine Editor license and redistribution of the Products in any form must
 * include this license and copyright notice.
 *
 * THE SPINE RUNTIMES ARE PROVIDED BY ESOTERIC SOFTWARE LLC "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL ESOTERIC SOFTWARE LLC BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES,
 * BUSINESS INTERRUPTION, OR LOSS OF USE, DATA, OR PROFITS) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THE SPINE RUNTIMES, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *****************************************************************************/
#ifdef TOOLS_ENABLED
#include "spine_resource_format_loader.h"
#include "spine.h"

RES ResourceFormatLoaderSpineSkel::load(const String &p_path, const String& p_original_path, Error *p_err)
{
	auto extName = p_path.get_extension();
	if (extName == "json")
	{
		Ref<SpineJsonSkeleton> ref = memnew(SpineJsonSkeleton);
		if (p_err)
			*p_err = OK;

		ref->set_path(p_path);

		return ref;
	}
	else
	{
		Ref<SpineBinarySkeleton> ref = memnew(SpineBinarySkeleton);
		if (p_err)
			*p_err = OK;
		ref->set_path(p_path);

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

#endif // TOOLS_ENABLED

