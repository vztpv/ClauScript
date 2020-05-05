#define _CRT_SECURE_NO_WARNINGS

#include "wiz/ClauText.h"
using namespace std::literals;

namespace wiz {

template <class T> // T has clear method, default constructor.
class Node
{
public:
	T * data;
	Node* left;
	Node* right;
public:
	static T* f(T* ptr = nullptr) {
		T* result = nullptr;
		if (nullptr == ptr) {
			result = new T();
		}
		else {
			ptr->clear();
			result = ptr;
		}

		return result;
	}
};

std::string ClauText::excute_module(const std::string& MainStr, wiz::load_data::UserType* _global, const ExecuteData& excuteData, Option& opt, int chk)
{
	wiz::Map2<std::string, std::pair<std::vector<std::string>, bool>>* __map = opt._map;
	opt._map = Node<wiz::Map2<std::string, std::pair<std::vector<std::string>, bool>>>::f(__map);
	wiz::Map2<std::string, std::pair<std::vector<std::string>, bool>>& _map = *opt._map;
																		   //
	wiz::load_data::UserType& global = *_global;
	//std::vector<std::thread*> waits;
	wiz::Map2<std::string, wiz::load_data::UserType>* _objectMap = opt.objectMap;
	opt.objectMap = Node<wiz::Map2<std::string, wiz::load_data::UserType>>::f(_objectMap);
	wiz::Map2<std::string, wiz::load_data::UserType>& objectMap = *opt.objectMap;

	wiz::Map2<std::string, wiz::load_data::UserType>* _moduleMap = opt.moduleMap;
	opt.moduleMap = Node<wiz::Map2<std::string, wiz::load_data::UserType>>::f(_moduleMap);
	wiz::Map2<std::string, wiz::load_data::UserType>& moduleMap = *opt.moduleMap;

	wiz::Map2<std::string, wiz::load_data::UserType>* objectMapPtr = nullptr;
	wiz::Map2<std::string, wiz::load_data::UserType>* moduleMapPtr = nullptr;

	std::string* _module_value = opt.module_value;
	opt.module_value = Node<std::string>::f(_module_value);
	std::string& module_value = *opt.module_value;

	// data, event load..
	wiz::ArrayStack<EventInfo>* _eventStack = opt.eventStack;
	opt.eventStack = Node<wiz::ArrayStack<EventInfo>>::f(_eventStack);
	wiz::ArrayStack<EventInfo>& eventStack = *opt.eventStack;

	wiz::Map2<std::string, int>* _convert = opt.convert;
	opt.convert = Node<wiz::Map2<std::string, int>>::f(_convert);
	wiz::Map2<std::string, int>& convert = *opt.convert;

	std::vector<wiz::load_data::UserType*>* __events = opt._events;
	opt._events = Node<std::vector<wiz::load_data::UserType*>>::f(__events);
	std::vector<wiz::load_data::UserType*>& _events = *opt._events;

	wiz::load_data::UserType* _events_ = opt.events;
	opt.events = Node<wiz::load_data::UserType>::f(_events_);
	wiz::load_data::UserType& events = *opt.events;

	wiz::load_data::UserType* eventPtr = nullptr;

	wiz::load_data::UserType* _Main = opt.Main;
	opt.Main = Node<wiz::load_data::UserType>::f(_Main);
	wiz::load_data::UserType& Main = *opt.Main;
	

	// start from Main.
	if (excuteData.chkInfo == false) { /// chk smartpointer.
		if (global.GetUserTypeItem("Main").empty() && MainStr.empty())
		{
			wiz::Out << "do not exist Main" << ENTER;
			return "ERROR -1";
		}

		wiz::load_data::UserType* _Main = nullptr;

		if (MainStr.empty()) {
			_Main = global.GetCopyUserTypeItem("Main")[0];
			Main.LinkUserType(_Main);
			global.RemoveUserTypeList("Main");
		}
		else {
			wiz::load_data::LoadData::LoadDataFromString(MainStr, Main);
		}


		EventInfo info;
		info.eventUT = Main.GetUserTypeList(0);
		info.userType_idx.push(0);
		std::pair<std::string, std::string> id_data = std::make_pair<std::string, std::string>("id", wiz::ToString(info.eventUT->GetUserTypeItem("$call")[0]->GetItem("id")[0].Get(0)));
		info.parameters.insert(
			id_data
		);
		info.id = info.parameters["id"];

		eventStack.push(info);

		// $load_only_data 
		{
			wiz::load_data::UserType* val = nullptr;
			auto x = info.eventUT->GetUserTypeItem("$load_only_data");
			if (!x.empty()) {
				for (int i = 0; i < x.size(); ++i) {
					val = x[i];

					wiz::load_data::UserType ut;
					std::string fileName = wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(0), ExecuteData()).ToString();
					fileName = wiz::String::substring(fileName, 1, fileName.size() - 2);
					std::string dirName = val->GetUserTypeList(1)->ToString();
					wiz::load_data::UserType* utTemp;

					if (dirName == "/./" || dirName == "root") {
						utTemp = &global;
					}
					else {
						dirName = wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(1), ExecuteData()).ToString();
						utTemp = global.GetUserTypeItem(dirName)[0];
					}

					std::string option;

					if (val->GetUserTypeListSize() >= 3) {
						option = val->GetUserTypeList(2)->ToString();
					}

					if (wiz::load_data::LoadData::LoadDataFromFile(fileName, ut)) {
						{
							int item_count = 0;
							int userType_count = 0;

							for (int i = 0; i < ut.GetIListSize(); ++i) {
								if (ut.IsItemList(i)) {
									utTemp->AddItem(std::move(ut.GetItemList(item_count).GetName()),
										std::move(ut.GetItemList(item_count).Get(0)));
									item_count++;
								}
								else {
									utTemp->AddUserTypeItem(std::move(*ut.GetUserTypeList(userType_count)));
									userType_count++;
								}
							}
						}

						//	auto _Main = ut.GetUserTypeItem("Main");
						//	if (!_Main.empty())
						//	{
						// error!
						//		wiz::Out << "err" << ENTER;

						//			return "ERROR -2"; /// exit?
						//		}
					}
					else {
						// error!
					}
				}
			}
		}


		if (nullptr == excuteData.pModule) {
			moduleMapPtr = &moduleMap;
		}
		else {
			moduleMapPtr = excuteData.pModule;
		}

		if (nullptr == excuteData.pObjectMap) {
			objectMapPtr = &objectMap;
		}
		else {
			objectMapPtr = excuteData.pObjectMap;
		}

		if (nullptr == excuteData.pEvents) {
			_events = global.GetUserTypeItem("Event");
			for (int i = 0; i < _events.size(); ++i) {
				events.LinkUserType(_events[i]);
			}
			global.RemoveUserTypeList("Event", false);
			eventPtr = &events;
		}
		else {
			eventPtr = excuteData.pEvents;
		}

		// event table setting
		for (int i = 0; i < eventPtr->GetUserTypeListSize(); ++i)
		{
			auto x = eventPtr->GetUserTypeList(i)->GetItem("id");
			if (!x.empty()) {
				//wiz::Out <<	x[0] << ENTER;
				auto temp = std::pair<std::string, int>(wiz::ToString(x[0].Get(0)), i);
				convert.insert(temp);
			}
			else {
				// error
			}
		}

