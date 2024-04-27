#include "json_builder.h"

namespace json {

KeyItemContext Builder::Key(std::string key) {
    if (nodes_stack_.empty()){
        throw std::logic_error("Dictionary was not initialized");
    }

    if (Node{} == *nodes_stack_.back() &&
            !nodes_stack_.back()->IsArray() && !nodes_stack_.back()->IsDict()) {
            throw std::logic_error("No initialization for previous key value");
    }
    
    if (nodes_stack_.back()->IsArray()) {
            throw std::logic_error("Array declaration was not finished");
    }

     if(nodes_stack_.back()->IsDict()){
        Dict& dict = std::get<Dict>(nodes_stack_.back()->GetNonConstValue());
        nodes_stack_.emplace_back(&dict[key]);
    }
   
    return KeyItemContext(*this);
}


Builder& Builder::Value(Node::Value value){
     //case json::Builder{}.Value("s"s).Value("1"s).Build()
    if (Node{} != root_ && nodes_stack_.empty()) {
            throw std::logic_error("Object is not ready Node{} != root_ && nodes_stack_ is empty");
    }
    
    
   
    if (nodes_stack_.empty()){
        root_ = value;
        return *this;
    } 
   
    
    if (nodes_stack_.back()->IsArray()){
        Array& arr = std::get<Array>(nodes_stack_.back()->GetNonConstValue());
        arr.emplace_back(value);
    } else {
        *nodes_stack_.back() = value;
        nodes_stack_.pop_back();
    }
 
    return *this; 
}

DictItemContext Builder::StartDict(){
    if(Node{} == root_){
        root_ = Dict{};
        nodes_stack_.emplace_back(&root_);
    } else if (nodes_stack_.empty()){
        throw std::logic_error("nodes_stack_ is empty. Example json::Builder{}.Value(1).StartDict().Build()");
    } else if (nodes_stack_.back()->IsDict()){
        throw std::logic_error("Key can not be dictionary");
    }
    else if(nodes_stack_.back()->IsArray()){
        Array& arr = std::get<Array>(nodes_stack_.back()->GetNonConstValue());
        auto& ref_dict = arr.emplace_back(Node(Dict{}));
        nodes_stack_.emplace_back(&ref_dict);
    } else if (Node{} == *nodes_stack_.back() &&
                   !nodes_stack_.back()->IsArray() && !nodes_stack_.back()->IsDict()) {
            *nodes_stack_.back() = Dict{};
       
    } 
    else {
            nodes_stack_.push_back(new Node(Dict{}));
    }
    return DictItemContext(*this);
}

ArrayItemContext Builder::StartArray(){
    if(Node{} == root_){
        root_ = Array{};
        nodes_stack_.emplace_back(&root_);
        return ArrayItemContext(*this);
    } else if (nodes_stack_.empty()){
        throw std::logic_error("nodes_stack_ is empty. Example json::Builder{}.Value(1).StartArray().Build()");
    }else if (nodes_stack_.back()->IsDict()) {
            throw std::logic_error("Array cannot be key of dict");
        }
    if(*nodes_stack_.back() == Node{}){
       *nodes_stack_.back() = Node(Array{}); 
    } else {
        nodes_stack_.emplace_back(new Node(Array{}));
    }
    return ArrayItemContext(*this);
}

Builder& Builder::EndDict(){
    if(nodes_stack_.empty()){
        throw std::logic_error("Dictionary is not ready");
    }
    if(nodes_stack_.back()->IsDict()){
        nodes_stack_.pop_back();
    } else {
        throw std::logic_error ("Contexts conflict");
    }
    return *this;

}

  Builder& Builder::EndArray() {
        if (nodes_stack_.empty()) {
            throw std::logic_error("Array is not ready");
        }
        if (nodes_stack_.back()->IsArray()) {
            nodes_stack_.pop_back();
        } else {
            throw std::logic_error("Contexts conflict");
        }
        return *this;
    }


Node Builder::Build() {
    if (Node{} == root_) {
            throw std::logic_error("Object is not ready");
        }
    if (!nodes_stack_.empty()) {
            throw std::logic_error("nodes_stack_ is not empty. Maybe initialization of arrays or dictionaries is not finished");
    }
    return root_;
}



//different Contexts

Builder& BaseContext::GetBuilder(){
        return builder_;
}


//rule 1 Непосредственно после Key вызван не Value, не StartDict и не StartArray.

ValueItemContextAfterKey KeyItemContext::Value(Node::Value value) {
    return ValueItemContextAfterKey(GetBuilder().Value(value));
}


DictItemContext KeyItemContext::StartDict() {
    return GetBuilder().StartDict();
}

ArrayItemContext KeyItemContext::StartArray() {
    return GetBuilder().StartArray();
}


//rule 2 После вызова Value, последовавшего за вызовом Key, вызван не Key и не EndDict.


KeyItemContext ValueItemContextAfterKey::Key(std::string key) {
    return GetBuilder().Key(key);
}

Builder& ValueItemContextAfterKey::EndDict() {
    return GetBuilder().EndDict();
}


//rule 3  За вызовом StartDict следует не Key и не EndDict.

KeyItemContext DictItemContext::Key(std::string key){
    return GetBuilder().Key(key);
}

Builder& DictItemContext::EndDict(){
    return GetBuilder().EndDict();
}


//rule 4 За вызовом StartArray следует не Value, не StartDict, не StartArray и не EndArray.

ItemContextAfterArrayWithValue ArrayItemContext::Value(Node::Value value) {
    return ItemContextAfterArrayWithValue(GetBuilder().Value(value));
}

DictItemContext ArrayItemContext::StartDict() {
    return GetBuilder().StartDict();
}

ArrayItemContext  ArrayItemContext::StartArray() {
    return GetBuilder().StartArray();
}

Builder& ArrayItemContext::EndArray() {
    return GetBuilder().EndArray();
}


//rule 5 После вызова StartArray и серии Value следует не Value, не StartDict, не StartArray и не EndArray.
    
ItemContextAfterArrayWithValue ItemContextAfterArrayWithValue::Value(Node::Value value) {
    return ItemContextAfterArrayWithValue(GetBuilder().Value(value));
}

DictItemContext ItemContextAfterArrayWithValue::StartDict() {
    return GetBuilder().StartDict();
}
ArrayItemContext ItemContextAfterArrayWithValue::StartArray() {
    return ArrayItemContext(GetBuilder().StartArray());
}
Builder& ItemContextAfterArrayWithValue::EndArray() {
    return GetBuilder().EndArray();
}



}

    
   
    

