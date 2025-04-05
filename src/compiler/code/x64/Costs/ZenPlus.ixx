export module code.x64.costs;

import code.cost;
import code.x64.costs;
import <cstdint>;

/**
 * Acknowledgment:
 * 
 * The instruction data used in this file was generated from the excellent resource available at:
 * https://uops.info/table.html
 * 
 * I am very grateful to the creator(s) and maintainer(s) of this invaluable tool.
 * I didn't see any specific authorship and licensing information on the website, but
 * the resource has been immensely helpful in generating this code, and for more in my project.
 * 
 * Thank you very much!
 */

using ALU = code::Cost::ALU;
using AGU = code::Cost::AGU;
using LSU = code::Cost::LSU;
using FADD = code::Cost::FADD;
using FMUL = code::Cost::FMUL;
using FMISC = code::Cost::FMISC;
using LOCK = code::Cost::LOCK;
using BRANCH = code::Cost::BRANCH;
using FPU = code::Cost::FPU;
using FANY = code::Cost::FANY;
using UNKNOWN = code::Cost::UNKNOWN;

// The following data is for the Zen+ CPU model.
export namespace code::x64 {
  export class ZenPlus : public Costs {
  public:
    code::Cost ADC_AL_I8() const final { return {0.50, 1, 1, 1, ALU}; }
    code::Cost ADC_AX_I16() const final { return {0.50, 1, 1, 1, ALU}; }
    code::Cost ADC_EAX_I32() const final { return {0.50, 1, 1, 1, ALU}; }
    code::Cost ADC_M16_I16() const final { return {1.00, 1, 9, 1, ALU | AGU | LSU}; }
    code::Cost ADC_M16_I8() const final { return {1.00, 1, 9, 1, ALU | AGU | LSU}; }
    code::Cost ADC_M16_R16() const final { return {1.00, 1, 9, 1, ALU | AGU | LSU}; }
    code::Cost ADC_M32_I32() const final { return {1.00, 1, 8, 1, ALU | AGU | LSU}; }
    code::Cost ADC_M32_I8() const final { return {1.00, 1, 8, 1, ALU | AGU | LSU}; }
    code::Cost ADC_M32_R32() const final { return {1.00, 1, 8, 1, ALU | AGU | LSU}; }
    code::Cost ADC_M64_I32() const final { return {1.00, 1, 8, 1, ALU | AGU | LSU}; }
    code::Cost ADC_M64_I8() const final { return {1.00, 1, 8, 1, ALU | AGU | LSU}; }
    code::Cost ADC_M64_R64() const final { return {1.00, 1, 8, 1, ALU | AGU | LSU}; }
    code::Cost ADC_M8_I8() const final { return {1.00, 1, 9, 1, ALU | AGU | LSU}; }
    code::Cost ADC_M8_R8() const final { return {1.00, 1, 9, 1, ALU | AGU | LSU}; }
    code::Cost ADC_R16_I16() const final { return {0.53, 1, 1, 1, ALU}; }
    code::Cost ADC_R16_I8() const final { return {0.50, 1, 1, 1, ALU}; }
    code::Cost ADC_R16_M16() const final { return {0.50, 1, 7, 1, ALU | AGU | LSU}; }
    code::Cost ADC_R32_I32() const final { return {0.60, 1, 1, 1, ALU}; }
    code::Cost ADC_R32_I8() const final { return {0.50, 1, 1, 1, ALU}; }
    code::Cost ADC_R32_M32() const final { return {0.50, 1, 7, 1, ALU | AGU | LSU}; }
    code::Cost ADC_R64_I32() const final { return {0.63, 1, 1, 1, ALU}; }
    code::Cost ADC_R64_I8() const final { return {0.50, 1, 1, 1, ALU}; }
    code::Cost ADC_R64_M64() const final { return {0.50, 1, 7, 1, ALU | AGU | LSU}; }
    code::Cost ADC_R8_I8() const final { return {0.52, 1, 1, 1, ALU}; }
    code::Cost ADC_R8_M8() const final { return {0.50, 1, 7, 1, ALU | AGU | LSU}; }
    code::Cost ADC_RAX_I32() const final { return {0.56, 1, 1, 1, ALU}; }
    code::Cost ADC_10_R8_R8() const final { return {0.50, 1, 1, 1, ALU}; }
    code::Cost ADC_11_R16_R16() const final { return {0.50, 1, 1, 1, ALU}; }
    code::Cost ADC_11_R32_R32() const final { return {0.50, 1, 1, 1, ALU}; }
    code::Cost ADC_11_R64_R64() const final { return {0.50, 1, 1, 1, ALU}; }
    code::Cost ADC_12_R8_R8() const final { return {0.50, 1, 1, 1, ALU}; }
    code::Cost ADC_13_R16_R16() const final { return {0.50, 1, 1, 1, ALU}; }
    code::Cost ADC_13_R32_R32() const final { return {0.50, 1, 1, 1, ALU}; }
    code::Cost ADC_13_R64_R64() const final { return {0.50, 1, 1, 1, ALU}; }
    code::Cost ADC_LOCK_M16_I16() const final { return {22.52, 1, 21, 1, ALU | AGU | LSU | LOCK}; }
    code::Cost ADC_LOCK_M16_I8() const final { return {22.52, 1, 21, 1, ALU | AGU | LSU | LOCK}; }
    code::Cost ADC_LOCK_M16_R16() const final { return {22.52, 1, 21, 1, ALU | AGU | LSU | LOCK}; }
    code::Cost ADC_LOCK_M32_I32() const final { return {22.52, 1, 21, 1, ALU | AGU | LSU | LOCK}; }
    code::Cost ADC_LOCK_M32_I8() const final { return {22.52, 1, 21, 1, ALU | AGU | LSU | LOCK}; }
    code::Cost ADC_LOCK_M32_R32() const final { return {22.52, 1, 21, 1, ALU | AGU | LSU | LOCK}; }
    code::Cost ADC_LOCK_M64_I32() const final { return {22.52, 1, 21, 1, ALU | AGU | LSU | LOCK}; }
    code::Cost ADC_LOCK_M64_I8() const final { return {22.52, 1, 21, 1, ALU | AGU | LSU | LOCK}; }
    code::Cost ADC_LOCK_M64_R64() const final { return {22.52, 1, 21, 1, ALU | AGU | LSU | LOCK}; }
    code::Cost ADC_LOCK_M8_I8() const final { return {22.52, 1, 21, 1, ALU | AGU | LSU | LOCK}; }
    code::Cost ADC_LOCK_M8_R8() const final { return {22.50, 1, 21, 1, ALU | AGU | LSU | LOCK}; }
    code::Cost ADC_R0_R8_I8() const final { return {0.50, 1, 1, 1, ALU}; }
    code::Cost ADC_R4_R8_I8() const final { return {0.50, 1, 1, 1, ALU}; }
    code::Cost ADD_AL_I8() const final { return {0.50, 1, 1, 1, ALU}; }
    code::Cost ADD_AX_I16() const final { return {0.50, 1, 1, 1, ALU}; }
    code::Cost ADD_EAX_I32() const final { return {0.50, 1, 1, 1, ALU}; }
    code::Cost ADD_M16_I16() const final { return {1.00, 5, 9, 1, ALU | AGU | LSU}; }
    code::Cost ADD_M16_I8() const final { return {1.00, 5, 9, 1, ALU | AGU | LSU}; }
    code::Cost ADD_M16_R16() const final { return {1.00, 1, 9, 1, ALU | AGU | LSU}; }
    code::Cost ADD_M32_I32() const final { return {1.00, 5, 8, 1, ALU | AGU | LSU}; }
    code::Cost ADD_M32_I8() const final { return {1.00, 5, 8, 1, ALU | AGU | LSU}; }
    code::Cost ADD_M32_R32() const final { return {1.00, 1, 8, 1, ALU | AGU | LSU}; }
    code::Cost ADD_M64_I32() const final { return {1.00, 5, 8, 1, ALU | AGU | LSU}; }
    code::Cost ADD_M64_I8() const final { return {1.00, 5, 8, 1, ALU | AGU | LSU}; }
    code::Cost ADD_M64_R64() const final { return {1.00, 1, 8, 1, ALU | AGU | LSU}; }
    code::Cost ADD_M8_I8() const final { return {1.00, 5, 9, 1, ALU | AGU | LSU}; }
    code::Cost ADD_M8_R8() const final { return {1.00, 1, 10, 1, ALU | AGU | LSU}; }
    code::Cost ADD_R16_I16() const final { return {0.36, 1, 1, 1, ALU}; }
    code::Cost ADD_R16_I8() const final { return {0.28, 1, 1, 1, ALU}; }
    code::Cost ADD_R16_M16() const final { return {0.50, 1, 7, 1, ALU | AGU | LSU}; }
    code::Cost ADD_R32_I32() const final { return {0.40, 1, 1, 1, ALU}; }
    code::Cost ADD_R32_I8() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost ADD_R32_M32() const final { return {0.50, 1, 7, 1, ALU | AGU | LSU}; }
    code::Cost ADD_R64_I32() const final { return {0.43, 1, 1, 1, ALU}; }
    code::Cost ADD_R64_I8() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost ADD_R64_M64() const final { return {0.50, 1, 7, 1, ALU | AGU | LSU}; }
    code::Cost ADD_R8_I8() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost ADD_R8_M8() const final { return {0.50, 1, 7, 1, ALU | AGU | LSU}; }
    code::Cost ADD_RAX_I32() const final { return {0.56, 1, 1, 1, ALU}; }
    code::Cost ADD_00_R8_R8() const final { return {0.38, 1, 1, 1, ALU}; }
    code::Cost ADD_01_R16_R16() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost ADD_01_R32_R32() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost ADD_01_R64_R64() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost ADD_02_R8_R8() const final { return {0.38, 1, 1, 1, ALU}; }
    code::Cost ADD_03_R16_R16() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost ADD_03_R32_R32() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost ADD_03_R64_R64() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost ADD_LOCK_M16_I16() const final { return {22.52, 7, 21, 1, ALU | AGU | LSU | LOCK}; }
    code::Cost ADD_LOCK_M16_I8() const final { return {22.52, 7, 21, 1, ALU | AGU | LSU | LOCK}; }
    code::Cost ADD_LOCK_M16_R16() const final { return {22.52, 1, 21, 1, ALU | AGU | LSU | LOCK}; }
    code::Cost ADD_LOCK_M32_I32() const final { return {22.52, 7, 21, 1, ALU | AGU | LSU | LOCK}; }
    code::Cost ADD_LOCK_M32_I8() const final { return {22.52, 7, 21, 1, ALU | AGU | LSU | LOCK}; }
    code::Cost ADD_LOCK_M32_R32() const final { return {22.52, 1, 21, 1, ALU | AGU | LSU | LOCK}; }
    code::Cost ADD_LOCK_M64_I32() const final { return {22.52, 7, 21, 1, ALU | AGU | LSU | LOCK}; }
    code::Cost ADD_LOCK_M64_I8() const final { return {22.52, 7, 21, 1, ALU | AGU | LSU | LOCK}; }
    code::Cost ADD_LOCK_M64_R64() const final { return {22.52, 1, 21, 1, ALU | AGU | LSU | LOCK}; }
    code::Cost ADD_LOCK_M8_I8() const final { return {22.52, 7, 21, 1, ALU | AGU | LSU | LOCK}; }
    code::Cost ADD_LOCK_M8_R8() const final { return {22.50, 1, 21, 1, ALU | AGU | LSU | LOCK}; }
    code::Cost ADD_R0_R8_I8() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost ADD_R4_R8_I8() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost AND_AL_I8() const final { return {0.50, 1, 1, 1, ALU}; }
    code::Cost AND_AX_I16() const final { return {0.50, 1, 1, 1, ALU}; }
    code::Cost AND_EAX_I32() const final { return {0.50, 1, 1, 1, ALU}; }
    code::Cost AND_M16_I16() const final { return {1.00, 5, 9, 1, ALU | AGU | LSU}; }
    code::Cost AND_M16_I8() const final { return {1.00, 5, 9, 1, ALU | AGU | LSU}; }
    code::Cost AND_M16_R16() const final { return {1.00, 1, 9, 1, ALU | AGU | LSU}; }
    code::Cost AND_M32_I32() const final { return {1.00, 5, 8, 1, ALU | AGU | LSU}; }
    code::Cost AND_M32_I8() const final { return {1.00, 5, 8, 1, ALU | AGU | LSU}; }
    code::Cost AND_M32_R32() const final { return {1.00, 1, 8, 1, ALU | AGU | LSU}; }
    code::Cost AND_M64_I32() const final { return {1.00, 5, 8, 1, ALU | AGU | LSU}; }
    code::Cost AND_M64_I8() const final { return {1.00, 5, 8, 1, ALU | AGU | LSU}; }
    code::Cost AND_M64_R64() const final { return {1.00, 1, 8, 1, ALU | AGU | LSU}; }
    code::Cost AND_M8_I8() const final { return {1.00, 5, 9, 1, ALU | AGU | LSU}; }
    code::Cost AND_M8_R8() const final { return {1.00, 1, 10, 1, ALU | AGU | LSU}; }
    code::Cost AND_R16_I16() const final { return {0.36, 1, 1, 1, ALU}; }
    code::Cost AND_R16_I8() const final { return {0.28, 1, 1, 1, ALU}; }
    code::Cost AND_R16_M16() const final { return {0.50, 1, 7, 1, ALU | AGU | LSU}; }
    code::Cost AND_R32_I32() const final { return {0.40, 1, 1, 1, ALU}; }
    code::Cost AND_R32_I8() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost AND_R32_M32() const final { return {0.50, 1, 7, 1, ALU | AGU | LSU}; }
    code::Cost AND_R64_I32() const final { return {0.43, 1, 1, 1, ALU}; }
    code::Cost AND_R64_I8() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost AND_R64_M64() const final { return {0.50, 1, 7, 1, ALU | AGU | LSU}; }
    code::Cost AND_R8_I8() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost AND_R8_M8() const final { return {0.50, 1, 7, 1, ALU | AGU | LSU}; }
    code::Cost AND_RAX_I32() const final { return {0.56, 1, 1, 1, ALU}; }
    code::Cost AND_20_R8_R8() const final { return {0.38, 1, 1, 1, ALU}; }
    code::Cost AND_21_R16_R16() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost AND_21_R32_R32() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost AND_21_R64_R64() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost AND_22_R8_R8() const final { return {0.38, 1, 1, 1, ALU}; }
    code::Cost AND_23_R16_R16() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost AND_23_R32_R32() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost AND_23_R64_R64() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost AND_LOCK_M16_I16() const final { return {22.52, 7, 21, 1, ALU | AGU | LSU | LOCK}; }
    code::Cost AND_LOCK_M16_I8() const final { return {22.52, 7, 21, 1, ALU | AGU | LSU | LOCK}; }
    code::Cost AND_LOCK_M16_R16() const final { return {22.52, 1, 21, 1, ALU | AGU | LSU | LOCK}; }
    code::Cost AND_LOCK_M32_I32() const final { return {22.52, 7, 21, 1, ALU | AGU | LSU | LOCK}; }
    code::Cost AND_LOCK_M32_I8() const final { return {22.52, 7, 21, 1, ALU | AGU | LSU | LOCK}; }
    code::Cost AND_LOCK_M32_R32() const final { return {22.52, 1, 21, 1, ALU | AGU | LSU | LOCK}; }
    code::Cost AND_LOCK_M64_I32() const final { return {22.52, 7, 21, 1, ALU | AGU | LSU | LOCK}; }
    code::Cost AND_LOCK_M64_I8() const final { return {22.52, 7, 21, 1, ALU | AGU | LSU | LOCK}; }
    code::Cost AND_LOCK_M64_R64() const final { return {22.52, 1, 21, 1, ALU | AGU | LSU | LOCK}; }
    code::Cost AND_LOCK_M8_I8() const final { return {22.52, 7, 21, 1, ALU | AGU | LSU | LOCK}; }
    code::Cost AND_LOCK_M8_R8() const final { return {22.50, 1, 21, 1, ALU | AGU | LSU | LOCK}; }
    code::Cost AND_LOCK_M8_R8() const final { return {22.52, 1, 21, 1, ALU | AGU | LSU | LOCK}; }
    code::Cost AND_R0_R8_I8() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost AND_R4_R8_I8() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost BSF_R16_M16() const final { return {4.00, 0, 9, 8, ALU | AGU | LSU}; }
    code::Cost BSF_R16_R16() const final { return {3.00, 0, 4, 6, ALU}; }
    code::Cost BSF_R32_M32() const final { return {4.00, 0, 9, 8, ALU | AGU | LSU}; }
    code::Cost BSF_R32_R32() const final { return {3.00, 0, 4, 6, ALU}; }
    code::Cost BSF_R64_M64() const final { return {4.00, 0, 9, 8, ALU | AGU | LSU}; }
    code::Cost BSF_R64_R64() const final { return {3.00, 0, 4, 6, ALU}; }
    code::Cost BSR_R16_M16() const final { return {5.00, 0, 9, 8, ALU | AGU | LSU}; }
    code::Cost BSR_R16_R16() const final { return {4.00, 0, 4, 6, ALU}; }
    code::Cost BSR_R32_M32() const final { return {5.00, 0, 9, 8, ALU | AGU | LSU}; }
    code::Cost BSR_R32_R32() const final { return {4.00, 0, 4, 6, ALU}; }
    code::Cost BSR_R64_M64() const final { return {5.00, 0, 9, 8, ALU | AGU | LSU}; }
    code::Cost BSR_R64_R64() const final { return {4.00, 0, 4, 6, ALU}; }
    code::Cost BSWAP_R32() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost BSWAP_R64() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost BT_M16_I8() const final { return {0.50, 5, 7, 1, ALU | AGU | LSU}; }
    code::Cost BT_M16_R16() const final { return {3.00, 7, 8, 5, ALU | AGU | LSU}; }
    code::Cost BT_M32_I8() const final { return {0.50, 5, 7, 1, ALU | AGU | LSU}; }
    code::Cost BT_M32_R32() const final { return {3.00, 7, 8, 5, ALU | AGU | LSU}; }
    code::Cost BT_M64_I8() const final { return {0.50, 5, 7, 1, ALU | AGU | LSU}; }
    code::Cost BT_M64_R64() const final { return {3.00, 7, 8, 5, ALU | AGU | LSU}; }
    code::Cost BT_R16_I8() const final { return {0.38, 1, 1, 1, ALU}; }
    code::Cost BT_R16_R16() const final { return {0.31, 1, 1, 1, ALU}; }
    code::Cost BT_R32_I8() const final { return {0.31, 1, 1, 1, ALU}; }
    code::Cost BT_R32_R32() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost BT_R64_I8() const final { return {0.31, 1, 1, 1, ALU}; }
    code::Cost BT_R64_R64() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost BTC_M16_I8() const final { return {2.00, 5, 12, 4, ALU | AGU | LSU}; }
    code::Cost BTC_M16_R16() const final { return {3.00, 7, 47, 8, ALU | AGU | LSU}; }
    code::Cost BTC_M32_I8() const final { return {2.00, 5, 11, 4, ALU | AGU | LSU}; }
    code::Cost BTC_M32_R32() const final { return {3.00, 7, 46, 8, ALU | AGU | LSU}; }
    code::Cost BTC_M64_I8() const final { return {2.00, 5, 11, 4, ALU | AGU | LSU}; }
    code::Cost BTC_M64_R64() const final { return {3.00, 7, 46, 8, ALU | AGU | LSU}; }
    code::Cost BTC_R16_I8() const final { return {0.50, 1, 2, 2, ALU}; }
    code::Cost BTC_R16_R16() const final { return {0.50, 1, 2, 2, ALU}; }
    code::Cost BTC_R32_I8() const final { return {0.50, 1, 2, 2, ALU}; }
    code::Cost BTC_R32_R32() const final { return {0.50, 1, 2, 2, ALU}; }
    code::Cost BTC_R64_I8() const final { return {0.50, 1, 2, 2, ALU}; }
    code::Cost BTC_R64_R64() const final { return {0.50, 1, 2, 2, ALU}; }
    code::Cost BTC_LOCK_M16_I8() const final { return {28.42, 7, 27, 4, ALU | AGU | LSU | LOCK}; }
    code::Cost BTC_LOCK_M16_R16() const final { return {27.70, 7, 45, 8, ALU | AGU | LSU | LOCK}; }
    code::Cost BTC_LOCK_M32_I8() const final { return {28.42, 7, 27, 4, ALU | AGU | LSU | LOCK}; }
    code::Cost BTC_LOCK_M32_R32() const final { return {27.70, 8, 44, 8, ALU | AGU | LSU | LOCK}; }
    code::Cost BTC_LOCK_M64_I8() const final { return {28.42, 8, 27, 4, ALU | AGU | LSU | LOCK}; }
    code::Cost BTC_LOCK_M64_R64() const final { return {27.70, 8, 44, 8, ALU | AGU | LSU | LOCK}; }
    code::Cost BTR_M16_I8() const final { return {2.00, 5, 12, 4, ALU | AGU | LSU}; }
    code::Cost BTR_M16_R16() const final { return {3.00, 7, 47, 8, ALU | AGU | LSU}; }
    code::Cost BTR_M32_I8() const final { return {2.00, 5, 11, 4, ALU | AGU | LSU}; }
    code::Cost BTR_M32_R32() const final { return {3.00, 7, 46, 8, ALU | AGU | LSU}; }
    code::Cost BTR_M64_I8() const final { return {2.00, 5, 11, 4, ALU | AGU | LSU}; }
    code::Cost BTR_M64_R64() const final { return {3.00, 7, 46, 8, ALU | AGU | LSU}; }
    code::Cost BTR_R16_I8() const final { return {0.50, 1, 2, 2, ALU}; }
    code::Cost BTR_R16_R16() const final { return {0.50, 1, 2, 2, ALU}; }
    code::Cost BTR_R32_I8() const final { return {0.50, 1, 2, 2, ALU}; }
    code::Cost BTR_R32_R32() const final { return {0.50, 1, 2, 2, ALU}; }
    code::Cost BTR_R64_I8() const final { return {0.50, 1, 2, 2, ALU}; }
    code::Cost BTR_R64_R64() const final { return {0.50, 1, 2, 2, ALU}; }
    code::Cost BTR_LOCK_M16_I8() const final { return {28.42, 7, 27, 4, ALU | AGU | LSU | LOCK}; }
    code::Cost BTR_LOCK_M16_R16() const final { return {27.70, 7, 45, 8, ALU | AGU | LSU | LOCK}; }
    code::Cost BTR_LOCK_M32_I8() const final { return {28.42, 7, 27, 4, ALU | AGU | LSU | LOCK}; }
    code::Cost BTR_LOCK_M32_R32() const final { return {27.70, 8, 44, 8, ALU | AGU | LSU | LOCK}; }
    code::Cost BTR_LOCK_M64_I8() const final { return {28.42, 8, 27, 4, ALU | AGU | LSU | LOCK}; }
    code::Cost BTR_LOCK_M64_R64() const final { return {27.70, 8, 44, 8, ALU | AGU | LSU | LOCK}; }
    code::Cost BTS_M16_I8() const final { return {2.00, 5, 12, 4, ALU | AGU | LSU}; }
    code::Cost BTS_M16_R16() const final { return {3.00, 7, 47, 8, ALU | AGU | LSU}; }
    code::Cost BTS_M32_I8() const final { return {2.00, 5, 11, 4, ALU | AGU | LSU}; }
    code::Cost BTS_M32_R32() const final { return {3.00, 7, 46, 8, ALU | AGU | LSU}; }
    code::Cost BTS_M64_I8() const final { return {2.00, 5, 11, 4, ALU | AGU | LSU}; }
    code::Cost BTS_M64_R64() const final { return {3.00, 7, 46, 8, ALU | AGU | LSU}; }
    code::Cost BTS_R16_I8() const final { return {0.50, 1, 2, 2, ALU}; }
    code::Cost BTS_R16_R16() const final { return {0.50, 1, 2, 2, ALU}; }
    code::Cost BTS_R32_I8() const final { return {0.50, 1, 2, 2, ALU}; }
    code::Cost BTS_R32_R32() const final { return {0.50, 1, 2, 2, ALU}; }
    code::Cost BTS_R64_I8() const final { return {0.50, 1, 2, 2, ALU}; }
    code::Cost BTS_R64_R64() const final { return {0.50, 1, 2, 2, ALU}; }
    code::Cost BTS_LOCK_M16_I8() const final { return {28.42, 7, 27, 4, ALU | AGU | LSU | LOCK}; }
    code::Cost BTS_LOCK_M16_R16() const final { return {27.70, 7, 45, 8, ALU | AGU | LSU | LOCK}; }
    code::Cost BTS_LOCK_M32_I8() const final { return {28.42, 7, 27, 4, ALU | AGU | LSU | LOCK}; }
    code::Cost BTS_LOCK_M32_R32() const final { return {27.70, 8, 44, 8, ALU | AGU | LSU | LOCK}; }
    code::Cost BTS_LOCK_M64_I8() const final { return {28.42, 8, 27, 4, ALU | AGU | LSU | LOCK}; }
    code::Cost BTS_LOCK_M64_R64() const final { return {27.70, 8, 44, 8, ALU | AGU | LSU | LOCK}; }
    code::Cost CALL_FAR_M32() const final { return {0.0, 0, 0, 0, ALU | AGU | LSU}; }
    code::Cost CALL_FAR_M48() const final { return {0.0, 0, 0, 0, ALU | AGU | LSU}; }
    code::Cost CALL_FAR_M80() const final { return {0.0, 0, 0, 0, ALU | AGU | LSU}; }
    code::Cost CALL_M64() const final { return {3.00, 0, 0, 5, ALU | AGU | LSU}; }
    code::Cost CALL_R64() const final { return {2.14, 0, 0, 2, ALU}; }
    code::Cost CALL_REL32() const final { return {1.00, 0, 0, 2, ALU}; }
    // TODO
    code::Cost CBW() const final { return {0.50, 1, 1, 1, ALU}; }
    code::Cost CDQ() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost CLC() const final { return {0.25, 0, 0, 1, ALU}; }
    code::Cost CLD() const final { return {3.00, 0, 0, 2, ALU}; }
    code::Cost CLI() const final { return {8.00, 0, 0, 6, ALU}; }
    code::Cost CLTS() const final { return {4.00, 0, 0, 10, ALU}; }
    code::Cost CMC() const final { return {0.50, 1, 1, 1, ALU}; }
    code::Cost CMOVB_R16_M16() const final { return {0.50, 1, 7, 1, ALU | AGU | LSU}; }
    code::Cost CMOVB_R16_R16() const final { return {0.33, 1, 1, 1, ALU}; }
    code::Cost CMOVB_R32_M32() const final { return {0.50, 1, 7, 1, ALU | AGU | LSU}; }
    code::Cost CMOVB_R32_R32() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost CMOVB_R64_M64() const final { return {0.50, 1, 7, 1, ALU | AGU | LSU}; }
    code::Cost CMOVB_R64_R64() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost CMOVBE_R16_M16() const final { return {1.13, 1, 7, 2, ALU | AGU | LSU}; }
    code::Cost CMOVBE_R16_R16() const final { return {1.12, 1, 2, 2, ALU}; }
    code::Cost CMOVBE_R32_M32() const final { return {1.13, 1, 7, 2, ALU | AGU | LSU}; }
    code::Cost CMOVBE_R32_R32() const final { return {1.12, 1, 2, 2, ALU}; }
    code::Cost CMOVBE_R64_M64() const final { return {1.13, 1, 7, 2, ALU | AGU | LSU}; }
    code::Cost CMOVBE_R64_R64() const final { return {1.12, 1, 2, 2, ALU}; }
    code::Cost CMOVL_R16_M16() const final { return {0.67, 1, 7, 2, ALU | AGU | LSU}; }
    code::Cost CMOVL_R16_R16() const final { return {0.67, 1, 2, 2, ALU}; }
    code::Cost CMOVL_R32_M32() const final { return {0.67, 1, 7, 2, ALU | AGU | LSU}; }
    code::Cost CMOVL_R32_R32() const final { return {0.67, 1, 2, 2, ALU}; }
    code::Cost CMOVL_R64_M64() const final { return {0.67, 1, 7, 2, ALU | AGU | LSU}; }
    code::Cost CMOVL_R64_R64() const final { return {0.67, 1, 2, 2, ALU}; }
    code::Cost CMOVLE_R16_M16() const final { return {0.67, 1, 7, 2, ALU | AGU | LSU}; }
    code::Cost CMOVLE_R16_R16() const final { return {0.67, 1, 2, 2, ALU}; }
    code::Cost CMOVLE_R32_M32() const final { return {0.67, 1, 7, 2, ALU | AGU | LSU}; }
    code::Cost CMOVLE_R32_R32() const final { return {0.67, 1, 2, 2, ALU}; }
    code::Cost CMOVLE_R64_M64() const final { return {0.67, 1, 7, 2, ALU | AGU | LSU}; }
    code::Cost CMOVLE_R64_R64() const final { return {0.67, 1, 2, 2, ALU}; }
    code::Cost CMOVNB_R16_M16() const final { return {0.50, 1, 7, 1, ALU | AGU | LSU}; }
    code::Cost CMOVNB_R16_R16() const final { return {0.33, 1, 1, 1, ALU}; }
    code::Cost CMOVNB_R32_M32() const final { return {0.50, 1, 7, 1, ALU | AGU | LSU}; }
    code::Cost CMOVNB_R32_R32() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost CMOVNB_R64_M64() const final { return {0.50, 1, 7, 1, ALU | AGU | LSU}; }
    code::Cost CMOVNB_R64_R64() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost CMOVNBE_R16_M16() const final { return {1.13, 1, 7, 2, ALU | AGU | LSU}; }
    code::Cost CMOVNBE_R16_R16() const final { return {1.12, 1, 2, 2, ALU}; }
    code::Cost CMOVNBE_R32_M32() const final { return {1.13, 1, 7, 2, ALU | AGU | LSU}; }
    code::Cost CMOVNBE_R32_R32() const final { return {1.12, 1, 2, 2, ALU}; }
    code::Cost CMOVNBE_R64_M64() const final { return {1.13, 1, 7, 2, ALU | AGU | LSU}; }
    code::Cost CMOVNBE_R64_R64() const final { return {1.12, 1, 2, 2, ALU}; }
    code::Cost CMOVNL_R16_M16() const final { return {0.67, 1, 7, 2, ALU | AGU | LSU}; }
    code::Cost CMOVNL_R16_R16() const final { return {0.67, 1, 2, 2, ALU}; }
    code::Cost CMOVNL_R32_M32() const final { return {0.67, 1, 7, 2, ALU | AGU | LSU}; }
    code::Cost CMOVNL_R32_R32() const final { return {0.67, 1, 2, 2, ALU}; }
    code::Cost CMOVNL_R64_M64() const final { return {0.67, 1, 7, 2, ALU | AGU | LSU}; }
    code::Cost CMOVNL_R64_R64() const final { return {0.67, 1, 2, 2, ALU}; }
    code::Cost CMOVNLE_R16_M16() const final { return {0.67, 1, 7, 2, ALU | AGU | LSU}; }
    code::Cost CMOVNLE_R16_R16() const final { return {0.67, 1, 2, 2, ALU}; }
    code::Cost CMOVNLE_R32_M32() const final { return {0.67, 1, 7, 2, ALU | AGU | LSU}; }
    code::Cost CMOVNLE_R32_R32() const final { return {0.67, 1, 2, 2, ALU}; }
    code::Cost CMOVNLE_R64_M64() const final { return {0.67, 1, 7, 2, ALU | AGU | LSU}; }
    code::Cost CMOVNLE_R64_R64() const final { return {0.67, 1, 2, 2, ALU}; }
    code::Cost CMOVNO_R16_M16() const final { return {0.50, 1, 7, 1, ALU | AGU | LSU}; }
    code::Cost CMOVNO_R16_R16() const final { return {0.33, 1, 1, 1, ALU}; }
    code::Cost CMOVNO_R32_M32() const final { return {0.50, 1, 7, 1, ALU | AGU | LSU}; }
    code::Cost CMOVNO_R32_R32() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost CMOVNO_R64_M64() const final { return {0.50, 1, 7, 1, ALU | AGU | LSU}; }
    code::Cost CMOVNO_R64_R64() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost CMOVNP_R16_M16() const final { return {0.50, 1, 7, 1, ALU | AGU | LSU}; }
    code::Cost CMOVNP_R16_R16() const final { return {0.33, 1, 1, 1, ALU}; }
    code::Cost CMOVNP_R32_M32() const final { return {0.50, 1, 7, 1, ALU | AGU | LSU}; }
    code::Cost CMOVNP_R32_R32() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost CMOVNP_R64_M64() const final { return {0.50, 1, 7, 1, ALU | AGU | LSU}; }
    code::Cost CMOVNP_R64_R64() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost CMOVNS_R16_M16() const final { return {0.50, 1, 7, 1, ALU | AGU | LSU}; }
    code::Cost CMOVNS_R16_R16() const final { return {0.33, 1, 1, 1, ALU}; }
    code::Cost CMOVNS_R32_M32() const final { return {0.50, 1, 7, 1, ALU | AGU | LSU}; }
    code::Cost CMOVNS_R32_R32() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost CMOVNS_R64_M64() const final { return {0.50, 1, 7, 1, ALU | AGU | LSU}; }
    code::Cost CMOVNS_R64_R64() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost CMOVNZ_R16_M16() const final { return {0.50, 1, 7, 1, ALU | AGU | LSU}; }
    code::Cost CMOVNZ_R16_R16() const final { return {0.33, 1, 1, 1, ALU}; }
    code::Cost CMOVNZ_R32_M32() const final { return {0.50, 1, 7, 1, ALU | AGU | LSU}; }
    code::Cost CMOVNZ_R32_R32() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost CMOVNZ_R64_M64() const final { return {0.50, 1, 7, 1, ALU | AGU | LSU}; }
    code::Cost CMOVNZ_R64_R64() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost CMOVO_R16_M16() const final { return {0.50, 1, 7, 1, ALU | AGU | LSU}; }
    code::Cost CMOVO_R16_R16() const final { return {0.33, 1, 1, 1, ALU}; }
    code::Cost CMOVO_R32_M32() const final { return {0.50, 1, 7, 1, ALU | AGU | LSU}; }
    code::Cost CMOVO_R32_R32() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost CMOVO_R64_M64() const final { return {0.50, 1, 7, 1, ALU | AGU | LSU}; }
    code::Cost CMOVO_R64_R64() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost CMOVP_R16_M16() const final { return {0.50, 1, 7, 1, ALU | AGU | LSU}; }
    code::Cost CMOVP_R16_R16() const final { return {0.33, 1, 1, 1, ALU}; }
    code::Cost CMOVP_R32_M32() const final { return {0.50, 1, 7, 1, ALU | AGU | LSU}; }
    code::Cost CMOVP_R32_R32() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost CMOVP_R64_M64() const final { return {0.50, 1, 7, 1, ALU | AGU | LSU}; }
    code::Cost CMOVP_R64_R64() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost CMOVS_R16_M16() const final { return {0.50, 1, 7, 1, ALU | AGU | LSU}; }
    code::Cost CMOVS_R16_R16() const final { return {0.33, 1, 1, 1, ALU}; }
    code::Cost CMOVS_R32_M32() const final { return {0.50, 1, 7, 1, ALU | AGU | LSU}; }
    code::Cost CMOVS_R32_R32() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost CMOVS_R64_M64() const final { return {0.50, 1, 7, 1, ALU | AGU | LSU}; }
    code::Cost CMOVS_R64_R64() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost CMOVZ_R16_M16() const final { return {0.50, 1, 7, 1, ALU | AGU | LSU}; }
    code::Cost CMOVZ_R16_R16() const final { return {0.33, 1, 1, 1, ALU}; }
    code::Cost CMOVZ_R32_M32() const final { return {0.50, 1, 7, 1, ALU | AGU | LSU}; }
    code::Cost CMOVZ_R32_R32() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost CMOVZ_R64_M64() const final { return {0.50, 1, 7, 1, ALU | AGU | LSU}; }
    code::Cost CMOVZ_R64_R64() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost CMP_AL_I8() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost CMP_AX_I16() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost CMP_EAX_I32() const final { return {0.31, 1, 1, 1, ALU}; }
    code::Cost CMP_M16_I16() const final { return {0.50, 5, 7, 1, ALU | AGU | LSU}; }
    code::Cost CMP_M16_I8() const final { return {0.50, 5, 7, 1, ALU | AGU | LSU}; }
    code::Cost CMP_M16_R16() const final { return {0.50, 1, 7, 1, ALU | AGU | LSU}; }
    code::Cost CMP_M32_I32() const final { return {0.50, 5, 7, 1, ALU | AGU | LSU}; }
    code::Cost CMP_M32_I8() const final { return {0.50, 5, 7, 1, ALU | AGU | LSU}; }
    code::Cost CMP_M32_R32() const final { return {0.50, 1, 7, 1, ALU | AGU | LSU}; }
    code::Cost CMP_M64_I32() const final { return {0.50, 5, 7, 1, ALU | AGU | LSU}; }
    code::Cost CMP_M64_I8() const final { return {0.50, 5, 7, 1, ALU | AGU | LSU}; }
    code::Cost CMP_M64_R64() const final { return {0.50, 1, 7, 1, ALU | AGU | LSU}; }
    code::Cost CMP_M8_I8() const final { return {0.50, 5, 7, 1, ALU | AGU | LSU}; }
    code::Cost CMP_M8_R8() const final { return {0.50, 1, 7, 1, ALU | AGU | LSU}; }
    code::Cost CMP_M8_R8() const final { return {0.50, 1, 7, 1, ALU | AGU | LSU}; }
    code::Cost CMP_R16_I16() const final { return {0.38, 1, 1, 1, ALU}; }
    code::Cost CMP_R16_I8() const final { return {0.31, 1, 1, 1, ALU}; }
    code::Cost CMP_R16_M16() const final { return {0.50, 1, 7, 1, ALU | AGU | LSU}; }
    code::Cost CMP_R32_I32() const final { return {0.44, 1, 1, 1, ALU}; }
    code::Cost CMP_R32_I8() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost CMP_R32_M32() const final { return {0.50, 1, 7, 1, ALU | AGU | LSU}; }
    code::Cost CMP_R64_I32() const final { return {0.44, 1, 1, 1, ALU}; }
    code::Cost CMP_R64_I8() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost CMP_R64_M64() const final { return {0.50, 1, 7, 1, ALU | AGU | LSU}; }
    code::Cost CMP_R8_I8() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost CMP_R8_M8() const final { return {0.50, 1, 7, 1, ALU | AGU | LSU}; }
    code::Cost CMP_RAX_I32() const final { return {0.38, 1, 1, 1, ALU}; }
    code::Cost CMP_38_R8_R8() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost CMP_39_R16_R16() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost CMP_39_R32_R32() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost CMP_39_R64_R64() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost CMP_3A_R8_R8() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost CMP_3B_R16_R16() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost CMP_3B_R32_R32() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost CMP_3B_R64_R64() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost CMP_R0_R8_I8() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost CMP_R4_R8_I8() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost CMPSB() const final { return {3.00, 3, 7, 6, ALU}; }
    code::Cost CMPSB_REPE() const final { return {18.00, 0, 0, 3, ALU}; }
    code::Cost CMPSB_REPNE() const final { return {18.00, 0, 0, 3, ALU}; }
    code::Cost CMPSD() const final { return {3.00, 3, 7, 6, ALU}; }
    code::Cost CMPSD_REPE() const final { return {18.00, 0, 0, 3, ALU}; }
    code::Cost CMPSD_REPNE() const final { return {18.00, 0, 0, 3, ALU}; }
    code::Cost CMPSW() const final { return {3.00, 3, 7, 6, ALU}; }
    code::Cost CMPSW_REPE() const final { return {18.00, 0, 0, 3, ALU}; }
    code::Cost CMPSW_REPNE() const final { return {18.00, 0, 0, 3, ALU}; }
    code::Cost CMPXCHG_M16_R16() const final { return {3.00, 0, 12, 6, ALU | AGU | LSU}; }
    code::Cost CMPXCHG_M32_R32() const final { return {3.00, 0, 11, 6, ALU | AGU | LSU}; }
    code::Cost CMPXCHG_M64_R64() const final { return {3.00, 0, 11, 6, ALU | AGU | LSU}; }
    code::Cost CMPXCHG_M8_R8() const final { return {3.00, 1, 12, 5, ALU | AGU | LSU}; }
    code::Cost CMPXCHG_R16_R16() const final { return {3.00, 0, 2, 5, ALU}; }
    code::Cost CMPXCHG_R32_R32() const final { return {3.00, 0, 2, 5, ALU}; }
    code::Cost CMPXCHG_R64_R64() const final { return {3.00, 0, 2, 5, ALU}; }
    code::Cost CMPXCHG_R8_R8() const final { return {2.00, 0, 3, 3, ALU}; }
    code::Cost CMPXCHG8B_M64() const final { return {6.00, 0, 16, 18, ALU | AGU | LSU}; }
    code::Cost CMPXCHG8B_LOCK_M64() const final { return {26.42, 0, 25, 18, ALU | AGU | LSU | LOCK}; }
    code::Cost CMPXCHG_LOCK_M16_R16() const final { return {27.90, 0, 27, 6, ALU | AGU | LSU | LOCK}; }
    code::Cost CMPXCHG_LOCK_M32_R32() const final { return {27.90, 0, 27, 6, ALU | AGU | LSU | LOCK}; }
    code::Cost CMPXCHG_LOCK_M64_R64() const final { return {27.90, 0, 27, 6, ALU | AGU | LSU | LOCK}; }
    code::Cost CMPXCHG_LOCK_M8_R8() const final { return {28.07, 1, 27, 5, ALU | AGU | LSU | LOCK}; }
    code::Cost CPUID() const final { return {121.50, 0, 0, 68, ALU}; }
    code::Cost CWD() const final { return {1.00, 2, 2, 2, ALU}; }
    code::Cost CWDE() const final { return {0.50, 1, 1, 1, ALU}; }
    code::Cost DEC_M16() const final { return {1.00, 5, 9, 1, ALU | AGU | LSU}; }
    code::Cost DEC_M32() const final { return {1.00, 5, 8, 1, ALU | AGU | LSU}; }
    code::Cost DEC_M64() const final { return {1.00, 5, 8, 1, ALU | AGU | LSU}; }
    code::Cost DEC_M8() const final { return {1.00, 5, 9, 1, ALU | AGU | LSU}; }
    code::Cost DEC_R16() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost DEC_R32() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost DEC_R64() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost DEC_R8() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost DEC_R8() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost DEC_LOCK_M16() const final { return {22.52, 7, 21, 1, ALU | AGU | LSU | LOCK}; }
    code::Cost DEC_LOCK_M32() const final { return {22.52, 7, 21, 1, ALU | AGU | LSU | LOCK}; }
    code::Cost DEC_LOCK_M64() const final { return {22.52, 7, 21, 1, ALU | AGU | LSU | LOCK}; }
    code::Cost DEC_LOCK_M8() const final { return {22.52, 7, 21, 1, ALU | AGU | LSU | LOCK}; }
    code::Cost DIV_M16() const final { return {14.00, 8.0, 21, 2, ALU | AGU | LSU}; }
    code::Cost DIV_M32() const final { return {14.00, 8.0, 29, 2, ALU | AGU | LSU}; }
    code::Cost DIV_M64() const final { return {14.00, 8.0, 45, 2, ALU | AGU | LSU}; }
    code::Cost DIV_M8() const final { return {13.00, 9.0, 16, 1, ALU | AGU | LSU}; }
    code::Cost DIV_R16() const final { return {14.00, 8.0, 17, 2, ALU}; }
    code::Cost DIV_R32() const final { return {14.00, 8.0, 25, 2, ALU}; }
    code::Cost DIV_R64() const final { return {14.00, 8.0, 41, 2, ALU}; }
    code::Cost DIV_R8() const final { return {13.00, 9.0, 12, 1, ALU}; }
    code::Cost DIV_R8() const final { return {13.00, 9.0, 12, 1, ALU}; }
    code::Cost ENTER_I16_I8() const final { return {21.20, 27, 27, 17, ALU}; }
    code::Cost ENTER_W_I16_I8() const final { return {21.00, 9, 9, 17, ALU}; }
    code::Cost HLT() const final { return {0.0, 0, 0, 0}; }
    code::Cost IDIV_M16() const final { return {14.00, 8.0, 21, 2, ALU | AGU | LSU}; }
    code::Cost IDIV_M32() const final { return {14.00, 8.0, 29, 2, ALU | AGU | LSU}; }
    code::Cost IDIV_M64() const final { return {14.00, 8.0, 45, 2, ALU | AGU | LSU}; }
    code::Cost IDIV_M8() const final { return {13.00, 9.0, 16, 1, ALU | AGU | LSU}; }
    code::Cost IDIV_R16() const final { return {14.00, 8.0, 17, 2, ALU}; }
    code::Cost IDIV_R32() const final { return {14.00, 8.0, 25, 2, ALU}; }
    code::Cost IDIV_R64() const final { return {14.00, 8.0, 41, 2, ALU}; }
    code::Cost IDIV_R8() const final { return {13.00, 9.0, 12, 1, ALU}; }
    code::Cost IDIV_R8() const final { return {13.00, 9.0, 12, 1, ALU}; }
    code::Cost IMUL_M16() const final { return {2.00, 0, 11, 3, ALU | AGU | LSU}; }
    code::Cost IMUL_M32() const final { return {2.00, 3, 10, 2, ALU | AGU | LSU}; }
    code::Cost IMUL_M64() const final { return {2.00, 3, 10, 2, ALU | AGU | LSU}; }
    code::Cost IMUL_M8() const final { return {1.00, 3, 9, 1, ALU | AGU | LSU}; }
    code::Cost IMUL_R16() const final { return {2.00, 0, 5, 3, ALU}; }
    code::Cost IMUL_R16_M16() const final { return {1.00, 3, 9, 1, ALU | AGU | LSU}; }
    code::Cost IMUL_R16_M16_I16() const final { return {1.00, 4, 10, 2, ALU | AGU | LSU}; }
    code::Cost IMUL_R16_M16_I8() const final { return {1.00, 4, 10, 2, ALU | AGU | LSU}; }
    code::Cost IMUL_R16_R16() const final { return {1.00, 3, 3, 1, ALU}; }
    code::Cost IMUL_R32() const final { return {2.00, 3, 4, 2, ALU}; }
    code::Cost IMUL_R32_M32() const final { return {1.00, 3, 9, 1, ALU | AGU | LSU}; }
    code::Cost IMUL_R32_M32_I32() const final { return {1.00, 7, 9, 2, ALU | AGU | LSU}; }
    code::Cost IMUL_R32_M32_I8() const final { return {1.00, 7, 9, 2, ALU | AGU | LSU}; }
    code::Cost IMUL_R32_R32() const final { return {1.00, 3, 3, 1, ALU}; }
    code::Cost IMUL_R64() const final { return {2.00, 3, 4, 2, ALU}; }
    code::Cost IMUL_R64_M64() const final { return {1.00, 3, 9, 1, ALU | AGU | LSU}; }
    code::Cost IMUL_R64_M64_I32() const final { return {1.00, 7, 9, 2, ALU | AGU | LSU}; }
    code::Cost IMUL_R64_M64_I8() const final { return {1.00, 7, 9, 2, ALU | AGU | LSU}; }
    code::Cost IMUL_R64_R64() const final { return {1.00, 3, 3, 1, ALU}; }
    code::Cost IMUL_R8() const final { return {1.00, 3, 3, 1, ALU}; }
    code::Cost IMUL_R8() const final { return {1.00, 3, 3, 1, ALU}; }
    code::Cost IMUL_IMMB_R16_R16_I8() const final { return {1.00, 4, 4, 2, ALU}; }
    code::Cost IMUL_IMMB_R32_R32_I8() const final { return {1.00, 3, 3, 1, ALU}; }
    code::Cost IMUL_IMMB_R64_R64_I8() const final { return {1.00, 3, 3, 1, ALU}; }
    code::Cost IMUL_IMMZ_R16_R16_I16() const final { return {1.00, 4, 4, 2, ALU}; }
    code::Cost IMUL_IMMZ_R32_R32_I32() const final { return {1.00, 3, 3, 1, ALU}; }
    code::Cost IMUL_IMMZ_R64_R64_I32() const final { return {1.00, 3, 3, 1, ALU}; }
    code::Cost IN_AL_DX() const final { return {9571.02, 0, 0, 52, ALU}; }
    code::Cost IN_AL_I8() const final { return {9572.53, 0, 0, 52, ALU}; }
    code::Cost IN_AX_DX() const final { return {9571.05, 0, 0, 52, ALU}; }
    code::Cost IN_AX_I8() const final { return {9571.07, 0, 0, 52, ALU}; }
    code::Cost IN_EAX_DX() const final { return {11299.55, 0, 0, 52, ALU}; }
    code::Cost IN_EAX_I8() const final { return {10527.08, 0, 0, 52, ALU}; }
    code::Cost INC_M16() const final { return {1.00, 5, 9, 1, ALU | AGU | LSU}; }
    code::Cost INC_M32() const final { return {1.00, 5, 8, 1, ALU | AGU | LSU}; }
    code::Cost INC_M64() const final { return {1.00, 5, 8, 1, ALU | AGU | LSU}; }
    code::Cost INC_M8() const final { return {1.00, 5, 9, 1, ALU | AGU | LSU}; }
    code::Cost INC_R16() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost INC_R32() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost INC_R64() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost INC_R8() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost INC_R8() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost INC_LOCK_M16() const final { return {22.52, 7, 21, 1, ALU | AGU | LSU | LOCK}; }
    code::Cost INC_LOCK_M32() const final { return {22.52, 7, 21, 1, ALU | AGU | LSU | LOCK}; }
    code::Cost INC_LOCK_M64() const final { return {22.52, 7, 21, 1, ALU | AGU | LSU | LOCK}; }
    code::Cost INC_LOCK_M8() const final { return {22.52, 7, 21, 1, ALU | AGU | LSU | LOCK}; }

