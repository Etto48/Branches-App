//
// MainPage.xaml.h
// Declaration of the MainPage class.
//

#pragma once

#include "MainPage.g.h"

namespace Branches
{
	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
	public ref class MainPage sealed
	{
	public:
		MainPage();

	private:

		void AddButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void RemoveButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void DataList_RightTapped(Platform::Object^ sender, Windows::UI::Xaml::Input::RightTappedRoutedEventArgs^ e);
		void savePartial(std::string dir);
		void DeleteEntryFromMaps(Platform::String^ entry);
		void DeleteEntry(Platform::String^ entry);
		void AddData(Platform::String^ data, bool emptyDataInputBox);

		void Partial_x(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void Partial_y(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void Partial_t(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void Partial_u(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void Partial_v(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void Edit_Data(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void DrawButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
	};
}