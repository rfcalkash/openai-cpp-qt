#include "openai.hpp"
#include "OpenaiExampleApp.hpp"

#include <iostream>

int main() {
    OpenaiExampleApp app;
    openai::start();

    auto edit = openai::edit().create(QJsonObject{
        {"model", "text-davinci-edit-001"},
        {"input", "What day of the wek is it?"},
        {"instruction", "Fix the spelling mistakes"}
    });
    std::cout << "Response is:\n" << QJsonDocument(edit).toJson(QJsonDocument::Indented).toStdString() << '\n';
}