		const int no = convert.at(info.id);
		for (int i = 0; i < eventPtr->GetUserTypeList(no)->GetUserTypeListSize(); ++i) {
			if (eventPtr->GetUserTypeList(no)->GetUserTypeList(i)->GetName() == "$local") {
				for (int j = 0; j < eventPtr->GetUserTypeList(no)->GetUserTypeList(i)->GetItemListSize(); ++j) {
					std::string name = wiz::ToString(eventPtr->GetUserTypeList(no)->GetUserTypeList(i)->GetItemList(j).Get(0));
					std::string value = "";
					std::pair<std::string, std::string> temp(name, value);
					info.locals.insert(temp);
				}
				break;
			}
		}
	}
	else {
		eventStack.push(excuteData.info);
	}

	// main loop
	while (!eventStack.empty())
	{
		// 
		EventInfo info = eventStack.top();
		std::string str;
		wiz::ArrayMap<std::string, std::string>::iterator x;
		for (int i = 0; i < info.parameters.size(); ++i) {
			if ((x = info.parameters.find("id")) != info.parameters.end()) {
				str = x->second;
				break;
			}
		}

		int no = convert.at(str);

		int state = 0;

		if (info.userType_idx.size() == 1 && info.userType_idx[0] >= eventPtr->GetUserTypeList(no)->GetUserTypeListSize())
		{
			if (eventStack.size() == 1)
			{
				module_value = eventStack.top().return_value;
			}

			eventStack.pop();
			continue;
		}

		{ /// has bug!! WARNNING!!
			wiz::load_data::UserType* val = nullptr;
			if (eventStack.top().userType_idx.size() == 1) {
				if (eventPtr->GetUserTypeList(no)->GetUserTypeListSize() > eventStack.top().userType_idx.top()) {
					val = eventPtr->GetUserTypeList(no)->GetUserTypeList(eventStack.top().userType_idx.top());

					if (eventStack.top().userType_idx.top() >= 1 && val->GetName() == "$else"
						&& wiz::ToString(eventPtr->GetUserTypeList(no)->GetUserTypeList(eventStack.top().userType_idx.top() - 1)->GetName()) != "$if") {
						return "ERROR not exist $if, front $else.";
					}
					if (eventStack.top().userType_idx.top() == 0 && val->GetName() == "$else") {
						return "ERROR not exist $if, front $else.";
					}
				}
				else {
					val = nullptr;
				}
			}
			else
			{
				// # of userType_idx == nowUT.size() + 1, and nowUT.size() == conditionStack.size()..
				while (!eventStack.top().nowUT.empty() && eventStack.top().nowUT.top()->GetUserTypeListSize() <= eventStack.top().userType_idx.top())
				{
					eventStack.top().nowUT.pop();
					eventStack.top().userType_idx.pop();
					eventStack.top().conditionStack.pop();
				}

				if (!eventStack.top().nowUT.empty() && eventStack.top().nowUT.top()->GetUserTypeListSize() > eventStack.top().userType_idx.top()) {
					val = eventStack.top().nowUT.top()->GetUserTypeList(eventStack.top().userType_idx.top());

					if (eventStack.top().userType_idx.top() >= 1 && val->GetName() == "$else"
						&& wiz::ToString(eventStack.top().nowUT.top()->GetUserTypeList(eventStack.top().userType_idx.top() - 1)->GetName()) != "$if") {
						return "ERROR not exist $if, front $else.";
					}
					if (eventStack.top().userType_idx.top() == 0 && val->GetName() == "$else") {
						return "ERROR not exist $if, front $else.";
					}
				}
				else // same to else if( eventSTack.top().nowUT.empty() ), also same to else if ( 1 == eventStack.top().userType_idx.size() )
				{
					if (eventPtr->GetUserTypeList(no)->GetUserTypeListSize() > eventStack.top().userType_idx.top()) {
						val = eventPtr->GetUserTypeList(no)->GetUserTypeList(eventStack.top().userType_idx.top());

						if (eventStack.top().userType_idx.top() >= 1 && val->GetName() == "$else"
							&& wiz::ToString(eventPtr->GetUserTypeList(no)->GetUserTypeList(eventStack.top().userType_idx.top() - 1)->GetName()) != "$if") {
							return "ERROR not exist $if, front $else.";
						}
						if (eventStack.top().userType_idx.top() == 0 && val->GetName() == "$else") {
							return "ERROR not exist $if, front $else.";
						}
					}
					else {
						val = nullptr;
					}
				}
			}

			while (val != nullptr)
			{
				if ("$copy"sv == val->GetName()) { // to = { } from = { } option = { 1 : internal data, default, 2 : total data }
					ExecuteData _excuteData; _excuteData.depth = excuteData.depth;
					_excuteData.chkInfo = true;
					_excuteData.info = eventStack.top();
					_excuteData.pObjectMap = objectMapPtr;
					_excuteData.pEvents = eventPtr;
					_excuteData.pModule = moduleMapPtr;

					const std::string to = val->GetUserTypeList(0)->ToString();
					const std::string from = val->GetUserTypeList(1)->ToString();
					std::string option = "1";	// 1 : internal
												// 2 : total.
					if (val->GetUserTypeListSize() >= 3) {
						option = val->GetUserTypeList(2)->ToString();
					}

					wiz::load_data::UserType* to_ut = wiz::load_data::UserType::Find(&global, to).second[0];
					wiz::load_data::UserType* from_ut = wiz::load_data::UserType::Find(&global, from).second[0];

					if (to_ut == from_ut) {
						wiz::Out << "to_ut == from_ut\n";

						eventStack.top().userType_idx.top()++;
						break;
					}
					if ("1" == option) {
						int itCount = 0;
						int utCount = 0;
						for (int i = 0; i < from_ut->GetIListSize(); ++i) {
							if (from_ut->IsItemList(i)) {
								to_ut->AddItemType(from_ut->GetItemList(itCount));
								itCount++;
							}
							else {
								to_ut->AddUserTypeItem(*from_ut->GetUserTypeList(utCount));
								utCount++;
							}
						}
					}
					else {
						to_ut->AddUserTypeItem(*from_ut);
					}
					
					eventStack.top().userType_idx.top()++;
					break;
				}
				else if ("$iterate"sv == val->GetName()) { // very slow? why??
					ExecuteData _excuteData; _excuteData.depth = excuteData.depth;
					_excuteData.chkInfo = true;
					_excuteData.info = eventStack.top();
					_excuteData.pObjectMap = objectMapPtr;
					_excuteData.pEvents = eventPtr;
					_excuteData.pModule = moduleMapPtr;

					std::string dir = val->GetUserTypeList(0)->ToString();
					std::vector<std::string> events; // event_ids

					for (int i = 0; i < val->GetUserTypeList(1)->GetItemListSize(); ++i) {
						events.push_back(wiz::ToString(val->GetUserTypeList(1)->GetItemList(i).Get(0)));
					}

					std::string recursive = "FALSE";

					if (val->GetUserTypeListSize() >= 3) {
						recursive = val->GetUserTypeList(2)->ToString();
						recursive = wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(2), _excuteData).ToString();
					}

					std::string before_value;

					if (val->GetUserTypeListSize() >= 4) {
						before_value = wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(3), _excuteData).ToString();
					}
					wiz::load_data::LoadData::Iterate(global, dir, events, recursive, before_value, _excuteData);


					eventStack.top().userType_idx.top()++;
					break;
				}
				else if ("$iterateA"sv == val->GetName()) { // very slow? why??
					ExecuteData _excuteData; _excuteData.depth = excuteData.depth;
					_excuteData.chkInfo = true;
					_excuteData.info = eventStack.top();
					_excuteData.pObjectMap = objectMapPtr;
					_excuteData.pEvents = eventPtr;
					_excuteData.pModule = moduleMapPtr;

					std::string dir = wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(0), _excuteData).ToString();
					std::vector<std::string> events; // event_ids

					for (int i = 0; i < val->GetUserTypeList(1)->GetItemListSize(); ++i) {
						events.push_back(wiz::ToString(val->GetUserTypeList(1)->GetItemList(i).Get(0)));
					}

					std::string recursive = "FALSE";

					if (val->GetUserTypeListSize() >= 3) {
						recursive = wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(2), _excuteData).ToString();
					}

					std::string before_value;

					if (val->GetUserTypeListSize() >= 4) {
						before_value = wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(3), _excuteData).ToString();
					}
					wiz::load_data::LoadData::Iterate(global, dir, events, recursive, before_value, _excuteData);


					eventStack.top().userType_idx.top()++;
					break;
				}
				else if ("$iterate2"sv == val->GetName()) { // very slow? why??
					ExecuteData _excuteData; _excuteData.depth = excuteData.depth;
					_excuteData.chkInfo = true;
					_excuteData.info = eventStack.top();
					_excuteData.pObjectMap = objectMapPtr;
					_excuteData.pEvents = eventPtr;
					_excuteData.pModule = moduleMapPtr;

					std::string dir = val->GetUserTypeList(0)->ToString();
					std::vector<std::string> events; // event_ids

					for (int i = 0; i < val->GetUserTypeList(1)->GetItemListSize(); ++i) {
						events.push_back(wiz::ToString(val->GetUserTypeList(1)->GetItemList(i).Get(0)));
					}

					std::string recursive = "FALSE";

					if (val->GetUserTypeListSize() >= 3) {
						recursive = wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(2), _excuteData).ToString();
					}
					wiz::load_data::LoadData::Iterate2(global, dir, events, recursive, _excuteData);


					eventStack.top().userType_idx.top()++;
					break;
				}
				else if ("$iterate3"sv == val->GetName()) { //
					ExecuteData _excuteData; _excuteData.depth = excuteData.depth;
					_excuteData.chkInfo = true;
					_excuteData.info = eventStack.top();
					_excuteData.pObjectMap = objectMapPtr;
					_excuteData.pEvents = eventPtr;
					_excuteData.pModule = moduleMapPtr;

					std::string dir = val->GetUserTypeList(0)->ToString();
					std::string name = wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(1), _excuteData).ToString();
					std::vector<std::string> events; // event_ids

					for (int i = 0; i < val->GetUserTypeList(2)->GetItemListSize(); ++i) {
						events.push_back(wiz::ToString(val->GetUserTypeList(2)->GetItemList(i).Get(0)));
					}

					std::string recursive = "FALSE";

					if (val->GetUserTypeListSize() >= 4) {
						recursive = wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(3), _excuteData).ToString();
					}

					std::string before_value;

					if (val->GetUserTypeListSize() >= 5) {
						before_value = wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(4), _excuteData).ToString();
					}
					wiz::load_data::LoadData::Iterate3(global, dir, name, events, recursive, before_value, _excuteData);


					eventStack.top().userType_idx.top()++;
					break;
				}
				else if ("$riterate"sv == val->GetName()) { // very slow? why??
					ExecuteData _excuteData; _excuteData.depth = excuteData.depth;
					_excuteData.chkInfo = true;
					_excuteData.info = eventStack.top();
					_excuteData.pObjectMap = objectMapPtr;
					_excuteData.pEvents = eventPtr;
					_excuteData.pModule = moduleMapPtr;

					std::string dir = val->GetUserTypeList(0)->ToString();
					std::vector<std::string> events; // event_ids

					for (int i = 0; i < val->GetUserTypeList(1)->GetItemListSize(); ++i) {
						events.push_back(wiz::ToString(val->GetUserTypeList(1)->GetItemList(i).Get(0)));
					}

					std::string recursive = "FALSE";

					if (val->GetUserTypeListSize() >= 3) {
						recursive = wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(2), _excuteData).ToString();
					}
					wiz::load_data::LoadData::RIterate(global, dir, events, recursive, _excuteData);


					eventStack.top().userType_idx.top()++;
					break;
				}
				else if ("$while"sv == val->GetName()) {
					ExecuteData _excuteData; _excuteData.depth = excuteData.depth;
					_excuteData.chkInfo = true;
					_excuteData.info = eventStack.top();
					_excuteData.pObjectMap = objectMapPtr;
					_excuteData.pEvents = eventPtr;
					_excuteData.pModule = moduleMapPtr;

					const std::string condition = wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(0), _excuteData).ToString();

					if ("TRUE" == condition) {
						eventStack.top().conditionStack.push("TRUE");
						eventStack.top().nowUT.push(val->GetUserTypeList(1));
						eventStack.top().userType_idx.push(0);
						break;
					}
					else {
						eventStack.top().userType_idx.top()++;
						break;
					}
				}
				else if ("$do"sv == val->GetName()) { // chk? - need example!
					ExecuteData _excuteData; _excuteData.depth = excuteData.depth;
					_excuteData.chkInfo = true;
					_excuteData.info = eventStack.top();
					_excuteData.pObjectMap = objectMapPtr;
					_excuteData.pEvents = eventPtr;
					_excuteData.pModule = moduleMapPtr;
					_excuteData.noUseInput = excuteData.noUseInput; //// check!
					_excuteData.noUseOutput = excuteData.noUseOutput;


					wiz::load_data::UserType subGlobal;
					wiz::load_data::LoadData::LoadDataFromString(val->GetUserTypeList(1)->ToString(), subGlobal);
					wiz::load_data::UserType inputUT;
					wiz::load_data::LoadData::LoadDataFromString(wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(0), _excuteData).ToString(), inputUT);


					wiz::load_data::LoadData::AddData(subGlobal, "/./", inputUT.ToString(), _excuteData);

					{
						ExecuteData _excuteData;
						_excuteData.noUseInput = excuteData.noUseInput;
						_excuteData.noUseOutput = excuteData.noUseOutput;

						Option opt;
						eventStack.top().return_value = excute_module("", &subGlobal, _excuteData, opt, 0); // return ?
					}

					eventStack.top().userType_idx.top()++;
					break;
				}
				else if ("$remove_usertype_total"sv == val->GetName()) { //// has bug?
					ExecuteData _excuteData; _excuteData.depth = excuteData.depth;
					_excuteData.chkInfo = true;
					_excuteData.info = eventStack.top();
					_excuteData.pObjectMap = objectMapPtr;
					_excuteData.pEvents = eventPtr;
					_excuteData.pModule = moduleMapPtr;

					std::string ut_name = wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(0), _excuteData).ToString();
					std::string condition = "TRUE";
					std::string start_dir = "root";

					if (val->GetUserTypeListSize() >= 2)
					{
						condition = val->GetUserTypeList(1)->ToString();
					}
					if (val->GetUserTypeListSize() >= 3) {
						start_dir = wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(2), _excuteData).ToString();
					}
					bool recursive = false;

					wiz::load_data::LoadData::RemoveUserTypeTotal(global, ut_name,  start_dir, _excuteData, recursive);

					eventStack.top().userType_idx.top()++;
					break;
				}
				else if ("$save"sv == val->GetName()) // save data, event, main!
				{
					ExecuteData _excuteData; _excuteData.depth = excuteData.depth;
					_excuteData.chkInfo = true;
					_excuteData.info = eventStack.top();
					_excuteData.pObjectMap = objectMapPtr;
					_excuteData.pEvents = eventPtr;
					_excuteData.pModule = moduleMapPtr;
					//todo
					// "filename" save_option(0~2)
					std::string fileName = wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(0), _excuteData).ToString();
					fileName = wiz::String::substring(fileName, 1, fileName.size() - 2);
					std::string option = wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(1), _excuteData).ToString();

					wiz::load_data::LoadData::SaveWizDB(global, fileName, option, "");
					wiz::load_data::LoadData::SaveWizDB(Main, fileName, option, "APPEND");
					wiz::load_data::LoadData::SaveWizDB(events, fileName, option, "APPEND");

					eventStack.top().userType_idx.top()++;
					break;
				}
				else if ("$save_data_only"sv == val->GetName())
				{
					ExecuteData _excuteData; _excuteData.depth = excuteData.depth;
					_excuteData.chkInfo = true;
					_excuteData.info = eventStack.top();
					_excuteData.pObjectMap = objectMapPtr;
					_excuteData.pEvents = eventPtr;
					_excuteData.pModule = moduleMapPtr;
					//todo
					// "filename" save_option(0~2)
					std::string fileName = wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(0), _excuteData).ToString();
					if (fileName.back() == '\"' && fileName.size() >= 2 && fileName[0] == fileName.back()) {
						fileName = wiz::String::substring(fileName, 1, fileName.size() - 2);
					}
					std::string option = wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(1), _excuteData).ToString();

					wiz::load_data::LoadData::SaveWizDB(global, fileName, option, "");

					eventStack.top().userType_idx.top()++;
					break;
				}

				else if ("$save_data_only2"sv == val->GetName())
				{
					ExecuteData _excuteData; _excuteData.depth = excuteData.depth;
					_excuteData.chkInfo = true;
					_excuteData.info = eventStack.top();
					_excuteData.pObjectMap = objectMapPtr;
					_excuteData.pEvents = eventPtr;
					_excuteData.pModule = moduleMapPtr;
					//todo
					// "filename" save_option(0~2)
					std::string dirName = wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(0), _excuteData).ToString();
					std::string fileName = wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(1), _excuteData).ToString();
					
					if (fileName.back() == '\"' && fileName.size() >= 2 && fileName[0] == fileName.back()) {
						fileName = wiz::String::substring(fileName, 1, fileName.size() - 2);
					}
					std::string option = wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(2), _excuteData).ToString();

					// todo - for? auto x = global.GetUserTypeItem(dirName);
					wiz::load_data::UserType* utTemp;

					utTemp = wiz::load_data::UserType::Find(&global, dirName).second[0];

					wiz::load_data::LoadData::SaveWizDB(*utTemp, fileName, option, "");


					eventStack.top().userType_idx.top()++;
					break;
				}
				else if ("$option"sv == val->GetName()) // first
				{
					ExecuteData _excuteData; _excuteData.depth = excuteData.depth;
					_excuteData.chkInfo = true;
					_excuteData.info = eventStack.top();
					_excuteData.pObjectMap = objectMapPtr;
					_excuteData.pEvents = eventPtr;
					_excuteData.pModule = moduleMapPtr;

					eventStack.top().option = wiz::load_data::LoadData::ToBool4(nullptr, global, *val, _excuteData).ToString();

					eventStack.top().userType_idx.top()++;
					break;
				}
				// done - ($push_back-insert!) $pop_back, $push_front, $pop_front ($front?, $back?)
				else if ("$pop_back"sv == val->GetName()) {
					ExecuteData _excuteData; _excuteData.depth = excuteData.depth;
					_excuteData.chkInfo = true;
					_excuteData.info = eventStack.top();
					_excuteData.pObjectMap = objectMapPtr;
					_excuteData.pEvents = eventPtr;
					_excuteData.pModule = moduleMapPtr;

					std::string dir = wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(0), _excuteData).ToString();
					wiz::load_data::UserType* ut = nullptr;
					auto finded = wiz::load_data::UserType::Find(&global, dir);
					ut = finded.second[0];

					std::string condition = "TRUE";
					if (val->GetUserTypeListSize() >= 2) {
						condition = val->GetUserTypeList(1)->ToString();
					}
					wiz::load_data::LoadData::Remove(global, dir, ut->GetIListSize() - 1,  _excuteData);

					eventStack.top().userType_idx.top()++;
					break;
				}
				else if ("$push_front"sv == val->GetName()) {
					ExecuteData _excuteData; _excuteData.depth = excuteData.depth;
					_excuteData.chkInfo = true;
					_excuteData.info = eventStack.top();
					_excuteData.pObjectMap = objectMapPtr;
					_excuteData.pEvents = eventPtr;
					_excuteData.pModule = moduleMapPtr;

					std::string value;
					std::string dir;
					if (val->GetUserTypeList(0)->GetItemListSize() > 0) {
						dir = wiz::load_data::LoadData::ToBool4(nullptr, global, val->GetUserTypeList(0)->GetItemList(0), _excuteData).ToString();
					}
					else ///val->Ge
					{
						dir = wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(0), _excuteData).ToString();
					}


					value = wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(1), _excuteData).ToString();

					std::string condition = "TRUE";
					if (val->GetUserTypeListSize() >= 3) {
						condition = val->GetUserTypeList(2)->ToString();
					}
					wiz::load_data::LoadData::AddDataAtFront(global, dir, value,  _excuteData);

					eventStack.top().userType_idx.top()++;
					break;
				}
				else if ("$pop_front"sv == val->GetName()) {
					ExecuteData _excuteData; _excuteData.depth = excuteData.depth;
					_excuteData.chkInfo = true;
					_excuteData.info = eventStack.top();
					_excuteData.pObjectMap = objectMapPtr;
					_excuteData.pEvents = eventPtr;
					_excuteData.pModule = moduleMapPtr;

					std::string dir = wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(0), _excuteData).ToString();

					std::string condition = "TRUE";
					if (val->GetUserTypeListSize() >= 2) {
						condition = val->GetUserTypeList(1)->ToString();
					}
					wiz::load_data::LoadData::Remove(global, dir, 0,  _excuteData);

					eventStack.top().userType_idx.top()++;
					break;
				}
				/*
				else if ("$wait"sv == val->GetName()) {
				for (int i = 0; i < waits.size(); ++i) {
				waits[i]->join();
				delete waits[i];
				}
				waits.resize(0);

				eventStack.top().userType_idx.top()++;
				break;
				}
				*/
				
				else if ("$print_option"sv == val->GetName()) {
				ExecuteData _excuteData; _excuteData.depth = excuteData.depth;
					_excuteData.chkInfo = true;
					_excuteData.info = eventStack.top();
					_excuteData.pObjectMap = objectMapPtr;
					_excuteData.pEvents = eventPtr;
					_excuteData.pModule = moduleMapPtr;
					
					std::vector<std::string> data(val->GetItemListSize());
					for (long long i = 0; i < val->GetItemListSize(); ++i) {
						data[i] = val->GetItemList(i).ToString();
					}

					bool x = false , y = false;
					for (long long i = 0; i < data.size(); ++i) {
						if ("CONSOLE" == data[i]) {
							x = true;
						}
						else if ("FILE" == data[i]) {
							y = true;
						}
					}


					if (x && y) {
						wiz::Out.SetPolicy(2);
					}
					else if (x) {
						wiz::Out.SetPolicy(0);
					}
					else { // y
						wiz::Out.SetPolicy(1);
					}

					eventStack.top().userType_idx.top()++;
					break;
				}
				else if ("$print_file_option"sv == val->GetName()) {
					ExecuteData _excuteData; _excuteData.depth = excuteData.depth;
					_excuteData.chkInfo = true;
					_excuteData.info = eventStack.top();
					_excuteData.pObjectMap = objectMapPtr;
					_excuteData.pEvents = eventPtr;
					_excuteData.pModule = moduleMapPtr;

					std::string fileName = val->GetItemList(0).ToString();
					wiz::Out.SetFileName(fileName.substr(1, fileName.size() - 2));


					eventStack.top().userType_idx.top()++;
					break;
				}
				else if ("$print_file_clear"sv == val->GetName()) {
					ExecuteData _excuteData; _excuteData.depth = excuteData.depth;
					_excuteData.chkInfo = true;
					_excuteData.info = eventStack.top();
					_excuteData.pObjectMap = objectMapPtr;
					_excuteData.pEvents = eventPtr;
					_excuteData.pModule = moduleMapPtr;

					wiz::Out.clear_file();


					eventStack.top().userType_idx.top()++;
					break;
				}
				else if ("$call"sv == val->GetName()) {
					ExecuteData _excuteData; _excuteData.depth = excuteData.depth;
					_excuteData.chkInfo = true;
					_excuteData.info = eventStack.top();
					_excuteData.pObjectMap = objectMapPtr;
					_excuteData.pEvents = eventPtr;
					_excuteData.pModule = moduleMapPtr;

					if (!val->GetItem("id").empty()) {
						info.id = wiz::ToString(val->GetItem("id")[0].Get(0));
					}
					else {
						//// removal?
						info.id = wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeItem("id")[0], _excuteData).ToString();
					}

					info.eventUT = eventPtr->GetUserTypeList(no);
					info.userType_idx.clear();
					info.userType_idx.push(0);
					info.return_value.clear();
					info.nowUT.clear();

					EventInfo info2; //
					info2 = info;

					if (info.id != eventStack.top().id) {
						info.parameters.clear();
					}
					info.conditionStack.clear();

					//
					if (info.id != eventStack.top().id) {
						for (int j = 0; j < val->GetItemListSize(); ++j) {
							if (val->GetItemListSize() > 0) {
								_excuteData.info = info2;
								std::string temp = wiz::ToString(val->GetItemList(j).Get(0));
								auto temp2 = std::pair<std::string, std::string>(wiz::ToString(val->GetItemList(j).GetName()), temp);

								info.parameters.insert(temp2);
							}
						}
						for (int j = 0; j < val->GetUserTypeListSize(); ++j) {
							if (val->GetUserTypeListSize() > 0) {
								_excuteData.info = info2;
								std::string temp = wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(j), _excuteData).ToString();
								auto temp2 = std::pair<std::string, std::string>(wiz::ToString(val->GetUserTypeList(j)->GetName()), temp);
								info.parameters.insert(temp2);
							}
						}
						eventStack.top().userType_idx.top()++;
					}
					else { // recursive call!
						if (val->GetItemListSize() > 0) {
							for (int j = 0; j < val->GetItemListSize(); ++j) {
								_excuteData.info = info;
								_excuteData.info.parameters = info2.parameters;

								std::string temp = wiz::ToString(val->GetItemList(j).Get(0));

								wiz::ArrayMap<std::string, std::string>::iterator x;
								if ((x = info.parameters.find(wiz::ToString(val->GetItemList(j).GetName()))) != info.parameters.end())
								{
									x->second = temp;
								}
							}
						}
						if (val->GetUserTypeListSize() > 0) {
							for (int j = 0; j < val->GetUserTypeListSize(); ++j) {
								_excuteData.info = info;
								_excuteData.info.parameters = info2.parameters;

								std::string temp = wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(j), _excuteData).ToString();

								wiz::ArrayMap<std::string, std::string>::iterator x;
								if ((x = info.parameters.find(wiz::ToString(val->GetUserTypeList(j)->GetName()))) != info.parameters.end())
								{
									x->second = temp;
								}
							}
						}

						eventStack.top().userType_idx.top()++;

						// remove remove_now_event_stack_a?
						if ("REMOVE_IF_CALL_ONESELF_EVENT" == eventStack.top().option) //
						{
							eventStack.pop();
						}
					}

					if (false == eventStack.empty() && eventStack.top().option == "REMOVE_IF_CALL_ANY_EVENT")
					{
						eventStack.pop();
					}


					info.locals.clear();
					const int no = convert.at(info.id);
					for (int i = 0; i < eventPtr->GetUserTypeList(no)->GetUserTypeListSize(); ++i) {
						if (eventPtr->GetUserTypeList(no)->GetUserTypeList(i)->GetName() == "$local") {
							for (int j = 0; j < eventPtr->GetUserTypeList(no)->GetUserTypeList(i)->GetItemListSize(); ++j) {
								std::string name = wiz::ToString(eventPtr->GetUserTypeList(no)->GetUserTypeList(i)->GetItemList(j).Get(0));
								std::string value = "";
								std::pair<std::string, std::string> temp(name, value);
								info.locals.insert(temp);
							}
							break;
						}
					}
					/*
					if (waits.size() >= 4) {
					for (int i = 0; i < waits.size(); ++i) {
					waits[i]->join();
					delete waits[i]; // chk ?
					}
					waits.resize(0);
					}
					*/

					/*if (false == val->GetItem("option").empty() && val->GetItem("option")[0].Get(0) == "USE_THREAD") {
					_excuteData.info = info;

					_excuteData.noUseInput = excuteData.noUseInput;
					_excuteData.noUseOutput = excuteData.noUseOutput;

					std::thread* A = new std::thread(excute_module, "", &global, _excuteData, opt, 0);

					waits.push_back(A);
					}*/
					//else {
					eventStack.push(info);
					//}

					break;
				}
				else if ("$call_by_data"sv == val->GetName()) {
					ExecuteData _excuteData; _excuteData.depth = excuteData.depth;
					_excuteData.chkInfo = true;
					_excuteData.info = eventStack.top();
					_excuteData.pObjectMap = objectMapPtr;
					_excuteData.pEvents = eventPtr;
					_excuteData.pModule = moduleMapPtr;
					_excuteData.noUseInput = excuteData.noUseInput; //// check!
					_excuteData.noUseOutput = excuteData.noUseOutput;


					std::string dir = val->GetItemList(0).ToString();
					wiz::load_data::UserType subGlobal;
					wiz::load_data::LoadData::LoadDataFromString(global.GetUserTypeItem(dir)[0]->ToString(), subGlobal);

					{
						ExecuteData _excuteData;
						_excuteData.noUseInput = excuteData.noUseInput;
						_excuteData.noUseOutput = excuteData.noUseOutput;
						Option _opt;
						eventStack.top().return_value = excute_module("", &subGlobal, _excuteData, _opt, 0);  // return ?
					}

					eventStack.top().userType_idx.top()++;
					break;
				}
				else if ("$call_by_data2"sv == val->GetName()) {
				ExecuteData _excuteData; _excuteData.depth = excuteData.depth;
				_excuteData.chkInfo = true;
				_excuteData.info = eventStack.top();
				_excuteData.pObjectMap = objectMapPtr;
				_excuteData.pEvents = eventPtr;
				_excuteData.pModule = moduleMapPtr;
				_excuteData.noUseInput = excuteData.noUseInput; //// check!
				_excuteData.noUseOutput = excuteData.noUseOutput;


				std::string dir = wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(0), _excuteData).ToString();

				wiz::load_data::UserType subGlobal;
				wiz::load_data::LoadData::LoadDataFromString(global.GetUserTypeItem(dir)[0]->ToString(), subGlobal);

				{
					ExecuteData _excuteData;
					_excuteData.noUseInput = excuteData.noUseInput;
					_excuteData.noUseOutput = excuteData.noUseOutput;
					Option _opt;
					eventStack.top().return_value = excute_module("", &subGlobal, _excuteData, _opt, 0);  // return ?
				}

				eventStack.top().userType_idx.top()++;
				break;
				}
				//// no $parameter.~
				else if ("$assign"sv == val->GetName()) /// -> assign2?
				{
					ExecuteData _excuteData; _excuteData.depth = excuteData.depth;
					_excuteData.chkInfo = true;
					_excuteData.info = eventStack.top();
					_excuteData.pObjectMap = objectMapPtr;
					_excuteData.pEvents = eventPtr;
					_excuteData.pModule = moduleMapPtr;

					std::pair<std::string, std::string> dir = wiz::load_data::LoadData::Find2(&global, wiz::ToString(val->GetItemList(0).Get(0)));
					std::string data = wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(0), _excuteData).ToString();

					if (dir.first == "" && wiz::String::startsWith(dir.second, "$local."))
					{
						eventStack.top().locals[wiz::String::substring(dir.second, 7)] = data;
					}
					else {
						wiz::load_data::LoadData::SetData(global, dir.first, dir.second, data, _excuteData);
					}
					eventStack.top().userType_idx.top()++;
					break;
				}

				else if ("$assign2"sv == val->GetName())
				{
					ExecuteData _excuteData; _excuteData.depth = excuteData.depth;
					_excuteData.chkInfo = true;
					_excuteData.info = eventStack.top();
					_excuteData.pObjectMap = objectMapPtr;
					_excuteData.pEvents = eventPtr;
					_excuteData.pModule = moduleMapPtr;


					std::pair<std::string, std::string> dir = wiz::load_data::LoadData::Find2(&global, wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(0), _excuteData).ToString());
					std::string data = wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(1), _excuteData).ToString();

					{
						if (dir.first == "" && wiz::String::startsWith(dir.second, "$local."))
						{
							eventStack.top().locals[wiz::String::substring(dir.second, 7)] = data;
						}
						else {
							wiz::load_data::LoadData::SetData(global, dir.first, dir.second, data, _excuteData);
						}
					}

					eventStack.top().userType_idx.top()++;
					break;
				}
				else if ("$assign_local"sv == val->GetName()) /// no  
				{
					ExecuteData _excuteData; _excuteData.depth = excuteData.depth;
					_excuteData.chkInfo = true;
					_excuteData.info = eventStack.top();
					_excuteData.pObjectMap = objectMapPtr;
					_excuteData.pEvents = eventPtr;
					_excuteData.pModule = moduleMapPtr;

					std::pair<std::string, std::string> dir = wiz::load_data::LoadData::Find2(&global, wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(0), _excuteData).ToString());
					std::string data = wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(1), _excuteData).ToString();

					{
						if (dir.first == "" && dir.second.size() > 1 && dir.second[0] == '@')
						{
							dir.second.erase(dir.second.begin());
						}
						if (dir.first == "" && wiz::String::startsWith(dir.second, "$local."))
						{
							eventStack.top().locals[wiz::String::substring(dir.second, 7)] = data;
						}
						else {
							// throw error??
						}
					}

					eventStack.top().userType_idx.top()++;
					break;
				}
				else if ("$assign_global"sv == val->GetName()) // 二쇱쓽!! dir=> dir/name ( dir= { name = val } } , @瑜??욎뿉 遺숈뿬???쒕떎. 
				{
					ExecuteData _excuteData; _excuteData.depth = excuteData.depth;
					_excuteData.chkInfo = true;
					_excuteData.info = eventStack.top();
					_excuteData.pObjectMap = objectMapPtr;
					_excuteData.pEvents = eventPtr;
					_excuteData.pModule = moduleMapPtr;

					std::pair<std::string, std::string> dir = wiz::load_data::LoadData::Find2(&global, 
						wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(0), _excuteData).ToString());
					std::string data = wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(1), _excuteData).ToString();

					//std::string condition;
					//if (val->GetUserTypeListSize() >= 3) {
					//	condition = wiz::load_data::LoadData::ToBool4(nullptr, global, val->GetUserTypeList(2)->ToString(), _excuteData).ToString();
					//}
					wiz::load_data::LoadData::SetData(global, dir.first, dir.second, data, _excuteData);

					// chk local?

					eventStack.top().userType_idx.top()++;
					break;
				}

				else if ("$assign_from_ut"sv == val->GetName()) {
					ExecuteData _excuteData; _excuteData.depth = excuteData.depth;
					_excuteData.chkInfo = true;
					_excuteData.info = eventStack.top();
					_excuteData.pObjectMap = objectMapPtr;
					_excuteData.pEvents = eventPtr;
					_excuteData.pModule = moduleMapPtr;

					std::pair<std::string, std::string> dir = wiz::load_data::LoadData::Find2(&global, wiz::ToString(val->GetItemList(0).Get(0)));
					std::string ut_str = wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(0), _excuteData).ToString();
					wiz::load_data::UserType ut;
					wiz::load_data::LoadData::LoadDataFromString(ut_str, ut);

					// check! ExecuteData() ?
					std::string data = wiz::load_data::LoadData::ToBool4(nullptr, ut, *val->GetUserTypeList(1), _excuteData).ToString();

					if (dir.first == "" && wiz::String::startsWith(dir.second, "$local."))
					{
						eventStack.top().locals[wiz::String::substring(dir.second, 7)] = data;
					}
					else {
						wiz::load_data::LoadData::SetData(global, dir.first, dir.second, data, _excuteData);
					}
					eventStack.top().userType_idx.top()++;
					break;
				}

				/// cf) insert3? - any position?
				else if ("$push_back"sv == val->GetName() || "$insert"sv == val->GetName() || "$insert2"sv == val->GetName())
				{
					ExecuteData _excuteData; _excuteData.depth = excuteData.depth;
					_excuteData.chkInfo = true;
					_excuteData.info = eventStack.top();
					_excuteData.pObjectMap = objectMapPtr;
					_excuteData.pEvents = eventPtr;
					_excuteData.pModule = moduleMapPtr;

					std::string value;
					std::string dir;
					if (val->GetUserTypeList(0)->GetItemListSize() > 0) {
						dir = wiz::load_data::LoadData::ToBool4(nullptr, global, val->GetUserTypeList(0)->GetItemList(0), _excuteData).ToString();
					}
					else ///val->Ge
					{
						dir = wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(0), _excuteData).ToString();
					}

					value = wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(1), _excuteData).ToString();

					std::string condition = "TRUE";
					if (val->GetUserTypeListSize() >= 3) {
						condition = val->GetUserTypeList(2)->ToString();
					}
					
					wiz::load_data::LoadData::AddData(global, dir, value,  _excuteData);
					
					eventStack.top().userType_idx.top()++;
					break;
				}
				else if ("$insert_noname_usertype"sv == val->GetName())
				{
					ExecuteData _excuteData; _excuteData.depth = excuteData.depth;
					_excuteData.chkInfo = true;
					_excuteData.info = eventStack.top();
					_excuteData.pObjectMap = objectMapPtr;
					_excuteData.pEvents = eventPtr;
					_excuteData.pModule = moduleMapPtr;

					std::string position = wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(0), _excuteData).ToString();;
					std::string data = wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(1), _excuteData).ToString();;
					std::string condition = val->GetUserTypeList(2)->ToString();

					wiz::load_data::LoadData::AddNoNameUserType(global, position, data,  _excuteData);

					eventStack.top().userType_idx.top()++;
					break;
				}
				else if ("$insert_by_idx"sv == val->GetName())
				{
					ExecuteData _excuteData; _excuteData.depth = excuteData.depth;
					_excuteData.chkInfo = true;
					_excuteData.info = eventStack.top();
					_excuteData.pObjectMap = objectMapPtr;
					_excuteData.pEvents = eventPtr;
					_excuteData.pModule = moduleMapPtr;

					std::string value;
					int idx = atoi(wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(1), _excuteData).ToString().c_str());
					std::string dir;
					if (val->GetUserTypeList(0)->GetItemListSize() > 0) {
						dir = wiz::load_data::LoadData::ToBool4(nullptr, global, val->GetUserTypeList(0)->GetItemList(0), _excuteData).ToString();
					}
					else ///val->Ge
					{
						dir = wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(0), _excuteData).ToString();
					}

					value = wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(2), _excuteData).ToString();

					std::string condition = "TRUE";
					if (val->GetUserTypeListSize() >= 4) {
						condition = val->GetUserTypeList(3)->ToString();
					}
					wiz::load_data::LoadData::Insert(global, dir, idx, value,  _excuteData);

					eventStack.top().userType_idx.top()++;
					break;
				}
				else if ("$make"sv == val->GetName()) // To Do? - make2? or remake? 
													// cf) make empty ut??
				{
					ExecuteData _excuteData; _excuteData.depth = excuteData.depth;
					_excuteData.chkInfo = true;
					_excuteData.info = eventStack.top();
					_excuteData.pObjectMap = objectMapPtr;
					_excuteData.pEvents = eventPtr;
					_excuteData.pModule = moduleMapPtr;

					std::string dir;
					bool is2 = false;
					if (val->GetItemListSize() > 0) { // remove?
						dir = wiz::load_data::LoadData::ToBool4(nullptr, global, val->GetItemList(0), _excuteData).ToString();
					}
					else // 
					{
					//	std::cout << val->GetUserTypeList(0)->ToString() << "\n";
						dir = wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(0), _excuteData).ToString();
						is2 = true;
					}

					std::string name;
					for (int i = dir.size() - 1; i >= 0; --i)
					{
						if (dir[i] == '/') {
							name = wiz::String::substring(dir, i + 1);
							dir = wiz::String::substring(dir, 0, i - 1);
							break;
						}
					}
					if (dir.empty()) { dir = "."; }

					std::string condition = "TRUE";
					if (is2 && val->GetUserTypeListSize() >= 2) {
						condition = val->GetUserTypeList(1)->ToString();
					}
					else if (false == is2 && val->GetUserTypeListSize() >= 1) {
						condition = val->GetUserTypeList(0)->ToString();
					}

					wiz::load_data::LoadData::AddUserType(global, dir, name, "",  _excuteData);


					eventStack.top().userType_idx.top()++;
					break;
				}
				else if ("$findIndex"sv == val->GetName()) // For list : { 1 2  3 4 5 }
				{
					ExecuteData _excuteData; _excuteData.depth = excuteData.depth;
					_excuteData.chkInfo = true;
					_excuteData.info = eventStack.top();
					_excuteData.pObjectMap = objectMapPtr;
					_excuteData.pEvents = eventPtr;
					_excuteData.pModule = moduleMapPtr;

					std::string dir = wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(0), _excuteData).ToString();
					std::string value = wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(1), _excuteData).ToString();

					wiz::load_data::UserType ut;
					wiz::load_data::LoadData::LoadDataFromString(wiz::load_data::UserType::Find(&global, dir).second[0]->ToString(), ut);

					for (int i = 0; i < ut.GetItemListSize(); ++i) {
						if (wiz::ToString(ut.GetItemList(i).Get(0)) == value) {
							eventStack.top().return_value = wiz::toStr(i);
							break;
						}
					}

					eventStack.top().userType_idx.top()++;
					break;
				}
				else if ("$remove"sv == val->GetName()) // remove by dir., remove all?
				{
					ExecuteData _excuteData; _excuteData.depth = excuteData.depth;
					_excuteData.chkInfo = true;
					_excuteData.info = eventStack.top();
					_excuteData.pObjectMap = objectMapPtr;
					_excuteData.pEvents = eventPtr;
					_excuteData.pModule = moduleMapPtr;

					std::string dir;

					dir = wiz::load_data::LoadData::ToBool4(nullptr, global, val->GetItemList(0), _excuteData).ToString();

					std::string condition = "TRUE";
					if (val->GetUserTypeListSize() >= 1) {
						condition = val->GetUserTypeList(0)->ToString();
					}
					wiz::load_data::LoadData::Remove(global, dir,  _excuteData);

					eventStack.top().userType_idx.top()++;
					break;
				}
				else if ("$remove2"sv == val->GetName()) // remove /dir/name 
													   // if name is empty, then chk!!
				{
					ExecuteData _excuteData; _excuteData.depth = excuteData.depth;
					_excuteData.chkInfo = true;
					_excuteData.info = eventStack.top();
					_excuteData.pObjectMap = objectMapPtr;
					_excuteData.pEvents = eventPtr;
					_excuteData.pModule = moduleMapPtr;

					std::string dir; // item -> userType
					dir = wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(0), _excuteData).ToString();
					std::string name;
					for (int i = dir.size() - 1; i >= 0; --i)
					{
						if (dir[i] == '/') {
							name = wiz::String::substring(dir, i + 1);
							dir = wiz::String::substring(dir, 0, i - 1);
							break;
						}
					}

					std::string condition = "TRUE";
					if (val->GetUserTypeListSize() >= 2) {
						condition = val->GetUserTypeList(1)->ToString();
					}
					wiz::load_data::LoadData::Remove(global, dir, name,  _excuteData);

					eventStack.top().userType_idx.top()++;
					break;
				}
				else if ("$remove3"sv == val->GetName()) /// remove itemlist by idx.
				{
					ExecuteData _excuteData; _excuteData.depth = excuteData.depth;
					_excuteData.chkInfo = true;
					_excuteData.info = eventStack.top();
					_excuteData.pObjectMap = objectMapPtr;
					_excuteData.pEvents = eventPtr;
					_excuteData.pModule = moduleMapPtr;

					std::string dir = wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(0), _excuteData).ToString();
					std::string value = wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(1), _excuteData).ToString();

					int idx = atoi(value.c_str());  // long long -> int?

					std::string condition = "TRUE";

					if (val->GetUserTypeListSize() >= 3) {
						condition = val->GetUserTypeList(2)->ToString();
					}

					wiz::load_data::LoadData::Remove(global, dir, idx,  _excuteData);
					// remove -> UserType::Find(&global, dir).second[0]->RemoveItemList(idx); /// change ?

					eventStack.top().userType_idx.top()++;
					break;
				}

				else if ("$setElement"sv == val->GetName())
				{
					ExecuteData _excuteData; _excuteData.depth = excuteData.depth;
					_excuteData.chkInfo = true;
					_excuteData.info = eventStack.top();
					_excuteData.pObjectMap = objectMapPtr;
					_excuteData.pEvents = eventPtr;
					_excuteData.pModule = moduleMapPtr;

					std::string dir = wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(0), _excuteData).ToString();
					std::string idx = wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(1), _excuteData).ToString();
					std::string value = wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(2), _excuteData).ToString();

					int _idx = stoi(idx);
					wiz::load_data::UserType::Find(&global, dir).second[0]->SetItem(_idx, value);

					eventStack.top().userType_idx.top()++;
					break;
				}
				else if ("$swap"sv == val->GetName()) // $swap2
				{
					ExecuteData _excuteData; _excuteData.depth = excuteData.depth;
					_excuteData.chkInfo = true;
					_excuteData.info = eventStack.top();
					_excuteData.pObjectMap = objectMapPtr;
					_excuteData.pEvents = eventPtr;
					_excuteData.pModule = moduleMapPtr;

					std::string dir = std::string(wiz::ToString(wiz::ToString(val->GetItemList(0).Get(0)).c_str())); // + 0
					std::string value1;
					std::string value2;

					value1 = wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(0), _excuteData).ToString();
					value2 = wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(1), _excuteData).ToString();
					if (value1 != value2) {
						int x = atoi(value1.c_str());
						int y = atoi(value2.c_str());

						std::string temp = wiz::ToString(wiz::load_data::UserType::Find(&global, dir).second[0]->GetItemList(x).Get(0));
						std::string temp2 = wiz::ToString(wiz::load_data::UserType::Find(&global, dir).second[0]->GetItemList(y).Get(0));

						wiz::load_data::LoadData::SetData(global, dir, x, temp2, _excuteData);
						wiz::load_data::LoadData::SetData(global, dir, y, temp, _excuteData);
					}

					eventStack.top().userType_idx.top()++;
					break;
				}
				else if ("$print"sv == val->GetName()) /// has many bugs..!?, for print list or print item?.
				{
					if (excuteData.noUseOutput) {
						eventStack.top().userType_idx.top()++;
						break;
					}
					ExecuteData _excuteData; _excuteData.depth = excuteData.depth;
					_excuteData.chkInfo = true;
					_excuteData.info = eventStack.top();
					_excuteData.pObjectMap = objectMapPtr;
					_excuteData.pEvents = eventPtr;
					_excuteData.pModule = moduleMapPtr;

					if (val->GetUserTypeListSize() == 1
						&& val->GetUserTypeList(0)->GetItemListSize() == 1)
					{
						std::string listName = wiz::ToString(val->GetUserTypeList(0)->GetItemList(0).Get(0));

						if (listName.size() >= 2 && listName[0] == '\"' && listName.back() == '\"')
						{
							listName = wiz::String::substring(listName, 1, listName.size() - 2);
							/*std::string data;
							int count = 0;
							for (int _i = 0; _i < listName.size(); ++_i) {
								data += listName[_i];
								if (listName[_i] < 0) {
									count++;
									if (count == 2) {
										data += "\0";
										count = 0;
									}
								}
							}
							wiz::Out << data;*/
							wiz::Out << listName;
						}
						else if (listName.size() == 2 && listName[0] == '\\' && listName[1] == 'n')
						{
							wiz::Out << '\n';
						}
						else if (wiz::String::startsWith(listName, "$local.")
							|| wiz::String::startsWith(listName, "$parameter.")
							)
						{
							std::string temp = wiz::load_data::LoadData::ToBool4(nullptr, global, val->GetUserTypeList(0)->GetItemList(0), _excuteData).ToString();
							if (temp.empty()) {
								wiz::Out << "EMPTY";
							}
							else {
								wiz::Out << temp;
							}
						}
						else if (wiz::String::startsWith(listName, "/") && listName.size() > 1)
						{
							std::string temp = wiz::load_data::LoadData::ToBool4(nullptr, global, val->GetUserTypeList(0)->GetItemList(0), _excuteData).ToString();
							if (temp != listName) // chk 
							{
								wiz::Out << temp;
							}
							else {
								//std::cout << global.ToString() << "\n";
								wiz::load_data::UserType* ut = wiz::load_data::UserType::Find(&global, listName).second[0];
								if (ut->GetItemListSize() == 0 && wiz::ToString(ut->GetItemList(0).GetName()).empty()) {
									wiz::Out << wiz::ToString(ut->GetItemList(0).Get(0));
								}
							}
						}
						else
						{
							auto x = wiz::load_data::UserType::Find(&global, listName);
							if (x.first) {
								wiz::load_data::UserType* ut = x.second[0];
								wiz::Out << ut->ToString();
							}
							else {
								wiz::Out << listName;
							}
						}
					}
					// ?
					else if (val->GetUserTypeListSize() == 1
						&& val->GetUserTypeList(0)->GetItemListSize() == 0
						&& val->GetUserTypeList(0)->GetUserTypeListSize() == 1)
					{
						std::string name = wiz::load_data::LoadData::ToBool4(nullptr, global,
							*val->GetUserTypeList(0), _excuteData).ToString();
						wiz::Out << name;
					}
					else
					{
						std::string start;
						std::string last;

						start = wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(1), _excuteData).ToString();
						last = wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(2), _excuteData).ToString();

						std::string listName = wiz::ToString(val->GetUserTypeList(0)->GetItemList(0).Get(0));
						int _start = atoi(start.c_str());
						int _last = atoi(last.c_str());
						wiz::load_data::UserType* ut = wiz::load_data::UserType::Find(&global, listName).second[0];
						for (int i = _start; i <= _last; ++i)
						{
							if (i != _start) { wiz::Out << " "; }
							wiz::Out << wiz::ToString(ut->GetItemList(i).Get(0));
						}
					}

					eventStack.top().userType_idx.top()++;
					break;
				}
				else if ("$print2"sv == val->GetName()) /// for print usertype.ToString();
				{
					if (excuteData.noUseOutput) {
						eventStack.top().userType_idx.top()++;
						break;
					}
					ExecuteData _excuteData; _excuteData.depth = excuteData.depth;
					_excuteData.chkInfo = true;
					_excuteData.info = eventStack.top();
					_excuteData.pObjectMap = objectMapPtr;
					_excuteData.pEvents = eventPtr;
					_excuteData.pModule = moduleMapPtr;

					std::string dir = wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(0), _excuteData).ToString();
					auto x = wiz::load_data::UserType::Find(&global, dir);

					for (int idx = 0; idx < x.second.size(); ++idx) {
						wiz::Out << x.second[idx]->ToString();
						if (idx < x.second.size() - 1) {
							wiz::Out << "\n";
						}
					}

					eventStack.top().userType_idx.top()++;
					break;
				}
				// comment copy??
				else if ("$load"sv == val->GetName())
				{
					ExecuteData _excuteData; _excuteData.depth = excuteData.depth;
					_excuteData.chkInfo = true;
					_excuteData.info = eventStack.top();
					_excuteData.pObjectMap = objectMapPtr;
					_excuteData.pEvents = eventPtr;
					_excuteData.pModule = moduleMapPtr;

					// load data and events from other file! 
					// itemlist -> usertypelist.tostring?
					for (int i = 0; i < val->GetItemListSize(); ++i) {
						wiz::load_data::UserType ut;
						std::string fileName = wiz::ToString(val->GetItemList(i).Get(0));
						fileName = wiz::String::substring(fileName, 1, fileName.size() - 2);

						if (wiz::load_data::LoadData::LoadDataFromFile(fileName, ut)) {
							{
								int item_count = 0;
								int userType_count = 0;

								for (int i = 0; i < ut.GetIListSize(); ++i) {
									if (ut.IsItemList(i)) {
										global.AddItem(std::move(ut.GetItemList(item_count).GetName()),
											std::move(ut.GetItemList(item_count).Get(0)));
										item_count++;
									}
									else {
										global.AddUserTypeItem(std::move(*ut.GetUserTypeList(userType_count)));
										userType_count++;
									}
								}
							}

							auto _Main = ut.GetUserTypeItem("Main");
							if (!_Main.empty())
							{
								// error!
								wiz::Out << "err" << ENTER;

								return "ERROR -2"; /// exit?
							}
						}
						else {
							// error!
						}
					}

					{
						convert.clear();
						auto _events = global.GetCopyUserTypeItem("Event");
						for (int i = 0; i < _events.size(); ++i) {
							eventPtr->LinkUserType(_events[i]);
						}
						global.RemoveUserTypeList("Event");

						// event table setting
						for (int i = 0; i < eventPtr->GetUserTypeListSize(); ++i)
						{
							auto x = eventPtr->GetUserTypeList(i)->GetItem("id");
							if (!x.empty()) {
								//wiz::Out <<	x[0] << ENTER;
								auto temp = std::pair<std::string, int>(wiz::ToString(x[0].Get(0)), i);
								convert.insert(temp);
							}
							else {
								// error
							}
						}

						// update no
						no = convert[str];
					}

					eventStack.top().userType_idx.top()++;
					break;

				}
				else if ("$load_only_data"sv == val->GetName()) // $load2?
				{
					ExecuteData _excuteData; _excuteData.depth = excuteData.depth;
					_excuteData.chkInfo = true;
					_excuteData.info = eventStack.top();
					_excuteData.pObjectMap = objectMapPtr;
					_excuteData.pEvents = eventPtr;
					_excuteData.pModule = moduleMapPtr;

					// to do, load data and events from other file!
					wiz::load_data::UserType ut;
					std::string fileName = wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(0), _excuteData).ToString();
					
					if (fileName.back() == '\"' && fileName.size() >= 2 && fileName.back() == fileName[0]) {
						fileName = wiz::String::substring(fileName, 1, fileName.size() - 2);
					}
					std::string dirName = val->GetUserTypeList(1)->ToString();
					wiz::load_data::UserType* utTemp;


					if (dirName == "/./" || dirName == "root") {
						utTemp = &global;
					}
					else {
						dirName = wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(1), ExecuteData()).ToString();
						utTemp = global.GetUserTypeItem(dirName)[0];
					}

					if (wiz::load_data::LoadData::LoadDataFromFile(fileName, ut)) {
						{
							int item_count = 0;
							int userType_count = 0;

							for (int i = 0; i < ut.GetIListSize(); ++i) {
								if (ut.IsItemList(i)) {
									utTemp->AddItem(std::move(ut.GetItemList(item_count).GetName()),
										std::move(ut.GetItemList(item_count).Get(0)));
									item_count++;
								}
								else {
									utTemp->AddUserTypeItem(std::move(*ut.GetUserTypeList(userType_count)));
									userType_count++;
								}
							}
						}

						//	auto _Main = ut.GetUserTypeItem("Main");
						//	if (!_Main.empty())
						//	{
						// error!
						//		wiz::Out << "err" << ENTER;

						//			return "ERROR -2"; /// exit?
						//		}
					}
					else {
						// error!
					}

					eventStack.top().userType_idx.top()++;
					break;

				}
				else if ("$load_json"sv == val->GetName()) // $load2?
				{
					ExecuteData _excuteData; _excuteData.depth = excuteData.depth;
					_excuteData.chkInfo = true;
					_excuteData.info = eventStack.top();
					_excuteData.pObjectMap = objectMapPtr;
					_excuteData.pEvents = eventPtr;
					_excuteData.pModule = moduleMapPtr;

					// to do, load data and events from other file!
					wiz::load_data::UserType ut;
					std::string fileName = wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(0), _excuteData).ToString();
					fileName = wiz::String::substring(fileName, 1, fileName.size() - 2);
					std::string dirName = val->GetUserTypeList(1)->ToString();
					wiz::load_data::UserType* utTemp = global.GetUserTypeItem(dirName)[0];


					if (dirName == "/./" || dirName == "root") {
						utTemp = &global;
					}
					else {
						dirName = wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(1), ExecuteData()).ToString();
						utTemp = global.GetUserTypeItem(dirName)[0];
					}


					if (wiz::load_data::LoadData::LoadDataFromFileWithJson(fileName, ut)) {
						{
							//for (int i = 0; i < ut.GetCommentListSize(); ++i) {
							//	utTemp->PushComment(std::move(ut.GetCommentList(i)));
							//}

							wiz::load_data::UserType* _ut = ut.GetUserTypeList(0);

							int item_count = 0;
							int userType_count = 0;

							for (int i = 0; i < _ut->GetIListSize(); ++i) {
								if (_ut->IsItemList(i)) {
									utTemp->AddItem(std::move(_ut->GetItemList(item_count).GetName()),
										std::move(_ut->GetItemList(item_count).Get(0)));
									item_count++;
								}
								else {
									utTemp->AddUserTypeItem(std::move(*_ut->GetUserTypeList(userType_count)));
									userType_count++;
								}
							}
						}

						//	auto _Main = ut.GetUserTypeItem("Main");
						//	if (!_Main.empty())
						//	{
						// error!
						//		wiz::Out << "err" << ENTER;

						//			return "ERROR -2"; /// exit?
						//		}
					}
					else {
						// error!
					}

					eventStack.top().userType_idx.top()++;
					break;

				}
				else if ("$load_my_json_schema"sv == val->GetName()) {
					ExecuteData _excuteData; _excuteData.depth = excuteData.depth;
					_excuteData.chkInfo = true;
					_excuteData.info = eventStack.top();
					_excuteData.pObjectMap = objectMapPtr;
					_excuteData.pEvents = eventPtr;
					_excuteData.pModule = moduleMapPtr;

					// to do, load data and events from other file!
					wiz::load_data::UserType ut;
					std::string fileName = wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(0), _excuteData).ToString();
					fileName = wiz::String::substring(fileName, 1, fileName.size() - 2);
					std::string dirName = val->GetUserTypeList(1)->ToString();
					wiz::load_data::UserType* utTemp = global.GetUserTypeItem(dirName)[0];

					if (dirName == "/./" || dirName == "root") {
						utTemp = &global;
					}
					else {
						dirName = wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(1), ExecuteData()).ToString();
						utTemp = global.GetUserTypeItem(dirName)[0];
					}

					if (wiz::load_data::LoadData::LoadDataFromFileWithJsonSchema(fileName, ut)) {
						wiz::load_data::UserType* _ut = ut.GetUserTypeList(0);

						int item_count = 0;
						int userType_count = 0;

						for (int i = 0; i < _ut->GetIListSize(); ++i) {
							if (_ut->IsItemList(i)) {
								utTemp->AddItem(std::move(_ut->GetItemList(item_count).GetName()),
									std::move(_ut->GetItemList(item_count).Get(0)));
								item_count++;
							}
							else {
								utTemp->AddUserTypeItem(std::move(*_ut->GetUserTypeList(userType_count)));
								userType_count++;
							}
						}

					}
					else {
						// error!
					}

					eventStack.top().userType_idx.top()++;
					break;
				}
				else if ("$encoding_utf-8"sv == val->GetName()) {
#if _WIN32
					SetConsoleOutputCP(65001); // UTF-8 Codepage 
#endif

					eventStack.top().userType_idx.top()++;
					break;
				}
				else if ("$encoding_default"sv == val->GetName()) {
#if _WIN32
					SetConsoleOutputCP(wiz::load_data::Utility::defaultConsoleEncoding); // UTF-8 Codepage 
#endif
					eventStack.top().userType_idx.top()++;
					break;
				}
				else if ("$clear_screen"sv == val->GetName())
				{
					system("cls");
					eventStack.top().userType_idx.top()++;
					break;
				}
				else if ("$_getch"sv == val->GetName())
				{
					if (excuteData.noUseInput) {
						eventStack.top().userType_idx.top()++;
						break;
					}

					GETCH();
					eventStack.top().userType_idx.top()++;
					break;
				}
				else if ("$input"sv == val->GetName())
				{
					if (excuteData.noUseInput) {
						eventStack.top().userType_idx.top()++;
						break;
					}
					std::string str;
					std::cin >> str;
					eventStack.top().return_value = str;
					eventStack.top().userType_idx.top()++;
					break;
				}
				// line
				else if ("$input2"sv == val->GetName()) {
					if (excuteData.noUseInput) { // when no use input?
						eventStack.top().userType_idx.top()++;
						break;
					}
					std::string str;
					std::getline(std::cin, str);
					eventStack.top().return_value = str;
					eventStack.top().userType_idx.top()++;
					break;
				}
				else if ("$return"sv == val->GetName())
				{
					//// can $return = { a b c }
					ExecuteData _excuteData; _excuteData.depth = excuteData.depth;
					_excuteData.chkInfo = true;
					_excuteData.info = eventStack.top();
					_excuteData.pObjectMap = objectMapPtr;
					_excuteData.pEvents = eventPtr;
					_excuteData.pModule = moduleMapPtr;

					eventStack.top().userType_idx.top()++;
					if (eventStack.size() > 1)
					{
						std::string temp = wiz::load_data::LoadData::ToBool4(nullptr, global, *val, _excuteData).ToString();
						/// if temp just one
						eventStack[eventStack.size() - 2].return_value = temp;
					}

					if (eventStack.size() == 1)
					{
						std::string temp = wiz::load_data::LoadData::ToBool4(nullptr, global, *val, _excuteData).ToString();

						module_value = temp;
					}

					eventStack.pop();
					break;
				}
				
				else if ("$return_data"sv == val->GetName()) { // for functional programming??
					eventStack.top().userType_idx.top()++;

					if (eventStack.size() > 1)
					{
						std::string temp = val->ToString();
						/// if temp just one
						eventStack[eventStack.size() - 2].return_value = temp;
					}

					if (eventStack.size() == 1)
					{
						std::string temp = val->ToString();

						module_value = temp;
					}

					eventStack.top().userType_idx.top()++;
					break;
				}
				
				else if ("$parameter"sv == val->GetName())
				{
					eventStack.top().userType_idx.top()++;
					break;
				}
				else if ("$local"sv == val->GetName())
				{
					eventStack.top().userType_idx.top()++;
					break;
				}
				// make sort stable.
				else if ("$sort"sv == val->GetName()) {
					ExecuteData _excuteData; _excuteData.depth = excuteData.depth;
					_excuteData.chkInfo = true;
					_excuteData.info = eventStack.top();
					_excuteData.pObjectMap = objectMapPtr;
					_excuteData.pEvents = eventPtr;
					_excuteData.pModule = moduleMapPtr;

					std::vector<SortInfo> siVec;
					wiz::load_data::UserType* utTemp =
						wiz::load_data::UserType::Find(&global,
							wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(0), _excuteData).ToString()).second[0];

					std::vector<wiz::load_data::Type*> temp;


					int item_count = 0, ut_count = 0;
					for (int i = 0; i < utTemp->GetIListSize(); ++i) {
						if (utTemp->IsItemList(i)) {
							temp.push_back(&(utTemp->GetItemList(item_count)));
							siVec.emplace_back(wiz::ToString(utTemp->GetItemList(item_count).GetName()), 1, i);
							item_count++;
						}
						else {
							temp.push_back(utTemp->GetUserTypeList(ut_count));
							siVec.emplace_back(wiz::ToString(utTemp->GetUserTypeList(ut_count)->GetName()), 2, i);
							ut_count++;
						}
					}

					std::sort(siVec.begin(), siVec.end());


					wiz::load_data::UserType ut;
					for (int i = 0; i < temp.size(); ++i)
					{
						if (siVec[i].iElement == 1) {
							ut.AddItem(siVec[i].data, static_cast<wiz::load_data::ItemType<WIZ_STRING_TYPE>*>(temp[siVec[i].idx])->Get(0));
						}
						else {
							ut.AddUserTypeItem(*(static_cast<wiz::load_data::UserType*>(temp[siVec[i].idx])));
						}
					}

					utTemp->Remove();

					//cf) chk? *utTemp = ut;
					wiz::load_data::LoadData::AddData(*(utTemp), "", ut.ToString(), _excuteData);


					eventStack.top().userType_idx.top()++;
					break;
				}
				else if ("$sort2"sv == val->GetName()) { // colName -> just one! ? 
					ExecuteData _excuteData; _excuteData.depth = excuteData.depth;
					_excuteData.chkInfo = true;
					_excuteData.info = eventStack.top();
					_excuteData.pObjectMap = objectMapPtr;
					_excuteData.pEvents = eventPtr;
					_excuteData.pModule = moduleMapPtr;

					/// condition = has just one? in one usertype!
					std::vector<SortInfo> siVec;
					wiz::load_data::UserType* utTemp =
						wiz::load_data::UserType::Find(&global,
							wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(0), _excuteData).ToString()).second[0];
					const std::string colName = wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(1), _excuteData).ToString();

					std::vector<wiz::load_data::Type*> temp;


					int item_count = 0, ut_count = 0;
					for (int i = 0; i < utTemp->GetIListSize(); ++i) {
						if (utTemp->IsItemList(i)) {
							//
							item_count++;
						}
						else {
							temp.push_back(utTemp->GetUserTypeList(ut_count));
							if (utTemp->GetUserTypeList(ut_count)->GetItem(colName).empty())
							{
								siVec.emplace_back("", 2, ut_count);
							}
							else {
								siVec.emplace_back(wiz::ToString(utTemp->GetUserTypeList(ut_count)->GetItem(colName)[0].Get(0)), 2, ut_count);
							}
							ut_count++;
						}
					}

					std::sort(siVec.begin(), siVec.end());


					wiz::load_data::UserType ut;
					for (int i = 0; i < temp.size(); ++i)
					{
						if (siVec[i].iElement == 1) {
							//
						}
						else {
							ut.AddUserTypeItem(*(static_cast<wiz::load_data::UserType*>(temp[siVec[i].idx])));
						}
					}

					utTemp->RemoveUserTypeList();

					//cf) chk? *utTemp = ut;
					wiz::load_data::LoadData::AddData(*(utTemp), "", ut.ToString(), _excuteData);


					eventStack.top().userType_idx.top()++;
					break;
				}
				else if ("$sort2_dsc"sv == val->GetName()) { // colName -> just one! ? 
														   /// condition = has just one? in one usertype!
					ExecuteData _excuteData; _excuteData.depth = excuteData.depth;
					_excuteData.chkInfo = true;
					_excuteData.info = eventStack.top();
					_excuteData.pObjectMap = objectMapPtr;
					_excuteData.pEvents = eventPtr;
					_excuteData.pModule = moduleMapPtr;

					std::vector<SortInfo2> siVec;
					wiz::load_data::UserType* utTemp =
						wiz::load_data::UserType::Find(&global,
							wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(0), _excuteData).ToString()).second[0];
					const std::string colName = wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(1), _excuteData).ToString();
					// $it?
					std::vector<wiz::load_data::Type*> temp;


					int item_count = 0, ut_count = 0;
					for (int i = 0; i < utTemp->GetIListSize(); ++i) {
						if (utTemp->IsItemList(i)) {
							//
							item_count++;
						}
						else {
							temp.push_back(utTemp->GetUserTypeList(ut_count));
							if (utTemp->GetUserTypeList(ut_count)->GetItem(colName).empty())
							{
								siVec.emplace_back("", 2, ut_count);
							}
							else {
								siVec.emplace_back(wiz::ToString(utTemp->GetUserTypeList(ut_count)->GetItem(colName)[0].Get(0)), 2, ut_count);
							}
							ut_count++;
						}
					}

					std::sort(siVec.begin(), siVec.end());


					wiz::load_data::UserType ut;
					for (int i = 0; i < temp.size(); ++i)
					{
						if (siVec[i].iElement == 1) {
							//
						}
						else {
							ut.AddUserTypeItem(*(static_cast<wiz::load_data::UserType*>(temp[siVec[i].idx])));
						}
					}

					utTemp->RemoveUserTypeList();

					//cf) chk? *utTemp = ut;
					wiz::load_data::LoadData::AddData(*(utTemp), "", ut.ToString(), _excuteData);


					eventStack.top().userType_idx.top()++;
					break;
				}
				else if ("$shell_mode"sv == val->GetName()) {
					ExecuteData _excuteData; _excuteData.depth = excuteData.depth;
					_excuteData.chkInfo = true;
					_excuteData.info = eventStack.top();
					_excuteData.pObjectMap = objectMapPtr;
					_excuteData.pEvents = eventPtr;
					_excuteData.pModule = moduleMapPtr;

					// check?
					if (excuteData.noUseInput || excuteData.noUseOutput) {
						eventStack.top().userType_idx.top()++;
						break;
					}
					
					ShellMode(global);

					eventStack.top().userType_idx.top()++;
					break;
				}
				// removal?
				else if ("$stable_sort"sv == val->GetName()) {
					ExecuteData _excuteData; _excuteData.depth = excuteData.depth;
					_excuteData.chkInfo = true;
					_excuteData.info = eventStack.top();
					_excuteData.pObjectMap = objectMapPtr;
					_excuteData.pEvents = eventPtr;
					_excuteData.pModule = moduleMapPtr;

					// todo
					// todo
					std::vector<SortInfo> siVec;
					wiz::load_data::UserType* utTemp =
						wiz::load_data::UserType::Find(&global,
							wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(0), _excuteData).ToString()).second[0];

					std::vector<wiz::load_data::Type*> temp;


					int item_count = 0, ut_count = 0;
					for (int i = 0; i < utTemp->GetIListSize(); ++i) {
						if (utTemp->IsItemList(i)) {
							temp.push_back(&(utTemp->GetItemList(item_count)));
							siVec.emplace_back(wiz::ToString(utTemp->GetItemList(item_count).GetName()), 1, i);
							item_count++;
						}
						else {
							temp.push_back(utTemp->GetUserTypeList(ut_count));
							siVec.emplace_back(wiz::ToString(utTemp->GetUserTypeList(ut_count)->GetName()), 2, i);
							ut_count++;
						}
					}

					std::stable_sort(siVec.begin(), siVec.end());


					wiz::load_data::UserType ut;
					for (int i = 0; i < temp.size(); ++i)
					{
						if (siVec[i].iElement == 1) {
							ut.AddItem(siVec[i].data, static_cast<wiz::load_data::ItemType<std::string>*>(temp[siVec[i].idx])->Get(0));
						}
						else {
							ut.AddUserTypeItem(*(static_cast<wiz::load_data::UserType*>(temp[siVec[i].idx])));
						}
					}

					utTemp->Remove();

					wiz::load_data::LoadData::AddData(*(utTemp), "", ut.ToString(), _excuteData);


					eventStack.top().userType_idx.top()++;
					break;
				}
				else if ("$if"sv == val->GetName()) // ToDo!!
				{
					ExecuteData _excuteData; _excuteData.depth = excuteData.depth;
					_excuteData.chkInfo = true;
					_excuteData.info = eventStack.top();
					_excuteData.pObjectMap = objectMapPtr;
					_excuteData.pEvents = eventPtr;
					_excuteData.pModule = moduleMapPtr;

					std::string temp;

					/*
					std::pair<std::vector<std::string>, bool> x;
					if (_map.end() == _map.find(temp)) {
					x = wiz::load_data::LoadData::ToBool4_A(nullptr, global, temp, _excuteData);
					_map.insert(std::make_pair(temp, x));
					}
					else {
					x = _map[temp];
					}
					*/
					temp = wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(0), _excuteData).ToString();


					if (!eventStack.top().conditionStack.empty())
					{
						if ("TRUE" == temp && eventStack.top().conditionStack.top() == "FALSE")
						{
							temp = "FALSE";
						}
						else if ("FALSE" == temp && eventStack.top().conditionStack.top() == "FALSE")
						{
							temp = "FALSE";
						}
						else if (!eventStack.top().nowUT.empty() && eventStack.top().userType_idx.top() + 1 < eventStack.top().nowUT.top()->GetUserTypeListSize()
							&& (eventStack.top().nowUT.top()->GetUserTypeList(eventStack.top().userType_idx.top() + 1)->GetName() == "$else"))
						{
							eventStack.top().conditionStack.push(temp);
						}
						else if ("TRUE" == temp)
						{
							eventStack.top().conditionStack.push(temp);
						}
					}
					else
					{
						if (eventStack.top().userType_idx.top() + 1 < eventPtr->GetUserTypeList(no)->GetUserTypeListSize() &&
							eventPtr->GetUserTypeList(no)->GetUserTypeList(eventStack.top().userType_idx.top() + 1)->GetName() == "$else")
						{
							eventStack.top().conditionStack.push(temp);
						}
						else if ("TRUE" == temp)
						{
							eventStack.top().conditionStack.push(temp);
						}
					}

					if ("TRUE" == temp)
					{
						eventStack.top().nowUT.push(val->GetUserTypeList(1));
						//val = eventStack.top().nowUT.top()->GetUserTypeList(0); 
						eventStack.top().userType_idx.top()++;
						eventStack.top().userType_idx.push(0);
						//eventStack.top().state.push(1);
						//state = 1;
						break;
					}//
					else if ("FALSE" == temp)
					{
						eventStack.top().userType_idx.top()++;
						break;
					}
					else
					{
						// debug..
						wiz::Out << "Error Debug : " << temp << ENTER;
						return "ERROR -3";
					}
				}
				else if ("$else"sv == val->GetName())
				{
					if (!eventStack.top().conditionStack.empty() && "FALSE" == eventStack.top().conditionStack.top())
					{
						eventStack.top().conditionStack.top() = "TRUE";
						eventStack.top().nowUT.push(val->GetUserTypeList(0));
						//val = eventStack.top().nowUT.top()->GetUserTypeList(0); // empty chk
						eventStack.top().userType_idx.top()++;
						eventStack.top().userType_idx.push(0);
						//eventStack.top().state.push(2);
						//state = 2; //
						break;
					}
					else
					{
						eventStack.top().userType_idx.top()++;
						break;
					}
				}
				else { //
					wiz::Out << "it does not work. : " << wiz::ToString(val->GetName()) << ENTER;

					eventStack.top().userType_idx.top()++;
					break;
				}
			}
		}
	}
	/*
	for (int i = 0; i < waits.size(); ++i) {
	waits[i]->join();
	delete waits[i];
	}
	waits.resize(0);
	*/

	if (1 == chk && !events.empty()) {
		auto _events = events.GetUserTypeItem("Event");
		for (int i = 0; i < _events.size(); ++i) {
			_global->LinkUserType(_events[i]);
		}
		events.RemoveUserTypeList("Event", false);
	}
	return module_value;
}

