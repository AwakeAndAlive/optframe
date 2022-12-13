// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFCORE_FNS_HPP_
#define OPTFCORE_FNS_HPP_

#include <functional>
#include <string>
//
#include <OptFrame/NS.hpp>

namespace optframe {

template <XESolution XES, typename ProblemType = void>
class FNS final : public NS<XES> {
  using XEv = typename XES::second_type;
  using XSH = XES;  // only single objective

 public:
#ifdef OPTFCORE_FUNC_STATIC
  typedef uptr<Move<XES>> (*FuncTypeNSRand)(sref<ProblemType>, const XES&);
#else
  typedef std::function<uptr<Move<XES>>(sref<ProblemType>, const XES&)>
      FuncTypeNSRand;
#endif

  sref<ProblemType> p;
  FuncTypeNSRand fRandom;

  FNS(sref<ProblemType> _p, FuncTypeNSRand _fRandom)
      : p{_p}, fRandom{_fRandom} {}

  uptr<Move<XES, XEv, XSH>> randomMove(const XES& se) override {
    return fRandom(p, se);
  }

  static std::string idComponent() {
    std::stringstream ss;
    ss << NS<XES>::idComponent() << ":FNS";
    return ss.str();
  }

  std::string id() const override { return idComponent(); }
};

// template specialization for empty problem = void

template <XESolution XES>
class FNS<XES, void> final : public NS<XES> {
  using XEv = typename XES::second_type;
  using XSH = XES;  // only single objective

 public:
#ifdef OPTFCORE_FUNC_STATIC
  typedef uptr<Move<XES>> (*FuncTypeNSRand)(const XES&);
#else
  typedef std::function<uptr<Move<XES>>(const XES&)> FuncTypeNSRand;
#endif

  FuncTypeNSRand fRandom;

  explicit FNS(FuncTypeNSRand _fRandom) : fRandom{_fRandom} {}

  uptr<Move<XES, XEv, XSH>> randomMove(const XES& se) override {
    return fRandom(se);
  }

  static std::string idComponent() {
    std::stringstream ss;
    ss << NS<XES>::idComponent() << ":FNS";
    return ss.str();
  }

  std::string id() const override { return idComponent(); }
};

}  // namespace optframe

#endif  // OPTFCORE_FNS_HPP_
