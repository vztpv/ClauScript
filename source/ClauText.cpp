#define _CRT_SECURE_NO_WARNINGS

#include "wiz/ClauText.h"
using namespace std::literals;

#include "wiz/smart_ptr.h"

namespace wiz {


	class UtInfo {
	public:
		wiz::SmartPtr<wiz::load_data::UserType> global;
		wiz::load_data::UserType* ut;
		std::string dir;
		long long itCount = 0;
		long long utCount = 0;
		long long count = 0;
	public:
		UtInfo(wiz::load_data::UserType* global, wiz::load_data::UserType* ut, const std::string& dir, long long itCount = 0, long long utCount = 0)
			: global(global), ut(ut), itCount(itCount), utCount(utCount), count(0), dir(dir)
		{
			//
		}
	};

	// for @insert, @update, @delete
	inline bool EqualFunc(wiz::load_data::UserType* global, wiz::load_data::UserType* eventUT, const wiz::load_data::ItemType<std::string>& x,
		wiz::load_data::ItemType<std::string> y, long long x_idx, const std::string& dir) {

		auto x_name = x.GetName();
		auto x_value = x.Get();

		bool use_not = false;
		if (wiz::String::startsWith(y.Get(), "!")) {
			use_not = true;
			y.Set(0, y.Get().substr(1));
		}

		{
			std::string name = y.GetName();

			if (wiz::String::startsWith(name, "&"sv) && name.size() >= 2) {
				long long idx = std::stoll(name.substr(1));
				if (idx < 0 || idx >= global->GetItemListSize()) {
					return false;
				}

				if (y.Get() == "%any"sv) {
					return !use_not;
				}

				x_idx = idx;
				x_name = global->GetItemList(idx).GetName();
				x_value = global->GetItemList(idx).Get();
			}
		}

		if (y.Get() == "%any"sv) {
			return !use_not;
		}

		if (wiz::String::startsWith(y.Get(), "%event_"sv)) {
			std::string event_id = y.Get().substr(7);

			wiz::ClauText clautext;
			wiz::ExecuteData executeData;
			executeData.pEvents = eventUT;
			executeData.noUseInput = true;
			executeData.noUseOutput = true;
			wiz::Option option;

			std::string statements;

			// do not use NONE!(in user?)
			statements += " Event = { id = NONE  ";
			statements += " $call = { id = " + event_id + " ";
			statements += " name = " + x_name.empty() ? "EMPTY_NAME" : x_name + " ";
			statements += " value = " + x_value + " ";
			statements += " is_user_type = FALSE ";
			//statements += " real_dir =  " + wiz::load_data::LoadData::GetRealDir(dir, global) + " ";
			statements += " relative_dir = " + dir + " ";
			statements += " idx = " + std::to_string(x_idx) + " "; // removal?
			statements += " } ";

			statements += " $return = { $return_value = { } }  ";
			statements += " } ";
			std::string result = clautext.execute_module(" Main = { $call = { id = NONE  } } " + statements, global, executeData, option, 1);

			bool success = false;
			if (result == "TRUE"sv) {
				success = true;
			}

			if (use_not) {
				return !success;
			}
			return success;
		}

		if (use_not) {
			return x_value != y.Get();
		}
		return x_value == y.Get();
	}


	bool _InsertFunc(wiz::SmartPtr<wiz::load_data::UserType> global, wiz::load_data::UserType* insert_ut, wiz::load_data::UserType* eventUT) {
		std::queue<UtInfo> que;

		std::string dir = "/.";

		que.push(UtInfo(global, insert_ut, dir));

		while (!que.empty()) {
			UtInfo x = que.front();
			que.pop();

			// find non-@
			long long ut_count = 0;
			long long it_count = 0;
			long long count = 0;

			for (long long i = 0; i < x.ut->GetIListSize(); ++i) {
				if (x.ut->IsItemList(i) && x.ut->GetItemList(it_count).GetName().empty()
					&& !wiz::String::startsWith(x.ut->GetItemList(it_count).Get(), "@"sv)) {
					// chk exist all case of value?
					auto item = x.global->GetItemIdx("");
					// no exist -> return false;
					if (item.empty()) {
						// LOG....
						return false;
					}

					bool pass = false;
					for (long long j = 0; j < item.size(); ++j) {
						if (EqualFunc(x.global, eventUT, x.global->GetItemList(item[j]), x.ut->GetItemList(it_count), item[j],
							x.dir)) {
							pass = true;
							break;
						}
					}
					if (!pass) {
						return false;
					}
				}
				else if (x.ut->IsItemList(i) && !x.ut->GetItemList(it_count).GetName().empty() && !wiz::String::startsWith(x.ut->GetItemList(it_count).GetName(), "@"sv)) {
					// chk exist all case of value?
					auto item = x.global->GetItemIdx(x.ut->GetItemList(it_count).GetName());
					// no exist -> return false;
					if (item.empty()) {
						// LOG....
						return false;
					}

					bool pass = false;

					for (long long j = 0; j < item.size(); ++j) {
						if (EqualFunc(x.global, eventUT, x.global->GetItemList(item[j]), x.ut->GetItemList(it_count), item[j],
							x.dir)) {
							pass = true;
							break;
						}
					}
					if (!pass) {
						return false;
					}

				}
				else if (x.ut->IsUserTypeList(i) && !wiz::String::startsWith(x.ut->GetUserTypeList(ut_count)->GetName(), "@"sv)) {
					// chk all case exist of @.
					// no exist -> return false;
					if (wiz::String::startsWith(x.ut->GetUserTypeList(ut_count)->GetName(), "$"sv)) {
						ut_count++;
						count++;
						continue;
					}

					auto usertype = x.global->GetUserTypeItemIdx(x.ut->GetUserTypeList(ut_count)->GetName());

					if (usertype.empty()) {
						return false;
					}

					ut_count++;
					count++;

					for (long long j = 0; j < usertype.size(); ++j) {
						que.push(UtInfo(x.global->GetUserTypeList(usertype[j]), x.ut->GetUserTypeList(ut_count - 1),
							x.dir));
					}

					continue;
				}

				if (x.ut->IsItemList(i)) {
					it_count++;
				}
				else {
					ut_count++;
				}
				count++;
			}
		}

		return true;
	}

	bool _RemoveFunc(wiz::SmartPtr<wiz::load_data::UserType> global, wiz::load_data::UserType* insert_ut, wiz::load_data::UserType* eventUT) {
		std::queue<UtInfo> que;
		std::string dir = "/.";
		que.push(UtInfo(global, insert_ut, dir));

		while (!que.empty()) {
			UtInfo x = que.front();
			que.pop();

			// find non-@
			long long ut_count = 0;
			long long it_count = 0;
			long long count = 0;

			for (long long i = 0; i < x.ut->GetIListSize(); ++i) {
				if (x.ut->IsItemList(i) && x.ut->GetItemList(it_count).GetName().empty()
					&& !wiz::String::startsWith(x.ut->GetItemList(it_count).Get(), "@"sv)) {
					// chk exist all case of value?
					auto item = x.global->GetItemIdx("");
					// no exist -> return false;
					if (item.empty()) {
						// LOG....
						return false;
					}

					bool pass = false;
					for (long long j = 0; j < item.size(); ++j) {
						if (EqualFunc(x.global, eventUT, x.global->GetItemList(item[j]), x.ut->GetItemList(it_count), item[j],
							x.dir)) {
							pass = true;
							break;
						}
					}
					if (!pass) {
						return false;
					}
				}
				else if (x.ut->IsItemList(i) && !x.ut->GetItemList(it_count).GetName().empty() && !wiz::String::startsWith(x.ut->GetItemList(it_count).GetName(), "@"sv)) {
					// chk exist all case of value?
					auto item = x.global->GetItemIdx(x.ut->GetItemList(it_count).GetName());
					// no exist -> return false;
					if (item.empty()) {
						if (x.ut->GetItemList(it_count).Get() == "!%any") {
							return true;
						}
						// LOG....
						return false;
					}

					bool pass = false;

					for (long long j = 0; j < item.size(); ++j) {
						if (EqualFunc(x.global, eventUT, x.global->GetItemList(item[j]), x.ut->GetItemList(it_count), item[j],
							x.dir)) {
							pass = true;
							break;
						}
					}
					if (!pass) {
						return false;
					}

				}
				else if (x.ut->IsUserTypeList(i) && !wiz::String::startsWith(x.ut->GetUserTypeList(ut_count)->GetName(), "@"sv)) {
					// chk all case exist of @.
					// no exist -> return false;
					if (wiz::String::startsWith(x.ut->GetUserTypeList(ut_count)->GetName(), "$"sv)) {
						ut_count++;
						count++;
						continue;
					}

					auto usertype = x.global->GetUserTypeItemIdx(x.ut->GetUserTypeList(ut_count)->GetName());

					if (usertype.empty()) {
						return false;
					}

					ut_count++;
					count++;

					for (long long j = 0; j < usertype.size(); ++j) {
						que.push(UtInfo(x.global->GetUserTypeList(usertype[j]), x.ut->GetUserTypeList(ut_count - 1),
							x.dir + "/$ut" + std::to_string(usertype[j])));
					}

					continue;
				}
				else if (x.ut->IsUserTypeList(i) && x.ut->GetUserTypeList(ut_count)->GetName() == "@$"sv) {
					//
				}

				if (x.ut->IsItemList(i)) {
					it_count++;
				}
				else {
					ut_count++;
				}
				count++;
			}
		}

		return true;
	}


	bool _UpdateFunc(wiz::SmartPtr<wiz::load_data::UserType> global, wiz::load_data::UserType* insert_ut, wiz::load_data::UserType* eventUT) {
		std::queue<UtInfo> que;
		std::string dir = "/.";
		que.push(UtInfo(global, insert_ut, dir));

		while (!que.empty()) {
			UtInfo x = que.front();
			que.pop();


			// find non-@
			long long ut_count = 0;
			long long it_count = 0;
			long long count = 0;

			for (long long i = 0; i < x.ut->GetIListSize(); ++i) {
				if (x.ut->IsItemList(i) && x.ut->GetItemList(it_count).GetName().empty()
					&& !wiz::String::startsWith(x.ut->GetItemList(it_count).Get(), "@"sv)) {
					// chk exist all case of value?
					auto item = x.global->GetItemIdx("");
					// no exist -> return false;
					if (item.empty()) {
						// LOG....
						return false;
					}

					bool pass = false;
					for (long long j = 0; j < item.size(); ++j) {
						if (EqualFunc(x.global, eventUT, x.global->GetItemList(item[j]), x.ut->GetItemList(it_count), item[j],
							x.dir)) {
							pass = true;
							break;
						}
					}
					if (!pass) {
						return false;
					}
				}
				else if (x.ut->IsItemList(i) && !x.ut->GetItemList(it_count).GetName().empty() && !wiz::String::startsWith(x.ut->GetItemList(it_count).GetName(), "@"sv)) {
					// chk exist all case of value?
					auto item = x.global->GetItemIdx(x.ut->GetItemList(it_count).GetName());
					// no exist -> return false;
					if (item.empty()) {
						// LOG....
						return false;
					}

					bool pass = false;

					for (long long j = 0; j < item.size(); ++j) {
						if (EqualFunc(x.global, eventUT, x.global->GetItemList(item[j]), x.ut->GetItemList(it_count), item[j],
							dir)) {
							pass = true;
							break;
						}
					}
					if (!pass) {
						return false;
					}

				}
				else if (x.ut->IsUserTypeList(i) && !wiz::String::startsWith(x.ut->GetUserTypeList(ut_count)->GetName(), "@"sv)) {
					// chk all case exist of @.
					// no exist -> return false;
					if (wiz::String::startsWith(x.ut->GetUserTypeList(ut_count)->GetName(), "$"sv)) {
						ut_count++;
						count++;
						continue;
					}

					auto usertype = x.global->GetUserTypeItemIdx(x.ut->GetUserTypeList(ut_count)->GetName());

					if (usertype.empty()) {
						return false;
					}

					ut_count++;
					count++;

					for (long long j = 0; j < usertype.size(); ++j) {
						que.push(UtInfo(x.global->GetUserTypeList(usertype[j]), x.ut->GetUserTypeList(ut_count - 1),
							dir + "$ut" + std::to_string(usertype[j])));
					}

					continue;
				}

				if (x.ut->IsItemList(i)) {
					it_count++;
				}
				else {
					ut_count++;
				}
				count++;
			}
		}

		return true;
	}

