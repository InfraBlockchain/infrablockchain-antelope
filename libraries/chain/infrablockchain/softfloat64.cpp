/**
 *  @file infrablockchain/softfloat64.cpp
 *  @author bezalel@infrablockchain.com
 *  @copyright defined in infrablockchain/LICENSE.txt
 */

#include <cstdlib>
#include <infrablockchain/chain/softfloat64.hpp>

#define _expF64UI( a ) ((int_fast16_t) ((a)>>52) & 0x7FF)
#define _signF64UI( a ) (((uint64_t) (a)>>63) != 0)
#define _packToF64UI( sign, exp, sig ) ((uint64_t) (((uint_fast64_t) (sign)<<63) + ((uint_fast64_t) (exp)<<52) + (sig)))

#define EXPTAB_SCALE 6
#define EXPTAB_MASK  ((1 << EXPTAB_SCALE) - 1)

#define LOGTAB_SCALE 8

/// implementing 64bit floating point 'power' function using Berkeley soft-float library

namespace infrablockchain { namespace chain {

      // .9670371139572337719125840413672004409288e-2
      static const float64_t EXPPOLY_32F_A0 = softfloat64::fromRaw(0x3f83ce0f3e46f431);

      // 1 / ln(2) * (1 << EXPTAB_SCALE) == 1.4426950408889634073599246810019 * (1 << EXPTAB_SCALE)
      static const float64_t exp_prescale = f64_mul( softfloat64::fromRaw(0x3ff71547652b82fe), ui32_to_f64(1 << EXPTAB_SCALE) );
      static const float64_t exp_postscale = f64_div( softfloat64::one(), ui32_to_f64(1 << EXPTAB_SCALE) );
      static const float64_t exp_max_val = ui32_to_f64(3000*(1 << EXPTAB_SCALE)); // log10(DBL_MAX) < 3000

      // 0.69314718055994530941723212145818
      static const float64_t ln_2 = softfloat64::fromRaw(0x3fe62e42fefa39ef);

      static const uint64_t expTab[] = {
         0x3ff0000000000000, // 1.000000
         0x3ff02c9a3e778061, // 1.010889
         0x3ff059b0d3158574, // 1.021897
         0x3ff0874518759bc8, // 1.033025
         0x3ff0b5586cf9890f, // 1.044274
         0x3ff0e3ec32d3d1a2, // 1.055645
         0x3ff11301d0125b51, // 1.067140
         0x3ff1429aaea92de0, // 1.078761
         0x3ff172b83c7d517b, // 1.090508
         0x3ff1a35beb6fcb75, // 1.102383
         0x3ff1d4873168b9aa, // 1.114387
         0x3ff2063b88628cd6, // 1.126522
         0x3ff2387a6e756238, // 1.138789
         0x3ff26b4565e27cdd, // 1.151189
         0x3ff29e9df51fdee1, // 1.163725
         0x3ff2d285a6e4030b, // 1.176397
         0x3ff306fe0a31b715, // 1.189207
         0x3ff33c08b26416ff, // 1.202157
         0x3ff371a7373aa9cb, // 1.215247
         0x3ff3a7db34e59ff7, // 1.228481
         0x3ff3dea64c123422, // 1.241858
         0x3ff4160a21f72e2a, // 1.255381
         0x3ff44e086061892d, // 1.269051
         0x3ff486a2b5c13cd0, // 1.282870
         0x3ff4bfdad5362a27, // 1.296840
         0x3ff4f9b2769d2ca7, // 1.310961
         0x3ff5342b569d4f82, // 1.325237
         0x3ff56f4736b527da, // 1.339668
         0x3ff5ab07dd485429, // 1.354256
         0x3ff5e76f15ad2148, // 1.369002
         0x3ff6247eb03a5585, // 1.383910
         0x3ff6623882552225, // 1.398980
         0x3ff6a09e667f3bcd, // 1.414214
         0x3ff6dfb23c651a2f, // 1.429613
         0x3ff71f75e8ec5f74, // 1.445181
         0x3ff75feb564267c9, // 1.460918
         0x3ff7a11473eb0187, // 1.476826
         0x3ff7e2f336cf4e62, // 1.492908
         0x3ff82589994cce13, // 1.509164
         0x3ff868d99b4492ed, // 1.525598
         0x3ff8ace5422aa0db, // 1.542211
         0x3ff8f1ae99157736, // 1.559004
         0x3ff93737b0cdc5e5, // 1.575981
         0x3ff97d829fde4e50, // 1.593142
         0x3ff9c49182a3f090, // 1.610490
         0x3ffa0c667b5de565, // 1.628027
         0x3ffa5503b23e255d, // 1.645755
         0x3ffa9e6b5579fdbf, // 1.663677
         0x3ffae89f995ad3ad, // 1.681793
         0x3ffb33a2b84f15fb, // 1.700106
         0x3ffb7f76f2fb5e47, // 1.718619
         0x3ffbcc1e904bc1d2, // 1.737334
         0x3ffc199bdd85529c, // 1.756252
         0x3ffc67f12e57d14b, // 1.775376
         0x3ffcb720dcef9069, // 1.794709
         0x3ffd072d4a07897c, // 1.814252
         0x3ffd5818dcfba487, // 1.834008
         0x3ffda9e603db3285, // 1.853979
         0x3ffdfc97337b9b5f, // 1.874168
         0x3ffe502ee78b3ff6, // 1.894576
         0x3ffea4afa2a490da, // 1.915207
         0x3ffefa1bee615a27, // 1.936062
         0x3fff50765b6e4540, // 1.957144
         0x3fffa7c1819e90d8, // 1.978456
      };

