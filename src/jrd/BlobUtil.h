/*
 *  The contents of this file are subject to the Initial
 *  Developer's Public License Version 1.0 (the "License");
 *  you may not use this file except in compliance with the
 *  License. You may obtain a copy of the License at
 *  http://www.ibphoenix.com/main.nfs?a=ibphoenix&page=ibp_idpl.
 *
 *  Software distributed under the License is distributed AS IS,
 *  WITHOUT WARRANTY OF ANY KIND, either express or implied.
 *  See the License for the specific language governing rights
 *  and limitations under the License.
 *
 *  The Original Code was created by Adriano dos Santos Fernandes
 *  for the Firebird Open Source RDBMS project.
 *
 *  Copyright (c) 2020 Adriano dos Santos Fernandes <adrianosf@gmail.com>
 *  and all contributors signed below.
 *
 *  All Rights Reserved.
 *  Contributor(s): ______________________________________.
 */

#ifndef JRD_BLOB_UTIL_H
#define JRD_BLOB_UTIL_H

#include "firebird.h"
#include "firebird/Message.h"
#include "../common/classes/fb_string.h"
#include "../common/classes/ImplementHelper.h"
#include "../common/status.h"
#include "../jrd/SystemPackages.h"

namespace Jrd {


class BlobUtilAppendProcedure :
	public Firebird::DisposeIface<Firebird::IExternalProcedureImpl<BlobUtilAppendProcedure, Firebird::ThrowStatusExceptionWrapper> >
{
public:
	BlobUtilAppendProcedure(Firebird::ThrowStatusExceptionWrapper* status,
		Firebird::IMetadataBuilder* inBuilder, Firebird::IMetadataBuilder* outBuilder);

public:
	void dispose() override
	{
		delete this;
	}

public:
	void getCharSet(Firebird::ThrowStatusExceptionWrapper* status, Firebird::IExternalContext* context,
		char* name, unsigned nameSize) override
	{
		strncpy(name, "UTF8", nameSize);
	}

	Firebird::IExternalResultSet* open(Firebird::ThrowStatusExceptionWrapper* status,
		Firebird::IExternalContext* context, void* inMsg, void* outMsg) override;
};

class BlobUtilCancelProcedure :
	public Firebird::DisposeIface<Firebird::IExternalProcedureImpl<BlobUtilCancelProcedure, Firebird::ThrowStatusExceptionWrapper> >
{
public:
	BlobUtilCancelProcedure(Firebird::ThrowStatusExceptionWrapper* status,
		Firebird::IMetadataBuilder* inBuilder, Firebird::IMetadataBuilder* outBuilder);

public:
	void dispose() override
	{
		delete this;
	}

public:
	void getCharSet(Firebird::ThrowStatusExceptionWrapper* status, Firebird::IExternalContext* context,
		char* name, unsigned nameSize) override
	{
		strncpy(name, "UTF8", nameSize);
	}

	Firebird::IExternalResultSet* open(Firebird::ThrowStatusExceptionWrapper* status,
		Firebird::IExternalContext* context, void* inMsg, void* outMsg) override;
};

class BlobUtilMakeFunction :
	public Firebird::DisposeIface<Firebird::IExternalFunctionImpl<BlobUtilMakeFunction, Firebird::ThrowStatusExceptionWrapper> >
{
public:
	BlobUtilMakeFunction(Firebird::ThrowStatusExceptionWrapper* status,
		Firebird::IMetadataBuilder* inBuilder, Firebird::IMetadataBuilder* outBuilder);

public:
	void dispose() override
	{
		delete this;
	}

public:
	void getCharSet(Firebird::ThrowStatusExceptionWrapper* status, Firebird::IExternalContext* context,
		char* name, unsigned nameSize) override
	{
		strncpy(name, "UTF8", nameSize);
	}

	void execute(Firebird::ThrowStatusExceptionWrapper* status,
		Firebird::IExternalContext* context, void* inMsg, void* outMsg) override;
};

class BlobUtilNewFunction :
	public Firebird::DisposeIface<Firebird::IExternalFunctionImpl<BlobUtilNewFunction, Firebird::ThrowStatusExceptionWrapper> >
{
public:
	BlobUtilNewFunction(Firebird::ThrowStatusExceptionWrapper* status,
		Firebird::IMetadataBuilder* inBuilder, Firebird::IMetadataBuilder* outBuilder);

public:
	void dispose() override
	{
		delete this;
	}

public:
	void getCharSet(Firebird::ThrowStatusExceptionWrapper* status, Firebird::IExternalContext* context,
		char* name, unsigned nameSize) override
	{
		strncpy(name, "UTF8", nameSize);
	}

	void execute(Firebird::ThrowStatusExceptionWrapper* status,
		Firebird::IExternalContext* context, void* inMsg, void* outMsg) override;
};

class BlobUtilOpenFunction :
	public Firebird::DisposeIface<Firebird::IExternalFunctionImpl<BlobUtilOpenFunction, Firebird::ThrowStatusExceptionWrapper> >
{
public:
	BlobUtilOpenFunction(Firebird::ThrowStatusExceptionWrapper* status,
		Firebird::IMetadataBuilder* inBuilder, Firebird::IMetadataBuilder* outBuilder);

public:
	void dispose() override
	{
		delete this;
	}

public:
	void getCharSet(Firebird::ThrowStatusExceptionWrapper* status, Firebird::IExternalContext* context,
		char* name, unsigned nameSize) override
	{
		strncpy(name, "UTF8", nameSize);
	}

	void execute(Firebird::ThrowStatusExceptionWrapper* status,
		Firebird::IExternalContext* context, void* inMsg, void* outMsg) override;
};

class BlobUtilSeekFunction :
	public Firebird::DisposeIface<Firebird::IExternalFunctionImpl<BlobUtilSeekFunction, Firebird::ThrowStatusExceptionWrapper> >
{
public:
	BlobUtilSeekFunction(Firebird::ThrowStatusExceptionWrapper* status,
		Firebird::IMetadataBuilder* inBuilder, Firebird::IMetadataBuilder* outBuilder);

public:
	void dispose() override
	{
		delete this;
	}

public:
	void getCharSet(Firebird::ThrowStatusExceptionWrapper* status, Firebird::IExternalContext* context,
		char* name, unsigned nameSize) override
	{
		strncpy(name, "UTF8", nameSize);
	}

	void execute(Firebird::ThrowStatusExceptionWrapper* status,
		Firebird::IExternalContext* context, void* inMsg, void* outMsg) override;
};

class BlobUtilReadFunction :
	public Firebird::DisposeIface<Firebird::IExternalFunctionImpl<BlobUtilReadFunction, Firebird::ThrowStatusExceptionWrapper> >
{
public:
	BlobUtilReadFunction(Firebird::ThrowStatusExceptionWrapper* status,
		Firebird::IMetadataBuilder* inBuilder, Firebird::IMetadataBuilder* outBuilder);

public:
	void dispose() override
	{
		delete this;
	}

public:
	void getCharSet(Firebird::ThrowStatusExceptionWrapper* status, Firebird::IExternalContext* context,
		char* name, unsigned nameSize) override
	{
		strncpy(name, "UTF8", nameSize);
	}

	void execute(Firebird::ThrowStatusExceptionWrapper* status,
		Firebird::IExternalContext* context, void* inMsg, void* outMsg) override;
};

class BlobUtilPackage : public SystemPackage
{
public:
	BlobUtilPackage(Firebird::MemoryPool& pool);
};


}	// namespace

#endif	// JRD_BLOB_UTIL_H