	// starts with '@' -> insert target
	// else -> condition target.
	bool InsertFunc(wiz::SmartPtr<wiz::load_data::UserType> global, wiz::load_data::UserType* insert_ut, wiz::load_data::UserType* eventUT) {
		if (!_InsertFunc(global, insert_ut, eventUT)) {
			return false;
		}
		std::string dir = "/.";
		std::queue<UtInfo> que;
		// insert
		que.push(UtInfo(global, insert_ut, dir));

		while (!que.empty()) {
			UtInfo x = que.front();
			que.pop();

			// find non-@
			long long ut_count = 0;
			long long it_count = 0;
			long long count = 0;

			//chk only @  ! - todo
			for (long long i = 0; i < x.ut->GetIListSize(); ++i) {
				if (x.ut->IsItemList(i) && x.ut->GetItemList(it_count).GetName().empty()
					&& wiz::String::startsWith(x.ut->GetItemList(it_count).Get(), "@"sv)) {

					if (wiz::String::startsWith(x.ut->GetItemList(it_count).Get(), "@%event_"sv)) {
						std::string event_id = x.ut->GetItemList(it_count).Get().substr(8);

						wiz::ClauText clautext;
						wiz::ExecuteData executeData;
						executeData.pEvents = insert_ut->GetParent();
						executeData.noUseInput = true;
						executeData.noUseOutput = true;
						wiz::Option option;

						std::string statements;

						// do not use NONE!(in user?)
						statements += " Event = { id = NONE  ";
						statements += " $call = { id = " + event_id + " ";
						statements += " name = EMPTY_STRING ";
						statements += " is_user_type = FALSE ";
						statements += " } ";

						statements += " $return = { $return_value = { } }  ";
						statements += " } ";
						std::string result = clautext.execute_module(" Main = { $call = { id = NONE  } } " + statements, x.global, executeData, option, 1);

						x.global->AddItem("", result);
					}
					else {
						x.global->AddItem("", x.ut->GetItemList(it_count).Get().substr(1));
					}

					it_count++;
				}
				else if (x.ut->IsItemList(i) && wiz::String::startsWith(x.ut->GetItemList(it_count).GetName(), "@"sv)) {
					if (wiz::String::startsWith(x.ut->GetItemList(it_count).Get(), "%event_"sv)) {
						std::string event_id = x.ut->GetItemList(it_count).Get().substr(7);

						wiz::ClauText clautext;
						wiz::ExecuteData executeData;
						executeData.pEvents = insert_ut->GetParent();
						executeData.noUseInput = true;
						executeData.noUseOutput = true;
						wiz::Option option;

						std::string statements;

						// do not use NONE!(in user?)
						statements += " Event = { id = NONE  ";
						statements += " $call = { id = " + event_id + " ";
						statements += " name =  " + x.ut->GetItemList(it_count).GetName() + " ";
						statements += " is_user_type = FALSE ";
						statements += " } ";

						statements += " $return = { $return_value = { } }  ";
						statements += " } ";
						std::string result = clautext.execute_module(" Main = { $call = { id = NONE  } } " + statements, x.global, executeData, option, 1);

						x.global->AddItem(
							x.ut->GetItemList(it_count).GetName().substr(1),
							result);
					}
					else {
						x.global->AddItem(
							x.ut->GetItemList(it_count).GetName().substr(1),
							x.ut->GetItemList(it_count).Get());
					}
					it_count++;
				}
				else if (x.ut->IsUserTypeList(i) && wiz::String::startsWith(x.ut->GetUserTypeList(ut_count)->GetName(), "@"sv)) {
					x.global->LinkUserType(x.ut->GetUserTypeList(ut_count));
					x.ut->GetUserTypeList(ut_count)->SetName(x.ut->GetUserTypeList(ut_count)->GetName().substr(1));
					x.ut->GetUserTypeList(ut_count) = nullptr;

					x.ut->RemoveUserTypeList(ut_count);
					count--;
					i--;
				}
				else if (x.ut->IsUserTypeList(i) && !wiz::String::startsWith(x.ut->GetUserTypeList(ut_count)->GetName(), "@"sv)) {
					if (wiz::String::startsWith(x.ut->GetUserTypeList(ut_count)->GetName(), "$"sv)) {
						auto temp = x.ut->GetUserTypeList(ut_count)->GetName();
						auto name = temp.substr(1);

						if (name.empty()) {

							for (long long j = 0; j < x.global->GetUserTypeListSize(); ++j) {
								if (_InsertFunc(x.global->GetUserTypeList(j), x.ut->GetUserTypeList(ut_count), eventUT)) {
									que.push(UtInfo(x.global->GetUserTypeList(j), x.ut->GetUserTypeList(ut_count)
										, x.dir + "/$ut" + std::to_string(j)
									));
								}
							}
						}
						else {

							auto usertype = x.global->GetUserTypeItemIdx(name);

							for (long long j = 0; j < usertype.size(); ++j) {
								if (_InsertFunc(x.global->GetUserTypeList(usertype[j]), x.ut->GetUserTypeList(ut_count), eventUT)) {
									que.push(UtInfo(x.global->GetUserTypeList(usertype[j]), x.ut->GetUserTypeList(ut_count),
										x.dir + "/$ut" + std::to_string(usertype[j])));
								}
							}
						}
					}
					else {
						auto usertype = x.global->GetUserTypeItemIdx(x.ut->GetUserTypeList(ut_count)->GetName());

						for (long long j = 0; j < usertype.size(); ++j) {
							//if (_InsertFunc(x.global->GetUserTypeList(usertype[j]), x.ut->GetUserTypeList(ut_count), eventUT)) {
							que.push(UtInfo(x.global->GetUserTypeList(usertype[j]), x.ut->GetUserTypeList(ut_count),
								x.dir + "/$ut" + std::to_string(usertype[j])));
							//}
						}
					}
					ut_count++;
				}
				else if (x.ut->IsUserTypeList(i)) {
					ut_count++;
				}
				else {
					it_count++;
				}

				count++;
			}
		}

		return true;
	}

	bool RemoveFunc(wiz::SmartPtr<wiz::load_data::UserType> global, wiz::load_data::UserType* insert_ut, wiz::load_data::UserType* eventUT) {
		if (!_RemoveFunc(global, insert_ut, eventUT)) {
			return false;
		}
		std::string dir = "/.";
		std::queue<UtInfo> que;
		// insert
		que.push(UtInfo(global, insert_ut, dir));

		while (!que.empty()) {
			UtInfo x = que.front();
			que.pop();

			// find non-@
			long long ut_count = x.ut->GetUserTypeListSize() - 1;
			long long it_count = x.ut->GetItemListSize() - 1;
			long long count = x.ut->GetIListSize() - 1;

			//chk only @  ! - todo
			for (long long i = x.ut->GetIListSize() - 1; i >= 0; --i) {
				if (x.ut->IsItemList(i) && x.ut->GetItemList(it_count).GetName().empty()
					&& wiz::String::startsWith(x.ut->GetItemList(it_count).Get(), "@"sv)) {

					if (wiz::String::startsWith(x.ut->GetItemList(it_count).Get(), "@%event_"sv)) {
						std::string event_id = x.ut->GetItemList(it_count).Get().substr(8);

						wiz::ClauText clautext;
						wiz::ExecuteData executeData;
						executeData.pEvents = insert_ut->GetParent();
						executeData.noUseInput = true;
						executeData.noUseOutput = true;
						wiz::Option option;

						wiz::load_data::UserType callUT;
						std::string statements;

						statements += " Event = { id = NONE  ";
						statements += " $call = { id = " + event_id + " ";
						statements += " name = _name  ";
						statements += " value = _value ";
						statements += " is_user_type = FALSE ";
						statements += " real_dir =  _real_dir "; //+wiz::load_data::LoadData::GetRealDir(dir, global) + " ";
						statements += " relative_dir = _dir "; // +dir + " ";
						statements += " idx = _idx "; // +std::to_string(x_idx) + " "; // removal?
						statements += " } ";

						statements += " $return = { $return_value = { } }  ";
						statements += " } ";

						wiz::load_data::LoadData::LoadDataFromString(statements, callUT);

						auto temp = x.global->GetItemIdx("");

						for (long long j = 0; j < temp.size(); ++j) {
							auto callInfo = wiz::load_data::UserType::Find(&callUT, "/./Event/$call");

							callInfo.second[0]->SetItem("name", "$NO_NAME");
							callInfo.second[0]->SetItem("value", x.global->GetItemList(temp[j]).Get());
							callInfo.second[0]->SetItem("real_dir", wiz::load_data::LoadData::GetRealDir(x.dir, x.global));
							callInfo.second[0]->SetItem("relative_dir", x.dir);
							callInfo.second[0]->SetItem("idx", std::to_string(temp[j]));

							std::string result = clautext.execute_module(" Main = { $call = { id = NONE  } } " + callUT.ToString(), x.global,
								executeData, option, 1);

							if (result == "TRUE"sv) { //x.ut->GetItemList(it_count).Get().substr(1)) {
								x.global->RemoveItemList(temp[j]);
							}
						}
					}
					else {
						x.global->RemoveItemList("", x.ut->GetItemList(it_count).Get().substr(1));
					}
					it_count--;
					//x.global->AddItemType(wiz::load_data::ItemType<std::string>("", x.ut->GetItemList(it_count).Get().substr(1)));
				}
				else if (x.ut->IsItemList(i) && wiz::String::startsWith(x.ut->GetItemList(it_count).GetName(), "@"sv)) {
					//x.global->AddItemType(wiz::load_data::ItemType<std::string>(
					//	x.ut->GetItemList(it_count).GetName().substr(1),
					//	x.ut->GetItemList(it_count).Get()));
					if (wiz::String::startsWith(x.ut->GetItemList(it_count).Get(), "%event_"sv)) {
						std::string event_id = x.ut->GetItemList(it_count).Get().substr(7);

						wiz::ClauText clautext;
						wiz::ExecuteData executeData;
						executeData.pEvents = insert_ut->GetParent();
						executeData.noUseInput = true;
						executeData.noUseOutput = true;
						wiz::Option option;
						wiz::load_data::UserType callUT;
						std::string statements;

						statements += " Event = { id = NONE  ";
						statements += " $call = { id = " + event_id + " ";
						statements += " name = _name  ";
						statements += " value = _value ";
						statements += " is_user_type = FALSE ";
						statements += " real_dir =  _real_dir "; //+wiz::load_data::LoadData::GetRealDir(dir, global) + " ";
						statements += " relative_dir = _dir "; // +dir + " ";
						statements += " idx = _idx "; // +std::to_string(x_idx) + " "; // removal?
						statements += " } ";

						statements += " $return = { $return_value = { } }  ";
						statements += " } ";

						wiz::load_data::LoadData::LoadDataFromString(statements, callUT);

						auto name = x.ut->GetItemList(it_count).GetName().substr(1);
						auto temp = x.global->GetItemIdx(name);

						if (wiz::String::startsWith(name, "&"sv) && name.size() >= 2) {
							long long idx = std::stoll(name.substr(1));

							if (idx < 0 || idx >= x.global->GetItemListSize()) { // .size()) {
								return false;
							}
							auto valName = x.ut->GetItemList(it_count).Get();

							auto callInfo = wiz::load_data::UserType::Find(&callUT, "/./Event/$call");

							callInfo.second[0]->SetItem("name", name);
							callInfo.second[0]->SetItem("value", x.global->GetItemList(idx).Get());
							callInfo.second[0]->SetItem("real_dir", wiz::load_data::LoadData::GetRealDir(x.dir, x.global));
							callInfo.second[0]->SetItem("relative_dir", x.dir);
							callInfo.second[0]->SetItem("idx", std::to_string(idx));

							std::string result = clautext.execute_module(" Main = { $call = { id = NONE  } } " + callUT.ToString(), x.global,
								executeData, option, 1);

							if (result == "TRUE"sv) {
								x.global->RemoveItemList(idx);
							}
						}
						else {
							for (long long j = 0; j < temp.size(); ++j) {
								auto callInfo = wiz::load_data::UserType::Find(&callUT, "/./Event/$call");

								callInfo.second[0]->SetItem("name", name);
								callInfo.second[0]->SetItem("value", x.global->GetItemList(temp[j]).Get());
								callInfo.second[0]->SetItem("real_dir", wiz::load_data::LoadData::GetRealDir(x.dir, x.global));
								callInfo.second[0]->SetItem("relative_dir", x.dir);
								callInfo.second[0]->SetItem("idx", std::to_string(temp[j]));

								std::string result = clautext.execute_module(" Main = { $call = { id = NONE  } } " + callUT.ToString(), x.global,
									executeData, option, 1);

								if (result == "TRUE"sv) {
									x.global->RemoveItemList(temp[j]);
								}
							}
						}
					}
					else {
						x.global->RemoveItemList(x.ut->GetItemList(it_count).GetName().substr(1), x.ut->GetItemList(it_count).Get());
					}

					it_count--;
				}
				else if (x.ut->IsUserTypeList(i) && wiz::String::startsWith(x.ut->GetUserTypeList(ut_count)->GetName(), "@"sv)) {
					if (x.ut->GetUserTypeList(ut_count)->GetName() == "@$"sv) {
						for (long long j = x.global->GetUserTypeListSize() - 1; j >= 0; --j) {
							if (_RemoveFunc(x.global->GetUserTypeList(j), x.ut->GetUserTypeList(ut_count), eventUT)) {
								delete[] x.global->GetUserTypeList(j);
								x.global->GetUserTypeList(j) = nullptr;
								x.global->RemoveUserTypeList(j);
							}
						}
					}
					else {
						auto usertype = x.global->GetUserTypeItemIdx(x.ut->GetUserTypeList(ut_count)->GetName().substr(1));

						for (long long j = usertype.size() - 1; j >= 0; --j) {
							if (_RemoveFunc(x.global->GetUserTypeList(usertype[j]), x.ut->GetUserTypeList(ut_count), eventUT)) {
								x.global->RemoveUserTypeList(usertype[j]);
							}
						}
					}
					ut_count--;
				}
				else if (x.ut->IsUserTypeList(i) && false == wiz::String::startsWith(x.ut->GetUserTypeList(ut_count)->GetName(), "@"sv)) {
					if (wiz::String::startsWith(x.ut->GetUserTypeList(ut_count)->GetName(), "$"sv)) {
						auto temp = x.ut->GetUserTypeList(ut_count)->GetName();
						auto name = temp.substr(1);

						if (name.empty()) {

							for (long long j = 0; j < x.global->GetUserTypeListSize(); ++j) {
								if (_InsertFunc(x.global->GetUserTypeList(j), x.ut->GetUserTypeList(ut_count), eventUT)) {
									que.push(UtInfo(x.global->GetUserTypeList(j), x.ut->GetUserTypeList(ut_count)
										, x.dir + "/$ut" + std::to_string(j)
									));
								}
							}
						}
						else {
							auto usertype = x.global->GetUserTypeItemIdx(name);

							for (long long j = 0; j < usertype.size(); ++j) {
								if (_InsertFunc(x.global->GetUserTypeList(usertype[j]), x.ut->GetUserTypeList(ut_count), eventUT)) {
									que.push(UtInfo(x.global->GetUserTypeList(usertype[j]), x.ut->GetUserTypeList(ut_count),
										x.dir + "/$ut" + std::to_string(usertype[j])));
								}
							}
						}
					}
					else {
						auto usertype = x.global->GetUserTypeItemIdx(x.ut->GetUserTypeList(ut_count)->GetName());

						for (long long j = 0; j < usertype.size(); ++j) {
							//if (_RemoveFunc(x.global->GetUserTypeList(usertype[j]), x.ut->GetUserTypeList(ut_count), eventUT)) {
							que.push(UtInfo(x.global->GetUserTypeList(usertype[j]), x.ut->GetUserTypeList(ut_count),
								x.dir + "/$ut" + std::to_string(usertype[j])));
							//}
						}
					}

					ut_count--;
				}
				else if (x.ut->IsUserTypeList(i)) {
					ut_count--;
				}
				else if (x.ut->IsItemList(i)) {
					it_count--;
				}

				count--;
			}
		}

		return true;
	}

