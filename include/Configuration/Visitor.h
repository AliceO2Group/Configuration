/// \file Visitor.h
/// \brief Definition of class for creating lambda-based visitors for boost::variant
///
/// \author Pascal Boeschoten (pascal.boeschoten@cern.ch)

#ifndef INCLUDE_CONFIGURATION_VISITOR_H_
#define INCLUDE_CONFIGURATION_VISITOR_H_

/// Visitor for use with boost::variant and boost::apply_visitor
/// Based on: http://stackoverflow.com/questions/7870498/using-declaration-in-variadic-template/7870614#7870614
namespace AliceO2 {
namespace Configuration {
namespace Visitor {
namespace Implementation {

/// Class for creating lambda-based visitors for boost::variant
template<typename ReturnT, typename ... Lambdas>
struct Visitor;

template<typename ReturnT, typename L1, typename ... Lambdas>
struct Visitor<ReturnT, L1, Lambdas...> : public L1, public Visitor<ReturnT, Lambdas...>
{
    using L1::operator();
    using Visitor<ReturnT, Lambdas...>::operator();
    Visitor(L1 l1, Lambdas ... lambdas)
        : L1(l1), Visitor<ReturnT, Lambdas...>(lambdas...)
    {
    }
};

template<typename ReturnT, typename L1>
struct Visitor<ReturnT, L1> : public L1, public boost::static_visitor<ReturnT>
{
    using L1::operator();
    Visitor(L1 l1)
        : L1(l1), boost::static_visitor<ReturnT>()
    {
    }
};

template<typename ReturnT>
struct Visitor<ReturnT> : public boost::static_visitor<ReturnT>
{
    Visitor()
        : boost::static_visitor<ReturnT>()
    {
    }
};
} // namespace Implementation

/// Creates a Visitor
template<typename ReturnType, typename ... Lambdas>
Implementation::Visitor<ReturnType, Lambdas...> make(Lambdas ... lambdas)
{
  return {lambdas...};
}

/// Apply lambdas as a visitor to the variant
template<typename ReturnType = void, typename Variant, typename ... Lambdas>
ReturnType apply(const Variant& variant, Lambdas ... lambdas)
{
  auto visitor = Visitor::make<ReturnType>(lambdas...);
  return boost::apply_visitor(visitor, variant);
}

} // namespace Visitor
} // namespace Configuration
} // namespace AliceO2

#endif /* INCLUDE_CONFIGURATION_VISITOR_H_ */
