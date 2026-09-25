#include "openai.hpp"
#include "OpenaiExampleApp.hpp"

#include <iostream>
#include <QTimer>

int main() {
    OpenaiExampleApp app;
    openai::start();

    // Audio endpoints use multipart. The library exposes them as synchronous helpers,
    // so we schedule them from the event loop to show an async-style flow.
    QTimer::singleShot(0, &app, [&app]() {
        auto transcription = openai::instance().audio.transcribe(QJsonObject{
            {"file", "audio.mp3"},
            {"model", "whisper-1"}
        });
        std::cout << "Response is:\n"
                  << QJsonDocument(transcription).toJson(QJsonDocument::Indented).toStdString()
                  << '\n';

        auto translation = openai::instance().audio.translate(QJsonObject{
            {"file", "german.m4a"},
            {"model", "whisper-1"}
        });
        std::cout << "Response is:\n"
                  << QJsonDocument(translation).toJson(QJsonDocument::Indented).toStdString()
                  << '\n';

        app.quit();
    });

    return app.exec();
}
