
#ifndef LOAD_DATA_TYPES_H
#define LOAD_DATA_TYPES_H

#include <string>
#include <vector>
#include <list>
#include <regex>

#include "global.h"
#include "newarrays.h"
#include "queues.h"
#include "cpp_string.h"

namespace wiz {
	namespace load_data {
		class Type {
		public:
			mutable WIZ_STRING_TYPE toBool4;
		protected:
			WIZ_STRING_TYPE name;

			void chk() {
				/*
				if ("_" == name || (String::startsWith(name, "$ut") && name.size() > 3) || (String::startsWith(name, "$it") && name.size() > 3)) {
					//std::cout << "name is " << name << ENTER;
					//std::cout << "in funciton chk() in Type" << ENTER;
					GETCH();
					throw "ERROR for name in Type";
				}
				*/
			}
		public:
			explicit Type(const WIZ_STRING_TYPE& name = "", const bool valid = true) : name(name) { }//chk();  }
			explicit Type(WIZ_STRING_TYPE&& name, const bool valid = true) : name(std::move(name)) { }//chk(); }
			Type(const Type& type)
				: name(type.name), toBool4(type.toBool4)
			{
				//chk();
			}
			virtual ~Type() { }
			bool IsFail() const { // change body
				return "" == name;
			}
			const WIZ_STRING_TYPE& GetName()const {
				return name;
			}
			WIZ_STRING_TYPE& GetName() {
				return name;
			}
			void SetName(const WIZ_STRING_TYPE& name)
			{
				this->name = name;

				//chk();
			}
			void SetName(WIZ_STRING_TYPE&& name)
			{
				this->name = name;

				//chk();
			}
			bool operator==(const Type& t) const {
				return name == t.name;
			}
			bool operator<(const Type& t) const {
				return name.ToString() < t.name.ToString();
			}
			Type& operator=(const Type& type)
			{
				name = type.name;
				this->toBool4 = type.toBool4;
				return *this;
			}
			void operator=(Type&& type)
			{
				name = std::move(type.name);
				this->toBool4 = std::move(type.toBool4);
			}
			virtual bool IsItemType()const = 0;
			virtual bool IsUserType()const {
				return !IsItemType();
			}
		};

		template < class T >
		class ItemType : public Type {
		public:
			typedef T item_type; //
		private:
			//std::vector<T> arr;
			T data;
			bool inited;
		public:
			
			ItemType(ItemType<T>&& ta) 
				: Type(ta)
			{
 				data = std::move(ta.data);
				inited = ta.inited;
			}
			ItemType(const ItemType<T>& ta) : Type(ta), data(ta.data), inited(ta.inited)
			{
				//
			}
		public:
			explicit ItemType()
				: Type("", true), inited(false) { }
			explicit ItemType(const WIZ_STRING_TYPE& name, const T& value, const bool valid=true)
				:Type(name, valid), data(value), inited(true)
			{
				//
			}
			explicit ItemType(WIZ_STRING_TYPE&& name, T&& value, const bool valid = true)
				:Type(std::move(name), valid), data(std::move(value)), inited(true)
			{
				//
			}
			virtual ~ItemType() { 
				////std::cout << "chk" << "\n";
			}
		public:
			void Remove(const int idx = 0)
			{
				data = T();
				inited = false;
			}
			bool Push(const T& val) { /// do not change..!!
				if (inited) { throw "ItemType already inited"; }
				data = val;
				inited = true;

				return true;
			}
			bool Push(T&& val) {
				if (inited) { throw "ItemType already inited"; }
				data = std::move(val);
				inited = true;

				return true;
			}
			T& Get(const int index=0) {
				if (!inited) {
					throw "ItemType, not inited";  }
				return data;
			}
			const T& Get(const int index=0) const {
				if (!inited) { 
					throw "ItemType, not inited"; }
				return data;
			}
			void Set(const int index, const T& val) {
				if (!inited) { 
					throw "ItemType, not inited"; } // removal?
				data = val;
			}
			void Set(const int index, T&& val) {
				if (!inited) { 
					throw "ItemType, not inited"; } // removal?
				data = std::move(val);
			}
			int size()const {
				return inited? 1 : 0;
			}
			bool empty()const { return !inited; }
			std::string ToString()const
			{
				if (wiz::ToString(Type::GetName()).empty()) {
					return wiz::ToString(Get(0));
				}
				return wiz::ToString(Type::GetName()) + " = " + wiz::ToString(Get(0));
			}
		public:
			ItemType<T>& operator=(const ItemType<T>& ta)
			{
				if (this == &ta) {
					return *this;
				}

				(Type&)(*this) = (const Type&)ta;
				data = ta.data;
				inited = ta.inited;
				return *this;
			}
			ItemType<T>& operator=(ItemType<T>&& ta)
			{
				if (this == &ta) { return *this; }

				(Type&)(*this) = (Type&&)ta;
				data = std::move(ta.data);
				inited = ta.inited;
				return *this;
			}


			virtual bool IsItemType()const {
				return true;
			}
		};

		class UserType : public Type {
		private:
			class Wrap {
			public:
				UserType* ut = nullptr;
				size_t idx = 0;
				size_t max = 0;
			public:
				Wrap() { }
				Wrap(UserType* ut) :
					ut(ut)
				{
					max = ut->GetUserTypeListSize();
				}
			};
			class Wrap2 {
			public:
				const UserType* ut = nullptr;
				size_t idx = 0;
				size_t max = 0;
			public:
				Wrap2() { }
				Wrap2(const UserType* ut) :
					ut(ut)
				{
					max = ut->GetUserTypeListSize();
				}
			};

			static void Delete(void* ptr) {
				std::vector<Wrap> _stack;
				
				_stack.push_back(Wrap((UserType*)ptr));

				while (!_stack.empty()) {
					if (_stack.back().idx >= _stack.back().max) {
						_stack.back().ut->userTypeList.clear();
						delete _stack.back().ut;

						_stack.pop_back();
						if (_stack.empty()) {
							break;
						}
						_stack.back().idx++;
						continue;
					}

					_stack.push_back(Wrap(_stack.back().ut->GetUserTypeList(_stack.back().idx)));
				}
			}


			class UserTypeCompare
			{
			public:
				bool operator() (const UserType* x, const UserType* y) const {
					return x->GetName().ToString() < y->GetName().ToString();
				}
			};
			
			class ItemTypeStringPtrCompare {
			public:
				bool operator() (const ItemType<WIZ_STRING_TYPE>* x, const ItemType<WIZ_STRING_TYPE>* y) const {
					return x->GetName().ToString() < y->GetName().ToString();
				}
			};
			
