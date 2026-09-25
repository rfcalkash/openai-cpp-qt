#include "openai.hpp"
#include "OpenaiExampleApp.hpp"

#include <iostream>

int main() {
    OpenaiExampleApp app;
    openai::start();

    auto image = openai::image().create(QJsonObject{
        {"prompt", "A cute baby sea otter"},
        {"n", 2},
        {"size", "512x512"}
    });
    std::cout << QJsonDocument(image).toJson(QJsonDocument::Indented).toStdString() << '\n';

    auto image_edit = openai::image().edit(QJsonObject{
        {"image", "otter.png"},
        {"prompt", "A cute baby sea otter wearing a beret"},
        {"n", 1},
        {"size", "256x256"}
    });
    std::cout << QJsonDocument(image_edit).toJson(QJsonDocument::Indented).toStdString() << '\n';

    auto image_variation = openai::image().variation(QJsonObject{
        {"image", "otter.png"},
        {"n", 2},
        {"size", "256x256"}
    });
    std::cout << QJsonDocument(image_variation).toJson(QJsonDocument::Indented).toStdString() << '\n';
}