	bool UpdateFunc(wiz::SmartPtr<wiz::load_data::UserType> global, wiz::load_data::UserType* insert_ut, wiz::load_data::UserType* eventUT) {
		if (!_UpdateFunc(global, insert_ut, eventUT)) {
			return false;
		}
		std::string dir = "/.";
		std::queue<UtInfo> que;
		// insert
		que.push(UtInfo(global, insert_ut, dir));

		while (!que.empty()) {
			UtInfo x = que.front();
			que.pop();

			// find non-@
			long long ut_count = 0;
			long long it_count = 0;
			long long count = 0;

			//chk only @  ! - todo
			for (long long i = 0; i < x.ut->GetIListSize(); ++i) {
				if (x.ut->IsItemList(i) && x.ut->GetItemList(it_count).GetName().empty()
					&& wiz::String::startsWith(x.ut->GetItemList(it_count).Get(), "@"sv)) {
					// think @&0 = 3 # 0 <- index, 3 <- value.
					//x.global->GetItemList(0).Set(0, x.ut->GetItemList(it_count).Get());
				}
				else if (x.ut->IsItemList(i) && wiz::String::startsWith(x.ut->GetItemList(it_count).GetName(), "@"sv)) {
					if (wiz::String::startsWith(x.ut->GetItemList(it_count).Get(), "%event_"sv)) {
						std::string event_id = x.ut->GetItemList(it_count).Get().substr(7);

						wiz::ClauText clautext;
						wiz::ExecuteData executeData;
						executeData.pEvents = insert_ut->GetParent();
						executeData.noUseInput = true;
						executeData.noUseOutput = true;
						wiz::Option option;

						wiz::load_data::UserType callUT;
						std::string statements;

						statements += " Event = { id = NONE  ";
						statements += " $call = { id = " + event_id + " ";
						statements += " name = _name  ";
						statements += " value = _value ";
						statements += " is_user_type = FALSE ";
						statements += " real_dir =  _real_dir "; //+wiz::load_data::LoadData::GetRealDir(dir, global) + " ";
						statements += " relative_dir = _dir "; // +dir + " ";
						statements += " idx = _idx "; // +std::to_string(x_idx) + " "; // removal?
						statements += " } ";

						statements += " $return = { $return_value = { } }  ";
						statements += " } ";

						wiz::load_data::LoadData::LoadDataFromString(statements, callUT);

						auto temp = wiz::load_data::UserType::Find(&callUT, "/./Event/$call").second[0];

						std::string name = x.ut->GetItemList(it_count).GetName().substr(1);
						auto position = x.global->GetItemIdx(name);

						{
							std::string name = x.ut->GetItemList(it_count).GetName().substr(1);
							if (wiz::String::startsWith(name, "&"sv) && name.size() >= 2) {
								long long idx = std::stoll(name.substr(1));
								if (idx < 0 || idx >= x.global->GetItemListSize()) {
									return false; // error
								}
								else {
									position.clear();
									position.push_back(idx);
								}
							}


							for (long long j = 0; j < position.size(); ++j) {

								temp->SetItem("name", x.ut->GetItemList(it_count).GetName().substr(1));
								temp->SetItem("value", x.global->GetItemList(position[j]).Get());
								temp->SetItem("relative_dir", x.dir);
								temp->SetItem("real_dir", wiz::load_data::LoadData::GetRealDir(x.dir, x.global));
								temp->SetItem("idx", std::to_string(x.global->GetIlistIndex(position[j], 1)));

								std::string result = clautext.execute_module(" Main = { $call = { id = NONE  } } " + callUT.ToString(), x.global, executeData, option, 1);

								x.global->GetItemList(position[j]).Set(0, result);

							}
						}
					}
					else {
						std::string name = x.ut->GetItemList(it_count).GetName().substr(1);
						if (wiz::String::startsWith(name, "&"sv) && name.size() >= 2) {
							long long idx = std::stoll(name.substr(1));
							if (idx < 0 || idx >= x.global->GetItemListSize()) {
								return false;
							}
							auto value = x.ut->GetItemList(it_count).Get();
							x.global->GetItemList(idx).Set(0, value);
						}
						else {
							x.global->SetItem(std::string(x.ut->GetItemList(it_count).GetName().substr(1)),
								x.ut->GetItemList(it_count).Get());
						}
					}
				}
				else if (x.ut->IsUserTypeList(i) && !wiz::String::startsWith(x.ut->GetUserTypeList(ut_count)->GetName(), "@"sv)) {
					if (wiz::String::startsWith(x.ut->GetUserTypeList(ut_count)->GetName(), "$"sv)) {
						auto temp = x.ut->GetUserTypeList(ut_count)->GetName();
						auto name = temp.substr(1);

						if (name.empty()) {

							for (long long j = 0; j < x.global->GetUserTypeListSize(); ++j) {
								if (_InsertFunc(x.global->GetUserTypeList(j), x.ut->GetUserTypeList(ut_count), eventUT)) {
									que.push(UtInfo(x.global->GetUserTypeList(j), x.ut->GetUserTypeList(ut_count)
										, x.dir + "/$ut" + std::to_string(j)
									));
								}
							}
						}
						else {

							auto usertype = x.global->GetUserTypeItemIdx(name);

							for (long long j = 0; j < usertype.size(); ++j) {
								if (_InsertFunc(x.global->GetUserTypeList(usertype[j]), x.ut->GetUserTypeList(ut_count), eventUT)) {
									que.push(UtInfo(x.global->GetUserTypeList(usertype[j]), x.ut->GetUserTypeList(ut_count),
										x.dir + "/$ut" + std::to_string(usertype[j])));
								}
							}
						}
					}
					else {
						auto usertype = x.global->GetUserTypeItemIdx(x.ut->GetUserTypeList(ut_count)->GetName());

						for (long long j = 0; j < usertype.size(); ++j) {
							//if (_UpdateFunc(x.global->GetUserTypeList(usertype[j]), x.ut->GetUserTypeList(ut_count), eventUT)) {
							que.push(UtInfo(x.global->GetUserTypeList(usertype[j]), x.ut->GetUserTypeList(ut_count),
								x.dir + "/$ut" + std::to_string(usertype[j])));
							//	}
						}
					}
				}

				if (x.ut->IsItemList(i)) {
					it_count++;
				}
				else {
					ut_count++;
				}
				count++;
			}
		}

		return true;
	}


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

std::string ClauText::execute_module(const std::string& MainStr, wiz::load_data::UserType* _global, const ExecuteData& executeData, Option& opt, int chk)
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
	if (executeData.chkInfo == false) { /// chk smartpointer.
		if (global.GetUserTypeItem("Main").empty() && MainStr.empty())
		{
			if (!executeData.noUseOutput) {
				wiz::Out << "do not exist Main" << ENTER;
			}
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
		std::pair<std::string, std::string> id_data = std::make_pair<std::string, std::string>("id", wiz::ToString(info.eventUT->GetUserTypeItem("$call")[0]->GetItem("id")[0].Get()));
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
					std::string fileName = wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(0), ExecuteData(executeData.noUseInput, executeData.noUseOutput)).ToString();
					fileName = wiz::String::substring(fileName, 1, fileName.size() - 2);
					std::string dirName = val->GetUserTypeList(1)->ToString();
					wiz::load_data::UserType* utTemp;

					if (dirName == "/./"sv || dirName == "root"sv) {
						utTemp = &global;
					}
					else {
						dirName = wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(1), ExecuteData(executeData.noUseInput, executeData.noUseOutput)).ToString();
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


		if (nullptr == executeData.pModule) {
			moduleMapPtr = &moduleMap;
		}
		else {
			moduleMapPtr = executeData.pModule;
		}

		if (nullptr == executeData.pObjectMap) {
			objectMapPtr = &objectMap;
		}
		else {
			objectMapPtr = executeData.pObjectMap;
		}

		if (nullptr == executeData.pEvents) {
			_events = global.GetUserTypeItem("Event");
			for (int i = 0; i < _events.size(); ++i) {
				events.LinkUserType(_events[i]);
			}
			global.RemoveUserTypeList("Event", false);
			eventPtr = &events;
		}
		else {
			eventPtr = executeData.pEvents;
		}

		if (!Main.GetUserTypeItem("Event").empty()) {
			eventPtr->AddUserTypeItem(*Main.GetUserTypeItem("Event")[0]);
		}

		// event table setting
		for (int i = 0; i < eventPtr->GetUserTypeListSize(); ++i)
		{
			if (eventPtr->GetUserTypeList(i)->GetName() != "Event"sv) {
				continue;
			}
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
		eventStack.push(executeData.info);
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

					if (eventStack.top().userType_idx.top() >= 1 && val->GetName() == "$else"sv
						&& wiz::ToString(eventPtr->GetUserTypeList(no)->GetUserTypeList(eventStack.top().userType_idx.top() - 1)->GetName()) != "$if"sv) {
						return "ERROR not exist $if, front $else.";
					}
					if (eventStack.top().userType_idx.top() == 0 && val->GetName() == "$else"sv) {
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

					if (eventStack.top().userType_idx.top() >= 1 && val->GetName() == "$else"sv
						&& wiz::ToString(eventStack.top().nowUT.top()->GetUserTypeList(eventStack.top().userType_idx.top() - 1)->GetName()) != "$if"sv) {
						return "ERROR not exist $if, front $else.";
					}
					if (eventStack.top().userType_idx.top() == 0 && val->GetName() == "$else"sv) {
						return "ERROR not exist $if, front $else.";
					}
				}
				else // same to else if( eventSTack.top().nowUT.empty() ), also same to else if ( 1 == eventStack.top().userType_idx.size() )
				{
					if (eventPtr->GetUserTypeList(no)->GetUserTypeListSize() > eventStack.top().userType_idx.top()) {
						val = eventPtr->GetUserTypeList(no)->GetUserTypeList(eventStack.top().userType_idx.top());

						if (eventStack.top().userType_idx.top() >= 1 && val->GetName() == "$else"sv
							&& wiz::ToString(eventPtr->GetUserTypeList(no)->GetUserTypeList(eventStack.top().userType_idx.top() - 1)->GetName()) != "$if"sv) {
							return "ERROR not exist $if, front $else.";
						}
						if (eventStack.top().userType_idx.top() == 0 && val->GetName() == "$else"sv) {
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
				if ("$query"sv == val->GetName()) {
					ExecuteData _executeData; _executeData.depth = executeData.depth;
					_executeData.chkInfo = true;
					_executeData.info = eventStack.top();
					_executeData.pObjectMap = objectMapPtr;
					_executeData.pEvents = eventPtr;
					_executeData.pModule = moduleMapPtr;
					_executeData.noUseInput = executeData.noUseInput;
					_executeData.noUseOutput = executeData.noUseOutput;

					// start dir
					std::string work_dir = wiz::load_data::LoadData::ToBool4(val, global, *val->GetUserTypeList(0), _executeData).ToString();

					wiz::load_data::UserType* work_space = wiz::load_data::UserType::Find(&global, work_dir).second[0];

					for (long long i = 1; i < val->GetUserTypeListSize(); ++i) {
						const std::string name = val->GetUserTypeList(i)->GetName();
						if (name == "$insert"sv) {
							eventStack.top().return_value = InsertFunc(work_space, val->GetUserTypeList(i), &events);
						}
						else if (name == "$update"sv) {
							eventStack.top().return_value = UpdateFunc(work_space, val->GetUserTypeList(i), &events);
						}
						else if (name == "$delete"sv) {
							eventStack.top().return_value = RemoveFunc(work_space, val->GetUserTypeList(i), &events);
						}
					}

					eventStack.top().userType_idx.top()++;
					break;
				}
				else if ("$copy"sv == val->GetName()) { // to = { } from = { } option = { 1 : internal data, default, 2 : total data }
					ExecuteData _executeData; _executeData.depth = executeData.depth;
					_executeData.chkInfo = true;
					_executeData.info = eventStack.top();
					_executeData.pObjectMap = objectMapPtr;
					_executeData.pEvents = eventPtr;
					_executeData.pModule = moduleMapPtr;
					_executeData.noUseInput = executeData.noUseInput;
					_executeData.noUseOutput = executeData.noUseOutput;
					

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
						if (!_executeData.noUseOutput) {
							wiz::Out << "to_ut == from_ut\n";
						}
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
					ExecuteData _executeData; _executeData.depth = executeData.depth;
					_executeData.chkInfo = true;
					_executeData.info = eventStack.top();
					_executeData.pObjectMap = objectMapPtr;
					_executeData.pEvents = eventPtr;
					_executeData.pModule = moduleMapPtr;
					_executeData.noUseInput = executeData.noUseInput;
					_executeData.noUseOutput = executeData.noUseOutput;

					std::string dir = val->GetUserTypeList(0)->ToString();
					std::vector<std::string> events; // event_ids

					for (int i = 0; i < val->GetUserTypeList(1)->GetItemListSize(); ++i) {
						events.push_back(wiz::ToString(val->GetUserTypeList(1)->GetItemList(i).Get(0)));
					}

					std::string recursive = "FALSE";

					if (val->GetUserTypeListSize() >= 3) {
						recursive = val->GetUserTypeList(2)->ToString();
						recursive = wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(2), _executeData).ToString();
					}

					std::string before_value;

					if (val->GetUserTypeListSize() >= 4) {
						before_value = wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(3), _executeData).ToString();
					}
					wiz::load_data::LoadData::Iterate(global, dir, events, recursive, before_value, _executeData);


					eventStack.top().userType_idx.top()++;
					break;
				}
				// $iterate .. dir <- string
				// $iterateA.. dir <- ToBool4
				else if ("$iterateA"sv == val->GetName()) { // very slow? why??
					ExecuteData _executeData; _executeData.depth = executeData.depth;
					_executeData.chkInfo = true;
					_executeData.info = eventStack.top();
					_executeData.pObjectMap = objectMapPtr;
					_executeData.pEvents = eventPtr;
					_executeData.pModule = moduleMapPtr;
					_executeData.noUseInput = executeData.noUseInput;
					_executeData.noUseOutput = executeData.noUseOutput;

					std::string dir = wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(0), _executeData).ToString();
					std::vector<std::string> events; // event_ids

					for (int i = 0; i < val->GetUserTypeList(1)->GetItemListSize(); ++i) {
						events.push_back(wiz::ToString(val->GetUserTypeList(1)->GetItemList(i).Get(0)));
					}

					std::string recursive = "FALSE";

					if (val->GetUserTypeListSize() >= 3) {
						recursive = wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(2), _executeData).ToString();
					}

					std::string before_value;

					if (val->GetUserTypeListSize() >= 4) {
						before_value = wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(3), _executeData).ToString();
					}
					wiz::load_data::LoadData::Iterate(global, dir, events, recursive, before_value, _executeData);


					eventStack.top().userType_idx.top()++;
					break;
				}
				else if ("$iterate2"sv == val->GetName()) { // very slow? why??
					ExecuteData _executeData; _executeData.depth = executeData.depth;
					_executeData.chkInfo = true;
					_executeData.info = eventStack.top();
					_executeData.pObjectMap = objectMapPtr;
					_executeData.pEvents = eventPtr;
					_executeData.pModule = moduleMapPtr;

					_executeData.noUseInput = executeData.noUseInput;
					_executeData.noUseOutput = executeData.noUseOutput;

					std::string dir = val->GetUserTypeList(0)->ToString();
					std::vector<std::string> events; // event_ids

					for (int i = 0; i < val->GetUserTypeList(1)->GetItemListSize(); ++i) {
						events.push_back(wiz::ToString(val->GetUserTypeList(1)->GetItemList(i).Get(0)));
					}

					std::string recursive = "FALSE";

					if (val->GetUserTypeListSize() >= 3) {
						recursive = wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(2), _executeData).ToString();
					}
					wiz::load_data::LoadData::Iterate2(global, dir, events, recursive, _executeData);