			class ItemTypeStringCompare {
			public:
				bool operator() (const ItemType<WIZ_STRING_TYPE>& x, const ItemType<WIZ_STRING_TYPE>& y) const {
					auto a = x.GetName().ToString();
					auto b = y.GetName().ToString();

					if (a == b) {
						return x.Get().ToString() < y.Get().ToString();
					}
					return a < b;
				}
			};
		public:
			void sort_item_list() {
				if (!useSortedItemList2) {
					std::sort(this->itemList.begin(), this->itemList.end(), ItemTypeStringCompare());
					useSortedItemList2 = true;
				}
			}
		private:
			int binary_find_ut(const std::vector<UserType*>& arr, std::string x) const
			{
				if (arr.empty()) { return -1; }

				int left = 0, right = arr.size() - 1;
				int middle = (left + right) / 2;

				while (left <= right) {
					long long test = strcmp(arr[middle]->GetName().ToString().c_str(), x.data());
					if (0 == test) {
						return middle;
					}
					else if (test < 0) {
						left = middle + 1;
					}
					else {
						right = middle - 1;
					}


					middle = (left + right) / 2;
				}
				return -1;
			}
			int binary_find_it(const std::vector<ItemType<WIZ_STRING_TYPE>*>& arr, const ItemType<WIZ_STRING_TYPE>& x) const {
				if (arr.empty()) { return -1; }

				int left = 0, right = arr.size() - 1;
				int middle = (left + right) / 2;

				while (left <= right) {
					long long test = strcmp(arr[middle]->GetName().ToString().c_str(), x.GetName().ToString().c_str());
					if (0 == test) {
						return middle;
					}
					else if (test < 0) {
						left = middle + 1;
					}
					else {
						right = middle - 1;
					}

					middle = (left + right) / 2;
				}
				return -1;
			}
		public:
			virtual bool IsItemType()const {
				return false;
			}
		public:
			size_t GetIListSize()const { return ilist.size(); }
			size_t GetItemListSize()const { return itemList.size(); }
			size_t GetUserTypeListSize()const { return userTypeList.size(); }
			ItemType<WIZ_STRING_TYPE>& GetItemList(const int idx) { return itemList[idx]; }
			const ItemType<WIZ_STRING_TYPE>& GetItemList(const int idx) const { return itemList[idx]; }
			UserType*& GetUserTypeList(const int idx) { return userTypeList[idx]; }
			const UserType*& GetUserTypeList(const int idx) const { return const_cast<const UserType*&>(userTypeList[idx]); }

			bool IsItemList(const int idx) const
			{
				return ilist[idx] == 1;
			}
			bool IsUserTypeList(const int idx) const
			{
				return ilist[idx] == 2;
			}
			void AddItem(const char* str1, size_t len1, const char* str2, size_t len2) {
				itemList.emplace_back(WIZ_STRING_TYPE(std::string(str1, len1)), WIZ_STRING_TYPE(std::string(str2, len2)));
				ilist.push_back(1);

				useSortedItemList2 = false;
			}

			void AddItem(const char* str1, size_t len1, const LineInfo& info1, const char* str2, size_t len2, const LineInfo& info2) {
				itemList.emplace_back(WIZ_STRING_TYPE(std::string(str1, len1), info1), WIZ_STRING_TYPE(std::string(str2, len2), info2));
				ilist.push_back(1);

				useSortedItemList2 = false;
			}
			void AddItemType(const ItemType<WIZ_STRING_TYPE>& strTa)
			{
				for (int i = 0; i < strTa.size(); ++i) {
					this->AddItem(strTa.GetName(), strTa.Get(i));
				}
			}
			void AddItemType(ItemType<WIZ_STRING_TYPE>&& strTa)
			{
				for (int i = 0; i < strTa.size(); ++i) {
					this->AddItem(std::move(strTa.GetName()), std::move(strTa.Get(i)));
				}
			}
		public:
			void SetParent(UserType* other)
			{
				parent = other;
			}
			UserType* GetParent() { return parent; }
			const UserType* GetParent()const { return parent; }

			void LinkUserType(UserType* ut) // danger fucntion?
			{
				userTypeList.push_back(ut);
				ilist.push_back(2);
				ut->parent = this;

				useSortedUserTypeList = false;
			}
		private:
			int before_pos = -1;

			UserType* parent = nullptr;
			std::vector<int> ilist;
			std::vector< ItemType<WIZ_STRING_TYPE> > itemList;
			std::vector< UserType* > userTypeList;
			mutable std::vector< UserType* > sortedUserTypeList;
			mutable bool useSortedItemList2 = false;
			mutable bool useSortedUserTypeList = false;
			bool isVirtual = false;
			bool isObject = true;

		public:
			bool IsObject() const {
				return isObject;
			}
			bool IsArray() const {
				return !isObject;
			}
			void SetIsObject(bool val) {
				isObject = val;
			}
			void SetIsVirtual(bool val) {
				isVirtual = val;
			}
			bool IsVirtual() const {
				return isVirtual;
			}
		public:
			explicit UserType(const char* str, size_t len) : Type(WIZ_STRING_TYPE(str, len)), parent(nullptr) {
				//
			}
			explicit UserType(const char* str, size_t len, const LineInfo& info) : Type(WIZ_STRING_TYPE(str, len, info)), parent(nullptr) {
				//
			}
			explicit UserType(WIZ_STRING_TYPE&& name, bool noRemove = false) : Type(std::move(name)), parent(nullptr) { }
			explicit UserType(const WIZ_STRING_TYPE& name = "", bool noRemove = false) : Type(name), parent(nullptr) { } //, userTypeList_sortFlagA(true), userTypeList_sortFlagB(true) { }
			UserType(const UserType& ut) : Type(ut) {
				Reset(ut);  // Initial
			}
			UserType(UserType&& ut) : Type(ut) {
				Reset2(std::move(ut));
			}
			virtual ~UserType() {
				_Remove();
			}
			UserType& operator=(const UserType& ut) {
				if (this == &ut) { return *this; }
				Type::operator=(ut);

				RemoveUserTypeList();
				Reset(ut);
				return *this;
			}
			UserType& operator=(UserType&& ut) {
				if (this == &ut) { return *this; }

				Type::operator=((ut));
				RemoveUserTypeList();
				Reset2(std::move(ut));
				return *this;
			}
			void Clear()
			{
				Remove();
			}
			void clear()
			{
				Clear();
			}
		private:
			void Reset(const UserType& ut) {
				std::vector<UserType*> _stack;
				std::vector<Wrap2> _stack2;

				_stack.push_back(this);
				_stack2.push_back(Wrap2((const UserType*)&ut));

				while (!_stack2.empty()) {
					if (_stack2.back().idx >= _stack2.back().max) {
						{
							_stack.back()->sortedUserTypeList.clear();

							_stack.back()->name = _stack2.back().ut->name;
							_stack.back()->ilist = _stack2.back().ut->ilist;
							_stack.back()->itemList = _stack2.back().ut->itemList;
							_stack.back()->useSortedItemList2 = false; // ut.useSortedItemList;
							_stack.back()->useSortedUserTypeList = false; //ut.useSortedUserTypeList;

							_stack.back()->isObject = _stack2.back().ut->isObject;
							_stack.back()->isVirtual = _stack2.back().ut->isVirtual;
						}

						{
							UserType* child = _stack.back();

							_stack.pop_back();

							if (!_stack.empty()) {
								_stack.back()->LinkUserType(child);
							}
						}

						_stack2.pop_back();
						if (_stack2.empty()) {
							break;
						}
						_stack2.back().idx++;
						continue;
					}

					{
						UserType* child = new UserType();
						_stack.push_back(child);
					}
					_stack2.push_back(Wrap2(_stack2.back().ut->GetUserTypeList(_stack2.back().idx)));
					_stack.back()->ReserveUserTypeList(_stack2.back().ut->GetUserTypeListSize());
				}
			}
			void Reset2(UserType&& ut) {
				//std::swap( userTypeList_sortFlagA, ut.userTypeList_sortFlagA );
				//std::swap( userTypeList_sortFlagB, ut.userTypeList_sortFlagB );

				//no use - //parent = ut.parent;
				//no use - //ut.parent = nullptr; /// chk..
				//SetName(ut.GetName());
				ilist = std::move(ut.ilist);
				itemList = std::move(ut.itemList);

				useSortedItemList2 = false; // fixed
				useSortedUserTypeList = false;

				std::swap(isObject, ut.isObject);
				std::swap(isVirtual, ut.isVirtual);

				userTypeList.reserve(ut.userTypeList.size());

				for (int i = 0; i < ut.userTypeList.size(); ++i) {
					userTypeList.push_back(std::move(ut.userTypeList[i]));
					ut.userTypeList[i] = nullptr;
					userTypeList.back()->parent = this;
				}
				ut.userTypeList.clear();
			}

