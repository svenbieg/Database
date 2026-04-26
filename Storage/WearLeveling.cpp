//==================
// WearLeveling.cpp
//==================

#include "WearLeveling.h"


//=======
// Using
//=======

#include "Storage/Block.h"
#include "Storage/Buffer.h"


//===========
// Namespace
//===========

namespace Storage {


//========
// Volume
//========

VOID WearLeveling::Erase(UINT64 offset, UINT size)
{
UINT64 redir=Redirect(offset);
while(1)
	{
	try
		{
		m_Volume->Erase(redir, size);
		}
	catch(ErrorException e)
		{
		redir=Spare(offset);
		}
	}
}

UINT WearLeveling::GetBlockSize()
{
return m_BlockSize;
}

UINT WearLeveling::GetPageSize()
{
return m_PageSize;
}

UINT64 WearLeveling::GetSize()
{
return m_Size;
}

VOID WearLeveling::Read(UINT64 offset, VOID* buf, SIZE_T size)
{
offset=Redirect(offset);
m_Volume->Read(offset, buf, size);
}

VOID WearLeveling::SetSize(UINT64 size)
{
if(size>m_Size)
	throw OutOfMemoryException();
}

VOID WearLeveling::Write(UINT64 offset, VOID const* buf, SIZE_T size)
{
UINT64 redir=Redirect(offset);
while(1)
	{
	try
		{
		m_Volume->Write(redir, buf, size);
		}
	catch(ErrorException e)
		{
		UINT64 spare=Spare(offset);
		UINT block_pos=redir%m_BlockSize;
		if(block_pos>0)
			{
			UINT src=redir/m_BlockSize;
			UINT dst=spare/m_BlockSize;
			auto buf=Buffer::Create(m_PageSize);
			auto buf_ptr=buf->Begin();
			for(UINT pos=0; pos<block_pos; )
				{
				UINT copy=TypeHelper::Min(block_pos-pos, m_PageSize);
				m_Volume->Read(redir+pos, buf_ptr, copy);
				m_Volume->Write(spare+pos, buf_ptr, copy);
				pos+=copy;
				}
			}
		redir=spare;
		}
	}
}


//==========================
// Con-/Destructors Private
//==========================

WearLeveling::WearLeveling(Volume* volume, UINT spare):
m_BlockSize(volume->GetBlockSize()),
m_Count(1),
m_PageSize(volume->GetPageSize()),
m_Position(0),
m_Size(volume->GetSize()),
m_Spare(spare),
m_Volume(volume)
{
m_Size-=m_Spare*m_BlockSize;
auto block=Block::Create(volume, 0);
SIZE_T pos=0;
UINT entry[2];
while(m_Count<=m_Spare)
	{
	pos+=block->Read(entry, 2*sizeof(UINT));
	if(entry[0]==-1)
		break;
	if(entry[1]!='RDIR')
		{
		if(pos>2*sizeof(UINT))
			throw AbortException();
		volume->Erase(0, m_BlockSize);
		return;
		}
	m_Redirect.set(entry[0], m_Count++);
	m_Position=(UINT)pos;
	}
}


//================
// Common Private
//================

UINT64 WearLeveling::Redirect(UINT64 offset)
{
offset+=m_Spare*m_BlockSize;
UINT block=offset/m_BlockSize;
UINT redir=0;
if(m_Redirect.try_get(block, &redir))
	{
	UINT block_pos=offset%m_BlockSize;
	offset=(UINT64)redir*m_BlockSize+block_pos;
	}
return offset;
}

UINT64 WearLeveling::Spare(UINT64 offset)
{
if(m_Count==m_Spare)
	throw AbortException();
offset+=m_Spare*m_BlockSize;
UINT block=offset/m_BlockSize;
UINT redir=m_Count++;
m_Redirect.set(block, redir);
UINT entry[2];
entry[0]=block;
entry[1]='RDIR';
m_Volume->Write(m_Position, entry, 2*sizeof(UINT));
m_Position+=2*sizeof(UINT);
UINT block_pos=offset%m_BlockSize;
return (UINT64)redir*m_BlockSize+block_pos;
}

}