    code::Cost INSB() const final { return {9571.07, 0, 0, 28, ALU | AGU | LSU}; }
    code::Cost INSB_REPE() const final { return {29.00, 0, 0, 16, ALU | AGU | LSU}; }
    code::Cost INSB_REPNE() const final { return {29.00, 0, 0, 16, ALU | AGU | LSU}; }
    code::Cost INSD() const final { return {11290.82, 0, 0, 28, ALU | AGU | LSU}; }
    code::Cost INSD_REPE() const final { return {29.00, 0, 0, 16, ALU | AGU | LSU}; }
    code::Cost INSD_REPE_REX64() const final { return {29.00, 0, 0, 16, ALU | AGU | LSU}; }
    code::Cost INSD_REPNE() const final { return {29.00, 0, 0, 16, ALU | AGU | LSU}; }
    code::Cost INSD_REPNE_REX64() const final { return {29.00, 0, 0, 16, ALU | AGU | LSU}; }
    code::Cost INSW() const final { return {9571.02, 0, 0, 28, ALU | AGU | LSU}; }
    code::Cost INSW_REPE() const final { return {29.00, 0, 0, 16, ALU | AGU | LSU}; }
    code::Cost INSW_REPNE() const final { return {29.00, 0, 0, 16, ALU | AGU | LSU}; }
    code::Cost INT_I8() const final { return {0.0, 0, 0, 0, ALU}; }
    code::Cost INT3() const final { return {0.0, 0, 0, 0, ALU}; }
    code::Cost INVD() const final { return {0.0, 0, 0, 0}; }  // Specialized operation
    code::Cost INVLPG_M8() const final { return {126.00, 0, 0, 24, ALU | AGU | LSU}; }
    code::Cost IRET() const final { return {0.0, 0, 0, 0, ALU | AGU | LSU}; }
    code::Cost IRETD() const final { return {0.0, 0, 0, 0, ALU | AGU | LSU}; }
    code::Cost JB_REL32() const final { return {0.50, 0, 0, 1, ALU | BRANCH}; }
    code::Cost JB_REL8() const final { return {0.50, 0, 0, 1, ALU | BRANCH}; }
    code::Cost JBE_REL32() const final { return {0.50, 0, 0, 1, ALU | BRANCH}; }
    code::Cost JBE_REL8() const final { return {0.50, 0, 0, 1, ALU | BRANCH}; }
    code::Cost JL_REL32() const final { return {0.50, 0, 0, 1, ALU | BRANCH}; }
    code::Cost JL_REL8() const final { return {0.50, 0, 0, 1, ALU | BRANCH}; }
    code::Cost JLE_REL32() const final { return {0.50, 0, 0, 1, ALU | BRANCH}; }
    code::Cost JLE_REL8() const final { return {0.50, 0, 0, 1, ALU | BRANCH}; }
    code::Cost JMP_M64() const final { return {2.30, 0, 0, 1, ALU | AGU | LSU | BRANCH}; }
    code::Cost JMP_R64() const final { return {2.14, 0, 0, 1, ALU | BRANCH}; }
    code::Cost JMP_REL32() const final { return {2.08, 0, 0, 1, ALU | BRANCH}; }
    code::Cost JMP_REL8() const final { return {2.00, 0, 0, 1, ALU | BRANCH}; }
    code::Cost JMP_FAR_M32() const final { return {0.0, 0, 0, 0, ALU | AGU | LSU | BRANCH}; }
    code::Cost JMP_FAR_M48() const final { return {0.0, 0, 0, 0, ALU | AGU | LSU | BRANCH}; }
    code::Cost JMP_FAR_M80() const final { return {0.0, 0, 0, 0, ALU | AGU | LSU | BRANCH}; }
    code::Cost JNB_REL32() const final { return {0.50, 0, 0, 1, ALU | BRANCH}; }
    code::Cost JNB_REL8() const final { return {0.50, 0, 0, 1, ALU | BRANCH}; }
    code::Cost JNBE_REL32() const final { return {0.50, 0, 0, 1, ALU | BRANCH}; }
    code::Cost JNBE_REL8() const final { return {0.50, 0, 0, 1, ALU | BRANCH}; }
    code::Cost JNL_REL32() const final { return {0.50, 0, 0, 1, ALU | BRANCH}; }
    code::Cost JNL_REL8() const final { return {0.50, 0, 0, 1, ALU | BRANCH}; }
    code::Cost JNLE_REL32() const final { return {0.50, 0, 0, 1, ALU | BRANCH}; }
    code::Cost JNLE_REL8() const final { return {0.50, 0, 0, 1, ALU | BRANCH}; }
    code::Cost JNO_REL32() const final { return {0.50, 0, 0, 1, ALU | BRANCH}; }
    code::Cost JNO_REL8() const final { return {0.50, 0, 0, 1, ALU | BRANCH}; }
    code::Cost JNP_REL32() const final { return {0.50, 0, 0, 1, ALU | BRANCH}; }
    code::Cost JNP_REL8() const final { return {0.50, 0, 0, 1, ALU | BRANCH}; }
    code::Cost JNS_REL32() const final { return {0.50, 0, 0, 1, ALU | BRANCH}; }
    code::Cost JNS_REL8() const final { return {0.50, 0, 0, 1, ALU | BRANCH}; }
    code::Cost JNZ_REL32() const final { return {0.50, 0, 0, 1, ALU | BRANCH}; }
    code::Cost JNZ_REL8() const final { return {0.50, 0, 0, 1, ALU | BRANCH}; }
    code::Cost JO_REL32() const final { return {0.50, 0, 0, 1, ALU | BRANCH}; }
    code::Cost JO_REL8() const final { return {0.50, 0, 0, 1, ALU | BRANCH}; }
    code::Cost JP_REL32() const final { return {0.50, 0, 0, 1, ALU | BRANCH}; }
    code::Cost JP_REL8() const final { return {0.50, 0, 0, 1, ALU | BRANCH}; }
    code::Cost JRCXZ_REL8() const final { return {0.50, 0, 0, 1, ALU | BRANCH}; }
    code::Cost JS_REL32() const final { return {0.50, 0, 0, 1, ALU | BRANCH}; }
    code::Cost JS_REL8() const final { return {0.50, 0, 0, 1, ALU | BRANCH}; }
    code::Cost JZ_REL32() const final { return {0.50, 0, 0, 1, ALU | BRANCH}; }
    code::Cost JZ_REL8() const final { return {0.50, 0, 0, 1, ALU | BRANCH}; }
    code::Cost LAHF() const final { return {2.00, 3, 3, 4, ALU}; }
    code::Cost LAR_R16_M16() const final { return {99.25, 87, 98, 32, ALU | AGU | LSU}; }
    code::Cost LAR_R16_R16() const final { return {96.25, 87, 91, 32, ALU}; }
    code::Cost LAR_R32_M16() const final { return {99.00, 86, 98, 32, ALU | AGU | LSU}; }
    code::Cost LAR_R32_R32() const final { return {96.00, 86, 91, 32, ALU}; }
    code::Cost LAR_R64_M16() const final { return {99.00, 86, 98, 32, ALU | AGU | LSU}; }
    code::Cost LAR_R64_R64() const final { return {96.00, 87, 91, 32, ALU}; }
    code::Cost LEA_B_R16() const final { return {0.50, 1, 2, 2, AGU}; }
    code::Cost LEA_B_R32() const final { return {0.25, 1, 1, 1, AGU}; }
    code::Cost LEA_B_R64() const final { return {0.25, 1, 1, 1, AGU}; }
    code::Cost LEA_B_D32_R16() const final { return {0.50, 1, 2, 2, AGU}; }
    code::Cost LEA_B_D32_R32() const final { return {0.44, 1, 1, 1, AGU}; }
    code::Cost LEA_B_D32_R64() const final { return {0.44, 1, 1, 1, AGU}; }
    code::Cost LEA_B_D8_R16() const final { return {0.50, 1, 2, 2, AGU}; }
    code::Cost LEA_B_D8_R32() const final { return {0.25, 1, 1, 1, AGU}; }
    code::Cost LEA_B_D8_R64() const final { return {0.25, 1, 1, 1, AGU}; }
    code::Cost LEA_B_I_R16() const final { return {0.50, 1, 2, 2, AGU}; }
    code::Cost LEA_B_I_R32() const final { return {0.25, 1, 1, 1, AGU}; }
    code::Cost LEA_B_I_R64() const final { return {0.25, 1, 1, 1, AGU}; }
    code::Cost LEA_B_I_D32_R16() const final { return {1.00, 1, 3, 2, AGU}; }
    code::Cost LEA_B_I_D32_R32() const final { return {0.54, 2, 2, 1, AGU}; }
    code::Cost LEA_B_I_D32_R64() const final { return {0.55, 2, 2, 1, AGU}; }
    code::Cost LEA_B_I_D8_R16() const final { return {0.75, 1, 3, 2, AGU}; }
    code::Cost LEA_B_I_D8_R32() const final { return {0.54, 2, 2, 1, AGU}; }
    code::Cost LEA_B_I_D8_R64() const final { return {0.54, 2, 2, 1, AGU}; }
    code::Cost LEA_B_IS_R16() const final { return {0.75, 1, 3, 2, AGU}; }
    code::Cost LEA_B_IS_R32() const final { return {0.53, 2, 2, 1, AGU}; }
    code::Cost LEA_B_IS_R64() const final { return {0.54, 2, 2, 1, AGU}; }
    code::Cost LEA_B_IS_D32_R16() const final { return {1.00, 1, 3, 2, AGU}; }
    code::Cost LEA_B_IS_D32_R32() const final { return {0.53, 2, 2, 1, AGU}; }
    code::Cost LEA_B_IS_D32_R64() const final { return {0.53, 2, 2, 1, AGU}; }
    code::Cost LEA_B_IS_D8_R16() const final { return {0.75, 1, 3, 2, AGU}; }
    code::Cost LEA_B_IS_D8_R32() const final { return {0.54, 2, 2, 1, AGU}; }
    code::Cost LEA_B_IS_D8_R64() const final { return {0.54, 2, 2, 1, AGU}; }
    code::Cost LEA_D32_R16() const final { return {0.75, 1, 1, 2, AGU}; }
    code::Cost LEA_D32_R32() const final { return {0.47, 0, 0, 1, AGU}; }
    code::Cost LEA_D32_R64() const final { return {0.50, 0, 0, 1, AGU}; }
    code::Cost LEA_I_D32_R16() const final { return {1.00, 1, 2, 2, AGU}; }
    code::Cost LEA_I_D32_R32() const final { return {0.50, 1, 1, 1, AGU}; }
    code::Cost LEA_I_D32_R64() const final { return {0.50, 1, 1, 1, AGU}; }
    code::Cost LEA_IS_D32_R16() const final { return {1.02, 1, 3, 2, AGU}; }
    code::Cost LEA_IS_D32_R32() const final { return {0.54, 2, 2, 1, AGU}; }
    code::Cost LEA_IS_D32_R64() const final { return {0.54, 2, 2, 1, AGU}; }
    code::Cost LEA_R_D32_R16() const final { return {0.73, 0, 0, 2, AGU}; }
    code::Cost LEA_R_D32_R32() const final { return {0.54, 0, 0, 1, AGU}; }
    code::Cost LEA_R_D32_R64() const final { return {0.54, 0, 0, 1, AGU}; }
    code::Cost LEAVE() const final { return {0.50, 1, 4, 2, ALU}; }
    code::Cost LEAVE_W() const final { return {1.00, 1, 5, 2, ALU}; }
    code::Cost LFS_R16_M32() const final { return {0.0, 0, 0, 0, ALU | AGU | LSU}; }
    code::Cost LFS_R32_M48() const final { return {0.0, 0, 0, 0, ALU | AGU | LSU}; }
    code::Cost LFS_R64_M80() const final { return {0.0, 0, 0, 0, ALU | AGU | LSU}; }
    code::Cost LGDT_M80() const final { return {84.00, 0, 0, 27, ALU | AGU | LSU}; }
    code::Cost LGS_R16_M32() const final { return {0.0, 0, 0, 0, ALU | AGU | LSU}; }
    code::Cost LGS_R32_M48() const final { return {0.0, 0, 0, 0, ALU | AGU | LSU}; }
    code::Cost LGS_R64_M80() const final { return {0.0, 0, 0, 0, ALU | AGU | LSU}; }
    code::Cost LIDT_M80() const final { return {84.00, 0, 0, 27, ALU | AGU | LSU}; }
    code::Cost LLDT_M16() const final { return {214.00, 0, 0, 62, ALU | AGU | LSU}; }
    code::Cost LLDT_R16() const final { return {215.00, 0, 0, 62, ALU}; }
    code::Cost LMSW_M16() const final { return {5.00, 0, 0, 16, ALU | AGU | LSU}; }
    code::Cost LMSW_R16() const final { return {5.00, 0, 0, 15, ALU}; }
    code::Cost LODSB() const final { return {3.00, 5, 7, 3, ALU}; }
    code::Cost LODSB_REPE() const final { return {18.00, 0, 0, 3, ALU}; }
    code::Cost LODSB_REPNE() const final { return {18.00, 0, 0, 3, ALU}; }
    code::Cost LODSD() const final { return {3.00, 4, 6, 3, ALU}; }
    code::Cost LODSD_REPE() const final { return {18.00, 0, 0, 3, ALU}; }
    code::Cost LODSD_REPNE() const final { return {18.00, 0, 0, 3, ALU}; }
    code::Cost LODSW() const final { return {3.00, 5, 7, 3, ALU}; }
    code::Cost LODSW_REPE() const final { return {18.00, 0, 0, 3, ALU}; }
    code::Cost LODSW_REPNE() const final { return {18.00, 0, 0, 3, ALU}; }
    code::Cost LOOP_REL8() const final { return {0.56, 1, 1, 1, ALU | BRANCH}; }
    code::Cost LOOPE_REL8() const final { return {0.56, 1, 1, 1, ALU | BRANCH}; }
    code::Cost LOOPNE_REL8() const final { return {0.56, 1, 1, 1, ALU | BRANCH}; }
    code::Cost LSL_R16_M16() const final { return {61.00, 86, 98, 11, ALU | AGU | LSU}; }
    code::Cost LSL_R16_R16() const final { return {96.25, 87, 91, 32, ALU}; }
    code::Cost LSL_R32_M16() const final { return {61.00, 86, 98, 11, ALU | AGU | LSU}; }
    code::Cost LSL_R32_R32() const final { return {96.00, 87, 91, 32, ALU}; }
    code::Cost LSL_R64_M16() const final { return {61.00, 86, 98, 11, ALU | AGU | LSU}; }
    code::Cost LSL_R64_R32() const final { return {96.00, 87, 91, 32, ALU}; }
    code::Cost LSS_R16_M32() const final { return {0.0, 0, 0, 0, ALU | AGU | LSU}; }
    code::Cost LSS_R32_M48() const final { return {0.0, 0, 0, 0, ALU | AGU | LSU}; }
    code::Cost LSS_R64_M80() const final { return {0.0, 0, 0, 0, ALU | AGU | LSU}; }
    code::Cost LTR_M16() const final { return {0.0, 0, 0, 0, ALU | AGU | LSU}; }
    code::Cost LTR_R16() const final { return {0.0, 0, 0, 0, ALU}; }
    code::Cost MOV_AL_Moffs8() const final { return {1.00, 7, 7, 2, ALU}; }
    code::Cost MOV_AX_Moffs16() const final { return {1.00, 1, 7, 2, ALU}; }
    code::Cost MOV_EAX_Moffs32() const final { return {1.00, 6, 6, 2, ALU}; }
    code::Cost MOV_M16_I16() const final { return {1.00, 9, 9, 1, ALU | AGU | LSU}; }
    code::Cost MOV_M16_R16() const final { return {1.00, 7, 9, 1, ALU | AGU | LSU}; }
    code::Cost MOV_M16_SEG() const final { return {1.00, 9, 9, 2, ALU | AGU | LSU}; }
    code::Cost MOV_M32_I32() const final { return {1.00, 8, 8, 1, ALU | AGU | LSU}; }
    code::Cost MOV_M32_R32() const final { return {1.00, 6, 8, 1, ALU | AGU | LSU}; }
    code::Cost MOV_M64_I32() const final { return {1.00, 8, 8, 1, ALU | AGU | LSU}; }
    code::Cost MOV_M64_R64() const final { return {1.00, 6, 8, 1, ALU | AGU | LSU}; }
    code::Cost MOV_M8_I8() const final { return {1.00, 9, 9, 1, ALU | AGU | LSU}; }
    code::Cost MOV_M8_R8() const final { return {1.00, 7, 9, 1, ALU | AGU | LSU}; }
    code::Cost MOV_Moffs16_AX() const final { return {1.00, 7, 7, 2, ALU}; }
    code::Cost MOV_Moffs32_EAX() const final { return {1.00, 6, 6, 2, ALU}; }
    code::Cost MOV_Moffs64_RAX() const final { return {1.00, 6, 6, 2, ALU}; }
    code::Cost MOV_Moffs8_AL() const final { return {1.00, 7, 7, 2, ALU}; }
    code::Cost MOV_R_M16() const final { return {0.0, 0, 0, 0, ALU | AGU | LSU}; }
    code::Cost MOV_R_R16() const final { return {0.0, 0, 0, 0, ALU}; }
    code::Cost MOV_R16_I16() const final { return {0.28, 1, 1, 1, ALU}; }
    code::Cost MOV_R16_M16() const final { return {0.50, 1, 7, 1, ALU | AGU | LSU}; }
    code::Cost MOV_R16_SEG() const final { return {1.00, 1, 1, 1, ALU}; }
    code::Cost MOV_R32_I32() const final { return {0.36, 0, 0, 1, ALU}; }
    code::Cost MOV_R32_M32() const final { return {0.50, 4, 6, 1, ALU | AGU | LSU}; }
    code::Cost MOV_R32_SEG() const final { return {1.00, 0, 0, 1, ALU}; }
    code::Cost MOV_R64_I32() const final { return {0.44, 0, 0, 1, ALU}; }
    code::Cost MOV_R64_I64() const final { return {1.00, 0, 0, 1, ALU}; }
    code::Cost MOV_R64_M64() const final { return {0.50, 4, 6, 1, ALU | AGU | LSU}; }
    code::Cost MOV_R64_SEG() const final { return {1.00, 0, 0, 1, ALU}; }
    code::Cost MOV_R8_I8() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost MOV_R8_M8() const final { return {0.50, 1, 7, 1, ALU | AGU | LSU}; }
    code::Cost MOV_R8_I8() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost MOV_RAX_Moffs64() const final { return {1.00, 6, 6, 2, ALU}; }
    code::Cost MOV_88_R8_R8() const final { return {0.37, 1, 1, 1, ALU}; }
    code::Cost MOV_89_R16_R16() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost MOV_89_R32_R32() const final { return {0.25, 0, 0, 1, ALU}; }
    code::Cost MOV_89_R64_R64() const final { return {0.25, 0, 0, 1, ALU}; }
    code::Cost MOV_8A_R8_R8() const final { return {0.37, 1, 1, 1, ALU}; }
    code::Cost MOV_8B_R16_R16() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost MOV_8B_R32_R32() const final { return {0.25, 0, 0, 1, ALU}; }
    code::Cost MOV_8B_R64_R64() const final { return {0.25, 0, 0, 1, ALU}; }
    code::Cost MOV_CR_CR_R64() const final { return {0.0, 0, 0, 0, ALU}; }
    code::Cost MOV_CR_R64_CR() const final { return {9.00, 0, 0, 32, ALU}; }
    code::Cost MOV_DR_DR_R64() const final { return {96.00, 98, 98, 52, ALU}; }
    code::Cost MOV_DR_R64_DR() const final { return {39.00, 43, 43, 32, ALU}; }
    code::Cost MOVSB() const final { return {3.00, 5, 5, 5, ALU}; }
    code::Cost MOVSB_REPE() const final { return {18.00, 0, 0, 3, ALU}; }
    code::Cost MOVSB_REPNE() const final { return {18.00, 0, 0, 3, ALU}; }
    code::Cost MOVSD() const final { return {3.00, 4, 4, 5, ALU}; }
    code::Cost MOVSD_REPE() const final { return {18.00, 0, 0, 3, ALU}; }
    code::Cost MOVSD_REPNE() const final { return {18.00, 0, 0, 3, ALU}; }
    code::Cost MOVSW() const final { return {3.00, 5, 5, 5, ALU}; }
    code::Cost MOVSW_REPE() const final { return {18.00, 0, 0, 3, ALU}; }
    code::Cost MOVSW_REPNE() const final { return {18.00, 0, 0, 3, ALU}; }
    code::Cost MOVSX_R16_M8() const final { return {0.50, 1, 7, 1, ALU | AGU | LSU}; }
    code::Cost MOVSX_R16_R8() const final { return {0.30, 1, 1, 1, ALU}; }
    code::Cost MOVSX_R16_R8() const final { return {0.33, 1, 1, 1, ALU}; }
    code::Cost MOVSX_R32_M16() const final { return {0.50, 5, 7, 1, ALU | AGU | LSU}; }
    code::Cost MOVSX_R32_M8() const final { return {0.50, 5, 7, 1, ALU | AGU | LSU}; }
    code::Cost MOVSX_R32_R16() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost MOVSX_R32_R8() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost MOVSX_R32_R8() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost MOVSX_R64_M16() const final { return {0.50, 5, 7, 1, ALU | AGU | LSU}; }
    code::Cost MOVSX_R64_M8() const final { return {0.50, 5, 7, 1, ALU | AGU | LSU}; }
    code::Cost MOVSX_R64_R16() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost MOVSX_R64_R8() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost MOVZX_R16_M8() const final { return {0.50, 1, 7, 1, ALU | AGU | LSU}; }
    code::Cost MOVZX_R16_R8() const final { return {0.30, 1, 1, 1, ALU}; }
    code::Cost MOVZX_R16_R8() const final { return {0.33, 1, 1, 1, ALU}; }
    code::Cost MOVZX_R32_M16() const final { return {0.50, 4, 6, 1, ALU | AGU | LSU}; }
    code::Cost MOVZX_R32_M8() const final { return {0.50, 4, 6, 1, ALU | AGU | LSU}; }
    code::Cost MOVZX_R32_R16() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost MOVZX_R32_R8() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost MOVZX_R32_R8() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost MOVZX_R64_M16() const final { return {0.50, 4, 6, 1, ALU | AGU | LSU}; }
    code::Cost MOVZX_R64_M8() const final { return {0.50, 4, 6, 1, ALU | AGU | LSU}; }
    code::Cost MOVZX_R64_R16() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost MOVZX_R64_R8() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost MUL_M16() const final { return {2.00, 0, 11, 3, ALU | AGU | LSU}; }
    code::Cost MUL_M32() const final { return {2.00, 3, 10, 2, ALU | AGU | LSU}; }
    code::Cost MUL_M64() const final { return {2.00, 3, 10, 2, ALU | AGU | LSU}; }
    code::Cost MUL_M8() const final { return {1.00, 3, 9, 1, ALU | AGU | LSU}; }
    code::Cost MUL_R16() const final { return {2.00, 0, 5, 3, ALU}; }
    code::Cost MUL_R32() const final { return {2.00, 3, 4, 2, ALU}; }
    code::Cost MUL_R64() const final { return {2.00, 3, 4, 2, ALU}; }
    code::Cost MUL_R8() const final { return {1.00, 3, 3, 1, ALU}; }
    code::Cost MUL_R8() const final { return {1.00, 3, 3, 1, ALU}; }
    code::Cost NEG_M16() const final { return {1.00, 5, 9, 1, ALU | AGU | LSU}; }
    code::Cost NEG_M32() const final { return {1.00, 5, 8, 1, ALU | AGU | LSU}; }
    code::Cost NEG_M64() const final { return {1.00, 5, 8, 1, ALU | AGU | LSU}; }
    code::Cost NEG_M8() const final { return {1.00, 5, 9, 1, ALU | AGU | LSU}; }
    code::Cost NEG_R16() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost NEG_R32() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost NEG_R64() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost NEG_R8() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost NEG_R8() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost NEG_LOCK_M16() const final { return {22.52, 7, 21, 1, ALU | AGU | LSU | LOCK}; }
    code::Cost NEG_LOCK_M32() const final { return {22.52, 7, 21, 1, ALU | AGU | LSU | LOCK}; }
    code::Cost NEG_LOCK_M64() const final { return {22.52, 7, 21, 1, ALU | AGU | LSU | LOCK}; }
    code::Cost NEG_LOCK_M8() const final { return {22.52, 7, 21, 1, ALU | AGU | LSU | LOCK}; }
    code::Cost NOP() const final { return {0.25, 0, 0, 1}; }
    code::Cost NOP_M16() const final { return {0.31, 0, 0, 1}; }
    code::Cost NOP_M32() const final { return {0.25, 0, 0, 1}; }
    code::Cost NOP_M64() const final { return {0.25, 0, 0, 1}; }
    code::Cost NOP_R16() const final { return {0.31, 0, 0, 1}; }
    code::Cost NOP_R32() const final { return {0.25, 0, 0, 1}; }
    code::Cost NOP_R64() const final { return {0.25, 0, 0, 1}; }
    code::Cost NOT_M16() const final { return {1.00, 7, 9, 1, ALU | AGU | LSU}; }
    code::Cost NOT_M32() const final { return {1.00, 7, 8, 1, ALU | AGU | LSU}; }
    code::Cost NOT_M64() const final { return {1.00, 7, 8, 1, ALU | AGU | LSU}; }
    code::Cost NOT_M8() const final { return {1.00, 7, 9, 1, ALU | AGU | LSU}; }
    code::Cost NOT_R16() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost NOT_R32() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost NOT_R64() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost NOT_R8() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost NOT_R8() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost NOT_LOCK_M16() const final { return {22.52, 7, 10, 1, ALU | AGU | LSU | LOCK}; }
    code::Cost NOT_LOCK_M32() const final { return {22.52, 7, 8, 1, ALU | AGU | LSU | LOCK}; }
    code::Cost NOT_LOCK_M64() const final { return {22.52, 7, 8, 1, ALU | AGU | LSU | LOCK}; }
    code::Cost NOT_LOCK_M8() const final { return {22.52, 7, 10, 1, ALU | AGU | LSU | LOCK}; }
    code::Cost OR_AL_I8() const final { return {0.50, 1, 1, 1, ALU}; }
    code::Cost OR_AX_I16() const final { return {0.50, 1, 1, 1, ALU}; }
    code::Cost OR_EAX_I32() const final { return {0.50, 1, 1, 1, ALU}; }
    code::Cost OR_M16_I16() const final { return {1.00, 5, 9, 1, ALU | AGU | LSU}; }
    code::Cost OR_M16_I8() const final { return {1.00, 5, 9, 1, ALU | AGU | LSU}; }
    code::Cost OR_M16_R16() const final { return {1.00, 1, 9, 1, ALU | AGU | LSU}; }
    code::Cost OR_M32_I32() const final { return {1.00, 5, 8, 1, ALU | AGU | LSU}; }
    code::Cost OR_M32_I8() const final { return {1.00, 5, 8, 1, ALU | AGU | LSU}; }
    code::Cost OR_M32_R32() const final { return {1.00, 1, 8, 1, ALU | AGU | LSU}; }
    code::Cost OR_M64_I32() const final { return {1.00, 5, 8, 1, ALU | AGU | LSU}; }
    code::Cost OR_M64_I8() const final { return {1.00, 5, 8, 1, ALU | AGU | LSU}; }
    code::Cost OR_M64_R64() const final { return {1.00, 1, 8, 1, ALU | AGU | LSU}; }
    code::Cost OR_M8_I8() const final { return {1.00, 5, 9, 1, ALU | AGU | LSU}; }
    code::Cost OR_M8_R8() const final { return {1.00, 1, 10, 1, ALU | AGU | LSU}; }
    code::Cost OR_R16_I16() const final { return {0.36, 1, 1, 1, ALU}; }
    code::Cost OR_R16_I8() const final { return {0.28, 1, 1, 1, ALU}; }
    code::Cost OR_R16_M16() const final { return {0.50, 1, 7, 1, ALU | AGU | LSU}; }
    code::Cost OR_R32_I32() const final { return {0.40, 1, 1, 1, ALU}; }
    code::Cost OR_R32_I8() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost OR_R32_M32() const final { return {0.50, 1, 7, 1, ALU | AGU | LSU}; }
    code::Cost OR_R64_I32() const final { return {0.43, 1, 1, 1, ALU}; }
    code::Cost OR_R64_I8() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost OR_R64_M64() const final { return {0.50, 1, 7, 1, ALU | AGU | LSU}; }
    code::Cost OR_R8_I8() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost OR_R8_M8() const final { return {0.50, 1, 7, 1, ALU | AGU | LSU}; }
    code::Cost OR_RAX_I32() const final { return {0.56, 1, 1, 1, ALU}; }
    code::Cost OR_08_R8_R8() const final { return {0.38, 1, 1, 1, ALU}; }
    code::Cost OR_09_R16_R16() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost OR_09_R32_R32() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost OR_09_R64_R64() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost OR_0A_R8_R8() const final { return {0.38, 1, 1, 1, ALU}; }
    code::Cost OR_0B_R16_R16() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost OR_0B_R32_R32() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost OR_0B_R64_R64() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost OR_LOCK_M16_I16() const final { return {22.52, 7, 21, 1, ALU | AGU | LSU | LOCK}; }
    code::Cost OR_LOCK_M16_I8() const final { return {22.52, 7, 21, 1, ALU | AGU | LSU | LOCK}; }
    code::Cost OR_LOCK_M16_R16() const final { return {22.52, 1, 21, 1, ALU | AGU | LSU | LOCK}; }
    code::Cost OR_LOCK_M32_I32() const final { return {22.52, 7, 21, 1, ALU | AGU | LSU | LOCK}; }
    code::Cost OR_LOCK_M32_I8() const final { return {22.52, 7, 21, 1, ALU | AGU | LSU | LOCK}; }
    code::Cost OR_LOCK_M32_R32() const final { return {22.52, 1, 21, 1, ALU | AGU | LSU | LOCK}; }
    code::Cost OR_LOCK_M64_I32() const final { return {22.52, 7, 21, 1, ALU | AGU | LSU | LOCK}; }
    code::Cost OR_LOCK_M64_I8() const final { return {22.52, 7, 21, 1, ALU | AGU | LSU | LOCK}; }
    code::Cost OR_LOCK_M64_R64() const final { return {22.52, 1, 21, 1, ALU | AGU | LSU | LOCK}; }
    code::Cost OR_LOCK_M8_I8() const final { return {22.52, 7, 21, 1, ALU | AGU | LSU | LOCK}; }
    code::Cost OR_LOCK_M8_R8() const final { return {22.50, 1, 21, 1, ALU | AGU | LSU | LOCK}; }
    code::Cost OR_R0_R8_I8() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost OR_R4_R8_I8() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost OUT_0_AL() const final { return {9602.05, 0, 0, 52, ALU}; }
    code::Cost OUT_0_AX() const final { return {9596.15, 0, 0, 52, ALU}; }
    code::Cost OUT_0_EAX() const final { return {9602.05, 0, 0, 52, ALU}; }
    code::Cost OUT_DX_AL() const final { return {9603.50, 0, 0, 52, ALU}; }
    code::Cost OUT_DX_AX() const final { return {9601.98, 0, 0, 52, ALU}; }
    code::Cost OUT_DX_EAX() const final { return {12438.27, 0, 0, 52, ALU}; }
    code::Cost OUT_I8_AL() const final { return {9602.05, 0, 0, 52, ALU}; }
    code::Cost OUT_I8_AX() const final { return {9606.43, 0, 0, 52, ALU}; }
    code::Cost OUT_I8_EAX() const final { return {12423.52, 0, 0, 52, ALU}; }
    code::Cost OUTSB() const final { return {9602.05, 0, 0, 31, ALU}; }
    code::Cost OUTSB_REPE() const final { return {30.00, 0, 0, 17, ALU}; }
    code::Cost OUTSB_REPNE() const final { return {30.00, 0, 0, 17, ALU}; }
    code::Cost OUTSD() const final { return {12399.82, 0, 0, 31, ALU}; }
    code::Cost OUTSD_REPE() const final { return {30.00, 0, 0, 17, ALU}; }
    code::Cost OUTSD_REPE_REX64() const final { return {30.00, 0, 0, 17, ALU}; }
    code::Cost OUTSD_REPNE() const final { return {30.00, 0, 0, 17, ALU}; }
    code::Cost OUTSD_REPNE_REX64() const final { return {30.00, 0, 0, 17, ALU}; }
    code::Cost OUTSW() const final { return {9603.50, 0, 0, 31, ALU}; }
    code::Cost OUTSW_REPE() const final { return {30.00, 0, 0, 17, ALU}; }
    code::Cost OUTSW_REPNE() const final { return {30.00, 0, 0, 17, ALU}; }
    code::Cost POP_FS() const final { return {0.0, 0, 0, 0, ALU | AGU | LSU}; }
    code::Cost POP_GS() const final { return {0.0, 0, 0, 0, ALU | AGU | LSU}; }
    code::Cost POP_M16() const final { return {2.00, 9, 9, 3, ALU | AGU | LSU}; }
    code::Cost POP_M64() const final { return {1.00, 8, 8, 2, ALU | AGU | LSU}; }
    code::Cost POP_R16() const final { return {1.15, 1, 7, 2, ALU | AGU | LSU}; }
    code::Cost POP_R64() const final { return {0.50, 4, 6, 1, ALU | AGU | LSU}; }
    code::Cost POP_W_FS() const final { return {0.0, 0, 0, 0, ALU | AGU | LSU}; }
    code::Cost POP_W_GS() const final { return {0.0, 0, 0, 0, ALU | AGU | LSU}; }
    code::Cost POPF() const final { return {13.00, 0, 0, 34, ALU | AGU | LSU}; }
    code::Cost PUSH_FS() const final { return {1.00, 8, 8, 2, ALU | AGU | LSU}; }
    code::Cost PUSH_GS() const final { return {1.00, 8, 8, 2, ALU | AGU | LSU}; }
    code::Cost PUSH_I32() const final { return {1.00, 8, 8, 1, ALU | AGU | LSU}; }
    code::Cost PUSH_I8() const final { return {1.00, 8, 8, 1, ALU | AGU | LSU}; }
    code::Cost PUSH_M16() const final { return {2.00, 12, 12, 3, ALU | AGU | LSU}; }
    code::Cost PUSH_M64() const final { return {1.00, 8, 8, 2, ALU | AGU | LSU}; }
    code::Cost PUSH_R16() const final { return {1.00, 7, 12, 2, ALU | AGU | LSU}; }
    code::Cost PUSH_R64() const final { return {1.00, 6, 8, 1, ALU | AGU | LSU}; }
    code::Cost PUSH_W_FS() const final { return {2.00, 12, 12, 3, ALU | AGU | LSU}; }
    code::Cost PUSH_W_GS() const final { return {2.00, 12, 12, 3, ALU | AGU | LSU}; }
    code::Cost PUSH_W_I16() const final { return {1.00, 12, 12, 2, ALU | AGU | LSU}; }
    code::Cost PUSH_W_I8() const final { return {1.00, 12, 12, 2, ALU | AGU | LSU}; }
    code::Cost PUSHF() const final { return {4.00, 10, 12, 9, ALU | AGU | LSU}; }
    code::Cost RCL_M16_1() const final { return {1.00, 1, 9, 1, ALU | AGU | LSU}; }
    code::Cost RCL_M16_CL() const final { return {4.00, 2, 12, 11, ALU | AGU | LSU}; }
    code::Cost RCL_M16_I8() const final { return {4.00, 2, 12, 11, ALU | AGU | LSU}; }
    code::Cost RCL_M32_1() const final { return {1.00, 1, 8, 1, ALU | AGU | LSU}; }
    code::Cost RCL_M32_CL() const final { return {4.00, 2, 11, 11, ALU | AGU | LSU}; }
    code::Cost RCL_M32_I8() const final { return {4.00, 2, 11, 11, ALU | AGU | LSU}; }
    code::Cost RCL_M64_1() const final { return {1.00, 1, 8, 1, ALU | AGU | LSU}; }
    code::Cost RCL_M64_CL() const final { return {4.00, 2, 11, 11, ALU | AGU | LSU}; }
    code::Cost RCL_M64_I8() const final { return {4.00, 3, 11, 11, ALU | AGU | LSU}; }
    code::Cost RCL_M8_1() const final { return {1.00, 1, 9, 1, ALU | AGU | LSU}; }
    code::Cost RCL_M8_CL() const final { return {4.00, 2, 12, 11, ALU | AGU | LSU}; }
    code::Cost RCL_M8_I8() const final { return {4.00, 2, 12, 11, ALU | AGU | LSU}; }
    code::Cost RCL_R16_1() const final { return {0.50, 1, 1, 1, ALU}; }
    code::Cost RCL_R16_CL() const final { return {4.00, 2, 4, 9, ALU}; }
    code::Cost RCL_R16_I8() const final { return {4.00, 2, 2, 9, ALU}; }
    code::Cost RCL_R32_1() const final { return {0.50, 1, 1, 1, ALU}; }
    code::Cost RCL_R32_CL() const final { return {4.00, 2, 4, 9, ALU}; }
    code::Cost RCL_R32_I8() const final { return {4.00, 2, 2, 9, ALU}; }
    code::Cost RCL_R64_1() const final { return {0.50, 1, 1, 1, ALU}; }
    code::Cost RCL_R64_CL() const final { return {4.00, 2, 4, 9, ALU}; }
    code::Cost RCL_R64_I8() const final { return {4.00, 2, 2, 9, ALU}; }
    code::Cost RCL_R8_1() const final { return {0.52, 1, 1, 1, ALU}; }
    code::Cost RCL_R8_CL() const final { return {4.00, 2, 3, 9, ALU}; }
    code::Cost RCL_R8_I8() const final { return {4.00, 2, 2, 9, ALU}; }
    code::Cost RCL_R8_CL() const final { return {4.00, 2, 4, 9, ALU}; }
    code::Cost RCL_R0_R8_1() const final { return {0.50, 1, 1, 1, ALU}; }
    code::Cost RCL_R0_R8_I8() const final { return {4.00, 2, 2, 9, ALU}; }
    code::Cost RCL_R4_R8_1() const final { return {0.50, 1, 1, 1, ALU}; }
    code::Cost RCL_R4_R8_I8() const final { return {4.00, 2, 2, 9, ALU}; }
    code::Cost RCR_M16_1() const final { return {1.00, 1, 9, 1, ALU | AGU | LSU}; }
    code::Cost RCR_M16_CL() const final { return {4.00, 2, 12, 9, ALU | AGU | LSU}; }
    code::Cost RCR_M16_I8() const final { return {4.00, 2, 12, 10, ALU | AGU | LSU}; }
    code::Cost RCR_M32_1() const final { return {1.00, 1, 8, 1, ALU | AGU | LSU}; }
    code::Cost RCR_M32_CL() const final { return {4.00, 2, 11, 9, ALU | AGU | LSU}; }
    code::Cost RCR_M32_I8() const final { return {4.00, 2, 11, 10, ALU | AGU | LSU}; }
    code::Cost RCR_M64_1() const final { return {1.00, 1, 8, 1, ALU | AGU | LSU}; }
    code::Cost RCR_M64_CL() const final { return {4.00, 2, 11, 9, ALU | AGU | LSU}; }
    code::Cost RCR_M64_I8() const final { return {4.00, 2, 11, 10, ALU | AGU | LSU}; }
    code::Cost RCR_M8_1() const final { return {1.00, 1, 9, 1, ALU | AGU | LSU}; }
    code::Cost RCR_M8_CL() const final { return {4.00, 2, 12, 9, ALU | AGU | LSU}; }
    code::Cost RCR_M8_I8() const final { return {4.00, 2, 12, 10, ALU | AGU | LSU}; }
    code::Cost RCR_R16_1() const final { return {0.50, 1, 1, 1, ALU}; }
    code::Cost RCR_R16_CL() const final { return {3.00, 2, 3, 7, ALU}; }
    code::Cost RCR_R16_I8() const final { return {3.00, 2, 2, 7, ALU}; }
    code::Cost RCR_R32_1() const final { return {0.50, 1, 1, 1, ALU}; }
    code::Cost RCR_R32_CL() const final { return {3.00, 2, 3, 7, ALU}; }
    code::Cost RCR_R32_I8() const final { return {3.00, 2, 2, 7, ALU}; }
    code::Cost RCR_R64_1() const final { return {0.50, 1, 1, 1, ALU}; }
    code::Cost RCR_R64_CL() const final { return {3.00, 2, 3, 7, ALU}; }
    code::Cost RCR_R64_I8() const final { return {3.00, 2, 2, 7, ALU}; }
    code::Cost RCR_R8_1() const final { return {0.52, 1, 1, 1, ALU}; }
    code::Cost RCR_R8_CL() const final { return {3.00, 2, 3, 7, ALU}; }
    code::Cost RCR_R8_I8() const final { return {3.00, 2, 2, 7, ALU}; }
    code::Cost RCR_R8_CL() const final { return {3.00, 2, 3, 7, ALU}; }
    code::Cost RCR_R0_R8_1() const final { return {0.50, 1, 1, 1, ALU}; }
    code::Cost RCR_R0_R8_I8() const final { return {3.00, 2, 2, 7, ALU}; }
    code::Cost RCR_R4_R8_1() const final { return {0.50, 1, 1, 1, ALU}; }
    code::Cost RCR_R4_R8_I8() const final { return {3.00, 2, 2, 7, ALU}; }
    code::Cost RDMSR() const final { return {148.00, 0, 0, 99}; }
    code::Cost RDPMC() const final { return {20.00, 0, 0, 17}; }
    code::Cost RDTSC() const final { return {36.84, 0, 0, 37}; }
    code::Cost RET_FAR() const final { return {0.0, 0, 0, 0, ALU | AGU | LSU}; }
    code::Cost RET_FAR_I16() const final { return {0.0, 0, 0, 0, ALU | AGU | LSU}; }
    code::Cost RET_FAR_Q() const final { return {0.0, 0, 0, 0, ALU | AGU | LSU}; }
    code::Cost RET_FAR_Q_I16() const final { return {0.0, 0, 0, 0, ALU | AGU | LSU}; }
    code::Cost RET_FAR_W() const final { return {0.0, 0, 0, 0, ALU | AGU | LSU}; }
    code::Cost RET_FAR_W_I16() const final { return {0.0, 0, 0, 0, ALU | AGU | LSU}; }
    code::Cost RET() const final { return {30.75, 0, 0, 1, ALU | AGU | LSU}; }
    code::Cost RET_I16() const final { return {30.85, 0, 0, 2, ALU | AGU | LSU}; }
    code::Cost ROL_M16_1() const final { return {1.00, 5, 9, 1, ALU | AGU | LSU}; }
    code::Cost ROL_M16_CL() const final { return {1.00, 0, 30, 1, ALU | AGU | LSU}; }
    code::Cost ROL_M16_I8() const final { return {1.00, 5, 9, 1, ALU | AGU | LSU}; }
    code::Cost ROL_M32_1() const final { return {1.00, 5, 8, 1, ALU | AGU | LSU}; }
    code::Cost ROL_M32_CL() const final { return {1.00, 0, 30, 1, ALU | AGU | LSU}; }
    code::Cost ROL_M32_I8() const final { return {1.00, 5, 8, 1, ALU | AGU | LSU}; }
    code::Cost ROL_M64_1() const final { return {1.00, 5, 8, 1, ALU | AGU | LSU}; }
    code::Cost ROL_M64_CL() const final { return {1.00, 0, 30, 1, ALU | AGU | LSU}; }
    code::Cost ROL_M64_I8() const final { return {1.00, 5, 8, 1, ALU | AGU | LSU}; }
    code::Cost ROL_M8_1() const final { return {1.00, 5, 9, 1, ALU | AGU | LSU}; }
    code::Cost ROL_M8_CL() const final { return {1.00, 0, 30, 1, ALU | AGU | LSU}; }
    code::Cost ROL_M8_I8() const final { return {1.00, 5, 9, 1, ALU | AGU | LSU}; }
    code::Cost ROL_R16_1() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost ROL_R16_CL() const final { return {0.25, 0, 26, 1, ALU}; }
    code::Cost ROL_R16_I8() const final { return {0.28, 1, 1, 1, ALU}; }
    code::Cost ROL_R32_1() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost ROL_R32_CL() const final { return {0.25, 0, 26, 1, ALU}; }
    code::Cost ROL_R32_I8() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost ROL_R64_1() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost ROL_R64_CL() const final { return {0.25, 0, 26, 1, ALU}; }
    code::Cost ROL_R64_I8() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost ROL_R8_1() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost ROL_R8_CL() const final { return {0.40, 0, 26, 1, ALU}; }
    code::Cost ROL_R8_I8() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost ROL_R8_CL() const final { return {0.25, 0, 26, 1, ALU}; }
    code::Cost ROL_R0_R8_1() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost ROL_R0_R8_I8() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost ROL_R4_R8_1() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost ROL_R4_R8_I8() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost ROR_M16_1() const final { return {1.00, 5, 9, 1, ALU | AGU | LSU}; }
    code::Cost ROR_M16_CL() const final { return {1.00, 0, 30, 1, ALU | AGU | LSU}; }
    code::Cost ROR_M16_I8() const final { return {1.00, 5, 9, 1, ALU | AGU | LSU}; }
    code::Cost ROR_M32_1() const final { return {1.00, 5, 8, 1, ALU | AGU | LSU}; }
    code::Cost ROR_M32_CL() const final { return {1.00, 0, 30, 1, ALU | AGU | LSU}; }
    code::Cost ROR_M32_I8() const final { return {1.00, 5, 8, 1, ALU | AGU | LSU}; }
    code::Cost ROR_M64_1() const final { return {1.00, 5, 8, 1, ALU | AGU | LSU}; }
    code::Cost ROR_M64_CL() const final { return {1.00, 0, 30, 1, ALU | AGU | LSU}; }
    code::Cost ROR_M64_I8() const final { return {1.00, 5, 8, 1, ALU | AGU | LSU}; }
    code::Cost ROR_M8_1() const final { return {1.00, 5, 9, 1, ALU | AGU | LSU}; }
    code::Cost ROR_M8_CL() const final { return {1.00, 0, 30, 1, ALU | AGU | LSU}; }
    code::Cost ROR_M8_I8() const final { return {1.00, 5, 9, 1, ALU | AGU | LSU}; }
    code::Cost ROR_R16_1() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost ROR_R16_CL() const final { return {0.25, 0, 26, 1, ALU}; }
    code::Cost ROR_R16_I8() const final { return {0.28, 1, 1, 1, ALU}; }
    code::Cost ROR_R32_1() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost ROR_R32_CL() const final { return {0.25, 0, 26, 1, ALU}; }
    code::Cost ROR_R32_I8() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost ROR_R64_1() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost ROR_R64_CL() const final { return {0.25, 0, 26, 1, ALU}; }
    code::Cost ROR_R64_I8() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost ROR_R8_1() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost ROR_R8_CL() const final { return {0.40, 0, 26, 1, ALU}; }
    code::Cost ROR_R8_I8() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost ROR_R8_CL() const final { return {0.25, 0, 26, 1, ALU}; }
    code::Cost ROR_R0_R8_1() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost ROR_R0_R8_I8() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost ROR_R4_R8_1() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost ROR_R4_R8_I8() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost RSM() const final { return {0.0, 0, 0, 0}; }  // Specialized operation
    code::Cost SAHF() const final { return {0.50, 2, 2, 2, ALU}; }
    code::Cost SAR_M16_1() const final { return {1.00, 5, 9, 1, ALU | AGU | LSU}; }
    code::Cost SAR_M16_CL() const final { return {1.00, 0, 30, 1, ALU | AGU | LSU}; }
    code::Cost SAR_M16_I8() const final { return {1.00, 5, 9, 1, ALU | AGU | LSU}; }
    code::Cost SAR_M32_1() const final { return {1.00, 5, 8, 1, ALU | AGU | LSU}; }
    code::Cost SAR_M32_CL() const final { return {1.00, 0, 30, 1, ALU | AGU | LSU}; }
    code::Cost SAR_M32_I8() const final { return {1.00, 5, 8, 1, ALU | AGU | LSU}; }
    code::Cost SAR_M64_1() const final { return {1.00, 5, 8, 1, ALU | AGU | LSU}; }
    code::Cost SAR_M64_CL() const final { return {1.00, 0, 30, 1, ALU | AGU | LSU}; }
    code::Cost SAR_M64_I8() const final { return {1.00, 5, 8, 1, ALU | AGU | LSU}; }
    code::Cost SAR_M8_1() const final { return {1.00, 5, 9, 1, ALU | AGU | LSU}; }
    code::Cost SAR_M8_CL() const final { return {1.00, 0, 30, 1, ALU | AGU | LSU}; }
    code::Cost SAR_M8_I8() const final { return {1.00, 5, 9, 1, ALU | AGU | LSU}; }
    code::Cost SAR_R16_1() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost SAR_R16_CL() const final { return {0.25, 0, 13, 1, ALU}; }
    code::Cost SAR_R16_I8() const final { return {0.28, 1, 1, 1, ALU}; }
    code::Cost SAR_R32_1() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost SAR_R32_CL() const final { return {0.25, 0, 13, 1, ALU}; }
    code::Cost SAR_R32_I8() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost SAR_R64_1() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost SAR_R64_CL() const final { return {0.25, 0, 13, 1, ALU}; }
    code::Cost SAR_R64_I8() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost SAR_R8_1() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost SAR_R8_CL() const final { return {0.40, 0, 26, 1, ALU}; }
    code::Cost SAR_R8_I8() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost SAR_R8_CL() const final { return {0.25, 0, 13, 1, ALU}; }
    code::Cost SAR_R0_R8_1() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost SAR_R0_R8_I8() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost SAR_R4_R8_1() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost SAR_R4_R8_I8() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost SBB_AL_I8() const final { return {0.50, 1, 1, 1, ALU}; }
    code::Cost SBB_AX_I16() const final { return {0.50, 1, 1, 1, ALU}; }
    code::Cost SBB_EAX_I32() const final { return {0.50, 1, 1, 1, ALU}; }
    code::Cost SBB_M16_I16() const final { return {1.00, 1, 9, 1, ALU | AGU | LSU}; }
    code::Cost SBB_M16_I8() const final { return {1.00, 1, 9, 1, ALU | AGU | LSU}; }
    code::Cost SBB_M16_R16() const final { return {1.00, 1, 9, 1, ALU | AGU | LSU}; }
    code::Cost SBB_M32_I32() const final { return {1.00, 1, 8, 1, ALU | AGU | LSU}; }
    code::Cost SBB_M32_I8() const final { return {1.00, 1, 8, 1, ALU | AGU | LSU}; }
    code::Cost SBB_M32_R32() const final { return {1.00, 1, 8, 1, ALU | AGU | LSU}; }
    code::Cost SBB_M64_I32() const final { return {1.00, 1, 8, 1, ALU | AGU | LSU}; }
    code::Cost SBB_M64_I8() const final { return {1.00, 1, 8, 1, ALU | AGU | LSU}; }
    code::Cost SBB_M64_R64() const final { return {1.00, 1, 8, 1, ALU | AGU | LSU}; }
    code::Cost SBB_M8_I8() const final { return {1.00, 1, 9, 1, ALU | AGU | LSU}; }
    code::Cost SBB_M8_R8() const final { return {1.00, 1, 9, 1, ALU | AGU | LSU}; }
    code::Cost SBB_R16_I16() const final { return {0.53, 1, 1, 1, ALU}; }
    code::Cost SBB_R16_I8() const final { return {0.50, 1, 1, 1, ALU}; }
    code::Cost SBB_R16_M16() const final { return {0.50, 1, 7, 1, ALU | AGU | LSU}; }
    code::Cost SBB_R32_I32() const final { return {0.60, 1, 1, 1, ALU}; }
    code::Cost SBB_R32_I8() const final { return {0.50, 1, 1, 1, ALU}; }
    code::Cost SBB_R32_M32() const final { return {0.50, 1, 7, 1, ALU | AGU | LSU}; }
    code::Cost SBB_R64_I32() const final { return {0.63, 1, 1, 1, ALU}; }
    code::Cost SBB_R64_I8() const final { return {0.50, 1, 1, 1, ALU}; }
    code::Cost SBB_R64_M64() const final { return {0.50, 1, 7, 1, ALU | AGU | LSU}; }
    code::Cost SBB_R8_I8() const final { return {0.52, 1, 1, 1, ALU}; }
    code::Cost SBB_R8_M8() const final { return {0.50, 1, 7, 1, ALU | AGU | LSU}; }
    code::Cost SBB_RAX_I32() const final { return {0.56, 1, 1, 1, ALU}; }
    code::Cost SBB_18_R8_R8() const final { return {0.50, 1, 1, 1, ALU}; }
    code::Cost SBB_19_R16_R16() const final { return {0.50, 1, 1, 1, ALU}; }
    code::Cost SBB_19_R32_R32() const final { return {0.50, 1, 1, 1, ALU}; }
    code::Cost SBB_19_R64_R64() const final { return {0.50, 1, 1, 1, ALU}; }
    code::Cost SBB_1A_R8_R8() const final { return {0.50, 1, 1, 1, ALU}; }
    code::Cost SBB_1B_R16_R16() const final { return {0.50, 1, 1, 1, ALU}; }
    code::Cost SBB_1B_R32_R32() const final { return {0.50, 1, 1, 1, ALU}; }
    code::Cost SBB_1B_R64_R64() const final { return {0.50, 1, 1, 1, ALU}; }
    code::Cost SBB_LOCK_M16_I16() const final { return {22.52, 1, 21, 1, ALU | AGU | LSU | LOCK}; }
    code::Cost SBB_LOCK_M16_I8() const final { return {22.52, 1, 21, 1, ALU | AGU | LSU | LOCK}; }
    code::Cost SBB_LOCK_M16_R16() const final { return {22.52, 1, 21, 1, ALU | AGU | LSU | LOCK}; }
    code::Cost SBB_LOCK_M32_I32() const final { return {22.52, 1, 21, 1, ALU | AGU | LSU | LOCK}; }
    code::Cost SBB_LOCK_M32_I8() const final { return {22.52, 1, 21, 1, ALU | AGU | LSU | LOCK}; }
    code::Cost SBB_LOCK_M32_R32() const final { return {22.52, 1, 21, 1, ALU | AGU | LSU | LOCK}; }
    code::Cost SBB_LOCK_M64_I32() const final { return {22.52, 1, 21, 1, ALU | AGU | LSU | LOCK}; }
    code::Cost SBB_LOCK_M64_I8() const final { return {22.52, 1, 21, 1, ALU | AGU | LSU | LOCK}; }
    code::Cost SBB_LOCK_M64_R64() const final { return {22.52, 1, 21, 1, ALU | AGU | LSU | LOCK}; }
    code::Cost SBB_LOCK_M8_I8() const final { return {22.52, 1, 21, 1, ALU | AGU | LSU | LOCK}; }
    code::Cost SBB_LOCK_M8_R8() const final { return {22.50, 1, 21, 1, ALU | AGU | LSU | LOCK}; }
    code::Cost SBB_R0_R8_I8() const final { return {0.50, 1, 1, 1, ALU}; }
    code::Cost SBB_R4_R8_I8() const final { return {0.50, 1, 1, 1, ALU}; }
    code::Cost SCASB() const final { return {3.00, 1, 7, 3, ALU}; }
    code::Cost SCASB_REPE() const final { return {18.00, 0, 0, 3, ALU}; }
    code::Cost SCASB_REPNE() const final { return {18.00, 0, 0, 3, ALU}; }
    code::Cost SCASD() const final { return {3.00, 1, 7, 3, ALU}; }
    code::Cost SCASD_REPE() const final { return {18.00, 0, 0, 3, ALU}; }
    code::Cost SCASD_REPNE() const final { return {18.00, 0, 0, 3, ALU}; }
    code::Cost SCASW() const final { return {3.00, 1, 7, 3, ALU}; }
    code::Cost SCASW_REPE() const final { return {18.00, 0, 0, 3, ALU}; }
    code::Cost SCASW_REPNE() const final { return {18.00, 0, 0, 3, ALU}; }
    code::Cost SETB_M8() const final { return {1.00, 7, 9, 1, ALU | AGU | LSU}; }
    code::Cost SETB_R8() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost SETB_R8() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost SETBE_M8() const final { return {1.00, 8, 10, 2, ALU | AGU | LSU}; }
    code::Cost SETBE_R8() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost SETBE_R8() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost SETL_M8() const final { return {1.00, 8, 10, 2, ALU | AGU | LSU}; }
    code::Cost SETL_R8() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost SETL_R8() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost SETLE_M8() const final { return {1.00, 8, 10, 2, ALU | AGU | LSU}; }
    code::Cost SETLE_R8() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost SETLE_R8() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost SETNB_M8() const final { return {1.00, 7, 9, 1, ALU | AGU | LSU}; }
    code::Cost SETNB_R8() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost SETNB_R8() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost SETNBE_M8() const final { return {1.00, 8, 10, 2, ALU | AGU | LSU}; }
    code::Cost SETNBE_R8() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost SETNBE_R8() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost SETNL_M8() const final { return {1.00, 8, 10, 2, ALU | AGU | LSU}; }
    code::Cost SETNL_R8() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost SETNL_R8() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost SETNLE_M8() const final { return {1.00, 8, 10, 2, ALU | AGU | LSU}; }
    code::Cost SETNLE_R8() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost SETNLE_R8() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost SETNO_M8() const final { return {1.00, 7, 9, 1, ALU | AGU | LSU}; }
    code::Cost SETNO_R8() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost SETNO_R8() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost SETNP_M8() const final { return {1.00, 7, 9, 1, ALU | AGU | LSU}; }
    code::Cost SETNP_R8() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost SETNP_R8() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost SETNS_M8() const final { return {1.00, 7, 9, 1, ALU | AGU | LSU}; }
    code::Cost SETNS_R8() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost SETNS_R8() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost SETNZ_M8() const final { return {1.00, 7, 9, 1, ALU | AGU | LSU}; }
    code::Cost SETNZ_R8() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost SETNZ_R8() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost SETO_M8() const final { return {1.00, 7, 9, 1, ALU | AGU | LSU}; }
    code::Cost SETO_R8() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost SETO_R8() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost SETP_M8() const final { return {1.00, 7, 9, 1, ALU | AGU | LSU}; }
    code::Cost SETP_R8() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost SETP_R8() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost SETS_M8() const final { return {1.00, 7, 9, 1, ALU | AGU | LSU}; }
    code::Cost SETS_R8() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost SETS_R8() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost SETZ_M8() const final { return {1.00, 7, 9, 1, ALU | AGU | LSU}; }
    code::Cost SETZ_R8() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost SETZ_R8() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost SGDT_M80() const final { return {4.00, 23, 27, 9, ALU | AGU | LSU}; }
    code::Cost SHL_M16_1() const final { return {1.00, 5, 9, 1, ALU | AGU | LSU}; }
    code::Cost SHL_M16_CL() const final { return {1.00, 0, 30, 1, ALU | AGU | LSU}; }
    code::Cost SHL_M16_I8() const final { return {1.00, 5, 9, 1, ALU | AGU | LSU}; }
    code::Cost SHL_M32_1() const final { return {1.00, 5, 8, 1, ALU | AGU | LSU}; }
    code::Cost SHL_M32_CL() const final { return {1.00, 0, 30, 1, ALU | AGU | LSU}; }
    code::Cost SHL_M32_I8() const final { return {1.00, 5, 8, 1, ALU | AGU | LSU}; }
    code::Cost SHL_M64_1() const final { return {1.00, 5, 8, 1, ALU | AGU | LSU}; }
    code::Cost SHL_M64_CL() const final { return {1.00, 0, 30, 1, ALU | AGU | LSU}; }
    code::Cost SHL_M64_I8() const final { return {1.00, 5, 8, 1, ALU | AGU | LSU}; }
    code::Cost SHL_M8_1() const final { return {1.00, 5, 9, 1, ALU | AGU | LSU}; }
    code::Cost SHL_M8_CL() const final { return {1.00, 0, 30, 1, ALU | AGU | LSU}; }
    code::Cost SHL_M8_I8() const final { return {1.00, 5, 9, 1, ALU | AGU | LSU}; }
    code::Cost SHL_R16_1() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost SHL_R16_CL() const final { return {0.25, 0, 13, 1, ALU}; }
    code::Cost SHL_R16_I8() const final { return {0.28, 1, 1, 1, ALU}; }
    code::Cost SHL_R32_1() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost SHL_R32_CL() const final { return {0.25, 0, 13, 1, ALU}; }
    code::Cost SHL_R32_I8() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost SHL_R64_1() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost SHL_R64_CL() const final { return {0.25, 0, 13, 1, ALU}; }
    code::Cost SHL_R64_I8() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost SHL_R8_1() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost SHL_R8_CL() const final { return {0.40, 0, 26, 1, ALU}; }
    code::Cost SHL_R8_I8() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost SHL_R8_CL() const final { return {0.25, 0, 13, 1, ALU}; }
    code::Cost SHL_R0_R8_1() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost SHL_R0_R8_I8() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost SHL_R4_R8_1() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost SHL_R4_R8_I8() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost SHLD_M16_R16_CL() const final { return {3.00, 0, 37, 8, ALU | AGU | LSU}; }
    code::Cost SHLD_M16_R16_I8() const final { return {3.00, 0, 12, 8, ALU | AGU | LSU}; }
    code::Cost SHLD_M32_R32_CL() const final { return {3.00, 0, 37, 8, ALU | AGU | LSU}; }
    code::Cost SHLD_M32_R32_I8() const final { return {3.00, 0, 11, 8, ALU | AGU | LSU}; }
    code::Cost SHLD_M64_R64_CL() const final { return {3.00, 0, 37, 8, ALU | AGU | LSU}; }
    code::Cost SHLD_M64_R64_I8() const final { return {3.00, 0, 11, 8, ALU | AGU | LSU}; }
    code::Cost SHLD_CL_R16_R16_CL() const final { return {3.00, 0, 16, 7, ALU}; }
    code::Cost SHLD_CL_R32_R32_CL() const final { return {3.00, 0, 16, 7, ALU}; }
    code::Cost SHLD_CL_R64_R64_CL() const final { return {3.00, 0, 16, 7, ALU}; }
    code::Cost SHLD_IMMB_R16_R16_I8() const final { return {3.00, 0, 3, 6, ALU}; }
    code::Cost SHLD_IMMB_R32_R32_I8() const final { return {3.00, 0, 3, 6, ALU}; }
    code::Cost SHLD_IMMB_R64_R64_I8() const final { return {3.00, 0, 3, 6, ALU}; }
    code::Cost SHR_M16_1() const final { return {1.00, 5, 9, 1, ALU | AGU | LSU}; }
    code::Cost SHR_M16_CL() const final { return {1.00, 0, 30, 1, ALU | AGU | LSU}; }
    code::Cost SHR_M16_I8() const final { return {1.00, 5, 9, 1, ALU | AGU | LSU}; }
    code::Cost SHR_M32_1() const final { return {1.00, 5, 8, 1, ALU | AGU | LSU}; }
    code::Cost SHR_M32_CL() const final { return {1.00, 0, 30, 1, ALU | AGU | LSU}; }
    code::Cost SHR_M32_I8() const final { return {1.00, 5, 8, 1, ALU | AGU | LSU}; }
    code::Cost SHR_M64_1() const final { return {1.00, 5, 8, 1, ALU | AGU | LSU}; }
    code::Cost SHR_M64_CL() const final { return {1.00, 0, 30, 1, ALU | AGU | LSU}; }
    code::Cost SHR_M64_I8() const final { return {1.00, 5, 8, 1, ALU | AGU | LSU}; }
    code::Cost SHR_M8_1() const final { return {1.00, 5, 9, 1, ALU | AGU | LSU}; }
    code::Cost SHR_M8_CL() const final { return {1.00, 0, 30, 1, ALU | AGU | LSU}; }
    code::Cost SHR_M8_I8() const final { return {1.00, 5, 9, 1, ALU | AGU | LSU}; }
    code::Cost SHR_R16_1() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost SHR_R16_CL() const final { return {0.25, 0, 13, 1, ALU}; }
    code::Cost SHR_R16_I8() const final { return {0.28, 1, 1, 1, ALU}; }
    code::Cost SHR_R32_1() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost SHR_R32_CL() const final { return {0.25, 0, 13, 1, ALU}; }
    code::Cost SHR_R32_I8() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost SHR_R64_1() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost SHR_R64_CL() const final { return {0.25, 0, 13, 1, ALU}; }
    code::Cost SHR_R64_I8() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost SHR_R8_1() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost SHR_R8_CL() const final { return {0.40, 0, 26, 1, ALU}; }
    code::Cost SHR_R8_I8() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost SHR_R8_CL() const final { return {0.25, 0, 13, 1, ALU}; }
    code::Cost SHR_R0_R8_1() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost SHR_R0_R8_I8() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost SHR_R4_R8_1() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost SHR_R4_R8_I8() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost SHRD_M16_R16_CL() const final { return {3.00, 0, 37, 8, ALU | AGU | LSU}; }
    code::Cost SHRD_M16_R16_I8() const final { return {3.00, 0, 12, 8, ALU | AGU | LSU}; }
    code::Cost SHRD_M32_R32_CL() const final { return {3.00, 0, 37, 8, ALU | AGU | LSU}; }
    code::Cost SHRD_M32_R32_I8() const final { return {3.00, 0, 11, 8, ALU | AGU | LSU}; }
    code::Cost SHRD_M64_R64_CL() const final { return {3.00, 0, 37, 8, ALU | AGU | LSU}; }
    code::Cost SHRD_M64_R64_I8() const final { return {3.00, 0, 11, 8, ALU | AGU | LSU}; }
    code::Cost SHRD_CL_R16_R16_CL() const final { return {3.00, 0, 16, 7, ALU}; }
    code::Cost SHRD_CL_R32_R32_CL() const final { return {3.00, 0, 16, 7, ALU}; }
    code::Cost SHRD_CL_R64_R64_CL() const final { return {3.00, 0, 16, 7, ALU}; }
    code::Cost SHRD_IMMB_R16_R16_I8() const final { return {3.00, 0, 3, 6, ALU}; }
    code::Cost SHRD_IMMB_R32_R32_I8() const final { return {3.00, 0, 3, 6, ALU}; }
    code::Cost SHRD_IMMB_R64_R64_I8() const final { return {3.00, 0, 3, 6, ALU}; }
    code::Cost SIDT_M80() const final { return {4.00, 23, 27, 9, ALU | AGU | LSU}; }
    code::Cost SLDT_M16() const final { return {3.00, 9, 9, 6, ALU | AGU | LSU}; }
    code::Cost SLDT_R16() const final { return {3.00, 1, 1, 6, ALU}; }
    code::Cost SLDT_R32() const final { return {3.00, 0, 0, 6, ALU}; }
    code::Cost SLDT_R64() const final { return {3.00, 0, 0, 6, ALU}; }
    code::Cost SMSW_M16() const final { return {3.00, 9, 9, 6, ALU | AGU | LSU}; }
    code::Cost SMSW_R16() const final { return {3.00, 1, 1, 6, ALU}; }
    code::Cost SMSW_R32() const final { return {3.00, 0, 0, 6, ALU}; }
    code::Cost SMSW_R64() const final { return {3.00, 0, 0, 6, ALU}; }
    code::Cost STC() const final { return {0.25, 0, 0, 1, ALU}; }
    code::Cost STD() const final { return {4.00, 0, 0, 2, ALU}; }
    code::Cost STI() const final { return {45.00, 0, 0, 14, ALU}; }
    code::Cost STOSB() const final { return {3.00, 0, 5, 3, ALU}; }
    code::Cost STOSB_REPE() const final { return {16.00, 0, 0, 6, ALU}; }
    code::Cost STOSB_REPNE() const final { return {16.00, 0, 0, 6, ALU}; }
    code::Cost STOSD() const final { return {3.00, 0, 4, 3, ALU}; }
    code::Cost STOSD_REPE() const final { return {16.00, 0, 0, 6, ALU}; }
    code::Cost STOSD_REPNE() const final { return {16.00, 0, 0, 6, ALU}; }
    code::Cost STOSW() const final { return {3.00, 0, 5, 3, ALU}; }
    code::Cost STOSW_REPE() const final { return {16.00, 0, 0, 6, ALU}; }
    code::Cost STOSW_REPNE() const final { return {16.00, 0, 0, 6, ALU}; }
    code::Cost STR_M16() const final { return {3.00, 9, 9, 6, ALU | AGU | LSU}; }
    code::Cost STR_R16() const final { return {3.00, 1, 1, 6, ALU}; }
    code::Cost STR_R32() const final { return {3.00, 0, 0, 6, ALU}; }
    code::Cost STR_R64() const final { return {3.00, 0, 0, 6, ALU}; }
    code::Cost SUB_AL_I8() const final { return {0.50, 1, 1, 1, ALU}; }
    code::Cost SUB_AX_I16() const final { return {0.50, 1, 1, 1, ALU}; }
    code::Cost SUB_EAX_I32() const final { return {0.50, 1, 1, 1, ALU}; }
    code::Cost SUB_M16_I16() const final { return {1.00, 5, 9, 1, ALU | AGU | LSU}; }
    code::Cost SUB_M16_I8() const final { return {1.00, 5, 9, 1, ALU | AGU | LSU}; }
    code::Cost SUB_M16_R16() const final { return {1.00, 1, 9, 1, ALU | AGU | LSU}; }
    code::Cost SUB_M32_I32() const final { return {1.00, 5, 8, 1, ALU | AGU | LSU}; }
    code::Cost SUB_M32_I8() const final { return {1.00, 5, 8, 1, ALU | AGU | LSU}; }
    code::Cost SUB_M32_R32() const final { return {1.00, 1, 8, 1, ALU | AGU | LSU}; }
    code::Cost SUB_M64_I32() const final { return {1.00, 5, 8, 1, ALU | AGU | LSU}; }
    code::Cost SUB_M64_I8() const final { return {1.00, 5, 8, 1, ALU | AGU | LSU}; }
    code::Cost SUB_M64_R64() const final { return {1.00, 1, 8, 1, ALU | AGU | LSU}; }
    code::Cost SUB_M8_I8() const final { return {1.00, 5, 9, 1, ALU | AGU | LSU}; }
    code::Cost SUB_M8_R8() const final { return {1.00, 1, 10, 1, ALU | AGU | LSU}; }
    code::Cost SUB_R16_I16() const final { return {0.36, 1, 1, 1, ALU}; }
    code::Cost SUB_R16_I8() const final { return {0.28, 1, 1, 1, ALU}; }
    code::Cost SUB_R16_M16() const final { return {0.50, 1, 7, 1, ALU | AGU | LSU}; }
    code::Cost SUB_R32_I32() const final { return {0.40, 1, 1, 1, ALU}; }
    code::Cost SUB_R32_I8() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost SUB_R32_M32() const final { return {0.50, 1, 7, 1, ALU | AGU | LSU}; }
    code::Cost SUB_R64_I32() const final { return {0.43, 1, 1, 1, ALU}; }
    code::Cost SUB_R64_I8() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost SUB_R64_M64() const final { return {0.50, 1, 7, 1, ALU | AGU | LSU}; }
    code::Cost SUB_R8_I8() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost SUB_R8_M8() const final { return {0.50, 1, 7, 1, ALU | AGU | LSU}; }
    code::Cost SUB_RAX_I32() const final { return {0.56, 1, 1, 1, ALU}; }
    code::Cost SUB_28_R8_R8() const final { return {0.38, 1, 1, 1, ALU}; }
    code::Cost SUB_29_R16_R16() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost SUB_29_R32_R32() const final { return {0.25, 0, 1, 1, ALU}; }
    code::Cost SUB_29_R64_R64() const final { return {0.25, 0, 1, 1, ALU}; }
    code::Cost SUB_2A_R8_R8() const final { return {0.38, 1, 1, 1, ALU}; }
    code::Cost SUB_2B_R16_R16() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost SUB_2B_R32_R32() const final { return {0.25, 0, 1, 1, ALU}; }
    code::Cost SUB_2B_R64_R64() const final { return {0.25, 0, 1, 1, ALU}; }
    code::Cost SUB_LOCK_M16_I16() const final { return {22.52, 7, 21, 1, ALU | AGU | LSU | LOCK}; }
    code::Cost SUB_LOCK_M16_I8() const final { return {22.52, 7, 21, 1, ALU | AGU | LSU | LOCK}; }
    code::Cost SUB_LOCK_M16_R16() const final { return {22.52, 1, 21, 1, ALU | AGU | LSU | LOCK}; }
    code::Cost SUB_LOCK_M32_I32() const final { return {22.52, 7, 21, 1, ALU | AGU | LSU | LOCK}; }
    code::Cost SUB_LOCK_M32_I8() const final { return {22.52, 7, 21, 1, ALU | AGU | LSU | LOCK}; }
    code::Cost SUB_LOCK_M32_R32() const final { return {22.52, 1, 21, 1, ALU | AGU | LSU | LOCK}; }
    code::Cost SUB_LOCK_M64_I32() const final { return {22.52, 7, 21, 1, ALU | AGU | LSU | LOCK}; }
    code::Cost SUB_LOCK_M64_I8() const final { return {22.52, 7, 21, 1, ALU | AGU | LSU | LOCK}; }
    code::Cost SUB_LOCK_M64_R64() const final { return {22.52, 1, 21, 1, ALU | AGU | LSU | LOCK}; }
    code::Cost SUB_LOCK_M8_I8() const final { return {22.52, 7, 21, 1, ALU | AGU | LSU | LOCK}; }
    code::Cost SUB_LOCK_M8_R8() const final { return {22.50, 1, 21, 1, ALU | AGU | LSU | LOCK}; }
    code::Cost SUB_R0_R8_I8() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost SUB_R4_R8_I8() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost SYSENTER() const final { return {0.0, 0, 0, 0}; }
    code::Cost SYSEXIT() const final { return {0.0, 0, 0, 0}; }
    code::Cost TEST_AL_I8() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost TEST_AX_I16() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost TEST_EAX_I32() const final { return {0.31, 1, 1, 1, ALU}; }
    code::Cost TEST_M16_I16() const final { return {0.50, 5, 7, 1, ALU | AGU | LSU}; }
    code::Cost TEST_M16_R16() const final { return {0.50, 1, 7, 1, ALU | AGU | LSU}; }
    code::Cost TEST_M32_I32() const final { return {0.50, 5, 7, 1, ALU | AGU | LSU}; }
    code::Cost TEST_M32_R32() const final { return {0.50, 1, 7, 1, ALU | AGU | LSU}; }
    code::Cost TEST_M64_I32() const final { return {0.50, 5, 7, 1, ALU | AGU | LSU}; }
    code::Cost TEST_M64_R64() const final { return {0.50, 1, 7, 1, ALU | AGU | LSU}; }
    code::Cost TEST_M8_I8() const final { return {0.50, 5, 7, 1, ALU | AGU | LSU}; }
    code::Cost TEST_M8_R8() const final { return {0.50, 1, 7, 1, ALU | AGU | LSU}; }
    code::Cost TEST_R16_I16() const final { return {0.38, 1, 1, 1, ALU}; }
    code::Cost TEST_R16_R16() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost TEST_R32_I32() const final { return {0.44, 1, 1, 1, ALU}; }
    code::Cost TEST_R32_R32() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost TEST_R64_I32() const final { return {0.44, 1, 1, 1, ALU}; }
    code::Cost TEST_R64_R64() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost TEST_R8_I8() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost TEST_R8_R8() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost TEST_RAX_I32() const final { return {0.38, 1, 1, 1, ALU}; }
    code::Cost TEST_R0_R8_I8() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost TEST_R4_R8_I8() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost UD2() const final { return {0.0, 0, 0, 0}; }
    code::Cost VERR_M16() const final { return {99.00, 97, 98, 32, ALU | AGU | LSU}; }
    code::Cost VERR_R16() const final { return {96.00, 91, 91, 32, ALU}; }
    code::Cost VERW_M16() const final { return {99.00, 97, 98, 32, ALU | AGU | LSU}; }
    code::Cost VERW_R16() const final { return {96.00, 91, 91, 32, ALU}; }
    code::Cost WBINVD() const final { return {69496.78, 0, 0, 111}; } // Specialized operation
    code::Cost WRMSR() const final { return {240.00, 0, 0, 105}; } // Specialized operation
    code::Cost XADD_M16_R16() const final { return {2.00, 1, 12, 4, ALU | AGU | LSU}; }
    code::Cost XADD_M32_R32() const final { return {2.00, 1, 11, 4, ALU | AGU | LSU}; }
    code::Cost XADD_M64_R64() const final { return {2.00, 1, 11, 4, ALU | AGU | LSU}; }
    code::Cost XADD_M8_R8() const final { return {2.00, 2, 12, 4, ALU | AGU | LSU}; }
    code::Cost XADD_R16_R16() const final { return {0.50, 1, 1, 2, ALU}; }
    code::Cost XADD_R32_R32() const final { return {0.33, 0, 1, 2, ALU}; }
    code::Cost XADD_R64_R64() const final { return {0.33, 0, 1, 2, ALU}; }
    code::Cost XADD_R8_R8() const final { return {0.50, 1, 1, 2, ALU}; }
    code::Cost XADD_LOCK_M16_R16() const final { return {28.44, 1, 27, 4, ALU | AGU | LSU | LOCK}; }
    code::Cost XADD_LOCK_M32_R32() const final { return {28.44, 1, 27, 4, ALU | AGU | LSU | LOCK}; }
    code::Cost XADD_LOCK_M64_R64() const final { return {28.44, 1, 27, 4, ALU | AGU | LSU | LOCK}; }
    code::Cost XADD_LOCK_M8_R8() const final { return {28.42, 1, 27, 4, ALU | AGU | LSU | LOCK}; }
    code::Cost XCHG_M16_R16() const final { return {29.38, 1, 10, 2, ALU | AGU | LSU}; }
    code::Cost XCHG_M32_R32() const final { return {29.37, 0, 11, 2, ALU | AGU | LSU}; }
    code::Cost XCHG_M64_R64() const final { return {29.37, 0, 11, 2, ALU | AGU | LSU}; }
    code::Cost XCHG_M8_R8() const final { return {29.38, 1, 10, 2, ALU | AGU | LSU}; }
    code::Cost XCHG_R16_AX() const final { return {1.00, 1, 1, 2, ALU}; }
    code::Cost XCHG_R16_R16() const final { return {0.50, 1, 1, 2, ALU}; }
    code::Cost XCHG_R32_EAX() const final { return {0.33, 0, 0, 2, ALU}; }
    code::Cost XCHG_R32_R32() const final { return {0.33, 0, 0, 2, ALU}; }
    code::Cost XCHG_R64_R64() const final { return {0.33, 0, 0, 2, ALU}; }
    code::Cost XCHG_R64_RAX() const final { return {0.33, 0, 0, 2, ALU}; }
    code::Cost XCHG_R8_R8() const final { return {0.50, 1, 1, 2, ALU}; }
    code::Cost XLAT() const final { return {2.00, 5, 7, 2, ALU | AGU | LSU}; }
    code::Cost XOR_AL_I8() const final { return {0.50, 1, 1, 1, ALU}; }
    code::Cost XOR_AX_I16() const final { return {0.50, 1, 1, 1, ALU}; }
    code::Cost XOR_EAX_I32() const final { return {0.50, 1, 1, 1, ALU}; }
    code::Cost XOR_M16_I16() const final { return {1.00, 5, 9, 1, ALU | AGU | LSU}; }
    code::Cost XOR_M16_I8() const final { return {1.00, 5, 9, 1, ALU | AGU | LSU}; }
    code::Cost XOR_M16_R16() const final { return {1.00, 1, 9, 1, ALU | AGU | LSU}; }
    code::Cost XOR_M32_I32() const final { return {1.00, 5, 8, 1, ALU | AGU | LSU}; }
    code::Cost XOR_M32_I8() const final { return {1.00, 5, 8, 1, ALU | AGU | LSU}; }
    code::Cost XOR_M32_R32() const final { return {1.00, 1, 8, 1, ALU | AGU | LSU}; }
    code::Cost XOR_M64_I32() const final { return {1.00, 5, 8, 1, ALU | AGU | LSU}; }
    code::Cost XOR_M64_I8() const final { return {1.00, 5, 8, 1, ALU | AGU | LSU}; }
    code::Cost XOR_M64_R64() const final { return {1.00, 1, 8, 1, ALU | AGU | LSU}; }
    code::Cost XOR_M8_I8() const final { return {1.00, 5, 9, 1, ALU | AGU | LSU}; }
    code::Cost XOR_M8_R8() const final { return {1.00, 1, 10, 1, ALU | AGU | LSU}; }
    code::Cost XOR_R16_I16() const final { return {0.36, 1, 1, 1, ALU}; }
    code::Cost XOR_R16_I8() const final { return {0.28, 1, 1, 1, ALU}; }
    code::Cost XOR_R16_M16() const final { return {0.50, 1, 7, 1, ALU | AGU | LSU}; }
    code::Cost XOR_R32_I32() const final { return {0.40, 1, 1, 1, ALU}; }
    code::Cost XOR_R32_I8() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost XOR_R32_M32() const final { return {0.50, 1, 7, 1, ALU | AGU | LSU}; }
    code::Cost XOR_R64_I32() const final { return {0.43, 1, 1, 1, ALU}; }
    code::Cost XOR_R64_I8() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost XOR_R64_M64() const final { return {0.50, 1, 7, 1, ALU | AGU | LSU}; }
    code::Cost XOR_R8_I8() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost XOR_R8_M8() const final { return {0.50, 1, 7, 1, ALU | AGU | LSU}; }
    code::Cost XOR_RAX_I32() const final { return {0.56, 1, 1, 1, ALU}; }
    code::Cost XOR_30_R8_R8() const final { return {0.38, 1, 1, 1, ALU}; }
    code::Cost XOR_31_R16_R16() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost XOR_31_R32_R32() const final { return {0.25, 0, 1, 1, ALU}; }
    code::Cost XOR_31_R64_R64() const final { return {0.25, 0, 1, 1, ALU}; }
    code::Cost XOR_32_R8_R8() const final { return {0.38, 1, 1, 1, ALU}; }
    code::Cost XOR_33_R16_R16() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost XOR_33_R32_R32() const final { return {0.25, 0, 1, 1, ALU}; }
    code::Cost XOR_33_R64_R64() const final { return {0.25, 0, 1, 1, ALU}; }
    code::Cost XOR_LOCK_M16_I16() const final { return {22.52, 7, 21, 1, ALU | AGU | LSU | LOCK}; }
    code::Cost XOR_LOCK_M16_I8() const final { return {22.52, 7, 21, 1, ALU | AGU | LSU | LOCK}; }
    code::Cost XOR_LOCK_M16_R16() const final { return {22.52, 1, 21, 1, ALU | AGU | LSU | LOCK}; }
    code::Cost XOR_LOCK_M32_I32() const final { return {22.52, 7, 21, 1, ALU | AGU | LSU | LOCK}; }
    code::Cost XOR_LOCK_M32_I8() const final { return {22.52, 7, 21, 1, ALU | AGU | LSU | LOCK}; }
    code::Cost XOR_LOCK_M32_R32() const final { return {22.52, 1, 21, 1, ALU | AGU | LSU | LOCK}; }
    code::Cost XOR_LOCK_M64_I32() const final { return {22.52, 7, 21, 1, ALU | AGU | LSU | LOCK}; }
    code::Cost XOR_LOCK_M64_I8() const final { return {22.52, 7, 21, 1, ALU | AGU | LSU | LOCK}; }
    code::Cost XOR_LOCK_M64_R64() const final { return {22.52, 1, 21, 1, ALU | AGU | LSU | LOCK}; }
    code::Cost XOR_LOCK_M8_I8() const final { return {22.52, 7, 21, 1, ALU | AGU | LSU | LOCK}; }
    code::Cost XOR_LOCK_M8_R8() const final { return {22.50, 1, 21, 1, ALU | AGU | LSU | LOCK}; }
    code::Cost XOR_R0_R8_I8() const final { return {0.25, 1, 1, 1, ALU}; }
    code::Cost XOR_R4_R8_I8() const final { return {0.25, 1, 1, 1, ALU}; }

