#include "openai.hpp"
#include "OpenaiExampleApp.hpp"

#include <iostream>

int main() {
    OpenaiExampleApp app;
    openai::start();

    // Image generation is a plain JSON POST -> can be fully async.
    openai::instance().postAsync("images/generations",
        QJsonObject{
            {"prompt", "A cute baby sea otter"},
            {"n", 2},
            {"size", "512x512"}
        },
        app.asyncDone([](openai::Json image) {
            std::cout << QJsonDocument(image).toJson(QJsonDocument::Indented).toStdString() << '\n';

            // Image edit / variation use multipart. The library currently exposes
            // those only as synchronous helpers, so call them inside the async callback.
            auto image_edit = openai::instance().image.edit(QJsonObject{
                {"image", "otter.png"},
                {"prompt", "A cute baby sea otter wearing a beret"},
                {"n", 1},
                {"size", "256x256"}
            });
            std::cout << QJsonDocument(image_edit).toJson(QJsonDocument::Indented).toStdString() << '\n';

            auto image_variation = openai::instance().image.variation(QJsonObject{
                {"image", "otter.png"},
                {"n", 2},
                {"size", "256x256"}
            });
            std::cout << QJsonDocument(image_variation).toJson(QJsonDocument::Indented).toStdString() << '\n';
        }));

    return app.runUntilDone();
}