      static const uint64_t __attribute__ ((aligned (16))) icvLogTab[] = {
         0, 0x3ff0000000000000, // 0.000000, 1.000000
         0x3f6ff00aa2b10bc0, 0x3fefe01fe01fe020, // 0.003899, 0.996109
         0x3f7fe02a6b106788, 0x3fefc07f01fc07f0, // 0.007782, 0.992248
         0x3f87dc475f810a76, 0x3fefa11caa01fa12, // 0.011651, 0.988417
         0x3f8fc0a8b0fc03e3, 0x3fef81f81f81f820, // 0.015504, 0.984615
         0x3f93cea44346a574, 0x3fef6310aca0dbb5, // 0.019343, 0.980843
         0x3f97b91b07d5b11a, 0x3fef44659e4a4271, // 0.023167, 0.977099
         0x3f9b9fc027af9197, 0x3fef25f644230ab5, // 0.026977, 0.973384
         0x3f9f829b0e783300, 0x3fef07c1f07c1f08, // 0.030772, 0.969697
         0x3fa1b0d98923d97f, 0x3feee9c7f8458e02, // 0.034552, 0.966038
         0x3fa39e87b9febd5f, 0x3feecc07b301ecc0, // 0.038319, 0.962406
         0x3fa58a5bafc8e4d4, 0x3feeae807aba01eb, // 0.042071, 0.958801
         0x3fa77458f632dcfc, 0x3fee9131abf0b767, // 0.045810, 0.955224
         0x3fa95c830ec8e3eb, 0x3fee741aa59750e4, // 0.049534, 0.951673
         0x3fab42dd711971be, 0x3fee573ac901e574, // 0.053245, 0.948148
         0x3fad276b8adb0b52, 0x3fee3a9179dc1a73, // 0.056941, 0.944649
         0x3faf0a30c01162a6, 0x3fee1e1e1e1e1e1e, // 0.060625, 0.941176
         0x3fb075983598e471, 0x3fee01e01e01e01e, // 0.064294, 0.937729
         0x3fb16536eea37ae0, 0x3fede5d6e3f8868a, // 0.067951, 0.934307
         0x3fb253f62f0a1416, 0x3fedca01dca01dca, // 0.071594, 0.930909
         0x3fb341d7961bd1d0, 0x3fedae6076b981db, // 0.075223, 0.927536
         0x3fb42edcbea646f0, 0x3fed92f2231e7f8a, // 0.078840, 0.924188
         0x3fb51b073f06183f, 0x3fed77b654b82c34, // 0.082444, 0.920863
         0x3fb60658a93750c3, 0x3fed5cac807572b2, // 0.086034, 0.917563
         0x3fb6f0d28ae56b4b, 0x3fed41d41d41d41d, // 0.089612, 0.914286
         0x3fb7da766d7b12cc, 0x3fed272ca3fc5b1a, // 0.093177, 0.911032
         0x3fb8c345d6319b20, 0x3fed0cb58f6ec074, // 0.096730, 0.907801
         0x3fb9ab42462033ac, 0x3fecf26e5c44bfc6, // 0.100269, 0.904594
         0x3fba926d3a4ad563, 0x3fecd85689039b0b, // 0.103797, 0.901408
         0x3fbb78c82bb0eda1, 0x3fecbe6d9601cbe7, // 0.107312, 0.898246
         0x3fbc5e548f5bc743, 0x3feca4b3055ee191, // 0.110814, 0.895105
         0x3fbd4313d66cb35d, 0x3fec8b265afb8a42, // 0.114305, 0.891986
         0x3fbe27076e2af2e5, 0x3fec71c71c71c71c, // 0.117783, 0.888889
         0x3fbf0a30c01162a6, 0x3fec5894d10d4986, // 0.121249, 0.885813
         0x3fbfec9131dbeaba, 0x3fec3f8f01c3f8f0, // 0.124703, 0.882759
         0x3fc0671512ca596e, 0x3fec26b5392ea01c, // 0.128146, 0.879725
         0x3fc0d77e7cd08e59, 0x3fec0e070381c0e0, // 0.131576, 0.876712
         0x3fc14785846742ac, 0x3febf583ee868d8b, // 0.134995, 0.873720
         0x3fc1b72ad52f67a0, 0x3febdd2b899406f7, // 0.138402, 0.870748
         0x3fc2266f190a5acb, 0x3febc4fd65883e7b, // 0.141798, 0.867797
         0x3fc29552f81ff523, 0x3febacf914c1bad0, // 0.145182, 0.864865
         0x3fc303d718e47fd2, 0x3feb951e2b18ff23, // 0.148555, 0.861953
         0x3fc371fc201e8f74, 0x3feb7d6c3dda338b, // 0.151916, 0.859060
         0x3fc3dfc2b0ecc629, 0x3feb65e2e3beee05, // 0.155266, 0.856187
         0x3fc44d2b6ccb7d1e, 0x3feb4e81b4e81b4f, // 0.158605, 0.853333
         0x3fc4ba36f39a55e5, 0x3feb37484ad806ce, // 0.161933, 0.850498
         0x3fc526e5e3a1b437, 0x3feb2036406c80d9, // 0.165250, 0.847682
         0x3fc59338d9982085, 0x3feb094b31d922a4, // 0.168555, 0.844884
         0x3fc5ff3070a793d3, 0x3feaf286bca1af28, // 0.171850, 0.842105
         0x3fc66acd4272ad50, 0x3feadbe87f94905e, // 0.175134, 0.839344
         0x3fc6d60fe719d21c, 0x3feac5701ac5701b, // 0.178408, 0.836601
         0x3fc740f8f54037a4, 0x3feaaf1d2f87ebfd, // 0.181670, 0.833876
         0x3fc7ab890210d909, 0x3fea98ef606a63be, // 0.184922, 0.831169
         0x3fc815c0a14357ea, 0x3fea82e65130e159, // 0.188164, 0.828479
         0x3fc87fa06520c910, 0x3fea6d01a6d01a6d, // 0.191395, 0.825806
         0x3fc8e928de886d40, 0x3fea574107688a4a, // 0.194615, 0.823151
         0x3fc9525a9cf456b4, 0x3fea41a41a41a41a, // 0.197826, 0.820513
         0x3fc9bb362e7dfb83, 0x3fea2c2a87c51ca0, // 0.201026, 0.817891
         0x3fca23bc1fe2b563, 0x3fea16d3f97a4b02, // 0.204216, 0.815287
         0x3fca8becfc882f18, 0x3fea01a01a01a01a, // 0.207395, 0.812698
         0x3fcaf3c94e80bff2, 0x3fe9ec8e951033d9, // 0.210565, 0.810127
         0x3fcb5b519e8fb5a4, 0x3fe9d79f176b682d, // 0.213724, 0.807571
         0x3fcbc286742d8cd6, 0x3fe9c2d14ee4a102, // 0.216874, 0.805031
         0x3fcc2968558c18c0, 0x3fe9ae24ea5510da, // 0.220014, 0.802508
         0x3fcc8ff7c79a9a21, 0x3fe999999999999a, // 0.223144, 0.800000
         0x3fccf6354e09c5dc, 0x3fe9852f0d8ec0ff, // 0.226264, 0.797508
         0x3fcd5c216b4fbb91, 0x3fe970e4f80cb872, // 0.229374, 0.795031
         0x3fcdc1bca0abec7d, 0x3fe95cbb0be377ae, // 0.232475, 0.792570
         0x3fce27076e2af2e5, 0x3fe948b0fcd6e9e0, // 0.235566, 0.790123
         0x3fce8c0252aa5a5f, 0x3fe934c67f9b2ce6, // 0.238648, 0.787692
         0x3fcef0adcbdc5936, 0x3fe920fb49d0e229, // 0.241720, 0.785276
         0x3fcf550a564b7b37, 0x3fe90d4f120190d5, // 0.244783, 0.782875
         0x3fcfb9186d5e3e2a, 0x3fe8f9c18f9c18fa, // 0.247836, 0.780488
         0x3fd00e6c45ad501c, 0x3fe8e6527af1373f, // 0.250880, 0.778116
         0x3fd0402594b4d040, 0x3fe8d3018d3018d3, // 0.253915, 0.775758
         0x3fd071b85fcd590d, 0x3fe8bfce8062ff3a, // 0.256941, 0.773414
         0x3fd0a324e27390e3, 0x3fe8acb90f6bf3aa, // 0.259958, 0.771084
         0x3fd0d46b579ab74b, 0x3fe899c0f601899c, // 0.262965, 0.768769
         0x3fd1058bf9ae4ad5, 0x3fe886e5f0abb04a, // 0.265964, 0.766467
         0x3fd136870293a8b0, 0x3fe87427bcc092b9, // 0.268953, 0.764179
         0x3fd1675cababa60e, 0x3fe8618618618618, // 0.271934, 0.761905
         0x3fd1980d2dd4236f, 0x3fe84f00c2780614, // 0.274905, 0.759644
         0x3fd1c898c16999fa, 0x3fe83c977ab2bedd, // 0.277868, 0.757396
         0x3fd1f8ff9e48a2f2, 0x3fe82a4a0182a4a0, // 0.280823, 0.755162
         0x3fd22941fbcf7965, 0x3fe8181818181818, // 0.283768, 0.752941
         0x3fd2596010df7639, 0x3fe8060180601806, // 0.286705, 0.750733
         0x3fd2895a13de86a3, 0x3fe7f405fd017f40, // 0.289633, 0.748538
         0x3fd2b9303ab89d24, 0x3fe7e225515a4f1d, // 0.292553, 0.746356
         0x3fd2e8e2bae11d30, 0x3fe7d05f417d05f4, // 0.295464, 0.744186
         0x3fd31871c9544184, 0x3fe7beb3922e017c, // 0.298367, 0.742029
         0x3fd347dd9a987d54, 0x3fe7ad2208e0ecc3, // 0.301261, 0.739884
         0x3fd3772662bfd85a, 0x3fe79baa6bb6398b, // 0.304147, 0.737752
         0x3fd3a64c556945e9, 0x3fe78a4c8178a4c8, // 0.307025, 0.735632
         0x3fd3d54fa5c1f70f, 0x3fe77908119ac60d, // 0.309894, 0.733524
         0x3fd404308686a7e3, 0x3fe767dce434a9b1, // 0.312756, 0.731429
         0x3fd432ef2a04e813, 0x3fe756cac201756d, // 0.315609, 0.729345
         0x3fd4618bc21c5ec2, 0x3fe745d1745d1746, // 0.318454, 0.727273
         0x3fd49006804009d0, 0x3fe734f0c541fe8d, // 0.321291, 0.725212
         0x3fd4be5f957778a0, 0x3fe724287f46debc, // 0.324119, 0.723164
         0x3fd4ec9732600269, 0x3fe713786d9c7c09, // 0.326940, 0.721127
         0x3fd51aad872df82d, 0x3fe702e05c0b8170, // 0.329753, 0.719101
         0x3fd548a2c3add262, 0x3fe6f26016f26017, // 0.332558, 0.717087
         0x3fd5767717455a6c, 0x3fe6e1f76b4337c7, // 0.335356, 0.715084
         0x3fd5a42ab0f4cfe1, 0x3fe6d1a62681c861, // 0.338145, 0.713092
         0x3fd5d1bdbf5809ca, 0x3fe6c16c16c16c17, // 0.340927, 0.711111
         0x3fd5ff3070a793d3, 0x3fe6b1490aa31a3d, // 0.343701, 0.709141
         0x3fd62c82f2b9c795, 0x3fe6a13cd1537290, // 0.346467, 0.707182
         0x3fd659b57303e1f2, 0x3fe691473a88d0c0, // 0.349225, 0.705234
         0x3fd686c81e9b14ae, 0x3fe6816816816817, // 0.351976, 0.703297
         0x3fd6b3bb2235943d, 0x3fe6719f3601671a, // 0.354720, 0.701370
         0x3fd6e08eaa2ba1e3, 0x3fe661ec6a5122f9, // 0.357456, 0.699454
         0x3fd70d42e2789235, 0x3fe6524f853b4aa3, // 0.360184, 0.697548
         0x3fd739d7f6bbd006, 0x3fe642c8590b2164, // 0.362905, 0.695652
         0x3fd7664e1239dbce, 0x3fe63356b88ac0de, // 0.365619, 0.693767
         0x3fd792a55fdd47a2, 0x3fe623fa77016240, // 0.368326, 0.691892
         0x3fd7bede0a37afbf, 0x3fe614b36831ae94, // 0.371025, 0.690027
         0x3fd7eaf83b82afc3, 0x3fe6058160581606, // 0.373716, 0.688172
         0x3fd816f41da0d495, 0x3fe5f66434292dfc, // 0.376401, 0.686327
         0x3fd842d1da1e8b17, 0x3fe5e75bb8d015e7, // 0.379078, 0.684492
         0x3fd86e919a330ba0, 0x3fe5d867c3ece2a5, // 0.381749, 0.682667
         0x3fd89a3386c1425a, 0x3fe5c9882b931057, // 0.384412, 0.680851
         0x3fd8c5b7c858b48a, 0x3fe5babcc647fa91, // 0.387068, 0.679045
         0x3fd8f11e873662c7, 0x3fe5ac056b015ac0, // 0.389717, 0.677249
         0x3fd91c67eb45a83d, 0x3fe59d61f123ccaa, // 0.392359, 0.675462
         0x3fd947941c2116fa, 0x3fe58ed2308158ed, // 0.394994, 0.673684
         0x3fd972a341135158, 0x3fe5805601580560, // 0.397622, 0.671916
         0x3fd99d958117e08a, 0x3fe571ed3c506b3a, // 0.400243, 0.670157
         0x3fd9c86b02dc0862, 0x3fe56397ba7c52e2, // 0.402858, 0.668407
         0x3fd9f323ecbf984b, 0x3fe5555555555555, // 0.405465, 0.666667
         0x3fda1dc064d5b995, 0x3fe54725e6bb82fe, // 0.408066, 0.664935
         0x3fda484090e5bb0a, 0x3fe5390948f40feb, // 0.410660, 0.663212
         0x3fda72a4966bd9ea, 0x3fe52aff56a8054b, // 0.413247, 0.661499
         0x3fda9cec9a9a0849, 0x3fe51d07eae2f815, // 0.415828, 0.659794
         0x3fdac718c258b0e4, 0x3fe50f22e111c4c5, // 0.418402, 0.658098
         0x3fdaf1293247786b, 0x3fe5015015015015, // 0.420969, 0.656410
         0x3fdb1b1e0ebdfc5b, 0x3fe4f38f62dd4c9b, // 0.423530, 0.654731
         0x3fdb44f77bcc8f62, 0x3fe4e5e0a72f0539, // 0.426084, 0.653061
         0x3fdb6eb59d3cf35d, 0x3fe4d843bedc2c4c, // 0.428632, 0.651399
         0x3fdb9858969310fb, 0x3fe4cab88725af6e, // 0.431173, 0.649746
         0x3fdbc1e08b0dad0a, 0x3fe4bd3edda68fe1, // 0.433708, 0.648101
         0x3fdbeb4d9da71b7b, 0x3fe4afd6a052bf5b, // 0.436237, 0.646465
         0x3fdc149ff115f026, 0x3fe4a27fad76014a, // 0.438759, 0.644836
         0x3fdc3dd7a7cdad4d, 0x3fe49539e3b2d067, // 0.441275, 0.643216
         0x3fdc66f4e3ff6ff7, 0x3fe4880522014880, // 0.443784, 0.641604
         0x3fdc8ff7c79a9a21, 0x3fe47ae147ae147b, // 0.446287, 0.640000
         0x3fdcb8e0744d7ac9, 0x3fe46dce34596066, // 0.448784, 0.638404
         0x3fdce1af0b85f3eb, 0x3fe460cbc7f5cf9a, // 0.451275, 0.636816
         0x3fdd0a63ae721e64, 0x3fe453d9e2c776ca, // 0.453759, 0.635236
         0x3fdd32fe7e00ebd5, 0x3fe446f86562d9fb, // 0.456237, 0.633663
         0x3fdd5b7f9ae2c683, 0x3fe43a2730abee4d, // 0.458710, 0.632099
         0x3fdd83e7258a2f3e, 0x3fe42d6625d51f87, // 0.461176, 0.630542
         0x3fddac353e2c5954, 0x3fe420b5265e5951, // 0.463636, 0.628993
         0x3fddd46a04c1c4a0, 0x3fe4141414141414, // 0.466090, 0.627451
         0x3fddfc859906d5b5, 0x3fe40782d10e6566, // 0.468538, 0.625917
         0x3fde24881a7c6c26, 0x3fe3fb013fb013fb, // 0.470980, 0.624390
         0x3fde4c71a8687704, 0x3fe3ee8f42a5af07, // 0.473416, 0.622871
         0x3fde744261d68787, 0x3fe3e22cbce4a902, // 0.475846, 0.621359
         0x3fde9bfa659861f5, 0x3fe3d5d991aa75c6, // 0.478270, 0.619855
         0x3fdec399d2468cc0, 0x3fe3c995a47babe7, // 0.480689, 0.618357
         0x3fdeeb20c640ddf4, 0x3fe3bd60d9232955, // 0.483101, 0.616867
         0x3fdf128f5faf06ec, 0x3fe3b13b13b13b14, // 0.485508, 0.615385
         0x3fdf39e5bc811e5b, 0x3fe3a524387ac822, // 0.487909, 0.613909
         0x3fdf6123fa7028ac, 0x3fe3991c2c187f63, // 0.490304, 0.612440
         0x3fdf884a36fe9ec2, 0x3fe38d22d366088e, // 0.492693, 0.610979
         0x3fdfaf588f78f31e, 0x3fe3813813813814, // 0.495077, 0.609524
         0x3fdfd64f20f61571, 0x3fe3755bd1c945ee, // 0.497455, 0.608076
         0x3fdffd2e0857f498, 0x3fe3698df3de0748, // 0.499828, 0.606635
         0x3fe011fab125ff8a, 0x3fe35dce5f9f2af8, // 0.502195, 0.605201
         0x3fe02552a5a5d0fe, 0x3fe3521cfb2b78c1, // 0.504556, 0.603774
         0x3fe0389eefce633b, 0x3fe34679ace01346, // 0.506912, 0.602353
         0x3fe04bdf9da926d2, 0x3fe33ae45b57bcb2, // 0.509262, 0.600939
         0x3fe05f14bd26459c, 0x3fe32f5ced6a1dfa, // 0.511607, 0.599532
         0x3fe0723e5c1cdf40, 0x3fe323e34a2b10bf, // 0.513946, 0.598131
         0x3fe0855c884b450e, 0x3fe3187758e9ebb6, // 0.516279, 0.596737
         0x3fe0986f4f573520, 0x3fe30d190130d190, // 0.518608, 0.595349
         0x3fe0ab76bece14d1, 0x3fe301c82ac40260, // 0.520931, 0.593968
         0x3fe0be72e4252a82, 0x3fe2f684bda12f68, // 0.523248, 0.592593
         0x3fe0d163ccb9d6b7, 0x3fe2eb4ea1fed14b, // 0.525560, 0.591224
         0x3fe0e44985d1cc8b, 0x3fe2e025c04b8097, // 0.527867, 0.589862
         0x3fe0f7241c9b497d, 0x3fe2d50a012d50a0, // 0.530169, 0.588506
         0x3fe109f39e2d4c96, 0x3fe2c9fb4d812ca0, // 0.532465, 0.587156
         0x3fe11cb81787ccf8, 0x3fe2bef98e5a3711, // 0.534756, 0.585812
         0x3fe12f719593efbc, 0x3fe2b404ad012b40, // 0.537041, 0.584475
         0x3fe1422025243d44, 0x3fe2a91c92f3c105, // 0.539322, 0.583144
         0x3fe154c3d2f4d5e9, 0x3fe29e4129e4129e, // 0.541597, 0.581818
         0x3fe1675cababa60e, 0x3fe293725bb804a5, // 0.543867, 0.580499
         0x3fe179eabbd899a0, 0x3fe288b01288b013, // 0.546132, 0.579186
         0x3fe18c6e0ff5cf06, 0x3fe27dfa38a1ce4d, // 0.548392, 0.577878
         0x3fe19ee6b467c96e, 0x3fe27350b8812735, // 0.550647, 0.576577
         0x3fe1b154b57da29e, 0x3fe268b37cd60127, // 0.552897, 0.575281
         0x3fe1c3b81f713c24, 0x3fe25e22708092f1, // 0.555142, 0.573991
         0x3fe1d610fe677003, 0x3fe2539d7e9177b2, // 0.557381, 0.572707
         0x3fe1e85f5e7040d0, 0x3fe2492492492492, // 0.559616, 0.571429
         0x3fe1faa34b87094c, 0x3fe23eb79717605b, // 0.561845, 0.570156
         0x3fe20cdcd192ab6d, 0x3fe23456789abcdf, // 0.564070, 0.568889
         0x3fe21f0bfc65beeb, 0x3fe22a0122a0122a, // 0.566290, 0.567627
         0x3fe23130d7bebf42, 0x3fe21fb78121fb78, // 0.568505, 0.566372
         0x3fe2434b6f483933, 0x3fe21579804855e6, // 0.570715, 0.565121
         0x3fe2555bce98f7cb, 0x3fe20b470c67c0d9, // 0.572920, 0.563877
         0x3fe26762013430df, 0x3fe2012012012012, // 0.575120, 0.562637
         0x3fe2795e1289b11a, 0x3fe1f7047dc11f70, // 0.577315, 0.561404
         0x3fe28b500df60782, 0x3fe1ecf43c7fb84c, // 0.579506, 0.560175
         0x3fe29d37fec2b08a, 0x3fe1e2ef3b3fb874, // 0.581692, 0.558952
         0x3fe2af15f02640ad, 0x3fe1d8f5672e4abd, // 0.583873, 0.557734
         0x3fe2c0e9ed448e8b, 0x3fe1cf06ada2811d, // 0.586049, 0.556522
         0x3fe2d2b4012edc9d, 0x3fe1c522fc1ce059, // 0.588221, 0.555315
         0x3fe2e47436e40268, 0x3fe1bb4a4046ed29, // 0.590387, 0.554113
         0x3fe2f62a99509546, 0x3fe1b17c67f2bae3, // 0.592550, 0.552916
         0x3fe307d7334f10be, 0x3fe1a7b9611a7b96, // 0.594707, 0.551724
         0x3fe3197a0fa7fe6a, 0x3fe19e0119e0119e, // 0.596860, 0.550538
         0x3fe32b1339121d71, 0x3fe19453808ca29c, // 0.599008, 0.549356
         0x3fe33ca2ba328994, 0x3fe18ab083902bdb, // 0.601152, 0.548180
         0x3fe34e289d9ce1d3, 0x3fe1811811811812, // 0.603291, 0.547009
         0x3fe35fa4edd36ea0, 0x3fe1778a191bd684, // 0.605425, 0.545842
         0x3fe37117b54747b5, 0x3fe16e0689427379, // 0.607555, 0.544681
         0x3fe38280fe58797e, 0x3fe1648d50fc3201, // 0.609681, 0.543524
         0x3fe393e0d3562a19, 0x3fe15b1e5f75270d, // 0.611802, 0.542373
         0x3fe3a5373e7ebdf9, 0x3fe151b9a3fdd5c9, // 0.613918, 0.541226
         0x3fe3b68449fffc22, 0x3fe1485f0e0acd3b, // 0.616030, 0.540084
         0x3fe3c7c7fff73205, 0x3fe13f0e8d344724, // 0.618137, 0.538947
         0x3fe3d9026a7156fa, 0x3fe135c81135c811, // 0.620240, 0.537815
         0x3fe3ea33936b2f5b, 0x3fe12c8b89edc0ac, // 0.622339, 0.536688
         0x3fe3fb5b84d16f42, 0x3fe12358e75d3033, // 0.624433, 0.535565
         0x3fe40c7a4880dce9, 0x3fe11a3019a74826, // 0.626523, 0.534447
         0x3fe41d8fe84672ae, 0x3fe1111111111111, // 0.628609, 0.533333
         0x3fe42e9c6ddf80bf, 0x3fe107fbbe011080, // 0.630690, 0.532225
         0x3fe43f9fe2f9ce67, 0x3fe0fef010fef011, // 0.632767, 0.531120
         0x3fe4509a5133bb0a, 0x3fe0f5edfab325a2, // 0.634839, 0.530021
         0x3fe4618bc21c5ec2, 0x3fe0ecf56be69c90, // 0.636907, 0.528926
         0x3fe472743f33aaad, 0x3fe0e40655826011, // 0.638971, 0.527835
         0x3fe48353d1ea88df, 0x3fe0db20a88f4696, // 0.641031, 0.526749
         0x3fe4942a83a2fc07, 0x3fe0d24456359e3a, // 0.643087, 0.525667
         0x3fe4a4f85db03ebb, 0x3fe0c9714fbcda3b, // 0.645138, 0.524590
         0x3fe4b5bd6956e273, 0x3fe0c0a7868b4171, // 0.647185, 0.523517
         0x3fe4c679afccee39, 0x3fe0b7e6ec259dc8, // 0.649228, 0.522449
         0x3fe4d72d3a39fd00, 0x3fe0af2f722eecb5, // 0.651267, 0.521385
         0x3fe4e7d811b75bb0, 0x3fe0a6810a6810a7, // 0.653301, 0.520325
         0x3fe4f87a3f5026e8, 0x3fe09ddba6af8360, // 0.655332, 0.519270
         0x3fe50913cc01686b, 0x3fe0953f39010954, // 0.657358, 0.518219
         0x3fe519a4c0ba3446, 0x3fe08cabb37565e2, // 0.659380, 0.517172
         0x3fe52a2d265bc5aa, 0x3fe0842108421084, // 0.661398, 0.516129
         0x3fe53aad05b99b7c, 0x3fe07b9f29b8eae2, // 0.663413, 0.515091
         0x3fe54b2467999497, 0x3fe073260a47f7c6, // 0.665423, 0.514056
         0x3fe55b9354b40bcd, 0x3fe06ab59c7912fb, // 0.667429, 0.513026
         0x3fe56bf9d5b3f399, 0x3fe0624dd2f1a9fc, // 0.669431, 0.512000
         0x3fe57c57f336f190, 0x3fe059eea0727586, // 0.671429, 0.510978
         0x3fe58cadb5cd7989, 0x3fe05197f7d73404, // 0.673423, 0.509960
         0x3fe59cfb25fae87d, 0x3fe04949cc1664c5, // 0.675413, 0.508946
         0x3fe5ad404c359f2c, 0x3fe0410410410410, // 0.677399, 0.507937
         0x3fe5bd7d30e71c73, 0x3fe038c6b78247fc, // 0.679381, 0.506931
         0x3fe5cdb1dc6c1764, 0x3fe03091b51f5e1a, // 0.681359, 0.505929
         0x3fe5ddde57149923, 0x3fe02864fc7729e9, // 0.683334, 0.504931
         0x3fe5ee02a9241675, 0x3fe0204081020408, // 0.685304, 0.503937
         0x3fe5fe1edad18918, 0x3fe0182436517a37, // 0.687271, 0.502947
         0x3fe60e32f44788d8, 0x3fe0101010101010, // 0.689233, 0.501961
         0x3fe62e42fefa39ef, 0x3fe0000000000000, // 0.693147, 0.500000
      };