    //     code::Cost ADDPS_XMM_M128() const final { return {0.5, 3, 11, 1, FADD}; }
    code::Cost ADDPS_XMM_XMM() const final { return {0.5, 3, 3, 1, FADD}; }
    code::Cost ADDSS_XMM_M32() const final { return {0.5, 3, 11, 1, FADD}; }
    code::Cost ADDSS_XMM_XMM() const final { return {0.5, 3, 3, 1, FADD}; }

    code::Cost ANDNPS_XMM_M128() const final { return {0.375, 1, 9, 1, FMUL}; }
    code::Cost ANDNPS_XMM_XMM() const final { return {0.25, 0, 1, 1, FMUL}; }
    code::Cost ANDPS_XMM_M128() const final { return {0.375, 1, 9, 1, FMUL}; }
    code::Cost ANDPS_XMM_XMM() const final { return {0.25, 1, 1, 1, FMUL}; }

    code::Cost CMPPS_XMM_M128_I8() const final { return {0.5, 1, 9, 1, FADD}; }
    code::Cost CMPPS_XMM_XMM_I8() const final { return {0.5, 1, 1, 1, FADD}; }
    code::Cost CMPSS_XMM_M32_I8() const final { return {0.5, 1, 9, 1, FADD}; }
    code::Cost CMPSS_XMM_XMM_I8() const final { return {0.5, 1, 1, 1, FADD}; }
    code::Cost COMISS_XMM_M32() const final { return {1.00, 7, 12, 2, FADD}; }
    code::Cost COMISS_XMM_XMM() const final { return {1.00, 7, 7, 2, FADD}; }

