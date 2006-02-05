dnl A function to detect the binary format used by 128-bit floats.
dnl Copyright (C) 2001-2006 Roberto Bagnara <bagnara@cs.unipr.it>
dnl
dnl This file is part of the Parma Polyhedra Library (PPL).
dnl
dnl The PPL is free software; you can redistribute it and/or modify it
dnl under the terms of the GNU General Public License as published by the
dnl Free Software Foundation; either version 2 of the License, or (at your
dnl option) any later version.
dnl
dnl The PPL is distributed in the hope that it will be useful, but WITHOUT
dnl ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
dnl FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
dnl for more details.
dnl
dnl You should have received a copy of the GNU General Public License
dnl along with this program; if not, write to the Free Software Foundation,
dnl Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02111-1307, USA.
dnl
dnl For the most up-to-date information see the Parma Polyhedra Library
dnl site: http://www.cs.unipr.it/ppl/ .
dnl
AC_DEFUN([AC_CXX_FLOAT128_BINARY_FORMAT],
[
ac_save_CPPFLAGS="$CPPFLAGS"
ac_save_LIBS="$LIBS"
AC_LANG_PUSH(C++)

AC_MSG_CHECKING([the binary format of 128-bit floats])
ac_cxx_float128_binary_format=unknown
AC_RUN_IFELSE([AC_LANG_SOURCE([[
#ifdef HAVE_STDINT_H
#include <stdint.h>
#endif
#ifdef HAVE_INTTYPES_H
#include <inttypes.h>
#endif

#if SIZEOF_FLOAT == 16
#define FLOAT128_TYPE float
#elif SIZEOF_DOUBLE == 16
#define FLOAT128_TYPE double
#elif SIZEOF_LONG_DOUBLE == 16
#define FLOAT128_TYPE long double
#endif

#ifdef FLOAT128_TYPE
typedef FLOAT128_TYPE float128_t;

float128_t
convert(uint64_t msp, uint64_t lsp) {
  union {
    float128_t value;
    struct {
#ifdef WORDS_BIGENDIAN
      uint64_t msp;
      uint64_t lsp;
#else
      uint64_t lsp;
      uint64_t msp;
#endif
    } parts;
  } u;

  u.parts.msp = msp;
  u.parts.lsp = lsp;
  return u.value;
}

int
main() {
  return (convert(0xaaacccaaacccaaacULL, 0xccaaacccaaacccaaULL)
          == -2.9126628883453982064004517242532288e+976L &&
          convert(0xcccaaacccaaacccaULL, 0xaacccaaacccaaaccULL)
          == -3.6599170164423507936822938310049653e-1634L)
    ? 0 : 1;
}

#else // !defined(FLOAT128_TYPE)

int
main() {
  return 1;
}

#endif // !defined(FLOAT128_TYPE)
]])],
  AC_DEFINE(CXX_FLOAT128_BINARY_FORMAT_IS_IEEE754_QUAD_PRECISION, 1,
    [Not zero if 128-bit floats use the IEEE754 Quad Precision binary format.])
  ac_cxx_float128_binary_format="IEEE754 Quad Precision",
  AC_DEFINE(CXX_FLOAT128_BINARY_FORMAT_IS_IEEE754_QUAD_PRECISION, 0))

if test x"$ac_cxx_float128_binary_format" = x"unknown"
then
AC_RUN_IFELSE([AC_LANG_SOURCE([[
#ifdef HAVE_STDINT_H
#include <stdint.h>
#endif
#ifdef HAVE_INTTYPES_H
#include <inttypes.h>
#endif

#if SIZEOF_FLOAT == 16
#define FLOAT128_TYPE float
#elif SIZEOF_DOUBLE == 16
#define FLOAT128_TYPE double
#elif SIZEOF_LONG_DOUBLE == 16
#define FLOAT128_TYPE long double
#endif

#ifdef FLOAT128_TYPE
typedef FLOAT128_TYPE float128_t;

float128_t
convert(uint64_t msp, uint64_t lsp) {
  union {
    float128_t value;
    struct {
#ifdef WORDS_BIGENDIAN
      uint64_t msp;
      uint64_t lsp;
#else
      uint64_t lsp;
      uint64_t msp;
#endif
    } parts;
  } u;

  u.parts.msp = msp;
  u.parts.lsp = lsp;
  return u.value;
}

int
main() {
  return (convert(0xaaacccaaacccaaacULL, 0xccaaacccaaacccaaULL)
          == -7.6048540943660203383375998875878002021755137452856942966991187375790618225432711954682137529456970686263412309600617913197560145993931792374946076061175594550915924955313391528566777352996816141138550473090399783817652516896553304678788796532976847530386107750759127004897950996937530751872097675156395937218234460013748219954603465088115161828048215763754331575034447236689765045674584188927116128770082933362914567237187017530082528540058002631800274192146485961758366625476271676375489358478966148719270989233284454992652229565352739884432045036085427546783826745250197623257802129011015273728848161395367551745780868250488191368846207890422268873532651627591649389757751752362072212699309947970918940313250863861141479770240104635553035870698789854752554391365757900620463269938427975381635241159694500569550818398323639621243086116154792038064941523448921284117826015673798399008555604372098051095571652170081985843614541956756469168604624857938653843172027861680499952062356914208628014745759010068414302636374017506453133466034362025545080555878082849488386388633197121003201192243150535143329731394874806398663589117246866205872431804204733617792317769570293754056793574570217834482046448788177551912250351735294891953434051815332631157674735614138641122106756749236824504094008852401427746414294902929999820878756040223246586814590339767277305131757114819123212897500810087015748205562614251410818122967399030151040544000563841334553718469935435435467196184665715776274355094471974036803615388313095637066824428688301387559074204015990782977635088047810891072724763113879034313256582868462323549141603475107584159170279311985555035822254439699354641467841523895459190280971487264907972306090449968125859834702627544039374770757804202780369083049631377117943503836158566134919890165392965694050010089785271180956974707841066789578714463804030217977213138932711313311673101336981407798868338574571646697479192174043005729926344062133712267722538847635563454980776602355539986984320762864338177037919835721981686778834558429264194379257521818498431326991615024245632036376472844033831041082865649867453234086495085785897620758602105639344460383483879837994732204331335249564434458633345708439547881275060781028856140511029543559841701048277393119691261302356763314623124427357421753631218361407626116820986377721964654181839884670481278855478057996766639496909094607181503284084821580817995489740980323820218685313435967428474842973086612053963343516426226771094376179468881265734324847319127862733279299033951360912038720313525324094079916937921290391173035983474894312808532257620563284126400481460163180987618432784215807933623038747129658746767737999870125917269554155887740155308886259497202672935871853706835670467770080598813171256855182510726902033818328368569915805303784312301890212694874227119220544172084552511408717615136830401766455214293141216171050374325461714383991698910564587610624980490526840861990674615172112493813749497898151186927777122955666180439323595468816129418014664347852590958362752922841159054568358354108159485566264221008508127941168192513144760720303715640604755054290633421601734103622748053919536319416025380484868142967373186019970714662893713233834399238357795019603619284595839414945178963942707310299674873908104634979966232181071013259653467885733418936194081202149071958107062125873498848807502257018093517304220495114497635240876547262237411350327960679115197531609627900227913193653254580253539005743563270152329126178570329413401577715075363838562221558204219798925982825556469998988615897432425107152113543617151738802778259560442930074253918797315241708532198010528564246933829811859278710445896556638416265987381678070434371355795025956293319362293063591248665429733880825207421011852020394068840244110140622151494488609575671090387230241296949998536132398625008053543954239573998167556201366323875845409929752349853515625e-1644L &&
          convert(0xcccaaacccaaacccaULL, 0xaacccaaacccaaaccULL)
          == -99658331877181425640389193712445288804009112642407197633229907048864350192381814628233384153539524368748305269642704450459572458913058829202094408933558533552137589226430537671503754737153845553845646099179512540696038707395491223325946106007770844660381340028079827237033670900446083793353682761885084154898636897779677124010119288945740273072415898996441722571487815052387317025675191665761918119006431828756780493604546658949166486641354783002536071366287780290680620995991797712341457334946893188786269086688063732222194404683551757689083590842400866213237312413463207537587813396338061744078437770542720749055069473347142994267706326342325536219464867910547533482061181116137767384001927599515332824741827726661184966512254203502805790565338206862173475388342339711722457620964017690492860707751327158273522191943184085888284707357024653025991470473697475045491586713324994056478341556198451786713470909185879382607340766256394396819602885198511409676789226542867632933493115191296L)
    ? 0 : 1;
}

#else // !defined(FLOAT128_TYPE)

int
main() {
  return 1;
}

#endif // !defined(FLOAT128_TYPE)
]])],
  AC_DEFINE(CXX_FLOAT128_BINARY_FORMAT_IS_INTEL_DOUBLE_EXTENDED, 1,
  [Not zero if 128-bit floats use the Intel Double-Extended binary format.])
  ac_cxx_float128_binary_format="Intel Double-Extended",
  AC_DEFINE(CXX_FLOAT128_BINARY_FORMAT_IS_INTEL_DOUBLE_EXTENDED, 0))
fi

AC_MSG_RESULT($ac_cxx_float128_binary_format)

AC_LANG_POP(C++)
CPPFLAGS="$ac_save_CPPFLAGS"
LIBS="$ac_save_LIBS"
])
