//=========
// Block.h
//=========

// ONFI 2.2 - Memory Organization
// A block is the smallest erasable unit of data

#pragma once


//=======
// Using
//=======

#include "Storage/Streams/RandomAccessStream.h"
#include "Storage/Volume.h"
#include "Handle.h"


//===========
// Namespace
//===========

namespace Storage {


//=======
// Block
//=======

class Block: public Object, public Storage::Streams::RandomAccessStream
{
public:
	// Friends
	friend Object;

	// Con-/Destructors
	static Handle<Block> Create(Volume* Volume, UINT Block);

	// Input-Stream
	SIZE_T Available()override;
	SIZE_T Read(VOID* Buffer, SIZE_T Size)override;

	// Output-Stream
	VOID Flush()override;
	SIZE_T Write(VOID const* Buffer, SIZE_T Size)override;

private:
	// Con-/Destructors
	Block(Volume* Volume, UINT Block, BYTE* Buffer, SIZE_T Size);

	// Common
	BYTE* m_Buffer;
	UINT64 m_Offset;
	UINT m_PageSize;
	UINT m_Position;
	UINT m_Size;
	Handle<Volume> m_Volume;
	UINT m_Written;
};

}