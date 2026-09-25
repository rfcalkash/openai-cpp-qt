#include "openai.hpp"
#include "OpenaiExampleApp.hpp"

#include <iostream>

int main() {
    OpenaiExampleApp app;
    openai::start();

    openai::instance().postAsync("completions",
        QJsonObject{
            {"model", "text-davinci-003"},
            {"prompt", "Say this is a test"},
            {"max_tokens", 7},
            {"temperature", 0}
        },
        app.asyncDone([&app](openai::Json completion) {
            std::cout << "Response is:\n"
                      << QJsonDocument(completion).toJson(QJsonDocument::Indented).toStdString()
                      << '\n';

            openai::instance().postAsync("images/generations",
                QJsonObject{
                    {"prompt", "A logo with a cello in a heart"},
                    {"n", 1},
                    {"size", "512x512"}
                },
                app.asyncDone([](openai::Json image) {
                    std::cout << "Image URL is: "
                              << image["data"].toArray()[0].toObject()["url"].toString().toStdString()
                              << '\n';
                }));
        }));

    return app.runUntilDone();
}
