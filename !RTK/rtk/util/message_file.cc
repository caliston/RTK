// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2004 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#include "rtk/os/messagetrans.h"
#include "rtk/util/message_file.h"

namespace rtk {
namespace util {

message_file::message_file(const string& pathname):
	_descriptor(0),
	_buffer(0)
{
	try
	{
		unsigned int size=0;
		os::MessageTrans_FileInfo(pathname.c_str(),0,&size);
		_buffer=new char[size];
		os::MessageTrans_OpenFile(&_descriptor,pathname.c_str(),_buffer);
	}
	catch (...)
	{
		delete[] _buffer;
		throw;
	}
}

message_file::~message_file()
{
	os::MessageTrans_CloseFile(&_descriptor);
	delete[] _buffer;
}

string message_file::lookup(const string& token)
{
	const char* message=0;
	unsigned int length=0;
	os::MessageTrans_Lookup(&_descriptor,token.c_str(),0,0,
		0,0,0,0,0,&message,&length);
	return string(message,message+length);
}

string message_file::lookup(const string& token,const string& arg0,
	const string& arg1,const string& arg2,const string& arg3,
	unsigned int args)
{
	unsigned int length=0;
	os::MessageTrans_Lookup(&_descriptor,token.c_str(),0,0,
		0,0,0,0,0,0,&length);
	unsigned int arg0_len=(args>0)?arg0.size():0;
	unsigned int arg1_len=(args>1)?arg1.size():0;
	unsigned int arg2_len=(args>2)?arg2.size():0;
	unsigned int arg3_len=(args>3)?arg3.size():0;
	unsigned int size=length+1+arg0_len+arg1_len+arg2_len+arg3_len;
	char buffer[size];
	const char* message=0;
	length=0;
	const char* arg0_cstr=(args>0)?arg0.c_str():0;
	const char* arg1_cstr=(args>1)?arg1.c_str():0;
	const char* arg2_cstr=(args>2)?arg2.c_str():0;
	const char* arg3_cstr=(args>3)?arg3.c_str():0;
	os::MessageTrans_Lookup(&_descriptor,token.c_str(),buffer,size,
		arg0_cstr,arg1_cstr,arg2_cstr,arg3_cstr,0,&message,&length);
	return string(message,message+length);
}

string message_file::lookup(const string& token,const string& arg0)
{
	return lookup(token,arg0,string(),string(),string(),1);
}

string message_file::lookup(const string& token,const string& arg0,
	const string& arg1)
{
	return lookup(token,arg0,arg1,string(),string(),2);
}

string message_file::lookup(const string& token,const string& arg0,
	const string& arg1,const string& arg2)
{
	return lookup(token,arg0,arg1,arg2,string(),3);
}

string message_file::lookup(const string& token,const string& arg0,
	const string& arg1,const string& arg2,const string& arg3)
{
	return lookup(token,arg0,arg1,arg2,arg3,4);
}

}; /* namespace util */
}; /* namespace rtk */
