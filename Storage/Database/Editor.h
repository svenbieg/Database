//==========
// Editor.h
//==========

#pragma once


//=======
// Using
//=======

#include "Storage/Database/Node.h"


//===========
// Namespace
//===========

namespace Storage {
	namespace Database {


//========
// Editor
//========

class Editor: public Object
{
public:
	// Con-/Destructors
	~Editor();
	static inline Handle<Editor> Create(Database* Database) { return new Editor(Database); }

	// Common
	UINT AllocateBlock();
	VOID Cancel();
	VOID Flush();
	VOID FreeBlock(UINT Block);

private:
	// Con-/Destructors
	Editor(Database* Database);

	// Common
	Handle<Database> m_Database;
};

}}