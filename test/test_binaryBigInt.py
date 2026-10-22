#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
BinaryBigInt 建構子與狀態驗證自動化測試套件 (Python Test Suite)
全面測試：
1. Default Constructor (預設建構子)
2. Decimal String Constructor (十進制字串：正、負、零、前導零、大數)
3. Binary String Constructor (二進制字串：0b 前綴、正負號、前導零)
4. Hexadecimal String Constructor (十六進制字串：0x 前綴、大小寫、正負號)
5. Invalid Formats Exception Handling (非法格式拋出例外檢驗)
6. Fuzz Testing (隨機混合壓力測試)
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

class BinaryBigIntTester:
    def __init__(self):
        self.script_dir = os.path.dirname(os.path.abspath(__file__))
        self.root_dir = os.path.dirname(self.script_dir)
        self.exe_path = os.path.join(self.script_dir, "test_binaryBigInt_bridge.exe")
        self.proc = None
        self.total_tests = 0
        self.passed_tests = 0
        self.failures = []

    def compile_bridge(self):
        src_bigint = os.path.join(self.root_dir, "src", "binaryBigInt.cpp")
        src_util = os.path.join(self.root_dir, "src", "bigNumberUtil.cpp")
        src_bridge = os.path.join(self.script_dir, "test_binaryBigInt_bridge.cpp")

        if os.path.exists(self.exe_path):
            exe_mtime = os.path.getmtime(self.exe_path)
            if (exe_mtime > os.path.getmtime(src_bigint) and
                exe_mtime > os.path.getmtime(src_util) and
                exe_mtime > os.path.getmtime(src_bridge)):
                print("[BUILD] 測試橋接器已是最新，跳過編譯。\n")
                return

        print("[BUILD] 編譯 C++ 測試橋接器 (test_binaryBigInt_bridge.cpp + binaryBigInt.cpp + bigNumberUtil.cpp)...")
        if os.path.exists(self.exe_path):
            try:
                os.remove(self.exe_path)
            except Exception:
                pass

        cmd = [
            "g++", "-O2", "-std=c++17",
            "-DBIGNUMBER_UTIL_NO_MAIN",
            src_bridge, src_bigint, src_util,
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

    def record_failure(self, desc: str, expected: str, actual: str):
        self.total_tests += 1
        msg = f"[FAIL] {desc}\n  預期: {expected}\n  實際: {actual}"
        self.failures.append(msg)
        print(f"❌ {msg}\n")
        if len(self.failures) >= 10:
            print(f"⚠️ 累計失敗達到 {len(self.failures)} 個，提早中斷測試以供檢查。")
            self.close()
            sys.exit(1)

    def assert_status(self, raw_out: str, exp_val: int, desc: str):
        self.total_tests += 1
        parts = raw_out.split()
        if parts[0] != "OK":
            self.record_failure(desc, f"OK {exp_val}", raw_out)
            return

        act_str = parts[1]
        act_zero = int(parts[2])
        act_neg = int(parts[3])
        act_bitlen = int(parts[4])

        exp_str = str(exp_val)
        exp_zero = 1 if exp_val == 0 else 0
        exp_neg = 1 if exp_val < 0 else 0
        exp_bitlen = exp_val.bit_length() if exp_val != 0 else 0

        errs = []
        if act_str != exp_str:
            errs.append(f"數值字串不符 (預期 {exp_str}, 實際 {act_str})")
        if act_zero != exp_zero:
            errs.append(f"is_zero 不符 (預期 {exp_zero}, 實際 {act_zero})")
        if act_neg != exp_neg:
            errs.append(f"is_negative 不符 (預期 {exp_neg}, 實際 {act_neg})")
        if act_bitlen != exp_bitlen:
            errs.append(f"bit_length 不符 (預期 {exp_bitlen}, 實際 {act_bitlen})")

        if errs:
            self.record_failure(desc, f"值={exp_str}, is_zero={exp_zero}, is_neg={exp_neg}, bit_len={exp_bitlen}",
                                f"值={act_str}, is_zero={act_zero}, is_neg={act_neg}, bit_len={act_bitlen} ({', '.join(errs)})")
        else:
            self.passed_tests += 1

    def assert_invalid(self, raw_out: str, input_str: str, desc: str):
        self.total_tests += 1
        if raw_out.startswith("ERR"):
            self.passed_tests += 1
        else:
            self.record_failure(desc, "ERR (拋出異常)", raw_out)

    # -------------------------------------------------------------
    # 測試模組 1：預設建構子
    # -------------------------------------------------------------
    def test_default(self):
        print("[1/6] 測試預設建構子 BinaryBigInt()...")
        out = self.query("DEFAULT")
        self.assert_status(out, 0, "BinaryBigInt() 預設建構子")
        print("  -> 通過預設建構子測試\n")

    # -------------------------------------------------------------
    # 測試模組 2：十進制字串建構子
    # -------------------------------------------------------------
    def test_decimal(self):
        print("[2/6] 測試十進制字串建構 (正、負、零、前導零、臨界值)...")
        cases = [
            ("0", 0), ("+0", 0), ("-0", 0),
            ("000", 0), ("+000", 0), ("-000", 0),
            ("1", 1), ("+1", 1), ("-1", -1),
            ("123", 123), ("+123", 123), ("-123", -123),
            ("00123", 123), ("+00123", 123), ("-00123", -123),
            ("2147483647", 2147483647),
            ("-2147483648", -2147483648),
            ("4294967295", 4294967295),
            ("4294967296", 4294967296),
            ("-4294967296", -4294967296),
            ("18446744073709551615", 18446744073709551615),
            ("-18446744073709551616", -18446744073709551616),
        ]
        # 隨機十進制大數
        for _ in range(30):
            d = random.randint(10, 80)
            val = random.randint(10**(d-1), 10**d - 1)
            sign = random.choice(["", "+", "-"])
            exp = -val if sign == "-" else val
            cases.append((f"{sign}{val}", exp))

        for s_in, exp in cases:
            out = self.query(f"NEW {s_in}")
            self.assert_status(out, exp, f"十進制建構: '{s_in}'")
        print(f"  -> 通過 {len(cases)} 組十進制建構測試\n")

    # -------------------------------------------------------------
    # 測試模組 3：二進制字串建構子
    # -------------------------------------------------------------
    def test_binary(self):
        print("[3/6] 測試二進制字串建構 (0b 前綴, 正、負、零、前導零)...")
        cases = [
            ("0b0", 0), ("+0b0", 0), ("-0b0", 0),
            ("0b000", 0), ("+0b000", 0), ("-0b000", 0),
            ("0b1", 1), ("+0b1", 1), ("-0b1", -1),
            ("0b101", 5), ("+0b101", 5), ("-0b101", -5),
            ("0b00101", 5), ("+0b00101", 5), ("-0b00101", -5),
            ("0b11111111", 255), ("-0b11111111", -255),
            ("0b10000000000000000000000000000000", 2147483648),
            ("0b100000000000000000000000000000000", 4294967296),
            ("-0b100000000000000000000000000000000", -4294967296),
        ]
        # 隨機二進制數
        for _ in range(30):
            bits = random.randint(1, 256)
            val = random.getrandbits(bits)
            bin_body = bin(val)[2:]
            sign = random.choice(["", "+", "-"])
            prefix = f"{sign}0b" if sign else "0b"
            lead_zeros = "0" * random.randint(0, 4)
            s_in = f"{prefix}{lead_zeros}{bin_body}"
            exp = -val if sign == "-" else val
            cases.append((s_in, exp))

        for s_in, exp in cases:
            out = self.query(f"NEW {s_in}")
            self.assert_status(out, exp, f"二進制建構: '{s_in}'")
        print(f"  -> 通過 {len(cases)} 組二進制建構測試\n")

    # -------------------------------------------------------------
    # 測試模組 4：十六進制字串建構子
    # -------------------------------------------------------------
    def test_hex(self):
        print("[4/6] 測試十六進制字串建構 (0x 前綴, 大小寫, 正負, 前導零)...")
        cases = [
            ("0x0", 0), ("+0x0", 0), ("-0x0", 0),
            ("0x000", 0), ("+0x000", 0), ("-0x000", 0),
            ("0x1", 1), ("+0x1", 1), ("-0x1", -1),
            ("0x1a", 26), ("0x1A", 26), ("+0x1A", 26), ("-0x1a", -26),
            ("0x001a", 26), ("+0x00FF", 255), ("-0x00ff", -255),
            ("0x7fffffff", 2147483647),
            ("0x80000000", 2147483648),
            ("0xffffffff", 4294967295),
            ("0x100000000", 4294967296),
            ("-0x100000000", -4294967296),
            ("0xDeAdBeEf", 0xdeadbeef),
        ]
        # 隨機十六進制數
        for _ in range(30):
            bits = random.randint(1, 256)
            val = random.getrandbits(bits)
            hex_body = hex(val)[2:]
            # 隨機大小寫混合
            mixed_hex = "".join(c.upper() if random.random() > 0.5 else c for c in hex_body)
            sign = random.choice(["", "+", "-"])
            lead_zeros = "0" * random.randint(0, 4)
            s_in = f"{sign}0x{lead_zeros}{mixed_hex}"
            exp = -val if sign == "-" else val
            cases.append((s_in, exp))

        for s_in, exp in cases:
            out = self.query(f"NEW {s_in}")
            self.assert_status(out, exp, f"十六進制建構: '{s_in}'")
        print(f"  -> 通過 {len(cases)} 組十六進制建構測試\n")

    # -------------------------------------------------------------
    # 測試模組 5：非法字串例外檢查
    # -------------------------------------------------------------
    def test_invalid(self):
        print("[5/6] 測試非法字串格式防禦 (應安全拋出例外)...")
        invalids = [
            # 空字串或單獨符號
            "+", "-",
            # 只有前綴無數字
            "0b", "+0b", "-0b",
            "0x", "+0x", "-0x",
            # 非法字元
            "123a", "12b34", "-99z",
            "0b102", "0b10a", "+0b201",
            "0x12g", "0xz", "-0x123h",
            # 多重符號
            "++123", "--123", "+-123",
            "+0b+1", "-0x-1",
            # 混雜符號或空白
            " 123", "123 ", "0b 101",
        ]
        for s_in in invalids:
            out = self.query(f"NEW {s_in}")
            self.assert_invalid(out, s_in, f"非法格式拒絕: '{s_in}'")
        print(f"  -> 通過 {len(invalids)} 組非法字串防禦測試\n")

    # -------------------------------------------------------------
    # 測試模組 6：隨機混合壓力測試 (Fuzzing)
    # -------------------------------------------------------------
    def test_fuzz(self):
        print("[6/6] 執行 500 組隨機混合壓力測試 (十進制/二進制/十六進制/正負)...")
        for i in range(500):
            kind = random.choice(["DEC", "BIN", "HEX"])
            bits = random.randint(1, 512)
            val = random.getrandbits(bits)
            sign = random.choice(["", "+", "-"])
            exp = -val if sign == "-" else val

            if kind == "DEC":
                s_in = f"{sign}{val}"
            elif kind == "BIN":
                s_in = f"{sign}0b{bin(val)[2:]}"
            else:
                s_in = f"{sign}0x{hex(val)[2:]}"

            out = self.query(f"NEW {s_in}")
            self.assert_status(out, exp, f"Fuzz [{kind}] 第 {i+1} 組")
        print("  -> 通過 500 組混合模糊壓力測試！\n")

    def run_all(self):
        start_time = time.time()
        self.compile_bridge()
        self.start_process()

        try:
            self.test_default()
            self.test_decimal()
            self.test_binary()
            self.test_hex()
            self.test_invalid()
            self.test_fuzz()
        finally:
            self.close()

        elapsed = time.time() - start_time
        print("==================================================")
        if not self.failures:
            print(f"[SUCCESS] 全部測試 100% 通過！總計檢驗 {self.passed_tests} 個斷言！")
        else:
            print(f"[FAIL] 發現 {len(self.failures)} 個錯誤！(通過 {self.passed_tests} / 總共 {self.total_tests})")
        print(f"[TIME] 耗時: {elapsed:.2f} 秒")
        print("==================================================")

if __name__ == "__main__":
    tester = BinaryBigIntTester()
    tester.run_all()