   float64_t softfloat64::pow( float64_t x, float64_t y ) {

      static const float64_t zero = softfloat64::zero(), one = softfloat64::one(), inf = softfloat64::inf(), nan = softfloat64::nan();
      bool xinf = softfloat64::isInf(x), yinf = softfloat64::isInf(y), xnan = softfloat64::isNaN(x), ynan = softfloat64::isNaN(y);
      float64_t ax = softfloat64::abs(x);
      bool useInf = (y > zero) == (ax > one);
      float64_t v;
      //special cases
      if(ynan) v = nan;
      else if(yinf) v = (ax == one || xnan) ? nan : (useInf ? inf : zero);
      else if(y == zero) v = one;
      else if(y == one ) v = x;
      else //here y is ok
      {
         if(xnan) v = nan;
         else if(xinf) v = (y < zero) ? zero : inf;
         else if(y == f64_roundToInt(y, 0/*softfloat_round_near_even*/, false)) v = softfloat64::powi(x, f64_to_i32(y, 0/*softfloat_round_near_even*/, false));
         else if(x  < zero) v = nan;
         // (0 ** 0) == 1
         else if(x == zero) v = (y < zero) ? inf : (y == zero ? one : zero);
         // here x and y are ok
         else v = softfloat64::exp(f64_mul( y, softfloat64::log(x) ));
      }

      return v;
   }

