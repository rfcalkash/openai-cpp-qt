#include "openai.hpp"
#include "OpenaiExampleApp.hpp"

#include <iostream>

int main() {
    OpenaiExampleApp app;
    openai::start();

    auto completion = openai::completion().create(QJsonObject{
        {"model", "text-davinci-003"},
        {"prompt", "Say this is a test"},
        {"max_tokens", 7},
        {"temperature", 0}
    });
    std::cout << "Response is:\n" << QJsonDocument(completion).toJson(QJsonDocument::Indented).toStdString() << '\n';
}
