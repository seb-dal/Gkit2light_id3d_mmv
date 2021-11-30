#pragma once

#include <functional>
#include <unordered_map>
#include <string>
#include <utility>


template<typename Type>
class Unique {
public:
	Unique() {}

	void set_Hash_function(std::function<std::string(const Type&)> f) {
		hash = f;
	}

	void add(const Type& e) {
		if (map.find(hash(e)) == map.end()) {
			map.emplace(hash(e), e);
		}
	}

	void add(Type& e) {
		if (map.find(hash(e)) == map.end()) {
			map.emplace(hash(e), e);
		}
	}

	size_t size() { return map.size(); }

	auto begin() {
		return map.begin();
	}

	auto end() {
		return map.end();
	}

protected:
	std::unordered_map<std::string, Type> map;

	std::function<std::string(const Type&)> hash;
};
