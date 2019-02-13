//////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2018, Image Engine Design Inc. All rights reserved.
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

#ifndef GAFFERSCENE_UDIMQUERY_H
#define GAFFERSCENE_UDIMQUERY_H

#include "GafferScene/Export.h"
#include "GafferScene/TypeIds.h"

#include "GafferScene/FilterPlug.h"
#include "GafferScene/ScenePlug.h"

#include "Gaffer/CompoundDataPlug.h"
#include "Gaffer/ComputeNode.h"
#include "Gaffer/StringPlug.h"

namespace GafferScene
{

class GAFFERSCENE_API UDIMQuery : public Gaffer::ComputeNode
{

	public :

		UDIMQuery( const std::string &name=defaultName<UDIMQuery>() );
		~UDIMQuery() override;

		IE_CORE_DECLARERUNTIMETYPEDEXTENSION( GafferScene::UDIMQuery, UDIMQueryTypeId, Gaffer::ComputeNode );

		GafferScene::ScenePlug *inPlug();
		const GafferScene::ScenePlug *inPlug() const;

		Gaffer::StringPlug *uvSetPlug();
		const Gaffer::StringPlug *uvSetPlug() const;

		Gaffer::StringPlug *attributesPlug();
		const Gaffer::StringPlug *attributesPlug() const;

		GafferScene::FilterPlug *filterPlug();
		const GafferScene::FilterPlug *filterPlug() const;

		Gaffer::CompoundObjectPlug *outPlug();
		const Gaffer::CompoundObjectPlug *outPlug() const;

		void affects( const Gaffer::Plug *input, AffectedPlugsContainer &outputs ) const override;

	protected :

		void hash( const Gaffer::ValuePlug *output, const Gaffer::Context *context, IECore::MurmurHash &h ) const override;
		void compute( Gaffer::ValuePlug *output, const Gaffer::Context *context ) const override;

	private :

		static size_t g_firstPlugIndex;

};

IE_CORE_DECLAREPTR( UDIMQuery );

} // namespace GafferScene

#endif // GAFFERSCENE_UDIMQUERY_H