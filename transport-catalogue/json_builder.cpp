#include "json_builder.h"

namespace json {

Builder::KeyItemContext Builder::Key(std::string key) {
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

Builder::DictItemContext Builder::StartDict(){
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

Builder::ArrayItemContext Builder::StartArray(){
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

Builder& Builder::BaseContext::GetBuilder(){
        return builder_;
}

Builder::KeyItemContext Builder::BaseContext::Key(std::string key) {
    return KeyItemContext(builder_.Key(key));
}
    
Builder& Builder::BaseContext::Value(Node::Value value){
    return builder_.Value(value);
}
    
Builder::DictItemContext Builder::BaseContext::StartDict() {
    return builder_.StartDict();
}
    
Builder::ArrayItemContext Builder::BaseContext::StartArray() {
    return builder_.StartArray();
}
    
Builder& Builder::BaseContext::EndDict() {
    return builder_.EndDict();
}
    
Builder& Builder::BaseContext::EndArray() {
    return builder_.EndArray();
}  



//rule 1 Непосредственно после Key вызван не Value, не StartDict и не StartArray.

//ValueItemContextAfterKey KeyItemContext::Value(Node::Value value) {
Builder::KeyItemContext Builder::KeyItemContext::Value(Node::Value value) {
    return KeyItemContext(GetBuilder().Value(value));
   
} 
    

//rule 4 За вызовом StartArray следует не Value, не StartDict, не StartArray и не EndArray.
//ItemContextAfterArrayWithValue ArrayItemContext::Value(Node::Value

Builder::ArrayItemContext Builder::ArrayItemContext::Value(Node::Value value) {
    return ArrayItemContext(GetBuilder().Value(value));

}    

}

    
   
    

