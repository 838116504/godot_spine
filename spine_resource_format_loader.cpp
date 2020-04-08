/******************************************************************************
 * Spine Runtimes Software License v2.5
 *
 * Copyright (c) 2013-2016, Esoteric Software
 * All rights reserved.
 *
 * You are granted a perpetual, non-exclusive, non-sublicensable, and
 * non-transferable license to use, install, execute, and perform the Spine
 * Runtimes software and derivative works solely for personal or internal
 * use. Without the written permission of Esoteric Software (see Section 2 of
 * the Spine Software License Agreement), you may not (a) modify, translate,
 * adapt, or develop new applications using the Spine Runtimes or otherwise
 * create derivative works or improvements of the Spine Runtimes or (b) remove,
 * delete, alter, or obscure any trademarks or any copyright, trademark, patent,
 * or other intellectual property or proprietary rights notices on or in the
 * Software, including any copy thereof. Redistributions in binary or source
 * form must include this license and terms.
 *
 * THIS SOFTWARE IS PROVIDED BY ESOTERIC SOFTWARE "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL ESOTERIC SOFTWARE BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES, BUSINESS INTERRUPTION, OR LOSS OF
 * USE, DATA, OR PROFITS) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
 * IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *****************************************************************************/
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
