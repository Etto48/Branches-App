#pragma once

#include "../branches_core/branches_core.h"
#include <locale>
#include <codecvt>
#include <string>


std::string Managed_Str_To_Std_Str(Platform::String^ ms)
{
	std::wstring w_str(ms->Begin());
	return std::string(w_str.begin(), w_str.end());
}

Platform::String^ Std_Str_To_Managed_Str(const std::string& input)
{
	std::wstring w_str = std::wstring(input.begin(), input.end());
	const wchar_t* w_chars = w_str.c_str();
	return (ref new Platform::String(w_chars));
}

std::tuple<bool,std::string,std::string,T> filterInput(Platform::String^ input,std::map<std::string,T> &symMap)
{
	auto in = Managed_Str_To_Std_Str(input);

	std::string name;
	bool constant = false;
	if (in.find('=') != std::string::npos)
	{
		name=in.substr(0,in.find('='));
		in = in.substr(in.find('=')+1);
		if (in.find('=') != std::string::npos) throw algebra_tools_::except("Too many =");
	}
	else
	{
		name = in;
	}
	std::string ret;
	try
	{
		vectorParser vp(in);
		try
		{
			ret = vp.evalToString(symMap);
			if (vp.getDimensions() == 1)
				constant = true;
		}
		catch (algebra_tools_::except& e)
		{
			if (std::string(e.what()).find("Not recognized symbol") == std::string::npos)
				throw;
			ret = vp.simplify();
		}
	}
	catch (algebra_tools_::except& e)
	{
		throw;
	}
	if(constant)
		return { constant,name,ret, vectorParser(in).evaluate(symMap)[0] };
	else
		return { constant,name,ret, 0};
}