    code::Cost CVTPI2PS_XMM_M64() const final { return {1.0, 4, 12, 1, FMISC}; }
    code::Cost CVTPI2PS_XMM_MM() const final { return {1.00, 4, 7, 2, FMISC}; }
    code::Cost CVTPS2PI_MM_M64() const final { return {1.0, 11, 12, 1, FMISC}; }
    code::Cost CVTPS2PI_MM_XMM() const final { return {1.0, 6, 6, 1, FMISC}; }
    code::Cost CVTSI2SS_XMM_M32() const final { return {1.0, 4, 12, 1, FMISC}; }
    code::Cost CVTSI2SS_XMM_M64() const final { return {1.0, 4, 12, 1, FMISC}; }
    code::Cost CVTSI2SS_XMM_R32() const final { return {1.00, 4, 10, 2, FMISC}; }
    code::Cost CVTSI2SS_XMM_R64() const final { return {1.00, 4, 10, 2, FMISC}; }
    code::Cost CVTSS2SI_R32_M32() const final { return {1.00, 13, 15, 2, FMISC}; }
    code::Cost CVTSS2SI_R32_XMM() const final { return {1.00, 10, 10, 2, FMISC}; }
    code::Cost CVTSS2SI_R64_M32() const final { return {1.00, 13, 15, 2, FMISC}; }
    code::Cost CVTSS2SI_R64_XMM() const final { return {1.00, 10, 10, 2, FMISC}; }
    code::Cost CVTTPS2PI_MM_M64() const final { return {1.0, 11, 12, 1, FMISC}; }
    code::Cost CVTTPS2PI_MM_XMM() const final { return {1.0, 6, 6, 1, FMISC}; }
    code::Cost CVTTSS2SI_R32_M32() const final { return {1.00, 13, 15, 2, FMISC}; }
    code::Cost CVTTSS2SI_R32_XMM() const final { return {1.00, 10, 10, 2, FMISC}; }
    code::Cost CVTTSS2SI_R64_M32() const final { return {1.00, 13, 15, 2, FMISC}; }
    code::Cost CVTTSS2SI_R64_XMM() const final { return {1.00, 10, 10, 2, FMISC}; }

