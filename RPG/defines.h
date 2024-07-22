#pragma once

#ifndef DECLARE_VARIABLE_STRING(x)

#define DECLARE_VARIABLE_STRING(x) static constexpr LPCSTR x = #x;

#endif