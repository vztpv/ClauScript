bool VM::_UpdateFunc(clau_parser::UserType* global, clau_parser::UserType* insert_ut, VM* vm) {
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
				&& !x.ut->GetItemList(it_count).Get()._Starts_with("@"sv)) {
				// chk exist all case of value?
				auto item = x.global->GetItemIdx("");
				// no exist -> return false;
				if (item.empty()) {
					// LOG....
					return false;
				}

				bool pass = false;
				for (long long j = 0; j < item.size(); ++j) {
					if (EqualFunc(x.global, x.global->GetItemList(item[j]), x.ut->GetItemList(it_count), item[j],
						x.dir, vm)) {
						pass = true;
						break;
					}
				}
				if (!pass) {
					return false;
				}
			}
			else if (x.ut->IsItemList(i) && !x.ut->GetItemList(it_count).GetName().empty() && !x.ut->GetItemList(it_count).GetName()._Starts_with("@"sv)) {
				// chk exist all case of value?
				auto item = x.global->GetItemIdx(x.ut->GetItemList(it_count).GetName());
				// no exist -> return false;
				if (item.empty()) {
					// LOG....
					return false;
				}

				bool pass = false;

				for (long long j = 0; j < item.size(); ++j) {
					if (EqualFunc(x.global, x.global->GetItemList(item[j]), x.ut->GetItemList(it_count), item[j],
						dir, vm)) {
						pass = true;
						break;
					}
				}
				if (!pass) {
					return false;
				}

			}
			else if (x.ut->IsUserTypeList(i) && !x.ut->GetUserTypeList(ut_count)->GetName()._Starts_with("@"sv)) {
				// chk all case exist of @.
				// no exist -> return false;
				if (x.ut->GetUserTypeList(ut_count)->GetName()._Starts_with("$"sv)) {
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
						dir + "/$ut" + std::to_string(usertype[j])));
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
