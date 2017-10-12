//////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2015, Image Engine Design Inc. All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions are
//  met:
//
//      * Redistributions of source code must retain the above
//        copyright notice, this list of conditions and the following
//        disclaimer.
//
//      * Redistributions in binary form must reproduce the above
//        copyright notice, this list of conditions and the following
//        disclaimer in the documentation and/or other materials provided with
//        the distribution.
//
//      * Neither the name of John Haddon nor the names of
//        any other contributors to this software may be used to endorse or
//        promote products derived from this software without specific prior
//        written permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
//  IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
//  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
//  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
//  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
//  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
//  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
//  PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
//  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
//  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
//  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//////////////////////////////////////////////////////////////////////////

#include "boost/python.hpp"

#include "IECorePython/ScopedGILRelease.h"

#include "Gaffer/Context.h"

#include "GafferUIBindings/GadgetBinding.h"

#include "GafferImage/ImagePlug.h"

#include "GafferImageUI/ImageGadget.h"
#include "GafferImageUIBindings/ImageGadgetBinding.h"

using namespace boost::python;
using namespace IECorePython;
using namespace Gaffer;
using namespace GafferUIBindings;
using namespace GafferImage;
using namespace GafferImageUI;

namespace
{

ImagePlugPtr getImage( const ImageGadget &v )
{
	return ImagePlugPtr( const_cast<ImagePlug *>( v.getImage() ) );
}

Imath::V2f pixelAt( const ImageGadget &g, const IECore::LineSegment3f &lineInGadgetSpace )
{
	// Need GIL release because this method may trigger a compute of the format.
	IECorePython::ScopedGILRelease gilRelease;
	return g.pixelAt( lineInGadgetSpace );
}

} // namespace

void GafferImageUIBindings::bindImageGadget()
{
	GadgetClass<ImageGadget>()
		.def( init<>() )
		.def( "setImage", &ImageGadget::setImage )
		.def( "getImage", &getImage )
		.def( "setContext", &ImageGadget::setContext )
		.def( "getContext", (Context *(ImageGadget::*)())&ImageGadget::getContext, return_value_policy<CastToIntrusivePtr>() )
		.def( "setSoloChannel", &ImageGadget::setSoloChannel )
		.def( "getSoloChannel", &ImageGadget::getSoloChannel )
		.def( "pixelAt", &pixelAt )
	;
}