    code::Cost DIVPS_XMM_M128() const final { return {3.00, 10, 18, 1, FMUL}; }
    code::Cost DIVPS_XMM_XMM() const final { return {3.00, 10, 10, 1, FMUL}; }
    code::Cost DIVSS_XMM_M32() const final { return {3.00, 10, 18, 1, FMUL}; }
    code::Cost DIVSS_XMM_XMM() const final { return {3.00, 10, 10, 1, FMUL}; }

    code::Cost FXRSTOR_M4096() const final { return {0.0, 0, 0, 0, UNKNOWN}; }
    code::Cost FXRSTOR64_M4096() const final { return {0.0, 0, 0, 0, UNKNOWN}; }
    code::Cost FXSAVE_M4096() const final { return {96.00, 0, 0, 87, UNKNOWN}; }
    code::Cost FXSAVE64_M4096() const final { return {96.00, 0, 0, 87, UNKNOWN}; }
    code::Cost LDMXCSR_M32() const final { return {1.5, 0, 0, 1, UNKNOWN}; }

    code::Cost MAXPS_XMM_M128() const final { return {0.5, 1, 9, 1, FADD}; }
    code::Cost MAXPS_XMM_XMM() const final { return {0.5, 1, 1, 1, FADD}; }
    code::Cost MAXSS_XMM_M32() const final { return {0.5, 1, 9, 1, FADD}; }
    code::Cost MAXSS_XMM_XMM() const final { return {0.5, 1, 1, 1, FADD}; }
    code::Cost MINPS_XMM_M128() const final { return {0.5, 1, 9, 1, FADD}; }
    code::Cost MINPS_XMM_XMM() const final { return {0.5, 1, 1, 1, FADD}; }
    code::Cost MINSS_XMM_M32() const final { return {0.5, 1, 9, 1, FADD}; }
    code::Cost MINSS_XMM_XMM() const final { return {0.5, 1, 1, 1, FADD}; }

