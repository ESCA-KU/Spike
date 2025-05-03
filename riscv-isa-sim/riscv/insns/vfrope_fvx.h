// vfrope.fvx vd, vs2, rs1

VI_CHECK_SSS(false); \
VI_VFP_COMMON \

uint64_t m          = READ_REG(31);\

for (reg_t i = P.VU.vstart->read(); i < vl; i = i + 2) { \
  VI_LOOP_ELEMENT_SKIP();
  switch (P.VU.vsew) { \
    case e16: { \
      break; \
    } \

    case e32: { \
      float32_t theta     = f32(READ_FREG(30)); \
      
      float32_t x_1 = P.VU.elt<float32_t>(rs2_num, i);
      float32_t x_2 = P.VU.elt<float32_t>(rs2_num, i+1);

      float32_t &y_1 = P.VU.elt<float32_t>(rd_num, i, true); \
      float32_t &y_2 = P.VU.elt<float32_t>(rd_num, i+1, true); \

      uint64_t base_index = READ_REG(rs1_num);\
      uint64_t resMode    = READ_REG(30);\

      f32_rope(x_1, x_2, &y_1, &y_2, theta, m, i, base_index);\
      
      //f32_ropeV2(x_1, x_2, &y_1, &y_2, resMode, m, i, base_index);\

      set_fp_exceptions; \
      break; \
    } \

    case e64: { \
      break; \
    } \
    default: \
      require(0); \
      break; \
  }; \
  DEBUG_RVV_FP_VF; \
VI_VFP_LOOP_END