//
// MainPage.xaml.cpp
// Implementation of the MainPage class.
//

#include "pch.h"
#include "MainPage.xaml.h"
#include "branches_core/branches_core.h"

#include "Input.h"

using namespace Branches;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

// The Blank Page item template is documented at https://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x409

std::map<std::string, T> symMap;
std::map<std::string, std::string> functions;

MainPage::MainPage()
{
	InitializeComponent();
}

void Branches::MainPage::AddData(Platform::String^ data,bool emptyDataInputBox)
{
	if (data != "")
	{
		ListViewItem^ newData = ref new ListViewItem();
		try
		{
			auto val = filterInput(data, symMap);
			bool constant = std::get<0>(val);


			if ((functions.find(std::get<1>(val)) == functions.end()) && (symMap.find(std::get<1>(val)) == symMap.end()))
			{
				if (constant)
				{
					symMap.emplace(std::get<1>(val), std::get<3>(val));

				}
				else
				{
					functions.emplace(std::get<1>(val), std::get<2>(val));
				}
			}
			else return;
			newData->Content = Std_Str_To_Managed_Str((std::get<1>(val) + "=" + std::get<2>(val)));
			MainPage::DataList->Items->Append(newData);
			if(emptyDataInputBox)
				MainPage::DataInputBox->Text = "";

		}
		catch (algebra_tools_::except& e)
		{
			MainPage::DebugText->Text = Std_Str_To_Managed_Str(e.what());
		}
	}
}

void Branches::MainPage::AddButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	AddData(MainPage::DataInputBox->Text, true);
}

void Branches::MainPage::DeleteEntryFromMaps(Platform::String^ entry)
{
	auto val = filterInput(entry, symMap);
	if (symMap.find(std::get<1>(val)) != symMap.end())
	{
		symMap.erase(std::get<1>(val));
	}
	else if (functions.find(std::get<1>(val)) != functions.end())
	{
		functions.erase(std::get<1>(val));
	}
}

void Branches::MainPage::DeleteEntry(Platform::String^ entry)
{
	for (int i = 0; i < MainPage::DataList->Items->Size; i++)
	{
		if (((ListViewItem^)MainPage::DataList->Items->GetAt(i))->Content->ToString() == entry)
		{
			DeleteEntryFromMaps(entry);
			MainPage::DataList->Items->RemoveAt(i);
			break;
		}
	}
}

void Branches::MainPage::RemoveButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	if (MainPage::DataList->SelectedIndex > -1)
	{

		auto selectedText = ((ListViewItem^)(MainPage::DataList->SelectedItem))->Content->ToString();

		DeleteEntry(selectedText);
		
	}
}

void Branches::MainPage::DataList_RightTapped(Platform::Object^ sender, Windows::UI::Xaml::Input::RightTappedRoutedEventArgs^ e)
{
	
	auto a = ((FrameworkElement^)e->OriginalSource)->DataContext;

	for (int i=0;i<MainPage::DataList->Items->Size;i++)
	{
		if (((ListViewItem^)MainPage::DataList->Items->GetAt(i))->Content->ToString() == a->ToString())
		{
			((ListViewItem^)MainPage::DataList->Items->GetAt(i))->IsSelected = true;
			MainPage::Menu->ShowAt(MainPage::DataList, e->GetPosition(MainPage::DataList));
			break;
		}
	}
}

void Branches::MainPage::savePartial(std::string dir)
{
	auto selectedItem = ((ListViewItem^)(MainPage::DataList->SelectedItem));
	if (selectedItem && selectedItem->Content->ToString() != "")
	{
		auto selectedText = selectedItem->Content->ToString();
		ListViewItem^ newData = ref new ListViewItem();
		try
		{
			auto val = filterInput(selectedText, symMap);
			auto der = Std_Str_To_Managed_Str(std::get<1>(val) + dir + "=" + algebraParser(std::get<2>(val)).derivative(dir));
			AddData(der, false);
		}
		catch (algebra_tools_::except &e)
		{
			MainPage::DebugText->Text = Std_Str_To_Managed_Str(e.what());
		}
	}
}

void Branches::MainPage::Partial_x(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	savePartial("x");
}

void Branches::MainPage::Partial_y(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	savePartial("y");
}

void Branches::MainPage::Partial_t(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	savePartial("t");
}

void Branches::MainPage::Partial_u(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	savePartial("u");
}

void Branches::MainPage::Partial_v(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	savePartial("v");
}

void Branches::MainPage::Edit_Data(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	auto selectedItem = ((ListViewItem^)(MainPage::DataList->SelectedItem));
	MainPage::DataInputBox->Text = selectedItem->Content->ToString();
	DeleteEntry(selectedItem->Content->ToString());
}


void Branches::MainPage::DrawButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{	
	
}
