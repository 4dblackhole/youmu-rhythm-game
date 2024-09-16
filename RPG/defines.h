#pragma once

#ifndef DECLARE_VARIABLE_STRING

#define DECLARE_VARIABLE_STRING(x) static constexpr LPCSTR x = #x

#endif


#if defined(DEBUG) | defined(_DEBUG)

#define DEBUG_BREAKPOINT int asdf__ASDfdsf11 = 3

#else
#define DEBUG_BREAKPOINT
#endif