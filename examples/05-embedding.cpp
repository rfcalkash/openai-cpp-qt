#include "openai.hpp"
#include "OpenaiExampleApp.hpp"

#include <iostream>

int main() {
    OpenaiExampleApp app;
    openai::start();

    auto res = openai::embedding().create(QJsonObject{
        {"model", "text-embedding-ada-002"},
        {"input", "The food was delicious and the waiter..."}
    });
    std::cout << "Response is:\n" << QJsonDocument(res).toJson(QJsonDocument::Indented).toStdString() << '\n';
}