			void _Remove()
			{
				//parent = nullptr;
				ilist = std::vector<int>(); 

				itemList = std::vector< ItemType<WIZ_STRING_TYPE> >();

				sortedUserTypeList.clear();
				useSortedItemList2 = false;
				useSortedUserTypeList = false;
				RemoveUserTypeList();
			}
		// static ??
		public: 
			int GetIlistIndex(const int index, const int type)
			{
				return _GetIlistIndex(ilist, index, type);
			}
			int GetUserTypeIndexFromIlistIndex(const int ilist_idx)
			{
				return _GetUserTypeIndexFromIlistIndex(ilist, ilist_idx);
			}
			int GetItemIndexFromIlistIndex(const int ilist_idx)
			{
				return _GetItemIndexFromIlistIndex(ilist, ilist_idx);
			}
		private:
			/// val : 1 or 2
			int _GetIndex(const std::vector<int>& ilist, const int val, const int start = 0) {
				for (int i = start; i < ilist.size(); ++i) {
					if (ilist[i] == val) { return i; }
				}
				return -1;
			}
		// test? - need more thinking!
			int _GetItemIndexFromIlistIndex(const std::vector<int>& ilist, const int ilist_idx) {
				if (ilist.size() == ilist_idx) { return ilist.size(); }
				int idx = _GetIndex(ilist, 1, 0);
				int item_idx = -1;

				while (idx != -1) {
					item_idx++;
					if (ilist_idx == idx) { return item_idx; }
					idx = _GetIndex(ilist, 1, idx + 1);
				}

				return -1;
			}
			int _GetUserTypeIndexFromIlistIndex(const std::vector<int>& ilist, const int ilist_idx) {
				if (ilist.size() == ilist_idx) { return ilist.size(); }
				int idx = _GetIndex(ilist, 2, 0);
				int usertype_idx = -1;

				while (idx != -1) {
					usertype_idx++;
					if (ilist_idx == idx) { return usertype_idx; }
					idx = _GetIndex(ilist, 2, idx + 1);
				}

				return -1;
			}
			/// type : 1 or 2
			int _GetIlistIndex(const std::vector<int>& ilist, const int index, const int type) {
				int count = -1;

				for (int i = 0; i < ilist.size(); ++i) {
					if (ilist[i] == type) {
						count++;
						if (index == count) {
							return i;
						}
					}
				}
				return -1;
			}
		public:
			void RemoveItemList(const int idx)
			{
				// left shift start idx, to end, at itemList. and resize!
				for (int i = idx + 1; i < GetItemListSize(); ++i) {
					itemList[i - 1] = std::move(itemList[i]);
				}
				itemList.resize(itemList.size() - 1);
				//  ilist left shift and resize - count itemType!
				int count = 0;
				for (int i = 0; i < ilist.size(); ++i) {
					if (ilist[i] == 1) { count++; }
					if (count == idx + 1) {
						// i占쏙옙占쏙옙 left shift!and resize!
						for (int k = i + 1; k < ilist.size(); ++k) {
							ilist[k - 1] = std::move(ilist[k]);
						}
						ilist.resize(ilist.size() - 1);
						break;
					}
				}

				useSortedItemList2 = false;
			}
			void RemoveUserTypeList(const int idx, const bool chk = true)
			{
				if (chk && userTypeList[idx]) {
					delete userTypeList[idx];
				}
				////std::cout << GetUserTypeListSize() << ENTER;
				// left shift start idx, to end, at itemList. and resize!
				for (int i = idx + 1; i < GetUserTypeListSize(); ++i) {
					userTypeList[i - 1] = std::move(userTypeList[i]);
				}
				userTypeList.resize(userTypeList.size() - 1);
				//  ilist left shift and resize - count itemType!
				int count = 0;
				for (int i = 0; i < ilist.size(); ++i) {
					if (ilist[i] == 2) { count++; }
					if (count == idx + 1) {
						// left shift!and resize!
						for (int k = i + 1; k < ilist.size(); ++k) {
							ilist[k - 1] = std::move(ilist[k]);
						}
						ilist.resize(ilist.size() - 1);
						break;
					}
				}

				useSortedUserTypeList = false;
			}
			void RemoveItemList(const WIZ_STRING_TYPE& varName)
			{
				int k = _GetIndex(ilist, 1, 0); 
				std::vector<ItemType<WIZ_STRING_TYPE>> tempDic;
				for (int i = 0; i < itemList.size(); ++i) {
					if (varName != wiz::ToString(itemList[i].GetName())) {
						tempDic.push_back(itemList[i]);
						k = _GetIndex(ilist, 1, k + 1);
					}
					else {
						// remove item, ilist left shift 1.
						for (int j = k + 1; j < ilist.size(); ++j) {
							ilist[j - 1] = ilist[j];
						}
						ilist.resize(ilist.size() - 1);
						k = _GetIndex(ilist, 1, k);
					}
				}
				itemList = std::move(tempDic);

				useSortedItemList2 = false;
			}
			void RemoveItemList() /// ALL
			{
				itemList = std::vector<ItemType<WIZ_STRING_TYPE>>();
				//
				std::vector<int> temp;
				for (int i = 0; i < ilist.size(); ++i) {
					if (ilist[i] == 2)
					{
						temp.push_back(2);
					}
				}
				ilist = std::move(temp);

				useSortedItemList2 = false;
			}
			void RemoveEmptyItem() // fixed..
			{
				int k = _GetIndex(ilist, 1, 0);
				std::vector<ItemType<WIZ_STRING_TYPE>> tempDic;
				for (int i = 0; i < itemList.size(); ++i) {
					if (itemList[i].size() > 0) {
						tempDic.push_back(itemList[i]);
						k = _GetIndex(ilist, 1, k + 1);
					}
					else {
						// remove item, ilist left shift 1.
						for (int j = k + 1; j < ilist.size(); ++j) {
							ilist[j - 1] = ilist[j];
						}
						ilist.resize(ilist.size() - 1);
						k = _GetIndex(ilist, 1, k);
					}
				}
				itemList = std::move(tempDic);

				useSortedItemList2 = false;
			}
			void Remove()
			{
				/// parent->removeUserType(name); - ToDo - X
				ilist = std::vector<int>();
				itemList = std::vector< ItemType<WIZ_STRING_TYPE> >();

				RemoveUserTypeList();


				sortedUserTypeList.clear();

				useSortedItemList2 = false;
				useSortedUserTypeList = false;
				//parent = nullptr;
			}
			void RemoveUserTypeList() { /// chk memory leak test!!
				for (int i = 0; i < userTypeList.size(); i++) {
					if (nullptr != userTypeList[i]) {
						Delete(userTypeList[i]); //
						userTypeList[i] = nullptr;
					}
				}
				// DO Empty..
				userTypeList.clear();

				std::vector<int> temp;
				for (int i = 0; i < ilist.size(); ++i) {
					if (ilist[i] == 1)
					{
						temp.push_back(1);
					}
				}
				ilist = std::move(temp);

				useSortedUserTypeList = false;
			}
			void RemoveUserTypeList(const WIZ_STRING_TYPE& varName, const bool chk = true)
			{
				int k = _GetIndex(ilist, 2, 0);
				std::vector<UserType*> tempDic;
				for (int i = 0; i < userTypeList.size(); ++i) {
					if (varName != wiz::ToString(userTypeList[i]->GetName())) {
						tempDic.push_back(userTypeList[i]);
						k = _GetIndex(ilist, 2, k + 1);
					}
					else {
						if (chk && userTypeList[i]) {
							delete userTypeList[i];
						}
						// remove usertypeitem, ilist left shift 1.
						for (int j = k + 1; j < ilist.size(); ++j) {
							ilist[j - 1] = ilist[j];
						}
						ilist.resize(ilist.size() - 1);
						k = _GetIndex(ilist, 2, k);
					}
				}
				userTypeList = std::move(tempDic);

				useSortedUserTypeList = false;
			}
		//			
			void RemoveList(const int idx) // ilist_idx!
			{
				// chk whether item or usertype.
				// find item_idx or usertype_idx.
				// remove item or remove usertype.
				if (ilist[idx] == 1) {
					int item_idx = -1;

					for (int i = 0; i < ilist.size() && i <= idx; ++i) {
						if (ilist[i] == 1) { item_idx++; }
					}

					RemoveItemList(item_idx);
				}
				else {
					int usertype_idx = -1;

					for (int i = 0; i < ilist.size() && i <= idx; ++i) {
						if (ilist[i] == 2) { usertype_idx++; }
					}

					RemoveUserTypeList(usertype_idx);
				}
			}
		public:
			bool empty()const { return ilist.empty(); }
			
