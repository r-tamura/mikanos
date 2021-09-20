/**
 * @file pci.hpp
 *
 * PCIバス制御プログラムを集めたファイル.
 */

#pragma once

#include <cstdint>
#include <array>

#include "error.hpp"

namespace pci {
  /** @brief CONFIG_ADDRESS レジスタのIOポートアドレス */
  const uint16_t kConfigAddress = 0x0cf8;
  /** @brief CONFIG_DATA レジスタのIOポートアドレス */
  const uint16_t kConfigData = 0x0cfc;

  /** @brief CONFIG_ADDRESSに指定された整数を書き込む */
  void WriteAddress(uint32_t address);
  /** @brief CONFIG_DATAに指定された整数を書き込む */
  void WriteData(uint32_t value);
  /** @brief CONFIG_DATAから32ビット整数を読み込む */
  uint32_t ReadData();
  /** @brief ベンダIDレジスタを読み取る（全ヘッダタイプ共通）*/
  uint16_t ReadVendorId(uint8_t bus, uint8_t device, uint8_t function);
  /** @brief デバイスIDレジスタを読み取る（全ヘッダタイプ共通）*/
  uint16_t ReadDeviceId(uint8_t bus, uint8_t device, uint8_t function);
  /** @brief ヘッダタイプレジスタを読み取る（全ヘッダタイプ共通）*/
  uint8_t ReadHeaderType(uint8_t bus, uint8_t device, uint8_t function);
  /** @brief クラスコードレジスタを読み取る（全ヘッダタイプ共通）
   * 返される32ビット整数の構造は以下の通り
   * - 31:24 : ベースクラス
   * - 23:16 : サブクラス
   * - 15:8  : インタフェース
   * - 7:0   : リビジョン
  */
  uint32_t ReadClassCode(uint8_t bus, uint8_t device, uint8_t function);

  /** @brief バス番号レジスタを読み取る（ヘッダタイプ 1 用）
  *
  * 返される 32 ビット整数の構造は次の通り．
  *   - 23:16 : サブオーディネイトバス番号
  *   - 15:8  : セカンダリバス番号
  *   - 7:0   : リビジョン番号
  */
  uint32_t ReadBusNumbers(uint8_t bus, uint8_t device, uint8_t function);

  /** @brief 単一ファンクションの場合に信を返す */
  bool IsSingleFunctionDevice(uint8_t header_type);

  /** @brief PCIデバイスを操作するための基礎データを格納する
   * バス番号,デバイス番号,ファンクション番号はデバイスを特定するのに必須.
   * その他の情報は単に利便性のために加えてある.
   */
  struct Device {
    uint8_t bus, device, function, header_type;
  };

  /** @brief ScanAllBus() により発見されたPCIデバイスの一覧 */
  inline std::array<Device, 32> devices;

  /** devicesの有効な要素の数 */
  inline int num_device;

  /** @brief PCIデバイスを全て探索しdevicesに格納する
   *
   * バス0から再帰的にPCIデバイスを探索し,devicesの先頭から詰めて書き込む.
   * 発見したデバイスの数をnum_devicesに設定する.
   */
  Error ScanAllBus();
}