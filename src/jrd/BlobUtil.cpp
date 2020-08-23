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

#include "firebird.h"
#include "../jrd/BlobUtil.h"
#include "../jrd/blb.h"
#include "../jrd/tra.h"

using namespace Jrd;
using namespace Firebird;


namespace
{
	blb* getBlobFromHandle(thread_db* tdbb, ISC_INT64 handle)
	{
		const auto transaction = tdbb->getTransaction();
		blb* blob;

		if (transaction->tra_blob_util_map.get(handle, blob))
			return blob;
		else
		{
			status_exception::raise(Arg::Gds(isc_invalid_blob_util_handle));
			return nullptr;
		}
	}

	FB_MESSAGE(BlobUtilBlobMessage, ThrowStatusExceptionWrapper,
		(FB_BLOB, blob)
	);

	FB_MESSAGE(BlobUtilStringMessage, ThrowStatusExceptionWrapper,
		(FB_INTL_VARCHAR(MAX_VARY_COLUMN_SIZE, 0), data)
	);

	FB_MESSAGE(BlobUtilHandleMessage, ThrowStatusExceptionWrapper,
		(FB_INTEGER, handle)
	);

	FB_MESSAGE(BlobUtilNewInput, ThrowStatusExceptionWrapper,
		(FB_BOOLEAN, segmented)
		(FB_BOOLEAN, tempStorage)
	);

	FB_MESSAGE(BlobUtilSeekInput, ThrowStatusExceptionWrapper,
		(FB_INTEGER, handle)
		(FB_INTEGER, mode)
		(FB_INTEGER, offset)
	);

	FB_MESSAGE(BlobUtilSeekOutput, ThrowStatusExceptionWrapper,
		(FB_INTEGER, offset)
	);

	FB_MESSAGE(BlobUtilAppendInput, ThrowStatusExceptionWrapper,
		(FB_INTEGER, handle)
		(FB_INTL_VARCHAR(MAX_VARY_COLUMN_SIZE, 0), data)
	);

	FB_MESSAGE(BlobUtilReadInput, ThrowStatusExceptionWrapper,
		(FB_INTEGER, handle)
		(FB_INTEGER, length)
	);
}