			// chk
			void InsertItemByIlist(const int ilist_idx, const WIZ_STRING_TYPE& name, const WIZ_STRING_TYPE& item ) {
				ilist.push_back(1);
				for (int i = ilist.size()-1; i > ilist_idx; --i) {
					ilist[i] = ilist[i - 1];
				}
				ilist[ilist_idx] = 1;


				int itemIndex = _GetItemIndexFromIlistIndex(ilist, ilist_idx);

				itemList.emplace_back("", WIZ_STRING_TYPE(""));

				if (itemIndex != -1) {
					for (int i = itemList.size() - 1; i > itemIndex; --i) {
						itemList[i] = std::move(itemList[i - 1]);
					}
					itemList[itemIndex] = ItemType<WIZ_STRING_TYPE>(name, item);
				}
				else {
					itemList[0] = ItemType<WIZ_STRING_TYPE>(name, item); // chk!!
				}

				useSortedItemList2 = false;
			}
			void InsertItemByIlist(const int ilist_idx, WIZ_STRING_TYPE&& name, std::string&& item) {
				ilist.push_back(1);

				
				for (int i = ilist.size() - 1; i > ilist_idx; --i) {
					ilist[i] = ilist[i - 1];
				}
				ilist[ilist_idx] = 1;

				int itemIndex = _GetItemIndexFromIlistIndex(ilist, ilist_idx);

				itemList.emplace_back("", WIZ_STRING_TYPE(""));
				if (itemIndex != -1) {
					for (int i = itemList.size() - 1; i > itemIndex; --i) {
						itemList[i] = std::move(itemList[i - 1]);
					}
					itemList[itemIndex] = ItemType<WIZ_STRING_TYPE>(std::move(name), std::move(item));
				}
				else {
					itemList[0] = ItemType<WIZ_STRING_TYPE>(std::move(name), std::move(item));
				}


				useSortedItemList2 = false;
			}
			// chk
			void InsertUserTypeByIlist(const int ilist_idx, UserType&& item) {
				ilist.push_back(2);
				UserType* temp = new UserType(std::move(item));

				temp->parent = this;

				for (int i = ilist.size() - 1; i > ilist_idx; --i) {
					ilist[i] = ilist[i - 1];
				}
				ilist[ilist_idx] = 2;

				int userTypeIndex = _GetUserTypeIndexFromIlistIndex(ilist, ilist_idx);
				userTypeList.push_back(nullptr);
				if (userTypeIndex != -1) {
					for (int i = userTypeList.size() - 1; i > userTypeIndex; --i) {
						userTypeList[i] = std::move(userTypeList[i - 1]);
					}
					userTypeList[userTypeIndex] = temp;
				}
				else {
					userTypeList[0] = temp;
				}
				

				useSortedUserTypeList = false;
			}
			void InsertUserTypeByIlist(const int ilist_idx, const UserType& item) {
				ilist.push_back(2);
				UserType* temp = new UserType(item);

				temp->parent = this;

				
				for (int i = ilist.size() - 1; i > ilist_idx; --i) {
					ilist[i] = ilist[i - 1];
				}
				ilist[ilist_idx] = 2;

				int userTypeIndex = _GetUserTypeIndexFromIlistIndex(ilist, ilist_idx);
				userTypeList.push_back(nullptr);
				if (userTypeIndex != -1) {
					for (int i = userTypeList.size() - 1; i > userTypeIndex; --i) {
						userTypeList[i] = std::move(userTypeList[i - 1]);
					}
					userTypeList[userTypeIndex] = temp;
				}
				else {
					userTypeList[0] = temp;
				}

				useSortedUserTypeList = false;
			}

			void ReserveIList(int offset)
			{
				if (offset > 0 && offset > ilist.size()) {
					ilist.reserve(offset);
				}
			}
			void ReserveItemList(int offset)
			{
				if (offset > 0 && offset > itemList.size()) {
					itemList.reserve(offset);
				}
			}
			void ResizeItemList(int offset)
			{
				if (offset > 0) {
					itemList.resize(offset);
				}
			}
			void ReserveUserTypeList(int offset)
			{
				if (offset > 0 && offset > userTypeList.size()) {
					userTypeList.reserve(offset);
				}
			}

			void AddItem(WIZ_STRING_TYPE&& name, WIZ_STRING_TYPE&& item) {
				{
					itemList.emplace_back(std::move(name), std::move(item));
				}
				ilist.push_back(1);

				useSortedItemList2 = false;

			}
			void AddItem(const WIZ_STRING_TYPE& name, const WIZ_STRING_TYPE& item) {
				itemList.emplace_back(name, item);
				ilist.push_back(1);

					useSortedItemList2 = false;
			}
			void AddItem(std::vector<WIZ_STRING_TYPE>&& name, std::vector<WIZ_STRING_TYPE>&& item, const int n) {
				// name.size() == item.size()
				int start_idx = itemList.size();
				itemList.reserve(itemList.size() + n);
				ilist.reserve(ilist.size() + n);
				int end_idx = itemList.size() + n;

				{
					for (int i = start_idx; i < end_idx; ++i) {
						itemList.emplace_back(std::move(name[i-start_idx]), std::move(item[i-start_idx]));
						ilist.push_back(1);
					}
				}

				useSortedItemList2 = false;
			}
			void AddItem(std::vector<WIZ_STRING_TYPE>& name, std::vector<WIZ_STRING_TYPE>& item, const int n) {
				// name.size() == item.size()
				int start_idx = itemList.size();
				itemList.reserve(itemList.size() + n);
				ilist.reserve(ilist.size() + n);
				int end_idx = itemList.size() + n;

				{
					for (int i = start_idx; i < end_idx; ++i) {
						itemList.emplace_back(std::move(name[i-start_idx]), std::move(item[i-start_idx]));
						ilist.push_back(1);
					}
				}

				useSortedItemList2 = false;
				
			}

