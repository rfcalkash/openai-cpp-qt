#include "openai.hpp"
#include "OpenaiExampleApp.hpp"

#include <iostream>

int main() {
    OpenaiExampleApp app;
    openai::start();

    openai::instance().postAsync("edits",
        QJsonObject{
            {"model", "text-davinci-edit-001"},
            {"input", "What day of the wek is it?"},
            {"instruction", "Fix the spelling mistakes"}
        },
        app.asyncDone([](openai::Json edit) {
            std::cout << "Response is:\n"
                      << QJsonDocument(edit).toJson(QJsonDocument::Indented).toStdString()
                      << '\n';
        }));

    return app.runUntilDone();
}
