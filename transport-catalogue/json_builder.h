#include "json.h"

namespace json {

    class BaseContext;
    class KeyItemContext;
    class ValueItemContextAfterKey;
    class DictItemContext;
    class ArrayItemContext;
    class ItemContextAfterArrayWithValue;

class Builder {
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


class BaseContext {
public:
    explicit BaseContext(Builder& builder_):builder_(builder_){};
    Builder& GetBuilder();
private:
    Builder& builder_;
};


//rule 1 Непосредственно после Key вызван не Value, не StartDict и не StartArray.
class KeyItemContext: public BaseContext {
public:    
    explicit KeyItemContext(Builder& builder):BaseContext(builder){};
    
    ValueItemContextAfterKey Value(Node::Value value);
    DictItemContext StartDict();
    ArrayItemContext StartArray();
};

//rule 2 После вызова Value, последовавшего за вызовом Key, вызван не Key и не EndDict.

class ValueItemContextAfterKey: public BaseContext {
public: 
    explicit ValueItemContextAfterKey(Builder& builder):BaseContext(builder){};
    
    KeyItemContext Key(std::string key);
    Builder& EndDict();
};



//rule 3  За вызовом StartDict следует не Key и не EndDict.
class DictItemContext: public BaseContext {
public:
    explicit DictItemContext(Builder& builder):BaseContext(builder){};
    
    KeyItemContext Key(std::string key);
    Builder& EndDict();

};


//rule 4 За вызовом StartArray следует не Value, не StartDict, не StartArray и не EndArray.

class ArrayItemContext: public BaseContext {
public:    
    explicit ArrayItemContext(Builder& builder):BaseContext(builder){};
    
    ItemContextAfterArrayWithValue Value(Node::Value value);
    DictItemContext StartDict();
    ArrayItemContext  StartArray();
    Builder& EndArray();
};


//rule 5 После вызова StartArray и серии Value следует не Value, не StartDict, не StartArray и не EndArray.
class ItemContextAfterArrayWithValue: public BaseContext {
public: 
    explicit ItemContextAfterArrayWithValue(Builder& builder):BaseContext(builder){};
    
    ItemContextAfterArrayWithValue Value(Node::Value value);
    DictItemContext StartDict();
    ArrayItemContext StartArray();
    Builder& EndArray();
};


} //namespace json