    code::Cost MOVAPS_M128_XMM() const final { return {1.0, 7, 8, 1, FMISC}; }
    code::Cost MOVAPS_XMM_M128() const final { return {0.50, 7, 8, 1, FMISC}; }
    code::Cost MOVAPS_0F28_XMM_XMM() const final { return {0.25, 0, 0, 1, FADD | FMUL}; }
    code::Cost MOVAPS_0F29_XMM_XMM() const final { return {0.25, 0, 0, 1, FADD | FMUL}; }
    code::Cost MOVHLPS_XMM_XMM() const final { return {0.5, 1, 1, 1, FADD | FMUL}; }
    code::Cost MOVHPS_M64_XMM() const final { return {1.00, 8, 8, 2, FMISC}; }
    code::Cost MOVHPS_XMM_M64() const final { return {0.5, 1, 9, 1, FMISC}; }
    code::Cost MOVLHPS_XMM_XMM() const final { return {0.5, 1, 1, 1, FADD | FMUL}; }
    code::Cost MOVLPS_M64_XMM() const final { return {1.0, 7, 8, 1, FMISC}; }
    code::Cost MOVLPS_XMM_M64() const final { return {0.375, 1, 9, 1, FMISC}; }
    code::Cost MOVMSKPS_R32_XMM() const final { return {1.0, 5, 5, 1, FADD}; }
    code::Cost MOVNTPS_M128_XMM() const final { return {1.0, 1246, 1249, 1, FMISC}; }

    code::Cost MOVSS_M32_XMM() const final { return {1.0, 7, 8, 1, FMISC}; }
    code::Cost MOVSS_XMM_M32() const final { return {0.50, 7, 8, 1, FMISC}; } // FANY
    code::Cost MOVSS_0F10_XMM_XMM() const final { return {0.25, 1, 1, 1, FADD | FMUL}; }
    code::Cost MOVSS_0F11_XMM_XMM() const final { return {0.25, 1, 1, 1, FADD | FMUL}; }

    code::Cost MOVUPS_M128_XMM() const final { return {1.0, 7, 8, 1, FMISC}; } // Not listed?
    code::Cost MOVUPS_XMM_M128() const final { return {0.50, 7, 8, 1, FMISC}; } // Not listed?
    code::Cost MOVUPS_0F10_XMM_XMM() const final { return {0.25, 0, 0, 1, FADD | FMUL}; }
    code::Cost MOVUPS_0F11_XMM_XMM() const final { return {0.25, 0, 0, 1, FADD | FMUL}; }

    code::Cost MULPS_XMM_M128() const final { return {0.5, 3, 11, 1, FMUL}; }
    code::Cost MULPS_XMM_XMM() const final { return {0.5, 3, 3, 1, FMUL}; }
    code::Cost MULSS_XMM_M32() const final { return {0.5, 3, 11, 1, FMUL}; }
    code::Cost MULSS_XMM_XMM() const final { return {0.5, 3, 3, 1, FMUL}; }

    code::Cost ORPS_XMM_M128() const final { return {0.375, 1, 9, 1, FMUL}; }
    code::Cost ORPS_XMM_XMM() const final { return {0.25, 1, 1, 1, FMUL}; }

    code::Cost PREFETCHNTA_M512() const final { return {0.50, 0, 0, 1}; } // Unsure
    code::Cost PREFETCHT0_M512() const final { return {0.50, 0, 0, 1}; } // Unsure
    code::Cost PREFETCHT1_M512() const final { return {0.50, 0, 0, 1}; } // Unsure
    code::Cost PREFETCHT2_M512() const final { return {0.50, 0, 0, 1}; } // Unsure

    code::Cost RCPPS_XMM_M128() const final { return {0.75, 12, 13, 1, FMISC}; }
    code::Cost RCPPS_XMM_XMM() const final { return {0.75, 5, 5, 1, FMISC}; }
    code::Cost RCPSS_XMM_M32() const final { return {0.75, 5, 13, 1, FMISC}; }
    code::Cost RCPSS_XMM_XMM() const final { return {0.75, 5, 5, 1, FMISC}; }
    code::Cost RSQRTPS_XMM_M128() const final { return {0.75, 13, 13, 1, FMUL}; }
    code::Cost RSQRTPS_XMM_XMM() const final { return {0.75, 5, 5, 1, FMUL}; }
    code::Cost RSQRTSS_XMM_M32() const final { return {0.75, 5, 13, 1, FMUL}; }
    code::Cost RSQRTSS_XMM_XMM() const final { return {0.75, 5, 5, 1, FMUL}; }

    code::Cost SFENCE() const final { return {23.00, 0, 0, 4}; }
    code::Cost SHUFPS_XMM_M128_I8() const final { return {0.5, 1, 9, 1, FMUL}; }
    code::Cost SHUFPS_XMM_XMM_I8() const final { return {0.5, 1, 1, 1, FMUL}; }

    code::Cost SQRTPS_XMM_M128() const final { return {2.5, 16.0, 22, 1, FMUL}; }
    code::Cost SQRTPS_XMM_XMM() const final { return {2.5, 8.0, 14, 1, FMUL}; }
    code::Cost SQRTSS_XMM_M32() const final { return {2.5, 8.0, 22, 1, FMUL}; }
    code::Cost SQRTSS_XMM_XMM() const final { return {2.5, 8, 14, 1, FMUL}; }

    code::Cost STMXCSR_M32() const final { return {15.00, 18, 18, 2}; } // Not listed?

    code::Cost SUBPS_XMM_M128() const final { return {0.5, 3, 11, 1, FADD}; }
    code::Cost SUBPS_XMM_XMM() const final { return {0.5, 3, 3, 1, FADD}; }
    code::Cost SUBSS_XMM_M32() const final { return {0.5, 3, 11, 1, FADD}; }
    code::Cost SUBSS_XMM_XMM() const final { return {0.5, 3, 3, 1, FADD}; }

    code::Cost UCOMISS_XMM_M32() const final { return {1.00, 7, 12, 2, FADD}; }
    code::Cost UCOMISS_XMM_XMM() const final { return {1.00, 7, 7, 2, FADD}; }

    code::Cost UNPCKHPS_XMM_M128() const final { return {0.5, 1, 9, 1, FMUL}; }
    code::Cost UNPCKHPS_XMM_XMM() const final { return {0.5, 1, 1, 1, FMUL}; }
    code::Cost UNPCKLPS_XMM_M128() const final { return {0.5, 1, 9, 1, FMUL}; }
    code::Cost UNPCKLPS_XMM_XMM() const final { return {0.5, 1, 1, 1, FMUL}; }

    code::Cost XORPS_XMM_M128() const final { return {0.375, 1, 9, 1, FMUL}; }
    code::Cost XORPS_XMM_XMM() const final { return {0.25, 0, 1, 1, FMUL}; }

    code::Cost ADDPD_XMM_M128() const final { return {0.5, 3, 11, 1, FADD}; }
    code::Cost ADDPD_XMM_XMM() const final { return {0.5, 3, 3, 1, FADD}; }
    code::Cost ADDSD_XMM_M64() const final { return {0.5, 3, 11, 1, FADD}; }
    code::Cost ADDSD_XMM_XMM() const final { return {0.5, 3, 3, 1, FADD}; }

    code::Cost ANDNPD_XMM_M128() const final { return {0.375, 1, 9, 1, FMUL}; }
    code::Cost ANDNPD_XMM_XMM() const final { return {0.25, 0, 1, 1, FMUL}; }
    code::Cost ANDPD_XMM_M128() const final { return {0.375, 1, 9, 1, FMUL}; }
    code::Cost ANDPD_XMM_XMM() const final { return {0.25, 1, 1, 1, FMUL}; }

    code::Cost CMPPD_XMM_M128_I8() const final { return {0.5, 1, 9, 1, FADD}; }
    code::Cost CMPPD_XMM_XMM_I8() const final { return {0.5, 1, 1, 1, FADD}; }
    code::Cost CMPSD_XMM_XMM_M64_I8() const final { return {0.5, 1, 9, 1, FADD}; }
    code::Cost CMPSD_XMM_XMM_XMM_I8() const final { return {0.5, 1, 1, 1, FADD}; }

    code::Cost COMISD_XMM_M64() const final { return {1.00, 7, 12, 2, FADD}; }
    code::Cost COMISD_XMM_XMM() const final { return {1.00, 7, 7, 2, FADD}; }

    code::Cost CVTDQ2PD_XMM_M64() const final { return {1.00, 13, 14, 2, FMISC}; }
    code::Cost CVTDQ2PD_XMM_XMM() const final { return {1.00, 7, 7, 2, FMISC}; }
    code::Cost CVTDQ2PS_XMM_M128() const final { return {1.0, 11, 12, 1, FMISC}; }
    code::Cost CVTDQ2PS_XMM_XMM() const final { return {1.0, 4, 4, 1, FMISC}; }
    code::Cost CVTPD2DQ_XMM_M128() const final { return {1.00, 13, 14, 2, FMISC}; }
    code::Cost CVTPD2DQ_XMM_XMM() const final { return {1.00, 7, 7, 2, FMISC}; }
    code::Cost CVTPD2PI_MM_M128() const final { return {1.00, 14, 23, 2, FMISC}; }
    code::Cost CVTPD2PI_MM_XMM() const final { return {1.00, 7, 7, 2, FMISC}; }
    code::Cost CVTPD2PS_XMM_M128() const final { return {1.0, 10, 11, 1, FMISC}; }
    code::Cost CVTPD2PS_XMM_XMM() const final { return {1.0, 3, 3, 1, FMISC}; }
    code::Cost CVTPI2PD_XMM_M64() const final { return {1.00, 13, 14, 2, FMISC}; }
    code::Cost CVTPI2PD_XMM_MM() const final { return {1.00, 7, 7, 2, FMISC}; }
    code::Cost CVTPS2DQ_XMM_M128() const final { return {1.0, 11, 12, 1, FMISC}; }
    code::Cost CVTPS2DQ_XMM_XMM() const final { return {1.0, 4, 4, 1, FMISC}; }
    code::Cost CVTPS2PD_XMM_M64() const final { return {1.0, 10, 11, 1, FMISC}; }
    code::Cost CVTPS2PD_XMM_XMM() const final { return {1.0, 3, 3, 1, FMISC}; }
    code::Cost CVTSD2SI_R32_M64() const final { return {1.00, 13, 15, 2, FMISC}; }
    code::Cost CVTSD2SI_R32_XMM() const final { return {1.00, 10, 10, 2, FMISC}; }
    code::Cost CVTSD2SI_R64_M64() const final { return {1.00, 13, 15, 2, FMISC}; }
    code::Cost CVTSD2SI_R64_XMM() const final { return {1.00, 10, 10, 2, FMISC}; }
    code::Cost CVTSD2SS_XMM_M64() const final { return {1.0, 4, 12, 1, FMISC}; }
    code::Cost CVTSD2SS_XMM_XMM() const final { return {1.0, 4, 4, 1, FMISC}; }
    code::Cost CVTSI2SD_XMM_M32() const final { return {1.0, 4, 12, 1, FMISC}; }
    code::Cost CVTSI2SD_XMM_M64() const final { return {1.0, 4, 12, 1, FMISC}; }
    code::Cost CVTSI2SD_XMM_R32() const final { return {1.00, 4, 10, 2, FMISC}; }
    code::Cost CVTSI2SD_XMM_R64() const final { return {1.00, 4, 10, 2, FMISC}; }
    code::Cost CVTSS2SD_XMM_M32() const final { return {1.0, 4, 12, 1, FMISC}; }
    code::Cost CVTSS2SD_XMM_XMM() const final { return {1.0, 4, 4, 1, FMISC}; }
    code::Cost CVTTPD2DQ_XMM_M128() const final { return {1.00, 13, 14, 2, FMISC}; }
    code::Cost CVTTPD2DQ_XMM_XMM() const final { return {1.00, 7, 7, 2, FMISC}; }
    code::Cost CVTTPD2PI_MM_M128() const final { return {1.00, 14, 23, 2, FMISC}; }
    code::Cost CVTTPD2PI_MM_XMM() const final { return {1.00, 7, 7, 2, FMISC}; }
    code::Cost CVTTPS2DQ_XMM_M128() const final { return {1.0, 11, 12, 1, FMISC}; }
    code::Cost CVTTPS2DQ_XMM_XMM() const final { return {1.0, 4, 4, 1, FMISC}; }
    code::Cost CVTTSD2SI_R32_M64() const final { return {1.00, 13, 15, 2, FMISC}; }
    code::Cost CVTTSD2SI_R32_XMM() const final { return {1.00, 10, 10, 2, FMISC}; }
    code::Cost CVTTSD2SI_R64_M64() const final { return {1.00, 13, 15, 2, FMISC}; }
    code::Cost CVTTSD2SI_R64_XMM() const final { return {1.00, 10, 10, 2, FMISC}; }

    code::Cost DIVPD_XMM_M128() const final { return {4.00, 8.0, 21, 1, FMUL}; }
    code::Cost DIVPD_XMM_XMM() const final { return {4.00, 8.0, 13, 1, FMUL}; }
    code::Cost DIVSD_XMM_M64() const final { return {4.00, 8.0, 21, 1, FMUL}; }
    code::Cost DIVSD_XMM_XMM() const final { return {4.00, 8.0, 13, 1, FMUL}; }

    code::Cost LFENCE() const final { return {17.00, 0, 0, 1}; } // Not sure?