   float64_t softfloat64::powi( float64_t x, int y ) {

      float64_t v;
      //special case: (0 ** 0) == 1
      if(x == softfloat64::zero()) {
         v = (y < 0) ? softfloat64::inf() : (y == 0 ? softfloat64::one() : softfloat64::zero());
         // here x and y are ok
      } else {
         float64_t a = softfloat64::one(), b = x;
         int p = std::abs(y);
         if( y < 0 ) {
            b = f64_div(softfloat64::one(), b);
         }
         while( p > 1 ) {
            if( p & 1 ) {
               a = f64_mul( a, b );
            }
            b = f64_mul( b, b );
            p >>= 1;
         }
         v = f64_mul( a, b );
      }

      return v;
   }

   float64_t softfloat64::exp( float64_t x ) {

      //special cases
      if(softfloat64::isNaN(x)) return softfloat64::nan();
      if(softfloat64::isInf(x)) return (x == softfloat64::inf()) ? x : softfloat64::zero();

      static const float64_t
         A5 = f64_div( softfloat64::one(), EXPPOLY_32F_A0 ),
         A4 = f64_div( softfloat64::fromRaw(0x3fe62e42fefa39f1), EXPPOLY_32F_A0 ), // .69314718055994546743029643825322 / EXPPOLY_32F_A0
         A3 = f64_div( softfloat64::fromRaw(0x3fcebfbdff82a45a), EXPPOLY_32F_A0 ), // .24022650695886477918181338054308 / EXPPOLY_32F_A0
         A2 = f64_div( softfloat64::fromRaw(0x3fac6b08d81fec75), EXPPOLY_32F_A0 ), // .55504108793649567998466049042729e-1 / EXPPOLY_32F_A0
         A1 = f64_div( softfloat64::fromRaw(0x3f83b2a72b4f3cd3), EXPPOLY_32F_A0 ), // .96180973140732918010002372686186e-2 / EXPPOLY_32F_A0
         A0 = f64_div( softfloat64::fromRaw(0x3f55e7aa1566c2a4), EXPPOLY_32F_A0 ); // .13369713757180123244806654839424e-2 / EXPPOLY_32F_A0

      float64_t x0;
      if(_expF64UI(x.v) > 1023 + 10)
         x0 = _signF64UI(x.v) ? softfloat64::negate( exp_max_val ) : exp_max_val;
      else
         x0 = f64_mul(x, exp_prescale);

      int val0 = f64_to_i32(x0, 0 /*softfloat_round_near_even*/, false);
      int t = (val0 >> EXPTAB_SCALE) + 1023;
      t = t < 0 ? 0 : (t > 2047 ? 2047 : t);
      float64_t buf; buf.v = _packToF64UI(0, t, 0);

      x0 = f64_mul( f64_sub( x0, f64_roundToInt(x0, 0 /*softfloat_round_near_even*/, false) ), exp_postscale );

      return f64_mul(
         f64_mul( f64_mul( buf, EXPPOLY_32F_A0 ), softfloat64::fromRaw(expTab[val0 & EXPTAB_MASK]) ),
         f64_add( f64_mul( f64_add( f64_mul( f64_add( f64_mul( f64_add( f64_mul( f64_add( f64_mul( A0, x0 ), A1 ), x0 ), A2 ), x0 ), A3 ), x0 ), A4 ), x0 ), A5 )
         );
   }