// rename?
bool IsEmpty(std::vector<int>& chk_brace, const std::string& str)
{
	for (int i = 0; i < str.size(); ++i) {
		if ('{' == str[i]) {
			chk_brace.push_back(0);
		}
		else if ('}' == str[i]) {
			if (chk_brace.empty()) {
				chk_brace.push_back(1);
				return false;
			}
			else {
				chk_brace.pop_back();
			}
		}
	}

	return chk_brace.empty();
}
void SaveWithOutEvent(std::ostream& stream, wiz::load_data::UserType* ut, int depth)
{
	int itemListCount = 0;
	int userTypeListCount = 0;

	for (int i = 0; i < ut->GetIListSize(); ++i) {
		//wiz::Out << "ItemList" << endl;
		if (ut->IsItemList(i)) {
			for (int j = 0; j < ut->GetItemList(itemListCount).size(); j++) {
				std::string temp;
				for (int k = 0; k < depth; ++k) {
					temp += "\t";
				}
				if (ut->GetItemList(itemListCount).GetName() != "") {
					temp += wiz::ToString(ut->GetItemList(itemListCount).GetName());
					temp += "=";
				}
				temp += wiz::ToString(ut->GetItemList(itemListCount).Get(j));
				if (j != ut->GetItemList(itemListCount).size() - 1) {
					temp += "\n";
				}
				stream << temp;
			}
			if (i != ut->GetIListSize() - 1) {
				stream << "\n";
			}
			itemListCount++;
		}
		else if (ut->IsUserTypeList(i)) {
			if (ut->GetUserTypeList(userTypeListCount)->GetName() == "Event"
				|| ut->GetUserTypeList(userTypeListCount)->GetName() == "Main") {
				userTypeListCount++;
				continue;
			}

			// wiz::Out << "UserTypeList" << endl;
			for (int k = 0; k < depth; ++k) {
				stream << "\t";
			}

			if (ut->GetUserTypeList(userTypeListCount)->GetName() != "") {
				stream << wiz::ToString(ut->GetUserTypeList(userTypeListCount)->GetName()) << "=";
			}

			stream << "{\n";

			SaveWithOutEvent(stream, ut->GetUserTypeList(userTypeListCount), depth + 1);
			stream << "\n";

			for (int k = 0; k < depth; ++k) {
				stream << "\t";
			}
			stream << "}";
			if (i != ut->GetIListSize() - 1) {
				stream << "\n";
			}

			userTypeListCount++;
		}
	}
}