			void AddUserTypeItem(UserType&& item) {
				UserType* temp = new UserType(std::move(item));
				temp->parent = this;
				//temp->SetName("");

				ilist.push_back(2);

				userTypeList.push_back(temp);

				if (useSortedUserTypeList) {
					sortedUserTypeList.push_back(userTypeList.back());
					miniInsertSort<wiz::load_data::UserType*, UserTypeCompare>(sortedUserTypeList);
				}
				else {
					useSortedUserTypeList = false;
				}
			}
			void AddUserTypeItem(const UserType& item) {
				UserType* temp = new UserType(item);
				temp->parent = this;

				ilist.push_back(2);

				userTypeList.push_back(temp);

				if (useSortedUserTypeList) {
					sortedUserTypeList.push_back(userTypeList.back());
					miniInsertSort<wiz::load_data::UserType*, UserTypeCompare>(sortedUserTypeList);
				}
				else {
					useSortedUserTypeList = false;
				}
			}
			void AddItemAtFront(WIZ_STRING_TYPE&& name, std::string&& item) {
				itemList.emplace(itemList.begin(), name, item);

				ilist.insert(ilist.begin(), 1);

				useSortedItemList2 = false;
				
			}
			void AddItemAtFront(const WIZ_STRING_TYPE& name, const WIZ_STRING_TYPE& item) {
				itemList.emplace(itemList.begin(), name, item);

				ilist.insert(ilist.begin(), 1);

				useSortedItemList2 = false;
				
			}
			void AddUserTypeItemAtFront(const UserType& item) {
				UserType* temp = new UserType(item);
				temp->parent = this;

				ilist.insert(ilist.begin(), 2);

				userTypeList.insert(userTypeList.begin(), temp);

				if (useSortedUserTypeList) {
					sortedUserTypeList.push_back(userTypeList[0]);
					miniInsertSort<wiz::load_data::UserType*, UserTypeCompare>(sortedUserTypeList);
				}
				else {
					useSortedUserTypeList = false;
				}
			}
			void AddUserTypeItemAtFront(UserType&& item) {
				UserType* temp = new UserType(item);
				temp->parent = this;

				ilist.insert(ilist.begin(), 2);

				userTypeList.insert(userTypeList.begin(), temp);

				if (useSortedUserTypeList) {
					sortedUserTypeList.push_back(userTypeList[0]);
					miniInsertSort<wiz::load_data::UserType*, UserTypeCompare>(sortedUserTypeList);
				}
				else {
					useSortedUserTypeList = false;
				}
			}


			// $it?
			std::vector<ItemType<WIZ_STRING_TYPE>> GetItem(const std::string& name, bool chk = false) const {
				std::vector<ItemType<WIZ_STRING_TYPE>> temp;
				if (String::startsWith(name, "$.") && name.size() >= 5) {
					// later, change to binary search?
					std::string str = name.substr(3, name.size() - 4);
					std::regex rgx(str);

					for (int i = 0; i < itemList.size(); ++i) {
						if (regex_match(wiz::ToString(itemList[i].GetName()), rgx)) {
							temp.push_back(itemList[i]);
						}
					}
				}
				else {
					for (int i = 0; i < itemList.size(); ++i) {
						if (itemList[i].GetName() == name) {
							temp.push_back(itemList[i]);
						}
					}
				}

				if (chk && USE_EMPTY_VECTOR_IN_LOAD_DATA_TYPES && temp.empty()) {
					temp.push_back(ItemType<DataType>("", ""));
				}
				return temp;
			}

			// rename..
			std::vector<int> GetItemPtr(const std::string& name, bool chk = false) {
				std::vector<int> temp;
				if (String::startsWith(name, "$.") && name.size() >= 5) {
					// later, change to binary search?
					std::string str = name.substr(3, name.size() - 4);
					std::regex rgx(str);

					for (int i = 0; i < itemList.size(); ++i) {
						if (std::regex_match(wiz::ToString(itemList[i].GetName()), rgx)) {
							temp.push_back(i);
						}
					}
				}
				else {
					for (int i = 0; i < itemList.size(); ++i) {
						if (itemList[i].GetName() == name) {
							temp.push_back(i);
						}
					}
				}

				if (chk && USE_EMPTY_VECTOR_IN_LOAD_DATA_TYPES && temp.empty()) {
					//temp.push_back(-1); // ItemType<DataType>("", ""));
				}
				return temp;
			}

			// regex to SetItem?
			bool SetItem(const WIZ_STRING_TYPE& name, const WIZ_STRING_TYPE& value) {
				int index = -1;

				for (int i = 0; i < itemList.size(); ++i) {
					if (itemList[i].GetName() == name)
					{
						itemList[i].Set(0, value);
						index = i;
					}
				}

				return -1 != index;
			}
			/// add set Data
			bool SetItem(const int var_idx, const WIZ_STRING_TYPE& value) {
				itemList[var_idx].Set(0, value);
				return true;
			}
			
			//// O(N) -> O(logN)?
			std::vector<UserType*> GetUserTypeItem(const std::string& name) const { /// chk...
				std::vector<UserType*> temp;

				if (String::startsWith(name, "$.") && name.size() >= 5) {
					// later, change to binary search?
					std::string str = name.substr(3, name.size() - 4);
					std::regex rgx(str.data());

					for (int i = 0; i < userTypeList.size(); ++i) {
						if (std::regex_match(wiz::ToString(userTypeList[i]->GetName()), rgx)) {
							temp.push_back(userTypeList[i]);
						}
					}

					return temp;
				}

				if (false == useSortedUserTypeList) {
					// make sortedUserTypeList.
					sortedUserTypeList = userTypeList;

					std::sort(sortedUserTypeList.begin(), sortedUserTypeList.end(), UserTypeCompare());

					useSortedUserTypeList = true;
				}
				// binary search
				{
					int idx = binary_find_ut(sortedUserTypeList, name);
					if (idx >= 0) {
						int start = idx;
						int last = idx;

						for (int i = idx - 1; i >= 0; --i) {
							if (name == sortedUserTypeList[i]->GetName()) {
								start--;
							}
							else {
								break;
							}
						}
						for (int i = idx + 1; i < sortedUserTypeList.size(); ++i) {
							if (name == sortedUserTypeList[i]->GetName()) {
								last++;
							}
							else {
								break;
							}
						}

						for (int i = start; i <= last; ++i) {
							temp.push_back(sortedUserTypeList[i]);
						}
					}
					else {
						////std::cout << "no found" << ENTER;
					}
				}

				/*
				for (int i = 0; i < userTypeList.size(); ++i) {
					if (userTypeList[i]->GetName() == name) {
						temp.push_back(userTypeList[i]);
					}
				}
				*/

				return temp;
			}