   float64_t softfloat64::log( float64_t x ) {

      //special cases
      if(softfloat64::isNaN(x) || x < softfloat64::zero()) return softfloat64::nan();
      if(x == softfloat64::zero()) return softfloat64::negate(softfloat64::inf());

      static const float64_t
         A7 = i32_to_f64(1),
         A6 = f64_div( softfloat64::negate(softfloat64::one()), ui32_to_f64(2) ),
         A5 = f64_div( softfloat64::one(), ui32_to_f64(3) ),
         A4 = f64_div( softfloat64::negate(softfloat64::one()), ui32_to_f64(4) ),
         A3 = f64_div( softfloat64::one(), ui32_to_f64(5) ),
         A2 = f64_div( softfloat64::negate(softfloat64::one()), ui32_to_f64(6) ),
         A1 = f64_div( softfloat64::one(), ui32_to_f64(7) ),
         A0 = f64_div( softfloat64::negate(softfloat64::one()), ui32_to_f64(8) );

      //first 8 bits of mantissa
      int h0 = (x.v >> (52 - LOGTAB_SCALE)) & ((1 << LOGTAB_SCALE) - 1);
      //buf == 0.00000000_the_rest_mantissa_bits
      float64_t buf; buf.v = _packToF64UI(0, 1023, x.v & ((1LL << (52 - LOGTAB_SCALE)) - 1));
      buf = f64_sub( buf, softfloat64::one() );

      float64_t tab0 = softfloat64::fromRaw(icvLogTab[2*h0]);
      float64_t tab1 = softfloat64::fromRaw(icvLogTab[2*h0 + 1]);

      float64_t x0 = f64_mul( buf, tab1 );
      //if last elements of icvLogTab
      if(h0 == 255) x0 = f64_add( x0, f64_div( softfloat64::negate(softfloat64::one()), ui32_to_f64(512)) );
      float64_t xq = f64_mul( x0, x0 );

      return f64_add(
         f64_add(
            f64_add( f64_mul( ln_2, i32_to_f64( _expF64UI(x.v) - 1023 )), tab0 ),
            f64_mul( f64_add( f64_mul( f64_add( f64_mul( f64_add( f64_mul( A0, xq ), A2 ), xq ), A4 ), xq ), A6 ), xq )
            ),
         f64_mul( f64_add( f64_mul( f64_add( f64_mul( f64_add( f64_mul( A1, xq ), A3 ), xq ), A5 ), xq ), A7 ), x0 )
         );
   }

} } /// infrablockchain::chain