    code::Cost MASKMOVDQU_XMM_XMM() const final { return {18.00, 13, 13, 60}; } // Not sure?
    code::Cost MAXPD_XMM_M128() const final { return {0.5, 1, 9, 1, FADD}; }
    code::Cost MAXPD_XMM_XMM() const final { return {0.5, 1, 1, 1, FADD}; }
    code::Cost MAXSD_XMM_M64() const final { return {0.5, 1, 9, 1, FADD}; }
    code::Cost MAXSD_XMM_XMM() const final { return {0.5, 1, 1, 1, FADD}; }

    code::Cost MFENCE() const final { return {77.13, 0, 0, 7}; } // Not sure?

    code::Cost MINPD_XMM_M128() const final { return {0.5, 1, 9, 1, FADD}; }
    code::Cost MINPD_XMM_XMM() const final { return {0.5, 1, 1, 1, FADD}; }
    code::Cost MINSD_XMM_M64() const final { return {0.5, 1, 9, 1, FADD}; }
    code::Cost MINSD_XMM_XMM() const final { return {0.5, 1, 1, 1, FADD}; }

    code::Cost MOVAPD_M128_XMM() const final { return {1.0, 7, 8, 1, FMISC}; }
    code::Cost MOVAPD_XMM_M128() const final { return {0.50, 7, 8, 1, FMISC}; }
    code::Cost MOVAPD_0F28_XMM_XMM() const final { return {0.25, 0, 0, 1, FADD | FMUL}; }
    code::Cost MOVAPD_0F29_XMM_XMM() const final { return {0.25, 0, 0, 1, FADD | FMUL}; }

    code::Cost MOVD_M32_XMM() const final { return {1.0, 7, 8, 1, FANY}; }
    code::Cost MOVD_R32_XMM() const final { return {1.0, 5, 5, 1, FMISC | ALU}; }
    code::Cost MOVD_XMM_M32() const final { return {0.50, 7, 8, 1, FANY}; }
    code::Cost MOVD_XMM_R32() const final { return {1.00, 5, 5, 1}; } // Not listed?

    code::Cost MOVDQ2Q_MM_XMM() const final { return {0.25, 1, 1, 1, FADD | FMUL}; }
    code::Cost MOVDQA_M128_XMM() const final { return {1.0, 7, 8, 1, FMISC}; }
    code::Cost MOVDQA_XMM_M128() const final { return {0.50, 7, 8, 1, FMISC}; }
    code::Cost MOVDQA_0F6F_XMM_XMM() const final { return {0.25, 0, 0, 1, FADD | FMUL}; }
    code::Cost MOVDQA_0F7F_XMM_XMM() const final { return {0.25, 0, 0, 1, FADD | FMUL}; }

    code::Cost MOVDQU_M128_XMM() const final { return {1.0, 7, 8, 1}; } // Not listed?
    code::Cost MOVDQU_XMM_M128() const final { return {0.50, 7, 8, 1}; } // Not listed?
    code::Cost MOVDQU_0F6F_XMM_XMM() const final { return {0.25, 0, 0, 1}; } // Not listed?
    code::Cost MOVDQU_0F7F_XMM_XMM() const final { return {0.25, 0, 0, 1}; } // Not listed?

    code::Cost MOVHPD_M64_XMM() const final { return {1.00, 8, 8, 2, FMISC}; }
    code::Cost MOVHPD_XMM_M64() const final { return {0.5, 1, 9, 1, FMISC}; }
    code::Cost MOVLPD_M64_XMM() const final { return {1.0, 7, 8, 1, FMISC}; }
    code::Cost MOVLPD_XMM_M64() const final { return {0.375, 1, 9, 1, FMISC}; }

    code::Cost MOVMSKPD_R32_XMM() const final { return {1.0, 5, 5, 1, FMISC}; }
    code::Cost MOVNTDQ_M128_XMM() const final { return {1.0, 1247, 1251, 1, FMISC}; }

    code::Cost MOVNTI_M32_R32() const final { return {1.00, 1244, 1253, 1, FMISC}; }
    code::Cost MOVNTI_M64_R64() const final { return {1.00, 1245, 1251, 1, FMISC}; }
    code::Cost MOVNTPD_M128_XMM() const final { return {1.0, 1247, 1251, 1, FMISC}; }

    code::Cost MOVQ_M64_XMM() const final { return {1.0, 7, 8, 1, FANY}; }
    code::Cost MOVQ_R64_XMM() const final { return {1.0, 5, 5, 1, FANY | ALU}; }
    code::Cost MOVQ_XMM_M64() const final { return {0.50, 7, 8, 1, FANY | FMISC}; }
    code::Cost MOVQ_XMM_R64() const final { return {1.00, 5, 5, 1, FANY | ALU}; }

    code::Cost MOVQ2DQ_XMM_MM() const final { return {0.25, 1, 1, 1, FADD | FMUL}; }
    code::Cost MOVQ_0F7E_XMM_XMM() const final { return {0.25, 1, 1, 1, FADD | FMUL | FMISC}; }
    code::Cost MOVQ_0FD6_XMM_XMM() const final { return {0.25, 1, 1, 1, FADD | FMUL | FMISC}; }
    code::Cost MOVSD_XMM_M64_XMM() const final { return {1.0, 7, 8, 1, FMISC}; }
    code::Cost MOVSD_XMM_XMM_M64() const final { return {0.50, 7, 8, 1, FMISC}; }
    code::Cost MOVSD_XMM_0F10_XMM_XMM() const final { return {0.25, 1, 1, 1, FMISC}; }
    code::Cost MOVSD_XMM_0F11_XMM_XMM() const final { return {0.25, 1, 1, 1, FMISC}; }
    code::Cost MOVUPD_M128_XMM() const final { return {1.0, 7, 8, 1, FMISC}; }
    code::Cost MOVUPD_XMM_M128() const final { return {0.50, 7, 8, 1, FMISC}; }

    code::Cost MOVUPD_0F10_XMM_XMM() const final { return {0.25, 0, 0, 1, FMISC}; }
    code::Cost MOVUPD_0F11_XMM_XMM() const final { return {0.25, 0, 0, 1, FMISC}; }


    code::Cost MULPD_XMM_M128() const final { return {0.5, 4, 12, 1, FMUL}; }
    code::Cost MULPD_XMM_XMM() const final { return {0.5, 4, 4, 1, FMUL}; }
    code::Cost MULSD_XMM_M64() const final { return {0.5, 4, 12, 1, FMUL}; }
    code::Cost MULSD_XMM_XMM() const final { return {0.5, 4, 4, 1, FMUL}; }
    code::Cost ORPD_XMM_M128() const final { return {0.375, 1, 9, 1, FMUL}; }
    code::Cost ORPD_XMM_XMM() const final { return {0.25, 1, 1, 1, FMUL}; }

    code::Cost PACKSSDW_XMM_M128() const final { return {0.5, 1, 9, 1, FADD | FMUL}; }
    code::Cost PACKSSDW_XMM_XMM() const final { return {0.5, 1, 1, 1, FADD | FMUL}; }
    code::Cost PACKSSWB_XMM_M128() const final { return {0.5, 1, 9, 1, FADD | FMUL}; }
    code::Cost PACKSSWB_XMM_XMM() const final { return {0.5, 1, 1, 1, FADD | FMUL}; }
    code::Cost PACKUSWB_XMM_M128() const final { return {0.5, 1, 9, 1, FADD | FMUL}; }
    code::Cost PACKUSWB_XMM_XMM() const final { return {0.5, 1, 1, 1, FADD | FMUL}; }
    
    code::Cost PADDB_XMM_M128() const final { return {0.41500000000000004, 1, 9, 1, FADD | FMUL}; }
    code::Cost PADDB_XMM_XMM() const final { return {0.33, 1, 1, 1, FADD | FMUL}; }
    code::Cost PADDD_XMM_M128() const final { return {0.41500000000000004, 1, 9, 1, FADD | FMUL}; }
    code::Cost PADDD_XMM_XMM() const final { return {0.33, 1, 1, 1, FADD | FMUL}; }
    code::Cost PADDQ_MM_M64() const final { return {0.41500000000000004, 1, 9, 1, FADD | FMUL}; }
    code::Cost PADDQ_MM_MM() const final { return {0.33999999999999997, 1, 1, 1, FADD | FMUL}; }
    code::Cost PADDQ_XMM_M128() const final { return {0.41500000000000004, 1, 9, 1, FADD | FMUL}; }
    code::Cost PADDQ_XMM_XMM() const final { return {0.33, 1, 1, 1, FADD | FMUL}; }
    code::Cost PADDSB_XMM_M128() const final { return {0.5, 1, 9, 1, FADD | FMUL}; }
    code::Cost PADDSB_XMM_XMM() const final { return {0.5, 1, 1, 1, FADD | FMUL}; }
    code::Cost PADDSW_XMM_M128() const final { return {0.5, 1, 9, 1, FADD | FMUL}; }
    code::Cost PADDSW_XMM_XMM() const final { return {0.5, 1, 1, 1, FADD | FMUL}; }
    code::Cost PADDUSB_XMM_M128() const final { return {0.5, 1, 9, 1, FADD | FMUL}; }
    code::Cost PADDUSB_XMM_XMM() const final { return {0.5, 1, 1, 1, FADD | FMUL}; }
    code::Cost PADDUSW_XMM_M128() const final { return {0.5, 1, 9, 1, FADD | FMUL}; }
    code::Cost PADDUSW_XMM_XMM() const final { return {0.5, 1, 1, 1, FADD | FMUL}; }
    code::Cost PADDW_XMM_M128() const final { return {0.41500000000000004, 1, 9, 1, FADD | FMUL}; }
    code::Cost PADDW_XMM_XMM() const final { return {0.33, 1, 1, 1, FADD | FMUL}; }
    
    code::Cost PAND_XMM_M128() const final { return {0.375, 1, 9, 1, FMUL}; }
    code::Cost PAND_XMM_XMM() const final { return {0.25, 1, 1, 1, FMUL}; }
    code::Cost PANDN_XMM_M128() const final { return {0.375, 1, 9, 1, FMUL}; }
    code::Cost PANDN_XMM_XMM() const final { return {0.25, 0, 1, 1, FMUL}; }

    code::Cost PAVGB_XMM_M128() const final { return {0.5, 1, 9, 1, FADD | FMUL}; }
    code::Cost PAVGB_XMM_XMM() const final { return {0.5, 1, 1, 1, FADD | FMUL}; }
    code::Cost PAVGW_XMM_M128() const final { return {0.5, 1, 9, 1, FADD | FMUL}; }
    code::Cost PAVGW_XMM_XMM() const final { return {0.5, 1, 1, 1, FADD | FMUL}; }

    code::Cost PCMPEQB_XMM_M128() const final { return {0.41500000000000004, 1, 9, 1, FADD | FMUL}; }
    code::Cost PCMPEQB_XMM_XMM() const final { return {0.33, 0, 1, 1, FADD | FMUL}; }
    code::Cost PCMPEQD_XMM_M128() const final { return {0.41500000000000004, 1, 9, 1, FADD | FMUL}; }
    code::Cost PCMPEQD_XMM_XMM() const final { return {0.33, 0, 1, 1, FADD | FMUL}; }
    code::Cost PCMPEQW_XMM_M128() const final { return {0.41500000000000004, 1, 9, 1, FADD | FMUL}; }
    code::Cost PCMPEQW_XMM_XMM() const final { return {0.33, 0, 1, 1, FADD | FMUL}; }
    code::Cost PCMPGTB_XMM_M128() const final { return {0.41500000000000004, 1, 9, 1, FADD | FMUL}; }
    code::Cost PCMPGTB_XMM_XMM() const final { return {0.33, 0, 1, 1, FADD | FMUL}; }
    code::Cost PCMPGTD_XMM_M128() const final { return {0.41500000000000004, 1, 9, 1, FADD | FMUL}; }
    code::Cost PCMPGTD_XMM_XMM() const final { return {0.33, 0, 1, 1, FADD | FMUL}; }
    code::Cost PCMPGTW_XMM_M128() const final { return {0.41500000000000004, 1, 9, 1, FADD | FMUL}; }
    code::Cost PCMPGTW_XMM_XMM() const final { return {0.33, 0, 1, 1, FADD | FMUL}; }

    code::Cost PEXTRW_R32_XMM_I8() const final { return {1.00, 6, 6, 2, FMISC | ALU}; }

    code::Cost PINSRW_XMM_M16_I8() const final { return {0.5, 1, 9, 1, FADD | FMUL}; }
    code::Cost PINSRW_XMM_R32_I8() const final { return {1.31, 1, 6, 2, FADD | FMUL}; }

    code::Cost PMADDWD_XMM_M128() const final { return {1.0, 3, 11, 1, FMUL}; }
    code::Cost PMADDWD_XMM_XMM() const final { return {1.0, 3, 3, 1, FMUL}; }

    code::Cost PMAXSW_XMM_M128() const final { return {0.41500000000000004, 1, 9, 1, FADD}; } // A guess
    code::Cost PMAXSW_XMM_XMM() const final { return {0.33, 1, 1, 1, FADD}; } // A guess
    code::Cost PMAXUB_XMM_M128() const final { return {0.41500000000000004, 1, 9, 1, FADD}; } // A guess
    code::Cost PMAXUB_XMM_XMM() const final { return {0.33, 1, 1, 1, FADD}; } // A guess
    code::Cost PMINSW_XMM_M128() const final { return {0.41500000000000004, 1, 9, 1, FADD}; } // A guess
    code::Cost PMINSW_XMM_XMM() const final { return {0.33, 1, 1, 1, FADD}; } // A guess
    code::Cost PMINUB_XMM_M128() const final { return {0.41500000000000004, 1, 9, 1, FADD}; } // A guess
    code::Cost PMINUB_XMM_XMM() const final { return {0.33, 1, 1, 1, FADD}; } // A guess

    code::Cost PMOVMSKB_R32_XMM() const final { return {1.0, 5, 5, 1, FADD}; }

    code::Cost PMULHUW_XMM_M128() const final { return {1.0, 3, 11, 1, FMUL}; }
    code::Cost PMULHUW_XMM_XMM() const final { return {1.0, 3, 3, 1, FMUL}; }
    code::Cost PMULHW_XMM_M128() const final { return {1.0, 3, 11, 1, FMUL}; }
    code::Cost PMULHW_XMM_XMM() const final { return {1.0, 3, 3, 1, FMUL}; }
    code::Cost PMULLW_XMM_M128() const final { return {1.0, 3, 11, 1, FMUL}; }
    code::Cost PMULLW_XMM_XMM() const final { return {1.0, 3, 3, 1, FMUL}; }
    code::Cost PMULUDQ_MM_M64() const final { return {1.0, 3, 11, 1, FMUL}; }
    code::Cost PMULUDQ_MM_MM() const final { return {1.0, 3, 3, 1, FMUL}; }
    code::Cost PMULUDQ_XMM_M128() const final { return {1.0, 3, 11, 1, FMUL}; }
    code::Cost PMULUDQ_XMM_XMM() const final { return {1.0, 3, 3, 1, FMUL}; }

    code::Cost POR_XMM_M128() const final { return {0.375, 1, 9, 1, FADD | FMUL}; }
    code::Cost POR_XMM_XMM() const final { return {0.25, 1, 1, 1, FADD | FMUL}; }

    code::Cost PSADBW_XMM_M128() const final { return {1.0, 3, 11, 1, FADD}; }
    code::Cost PSADBW_XMM_XMM() const final { return {1.0, 3, 3, 1, FADD}; }

    code::Cost PSHUFD_XMM_M128_I8() const final { return {0.5, 8, 9, 1, FADD | FMUL}; }
    code::Cost PSHUFD_XMM_XMM_I8() const final { return {0.5, 1, 1, 1, FADD | FMUL}; }
    code::Cost PSHUFHW_XMM_M128_I8() const final { return {0.5, 8, 9, 1, FADD | FMUL}; }
    code::Cost PSHUFHW_XMM_XMM_I8() const final { return {0.5, 1, 1, 1, FADD | FMUL}; }
    code::Cost PSHUFLW_XMM_M128_I8() const final { return {0.5, 8, 9, 1, FADD | FMUL}; }
    code::Cost PSHUFLW_XMM_XMM_I8() const final { return {0.5, 1, 1, 1, FADD | FMUL}; }

    code::Cost PSLLD_XMM_I8() const final { return {1.0, 1, 1, 1, FADD | FMUL}; }
    code::Cost PSLLD_XMM_M128() const final { return {1.0, 1, 9, 1, FADD | FMUL}; }
    code::Cost PSLLD_XMM_XMM() const final { return {1.0, 1, 1, 1, FADD | FMUL}; }
    code::Cost PSLLDQ_XMM_I8() const final { return {0.5, 1, 1, 1, FADD | FMUL}; }
    code::Cost PSLLQ_XMM_I8() const final { return {1.0, 1, 1, 1, FADD | FMUL}; }
    code::Cost PSLLQ_XMM_M128() const final { return {1.0, 1, 9, 1, FADD | FMUL}; }
    code::Cost PSLLQ_XMM_XMM() const final { return {1.0, 1, 1, 1, FADD | FMUL}; }
    code::Cost PSLLW_XMM_I8() const final { return {1.0, 1, 1, 1, FADD | FMUL}; }
    code::Cost PSLLW_XMM_M128() const final { return {1.0, 1, 9, 1, FADD | FMUL}; }
    code::Cost PSLLW_XMM_XMM() const final { return {1.0, 1, 1, 1, FADD | FMUL}; }

    code::Cost PSRAD_XMM_I8() const final { return {1.0, 1, 1, 1, FADD | FMUL}; }
    code::Cost PSRAD_XMM_M128() const final { return {1.0, 1, 9, 1, FADD | FMUL}; }
    code::Cost PSRAD_XMM_XMM() const final { return {1.0, 1, 1, 1, FADD | FMUL}; }
    code::Cost PSRAW_XMM_I8() const final { return {1.0, 1, 1, 1, FADD | FMUL}; }
    code::Cost PSRAW_XMM_M128() const final { return {1.0, 1, 9, 1, FADD | FMUL}; }
    code::Cost PSRAW_XMM_XMM() const final { return {1.0, 1, 1, 1, FADD | FMUL}; }

    code::Cost PSRLD_XMM_I8() const final { return {1.0, 1, 1, 1, FADD | FMUL}; }
    code::Cost PSRLD_XMM_M128() const final { return {1.0, 1, 9, 1, FADD | FMUL}; }
    code::Cost PSRLD_XMM_XMM() const final { return {1.0, 1, 1, 1, FADD | FMUL}; }
    code::Cost PSRLDQ_XMM_I8() const final { return {0.5, 1, 1, 1, FADD | FMUL}; }
    code::Cost PSRLQ_XMM_I8() const final { return {1.0, 1, 1, 1, FADD | FMUL}; }
    code::Cost PSRLQ_XMM_M128() const final { return {1.0, 1, 9, 1, FADD | FMUL}; }
    code::Cost PSRLQ_XMM_XMM() const final { return {1.0, 1, 1, 1, FADD | FMUL}; }
    code::Cost PSRLW_XMM_I8() const final { return {1.0, 1, 1, 1, FADD | FMUL}; }
    code::Cost PSRLW_XMM_M128() const final { return {1.0, 1, 9, 1, FADD | FMUL}; }
    code::Cost PSRLW_XMM_XMM() const final { return {1.0, 1, 1, 1, FADD | FMUL}; }

    code::Cost PSUBB_XMM_M128() const final { return {0.41500000000000004, 1, 9, 1, FADD | FMUL}; }
    code::Cost PSUBB_XMM_XMM() const final { return {0.33, 0, 1, 1, FADD | FMUL}; }
    code::Cost PSUBD_XMM_M128() const final { return {0.41500000000000004, 1, 9, 1, FADD | FMUL}; }
    code::Cost PSUBD_XMM_XMM() const final { return {0.33, 0, 1, 1, FADD | FMUL}; }
    code::Cost PSUBQ_MM_M64() const final { return {0.41500000000000004, 1, 9, 1, FADD | FMUL}; }
    code::Cost PSUBQ_MM_MM() const final { return {0.33999999999999997, 0, 1, 1, FADD | FMUL}; }
    code::Cost PSUBQ_XMM_M128() const final { return {0.41500000000000004, 1, 9, 1, FADD | FMUL}; }
    code::Cost PSUBQ_XMM_XMM() const final { return {0.33, 0, 1, 1, FADD | FMUL}; }
    code::Cost PSUBSB_XMM_M128() const final { return {0.5, 1, 9, 1, FADD | FMUL}; }
    code::Cost PSUBSB_XMM_XMM() const final { return {0.5, 0, 1, 1, FADD | FMUL}; }
    code::Cost PSUBSW_XMM_M128() const final { return {0.5, 1, 9, 1, FADD | FMUL}; }
    code::Cost PSUBSW_XMM_XMM() const final { return {0.5, 0, 1, 1, FADD | FMUL}; }
    code::Cost PSUBUSB_XMM_M128() const final { return {0.5, 1, 9, 1, FADD | FMUL}; }
    code::Cost PSUBUSB_XMM_XMM() const final { return {0.5, 0, 1, 1, FADD | FMUL}; }
    code::Cost PSUBUSW_XMM_M128() const final { return {0.5, 1, 9, 1, FADD | FMUL}; }
    code::Cost PSUBUSW_XMM_XMM() const final { return {0.5, 0, 1, 1, FADD | FMUL}; }
    code::Cost PSUBW_XMM_M128() const final { return {0.41500000000000004, 1, 9, 1, FADD | FMUL}; }
    code::Cost PSUBW_XMM_XMM() const final { return {0.33, 0, 1, 1, FADD | FMUL}; }

    code::Cost PUNPCKHBW_XMM_M128() const final { return {0.5, 1, 9, 1, FADD | FMUL}; }
    code::Cost PUNPCKHBW_XMM_XMM() const final { return {0.5, 1, 1, 1, FADD | FMUL}; }
    code::Cost PUNPCKHDQ_XMM_M128() const final { return {0.5, 1, 9, 1, FADD | FMUL}; }
    code::Cost PUNPCKHDQ_XMM_XMM() const final { return {0.5, 1, 1, 1, FADD | FMUL}; }
    code::Cost PUNPCKHQDQ_XMM_M128() const final { return {0.5, 1, 9, 1, FADD | FMUL}; }
    code::Cost PUNPCKHQDQ_XMM_XMM() const final { return {0.5, 1, 1, 1, FADD | FMUL}; }
    code::Cost PUNPCKHWD_XMM_M128() const final { return {0.5, 1, 9, 1, FADD | FMUL}; }
    code::Cost PUNPCKHWD_XMM_XMM() const final { return {0.5, 1, 1, 1, FADD | FMUL}; }
    code::Cost PUNPCKLBW_XMM_M128() const final { return {0.5, 1, 9, 1, FADD | FMUL}; }
    code::Cost PUNPCKLBW_XMM_XMM() const final { return {0.5, 1, 1, 1, FADD | FMUL}; }
    code::Cost PUNPCKLDQ_XMM_M128() const final { return {0.5, 1, 9, 1, FADD | FMUL}; }
    code::Cost PUNPCKLDQ_XMM_XMM() const final { return {0.5, 1, 1, 1, FADD | FMUL}; }
    code::Cost PUNPCKLQDQ_XMM_M128() const final { return {0.5, 1, 9, 1, FADD | FMUL}; }
    code::Cost PUNPCKLQDQ_XMM_XMM() const final { return {0.5, 1, 1, 1, FADD | FMUL}; }
    code::Cost PUNPCKLWD_XMM_M128() const final { return {0.5, 1, 9, 1, FADD | FMUL}; }
    code::Cost PUNPCKLWD_XMM_XMM() const final { return {0.5, 1, 1, 1, FADD | FMUL}; }

    code::Cost PXOR_XMM_M128() const final { return {0.375, 1, 9, 1, FADD | FMUL}; }
    code::Cost PXOR_XMM_XMM() const final { return {0.25, 0, 1, 1, FADD | FMUL}; }

    code::Cost SHUFPD_XMM_M128_I8() const final { return {0.5, 1, 9, 1, FMUL}; }
    code::Cost SHUFPD_XMM_XMM_I8() const final { return {0.5, 1, 1, 1, FMUL}; }

    code::Cost SQRTPD_XMM_M128() const final { return {2.5, 16.0, 28, 1, FMUL}; }
    code::Cost SQRTPD_XMM_XMM() const final { return {2.5, 8.0, 20, 1, FMUL}; }
    code::Cost SQRTSD_XMM_M64() const final { return {2.5, 8.0, 28, 1, FMUL}; }
    code::Cost SQRTSD_XMM_XMM() const final { return {2.5, 8, 20, 1, FMUL}; }

    code::Cost SUBPD_XMM_M128() const final { return {0.5, 3, 11, 1, FADD}; }
    code::Cost SUBPD_XMM_XMM() const final { return {0.5, 3, 3, 1, FADD}; }
    code::Cost SUBSD_XMM_M64() const final { return {0.5, 3, 11, 1, FADD}; }
    code::Cost SUBSD_XMM_XMM() const final { return {0.5, 3, 3, 1, FADD}; }

    code::Cost UCOMISD_XMM_M64() const final { return {1.00, 7, 12, 2, FADD}; }
    code::Cost UCOMISD_XMM_XMM() const final { return {1.00, 7, 7, 2, FADD}; }

    code::Cost UNPCKHPD_XMM_M128() const final { return {0.5, 1, 9, 1, FMUL}; }
    code::Cost UNPCKHPD_XMM_XMM() const final { return {0.5, 1, 1, 1, FMUL}; }
    code::Cost UNPCKLPD_XMM_M128() const final { return {0.5, 1, 9, 1, FMUL}; }
    code::Cost UNPCKLPD_XMM_XMM() const final { return {0.5, 1, 1, 1, FMUL}; }