			// rename...
			std::vector<int> GetUserTypeItemPtr(const std::string& name) { /// chk...
				std::vector<int> temp;

				if (false == useSortedUserTypeList) {
					// make sortedUserTypeList.
					sortedUserTypeList = userTypeList;

					std::sort(sortedUserTypeList.begin(), sortedUserTypeList.end(), UserTypeCompare());

					useSortedUserTypeList = true;
				}
				// binary search
				{
					for (int i = 0; i < userTypeList.size(); ++i) {
						userTypeList[i]->before_pos = i;
					}
					int idx = binary_find_ut(sortedUserTypeList, name);
					if (idx >= 0) {
						int start = idx;
						int last = idx;

						for (int i = idx - 1; i >= 0; --i) {
							if (name == sortedUserTypeList[i]->GetName()) {
								start--;
							}
							else {
								break;
							}
						}
						for (int i = idx + 1; i < sortedUserTypeList.size(); ++i) {
							if (name == sortedUserTypeList[i]->GetName()) {
								last++;
							}
							else {
								break;
							}
						}

						for (int i = start; i <= last; ++i) {
							temp.push_back(sortedUserTypeList[i]->before_pos);
						}
					}
					else {
						////std::cout << "no found" << ENTER;
					}
				}

				/*
				for (int i = 0; i < userTypeList.size(); ++i) {
					if (userTypeList[i]->GetName() == name) {
						temp.push_back(userTypeList[i]);
					}
				}
				*/

				return temp;
			}
			// deep copy.
			std::vector<UserType*> GetCopyUserTypeItem(const std::string& name) const { /// chk...
				std::vector<UserType*> temp;

				if (false == useSortedUserTypeList) {
					// make sortedUserTypeList.
					sortedUserTypeList = userTypeList;

					std::sort(sortedUserTypeList.begin(), sortedUserTypeList.end(), UserTypeCompare());

					useSortedUserTypeList = true;
				}
				// binary search
				{
					int idx = binary_find_ut(sortedUserTypeList, name);
					if (idx >= 0) {
						int start = idx;
						int last = idx;

						for (int i = idx - 1; i >= 0; --i) {
							if (name == sortedUserTypeList[i]->GetName()) {
								start--;
							}
							else {
								break;
							}
						}
						for (int i = idx + 1; i < sortedUserTypeList.size(); ++i) {
							if (name == sortedUserTypeList[i]->GetName()) {
								last++;
							}
							else {
								break;
							}
						}

						for (int i = start; i <= last; ++i) {
							temp.push_back(new UserType(*sortedUserTypeList[i]));
						}
					}
					else {
						////std::cout << "no found" << ENTER;
					}
				}

				/*
				for (int i = 0; i < userTypeList.size(); ++i) {
					if (userTypeList[i]->GetName() == name) {
						temp.push_back(new UserType(*userTypeList[i]));
					}
				}
				*/
				return temp;
			}
		public:
			bool GetUserTypeItemRef(const int idx, UserType*& ref)
			{
				ref = userTypeList[idx];
				return true;
			}
			bool GetLastUserTypeItemRef(UserType*& ref) {
				if (false == userTypeList.empty()) {
					ref = userTypeList.back();
					return true;
				}
				return false;
			}
		private:
			/// save1 - like EU4 savefiles.
			void Save1(std::ostream& stream, const UserType* ut, const int depth = 0) const {
				int itemListCount = 0;
				int userTypeListCount = 0;

				const bool existUserType = ut->GetUserTypeListSize() > 0;


				for (int i = 0; i < ut->ilist.size(); ++i) {
					////std::cout << "ItemList" << endl;
					if (ut->ilist[i] == 1) {
						for (int j = 0; j < ut->itemList[itemListCount].size(); j++) {
							std::string temp;
							if (existUserType) {
								for (int k = 0; k < depth; ++k) {
									temp += "\t";
								}
							}
							if (wiz::ToString(ut->itemList[itemListCount].GetName()) != "") {	
								temp += wiz::ToString(ut->itemList[itemListCount].GetName());
								temp += "=";
							}
							temp += wiz::ToString(ut->itemList[itemListCount].Get(j));
							if (j != ut->itemList[itemListCount].size() - 1) {
								temp += " "; 
							}
							stream << temp;
						}
						if (i != ut->ilist.size() - 1) {
							if (existUserType) {
								stream << "\n";
							}
							else {
								stream << " "; // \n
							}
						}
						itemListCount++;
					}
					else if (ut->ilist[i] == 2) {
						// //std::cout << "UserTypeList" << endl;
						for (int k = 0; k < depth; ++k) {
							stream << "\t";
						}

						if (wiz::ToString(ut->userTypeList[userTypeListCount]->GetName()) != "") {
							stream << wiz::ToString(ut->userTypeList[userTypeListCount]->GetName()) << "=";
						}

						stream << " {\n";
						
						Save1(stream, ut->userTypeList[userTypeListCount], depth + 1);
						stream << "\n";
						
						for (int k = 0; k < depth; ++k) {
							stream << "\t";
						}
						stream << " } ";
						if (i != ut->ilist.size() - 1) {
							stream << "\n";
						}

						userTypeListCount++;
					}
				}
			}
			/// save2 - for more speed loading data!
			void Save2(std::ostream& stream, const UserType* ut, const int depth = 0) const {
				int itemListCount = 0;
				int userTypeListCount = 0;

				
				for (int i = 0; i < ut->ilist.size(); ++i) {
					////std::cout << "ItemList" << endl;
					if (ut->ilist[i] == 1) {
						for (int j = 0; j < ut->itemList[itemListCount].size(); j++) {
							//for (int k = 0; k < depth; ++k) {
							//	stream << "\t";
							//}
							if (wiz::ToString(ut->itemList[itemListCount].GetName()) != "")
								stream << wiz::ToString(ut->itemList[itemListCount].GetName()) << " = ";
							stream << wiz::ToString(ut->itemList[itemListCount].Get(j));
							if (j != ut->itemList[itemListCount].size() - 1)
								stream << " ";
						}
						if (i != ut->ilist.size() - 1) {
							stream << " ";//"\n";
						}
						itemListCount++;
					}
					else if (ut->ilist[i] == 2) {
						// //std::cout << "UserTypeList" << endl;
						if (wiz::ToString(ut->userTypeList[userTypeListCount]->GetName()) != "")
						{
							stream << wiz::ToString(ut->userTypeList[userTypeListCount]->GetName()) << " = ";
						}
						stream << "{\n";
						
						Save2(stream, ut->userTypeList[userTypeListCount], depth + 1);
						stream << "\n";
						
						for (int k = 0; k < depth; ++k) {
							stream << "\t";
						}
						stream << "}";
						if (i != ut->ilist.size() - 1) {
							stream << "\n";
						}
						userTypeListCount++;
					}
				}
			}
		
			bool IsArrayWithJson(const UserType* ut) const
			{
				if (ut->GetIListSize() == 0) {
					return false;
				}

				bool isArray = true;

				for (int i = 0; i < ut->GetItemListSize(); ++i) {
					if (wiz::ToString(ut->GetItemList(i).GetName()).empty()) {
						isArray = true;
					}
					else {
						return false;
					}
				}

				for (int i = 0; i < ut->GetUserTypeListSize(); ++i) {
					if (wiz::ToString(ut->GetUserTypeList(i)->GetName()).empty()) {
						isArray = true;
					}
					else {
						return false;
					}
				}

				return isArray; // && 0 == ut->GetUserTypeListSize();
			}
			
