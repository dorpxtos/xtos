// cerr -- initialize standard error stream
#include <fstream>
#include <iostream>

#pragma warning(disable: 4074)
#pragma init_seg(compiler)
static std::_Init_locks  initlocks;

_STD_BEGIN
		// OBJECT DECLARATIONS

__PURE_APPDOMAIN_GLOBAL static filebuf ferr(_cpp_stderr);

 #if defined(_M_CEE_PURE)
__PURE_APPDOMAIN_GLOBAL extern ostream cerr(&ferr);

 #else /* defined(_M_CEE_PURE) */
__PURE_APPDOMAIN_GLOBAL extern _CRTDATA2 ostream cerr(&ferr);
 #endif /* defined(_M_CEE_PURE) */

		// INITIALIZATION CODE
struct _Init_cerr
	{	// ensures that cerr is initialized
	__CLR_OR_THIS_CALL _Init_cerr()
		{	// initialize cerr
		_Ptr_cerr = &cerr;
		cerr.tie(_Ptr_cout);
		cerr.setf(ios_base::unitbuf);
		}
	};

__PURE_APPDOMAIN_GLOBAL static _Init_cerr init_cerr;

_STD_END

/*
 * Copyright (c) by P.J. Plauger. All rights reserved.
 * Consult your license regarding permissions and restrictions.
V6.50:0009 */
