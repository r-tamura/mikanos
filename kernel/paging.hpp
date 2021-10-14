/**
 * @file paging.hpp
 *
 * メモリページング用のプログラムを集めたファイル
 */

#pragma once

#include <cstddef>

/** @brief 静的に確保するページディレクトリの個数
 *
 * 個の定数は SetupIdentitypageMap で使用される.
 * 1つのページディレクトリには512個の2MiBページを設定できるので,
 * kPageDirectoryCount x 1GiBの仮想アドレスがマッピングされることになる.
 */
const size_t kPageDirectoryCount = 64;
/** @brief 仮想アドレス=物理アドレスとなるようにページテーブルを設定する.
 * 最終的に CR3 レジスタが正しく設定されたページテーブルを指すようになる.
 */
void SetupIdentityPageTable();

void InitializePaging();