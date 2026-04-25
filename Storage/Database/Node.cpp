//==========
// Node.cpp
//==========

#include "Node.h"


//=======
// Using
//=======

#include "Storage/Database/Database.h"


//===========
// Namespace
//===========

namespace Storage {
	namespace Database {


//============================
// Con-/Destructors Protected
//============================

Node::Node(Database* database, UINT block):
m_Database(database),
m_Block(block),
m_Read(0),
m_Written(0)
{}

}}