			// todo - fix bug?
			void SaveWithJson(std::ostream& stream, const UserType* ut, const int depth = 0) const {
				int itemListCount = 0;
				int userTypeListCount = 0;

				/*
				for (int i = 0; i < ut->commentList.size(); ++i) {
					for (int k = 0; k < depth; ++k) {
						stream << "\t";
					}

					stream << "// ";
					stream << (ut->commentList[i]);

					if (i < ut->commentList.size() - 1 || false == ut->ilist.empty()) {
						stream << "\n";
					}

				}
				*/

				for (int i = 0; i < ut->ilist.size(); ++i) {
					////std::cout << "ItemList" << endl;
					if (ut->ilist[i] == 1) {
						//if (userTypeListCount > 0 && 0 == ut->userTypeList[userTypeListCount - 1]->GetIListSize() && ut->userTypeList[userTypeListCount - 1]->GetName().empty())
						//{
							//
						//}
						//else if (i != 0) {
						//	stream << " ,\n";
						//}

						for (int j = 0; j < ut->itemList[itemListCount].size(); j++) {
							for (int k = 0; k < depth; ++k) {
								stream << "\t";
							}
							if (wiz::ToString(ut->itemList[itemListCount].GetName()) != "") {
								////if (ut->itemList[itemListCount].GetName()[0] == '\"') {
									stream << wiz::ToString(ut->itemList[itemListCount].GetName()) << " : ";
								////}
								////else {
								////	stream << "\"" + ut->itemList[itemListCount].GetName() + "\"" << " : ";
								////}
							}
							else {
								//stream << "\"_\" : ";
							}
							////if (ut->itemList[itemListCount].Get(j)[0] == '\"') {
							////	if (ut->itemList[itemListCount].Get(j) == "\"\"") {
							////		stream << "\" \"";
									////	}
									////	else {
									stream << wiz::ToString(ut->itemList[itemListCount].Get(j));
									////	}
							}
						////	else {
						////		stream << "\"" + ut->itemList[itemListCount].Get(j) + "\"";
						////	}
						////}
						if (i != ut->ilist.size() - 1) {
							stream << " ,\n";
						}
						itemListCount++;
					}
					else if (ut->ilist[i] == 2) {
						//if (0 == ut->userTypeList[userTypeListCount]->GetIListSize() && ut->userTypeList[userTypeListCount]->GetName().empty())
						//{
						//	userTypeListCount++;
						//
						//	continue;
						//}
						//else 
						//if (i != 0) {
						//	stream << " ,\n";
						//}
						
						for (int k = 0; k < depth; ++k) {
							stream << "\t";
						}
						// //std::cout << "UserTypeList" << endl;
						
						if (wiz::ToString(ut->userTypeList[userTypeListCount]->GetName()) != "")
						{
						////	if (ut->userTypeList[userTypeListCount]->GetName()[0] == '\"') {
						stream << wiz::ToString(ut->userTypeList[userTypeListCount]->GetName()) << " : ";
								////	}
								////	else {
								////		stream << "\"" + ut->userTypeList[userTypeListCount]->GetName() + "\"" << " : ";
								////	}
								////	}
								////else {
						//	stream << "\"_\"" << " : ";
						}
						
						if (IsArrayWithJson(ut->userTypeList[userTypeListCount])) {
							stream << "[\n";
						}
						else {
							stream << "{\n";
						}

						SaveWithJson(stream, ut->userTypeList[userTypeListCount], depth + 1);
						stream << "\n";

						for (int k = 0; k < depth; ++k) {
							stream << "\t";
						}
						
						if (IsArrayWithJson(ut->userTypeList[userTypeListCount])) {
							stream << "]";
						}
						else {
							stream << "}";
						}

						if (i != ut->ilist.size() - 1) {
							stream << " ,\n";
						}
						userTypeListCount++;
					}
				}
			}
			void SaveWithJson2(std::ostream& stream, const UserType* ut, const int depth = 0) const {
				int itemListCount = 0;
				int userTypeListCount = 0;

				/*
				for (int i = 0; i < ut->commentList.size(); ++i) {
				for (int k = 0; k < depth; ++k) {
				stream << "\t";
				}

				stream << "// ";
				stream << (ut->commentList[i]);

				if (i < ut->commentList.size() - 1 || false == ut->ilist.empty()) {
				stream << "\n";
				}

				}
				*/

				for (int i = 0; i < ut->ilist.size(); ++i) {
					////std::cout << "ItemList" << endl;
					if (ut->ilist[i] == 1) {
						//if (userTypeListCount > 0 && 0 == ut->userTypeList[userTypeListCount - 1]->GetIListSize() && ut->userTypeList[userTypeListCount - 1]->GetName().empty())
						//{
						//
						//}
						//else if (i != 0) {
						//	stream << " ,\n";
						//}

						for (int j = 0; j < ut->itemList[itemListCount].size(); j++) {
							for (int k = 0; k < depth; ++k) {
								stream << "\t";
							}
							if (wiz::ToString(ut->itemList[itemListCount].GetName()) != "") {
								if (wiz::ToString(ut->itemList[itemListCount].GetName())[0] == '\"') {
									stream << wiz::ToString(ut->itemList[itemListCount].GetName()) << " : ";
								}
								else {
									stream << "\"" + wiz::ToString(ut->itemList[itemListCount].GetName()) + "\"" << " : ";
								}
							}
							else {
								//stream << "\"_\" : ";
							}
							if (wiz::ToString(ut->itemList[itemListCount].Get(j))[0] == '\"') {
								////	if (ut->itemList[itemListCount].Get(j) == "\"\"") {
								////		stream << "\" \"";
								////	}
								////	else {
								stream << wiz::ToString(ut->itemList[itemListCount].Get(j));
								////	}
							}
							else {
								stream << "\"" + wiz::ToString(ut->itemList[itemListCount].Get(j)) + "\"";
							}
						}
							////}
							if (i != ut->ilist.size() - 1) {
								stream << " ,\n";
							}
							itemListCount++;
						//}
					}
					else if (ut->ilist[i] == 2) {
						//if (0 == ut->userTypeList[userTypeListCount]->GetIListSize() && ut->userTypeList[userTypeListCount]->GetName().empty())
						//{
						//	userTypeListCount++;
						//
						//	continue;
						//}
						//else 
						//if (i != 0) {
						//	stream << " ,\n";
						//}

						for (int k = 0; k < depth; ++k) {
							stream << "\t";
						}
						// //std::cout << "UserTypeList" << endl;
						if (ut->userTypeList[userTypeListCount]->GetIListSize() > 0) {
						
							if (wiz::ToString(ut->userTypeList[userTypeListCount]->GetName()) != "")
							{
									if (wiz::ToString(ut->userTypeList[userTypeListCount]->GetName())[0] == '\"') {
								stream << wiz::ToString(ut->userTypeList[userTypeListCount]->GetName()) << " : ";
									}
									else {
										stream << "\"" + wiz::ToString(ut->userTypeList[userTypeListCount]->GetName()) + "\"" << " : ";
									}
								////	}
								////else {
								//	stream << "\"_\"" << " : ";
							}


							if (IsArrayWithJson(ut->userTypeList[userTypeListCount])) {
								stream << "[\n";
							}
							else {
								stream << "{\n";
							}

							SaveWithJson2(stream, ut->userTypeList[userTypeListCount], depth + 1);
							stream << "\n";

							for (int k = 0; k < depth; ++k) {
								stream << "\t";
							}

							if (IsArrayWithJson(ut->userTypeList[userTypeListCount])) {
								stream << "]";
							}
							else {
								stream << "}";
							}
							if (i != ut->ilist.size() - 1) {
								stream << " ,\n";
							}
						}
						
						userTypeListCount++;
					}
				}
			}
		public:
			void Save1(std::ostream& stream, int depth = 0) const {
				Save1(stream, this, depth);
			}
			void Save2(std::ostream& stream, int depth = 0) const {
				Save2(stream, this, depth);
			}
			void SaveWithJson(std::ostream& stream, int depth = 0) const
			{
				//stream << "{\n";
				SaveWithJson(stream, this, depth + 1);
				//stream << "\n}";
			}
			void SaveWithJson2(std::ostream& stream, int depth = 0) const
			{
				stream << "{\n";
				SaveWithJson2(stream, this, depth + 1);
				stream << "\n}";
			}


