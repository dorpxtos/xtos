// clog -- initialize standard log stream
#include <fstream>
#include <iostream>

#ifndef MRTDLL
 #pragma warning(disable: 4074)
 #pragma init_seg(compiler)
static std::_Init_locks  initlocks;
#endif /* MRTDLL */

_STD_BEGIN
		// OBJECT DECLARATIONS

__PURE_APPDOMAIN_GLOBAL static filebuf flog(_cpp_stderr);

 #if defined(_M_CEE_PURE)
__PURE_APPDOMAIN_GLOBAL extern ostream clog(&flog);

 #else /* defined(_M_CEE_PURE) */
__PURE_APPDOMAIN_GLOBAL extern _CRTDATA2 ostream clog(&flog);
 #endif /* defined(_M_CEE_PURE) */

		// INITIALIZATION CODE
struct _Init_clog
	{	// ensures that clog is initialized
	__CLR_OR_THIS_CALL _Init_clog()
		{	// initialize clog
		_Ptr_clog = &clog;
		clog.tie(_Ptr_cout);
		}
	};

__PURE_APPDOMAIN_GLOBAL static _Init_clog init_clog;

_STD_END

/*
 * Copyright (c) by P.J. Plauger. All rights reserved.
 * Consult your license regarding permissions and restrictions.
V6.50:0009 */