namespace Jrd {

//--------------------------------------

BlobUtilAppendProcedure::BlobUtilAppendProcedure(ThrowStatusExceptionWrapper* status,
	IMetadataBuilder* inBuilder, IMetadataBuilder* outBuilder)
{
	BlobUtilAppendInput::setup(status, inBuilder);
}

IExternalResultSet* BlobUtilAppendProcedure::open(ThrowStatusExceptionWrapper* status,
	IExternalContext* context, void* inMsg, void* outMsg)
{
	const auto in = static_cast<BlobUtilAppendInput::Type*>(inMsg);

	const auto tdbb = JRD_get_thread_data();
	Attachment::SyncGuard guard(tdbb->getAttachment(), FB_FUNCTION);

	const auto blob = getBlobFromHandle(tdbb, in->handle);

	if (in->data.length > 0)
		blob->BLB_put_data(tdbb, (const UCHAR*) in->data.str, in->data.length);
	else if (in->data.length == 0 && !(blob->blb_flags & BLB_stream))
		blob->BLB_put_segment(tdbb, (const UCHAR*) in->data.str, 0);

	return nullptr;
}

//--------------------------------------

BlobUtilCancelProcedure::BlobUtilCancelProcedure(ThrowStatusExceptionWrapper* status,
	IMetadataBuilder* inBuilder, IMetadataBuilder* outBuilder)
{
	BlobUtilHandleMessage::setup(status, inBuilder);
}

IExternalResultSet* BlobUtilCancelProcedure::open(ThrowStatusExceptionWrapper* status,
	IExternalContext* context, void* inMsg, void* outMsg)
{
	const auto in = static_cast<BlobUtilAppendInput::Type*>(inMsg);

	const auto tdbb = JRD_get_thread_data();
	Attachment::SyncGuard guard(tdbb->getAttachment(), FB_FUNCTION);

	const auto transaction = tdbb->getTransaction();
	const auto blob = getBlobFromHandle(tdbb, in->handle);

	transaction->tra_blob_util_map.remove(in->handle);
	blob->BLB_cancel(tdbb);

	return nullptr;
}

//--------------------------------------

BlobUtilMakeFunction::BlobUtilMakeFunction(ThrowStatusExceptionWrapper* status,
	IMetadataBuilder* inBuilder, IMetadataBuilder* outBuilder)
{
	BlobUtilHandleMessage::setup(status, inBuilder);
	BlobUtilBlobMessage::setup(status, outBuilder);
}

void BlobUtilMakeFunction::execute(ThrowStatusExceptionWrapper* status,
	IExternalContext* context, void* inMsg, void* outMsg)
{
	const auto in = static_cast<BlobUtilHandleMessage::Type*>(inMsg);
	auto out = static_cast<BlobUtilBlobMessage::Type*>(outMsg);

	const auto tdbb = JRD_get_thread_data();
	Attachment::SyncGuard guard(tdbb->getAttachment(), FB_FUNCTION);

	const auto transaction = tdbb->getTransaction();

	const auto blob = getBlobFromHandle(tdbb, in->handle);

	if (!(blob->blb_flags & BLB_temporary))
		ERR_post(Arg::Gds(isc_cannot_make_blob_opened_handle));

	out->blobNull = FB_FALSE;
	out->blob.gds_quad_low = (ULONG) blob->getTempId();
	out->blob.gds_quad_high = ((FB_UINT64) blob->getTempId()) >> 32;

	transaction->tra_blob_util_map.remove(in->handle);
	blob->BLB_close(tdbb);
}

//--------------------------------------

BlobUtilNewFunction::BlobUtilNewFunction(ThrowStatusExceptionWrapper* status,
	IMetadataBuilder* inBuilder, IMetadataBuilder* outBuilder)
{
	BlobUtilNewInput::setup(status, inBuilder);
	BlobUtilHandleMessage::setup(status, outBuilder);
}

void BlobUtilNewFunction::execute(ThrowStatusExceptionWrapper* status,
	IExternalContext* context, void* inMsg, void* outMsg)
{
	const auto in = static_cast<BlobUtilNewInput::Type*>(inMsg);
	auto out = static_cast<BlobUtilHandleMessage::Type*>(outMsg);

	thread_db* tdbb = JRD_get_thread_data();
	Attachment::SyncGuard guard(tdbb->getAttachment(), FB_FUNCTION);

	const auto transaction = tdbb->getTransaction();

	const UCHAR bpb[] = {
		isc_bpb_version1,
		isc_bpb_type, 1, UCHAR(in->segmented ? isc_bpb_type_segmented : isc_bpb_type_stream),
		isc_bpb_storage, 1, UCHAR(in->tempStorage ? isc_bpb_storage_temp : isc_bpb_storage_main)
	};

	bid id;
	blb* newBlob = blb::create2(tdbb, transaction, &id, sizeof(bpb), bpb);

	transaction->tra_blob_util_map.put(++transaction->tra_blob_util_next, newBlob);

	out->handleNull = FB_FALSE;
	out->handle = transaction->tra_blob_util_next;
}

//--------------------------------------

BlobUtilOpenFunction::BlobUtilOpenFunction(ThrowStatusExceptionWrapper* status,
	IMetadataBuilder* inBuilder, IMetadataBuilder* outBuilder)
{
	BlobUtilBlobMessage::setup(status, inBuilder);
	BlobUtilHandleMessage::setup(status, outBuilder);
}

void BlobUtilOpenFunction::execute(ThrowStatusExceptionWrapper* status,
	IExternalContext* context, void* inMsg, void* outMsg)
{
	const auto in = static_cast<BlobUtilBlobMessage::Type*>(inMsg);
	auto out = static_cast<BlobUtilHandleMessage::Type*>(outMsg);

	const auto tdbb = JRD_get_thread_data();
	Attachment::SyncGuard guard(tdbb->getAttachment(), FB_FUNCTION);

	const auto transaction = tdbb->getTransaction();

	bid blobId = *(bid*) &in->blob;
	blb* blob = blb::open(tdbb, transaction, &blobId);

	transaction->tra_blob_util_map.put(++transaction->tra_blob_util_next, blob);

	out->handleNull = FB_FALSE;
	out->handle = transaction->tra_blob_util_next;
}


//--------------------------------------

BlobUtilSeekFunction::BlobUtilSeekFunction(ThrowStatusExceptionWrapper* status,
	IMetadataBuilder* inBuilder, IMetadataBuilder* outBuilder)
{
	BlobUtilSeekInput::setup(status, inBuilder);
	BlobUtilSeekOutput::setup(status, outBuilder);
}

void BlobUtilSeekFunction::execute(ThrowStatusExceptionWrapper* status,
	IExternalContext* context, void* inMsg, void* outMsg)
{
	const auto in = static_cast<BlobUtilSeekInput::Type*>(inMsg);
	auto out = static_cast<BlobUtilSeekOutput::Type*>(outMsg);

	const auto tdbb = JRD_get_thread_data();
	Attachment::SyncGuard guard(tdbb->getAttachment(), FB_FUNCTION);

	const auto transaction = tdbb->getTransaction();
	const auto blob = getBlobFromHandle(tdbb, in->handle);

	if (!(in->mode >= 0 && in->mode <= 2))
		status_exception::raise(Arg::Gds(isc_random) << "Seek mode must be 1, 2 or 3");

	out->offsetNull = FB_FALSE;
	out->offset = blob->BLB_lseek(in->mode, in->offset);
}

//--------------------------------------

BlobUtilReadFunction::BlobUtilReadFunction(ThrowStatusExceptionWrapper* status,
	IMetadataBuilder* inBuilder, IMetadataBuilder* outBuilder)
{
	BlobUtilReadInput::setup(status, inBuilder);
	BlobUtilStringMessage::setup(status, outBuilder);
}

void BlobUtilReadFunction::execute(ThrowStatusExceptionWrapper* status,
	IExternalContext* context, void* inMsg, void* outMsg)
{
	const auto in = static_cast<BlobUtilReadInput::Type*>(inMsg);
	auto out = static_cast<BlobUtilStringMessage::Type*>(outMsg);

	if (!in->lengthNull && in->length <= 0)
		status_exception::raise(Arg::Gds(isc_random) << "Length must be NULL or greater than 0");

	const auto tdbb = JRD_get_thread_data();
	Attachment::SyncGuard guard(tdbb->getAttachment(), FB_FUNCTION);

	const auto transaction = tdbb->getTransaction();
	const auto blob = getBlobFromHandle(tdbb, in->handle);

	if (in->lengthNull)
		out->data.length = blob->BLB_get_segment(tdbb, (UCHAR*) out->data.str, sizeof(out->data.str));
	else
	{
		out->data.length = blob->BLB_get_data(tdbb, (UCHAR*) out->data.str,
			MIN(in->length, sizeof(out->data.str)), false);
	}

	out->dataNull = out->data.length == 0 && (blob->blb_flags & BLB_eof) ? FB_TRUE : FB_FALSE;
}

//--------------------------------------

BlobUtilPackage::BlobUtilPackage(Firebird::MemoryPool& pool)
	: SystemPackage(
		pool,
		"RDB$BLOB_UTIL",
		ODS_13_0,
		// procedures
		{
			SystemProcedure(
				pool,
				"APPEND",
				SystemRoutineFactory<BlobUtilAppendProcedure>(),
				prc_executable,
				// input parameters
				{
					{"HANDLE", fld_long_number, false},
					{"DATA", fld_varybinary_max, false}
				},
				// output parameters
				{}
			),
			SystemProcedure(
				pool,
				"CANCEL",
				SystemRoutineFactory<BlobUtilCancelProcedure>(),
				prc_executable,
				// input parameters
				{
					{"HANDLE", fld_long_number, false},
				},
				// output parameters
				{}
			)
		},
		// functions
		{
			SystemFunction(
				pool,
				"MAKE_BLOB",
				SystemRoutineFactory<BlobUtilMakeFunction>(),
				// parameters
				{
					{"HANDLE", fld_long_number, false}
				},
				{fld_blob, false}
			),
			SystemFunction(
				pool,
				"NEW",
				SystemRoutineFactory<BlobUtilNewFunction>(),
				// parameters
				{
					{"SEGMENTED", fld_bool, false},
					{"TEMP_STORAGE", fld_bool, false}
				},
				{fld_long_number, false}
			),
			SystemFunction(
				pool,
				"OPEN_BLOB",
				SystemRoutineFactory<BlobUtilOpenFunction>(),
				// parameters
				{
					{"BLOB", fld_blob, false}
				},
				{fld_long_number, false}
			),
			SystemFunction(
				pool,
				"SEEK",
				SystemRoutineFactory<BlobUtilSeekFunction>(),
				// parameters
				{
					{"HANDLE", fld_long_number, false},
					{"MODE", fld_long_number, false},
					{"OFFSET", fld_long_number, false}
				},
				{fld_long_number, false}
			),
			SystemFunction(
				pool,
				"READ",
				SystemRoutineFactory<BlobUtilReadFunction>(),
				// parameters
				{
					{"HANDLE", fld_long_number, false},
					{"LENGTH", fld_long_number, true}
				},
				{fld_varybinary_max, true}
			)
		}
	)
{
}

}	// namespace Jrd
