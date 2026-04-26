//==============
// Database.cpp
//==============

#include "Database.h"


//===========
// Namespace
//===========

namespace Storage {
	namespace Database {


//==========================
// Con-/Destructors Private
//==========================

Database::Database(Volume* volume, FileCreateMode create):
m_Volume(volume)
{
if(create==FileCreateMode::CreateNew)
	{
	}
}

}}