			std::string ItemListToString()const
			{
				std::string temp;
				int itemListCount = 0;

				for (int i = 0; i < itemList.size(); ++i) {
					for (int j = 0; j < itemList[itemListCount].size(); j++) {
						if (wiz::ToString(itemList[itemListCount].GetName()) != "")
							temp = temp + wiz::ToString(itemList[itemListCount].GetName()) + " = ";
						temp = temp + wiz::ToString(itemList[itemListCount].Get(j)) + " ";
						if (j != itemList[itemListCount].size() - 1) {
							temp = temp + "/";
						}
					}
					if (i != itemList.size() - 1)
					{
						temp = temp + "/";
					}
					itemListCount++;
				}
				return temp;
			}
			std::string ItemListNamesToString()const
			{
				std::string temp;
				int itemListCount = 0;

				for (int i = 0; i < itemList.size(); ++i) {
					for (int j = 0; j < itemList[itemListCount].size(); j++) {
						if (wiz::ToString(itemList[itemListCount].GetName()) != "")
							temp = temp + wiz::ToString(itemList[itemListCount].GetName());
						else
							temp = temp + " ";

						if (j != itemList[itemListCount].size() - 1) {
							temp = temp + "/";
						}
					}
					if (i != itemList.size() - 1)
					{
						temp = temp + "/";
					}
					itemListCount++;
				}
				return temp;
			}
			std::vector<std::string> userTypeListNamesToStringArray()const
			{
				std::vector<std::string> temp;
				int userTypeListCount = 0;

				for (int i = 0; i < userTypeList.size(); ++i) {
					if (wiz::ToString(userTypeList[userTypeListCount]->GetName()) != "") {
						temp.push_back(wiz::ToString(userTypeList[userTypeListCount]->GetName()));
					}
					else {
						temp.push_back(" "); // chk!! cf) wiz::load_data::Utility::Find function...
					}
					userTypeListCount++;
				}
				return temp;
			}
			std::string UserTypeListNamesToString()const
			{
				std::string temp;
				int userTypeListCount = 0;

				for (int i = 0; i < userTypeList.size(); ++i) {
					if (wiz::ToString(userTypeList[userTypeListCount]->GetName()) != "") {
						temp = temp + wiz::ToString(userTypeList[userTypeListCount]->GetName());
					}
					else {
						temp = temp + " "; // chk!! cf) wiz::load_data::Utility::Find function...
					}

					if (i != itemList.size() - 1)
					{
						temp = temp + "/";
					}
					userTypeListCount++;
				}
				return temp;
			}
			std::string ToString()const
			{
				std::string temp;
				int itemListCount = 0;
				int userTypeListCount = 0;

				for (int i = 0; i < ilist.size(); ++i) {
					////std::cout << "ItemList" << endl;
					if (ilist[i] == 1) {
						for (int j = 0; j < itemList[itemListCount].size(); j++) {
							if (wiz::ToString(itemList[itemListCount].GetName()) != "") {
								temp.append(wiz::ToString(itemList[itemListCount].GetName()));
								temp.append(" = ");
							}
							temp.append(wiz::ToString(itemList[itemListCount].Get(j)));
							if (j != itemList[itemListCount].size() - 1)
							{
								temp.append(" ");
							}
						}
						if (i != ilist.size() - 1) {
							temp.append(" ");
						}
						itemListCount++;
					}
					else if (ilist[i] == 2) {
						// //std::cout << "UserTypeList" << endl;
						if (wiz::ToString(userTypeList[userTypeListCount]->GetName()) != "") {
							temp.append(wiz::ToString(userTypeList[userTypeListCount]->GetName()));
							temp.append(" = ");
						}
						temp.append( " { ");
						temp.append(userTypeList[userTypeListCount]->ToString());
						temp.append(" ");
						temp.append(" }");
						if (i != ilist.size() - 1) {
							temp.append(" ");
						}

						userTypeListCount++;
					}
				}
				return temp;
			}
		public:
			static std::pair<bool, std::vector< UserType*> > Find(UserType* global, const std::string& _position) /// option, option_offset
			{
				std::string position = _position;

				if (!position.empty() && position[0] == '@') { position.erase(position.begin()); }
				
				std::vector<std::string> x;


				//std::cout << "string view is " << pos_sv << " ";
				std::vector<std::string> tokens = tokenize(position, '/');
			
				for (size_t i = 0; i < tokens.size(); ++i) {
					std::string temp = tokens[i];
					
					//std::cout << tokens[i] << " ";

					if (temp == ".") {
						continue;
					}

					if (x.empty()) {
						x.push_back(temp);
					}
					else if (x.back() != ".." && temp == "..") {
						x.pop_back();
					}
					else {
						x.push_back(temp);
					}
				}

				return _Find(std::move(x), global);
			}
		private:	
			// find userType! not itemList!,// this has bug
			static std::pair<bool, std::vector< UserType*> > _Find(std::vector<std::string>&& tokens, UserType* global) /// option, option_offset
			{
				std::vector< UserType* > temp;
				int start = 0;

				if (tokens.empty()) { temp.push_back(global); return{ true, temp }; }
				if (tokens.size() == 1 && tokens[0] == ".") { temp.push_back(global); return{ true, temp }; }
				//if (position.size() == 1 && position[0] == "/./") { temp.push_back(global); return{ true, temp }; } // chk..
				//if (position.size() == 1 && position[0] == "/.") { temp.push_back(global); return{ true, temp }; }
				//if (position.size() == 1 && position[0] == "/") { temp.push_back(global); return{ true, temp }; }
				
				if (tokens.size() > 1 && tokens[0] == ".")
				{
					start = 1;
					//position = String::substring(position, 3);
				}

				std::list<std::pair< UserType*, int >> utDeck;
				std::pair<UserType*, int> utTemp;
				utTemp.first = global;
				utTemp.second = 0;
				std::vector<std::string> strVec;

				//std::cout << "position is " << position << "\t";
				for (int i = start; i < tokens.size(); ++i) {
					std::string strTemp = tokens[i];

					//std::cout << strTemp << " ";

					if (strTemp == "root" && i == 0) {
					}
					else {
						strVec.push_back(strTemp);
					}

					if ((strVec.size() >= 1) && (" " == strVec[strVec.size() - 1])) /// chk!!
					{
						strVec[strVec.size() - 1] = "";
					}
					else if ((strVec.size() >= 1) && ("_" == strVec[strVec.size() - 1]))
					{
						strVec[strVec.size() - 1] = "";
					}
				}

				// it has bug!
				{
					int count = 0;

					for (int i = 0; i < strVec.size(); ++i) {
						if (strVec[i] == "..") {
							count++;
						}
						else {
							break;
						}
					}

					std::reverse(strVec.begin(), strVec.end());

					for (int i = 0; i < count; ++i) {
						if (utTemp.first == nullptr) {
							return{ false, std::vector< UserType* >() };
						}
						utTemp.first = utTemp.first->GetParent();
						strVec.pop_back();
					}
					std::reverse(strVec.begin(), strVec.end());
				}
				//std::cout << "\n";

				utDeck.push_front(utTemp);

				bool exist = false;
				while (false == utDeck.empty()) {
					utTemp = utDeck.front();
					utDeck.pop_front();
					
					if (utTemp.second < strVec.size() && strVec[utTemp.second] == "$")
					{
						for (int j = utTemp.first->GetUserTypeListSize() - 1; j >= 0; --j) {
							UserType* x = utTemp.first->GetUserTypeList(j);
							utDeck.push_front(std::make_pair(x, utTemp.second + 1));
						}
					}
					else if (utTemp.second < strVec.size() && String::startsWith(strVec[utTemp.second], "$.")) /// $."abc"
					{
						std::string rex_str = strVec[utTemp.second].substr(3, strVec[utTemp.second].size() - 4);
						std::regex rgx(rex_str.data());

						for (int j = utTemp.first->GetUserTypeListSize() - 1; j >= 0; --j) {
							if (std::regex_match(wiz::ToString(utTemp.first->GetUserTypeList(j)->GetName()), rgx)) {
								UserType* x = utTemp.first->GetUserTypeList(j);
								utDeck.push_front(std::make_pair(x, utTemp.second + 1));
							}
						}
					}
					else if (utTemp.second < strVec.size() &&
						(utTemp.first->GetUserTypeItem(strVec[utTemp.second]).empty() == false))
					{
						auto  x = utTemp.first->GetUserTypeItem(strVec[utTemp.second]);
						for (int j = x.size() - 1; j >= 0; --j) {
							utDeck.push_front(std::make_pair(x[j], utTemp.second + 1));
						}
					}

					if (utTemp.second == strVec.size()) {
						exist = true;
						temp.push_back(utTemp.first);
					}
				}
				if (false == exist) {
					return{ false, std::vector<UserType*>() }; 
				}
				return{ true, temp };
			}	
		};
	}
}

#endif