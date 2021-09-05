#include <iostream>
#include <string>
#include <map>
#include <memory>
#include <deque>

template <typename KeyType, typename ContainerType> 
class Trie;

template <typename KeyType, typename ContainerType> 
class Node{
private:
	bool is_leaf;
	typedef Node<KeyType, ContainerType> NODE;
	std::map<KeyType, std::shared_ptr<NODE>> nodes;

public:
	Node() : is_leaf(false) {}
	
	void SetChild(const KeyType& key){
		//std::shared_ptr<NODE> node(new NODE());
		//nodes.insert(make_pair(key, node));

		nodes[key] = std::make_shared<NODE>();
 	}

	std::shared_ptr<NODE> GetChild(const KeyType& key) const{
		auto iter = nodes.find(key);
		if(iter == nodes.end()){
			return std::shared_ptr<NODE>(); //return nullptr
		}else{
			return iter->second;
		}
	}

	bool IsLeaf() const{ 
		return is_leaf; 
	}

	void SetLeaf(bool is_leaf_){
		is_leaf = is_leaf_;
	}

	void EraseKey(const KeyType& key){
		nodes.erase(key);
	}

	friend class Trie<KeyType, ContainerType>;

};

template <typename KeyType, typename ContainerType> 
class Trie{
private:
	typedef Node<KeyType, ContainerType> NODE;
	std::shared_ptr<NODE> root;
	void GetPrefixContainers(std::shared_ptr<NODE> current_node,
							std::deque<ContainerType> &result, ContainerType prefix) const;

public:
	Trie() {
		root = std::make_shared<NODE>();
	}

	bool Insert(const ContainerType& container);
	
	bool Find(const ContainerType& container) const;

	bool Remove(const ContainerType& container);

	std::deque<ContainerType> ContainingPrefix(const ContainerType &prefix) const;

};

template <typename KeyType, typename ContainerType> 
bool Trie<KeyType, ContainerType>::Insert(const ContainerType& container){
	auto current_node = root;
	auto iter = container.begin();
	while(iter != container.end()){
		auto child_node = current_node->GetChild(*iter);
		if(child_node.get() == nullptr){ 
			break;
		}
		current_node = child_node;
		iter++;
	}

	while (iter != container.end()){
		current_node->SetChild(*iter);
		current_node = current_node->GetChild(*iter);
		iter++;
	}
	current_node->SetLeaf(true);

	for(auto print_iter = container.begin(); print_iter != container.end(); print_iter++){
		std::cout << *print_iter;
	}
	std::cout << " inserted." << std::endl;
	return true;
};

template <typename KeyType, typename ContainerType> 
bool Trie<KeyType, ContainerType>::Find(const ContainerType& container) const{
	auto current_node = root;
	for(auto iter = container.begin(); iter != container.end(); iter++){
		current_node = current_node->GetChild(*iter);
		if(current_node.get() == nullptr){
			for(auto print_iter = container.begin(); print_iter != container.end(); print_iter++){
				std::cout << *print_iter;
			}
			std::cout << " NOT FOUND" << std::endl;
			return false;
		}
	}

	if(current_node->IsLeaf()){
		for(auto print_iter = container.begin(); print_iter != container.end(); print_iter++){
			std::cout << *print_iter;
		}
		std::cout << " found" << std::endl;
		return true;
	}else{
		for(auto print_iter = container.begin(); print_iter != container.end(); print_iter++){
			std::cout << *print_iter;
		}
		std::cout << " NOT FOUND" << std::endl;
		return false;
	}
};

template <typename KeyType, typename ContainerType> 
bool Trie<KeyType, ContainerType>::Remove(const ContainerType& container){
	std::deque<std::shared_ptr<NODE>> node_deque;
	auto current_node = root;

	for(auto iter = container.begin(); iter != container.end(); iter++){
		current_node = current_node->GetChild(*iter);

		if(current_node.get() == nullptr){ //parola non presente
			node_deque.clear();
			return false;
		}

		node_deque.push_back(current_node);
	}

	if(!current_node->IsLeaf()){
		return false;
	}

	current_node->SetLeaf(false);

	if(node_deque.size() != container.size()){
		return false;
	}

	for(auto reverse_iter = node_deque.rbegin(); reverse_iter != node_deque.rend(); reverse_iter++){
		auto child_deque = (*reverse_iter)->GetChild(container[node_deque.size()-1]);
		if(child_deque.get() == nullptr){
			(*reverse_iter)->EraseKey(container[node_deque.size()-1]);
		}
		node_deque.pop_back();
	}

	std::cout << std::endl;
	for(auto print_iter = container.begin(); print_iter != container.end(); print_iter++){
		std::cout << *print_iter;
	}
	std::cout << " has been removed!" << std::endl;
	return true;
};

