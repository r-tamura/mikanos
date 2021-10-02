/**
 * @file logger.hpp
 *
 * カーネルロガーの実装.
 */

#pragma once

enum LogLevel {
  kError = 3,
  kWarn  = 4,
  kInfo  = 6,
  kDebug = 7,
};

/** @brief グローバルなログ優先度のしきい値を変更する */
void SetLogLevel(LogLevel level);

/** @brief ログを指定された優先度で記録する */
int Log(LogLevel level, const char* format, ...);