    code::Cost XORPD_XMM_M128() const final { return {0.375, 1, 9, 1, FADD}; }
    code::Cost XORPD_XMM_XMM() const final { return {0.25, 0, 1, 1, FADD}; }

    code::Cost ADDSUBPD_XMM_M128() const final { return {0.5, 3, 11, 1, FADD}; } // Guessing
    code::Cost ADDSUBPD_XMM_XMM() const final { return {0.5, 3, 3, 1, FADD}; } // Guessing
    code::Cost ADDSUBPS_XMM_M128() const final { return {0.5, 3, 11, 1, FADD}; } // Guessing
    code::Cost ADDSUBPS_XMM_XMM() const final { return {0.5, 3, 3, 1, FADD}; } // Guessing

    code::Cost HADDPD_XMM_M128() const final { return {2.00, 6, 14, 4, FADD}; }
    code::Cost HADDPD_XMM_XMM() const final { return {2.00, 6, 7, 4, FADD}; }
    code::Cost HADDPS_XMM_M128() const final { return {2.00, 6, 14, 4, FADD}; }
    code::Cost HADDPS_XMM_XMM() const final { return {2.00, 6, 7, 4, FADD}; }
    code::Cost HSUBPD_XMM_M128() const final { return {2.00, 6, 14, 4, FADD}; }
    code::Cost HSUBPD_XMM_XMM() const final { return {2.00, 6, 7, 4, FADD}; }
    code::Cost HSUBPS_XMM_M128() const final { return {2.00, 6, 14, 4, FADD}; }
    code::Cost HSUBPS_XMM_XMM() const final { return {2.00, 6, 7, 4, FADD}; }

    code::Cost LDDQU_XMM_M128() const final { return {0.50, 7, 8, 1, UNKNOWN}; }

    code::Cost MOVDDUP_XMM_M64() const final { return {0.50, 7, 8, 1, FMISC}; }
    code::Cost MOVDDUP_XMM_XMM() const final { return {0.5, 1, 1, 1, FMISC}; }
    code::Cost MOVSHDUP_XMM_M128() const final { return {0.50, 7, 8, 1, FMISC}; }
    code::Cost MOVSHDUP_XMM_XMM() const final { return {0.5, 1, 1, 1, FMISC}; }
    code::Cost MOVSLDUP_XMM_M128() const final { return {0.50, 7, 8, 1, FMISC}; }
    code::Cost MOVSLDUP_XMM_XMM() const final { return {0.5, 1, 1, 1, FMISC}; }

    code::Cost BLENDPD_XMM_M128_I8() const final { return {0.5, 1, 9, 1, UNKNOWN}; }
    code::Cost BLENDPD_XMM_XMM_I8() const final { return {0.5, 1, 1, 1, UNKNOWN}; }
    code::Cost BLENDPS_XMM_M128_I8() const final { return {0.5, 1, 9, 1, UNKNOWN}; }
    code::Cost BLENDPS_XMM_XMM_I8() const final { return {0.5, 1, 1, 1, UNKNOWN}; }
    code::Cost BLENDVPD_XMM_M128() const final { return {0.5, 1, 9, 1, UNKNOWN}; }
    code::Cost BLENDVPD_XMM_XMM() const final { return {0.5, 1, 1, 1, UNKNOWN}; }
    code::Cost BLENDVPS_XMM_M128() const final { return {0.5, 1, 9, 1, UNKNOWN}; }
    code::Cost BLENDVPS_XMM_XMM() const final { return {0.5, 1, 1, 1, UNKNOWN}; }

    code::Cost CRC32_R32_M16() const final { return {3.00, 3, 9, 4, UNKNOWN}; }
    code::Cost CRC32_R32_M32() const final { return {3.00, 3, 9, 4, UNKNOWN}; }
    code::Cost CRC32_R32_M8() const final { return {3.00, 3, 9, 4, UNKNOWN}; }
    code::Cost CRC32_R32_R16() const final { return {3.00, 3, 3, 3, UNKNOWN}; }
    code::Cost CRC32_R32_R32() const final { return {3.00, 3, 3, 3, UNKNOWN}; }
    code::Cost CRC32_R32_R8() const final { return {3.00, 3, 3, 3, UNKNOWN}; }
    code::Cost CRC32_R32_R8() const final { return {3.00, 3, 3, 3, UNKNOWN}; }
    code::Cost CRC32_R64_M64() const final { return {3.00, 3, 9, 4, UNKNOWN}; }
    code::Cost CRC32_R64_M8() const final { return {3.00, 3, 9, 4, UNKNOWN}; }
    code::Cost CRC32_R64_R64() const final { return {3.00, 3, 3, 3, UNKNOWN}; }
    code::Cost CRC32_R64_R8() const final { return {3.00, 3, 3, 3, UNKNOWN}; }
    
    code::Cost DPPD_XMM_M128_I8() const final { return {4.00, 10, 18, 5, UNKNOWN}; }
    code::Cost DPPD_XMM_XMM_I8() const final { return {3.00, 10, 10, 3, UNKNOWN}; }
    code::Cost DPPS_XMM_M128_I8() const final { return {5.00, 15, 23, 10, UNKNOWN}; }
    code::Cost DPPS_XMM_XMM_I8() const final { return {4.00, 15, 15, 8, UNKNOWN}; }
    code::Cost EXTRACTPS_M32_XMM_I8() const final { return {1.00, 8, 8, 2, UNKNOWN}; }
    code::Cost EXTRACTPS_R32_XMM_I8() const final { return {1.00, 6, 6, 2, UNKNOWN}; }
    code::Cost INSERTPS_XMM_M32_I8() const final { return {0.5, 1, 9, 1, UNKNOWN}; }
    code::Cost INSERTPS_XMM_XMM_I8() const final { return {0.5, 1, 1, 1, UNKNOWN}; }
    code::Cost MOVNTDQA_XMM_M128() const final { return {0.50, 7, 8, 1, UNKNOWN}; }
    code::Cost MPSADBW_XMM_M128_I8() const final { return {3.00, 4, 12, 6, UNKNOWN}; }
    code::Cost MPSADBW_XMM_XMM_I8() const final { return {2.00, 4, 4, 4, UNKNOWN}; }
    code::Cost PACKUSDW_XMM_M128() const final { return {0.5, 1, 9, 1, UNKNOWN}; }
    code::Cost PACKUSDW_XMM_XMM() const final { return {0.5, 1, 1, 1, UNKNOWN}; }
    code::Cost PBLENDVB_XMM_M128() const final { return {1.0, 1, 9, 1, UNKNOWN}; }
    code::Cost PBLENDVB_XMM_XMM() const final { return {1.0, 1, 1, 1, UNKNOWN}; }
    code::Cost PBLENDW_XMM_M128_I8() const final { return {0.41500000000000004, 1, 9, 1, UNKNOWN}; }
    code::Cost PBLENDW_XMM_XMM_I8() const final { return {0.355, 1, 1, 1, UNKNOWN}; }
    code::Cost PCMPEQQ_XMM_M128() const final { return {0.5, 1, 9, 1, UNKNOWN}; }
    code::Cost PCMPEQQ_XMM_XMM() const final { return {0.5, 0, 1, 1, UNKNOWN}; }
    code::Cost PCMPESTRI_XMM_M128_I8() const final { return {4.00, 10, 25, 12, UNKNOWN}; }
    code::Cost PCMPESTRI_XMM_XMM_I8() const final { return {3.00, 10, 13, 6, UNKNOWN}; }
    code::Cost PCMPESTRI64_XMM_M128_I8() const final { return {7.00, 10, 26, 10, UNKNOWN}; }
    code::Cost PCMPESTRI64_XMM_XMM_I8() const final { return {6.00, 10, 13, 8, UNKNOWN}; }
    code::Cost PCMPESTRM_XMM_M128_I8() const final { return {4.00, 7, 15, 12, UNKNOWN}; }
    code::Cost PCMPESTRM_XMM_XMM_I8() const final { return {3.00, 7, 14, 7, UNKNOWN}; }
    code::Cost PCMPESTRM64_XMM_M128_I8() const final { return {4.00, 7, 15, 12, UNKNOWN}; }
    code::Cost PCMPESTRM64_XMM_XMM_I8() const final { return {3.00, 7, 14, 7, UNKNOWN}; }
    code::Cost PCMPGTQ_XMM_M128() const final { return {1.0, 1, 9, 1, UNKNOWN}; }
    code::Cost PCMPGTQ_XMM_XMM() const final { return {1.0, 0, 1, 1, UNKNOWN}; }
    code::Cost PCMPISTRI_XMM_M128_I8() const final { return {2.00, 10, 25, 3, UNKNOWN}; }
    code::Cost PCMPISTRI_XMM_XMM_I8() const final { return {2.00, 10, 10, 2, UNKNOWN}; }
    code::Cost PCMPISTRM_XMM_M128_I8() const final { return {2.00, 7, 15, 4, UNKNOWN}; }
    code::Cost PCMPISTRM_XMM_XMM_I8() const final { return {2.00, 7, 10, 3, UNKNOWN}; }
    code::Cost PEXTRB_M8_XMM_I8() const final { return {1.00, 9, 18, 2, UNKNOWN}; }
    code::Cost PEXTRB_R32_XMM_I8() const final { return {1.00, 6, 6, 2, UNKNOWN}; }
    code::Cost PEXTRD_M32_XMM_I8() const final { return {1.00, 8, 8, 2, UNKNOWN}; }
    code::Cost PEXTRD_R32_XMM_I8() const final { return {1.00, 6, 6, 2, UNKNOWN}; }
    code::Cost PEXTRQ_M64_XMM_I8() const final { return {1.00, 8, 8, 2, UNKNOWN}; }
    code::Cost PEXTRQ_R64_XMM_I8() const final { return {1.00, 6, 6, 2, UNKNOWN}; }
    code::Cost PEXTRW_SSE4_M16_XMM_I8() const final { return {1.00, 9, 18, 2, UNKNOWN}; }
    code::Cost PHMINPOSUW_XMM_M128() const final { return {1.25, 10, 11, 1, UNKNOWN}; }
    code::Cost PHMINPOSUW_XMM_XMM() const final { return {1.25, 3, 3, 1, UNKNOWN}; }
    code::Cost PINSRB_XMM_M8_I8() const final { return {0.5, 1, 9, 1, UNKNOWN}; }
    code::Cost PINSRB_XMM_R32_I8() const final { return {1.33, 1, 6, 2, UNKNOWN}; }
    code::Cost PINSRD_XMM_M32_I8() const final { return {0.5, 1, 9, 1, UNKNOWN}; }
    code::Cost PINSRD_XMM_R32_I8() const final { return {1.33, 1, 6, 2, UNKNOWN}; }
    code::Cost PINSRQ_XMM_M64_I8() const final { return {0.5, 1, 9, 1, UNKNOWN}; }
    code::Cost PINSRQ_XMM_R64_I8() const final { return {1.31, 1, 6, 2, UNKNOWN}; }
    code::Cost PMAXSB_XMM_M128() const final { return {0.41500000000000004, 1, 9, 1, UNKNOWN}; }
    code::Cost PMAXSB_XMM_XMM() const final { return {0.33, 1, 1, 1, UNKNOWN}; }
    code::Cost PMAXSD_XMM_M128() const final { return {0.41500000000000004, 1, 9, 1, UNKNOWN}; }
    code::Cost PMAXSD_XMM_XMM() const final { return {0.33, 1, 1, 1, UNKNOWN}; }
    code::Cost PMAXUD_XMM_M128() const final { return {0.41500000000000004, 1, 9, 1, UNKNOWN}; }
    code::Cost PMAXUD_XMM_XMM() const final { return {0.33, 1, 1, 1, UNKNOWN}; }
    code::Cost PMAXUW_XMM_M128() const final { return {0.41500000000000004, 1, 9, 1, UNKNOWN}; }
    code::Cost PMAXUW_XMM_XMM() const final { return {0.33, 1, 1, 1, UNKNOWN}; }
    code::Cost PMINSB_XMM_M128() const final { return {0.41500000000000004, 1, 9, 1, UNKNOWN}; }
    code::Cost PMINSB_XMM_XMM() const final { return {0.33, 1, 1, 1, UNKNOWN}; }
    code::Cost PMINSD_XMM_M128() const final { return {0.41500000000000004, 1, 9, 1, UNKNOWN}; }
    code::Cost PMINSD_XMM_XMM() const final { return {0.33, 1, 1, 1, UNKNOWN}; }
    code::Cost PMINUD_XMM_M128() const final { return {0.41500000000000004, 1, 9, 1, UNKNOWN}; }
    code::Cost PMINUD_XMM_XMM() const final { return {0.33, 1, 1, 1, UNKNOWN}; }
    code::Cost PMINUW_XMM_M128() const final { return {0.41500000000000004, 1, 9, 1, UNKNOWN}; }
    code::Cost PMINUW_XMM_XMM() const final { return {0.33, 1, 1, 1, UNKNOWN}; }
    code::Cost PMOVSXBD_XMM_M32() const final { return {0.5, 8, 9, 1, UNKNOWN}; }
    code::Cost PMOVSXBD_XMM_XMM() const final { return {0.5, 1, 1, 1, UNKNOWN}; }
    code::Cost PMOVSXBQ_XMM_M16() const final { return {0.5, 8, 9, 1, UNKNOWN}; }
    code::Cost PMOVSXBQ_XMM_XMM() const final { return {0.5, 1, 1, 1, UNKNOWN}; }
    code::Cost PMOVSXBW_XMM_M64() const final { return {0.5, 8, 9, 1, UNKNOWN}; }
    code::Cost PMOVSXBW_XMM_XMM() const final { return {0.5, 1, 1, 1, UNKNOWN}; }
    code::Cost PMOVSXDQ_XMM_M64() const final { return {0.5, 8, 9, 1, UNKNOWN}; }
    code::Cost PMOVSXDQ_XMM_XMM() const final { return {0.5, 1, 1, 1, UNKNOWN}; }
    code::Cost PMOVSXWD_XMM_M64() const final { return {0.5, 8, 9, 1, UNKNOWN}; }
    code::Cost PMOVSXWD_XMM_XMM() const final { return {0.5, 1, 1, 1, UNKNOWN}; }
    code::Cost PMOVSXWQ_XMM_M32() const final { return {0.5, 8, 9, 1, UNKNOWN}; }
    code::Cost PMOVSXWQ_XMM_XMM() const final { return {0.5, 1, 1, 1, UNKNOWN}; }
    code::Cost PMOVZXBD_XMM_M32() const final { return {0.5, 8, 9, 1, UNKNOWN}; }
    code::Cost PMOVZXBD_XMM_XMM() const final { return {0.5, 1, 1, 1, UNKNOWN}; }
    code::Cost PMOVZXBQ_XMM_M16() const final { return {0.5, 8, 9, 1, UNKNOWN}; }
    code::Cost PMOVZXBQ_XMM_XMM() const final { return {0.5, 1, 1, 1, UNKNOWN}; }
    code::Cost PMOVZXBW_XMM_M64() const final { return {0.5, 8, 9, 1, UNKNOWN}; }
    code::Cost PMOVZXBW_XMM_XMM() const final { return {0.5, 1, 1, 1, UNKNOWN}; }
    code::Cost PMOVZXDQ_XMM_M64() const final { return {0.5, 8, 9, 1, UNKNOWN}; }
    code::Cost PMOVZXDQ_XMM_XMM() const final { return {0.5, 1, 1, 1, UNKNOWN}; }
    code::Cost PMOVZXWD_XMM_M64() const final { return {0.5, 8, 9, 1, UNKNOWN}; }
    code::Cost PMOVZXWD_XMM_XMM() const final { return {0.5, 1, 1, 1, UNKNOWN}; }
    code::Cost PMOVZXWQ_XMM_M32() const final { return {0.5, 8, 9, 1, UNKNOWN}; }
    code::Cost PMOVZXWQ_XMM_XMM() const final { return {0.5, 1, 1, 1, UNKNOWN}; }
    code::Cost PMULDQ_XMM_M128() const final { return {1.0, 3, 11, 1, UNKNOWN}; }
    code::Cost PMULDQ_XMM_XMM() const final { return {1.0, 3, 3, 1, UNKNOWN}; }
    code::Cost PMULLD_XMM_M128() const final { return {1.5, 4, 12, 1, UNKNOWN}; }
    code::Cost PMULLD_XMM_XMM() const final { return {1.5, 4, 4, 1, UNKNOWN}; }
    code::Cost POPCNT_R16_M16() const final { return {0.50, 1, 7, 1, UNKNOWN}; }
    code::Cost POPCNT_R16_R16() const final { return {0.40, 1, 1, 1, UNKNOWN}; }
    code::Cost POPCNT_R32_M32() const final { return {0.50, 0, 7, 1, UNKNOWN}; }
    code::Cost POPCNT_R32_R32() const final { return {0.31, 0, 1, 1, UNKNOWN}; }
    code::Cost POPCNT_R64_M64() const final { return {0.50, 0, 7, 1, UNKNOWN}; }
    code::Cost POPCNT_R64_R64() const final { return {0.31, 0, 1, 1, UNKNOWN}; }
    code::Cost PTEST_XMM_M128() const final { return {1.00, 6, 10, 2, UNKNOWN}; }
    code::Cost PTEST_XMM_XMM() const final { return {1.0, 5, 5, 1, UNKNOWN}; }
    code::Cost ROUNDPD_XMM_M128_I8() const final { return {1.0, 11, 12, 1, UNKNOWN}; }
    code::Cost ROUNDPD_XMM_XMM_I8() const final { return {1.0, 4, 4, 1, UNKNOWN}; }
    code::Cost ROUNDPS_XMM_M128_I8() const final { return {1.0, 11, 12, 1, UNKNOWN}; }
    code::Cost ROUNDPS_XMM_XMM_I8() const final { return {1.0, 4, 4, 1, UNKNOWN}; }
    code::Cost ROUNDSD_XMM_M64_I8() const final { return {1.0, 0, 12, 1, UNKNOWN}; }
    code::Cost ROUNDSD_XMM_XMM_I8() const final { return {1.0, 0, 4, 1, UNKNOWN}; }
    code::Cost ROUNDSS_XMM_M32_I8() const final { return {1.0, 0, 12, 1, UNKNOWN}; }
    code::Cost ROUNDSS_XMM_XMM_I8() const final { return {1.0, 0, 4, 1, UNKNOWN}; }
    code::Cost PABSB_MM_M64() const final { return {0.5, 8, 9, 1, UNKNOWN}; }
    code::Cost PABSB_MM_MM() const final { return {0.5, 1, 1, 1, UNKNOWN}; }
    code::Cost PABSB_XMM_M128() const final { return {0.5, 8, 9, 1, UNKNOWN}; }
    code::Cost PABSB_XMM_XMM() const final { return {0.5, 1, 1, 1, UNKNOWN}; }
    code::Cost PABSD_MM_M64() const final { return {0.5, 8, 9, 1, UNKNOWN}; }
    code::Cost PABSD_MM_MM() const final { return {0.5, 1, 1, 1, UNKNOWN}; }
    code::Cost PABSD_XMM_M128() const final { return {0.5, 8, 9, 1, UNKNOWN}; }
    code::Cost PABSD_XMM_XMM() const final { return {0.5, 1, 1, 1, UNKNOWN}; }
    code::Cost PABSW_MM_M64() const final { return {0.5, 8, 9, 1, UNKNOWN}; }
    code::Cost PABSW_MM_MM() const final { return {0.5, 1, 1, 1, UNKNOWN}; }
    code::Cost PABSW_XMM_M128() const final { return {0.5, 8, 9, 1, UNKNOWN}; }
    code::Cost PABSW_XMM_XMM() const final { return {0.5, 1, 1, 1, UNKNOWN}; }
    code::Cost PALIGNR_MM_M64_I8() const final { return {0.5, 1, 9, 1, UNKNOWN}; }
    code::Cost PALIGNR_MM_MM_I8() const final { return {0.5, 1, 1, 1, UNKNOWN}; }
    code::Cost PALIGNR_XMM_M128_I8() const final { return {0.5, 1, 9, 1, UNKNOWN}; }
    code::Cost PALIGNR_XMM_XMM_I8() const final { return {0.5, 1, 1, 1, UNKNOWN}; }
    code::Cost PHADDD_MM_M64() const final { return {2.00, 2, 11, 4, UNKNOWN}; }
    code::Cost PHADDD_MM_MM() const final { return {2.00, 2, 3, 3, UNKNOWN}; }
    code::Cost PHADDD_XMM_M128() const final { return {2.00, 2, 11, 4, UNKNOWN}; }
    code::Cost PHADDD_XMM_XMM() const final { return {2.00, 2, 3, 4, UNKNOWN}; }
    code::Cost PHADDSW_MM_M64() const final { return {2.00, 2, 11, 4, UNKNOWN}; }
    code::Cost PHADDSW_MM_MM() const final { return {2.00, 2, 2, 3, UNKNOWN}; }
    code::Cost PHADDSW_XMM_M128() const final { return {2.00, 2, 11, 4, UNKNOWN}; }
    code::Cost PHADDSW_XMM_XMM() const final { return {2.00, 2, 3, 4, UNKNOWN}; }
    code::Cost PHADDW_MM_M64() const final { return {2.00, 2, 11, 4, UNKNOWN}; }
    code::Cost PHADDW_MM_MM() const final { return {2.00, 2, 3, 3, UNKNOWN}; }
    code::Cost PHADDW_XMM_M128() const final { return {2.00, 2, 11, 4, UNKNOWN}; }
    code::Cost PHADDW_XMM_XMM() const final { return {2.00, 2, 3, 4, UNKNOWN}; }
    code::Cost PHSUBD_MM_M64() const final { return {2.00, 2, 11, 4, UNKNOWN}; }
    code::Cost PHSUBD_MM_MM() const final { return {2.00, 2, 3, 3, UNKNOWN}; }
    code::Cost PHSUBD_XMM_M128() const final { return {2.00, 2, 11, 4, UNKNOWN}; }
    code::Cost PHSUBD_XMM_XMM() const final { return {2.00, 2, 3, 4, UNKNOWN}; }
    code::Cost PHSUBSW_MM_M64() const final { return {2.00, 2, 11, 4, UNKNOWN}; }
    code::Cost PHSUBSW_MM_MM() const final { return {2.00, 2, 2, 3, UNKNOWN}; }
    code::Cost PHSUBSW_XMM_M128() const final { return {2.00, 2, 11, 4, UNKNOWN}; }
    code::Cost PHSUBSW_XMM_XMM() const final { return {2.00, 2, 3, 4, UNKNOWN}; }
    code::Cost PHSUBW_MM_M64() const final { return {2.00, 2, 11, 4, UNKNOWN}; }
    code::Cost PHSUBW_MM_MM() const final { return {2.00, 2, 3, 3, UNKNOWN}; }
    code::Cost PHSUBW_XMM_M128() const final { return {2.00, 2, 11, 4, UNKNOWN}; }
    code::Cost PHSUBW_XMM_XMM() const final { return {2.00, 2, 3, 4, UNKNOWN}; }
    code::Cost PMADDUBSW_MM_M64() const final { return {1.0, 4, 12, 1, UNKNOWN}; }
    code::Cost PMADDUBSW_MM_MM() const final { return {1.0, 4, 4, 1, UNKNOWN}; }
    code::Cost PMADDUBSW_XMM_M128() const final { return {1.0, 4, 12, 1, UNKNOWN}; }
    code::Cost PMADDUBSW_XMM_XMM() const final { return {1.0, 4, 4, 1, UNKNOWN}; }
    code::Cost PMULHRSW_MM_M64() const final { return {1.0, 4, 12, 1, UNKNOWN}; }
    code::Cost PMULHRSW_MM_MM() const final { return {1.0, 4, 4, 1, UNKNOWN}; }
    code::Cost PMULHRSW_XMM_M128() const final { return {1.0, 4, 12, 1, UNKNOWN}; }
    code::Cost PMULHRSW_XMM_XMM() const final { return {1.0, 4, 4, 1, UNKNOWN}; }
    code::Cost PSHUFB_MM_M64() const final { return {0.5, 1, 9, 1, UNKNOWN}; }
    code::Cost PSHUFB_MM_MM() const final { return {0.5, 1, 1, 1, UNKNOWN}; }
    code::Cost PSHUFB_XMM_M128() const final { return {0.5, 1, 9, 1, UNKNOWN}; }
    code::Cost PSHUFB_XMM_XMM() const final { return {0.5, 1, 1, 1, UNKNOWN}; }
    code::Cost PSIGNB_MM_M64() const final { return {0.5, 1, 9, 1, UNKNOWN}; }
    code::Cost PSIGNB_MM_MM() const final { return {0.5, 1, 1, 1, UNKNOWN}; }
    code::Cost PSIGNB_XMM_M128() const final { return {0.5, 1, 9, 1, UNKNOWN}; }
    code::Cost PSIGNB_XMM_XMM() const final { return {0.5, 1, 1, 1, UNKNOWN}; }
    code::Cost PSIGND_MM_M64() const final { return {0.5, 1, 9, 1, UNKNOWN}; }
    code::Cost PSIGND_MM_MM() const final { return {0.5, 1, 1, 1, UNKNOWN}; }
    code::Cost PSIGND_XMM_M128() const final { return {0.5, 1, 9, 1, UNKNOWN}; }
    code::Cost PSIGND_XMM_XMM() const final { return {0.5, 1, 1, 1, UNKNOWN}; }
    code::Cost PSIGNW_MM_M64() const final { return {0.5, 1, 9, 1, UNKNOWN}; }
    code::Cost PSIGNW_MM_MM() const final { return {0.5, 1, 1, 1, UNKNOWN}; }
    code::Cost PSIGNW_XMM_M128() const final { return {0.5, 1, 9, 1, UNKNOWN}; }
    code::Cost PSIGNW_XMM_XMM() const final { return {0.5, 1, 1, 1, UNKNOWN}; }
  };
};