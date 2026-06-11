///////////////////////////////////////////////////////////////////////////////
// $Workfile: stdafx.h $
// $Archive: /ZipArchive/stdafx.h $
// $Date: 02-03-23 14:27 $ $Author: Tadeusz Dracz $
////////////////////////////////////////////////////////////////////////////////
// This source file is part of the ZipArchive library source distribution and
// is Copyright 2000-2003 by Tadeusz Dracz (http://www.artpol-software.com/)
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// 
// For the licensing details see the file License.txt
////////////////////////////////////////////////////////////////////////////////


#if !defined(AFX_STDAFX_H__926F70F4_1B34_49AA_9532_498E8D2F3495__INCLUDED_)
#define AFX_STDAFX_H__926F70F4_1B34_49AA_9532_498E8D2F3495__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define ZIPINLINE inline

#define ZIP_ULONGLONG uint64_t
#define ZIP_LONGLONG int64_t

#include <platform.h>
#include <mfc_compat.h>
#undef MACOS
#undef TARGET_OS_MAC
typedef unsigned char Byte;
//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_STDAFX_H__926F70F4_1B34_49AA_9532_498E8D2F3495__INCLUDED_)
 
