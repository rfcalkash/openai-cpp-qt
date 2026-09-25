#include "openai.hpp"
#include "OpenaiExampleApp.hpp"

#include <iostream>

int main() {
    OpenaiExampleApp app;
    openai::start();

    auto chat = openai::chat().create(QJsonObject{
        {"model", "gpt-3.5-turbo"},
        {"messages", QJsonArray{{QJsonObject{{"role", "user"}, {"content", "blah"}}}}},
        {"max_tokens", 7},
        {"temperature", 0}
    });
    std::cout << "Response is:\n" << QJsonDocument(chat).toJson(QJsonDocument::Indented).toStdString() << '\n';
}