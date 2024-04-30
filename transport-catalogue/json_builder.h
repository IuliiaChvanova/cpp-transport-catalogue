#include "json.h"

namespace json {

class Builder {    

    class BaseContext;
    class KeyItemContext;
    class DictItemContext;
    class ArrayItemContext;
    
    class BaseContext {
public:
    explicit BaseContext(Builder& builder_):builder_(builder_){};
    Builder& GetBuilder();
    KeyItemContext Key(std::string key);
    Builder& Value(Node::Value value);
    DictItemContext StartDict();
    ArrayItemContext StartArray();
    Builder& EndDict();
    Builder& EndArray();
private:
    Builder& builder_;
};
    //rule 1 Непосредственно после Key вызван не Value, не StartDict и не StartArray.
//rule 2 После вызова Value, последовавшего за вызовом Key, вызван не Key и не EndDict.   
    class KeyItemContext: public BaseContext {
public:    
    explicit KeyItemContext(Builder& builder):BaseContext(builder){};
  
    KeyItemContext Value(Node::Value value);
    Builder& EndArray() = delete;
};
    //rule 3  За вызовом StartDict следует не Key и не EndDict.
    class DictItemContext: public BaseContext {
public:
    explicit DictItemContext(Builder& builder):BaseContext(builder){};
    
    Builder& Value(Node::Value value) = delete;
    DictItemContext StartDict() = delete;
    ArrayItemContext StartArray() = delete;
    Builder& EndArray() = delete;

};
    //rule 4 За вызовом StartArray следует не Value, не StartDict, не StartArray и не EndArray.
//rule 5 После вызова StartArray и серии Value следует не Value, не StartDict, не StartArray и не EndArray.   
    class ArrayItemContext: public BaseContext {
public:    
    explicit ArrayItemContext(Builder& builder):BaseContext(builder){};
    
    ArrayItemContext Value(Node::Value value);
    KeyItemContext Key(std::string key) = delete;
    Builder& EndDict() = delete;
};
public:
    Builder() = default;
    KeyItemContext Key(std::string key);
    Builder& Value(Node::Value value);
    DictItemContext StartDict();
    ArrayItemContext StartArray();
    Builder& EndDict();
    Builder& EndArray();
    Node Build();
private:
    Node root_; 
    std::vector<Node*> nodes_stack_;
};

/*
1) Непосредственно после Key вызван не Value, не StartDict и не StartArray.
2) После вызова Value, последовавшего за вызовом Key, вызван не Key и не EndDict.
3) За вызовом StartDict следует не Key и не EndDict.
4) За вызовом StartArray следует не Value, не StartDict, не StartArray и не EndArray.
5) После вызова StartArray и серии Value следует не Value, не StartDict, не StartArray и не EndArray.


json::Builder{}.StartDict().Key("1"s).Key(""s);  // правило 1
json::Builder{}.StartDict().Key("1"s).Value(1).Value(1);  // правило 2
json::Builder{}.StartDict().Build();  // правило 3
json::Builder{}.StartArray().Key("1"s);  // правило 4
json::Builder{}.StartArray().EndDict();  // правило 4
json::Builder{}.StartArray().Value(1).Value(2).EndDict();  // правило 5
*/


} //namespace json 



