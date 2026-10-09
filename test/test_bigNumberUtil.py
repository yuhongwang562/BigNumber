#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
bigNumberUtil 自動化驗證測試套件 (Python Test Suite)
全面測試：
1. decimalStr_to_uint32arr (十進位解析與表示)
2. binaryStr_to_uint32arr (二進位解析與表示)
3. HexStr_to_uint32arr (十六進位解析與表示)
4. uint32arr_to_BaseStr (2 ~ 16 全進位轉換)
5. compare_magnitude (無符號大小比較)
6. add_magnitude (無符號加法、連續進位、自引用別名)
7. sub_magnitude (無符號減法、連續借位、自引用別名)
8. 大規模隨機模糊測試 (Fuzz Testing)
"""

import os
import sys
import subprocess
import random
import time

if sys.platform.startswith('win'):
    try:
        sys.stdout.reconfigure(encoding='utf-8')
        sys.stderr.reconfigure(encoding='utf-8')
    except Exception:
        pass

# 輔助函式：Python 原生將任意整數轉換為 2~16 進位字串
def int_to_base_str(n: int, base: int) -> str:
    if n == 0:
        return "0"
    digits = "0123456789abcdef"
    res = []
    while n > 0:
        res.append(digits[n % base])
        n //= base
    return "".join(reversed(res))

class BigNumberTester:
    def __init__(self):
        self.script_dir = os.path.dirname(os.path.abspath(__file__))
        self.root_dir = os.path.dirname(self.script_dir)
        self.exe_path = os.path.join(self.script_dir, "test_bridge.exe")
        self.proc = None
        self.total_tests = 0
        self.passed_tests = 0

    def compile_bridge(self):
        src_util = os.path.join(self.root_dir, "src", "bigNumberUtil.cpp")
        src_bridge = os.path.join(self.script_dir, "test_bridge.cpp")

        # 若 exe 已存在且比原始碼更新，直接跳過編譯
        if os.path.exists(self.exe_path):
            exe_mtime = os.path.getmtime(self.exe_path)
            if exe_mtime > os.path.getmtime(src_util) and exe_mtime > os.path.getmtime(src_bridge):
                print("[BUILD] 測試橋接器已是最新，跳過編譯。\n")
                return

        print("[BUILD] 編譯 C++ 測試橋接器 (test_bridge.cpp + bigNumberUtil.cpp)...")
        # 若需要重新編譯，先移除舊的 exe
        if os.path.exists(self.exe_path):
            try:
                os.remove(self.exe_path)
            except Exception:
                pass

        cmd = [
            "g++", "-O2", "-std=c++17",
            "-DBIGNUMBER_UTIL_NO_MAIN",
            src_bridge, src_util,
            "-o", self.exe_path
        ]
        res = subprocess.run(cmd, capture_output=True, text=True)
        if res.returncode != 0:
            print(f"[FAIL] 編譯失敗:\n{res.stderr}")
            sys.exit(1)
        print("[OK] 編譯成功！\n")

    def start_process(self):
        self.proc = subprocess.Popen(
            [self.exe_path],
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True,
            bufsize=1
        )

    def close(self):
        if self.proc:
            try:
                self.proc.stdin.write("EXIT\n")
                self.proc.stdin.flush()
                self.proc.wait(timeout=2)
            except Exception:
                self.proc.kill()

    def query(self, cmd_line: str) -> str:
        self.proc.stdin.write(cmd_line + "\n")
        self.proc.stdin.flush()
        out = self.proc.stdout.readline()
        if not out:
            stderr = self.proc.stderr.read()
            raise RuntimeError(f"C++ 程式崩潰或無回應！指令: {cmd_line}\nStderr: {stderr}")
        return out.strip()

    def assert_eq(self, actual: str, expected: str, desc: str):
        self.total_tests += 1
        if actual != expected:
            print(f"\n❌ [FAIL] {desc}")
            print(f"  預期: {expected}")
            print(f"  實際: {actual}")
            self.close()
            sys.exit(1)
        self.passed_tests += 1

    # -------------------------------------------------------------
    # 測試模組 1：十進位轉換 (decimalStr_to_uint32arr & Base 10)
    # -------------------------------------------------------------
    def test_decimal(self):
        print("[1/8] 測試十進位解析與表示 (含 0, Limb 邊界, 稀疏 Limb, 大數)...")
        cases = [
            0, 1, 2, 9, 10,
            2147483647,             # 2^31 - 1
            2147483648,             # 2^31
            4294967295,             # 2^32 - 1 (1 Limb 滿位)
            4294967296,             # 2^32 (跨 2 Limb 邊界)
            4294967297,             # 2^32 + 1
            18446744073709551615,   # 2^64 - 1 (2 Limb 滿位)
            18446744073709551616,   # 2^64 (跨 3 Limb 邊界)
            18446744073709551617,   # 2^64 + 1 (稀疏中間 0 Limb)
        ]
        # 加入隨機大數 (10 ~ 200 位)
        for _ in range(50):
            d = random.randint(10, 200)
            cases.append(random.randint(10**(d-1), 10**d - 1))

        for val in cases:
            s_val = str(val)
            out = self.query(f"DEC {s_val}")
            self.assert_eq(out, s_val, f"DEC 轉換失敗: {s_val[:30]}...")
        print(f"  -> 通過 {len(cases)} 組十進位邊界測試")

    # -------------------------------------------------------------
    # 測試模組 2：二進位轉換 (binaryStr_to_uint32arr & Base 2)
    # -------------------------------------------------------------
    def test_binary(self):
        print("[2/8] 測試二進位解析與表示 (符合契約無前導零)...")
        cases = [0, 1, 2, 3, 7, 8, 15, 16, 31, 32, 63, 64, 127, 128, 255]
        # 32 位元、33 位元、64 位元、65 位元臨界值
        cases.extend([
            (1 << 31) - 1, (1 << 31),
            (1 << 32) - 1, (1 << 32), (1 << 32) + 1,
            (1 << 64) - 1, (1 << 64), (1 << 64) + 1
        ])
        for _ in range(50):
            bits = random.randint(1, 512)
            cases.append(random.getrandbits(bits))

        for val in cases:
            bin_str = bin(val)[2:] # 無前導零，0 則是 "0"
            out = self.query(f"BIN {bin_str}")
            self.assert_eq(out, bin_str, f"BIN 轉換失敗: {bin_str[:30]}...")
        print(f"  -> 通過 {len(cases)} 組二進位邊界測試")

    # -------------------------------------------------------------
    # 測試模組 3：十六進位轉換 (HexStr_to_uint32arr & Base 16)
    # -------------------------------------------------------------
    def test_hex(self):
        print("[3/8] 測試十六進位解析與表示 (符合契約無前導零, 支援 a-f)...")
        cases = [
            0, 1, 10, 15, 16, 255, 256,
            0x7fffffff, 0x80000000, 0xffffffff,
            0x100000000, 0x100000001,
            0xffffffffffffffff, 0x10000000000000000
        ]
        for _ in range(50):
            bits = random.randint(1, 512)
            cases.append(random.getrandbits(bits))

        for val in cases:
            hex_str = hex(val)[2:] # 全小寫，無前導零
            out = self.query(f"HEX {hex_str}")
            self.assert_eq(out, hex_str, f"HEX 轉換失敗: {hex_str[:30]}...")
        print(f"  -> 通過 {len(cases)} 組十六進位邊界測試")

    # -------------------------------------------------------------
    # 測試模組 4：全進制轉換 (uint32arr_to_BaseStr, 2 <= base <= 16)
    # -------------------------------------------------------------
    def test_all_bases(self):
        print("[4/8] 測試 2 ~ 16 全進制轉換 (Base 2 到 16)...")
        test_numbers = [
            0, 1, 42, 255, 1024,
            4294967295, 4294967296,
            18446744073709551615, 18446744073709551616,
            random.getrandbits(128)
        ]
        count = 0
        for val in test_numbers:
            for b in range(2, 17):
                exp = int_to_base_str(val, b)
                out = self.query(f"BASE {val} {b}")
                self.assert_eq(out, exp, f"Base {b} 轉換失敗，數值: {val}")
                count += 1
        print(f"  -> 通過 {count} 組進制轉換測試")

    # -------------------------------------------------------------
    # 測試模組 5：絕對值比較 (compare_magnitude)
    # -------------------------------------------------------------
    def test_compare(self):
        print("[5/8] 測試 compare_magnitude 邊界與不同長度...")
        cases = [
            (0, 0, 0),
            (100, 100, 0),
            (0, 1, -1),
            (1, 0, 1),
            (4294967295, 4294967295, 0),
            (4294967296, 4294967295, 1),   # 2 Limb vs 1 Limb
            (4294967295, 4294967296, -1),  # 1 Limb vs 2 Limb
            (4294967296, 4294967297, -1),  # 相同長度，低位不同
            (4294967297, 4294967296, 1),
            (18446744073709551616, 18446744073709551615, 1),
        ]
        # 隨機數比較
        for _ in range(50):
            a = random.getrandbits(random.randint(1, 256))
            b = random.getrandbits(random.randint(1, 256))
            exp = 0 if a == b else (1 if a > b else -1)
            cases.append((a, b, exp))

        for a, b, exp in cases:
            out = self.query(f"CMP {a} {b}")
            self.assert_eq(out, str(exp), f"CMP 比較失敗: {a} vs {b}")
        print(f"  -> 通過 {len(cases)} 組比較測試")

    # -------------------------------------------------------------
    # 測試模組 6：大數加法與自引用別名 (add_magnitude)
    # -------------------------------------------------------------
    def test_add(self):
        print("[6/8] 測試 add_magnitude (含連鎖進位, 0, 自引用別名 a += b, a += a)...")
        cases = [
            (0, 0), (0, 123), (123, 0),
            (4294967295, 1),                        # 2^32 - 1 + 1 (跨 Limb)
            (4294967295, 4294967295),                # 滿位相加
            (18446744073709551615, 1),              # 2^64 - 1 + 1 (跨 2 Limb 連鎖進位)
            (18446744073709551616, 18446744073709551616),
            (10**100 - 1, 1),                       # 100 個 9 + 1 (長距離連鎖進位)
        ]
        for _ in range(100):
            a = random.getrandbits(random.randint(1, 512))
            b = random.getrandbits(random.randint(1, 512))
            cases.append((a, b))

        for a, b in cases:
            exp = str(a + b)
            # 1. 正常加法 (result, a, b)
            out = self.query(f"ADD {a} {b}")
            self.assert_eq(out, exp, f"ADD 失敗: {a} + {b}")

            # 2. 自引用別名 a += b
            out_alias = self.query(f"ADD_ALIAS_A {a} {b}")
            self.assert_eq(out_alias, exp, f"ADD 別名 (a += b) 失敗: {a} + {b}")

            # 3. 自引用別名 a += a
            exp_self = str(a + a)
            out_self = self.query(f"ADD_ALIAS_SELF {a}")
            self.assert_eq(out_self, exp_self, f"ADD 自加 (a += a) 失敗: {a} + {a}")

        print(f"  -> 通過 {len(cases) * 3} 組加法與別名測試")

    # -------------------------------------------------------------
    # 測試模組 7：大數減法與自引用別名 (sub_magnitude, a >= b)
    # -------------------------------------------------------------
    def test_sub(self):
        print("[7/8] 測試 sub_magnitude (含連鎖借位, a == b, 減 0, 別名 a -= b, a -= a)...")
        cases = [
            (0, 0), (123, 0), (123, 123),
            (4294967296, 1),                        # 2^32 - 1 = 4294967295 (跨 Limb 借位)
            (4294967296, 4294967295),                # 2^32 - (2^32 - 1) = 1
            (18446744073709551616, 1),              # 2^64 - 1 (跨多 Limb 連鎖借位)
            (18446744073709551616, 4294967296),
            (18446744073709551616, 18446744073709551616),
            (10**100, 1),                           # 1 後面 100 個 0 減 1 (極限連鎖借位)
        ]
        for _ in range(100):
            a = random.getrandbits(random.randint(1, 512))
            b = random.getrandbits(random.randint(1, 512))
            if a < b:
                a, b = b, a # 契約保證 a >= b
            cases.append((a, b))

        for a, b in cases:
            exp = str(a - b)
            # 1. 正常減法
            out = self.query(f"SUB {a} {b}")
            self.assert_eq(out, exp, f"SUB 失敗: {a} - {b}")

            # 2. 自引用別名 a -= b
            out_alias = self.query(f"SUB_ALIAS_A {a} {b}")
            self.assert_eq(out_alias, exp, f"SUB 別名 (a -= b) 失敗: {a} - {b}")

            # 3. 自引用別名 a -= a 必定得 0
            out_self = self.query(f"SUB_ALIAS_SELF {a}")
            self.assert_eq(out_self, "0", f"SUB 自減 (a -= a) 失敗，數值: {a}")

        print(f"  -> 通過 {len(cases) * 3} 組減法與別名測試")

    # -------------------------------------------------------------
    # 測試模組 8：千組混合隨機模糊壓力測試 (Fuzz Testing)
    # -------------------------------------------------------------
    def test_fuzz(self):
        print("[8/8] 執行 1,000 組超大數混合模糊壓力測試 (Fuzzing)...")
        ops = ["ADD", "SUB", "CMP", "BASE"]
        for i in range(1000):
            op = random.choice(ops)
            if op == "ADD":
                a = random.getrandbits(random.randint(1, 1024))
                b = random.getrandbits(random.randint(1, 1024))
                exp = str(a + b)
                out = self.query(f"ADD {a} {b}")
                self.assert_eq(out, exp, f"Fuzz ADD 失敗第 {i} 組")
            elif op == "SUB":
                a = random.getrandbits(random.randint(1, 1024))
                b = random.getrandbits(random.randint(1, 1024))
                if a < b: a, b = b, a
                exp = str(a - b)
                out = self.query(f"SUB {a} {b}")
                self.assert_eq(out, exp, f"Fuzz SUB 失敗第 {i} 組")
            elif op == "CMP":
                a = random.getrandbits(random.randint(1, 1024))
                b = random.getrandbits(random.randint(1, 1024))
                exp = 0 if a == b else (1 if a > b else -1)
                out = self.query(f"CMP {a} {b}")
                self.assert_eq(out, str(exp), f"Fuzz CMP 失敗第 {i} 組")
            elif op == "BASE":
                val = random.getrandbits(random.randint(1, 512))
                base = random.randint(2, 16)
                exp = int_to_base_str(val, base)
                out = self.query(f"BASE {val} {base}")
                self.assert_eq(out, exp, f"Fuzz BASE 失敗第 {i} 組")
        print("  -> 通過 1,000 組混合模糊壓力測試！")

    def run_all(self):
        start_time = time.time()
        self.compile_bridge()
        self.start_process()

        try:
            self.test_decimal()
            self.test_binary()
            self.test_hex()
            self.test_all_bases()
            self.test_compare()
            self.test_add()
            self.test_sub()
            self.test_fuzz()
        finally:
            self.close()

        elapsed = time.time() - start_time
        print(f"\n==================================================")
        print(f"[SUCCESS] 全部測試 100% 通過！總計檢驗 {self.passed_tests} 個斷言！")
        print(f"[TIME] 耗時: {elapsed:.2f} 秒")
        print(f"==================================================")

if __name__ == "__main__":
    tester = BigNumberTester()
    tester.run_all()
