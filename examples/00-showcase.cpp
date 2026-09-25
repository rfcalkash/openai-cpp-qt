#include "openai.hpp"
#include "OpenaiExampleApp.hpp"

#include <iostream>

int main() {
    OpenaiExampleApp app;
    openai::start(); // Will use the api key provided by `OPENAI_API_KEY` environment variable
    // openai::start("your_API_key", "optional_organization"); // Or you can handle it yourself

    auto completion = openai::completion().create(QJsonObject{
        {"model", "text-davinci-003"},
        {"prompt", "Say this is a test"},
        {"max_tokens", 7},
        {"temperature", 0}
    });
    std::cout << "Response is:\n" << QJsonDocument(completion).toJson(QJsonDocument::Indented).toStdString() << '\n';

    auto image = openai::image().create(QJsonObject{
        {"prompt", "A logo with a cello in a heart"},
        {"n", 1},
        {"size", "512x512"}
    }); // Using initializer lists
    std::cout << "Image URL is: " << image["data"].toArray()[0].toObject()["url"].toString().toStdString() << '\n';
}
