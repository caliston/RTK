// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2004 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#ifndef _RTK_SWI_MESSAGETRANS
#define _RTK_SWI_MESSAGETRANS

namespace rtk {
namespace swi {

const int MessageTrans_FileInfo         =0x41500;
const int MessageTrans_OpenFile         =0x41501;
const int MessageTrans_Lookup           =0x41502;
const int MessageTrans_MakeMenus        =0x41503;
const int MessageTrans_CloseFile        =0x41504;
const int MessageTrans_EnumerateTokens  =0x41505;
const int MessageTrans_ErrorLookup      =0x41506;
const int MessageTrans_GSLookup         =0x41507;
const int MessageTrans_CopyError        =0x41508;
const int MessageTrans_Dictionary       =0x41509;

}; /* namespace swi */
}; /* namespace rtk */

#endif
