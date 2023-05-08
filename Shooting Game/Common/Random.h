#pragma once
#include <random>


// 乱数を扱うクラス
class Random
{
public:
	Random();

	/// <summary> 任意の確率でtrueを返す </summary>
	/// <param name="p"> 確率 </param>
	/// <returns> trueかfalse </returns>
	bool GetRandom(const double& p);

	/// <summary> 一定確率で任意の範囲の整数を返す </summary>
	/// <param name="minVal"> 最小値 </param>
	/// <param name="maxVal"> 最大値 </param>
	/// <returns> 任意の範囲の整数 </returns>
	int GetRandom(const int& minVal, const int& maxVal);

	/// <summary> 一定確率で任意の範囲の浮動小数点数(float)を返す </summary>
	/// <param name="minVal"> 最小値 </param>
	/// <param name="maxVal"> 最大値 </param>
	/// <returns> 任意の範囲の浮動小数点数(float) </returns>
	float GetRandom(const float& minVal, const float& maxVal);

	/// <summary> 一定確率で任意の範囲の浮動小数点数(double)を返す </summary>
		/// <param name="minVal"> 最小値 </param>
		/// <param name="maxVal"> 最大値 </param>
		/// <returns> 任意の範囲の浮動小数点数(double) </returns>
	double GetRandom(const double& minVal, const double& maxVal);
private:
	std::mt19937_64 mt_;
};