template<class KeyType, class ContainerType>
void Trie<KeyType, ContainerType>::GetPrefixContainers(std::shared_ptr<NODE> current_node,
														std::deque<ContainerType> &result, ContainerType prefix) const{
	if (current_node->IsLeaf()) {
		result.push_back(prefix);
	}
	for (auto iter = current_node->nodes.begin(); iter != current_node->nodes.end(); iter++) {
		if (iter->second.get() != nullptr) {
			prefix.push_back(iter->first);
			GetPrefixContainers(iter->second, result, prefix);
			prefix.pop_back();
		}
	}
};

template<class KeyType, class ContainerType>
std::deque<ContainerType> Trie<KeyType, ContainerType>::ContainingPrefix(const ContainerType &prefix) const{

	std::deque<ContainerType> result;
	auto current_node = root;
	for (auto iter = prefix.begin(); iter != prefix.end(); iter++) {
		current_node = current_node->GetChild(*iter);
		if (current_node.get() == nullptr) {
			return result;
		}
	}
	GetPrefixContainers(current_node, result, prefix);
	return result;
};

int main(int argc, char const *argv[]){
	Trie<char, std::string> words;
	words.Insert("aa");
	words.Insert("ab");		
	words.Insert("aabb");
	words.Insert("abab");
	words.Insert("Dolphin");
	words.Insert("Dodo");
	words.Insert("Dodorina");

	std::cout << std::endl;

	words.Find("AA");
	words.Find("Dodo");
	words.Find("Dugtrio");
	words.Find("Dodorina");
	words.Find("aab");
	words.Find("aabb");

	std::cout << std::endl << "Containing prefix a: " << std::endl;

	std::deque<std::string> a = words.ContainingPrefix("a");
	for (auto iter = a.begin(); iter != a.end(); iter++) {
		std::cout << *iter << " ";
	}

	std::cout << std::endl;

	std::cout << std::endl << "Containing prefix D: " << std::endl;

	std::deque<std::string> D = words.ContainingPrefix("D");
	for (auto iter = D.begin(); iter != D.end(); iter++) {
		std::cout << *iter << " ";
	}

	std::cout << std::endl;

	std::cout << std::endl << "Containing prefix Dodo: " << std::endl;

	std::deque<std::string> Dodo = words.ContainingPrefix("Dodo");
	for (auto iter = Dodo.begin(); iter != Dodo.end(); iter++) {
		std::cout << *iter << " ";
	}

	std::cout << std::endl;

	words.Remove("Dodorina");

	words.Find("Dolphin");
	words.Find("Dodo");
	words.Find("Dodorina");

	words.Remove("Dodo");
	
	words.Find("Dolphin");
	words.Find("Dodo");
	words.Find("Dodorina");	

	std::cout << std::endl;

	Trie<int, std::deque<int>> integer_numbers;
	std::deque<int> digits1;
	digits1.push_back(1);
	digits1.push_back(1);
	digits1.push_back(1);

	std::deque<int> digits2;
	digits2.push_back(1);
	digits2.push_back(1);
	digits2.push_back(2);

	std::deque<int> digits3;
	digits3.push_back(4);
	digits3.push_back(3);

	integer_numbers.Insert(digits1);
	integer_numbers.Insert(digits2);
	integer_numbers.Insert(digits3);

	std::cout << std::endl;

	integer_numbers.Find(digits1);
	integer_numbers.Find(digits2);
	integer_numbers.Find(digits3);
	
	integer_numbers.Remove(digits3);

	integer_numbers.Find(digits1);
	integer_numbers.Find(digits2);
	integer_numbers.Find(digits3);

	integer_numbers.Remove(digits1);

	integer_numbers.Find(digits1);
	integer_numbers.Find(digits2);
	integer_numbers.Find(digits3);
	
	return 0;
}