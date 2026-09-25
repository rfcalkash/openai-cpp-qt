#include "openai.hpp"
#include "OpenaiExampleApp.hpp"

#include <iostream>

int main() {
    OpenaiExampleApp app;
    openai::start();

    {
        // Change "audio.mp3" to your audio file location
        auto transcription = openai::audio().transcribe(QJsonObject{
            {"file", "audio.mp3"},
            {"model", "whisper-1"}
        });
        std::cout << "Response is:\n" << QJsonDocument(transcription).toJson(QJsonDocument::Indented).toStdString() << '\n';
    }

    { 
        // Change "german.m4a" to your audio file location
        auto translation = openai::audio().translate(QJsonObject{
            {"file", "german.m4a"},
            {"model", "whisper-1"}
        });
        std::cout << "Response is:\n" << QJsonDocument(translation).toJson(QJsonDocument::Indented).toStdString() << '\n';
    }

}
