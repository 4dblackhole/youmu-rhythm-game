#pragma once

#ifndef DECLARE_VARIABLE_STRING

#define DECLARE_VARIABLE_STRING(x) static constexpr LPCSTR x = #x

#endif