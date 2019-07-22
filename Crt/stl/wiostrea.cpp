// wiostream -- _Winit members, dummy for Microsoft
#include <iostream>
_STD_BEGIN

		// OBJECT DECLARATIONS
__PURE_APPDOMAIN_GLOBAL int _Winit::_Init_cnt = -1;

_CRTIMP2_PURE __thiscall _Winit::_Winit()
	{	// initialize standard wide streams first time
	if (0 <= _Init_cnt)
		++_Init_cnt;
	else
		_Init_cnt = 1;
	}

_CRTIMP2_PURE __thiscall _Winit::~_Winit() _NOEXCEPT
	{	// flush standard wide streams last time
	if (--_Init_cnt == 0)
		{	// flush standard wide streams
		if (_Ptr_wcout != 0)
			_Ptr_wcout->flush();
		if (_Ptr_wcerr != 0)
			_Ptr_wcerr->flush();
		if (_Ptr_wclog != 0)
			_Ptr_wclog->flush();
		}
_STD_END
	}

/*
 * Copyright (c) by P.J. Plauger. All rights reserved.
 * Consult your license regarding permissions and restrictions.
V6.50:0009 */