// check $~ ??
void ClauText::ShellMode(wiz::load_data::UserType& global) {
	std::vector<int> chk_brace;
	std::string command;
	std::string totalCommand;

	while (true)
	{
		wiz::Out << "<< : "; //
		std::getline(std::cin, command);

		if (command.empty()) {
			continue;
		}

		// Evnent = {
		//		$call = { id = 0 } # tab or 들여쓰기!!
		if (!command.empty() && '$' == command[0]) {
			if ("$print" == command) {
				wiz::Out << ">> : global" << ENTER;
				//cout << global.ToString() << endl;
				global.Save1(std::cout);
				wiz::Out << ENTER;
			}
			else if ("$print_data_only" == command) {
				wiz::Out << ">> : global" << ENTER;
				SaveWithOutEvent(std::cout, &global, 0);
				wiz::Out << ENTER;
			}
			else if ("$exit" == command) {
				break;
			}
			else if ("$option" == command) {

			}
			else if (wiz::String::startsWith(command, "$call"))
			{
				wiz::load_data::UserType test;

				try {
					if (wiz::load_data::LoadData::LoadDataFromString(command, test))
					{
						try {
							wiz::load_data::UserType ut = global;
							const std::string id = wiz::ToString(test.GetItemList(0).Get(0));
							Option opt;
							const std::string result = excute_module("Main = { $call = { id = " + id + "} }", &ut, ExecuteData(), opt, 1);

							global = std::move(ut);
							wiz::Out << ENTER << "excute result is : " << result << ENTER;
						}
						catch (...) // any exception..
						{
							wiz::Out << ">> : $call id or excute module error" << ENTER;
						}
					}
					else {
						wiz::Out << ">> : $call Error" << ENTER;
					}
				}
				catch (...) {
					wiz::Out << ">> : $call load data from string error" << ENTER;
				}
			}
			else if (wiz::String::startsWith(command, "$load"))
			{
				wiz::load_data::UserType test;

				if (wiz::load_data::LoadData::LoadDataFromString(command, test))
				{
					try {
						const std::string name = wiz::ToString(test.GetItemList(0).Get(0));
						const std::string result = wiz::String::substring(name, 1, name.size() - 2);

						if (wiz::load_data::LoadData::LoadDataFromFile(result, global)) {}
						else {
							wiz::Out << ">> : load data from file error" << ENTER;
						}
					}
					catch (...) // any exception..
					{
						wiz::Out << ">> : load error" << ENTER;
					}
				}
				else {
					wiz::Out << ">> : $load syntax Error" << ENTER;
				}
			}
			else if (wiz::String::startsWith(command, "$save_event_only"))
			{
				wiz::load_data::UserType test;

				if (wiz::load_data::LoadData::LoadDataFromString(command, test))
				{
					std::ofstream outFile;

					try {
						const std::string name = wiz::ToString(test.GetItemList(0).Get(0));
						const std::string result = wiz::String::substring(name, 1, name.size() - 2);

						outFile.open(result);
						if (outFile.fail()) {
							//
						}
						else {
							for (int i = 0; i < global.GetUserTypeListSize(); ++i) {
								if (global.GetUserTypeList(i)->GetName() == "Event") {
									outFile << "Event = {\n";
									global.GetUserTypeList(i)->Save1(outFile, 1);
									outFile << "\n}\n";
								}
							}
							outFile.close();
						}
					}
					catch (...) // any exception..
					{
						if (outFile.is_open()) {
							outFile.close();
						}
						wiz::Out << ">> : $save_event_only error" << ENTER;
					}
				}
				else {
					wiz::Out << ">> : $save_event_only syntax Error" << ENTER;
				}
			}

			else if (wiz::String::startsWith(command, "$save_data_only"))
			{
				wiz::load_data::UserType test;

				if (wiz::load_data::LoadData::LoadDataFromString(command, test))
				{
					std::ofstream outFile;

					try {
						const std::string name = wiz::ToString(test.GetItemList(0).Get(0));
						const std::string result = wiz::String::substring(name, 1, name.size() - 2);

						outFile.open(result);
						if (outFile.fail()) {
							//
						}
						else {
							SaveWithOutEvent(outFile, &global, 0);
							outFile.close();
						}
					}
					catch (...) // any exception..
					{
						if (outFile.is_open()) {
							outFile.close();
						}
						wiz::Out << ">> : $save_data_only error" << ENTER;
					}
				}
			}
			else if (wiz::String::startsWith(command, "$save"))
			{
				wiz::load_data::UserType test;
				if (wiz::load_data::LoadData::LoadDataFromString(command, test))
				{
					const std::string name = wiz::ToString(test.GetItemList(0).Get(0));
					const std::string result = wiz::String::substring(name, 1, name.size() - 2);

					if (wiz::load_data::LoadData::SaveWizDB(global, result, "1")) {

					}
					else {
						wiz::Out << ">> : $save error" << ENTER;
					}
				}
				else {
					wiz::Out << ">> : $save syntax Error" << ENTER;
				}
			}
			else if ("$cls" == command) {
				system("cls"); // for windows!
			}
		}
		else {
			if (IsEmpty(chk_brace, command)) {
				command.append("\n");

				totalCommand.append(command);
				if (wiz::load_data::LoadData::LoadDataFromString(totalCommand, global)) {
					wiz::Out << ">> : Data Added!" << ENTER;
				}
				else {
					wiz::Out << ">> : Error : loaddata from string " << ENTER;
				}
				command = "";
				totalCommand = "";
			}
			else {
				if (chk_brace[0] == 1) {
					wiz::Out << ">> : Error in command, reset command" << ENTER;
					totalCommand = "";
					command = "";
					chk_brace.clear();
				}
				else {
					command.append("\n");

					totalCommand.append(command);
					command = "";
				}
			}
		}
	}
}

}