					eventStack.top().userType_idx.top()++;
					break;
				}
				else if ("$iterate3"sv == val->GetName()) { //
					ExecuteData _executeData; _executeData.depth = executeData.depth;
					_executeData.chkInfo = true;
					_executeData.info = eventStack.top();
					_executeData.pObjectMap = objectMapPtr;
					_executeData.pEvents = eventPtr;
					_executeData.pModule = moduleMapPtr;
					_executeData.noUseInput = executeData.noUseInput;
					_executeData.noUseOutput = executeData.noUseOutput;

					std::string dir = val->GetUserTypeList(0)->ToString();
					std::string name = wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(1), _executeData).ToString();
					std::vector<std::string> events; // event_ids

					for (int i = 0; i < val->GetUserTypeList(2)->GetItemListSize(); ++i) {
						events.push_back(wiz::ToString(val->GetUserTypeList(2)->GetItemList(i).Get(0)));
					}

					std::string recursive = "FALSE";

					if (val->GetUserTypeListSize() >= 4) {
						recursive = wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(3), _executeData).ToString();
					}

					std::string before_value;

					if (val->GetUserTypeListSize() >= 5) {
						before_value = wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(4), _executeData).ToString();
					}
					wiz::load_data::LoadData::Iterate3(global, dir, name, events, recursive, before_value, _executeData);


					eventStack.top().userType_idx.top()++;
					break;
				}
				else if ("$riterate"sv == val->GetName()) { // very slow? why??
					ExecuteData _executeData; _executeData.depth = executeData.depth;
					_executeData.chkInfo = true;
					_executeData.info = eventStack.top();
					_executeData.pObjectMap = objectMapPtr;
					_executeData.pEvents = eventPtr;
					_executeData.pModule = moduleMapPtr;

					_executeData.noUseInput = executeData.noUseInput;
					_executeData.noUseOutput = executeData.noUseOutput;

					std::string dir = val->GetUserTypeList(0)->ToString();
					std::vector<std::string> events; // event_ids

					for (int i = 0; i < val->GetUserTypeList(1)->GetItemListSize(); ++i) {
						events.push_back(wiz::ToString(val->GetUserTypeList(1)->GetItemList(i).Get(0)));
					}

					std::string recursive = "FALSE";

					if (val->GetUserTypeListSize() >= 3) {
						recursive = wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(2), _executeData).ToString();
					}
					wiz::load_data::LoadData::RIterate(global, dir, events, recursive, _executeData);


					eventStack.top().userType_idx.top()++;
					break;
				}
				else if ("$while"sv == val->GetName()) {
					ExecuteData _executeData; _executeData.depth = executeData.depth;
					_executeData.chkInfo = true;
					_executeData.info = eventStack.top();
					_executeData.pObjectMap = objectMapPtr;
					_executeData.pEvents = eventPtr;
					_executeData.pModule = moduleMapPtr;


					_executeData.noUseInput = executeData.noUseInput;
					_executeData.noUseOutput = executeData.noUseOutput;


					const std::string condition = wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(0), _executeData).ToString();

					if ("TRUE"sv == condition) {
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
					ExecuteData _executeData; _executeData.depth = executeData.depth;
					_executeData.chkInfo = true;
					_executeData.info = eventStack.top();
					_executeData.pObjectMap = objectMapPtr;
					_executeData.pEvents = eventPtr;
					_executeData.pModule = moduleMapPtr;
					_executeData.noUseInput = executeData.noUseInput; //// check!
					_executeData.noUseOutput = executeData.noUseOutput;


					wiz::load_data::UserType subGlobal;
					wiz::load_data::LoadData::LoadDataFromString(val->GetUserTypeList(1)->ToString(), subGlobal);
					wiz::load_data::UserType inputUT;
					wiz::load_data::LoadData::LoadDataFromString(wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(0), _executeData).ToString(), inputUT);


					wiz::load_data::LoadData::AddData(subGlobal, "/./", inputUT.ToString(), _executeData);

					{
						ExecuteData _executeData;
						_executeData.noUseInput = executeData.noUseInput;
						_executeData.noUseOutput = executeData.noUseOutput;

						Option opt;
						eventStack.top().return_value = execute_module("", &subGlobal, _executeData, opt, 0); // return ?
					}

					eventStack.top().userType_idx.top()++;
					break;
				}
				else if ("$remove_usertype_total"sv == val->GetName()) { //// has bug?
					ExecuteData _executeData; _executeData.depth = executeData.depth;
					_executeData.chkInfo = true;
					_executeData.info = eventStack.top();
					_executeData.pObjectMap = objectMapPtr;
					_executeData.pEvents = eventPtr;
					_executeData.pModule = moduleMapPtr;
					_executeData.noUseInput = executeData.noUseInput; //// check!
					_executeData.noUseOutput = executeData.noUseOutput;

					std::string ut_name = wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(0), _executeData).ToString();
					
					std::string start_dir = "root";

					if (val->GetUserTypeListSize() >= 2) {
						start_dir = wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(1), _executeData).ToString();
					}
					bool recursive = false;

					wiz::load_data::LoadData::RemoveUserTypeTotal(global, ut_name,  start_dir, _executeData, recursive);

					eventStack.top().userType_idx.top()++;
					break;
				}
				else if ("$save"sv == val->GetName()) // save data, event, main!
				{
					ExecuteData _executeData; _executeData.depth = executeData.depth;
					_executeData.chkInfo = true;
					_executeData.info = eventStack.top();
					_executeData.pObjectMap = objectMapPtr;
					_executeData.pEvents = eventPtr;
					_executeData.pModule = moduleMapPtr;
					_executeData.noUseInput = executeData.noUseInput; //// check!
					_executeData.noUseOutput = executeData.noUseOutput;


					//todo
					// "filename" save_option(0~2)
					std::string fileName = wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(0), _executeData).ToString();
					fileName = wiz::String::substring(fileName, 1, fileName.size() - 2);
					std::string option = wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(1), _executeData).ToString();

					wiz::load_data::LoadData::SaveWizDB(global, fileName, option, "");
					wiz::load_data::LoadData::SaveWizDB(Main, fileName, option, "APPEND");
					wiz::load_data::LoadData::SaveWizDB(events, fileName, option, "APPEND");

					eventStack.top().userType_idx.top()++;
					break;
				}
				else if ("$save_data_only"sv == val->GetName())
				{
					ExecuteData _executeData; _executeData.depth = executeData.depth;
					_executeData.chkInfo = true;
					_executeData.info = eventStack.top();
					_executeData.pObjectMap = objectMapPtr;
					_executeData.pEvents = eventPtr;
					_executeData.pModule = moduleMapPtr;
					_executeData.noUseInput = executeData.noUseInput; //// check!
					_executeData.noUseOutput = executeData.noUseOutput;


					//todo
					// "filename" save_option(0~2)
					std::string fileName = wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(0), _executeData).ToString();
					if (fileName.back() == '\"' && fileName.size() >= 2 && fileName[0] == fileName.back()) {
						fileName = wiz::String::substring(fileName, 1, fileName.size() - 2);
					}
					std::string option = wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(1), _executeData).ToString();

					wiz::load_data::LoadData::SaveWizDB(global, fileName, option, "");

					eventStack.top().userType_idx.top()++;
					break;
				}

				else if ("$save_data_only2"sv == val->GetName())
				{
					ExecuteData _executeData; _executeData.depth = executeData.depth;
					_executeData.chkInfo = true;
					_executeData.info = eventStack.top();
					_executeData.pObjectMap = objectMapPtr;
					_executeData.pEvents = eventPtr;
					_executeData.pModule = moduleMapPtr;

					_executeData.noUseInput = executeData.noUseInput; //// check!
					_executeData.noUseOutput = executeData.noUseOutput;



					//todo
					// "filename" save_option(0~2)
					std::string dirName = wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(0), _executeData).ToString();
					std::string fileName = wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(1), _executeData).ToString();
					
					if (fileName.back() == '\"' && fileName.size() >= 2 && fileName[0] == fileName.back()) {
						fileName = wiz::String::substring(fileName, 1, fileName.size() - 2);
					}
					std::string option = wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(2), _executeData).ToString();

					// todo - for? auto x = global.GetUserTypeItem(dirName);
					wiz::load_data::UserType* utTemp;

					utTemp = wiz::load_data::UserType::Find(&global, dirName).second[0];

					wiz::load_data::LoadData::SaveWizDB(*utTemp, fileName, option, "");


					eventStack.top().userType_idx.top()++;
					break;
				}
				else if ("$option"sv == val->GetName()) // first
				{
					ExecuteData _executeData; _executeData.depth = executeData.depth;
					_executeData.chkInfo = true;
					_executeData.info = eventStack.top();
					_executeData.pObjectMap = objectMapPtr;
					_executeData.pEvents = eventPtr;
					_executeData.pModule = moduleMapPtr;

					_executeData.noUseInput = executeData.noUseInput; //// check!
					_executeData.noUseOutput = executeData.noUseOutput;

					eventStack.top().option = wiz::load_data::LoadData::ToBool4(nullptr, global, *val, _executeData).ToString();

					eventStack.top().userType_idx.top()++;
					break;
				}

				// done - ($push_back-insert!) $pop_back, $push_front, $pop_front ($front?, $back?)
				else if ("$pop_back"sv == val->GetName()) {
					ExecuteData _executeData; _executeData.depth = executeData.depth;
					_executeData.chkInfo = true;
					_executeData.info = eventStack.top();
					_executeData.pObjectMap = objectMapPtr;
					_executeData.pEvents = eventPtr;
					_executeData.pModule = moduleMapPtr;

					_executeData.noUseInput = executeData.noUseInput; //// check!
					_executeData.noUseOutput = executeData.noUseOutput;

					std::string dir = wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(0), _executeData).ToString();
					wiz::load_data::UserType* ut = nullptr;
					auto finded = wiz::load_data::UserType::Find(&global, dir);
					ut = finded.second[0];

					wiz::load_data::LoadData::Remove(global, dir, ut->GetIListSize() - 1,  _executeData);

					eventStack.top().userType_idx.top()++;
					break;
				}
				else if ("$push_front"sv == val->GetName()) {
					ExecuteData _executeData; _executeData.depth = executeData.depth;
					_executeData.chkInfo = true;
					_executeData.info = eventStack.top();
					_executeData.pObjectMap = objectMapPtr;
					_executeData.pEvents = eventPtr;
					_executeData.pModule = moduleMapPtr;

					_executeData.noUseInput = executeData.noUseInput; //// check!
					_executeData.noUseOutput = executeData.noUseOutput;

					std::string value;
					std::string dir;
					
					dir = wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(0), _executeData).ToString();
					
					value = wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(1), _executeData).ToString();

					wiz::load_data::LoadData::AddDataAtFront(global, dir, value,  _executeData);

					eventStack.top().userType_idx.top()++;
					break;
				}
				else if ("$pop_front"sv == val->GetName()) {
					ExecuteData _executeData; _executeData.depth = executeData.depth;
					_executeData.chkInfo = true;
					_executeData.info = eventStack.top();
					_executeData.pObjectMap = objectMapPtr;
					_executeData.pEvents = eventPtr;
					_executeData.pModule = moduleMapPtr;

					_executeData.noUseInput = executeData.noUseInput; //// check!
					_executeData.noUseOutput = executeData.noUseOutput;

					std::string dir = wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(0), _executeData).ToString();

					wiz::load_data::LoadData::Remove(global, dir, 0,  _executeData);

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
				ExecuteData _executeData; _executeData.depth = executeData.depth;
					_executeData.chkInfo = true;
					_executeData.info = eventStack.top();
					_executeData.pObjectMap = objectMapPtr;
					_executeData.pEvents = eventPtr;
					_executeData.pModule = moduleMapPtr;
					
					_executeData.noUseInput = executeData.noUseInput; //// check!
					_executeData.noUseOutput = executeData.noUseOutput;

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
					ExecuteData _executeData; _executeData.depth = executeData.depth;
					_executeData.chkInfo = true;
					_executeData.info = eventStack.top();
					_executeData.pObjectMap = objectMapPtr;
					_executeData.pEvents = eventPtr;
					_executeData.pModule = moduleMapPtr;
					_executeData.noUseInput = executeData.noUseInput; //// check!
					_executeData.noUseOutput = executeData.noUseOutput;

					std::string fileName = val->GetItemList(0).ToString();
					wiz::Out.SetFileName(fileName.substr(1, fileName.size() - 2));


					eventStack.top().userType_idx.top()++;
					break;
				}
				else if ("$print_file_clear"sv == val->GetName()) {
					ExecuteData _executeData; _executeData.depth = executeData.depth;
					_executeData.chkInfo = true;
					_executeData.info = eventStack.top();
					_executeData.pObjectMap = objectMapPtr;
					_executeData.pEvents = eventPtr;
					_executeData.pModule = moduleMapPtr;

					_executeData.noUseInput = executeData.noUseInput; //// check!
					_executeData.noUseOutput = executeData.noUseOutput;

					wiz::Out.clear_file();


					eventStack.top().userType_idx.top()++;
					break;
				}
				else if ("$call"sv == val->GetName()) {
					ExecuteData _executeData; _executeData.depth = executeData.depth;
					_executeData.chkInfo = true;
					_executeData.info = eventStack.top();
					_executeData.pObjectMap = objectMapPtr;
					_executeData.pEvents = eventPtr;
					_executeData.pModule = moduleMapPtr;

					_executeData.noUseInput = executeData.noUseInput; //// check!
					_executeData.noUseOutput = executeData.noUseOutput;

					if (!val->GetItem("id").empty()) {
						info.id = wiz::ToString(val->GetItem("id")[0].Get(0));
					}
					else {
						//// removal?
						info.id = wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeItem("id")[0], _executeData).ToString();
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
								_executeData.info = info2;
								std::string temp = wiz::ToString(val->GetItemList(j).Get(0));
								auto temp2 = std::pair<std::string, std::string>(wiz::ToString(val->GetItemList(j).GetName()), temp);

								info.parameters.insert(temp2);
							}
						}
						for (int j = 0; j < val->GetUserTypeListSize(); ++j) {
							if (val->GetUserTypeListSize() > 0) {
								_executeData.info = info2;
								std::string temp = wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(j), _executeData).ToString();
								auto temp2 = std::pair<std::string, std::string>(wiz::ToString(val->GetUserTypeList(j)->GetName()), temp);
								info.parameters.insert(temp2);
							}
						}
						eventStack.top().userType_idx.top()++;
					}
					else { // recursive call!
						if (val->GetItemListSize() > 0) {
							for (int j = 0; j < val->GetItemListSize(); ++j) {
								_executeData.info = info;
								_executeData.info.parameters = info2.parameters;

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
								_executeData.info = info;
								_executeData.info.parameters = info2.parameters;

								std::string temp = wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(j), _executeData).ToString();

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
					_executeData.info = info;

					_executeData.noUseInput = executeData.noUseInput;
					_executeData.noUseOutput = executeData.noUseOutput;

					std::thread* A = new std::thread(execute_module, "", &global, _executeData, opt, 0);

					waits.push_back(A);
					}*/
					//else {
					eventStack.push(info);
					//}

					break;
				}
				else if ("$call_by_data"sv == val->GetName()) {
					ExecuteData _executeData; _executeData.depth = executeData.depth;
					_executeData.chkInfo = true;
					_executeData.info = eventStack.top();
					_executeData.pObjectMap = objectMapPtr;
					_executeData.pEvents = eventPtr;
					_executeData.pModule = moduleMapPtr;

					_executeData.noUseInput = executeData.noUseInput; //// check!
					_executeData.noUseOutput = executeData.noUseOutput;


					std::string dir = val->GetItemList(0).ToString();
					wiz::load_data::UserType subGlobal;
					wiz::load_data::LoadData::LoadDataFromString(global.GetUserTypeItem(dir)[0]->ToString(), subGlobal);

					{
						ExecuteData _executeData;
						_executeData.noUseInput = executeData.noUseInput;
						_executeData.noUseOutput = executeData.noUseOutput;
						Option _opt;
						eventStack.top().return_value = execute_module("", &subGlobal, _executeData, _opt, 0);  // return ?
					}

					eventStack.top().userType_idx.top()++;
					break;
				}
				else if ("$call_by_data2"sv == val->GetName()) {
					ExecuteData _executeData; _executeData.depth = executeData.depth;
					_executeData.chkInfo = true;
					_executeData.info = eventStack.top();
					_executeData.pObjectMap = objectMapPtr;
					_executeData.pEvents = eventPtr;
					_executeData.pModule = moduleMapPtr;
					_executeData.noUseInput = executeData.noUseInput; //// check!
					_executeData.noUseOutput = executeData.noUseOutput;


					std::string dir = wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(0), _executeData).ToString();

					wiz::load_data::UserType subGlobal;
					wiz::load_data::LoadData::LoadDataFromString(global.GetUserTypeItem(dir)[0]->ToString(), subGlobal);

					{
						ExecuteData _executeData;
						_executeData.noUseInput = executeData.noUseInput;
						_executeData.noUseOutput = executeData.noUseOutput;
						Option _opt;
						eventStack.top().return_value = execute_module("", &subGlobal, _executeData, _opt, 0);  // return ?
					}

					eventStack.top().userType_idx.top()++;
					break;
				}
				//// no $parameter.~
				else if ("$assign"sv == val->GetName()) /// -> assign2?
				{
					ExecuteData _executeData; _executeData.depth = executeData.depth;
					_executeData.chkInfo = true;
					_executeData.info = eventStack.top();
					_executeData.pObjectMap = objectMapPtr;
					_executeData.pEvents = eventPtr;
					_executeData.pModule = moduleMapPtr;

					_executeData.noUseInput = executeData.noUseInput; //// check!
					_executeData.noUseOutput = executeData.noUseOutput;

					std::pair<std::string, std::string> dir = wiz::load_data::LoadData::Find2(&global, wiz::ToString(val->GetItemList(0).Get(0)));
					std::string data = wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(0), _executeData).ToString();

					if (dir.first == "" && wiz::String::startsWith(dir.second, "$local."))
					{
						eventStack.top().locals[wiz::String::substring(dir.second, 7)] = data;
					}
					else {
						wiz::load_data::LoadData::SetData(global, dir.first, dir.second, data, _executeData);
					}
					eventStack.top().userType_idx.top()++;
					break;
				}

				else if ("$assign2"sv == val->GetName())
				{
					ExecuteData _executeData; _executeData.depth = executeData.depth;
					_executeData.chkInfo = true;
					_executeData.info = eventStack.top();
					_executeData.pObjectMap = objectMapPtr;
					_executeData.pEvents = eventPtr;
					_executeData.pModule = moduleMapPtr;

					_executeData.noUseInput = executeData.noUseInput; //// check!
					_executeData.noUseOutput = executeData.noUseOutput;

					std::pair<std::string, std::string> dir = wiz::load_data::LoadData::Find2(&global, wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(0), _executeData).ToString());
					std::string data = wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(1), _executeData).ToString();

					{
						if (dir.first == "" && wiz::String::startsWith(dir.second, "$local."))
						{
							eventStack.top().locals[wiz::String::substring(dir.second, 7)] = data;
						}
						else {
							wiz::load_data::LoadData::SetData(global, dir.first, dir.second, data, _executeData);
						}
					}

					eventStack.top().userType_idx.top()++;
					break;
				}

				else if ("$assign_local"sv == val->GetName()) /// no  
				{
					ExecuteData _executeData; _executeData.depth = executeData.depth;
					_executeData.chkInfo = true;
					_executeData.info = eventStack.top();
					_executeData.pObjectMap = objectMapPtr;
					_executeData.pEvents = eventPtr;
					_executeData.pModule = moduleMapPtr;

					_executeData.noUseInput = executeData.noUseInput; //// check!
					_executeData.noUseOutput = executeData.noUseOutput;

					std::pair<std::string, std::string> dir = wiz::load_data::LoadData::Find2(&global, wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(0), _executeData).ToString());
					std::string data = wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(1), _executeData).ToString();

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
					ExecuteData _executeData; _executeData.depth = executeData.depth;
					_executeData.chkInfo = true;
					_executeData.info = eventStack.top();
					_executeData.pObjectMap = objectMapPtr;
					_executeData.pEvents = eventPtr;
					_executeData.pModule = moduleMapPtr;

					_executeData.noUseInput = executeData.noUseInput; //// check!
					_executeData.noUseOutput = executeData.noUseOutput;

					std::pair<std::string, std::string> dir = wiz::load_data::LoadData::Find2(&global, 
						wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(0), _executeData).ToString());
					std::string data = wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(1), _executeData).ToString();

					//std::string condition;
					//if (val->GetUserTypeListSize() >= 3) {
					//	condition = wiz::load_data::LoadData::ToBool4(nullptr, global, val->GetUserTypeList(2)->ToString(), _executeData).ToString();
					//}
					wiz::load_data::LoadData::SetData(global, dir.first, dir.second, data, _executeData);

					// chk local?

					eventStack.top().userType_idx.top()++;
					break;
				}

				else if ("$assign_from_ut"sv == val->GetName()) {
					ExecuteData _executeData; _executeData.depth = executeData.depth;
					_executeData.chkInfo = true;
					_executeData.info = eventStack.top();
					_executeData.pObjectMap = objectMapPtr;
					_executeData.pEvents = eventPtr;
					_executeData.pModule = moduleMapPtr;

					_executeData.noUseInput = executeData.noUseInput; //// check!
					_executeData.noUseOutput = executeData.noUseOutput;

					std::pair<std::string, std::string> dir = wiz::load_data::LoadData::Find2(&global, wiz::ToString(val->GetItemList(0).Get(0)));
					std::string ut_str = wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(0), _executeData).ToString();
					wiz::load_data::UserType ut;
					wiz::load_data::LoadData::LoadDataFromString(ut_str, ut);

					// check! ExecuteData(executeData.noUseInput, executeData.noUseOutput) ?
					std::string data = wiz::load_data::LoadData::ToBool4(nullptr, ut, *val->GetUserTypeList(1), _executeData).ToString();

					if (dir.first == "" && wiz::String::startsWith(dir.second, "$local."))
					{
						eventStack.top().locals[wiz::String::substring(dir.second, 7)] = data;
					}
					else {
						wiz::load_data::LoadData::SetData(global, dir.first, dir.second, data, _executeData);
					}
					eventStack.top().userType_idx.top()++;
					break;
				}

				/// cf) insert3? - any position?
				else if ("$push_back"sv == val->GetName() || "$insert"sv == val->GetName() || "$insert2"sv == val->GetName())
				{
					ExecuteData _executeData; _executeData.depth = executeData.depth;
					_executeData.chkInfo = true;
					_executeData.info = eventStack.top();
					_executeData.pObjectMap = objectMapPtr;
					_executeData.pEvents = eventPtr;
					_executeData.pModule = moduleMapPtr;

					_executeData.noUseInput = executeData.noUseInput; //// check!
					_executeData.noUseOutput = executeData.noUseOutput;

					std::string value;
					std::string dir;
					if (val->GetUserTypeList(0)->GetItemListSize() > 0) {
						dir = wiz::load_data::LoadData::ToBool4(nullptr, global, val->GetUserTypeList(0)->GetItemList(0), _executeData).ToString();
					}
					else ///val->Ge
					{
						dir = wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(0), _executeData).ToString();
					}

					value = wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(1), _executeData).ToString();
					
					wiz::load_data::LoadData::AddData(global, dir, value,  _executeData);
					
					eventStack.top().userType_idx.top()++;
					break;
				}
				else if ("$insert_noname_usertype"sv == val->GetName())
				{
					ExecuteData _executeData; _executeData.depth = executeData.depth;
					_executeData.chkInfo = true;
					_executeData.info = eventStack.top();
					_executeData.pObjectMap = objectMapPtr;
					_executeData.pEvents = eventPtr;
					_executeData.pModule = moduleMapPtr;

					_executeData.noUseInput = executeData.noUseInput; //// check!
					_executeData.noUseOutput = executeData.noUseOutput;

					std::string position = wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(0), _executeData).ToString();;
					std::string data = wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(1), _executeData).ToString();;

					wiz::load_data::LoadData::AddNoNameUserType(global, position, data,  _executeData);

					eventStack.top().userType_idx.top()++;
					break;
				}
				else if ("$insert_by_idx"sv == val->GetName())
				{
					ExecuteData _executeData; _executeData.depth = executeData.depth;
					_executeData.chkInfo = true;
					_executeData.info = eventStack.top();
					_executeData.pObjectMap = objectMapPtr;
					_executeData.pEvents = eventPtr;
					_executeData.pModule = moduleMapPtr;

					_executeData.noUseInput = executeData.noUseInput; //// check!
					_executeData.noUseOutput = executeData.noUseOutput;

					std::string value;
					int idx = atoi(wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(1), _executeData).ToString().c_str());
					std::string dir;
					if (val->GetUserTypeList(0)->GetItemListSize() > 0) {
						dir = wiz::load_data::LoadData::ToBool4(nullptr, global, val->GetUserTypeList(0)->GetItemList(0), _executeData).ToString();
					}
					else ///val->Ge
					{
						dir = wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(0), _executeData).ToString();
					}

					value = wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(2), _executeData).ToString();

					wiz::load_data::LoadData::Insert(global, dir, idx, value,  _executeData);

					eventStack.top().userType_idx.top()++;
					break;
				}
				else if ("$make"sv == val->GetName()) // To Do? - make2? or remake? 
													// cf) make empty ut??
				{
					ExecuteData _executeData; _executeData.depth = executeData.depth;
					_executeData.chkInfo = true;
					_executeData.info = eventStack.top();
					_executeData.pObjectMap = objectMapPtr;
					_executeData.pEvents = eventPtr;
					_executeData.pModule = moduleMapPtr;

					_executeData.noUseInput = executeData.noUseInput; //// check!
					_executeData.noUseOutput = executeData.noUseOutput;

					std::string dir;
					bool is2 = false;
					if (val->GetItemListSize() > 0) { // remove?
						dir = wiz::load_data::LoadData::ToBool4(nullptr, global, val->GetItemList(0), _executeData).ToString();
					}
					else // 
					{
					//	wiz::Out << val->GetUserTypeList(0)->ToString() << "\n";
						dir = wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(0), _executeData).ToString();
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

					wiz::load_data::LoadData::AddUserType(global, dir, name, "",  _executeData);


					eventStack.top().userType_idx.top()++;
					break;
				}
				else if ("$findIndex"sv == val->GetName()) // For list : { 1 2  3 4 5 }
				{
					ExecuteData _executeData; _executeData.depth = executeData.depth;
					_executeData.chkInfo = true;
					_executeData.info = eventStack.top();
					_executeData.pObjectMap = objectMapPtr;
					_executeData.pEvents = eventPtr;
					_executeData.pModule = moduleMapPtr;

					_executeData.noUseInput = executeData.noUseInput; //// check!
					_executeData.noUseOutput = executeData.noUseOutput;

					std::string dir = wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(0), _executeData).ToString();
					std::string value = wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(1), _executeData).ToString();

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
					ExecuteData _executeData; _executeData.depth = executeData.depth;
					_executeData.chkInfo = true;
					_executeData.info = eventStack.top();
					_executeData.pObjectMap = objectMapPtr;
					_executeData.pEvents = eventPtr;
					_executeData.pModule = moduleMapPtr;

					_executeData.noUseInput = executeData.noUseInput; //// check!
					_executeData.noUseOutput = executeData.noUseOutput;


					std::string dir;

					dir = wiz::load_data::LoadData::ToBool4(nullptr, global, val->GetItemList(0), _executeData).ToString();

					wiz::load_data::LoadData::Remove(global, dir,  _executeData);

					eventStack.top().userType_idx.top()++;
					break;
				}
				else if ("$remove2"sv == val->GetName()) // remove /dir/name 
													   // if name is empty, then chk!!
				{
					ExecuteData _executeData; _executeData.depth = executeData.depth;
					_executeData.chkInfo = true;
					_executeData.info = eventStack.top();
					_executeData.pObjectMap = objectMapPtr;
					_executeData.pEvents = eventPtr;
					_executeData.pModule = moduleMapPtr;

					_executeData.noUseInput = executeData.noUseInput; //// check!
					_executeData.noUseOutput = executeData.noUseOutput;

					std::string dir; // item -> userType
					dir = wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(0), _executeData).ToString();
					std::string name;
					for (int i = dir.size() - 1; i >= 0; --i)
					{
						if (dir[i] == '/') {
							name = wiz::String::substring(dir, i + 1);
							dir = wiz::String::substring(dir, 0, i - 1);
							break;
						}
					}

					wiz::load_data::LoadData::Remove(global, dir, name,  _executeData);

					eventStack.top().userType_idx.top()++;
					break;
				}
				else if ("$remove3"sv == val->GetName()) /// remove itemlist by idx.
				{
					ExecuteData _executeData; _executeData.depth = executeData.depth;
					_executeData.chkInfo = true;
					_executeData.info = eventStack.top();
					_executeData.pObjectMap = objectMapPtr;
					_executeData.pEvents = eventPtr;
					_executeData.pModule = moduleMapPtr;

					_executeData.noUseInput = executeData.noUseInput; //// check!
					_executeData.noUseOutput = executeData.noUseOutput;

					std::string dir = wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(0), _executeData).ToString();
					std::string value = wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(1), _executeData).ToString();

					int idx = atoi(value.c_str());  // long long -> int?

					std::string condition = "TRUE";

					if (val->GetUserTypeListSize() >= 3) {
						condition = val->GetUserTypeList(2)->ToString();
					}

					wiz::load_data::LoadData::Remove(global, dir, idx,  _executeData);
					// remove -> UserType::Find(&global, dir).second[0]->RemoveItemList(idx); /// change ?

					eventStack.top().userType_idx.top()++;
					break;
				}

				else if ("$setElement"sv == val->GetName())
				{
					ExecuteData _executeData; _executeData.depth = executeData.depth;
					_executeData.chkInfo = true;
					_executeData.info = eventStack.top();
					_executeData.pObjectMap = objectMapPtr;
					_executeData.pEvents = eventPtr;
					_executeData.pModule = moduleMapPtr;


					_executeData.noUseInput = executeData.noUseInput; //// check!
					_executeData.noUseOutput = executeData.noUseOutput;

					std::string dir = wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(0), _executeData).ToString();
					std::string idx = wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(1), _executeData).ToString();
					std::string value = wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(2), _executeData).ToString();

					int _idx = stoi(idx);
					wiz::load_data::UserType::Find(&global, dir).second[0]->SetItem(_idx, std::move(value));

					eventStack.top().userType_idx.top()++;
					break;
				}
				else if ("$swap"sv == val->GetName()) // $swap2
				{
					ExecuteData _executeData; _executeData.depth = executeData.depth;
					_executeData.chkInfo = true;
					_executeData.info = eventStack.top();
					_executeData.pObjectMap = objectMapPtr;
					_executeData.pEvents = eventPtr;
					_executeData.pModule = moduleMapPtr;



					_executeData.noUseInput = executeData.noUseInput; //// check!

					_executeData.noUseOutput = executeData.noUseOutput;

					std::string dir = std::string(wiz::ToString(val->GetItemList(0).Get(0)).c_str()); // + 0
					std::string value1;
					std::string value2;

					value1 = wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(0), _executeData).ToString();
					value2 = wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(1), _executeData).ToString();
					if (value1 != value2) {
						int x = atoi(value1.c_str());
						int y = atoi(value2.c_str());

						std::string temp = wiz::ToString(wiz::load_data::UserType::Find(&global, dir).second[0]->GetItemList(x).Get(0));
						std::string temp2 = wiz::ToString(wiz::load_data::UserType::Find(&global, dir).second[0]->GetItemList(y).Get(0));

						wiz::load_data::LoadData::SetData(global, dir, x, temp2, _executeData);
						wiz::load_data::LoadData::SetData(global, dir, y, temp, _executeData);
					}

					eventStack.top().userType_idx.top()++;
					break;
				}
				else if ("$print"sv == val->GetName()) /// has many bugs..!?, for print list or print item?.
				{
					if (executeData.noUseOutput) {
						eventStack.top().userType_idx.top()++;
						break;
					}
					ExecuteData _executeData; _executeData.depth = executeData.depth;
					_executeData.chkInfo = true;
					_executeData.info = eventStack.top();
					_executeData.pObjectMap = objectMapPtr;
					_executeData.pEvents = eventPtr;
					_executeData.pModule = moduleMapPtr;


					_executeData.noUseInput = executeData.noUseInput; //// check!
					_executeData.noUseOutput = executeData.noUseOutput;

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
							std::string temp = wiz::load_data::LoadData::ToBool4(nullptr, global, val->GetUserTypeList(0)->GetItemList(0), _executeData).ToString();
							if (temp.empty()) {
								wiz::Out << "EMPTY";
							}
							else {
								wiz::Out << temp;
							}
						}
						else if (wiz::String::startsWith(listName, "/") && listName.size() > 1)
						{
							std::string temp = wiz::load_data::LoadData::ToBool4(nullptr, global, val->GetUserTypeList(0)->GetItemList(0), _executeData).ToString();
							if (temp != listName) // chk 
							{
								wiz::Out << temp;
							}
							else {
								//wiz::Out << global.ToString() << "\n";
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
							*val->GetUserTypeList(0), _executeData).ToString();
						wiz::Out << name;
					}
					else
					{
						std::string start;
						std::string last;

						start = wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(1), _executeData).ToString();
						last = wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(2), _executeData).ToString();

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
					if (executeData.noUseOutput) {
						eventStack.top().userType_idx.top()++;
						break;
					}
					ExecuteData _executeData; _executeData.depth = executeData.depth;
					_executeData.chkInfo = true;
					_executeData.info = eventStack.top();
					_executeData.pObjectMap = objectMapPtr;
					_executeData.pEvents = eventPtr;
					_executeData.pModule = moduleMapPtr;


					_executeData.noUseInput = executeData.noUseInput; //// check!
					_executeData.noUseOutput = executeData.noUseOutput;

					std::string dir = wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(0), _executeData).ToString();
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
					ExecuteData _executeData; _executeData.depth = executeData.depth;
					_executeData.chkInfo = true;
					_executeData.info = eventStack.top();
					_executeData.pObjectMap = objectMapPtr;
					_executeData.pEvents = eventPtr;
					_executeData.pModule = moduleMapPtr;



					_executeData.noUseInput = executeData.noUseInput; //// check!
					_executeData.noUseOutput = executeData.noUseOutput;
					// load data and events from other file! 
					// itemlist -> usertypelist.tostring?
					for (int i = 0; i < val->GetItemListSize(); ++i) {
						wiz::load_data::UserType ut;
						std::string fileName = wiz::ToString(val->GetItemList(i).Get(0));
						fileName = wiz::String::substring(fileName, 1, fileName.size() - 2);
						//int a = clock();
						if (wiz::load_data::LoadData::LoadDataFromFile(fileName, ut)) {
							//int b = clock();
							//wiz::Out << b - a << "ms\n";
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
								if (executeData.noUseOutput) {
									wiz::Out << "err" << ENTER;
								}
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
					ExecuteData _executeData; _executeData.depth = executeData.depth;
					_executeData.chkInfo = true;
					_executeData.info = eventStack.top();
					_executeData.pObjectMap = objectMapPtr;
					_executeData.pEvents = eventPtr;
					_executeData.pModule = moduleMapPtr;


					_executeData.noUseInput = executeData.noUseInput; //// check!
					_executeData.noUseOutput = executeData.noUseOutput;

					// to do, load data and events from other file!
					wiz::load_data::UserType ut;
					std::string fileName = wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(0), _executeData).ToString();
					
					if (fileName.back() == '\"' && fileName.size() >= 2 && fileName.back() == fileName[0]) {
						fileName = wiz::String::substring(fileName, 1, fileName.size() - 2);
					}
					std::string dirName = val->GetUserTypeList(1)->ToString();
					wiz::load_data::UserType* utTemp;


					if (dirName == "/./" || dirName == "root") {
						utTemp = &global;
					}
					else {
						dirName = wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(1), ExecuteData(executeData.noUseInput, executeData.noUseOutput)).ToString();
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
					ExecuteData _executeData; _executeData.depth = executeData.depth;
					_executeData.chkInfo = true;
					_executeData.info = eventStack.top();
					_executeData.pObjectMap = objectMapPtr;
					_executeData.pEvents = eventPtr;
					_executeData.pModule = moduleMapPtr;


					_executeData.noUseInput = executeData.noUseInput; //// check!
					_executeData.noUseOutput = executeData.noUseOutput;

					// to do, load data and events from other file!
					wiz::load_data::UserType ut;
					std::string fileName = wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(0), _executeData).ToString();
					fileName = wiz::String::substring(fileName, 1, fileName.size() - 2);
					std::string dirName = val->GetUserTypeList(1)->ToString();
					wiz::load_data::UserType* utTemp = global.GetUserTypeItem(dirName)[0];


					if (dirName == "/./" || dirName == "root") {
						utTemp = &global;
					}
					else {
						dirName = wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(1), ExecuteData(executeData.noUseInput, executeData.noUseOutput)).ToString();
						utTemp = global.GetUserTypeItem(dirName)[0];
					}

					//int a = clock();
					if (wiz::load_data::LoadData::LoadDataFromFileWithJson(fileName, ut)) {
						//int b = clock();
						//wiz::Out << b - a << "ms\n";
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
					ExecuteData _executeData; _executeData.depth = executeData.depth;
					_executeData.chkInfo = true;
					_executeData.info = eventStack.top();
					_executeData.pObjectMap = objectMapPtr;
					_executeData.pEvents = eventPtr;
					_executeData.pModule = moduleMapPtr;


					_executeData.noUseInput = executeData.noUseInput; //// check!
					_executeData.noUseOutput = executeData.noUseOutput;

					// to do, load data and events from other file!
					wiz::load_data::UserType ut;
					std::string fileName = wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(0), _executeData).ToString();
					fileName = wiz::String::substring(fileName, 1, fileName.size() - 2);
					std::string dirName = val->GetUserTypeList(1)->ToString();
					wiz::load_data::UserType* utTemp = global.GetUserTypeItem(dirName)[0];

					if (dirName == "/./" || dirName == "root") {
						utTemp = &global;
					}
					else {
						dirName = wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(1), ExecuteData(executeData.noUseInput, executeData.noUseOutput)).ToString();
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
				else if ("$clear_screen"sv == val->GetName())
				{

#if _WIN32
					system("cls");
#endif
					eventStack.top().userType_idx.top()++;
					break;
				}
				else if ("$_getch"sv == val->GetName())
				{
					if (executeData.noUseInput) {
						eventStack.top().userType_idx.top()++;
						break;
					}

					GETCH();
					eventStack.top().userType_idx.top()++;
					break;
				}
				
				else if ("$input"sv == val->GetName())
				{
					if (executeData.noUseInput) {
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
					if (executeData.noUseInput) { // when no use input?
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
					ExecuteData _executeData; _executeData.depth = executeData.depth;
					_executeData.chkInfo = true;
					_executeData.info = eventStack.top();
					_executeData.pObjectMap = objectMapPtr;
					_executeData.pEvents = eventPtr;
					_executeData.pModule = moduleMapPtr;


					_executeData.noUseInput = executeData.noUseInput; //// check!
					_executeData.noUseOutput = executeData.noUseOutput;

					eventStack.top().userType_idx.top()++;
					if (eventStack.size() > 1)
					{
						std::string temp = wiz::load_data::LoadData::ToBool4(nullptr, global, *val, _executeData).ToString();
						/// if temp just one
						eventStack[eventStack.size() - 2].return_value = temp;
					}

					if (eventStack.size() == 1)
					{
						std::string temp = wiz::load_data::LoadData::ToBool4(nullptr, global, *val, _executeData).ToString();

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
					ExecuteData _executeData; _executeData.depth = executeData.depth;
					_executeData.chkInfo = true;
					_executeData.info = eventStack.top();
					_executeData.pObjectMap = objectMapPtr;
					_executeData.pEvents = eventPtr;
					_executeData.pModule = moduleMapPtr;


					_executeData.noUseInput = executeData.noUseInput; //// check!
					_executeData.noUseOutput = executeData.noUseOutput;

					std::vector<SortInfo> siVec;
					wiz::load_data::UserType* utTemp =
						wiz::load_data::UserType::Find(&global,
							wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(0), _executeData).ToString()).second[0];

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
							ut.AddItem(siVec[i].data, static_cast<wiz::load_data::ItemType<std::string>*>(temp[siVec[i].idx])->Get(0));
						}
						else {
							ut.AddUserTypeItem(*(static_cast<wiz::load_data::UserType*>(temp[siVec[i].idx])));
						}
					}

					utTemp->Remove();

					//cf) chk? *utTemp = ut;
					wiz::load_data::LoadData::AddData(*(utTemp), "", ut.ToString(), _executeData);


					eventStack.top().userType_idx.top()++;
					break;
				}
				else if ("$sort2"sv == val->GetName()) { // colName -> just one! ? 
					ExecuteData _executeData; _executeData.depth = executeData.depth;
					_executeData.chkInfo = true;
					_executeData.info = eventStack.top();
					_executeData.pObjectMap = objectMapPtr;
					_executeData.pEvents = eventPtr;
					_executeData.pModule = moduleMapPtr;


					_executeData.noUseInput = executeData.noUseInput; //// check!
					_executeData.noUseOutput = executeData.noUseOutput;

					/// condition = has just one? in one usertype!
					std::vector<SortInfo> siVec;
					wiz::load_data::UserType* utTemp =
						wiz::load_data::UserType::Find(&global,
							wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(0), _executeData).ToString()).second[0];
					const std::string colName = wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(1), _executeData).ToString();

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
					wiz::load_data::LoadData::AddData(*(utTemp), "", ut.ToString(), _executeData);


					eventStack.top().userType_idx.top()++;
					break;
				}
				else if ("$sort2_dsc"sv == val->GetName()) { // colName -> just one! ? 
														   /// condition = has just one? in one usertype!
					ExecuteData _executeData; _executeData.depth = executeData.depth;
					_executeData.chkInfo = true;
					_executeData.info = eventStack.top();
					_executeData.pObjectMap = objectMapPtr;
					_executeData.pEvents = eventPtr;
					_executeData.pModule = moduleMapPtr;



					_executeData.noUseInput = executeData.noUseInput; //// check!
					_executeData.noUseOutput = executeData.noUseOutput;

					std::vector<SortInfo2> siVec;
					wiz::load_data::UserType* utTemp =
						wiz::load_data::UserType::Find(&global,
							wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(0), _executeData).ToString()).second[0];
					const std::string colName = wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(1), _executeData).ToString();
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
					wiz::load_data::LoadData::AddData(*(utTemp), "", ut.ToString(), _executeData);


					eventStack.top().userType_idx.top()++;
					break;
				}
				else if ("$shell_mode"sv == val->GetName()) {
					ExecuteData _executeData; _executeData.depth = executeData.depth;
					_executeData.chkInfo = true;
					_executeData.info = eventStack.top();
					_executeData.pObjectMap = objectMapPtr;
					_executeData.pEvents = eventPtr;
					_executeData.pModule = moduleMapPtr;

					_executeData.noUseInput = executeData.noUseInput; //// check!
					_executeData.noUseOutput = executeData.noUseOutput;


					// check?
					if (executeData.noUseInput || executeData.noUseOutput) {
						eventStack.top().userType_idx.top()++;
						break;
					}
#if _WIN32
					ShellMode(global, _executeData);
#endif
					eventStack.top().userType_idx.top()++;
					break;
				}
				else if ("$edit_mode"sv == val->GetName()) {
				ExecuteData _executeData; _executeData.depth = executeData.depth;
				_executeData.chkInfo = true;
				_executeData.info = eventStack.top();
				_executeData.pObjectMap = objectMapPtr;
				_executeData.pEvents = eventPtr;
				_executeData.pModule = moduleMapPtr;

				_executeData.noUseInput = executeData.noUseInput; //// check!
				_executeData.noUseOutput = executeData.noUseOutput;


				// check?
				if (executeData.noUseInput || executeData.noUseOutput) {
					eventStack.top().userType_idx.top()++;
					break;
				}
#if _WIN32
				MStyleTest(&global, _executeData);
#endif
				eventStack.top().userType_idx.top()++;
				break;
				}
				// removal?
				else if ("$stable_sort"sv == val->GetName()) {
					ExecuteData _executeData; _executeData.depth = executeData.depth;
					_executeData.chkInfo = true;
					_executeData.info = eventStack.top();
					_executeData.pObjectMap = objectMapPtr;
					_executeData.pEvents = eventPtr;
					_executeData.pModule = moduleMapPtr;

					_executeData.noUseInput = executeData.noUseInput; //// check!
					_executeData.noUseOutput = executeData.noUseOutput;

					// todo
					// todo
					std::vector<SortInfo> siVec;
					wiz::load_data::UserType* utTemp =
						wiz::load_data::UserType::Find(&global,
							wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(0), _executeData).ToString()).second[0];

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
							ut.AddItem((siVec[i].data), static_cast<wiz::load_data::ItemType<std::string>*>(temp[siVec[i].idx])->Get(0));
						}
						else {
							ut.AddUserTypeItem(*(static_cast<wiz::load_data::UserType*>(temp[siVec[i].idx])));
						}
					}

					utTemp->Remove();

					wiz::load_data::LoadData::AddData(*(utTemp), "", ut.ToString(), _executeData);


					eventStack.top().userType_idx.top()++;
					break;
				}
				else if ("$if"sv == val->GetName()) // ToDo!!
				{
					ExecuteData _executeData; _executeData.depth = executeData.depth;
					_executeData.chkInfo = true;
					_executeData.info = eventStack.top();
					_executeData.pObjectMap = objectMapPtr;
					_executeData.pEvents = eventPtr;
					_executeData.pModule = moduleMapPtr;


					_executeData.noUseInput = executeData.noUseInput; //// check!
					_executeData.noUseOutput = executeData.noUseOutput;

					std::string cond;
					cond = wiz::load_data::LoadData::ToBool4(nullptr, global, *val->GetUserTypeList(0), _executeData).ToString();


					if (!eventStack.top().conditionStack.empty())
					{
						if ("TRUE" == cond && eventStack.top().conditionStack.top() == "FALSE")
						{
							cond = "FALSE";
						}
						else if ("FALSE" == cond && eventStack.top().conditionStack.top() == "FALSE")
						{
							cond = "FALSE";
						}
						else if (!eventStack.top().nowUT.empty() && eventStack.top().userType_idx.top() + 1 < eventStack.top().nowUT.top()->GetUserTypeListSize()
							&& (eventStack.top().nowUT.top()->GetUserTypeList(eventStack.top().userType_idx.top() + 1)->GetName() == "$else"))
						{
							eventStack.top().conditionStack.push(cond);
						}
						else if ("TRUE" == cond)
						{
							eventStack.top().conditionStack.push(cond);
						}
					}
					else
					{
						if (eventStack.top().userType_idx.top() + 1 < eventPtr->GetUserTypeList(no)->GetUserTypeListSize() &&
							eventPtr->GetUserTypeList(no)->GetUserTypeList(eventStack.top().userType_idx.top() + 1)->GetName() == "$else")
						{
							eventStack.top().conditionStack.push(cond);
						}
						else if ("TRUE" == cond)
						{
							eventStack.top().conditionStack.push(cond);
						}
					}

					if ("TRUE" == cond)
					{
						eventStack.top().nowUT.push(val->GetUserTypeList(1));
						//val = eventStack.top().nowUT.top()->GetUserTypeList(0); 
						eventStack.top().userType_idx.top()++;
						eventStack.top().userType_idx.push(0);
						//eventStack.top().state.push(1);
						//state = 1;
						break;
					}//
					else if ("FALSE" == cond)
					{
						eventStack.top().userType_idx.top()++;
						break;
					}
					else
					{
						// debug..
						if (!executeData.noUseOutput) {
							wiz::Out << "Error Debug : " << cond << ENTER;
						}
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
					if (!executeData.noUseOutput) {
						wiz::Out << "it does not work. : " << wiz::ToString(val->GetName()) << ENTER;
					}
					eventStack.top().userType_idx.top()++;
					break;
				}
			}
		}
	}

	if (1 == chk && !events.empty()) {
		auto _events = events.GetUserTypeItemIdx("Event");
		for (int i = 0; i < _events.size(); ++i) {
			if (events.GetUserTypeList(_events[i])->GetItem("id")[0].Get() == "NONE"sv) {
				continue;
			}
			_global->LinkUserType(events.GetUserTypeList(_events[i]));
			events.GetUserTypeList(_events[i]) = nullptr;
		}
		events.RemoveUserTypeList("Event");
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
void ClauText::ShellMode(wiz::load_data::UserType& global, const ExecuteData& executeData) {
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
				global.Save1(std::cout); /// todo - change cout?
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
							const std::string result = execute_module("Main = { $call = { id = " + id + "} }", &ut, ExecuteData(executeData.noUseInput, executeData.noUseOutput), opt, 1);

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
// for $edit_mode?
void ClauText::MStyleTest(wiz::load_data::UserType* pUt, const ExecuteData& executeData)
{
#ifdef _MSC_VER
	wiz::StringBuilder builder(1024);
	std::vector<wiz::load_data::ItemType<wiz::load_data::UserType*>> utVec;
	std::vector<MData> mdVec;
	//std::vector<vector<MData>> mdVec2;
	wiz::load_data::ItemType<wiz::load_data::UserType*> global;
	wiz::load_data::UserType* utTemp = pUt;
	std::vector<wiz::load_data::UserType*> utVec2;
	std::vector<int> idxVec; //for store before idx
	std::vector<std::string> strVec; // for value..
	int braceNum = 0;
	int idx = 0;
	bool isFirst = true;
	bool isReDraw = true;
	int sizeOfWindow = 30;
	size_t Start = 0;
	size_t End = 0;
	int state = 0;

	global.Push(utTemp);

	utVec.push_back(global);

	utVec2.push_back(utTemp);

	system("cls");

	int count_userType = 0;
	int count_item = 0;

	while (true) {
		if (isFirst) {
			mdVec = std::vector<MData>();
			count_userType = 0;
			count_item = 0;

			for (int h = 0; h < utVec[braceNum].size(); ++h) {
				for (int i = 0; i < utVec[braceNum].Get(h)->GetUserTypeListSize(); ++i) {
					MData mdTemp{ true, wiz::ToString(utVec[braceNum].Get(h)->GetUserTypeList(i)->GetName()), h };
					if (mdTemp.varName.empty()) {
						mdTemp.varName = " ";
					}

					mdVec.push_back(mdTemp);
					count_userType++;
				}
			}
			for (int h = 0; h < utVec[braceNum].size(); ++h) {
				for (int i = 0; i < utVec[braceNum].Get(h)->GetItemListSize(); ++i) {
					MData mdTemp{ false, wiz::ToString(utVec[braceNum].Get(h)->GetItemList(i).GetName()), h };
					if (mdTemp.varName.empty()) {
						mdTemp.varName = " ";
					}
					mdVec.push_back(mdTemp);
					count_item++;
				}
			}
			isFirst = false;
		}
		if (isReDraw) {
			setcolor(0, 0);
			system("cls");

			End = std::min(Start + sizeOfWindow - 1, mdVec.size() - 1);
			if (mdVec.empty()) {
				End = Start - 1;
			}
			// draw mdVec and cursor - chk!!
			else {
				for (int i = Start; i <= End; ++i) {
					if (mdVec[i].isDir) { setcolor(0, 10); }
					else { setcolor(0, 7); }
					if (false == mdVec[i].varName.empty()) {
						wiz::Out << "  " << mdVec[i].varName;
					}
					else
					{
						wiz::Out << "  " << " ";
					}
					if (i != mdVec.size() - 1) { wiz::Out << ENTER; }
				}

				gotoxy(0, idx - Start);

				setcolor(0, 12);
				wiz::Out << "●";
				setcolor(0, 0);
				gotoxy(0, 0);
			}

			isReDraw = false;
		}

		// std::move and chk enterkey. - todo!!
		{
			FFLUSH();
			char ch = GETCH();
			FFLUSH();

			if ('q' == ch) { return; }

			// todo - add, remove, save
			if (strVec.empty() && Start <= End && idx > 0 && ('w' == ch || 'W' == ch))
			{
				// draw mdVec and cursor - chk!!
				if (idx == Start) {
					system("cls");

					int count = 0;
					int newEnd = Start - 1;
					int newStart = std::max(0, newEnd - sizeOfWindow + 1);

					Start = newStart; End = newEnd;
					idx--;

					for (int i = Start; i <= End; ++i) {
						if (mdVec[i].isDir) { setcolor(0, 10); }
						else { setcolor(0, 7); }
						wiz::Out << "  " << mdVec[i].varName;
						if (mdVec[i].varName.empty()) { wiz::Out << " "; }
						if (i != mdVec.size() - 1) { wiz::Out << ENTER; }
						count++;
					}
					gotoxy(0, idx - Start);
					setcolor(0, 12);
					wiz::Out << "●";
					setcolor(0, 0);
				}
				else {
					gotoxy(0, idx - Start);
					setcolor(0, 0);
					wiz::Out << "  ";
					idx--;

					gotoxy(0, idx - Start);
					setcolor(0, 12);
					wiz::Out << "●";
					setcolor(0, 0);
				}
			}
			else if (
				strVec.empty() && Start <= End && (idx < mdVec.size() - 1)
				&& ('s' == ch || 'S' == ch)
				)
			{
				if (idx == End) {
					system("cls");

					int count = 0;
					size_t newStart = End + 1;
					size_t newEnd = std::min(newStart + sizeOfWindow - 1, mdVec.size() - 1);

					Start = newStart; End = newEnd;
					idx++;

					for (int i = Start; i <= End; ++i) {
						if (mdVec[i].isDir) { setcolor(0, 10); }
						else { setcolor(0, 7); }
						wiz::Out << "  " << mdVec[i].varName;
						if (i != mdVec.size() - 1) { wiz::Out << ENTER; }
						count++;
					}
					gotoxy(0, 0);
					setcolor(0, 12);
					wiz::Out << "●";
					setcolor(0, 0);
				}
				else {
					gotoxy(0, idx - Start);
					setcolor(0, 0);
					wiz::Out << "  ";
					idx++;

					gotoxy(0, idx - Start);
					setcolor(0, 12);
					wiz::Out << "●";
					setcolor(0, 0);
				}
			}
			if (!strVec.empty() && Start <= End && idx > 0 && ('w' == ch || 'W' == ch))
			{
				// draw mdVec and cursor - chk!!
				if (idx == Start) {
					system("cls");

					int count = 0;
					int newEnd = Start - 1;
					int newStart = std::max(0, newEnd - sizeOfWindow + 1);

					Start = newStart; End = newEnd;
					idx--;

					for (int i = Start; i <= End; ++i) {
						setcolor(0, 7);
						wiz::Out << "  " << strVec[i];
						if (i != strVec.size() - 1) { wiz::Out << ENTER; }
						count++;
					}
					gotoxy(0, idx - Start);
					setcolor(0, 12);
					wiz::Out << "●";
					setcolor(0, 0);
				}
				else {
					gotoxy(0, idx - Start);
					setcolor(0, 0);
					wiz::Out << "  ";
					idx--;

					gotoxy(0, idx - Start);
					setcolor(0, 12);
					wiz::Out << "●";
					setcolor(0, 0);
				}
			}
			else if (
				!strVec.empty() && Start <= End && (idx < strVec.size() - 1)
				&& ('s' == ch || 'S' == ch)
				)
			{
				if (idx == End) {
					setcolor(0, 0);
					system("cls");

					int count = 0;
					size_t newStart = End + 1;
					size_t newEnd = std::min(newStart + sizeOfWindow - 1, strVec.size() - 1);

					Start = newStart; End = newEnd;
					idx++;

					for (int i = Start; i <= End; ++i) {
						setcolor(0, 7);
						wiz::Out << "  " << strVec[i];
						if (i != strVec.size() - 1) { wiz::Out << ENTER; }
						count++;
					}
					gotoxy(0, 0);
					setcolor(0, 12);
					wiz::Out << "●";
					setcolor(0, 0);
				}
				else {
					gotoxy(0, idx - Start);
					setcolor(0, 0);
					wiz::Out << "  ";
					idx++;

					gotoxy(0, idx - Start);
					setcolor(0, 12);
					wiz::Out << "●";
					setcolor(0, 0);
				}
			}
			else if ('\r' == ch || '\n' == ch) {
				/// To Do..
				gotoxy(0, 0);
				if (count_userType == 0 && count_item == 0)
				{
					//
				}
				else if (state == 0 && strVec.empty() && idx < count_userType) { // utVec[braceNum].Get(mdVec[idx].no)->GetUserTypeListSize()) {
					setcolor(0, 0);
					system("cls");

					// usertypelist
					braceNum++;
					idxVec.push_back(idx); /// idx?

					if (braceNum >= utVec.size()) {
						utVec.push_back(wiz::load_data::ItemType<wiz::load_data::UserType*>());
						utVec2.push_back(nullptr);
					}

					wiz::load_data::ItemType< wiz::load_data::UserType*> ref;
					ref.Push(nullptr);
					std::string strTemp = mdVec[idxVec[braceNum - 1]].varName;
					if (strTemp == " " || strTemp == "_")
					{
						strTemp = "";
					}

					if (utVec[braceNum - 1].Get(mdVec[idxVec[braceNum - 1]].no)->GetUserTypeItemRef(idxVec[braceNum - 1], ref.Get(0)))
					{
						//
					}
					utVec[braceNum] = ref;

					utVec2[braceNum - 1]->GetUserTypeItemRef(idxVec[braceNum - 1], ref.Get(0));
					utVec2[braceNum] = ref.Get(mdVec[idxVec[braceNum - 1]].no);

					Start = 0;
					idx = 0;
					isFirst = true;
					isReDraw = true;
				}
				else
				{
					if (
						!mdVec.empty() &&
						strVec.empty()
						)
					{
						setcolor(0, 0);
						system("cls");

						std::string strTemp = mdVec[idx].varName;
						if (strTemp == " " || strTemp == "_") { strTemp = ""; }
						const int count = 1; // utVec[braceNum].Get(mdVec[idx].no)->GetItem(strTemp).size();
						setcolor(0, 7);

						for (int i = 0; i < count; ++i) {
							setcolor(0, 7);

							auto x = utVec[braceNum].Get(mdVec[idx].no)->GetItemList(idx - count_userType);
							std::string temp = wiz::ToString(x.Get(0));
							wiz::Out << "  " << temp;
							strVec.push_back(temp);
							//}
							if (i != count - 1) { wiz::Out << ENTER; }
						}
					}

					if (state == 0) {
						gotoxy(0, 0);
						state = 1;

						idxVec.push_back(idx);

						idx = 0;
						Start = 0;

						setcolor(0, 12);
						wiz::Out << "●";
						setcolor(0, 0);
					}
					else if (state == 1) { /// cf) state = 2;
						gotoxy(0, idx); /// chk..
						Start = idx;
					}


					// idx = 0;
					End = std::min(Start + sizeOfWindow - 1, strVec.size() - 1);
					if (strVec.empty()) { End = Start - 1; }


					// chk
					count_userType = 0;
					count_item = 1;
				}
			}
			else if (0 == state && 'f' == ch)
			{
				std::string temp;
				int x = 0;
				system("cls");
				setcolor(0, 7);
				wiz::Out << "row input : ";
				std::cin >> temp;
				FFLUSH();

				if (wiz::load_data::Utility::IsInteger(temp)) {
					x = stoi(temp); // toInt
					if (x < 0) { x = 0; }
					else if (x >= mdVec.size()) {
						x = mdVec.size() - 1;
					}

					// chk!! ToDo ?
					idx = x;
					x = std::max(0, x - sizeOfWindow / 2);
					Start = x;
					isReDraw = true; /// chk!! To Do - OnlyRedraw? Reset? // int + changed?
				}
				else
				{
					isReDraw = true; /// OnlyDraw = true?, no search?
				}
			}
			else if (0 == state && '1' == ch)
			{
				int index = idx;
				std::string temp = mdVec[idx].varName;

				for (int i = idx - 1; i >= 0; --i) {
					if (mdVec[i].varName == temp) {
						index = i;
					}
					else
					{
						break;
					}
				}

				idx = index;
				Start = std::max(0, idx - sizeOfWindow / 2);
				isReDraw = true;
			}
			else if (0 == state && '2' == ch)
			{
				int index = idx;
				std::string temp = mdVec[idx].varName;

				for (int i = idx + 1; i < mdVec.size(); ++i) {
					if (mdVec[i].varName == temp) {
						index = i;
					}
					else
					{
						break;
					}
				}

				idx = index;
				Start = std::max(0, idx - sizeOfWindow / 2);
				isReDraw = true;

			}
			else {
				if ('q' == ch) { return; } // quit
				else if ('b' == ch && braceNum > 0 && strVec.empty() && state == 0) {  // back
					braceNum--;

					setcolor(0, 0);
					system("cls");

					isFirst = true;
					isReDraw = true;
					//Start = idxVec.back();

					idx = idxVec.back();
					idxVec.pop_back();

					if (0 <= idx - sizeOfWindow / 2)
					{
						Start = idx - sizeOfWindow / 2;
					}
					else {
						Start = 0;
					}
				}
				else if ('b' == ch && !idxVec.empty()) /// state == 1 ?
				{
					idx = idxVec.back();
					idxVec.pop_back();

					if (0 <= idx - sizeOfWindow / 2)
					{
						Start = idx - sizeOfWindow / 2;
					}
					else {
						Start = 0;
					}

					state = 0;
					setcolor(0, 0);
					system("cls");
					strVec.clear();

					isFirst = true;
					isReDraw = true;
				}
				else if ('e' == ch) {
					setcolor(0, 0);
					system("cls");
					setcolor(7, 0);

					wiz::Out << "edit mode" << ENTER;
					wiz::Out << "add - a, change - c, remove - r, save - s" << ENTER;

					//GETCH(); // why '\0' or 0?
					ch = GETCH();
					FFLUSH();

					if ('a' == ch) { // add
						int select = -1;
						std::string var;
						std::string val;

						setcolor(0, 7);
						// need more test!!
						wiz::Out << "add UserType : 1, add Item : 2, add usertype that name is \"\": 3 your input : ";
						std::cin >> select;
						FFLUSH();

						// add userType?
						if (1 == select) {
							// name of UserType.
							wiz::Out << "what is new UserType name? : ";
							std::cin >> var;
							FFLUSH();
							utVec2[braceNum]->AddUserTypeItem(wiz::load_data::UserType(std::move(var)));
						}
						// addd Item?
						else if (2 == select) {
							// var, val /// state에 따라?
							wiz::Out << "var : ";
							std::cin >> var;
							wiz::Out << "val : ";
							FFLUSH();
							std::getline(std::cin, val);
							utVec2[braceNum]->AddItem(std::move(var), std::move(val));
						}
						else if (3 == select)
						{
							utVec2[braceNum]->AddUserTypeItem(wiz::load_data::UserType(""));
						}
					}
					else if ('c' == ch && Start <= End) { // change var or value
														  // idx
						if (idx < count_userType) {
							std::string temp;
							setcolor(0, 7);
							wiz::Out << "change userType name : ";

							FFLUSH();
							std::getline(std::cin, temp);

							int count = 0;
							for (int h = 0; h < utVec[braceNum].size(); ++h) {
								for (int i = 0; i < utVec[braceNum].Get(h)->GetUserTypeListSize(); ++i) {
									if (count == idx) {
										utVec[braceNum].Get(h)->GetUserTypeList(i)->SetName(std::move(temp));
									}
									count++;
								}
							}
						}
						else {
							std::string temp;
							setcolor(0, 7);

							std::string name, value;
							if (1 == state) { // val
								wiz::Out << "change val : " << ENTER;
								std::getline(std::cin, temp);

								value = temp;

								int count = 0;
								count_userType = 0; // int ~ ?
								for (int h = 0; h < utVec[braceNum].size(); ++h) {
									for (int i = 0; i < utVec[braceNum].Get(h)->GetUserTypeListSize(); ++i) {
										count++;
										count_userType++;
									}
								}
								for (int h = 0; h < utVec[braceNum].size(); ++h) {
									for (int i = 0; i < utVec[braceNum].Get(h)->GetItemListSize(); ++i) {
										if (idxVec.back() == count) {
											utVec[braceNum].Get(h)->GetItemList(i).Get(idx) = std::move(value);
										}
										count++;
									}
								}
								idx = idxVec.back();
								idxVec.pop_back();
								// max!
								if (0 <= idx - sizeOfWindow / 2)
								{
									Start = idx - sizeOfWindow / 2;
								}
								else {
									Start = 0;
								}
								strVec.clear();
								state = 0;
							}
							else if (0 == state) { // var
								wiz::Out << "change var : " << ENTER;
								std::cin >> temp;
								FFLUSH();
								name = temp;
								int count = 0;
								for (int h = 0; h < utVec[braceNum].size(); ++h) {
									for (int i = 0; i < utVec[braceNum].Get(h)->GetUserTypeListSize(); ++i) {
										count++;
									}
								}
								for (int h = 0; h < utVec[braceNum].size(); ++h) {
									for (int i = 0; i < utVec[braceNum].Get(h)->GetItemListSize(); ++i) {
										if (idx == count) {
											utVec[braceNum].Get(h)->GetItemList(i).SetName(std::move(name));
										}
										count++;
									}
								}
							}
						}
					}
					else if ('r' == ch && Start <= End) { // remove
						if (idx < count_userType)
						{
							int count = 0;
							for (int h = 0; h < utVec[braceNum].size(); ++h) {
								for (int i = 0; i < utVec[braceNum].Get(h)->GetUserTypeListSize(); ++i) {
									if (count == idx) {
										std::string temp = mdVec[idx].varName;
										if (temp == " " || temp == "_") {
											temp = "";
										}
										utVec[braceNum].Get(h)->RemoveUserTypeList(std::move(temp));
									}
									count++;
								}
							}
							idx = 0;
							Start = 0;
						}
						else {
							if (state == 0) {
								int count = 0;
								int count_ut = 0;
								for (int h = 0; h < utVec[braceNum].size(); ++h) {
									for (int i = 0; i < utVec[braceNum].Get(h)->GetUserTypeListSize(); ++i) {
										count++;
										count_ut++;
									}
								}
								for (int h = 0; h < utVec[braceNum].size(); ++h) {
									for (int i = 0; i < utVec[braceNum].Get(h)->GetItemListSize(); ++i) {
										if (count == idx) {
											std::string temp = mdVec[idx].varName;
											if (temp == " " || temp == "_") { temp = ""; }

											utVec[braceNum].Get(h)->RemoveItemList(std::move(temp));
										}
										count++;
									}
								}
								idx = 0;
								Start = 0;
							}
							else if (1 == state)
							{
								int count = 0;
								int count_ut = 0;
								for (int h = 0; h < utVec[braceNum].size(); ++h) {
									for (int i = 0; i < utVec[braceNum].Get(h)->GetUserTypeListSize(); ++i) {
										count++;
										count_ut++;
									}
								}
								for (int h = 0; h < utVec[braceNum].size(); ++h) {
									for (int i = 0; i < utVec[braceNum].Get(h)->GetItemListSize(); ++i) {
										if (count == idxVec.back()) {
											utVec[braceNum].Get(h)->GetItemList(count - count_ut).Remove(0);
											if (utVec[braceNum].Get(h)->GetItemList(count - count_ut).size() == 0) {
												utVec[braceNum].Get(h)->GetItemList(count - count_ut).Remove();
												utVec[braceNum].Get(h)->RemoveEmptyItem();
											}
										}
										count++;
									}
								}

								idxVec.pop_back();
								idx = 0;
								// max!
								if (0 <= idx - sizeOfWindow / 2)
								{
									Start = idx - sizeOfWindow / 2;
								}
								else {
									Start = 0;
								}
								strVec.clear();
								state = 0;
							}
						}
					}
					else if ('s' == ch) { // save total data.
						std::string temp;

						setcolor(0, 7);
						wiz::Out << "save file name : ";
						FFLUSH();
						std::getline(std::cin, temp);

						wiz::load_data::LoadData::SaveWizDB(*utTemp, temp, "1");
					}

					if (1 == state)
					{
						idxVec.back();
						idxVec.pop_back();
						idx = 0;
						// max!
						if (0 <= idx - sizeOfWindow / 2)
						{
							Start = idx - sizeOfWindow / 2;
						}
						else {
							Start = 0;
						}
						strVec.clear();
						state = 0;
					}

					/// else if( l? reload?
					isFirst = true; // 
					isReDraw = true; //
				}
				else if ('t' == ch && braceNum == 0) { // pass???
					isFirst = true;
					isReDraw = true;
					setcolor(0, 0);
					system("cls");

					setcolor(7, 0);
					wiz::Out << "text edit mode" << ENTER;

					// Add, AddUserType, Set, Remove, RemoveAll ?.
					std::string temp;
					FFLUSH();
					std::getline(std::cin, temp);

					std::vector<std::string> strVecTemp = wiz::tokenize(temp, '|');

					if (!strVecTemp.empty()) {
						try {
							if ("add" == strVecTemp[0])
							{
								if (false == wiz::load_data::LoadData::AddData(*utTemp, strVecTemp[1], strVecTemp[2], ExecuteData(executeData.noUseInput, executeData.noUseOutput)))
								{
									wiz::Out << "fail to add" << ENTER; /// To Do to following code.
								}
							}
							else if ("addusertype" == strVecTemp[0])
							{
								wiz::load_data::LoadData::AddUserType(*utTemp, strVecTemp[1], strVecTemp[2], strVecTemp[3], ExecuteData(executeData.noUseInput, executeData.noUseOutput));
							}
							else if ("set" == strVecTemp[0])
							{
								wiz::load_data::LoadData::SetData(*utTemp, strVecTemp[1], strVecTemp[2], strVecTemp[3], ExecuteData(executeData.noUseInput, executeData.noUseOutput));
							}
							else if ("remove" == strVecTemp[0])
							{
								wiz::load_data::LoadData::Remove(*utTemp, strVecTemp[1], strVecTemp[2], ExecuteData(executeData.noUseInput, executeData.noUseOutput));
							}
							//else if ("removenonameitem" == strVecTemp[0])
							//{
							//	wiz::load_data::LoadData::RemoveNoNameItem(*utTemp, strVecTemp[1], strVecTemp[2]);
							//}
							else if ("removeall" == strVecTemp[0])
							{
								wiz::load_data::LoadData::Remove(*utTemp, strVecTemp[1], strVecTemp[2], ExecuteData(executeData.noUseInput, executeData.noUseOutput));
							}
							else if ("searchitem" == strVecTemp[0])
							{
								wiz::Out << wiz::load_data::LoadData::SearchItem(*utTemp, strVecTemp[1], ExecuteData(executeData.noUseInput, executeData.noUseOutput)) << ENTER;
								GETCH();
							}
							else if ("searchusertype" == strVecTemp[0])
							{
								wiz::Out << wiz::load_data::LoadData::SearchUserType(*utTemp, strVecTemp[1], ExecuteData(executeData.noUseInput, executeData.noUseOutput)) << ENTER;
								GETCH();
							}
						}
						catch (std::exception& e) {}
						catch (wiz::Error& e) {}
						catch (const char* e) {}
						catch (const std::string& e) {}
					}
					//
					idx = 0;
					Start = 0;
				}
			}
		}
	}
#endif
}

}

