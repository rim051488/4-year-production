#pragma once
#include <concepts>

class ComponentBase;

// コンポーネントの取得関数用のコンセプト
template<class T>
concept CompC =
	requires (T c) { T::id_; T::baseID_; }&&
std::derived_from<T, ComponentBase> ;
