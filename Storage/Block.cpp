//===========
// Block.cpp
//===========

// ONFI 2.2 - Memory Organization
// A block is the smallest erasable unit of data

#include "Block.h"


//=======
// Using
//=======

#include "MemoryHelper.h"


//===========
// Namespace
//===========

namespace Storage {


//==================
// Con-/Destructors
//==================

Handle<Block> Block::Create(Volume* volume, UINT block)
{
UINT page_size=volume->GetPageSize();
return Object::CreateEx<Block, Volume*>(page_size, 4, volume, block);
}


//==============
// Input-Stream
//==============

SIZE_T Block::Available()
{
return m_Size-m_Position;
}

SIZE_T Block::Read(VOID* buf, SIZE_T size)
{
BYTE* dst=(BYTE*)buf;
SIZE_T read=0;
while(read<size)
	{
	UINT page_pos=m_Position%m_PageSize;
	if(page_pos==0)
		m_Volume->Read(m_Offset+m_Position, m_Buffer, m_PageSize);
	UINT available=m_PageSize-page_pos;
	SIZE_T copy=TypeHelper::Min(size-read, available);
	MemoryHelper::Copy(&dst[read], &m_Buffer[page_pos], copy);
	m_Position+=copy;
	read+=copy;
	}
return read;
}


//===============
// Output-Stream
//===============

VOID Block::Flush()
{
if(!m_Written)
	return;
if(m_Position==0)
	m_Volume->Erase(m_Offset, m_Size);
UINT page_pos=m_Position%m_PageSize;
m_Volume->Write(m_Offset+m_Position, &m_Buffer[page_pos], m_Written);
m_Position+=m_Written;
m_Written=0;
}

SIZE_T Block::Write(VOID const* buf, SIZE_T size)
{
BYTE* src=(BYTE*)buf;
SIZE_T written=0;
while(written<size)
	{
	UINT page_pos=(m_Position+m_Written)%m_PageSize;
	if(page_pos==0)
		Flush();
	UINT available=m_PageSize-page_pos;
	SIZE_T copy=TypeHelper::Min(size-written, available);
	MemoryHelper::Copy(&m_Buffer[page_pos], &src[written], copy);
	m_Written+=copy;
	written+=copy;
	}
return written;
}


//==========================
// Con-/Destructors Private
//==========================

Block::Block(Volume* vol, UINT block, BYTE* buf, SIZE_T size):
RandomAccessStream(StreamFormat::UTF8),
m_Buffer(buf),
m_PageSize(size),
m_Position(0),
m_Size(vol->GetBlockSize()),
m_Volume(vol),
m_Written(0)
{
m_Offset=(UINT64)block*m_Size;
}

}