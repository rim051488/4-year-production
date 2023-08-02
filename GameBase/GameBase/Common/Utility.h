#pragma once
#include <string>
#include <fstream>

/// <summary>
/// ファイルストリームから文字列を読み取る
/// </summary>
/// <param name="file"> ファイルストリーム </param>
/// <returns> 読み取った文字列 </returns>
std::string ReadString(std::ifstream& file);

