#include "openai.hpp"
#include "OpenaiExampleApp.hpp"

#include <iostream>

// A class which handles a openai instance by reference
class Foo {
    std::reference_wrapper<openai::OpenAI> openai_;
    // Or
    // OpenAI& openai_;

public:
    Foo(openai::OpenAI& openai) : openai_{openai} {
        auto res = openai_.get().completion.create(QJsonObject{
            {"model", "text-davinci-003"},
            {"prompt", "Say Foo class ctor called"}
        }); // parameters channel, username, iconemoji reused from bar()
        std::cout << QJsonDocument(res).toJson().toStdString() << "\n\n";
    }
}; 

// Pass by ref in parameter function
void bar(openai::OpenAI& openai) {
    // You can try catch API method if an error occur
    try {
        auto res = openai.completion.create(QJsonObject{
            {"model", "text-davinci-003"},
            {"prompt", "Say bar() function called"}
        });
        std::cout << QJsonDocument(res).toJson().toStdString() << "\n\n";
    }
    catch(std::exception const& e) {
        std::cerr << "Exception:" <<  e.what() << "\n\n";
    }
}


int main() {
    OpenaiExampleApp app;
    // Create openai instance
    auto openai_instance = openai::OpenAI::create();
    bar(openai_instance);
    Foo foo{openai_instance};

    // You can create other openai instances with different tokens and parameters
    {
        auto another_openai_instance = openai::OpenAI::create("wrong api key");
        // another_openai_instance.setThrowException(false); // You can set throw exception to false if you want
        
        try {
            another_openai_instance.completion.create(QJsonObject{
                {"model", "text-davinci-003"},
                {"prompt", "Say this should throw since token is invalid here"}
            }); 
        }
        catch(std::exception const& e) {
            std::cerr << "09-instances failed purposely because of " << e.what() << "\n\n";
        }
    }
}
