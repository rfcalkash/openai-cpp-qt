# OpenAI C++ library (Qt fork)

[![Language](https://img.shields.io/badge/language-C++-blue.svg)](https://isocpp.org/)  [![Standard](https://img.shields.io/badge/c%2B%2B-17-blue.svg)](https://en.wikipedia.org/wiki/C%2B%2B#Standardization) [![License](https://img.shields.io/github/license/mashape/apistatus.svg)](https://opensource.org/licenses/MIT) ![Github worflow](https://github.com/olrea/openai-cpp/actions/workflows/cmake.yml/badge.svg)
 [![GitHub version](https://badge.fury.io/gh/olrea%2Fopenai-cpp.svg)](https://github.com/olrea/openai-cpp/releases) 

## About this fork

This project is a fork of the community-maintained **[OpenAI-C++](https://github.com/olrea/openai-cpp)** library originally written by **Olrea** and **Florian Dang**.
The original library provided convenient access to the [OpenAI API](https://openai.com/api/) from C++ applications and was distributed as a lightweight header-only library.

This fork keeps the same high-level API structure and the original acknowledgement section below, but performs a major refactor of the underlying engine:

* **CURL has been removed.** All HTTP requests are now handled by **Qt Network** (`QNetworkAccessManager`, `QNetworkRequest`, `QNetworkReply`).
* **nlohmann/json has been removed.** All JSON input/output now uses **Qt Core** JSON classes (`QJsonObject`, `QJsonArray`, `QJsonDocument`, `QJsonValue`).
* **Qt integration** makes the library a natural fit for Qt desktop, mobile or embedded projects.
* **Async support** is added on top of Qt's event-driven networking, while the classic synchronous API remains available.

If you are migrating from the original library, replace `nlohmann::json` payloads with `QJsonObject`. The category methods (`openai::completion().create`, `openai::image().create`, `openai::chat().create`, etc.) still exist and behave the same way.

## A lightweight header-only modern C++ library

OpenAI-C++ library is a **community-maintained** library which provides convenient access to the [OpenAI API](https://openai.com/api/) from applications written in the C++ language.  
The library is small with a single header file: `include/openai/openai.hpp`.

## Requirements

+ C++17/C++20 compatible compiler
+ Qt 6 (`Core` and `Network` components are required)
+ A valid OpenAI API key, either passed explicitly or through the `OPENAI_API_KEY` environment variable

## OpenAI C++ current implementation

The library should implement all requests on [OpenAI references](https://platform.openai.com/docs/api-reference). If any are missing (due to an update), feel free to open an issue.

| API reference | Method | Example file |
| --- | --- | --- |
| API models | [List models](https://platform.openai.com/docs/api-reference/models/list) ✅ | [1-model.cpp](examples/01-model.cpp) / [1-model_async.cpp](examples/01-model_async.cpp) |
| API models | [Retrieve model](https://platform.openai.com/docs/api-reference/models/retrieve) ✅ | [1-model.cpp](examples/01-model.cpp) / [1-model_async.cpp](examples/01-model_async.cpp) |
| API completions | [Create completion](https://platform.openai.com/docs/api-reference/completions/create) ✅ | [2-completion.cpp](examples/02-completion.cpp) / [2-completion_async.cpp](examples/02-completion_async.cpp) |
| API edits | [Create completion](https://platform.openai.com/docs/api-reference/completions/create) | [3-edit.cpp](examples/03-edit.cpp) / [3-edit_async.cpp](examples/03-edit_async.cpp) |
| API images | [Create image](https://platform.openai.com/docs/api-reference/images) ✅ | [4-image.cpp](examples/04-image.cpp) / [4-image_async.cpp](examples/04-image_async.cpp) |
| API images | [Create image edit](https://platform.openai.com/docs/api-reference/images/create-edit) ✅ | [4-image.cpp](examples/04-image.cpp) / [4-image_async.cpp](examples/04-image_async.cpp) |
| API images | [Create image variation](https://platform.openai.com/docs/api-reference/images/create-variation) ✅ | [4-image.cpp](examples/04-image.cpp) / [4-image_async.cpp](examples/04-image_async.cpp) |
| API embeddings | [Create embeddings](https://platform.openai.com/docs/api-reference/embeddings/create) ✅ | [5-embedding.cpp](examples/05-embedding.cpp) / [5-embedding_async.cpp](examples/05-embedding_async.cpp) |
| API files | [List file](https://platform.openai.com/docs/api-reference/files/list) ✅ | [6-file.cpp](examples/06-file.cpp) / [6-file_async.cpp](examples/06-file_async.cpp) |
| API files | [Upload file](https://platform.openai.com/docs/api-reference/files/upload) ✅ | [6-file.cpp](examples/06-file.cpp) / [6-file_async.cpp](examples/06-file_async.cpp) |
| API files | [Delete file](https://platform.openai.com/docs/api-reference/files/delete) ✅ | [6-file.cpp](examples/06-file.cpp) / [6-file_async.cpp](examples/06-file_async.cpp) |
| API files | [Retrieve file](https://platform.openai.com/docs/api-reference/files/retrieve) ✅ | [6-file.cpp](examples/06-file.cpp) / [6-file_async.cpp](examples/06-file_async.cpp) |
| API files | [Retrieve file content](https://platform.openai.com/docs/api-reference/files/retrieve-content) ✅ | [6-file.cpp](examples/06-file.cpp) / [6-file_async.cpp](examples/06-file_async.cpp) |
| API fine-tunes | [Create fine-tune](https://platform.openai.com/docs/api-reference/fine-tunes/create) ✅ | [7-fine-tune.cpp](examples/07-fine-tune.cpp) / [7-fine-tune_async.cpp](examples/07-fine-tune_async.cpp) |
| API fine-tunes | [List fine-tune](https://platform.openai.com/docs/api-reference/fine-tunes/list) ✅ | [7-fine-tune.cpp](examples/07-fine-tune.cpp) / [7-fine-tune_async.cpp](examples/07-fine-tune_async.cpp) |
| API fine-tunes | [Retrieve fine-tune](https://platform.openai.com/docs/api-reference/fine-tunes/retrieve) ✅ | [7-fine-tune.cpp](examples/07-fine-tune.cpp) / [7-fine-tune_async.cpp](examples/07-fine-tune_async.cpp) |
| API fine-tunes | [Cancel fine-tune](https://platform.openai.com/docs/api-reference/fine-tunes/cancel) ✅ | [7-fine-tune.cpp](examples/07-fine-tune.cpp) / [7-fine-tune_async.cpp](examples/07-fine-tune_async.cpp) |
| API fine-tunes | [List fine-tune events](https://platform.openai.com/docs/api-reference/fine-tunes/events) ✅ | [7-fine-tune.cpp](examples/07-fine-tune.cpp) / [7-fine-tune_async.cpp](examples/07-fine-tune_async.cpp) |
| API fine-tunes | [Delete fine-tune model](https://platform.openai.com/docs/api-reference/fine-tunes/delete-model) ✅ | [7-fine-tune.cpp](examples/07-fine-tune.cpp) / [7-fine-tune_async.cpp](examples/07-fine-tune_async.cpp) |
| API chat | [Create chat completion](https://platform.openai.com/docs/api-reference/chat/create) ✅ | [10-chat.cpp](examples/10-chat.cpp) / [10-chat_async.cpp](examples/10-chat_async.cpp) |
| API audio | [Create transcription](https://platform.openai.com/docs/api-reference/audio/create) ✅ | [11-audio.cpp](examples/11-audio.cpp) / [11-audio_async.cpp](examples/11-audio_async.cpp) |
| API audio | [Create translation](https://platform.openai.com/docs/api-reference/audio/create) ✅ | [11-audio.cpp](examples/11-audio.cpp) / [11-audio_async.cpp](examples/11-audio_async.cpp) |
| API moderation | [Create moderation](https://platform.openai.com/docs/api-reference/moderations/create) ✅ | [12-moderation.cpp](examples/12-moderation.cpp) / [12-moderation_async.cpp](examples/12-moderation_async.cpp) |

## Installation

The library consists of one file: [include/openai/openai.hpp](include/openai/openai.hpp).
Just copy the [include/openai](include/openai) folder into your project and use `#include "openai.hpp"`.

Your build system must link the Qt 6 `Core` and `Network` libraries:

```cmake
find_package(Qt6 REQUIRED COMPONENTS Core Network)
target_link_libraries(your_target Qt6::Core Qt6::Network)
```

If you build from a `QObject`-based project (e.g. Qt Widgets / QML), you can pass an existing `QNetworkAccessManager*` to the session so it shares the same thread and event loop with the rest of your application.

## Usage

### Simple synchronous showcase

The library needs to be configured with your account's secret key which is available on the [website](https://platform.openai.com/account/api-keys). It is recommended to set your `OPENAI_API_KEY` environment variable before using the library (or you can also set the API key directly in the code):

```bash
export OPENAI_API_KEY='sk-...'
```

The synchronous version of the following code is available at [examples/00-showcase.cpp](examples/00-showcase.cpp).

```cpp
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
    std::cout << "Response is:\n"
              << QJsonDocument(completion).toJson(QJsonDocument::Indented).toStdString()
              << '\n';

    auto image = openai::image().create(QJsonObject{
        {"prompt", "A cute koala playing the violin"},
        {"n", 1},
        {"size", "512x512"}
    });
    std::cout << "Image URL is: "
              << image["data"].toArray()[0].toObject()["url"].toString().toStdString()
              << '\n';
}
```

Since `openai::Json` is now a typedef to `QJsonObject`, payload construction uses Qt JSON initializer lists and responses are read back with Qt's standard accessors.

### Asynchronous usage

Asynchronous requests are Qt-native: the caller stays non-blocking and a callback is invoked when the network reply finishes.
The pattern is implemented in the same header and demonstrated by the `*_async.cpp` examples.

The async version of the showcase is available at [examples/00-showcase_async.cpp](examples/00-showcase_async.cpp).

```cpp
#include "openai.hpp"
#include "OpenaiExampleApp.hpp" // helper used by the async examples
#include <iostream>

int main() {
    OpenaiExampleApp app; // a tiny QCoreApplication wrapper used by the Qt examples
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
                    {"prompt", "A cute koala playing the violin"},
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
```

Key differences from synchronous mode:

* You must run a Qt event loop (`QCoreApplication::exec()`) in your process.
* The request methods are `postAsync`, `getAsync` and `delAsync` instead of `post`, `get` and `del`.
* The response is delivered through a callback taking `openai::Json`.
* `OpenaiExampleApp` in the examples only helps counting pending callbacks and quitting the event loop automatically; in a real GUI or service, you already have your own `QCoreApplication/QApplication` event loop.

### Build the examples

```bash
mkdir build && cd build
cmake .. && make
examples/00-showcase
examples/00-showcase_async
examples/01-model
examples/01-model_async
# ...
```

In your project, if you want to get verbose output like when running the examples, you can define `#define OPENAI_VERBOSE_OUTPUT` (or pass `-DOPENAI_VERBOSE_OUTPUT=1` to the compiler).

### Advanced usage

#### A word about error handling

By default, **OpenAI-CPP** will throw a runtime error exception if the request does not succeed. You are free to handle these exceptions the way you like.
You can prevent throw exceptions by setting `setThrowException(false)` (see example in [examples/09-instances.cpp](examples/09-instances.cpp)). If you do that, a warning will be displayed instead.

### More control

You can use the `openai::post()`, `openai::get()` or `openai::del()` methods to fully control what you are sending (e.g. can be useful when a new method from OpenAI API is available and not provided by **OpenAI-CPP** yet).

For asynchronous low-level access, use `openai::instance().postAsync(...)`, `openai::instance().getAsync(...)` or `openai::instance().delAsync(...)`.

#### Manage OpenAI-CPP instance

Here are two approaches to keep alive the **OpenAI-CPP** session in your program so you can use it anytime, anywhere.

##### Use the default instance()

This is the default behavior. **OpenAI-CPP** provides free convenient functions : `openai::start(const QString& token)` and `openai::instance()`.
Initialize and configure the **OpenAI-CPP** instance with:

```cpp
auto& openai = openai::start();
```

When you are in another scope and you have lost the `openai` reference, you can grab it again with :  

```cpp
auto& openai = openai::instance();
```

It might not be the recommended way but since we generally want to handle only one OpenAI instance (one token), this approach is highly convenient.

##### Pass by reference if you want to manage multiple secret keys

An other approach is to pass the *OpenAI* instance by reference, store it, and call the appropriate methods when needed.

```cpp
void bar(openai::OpenAI& openai) {
    openai.completion.create(QJsonObject{
        {"model", "text-davinci-003"},
        {"prompt", "Say bar() function called"}
    });
}

int main() {
    openai::OpenAI openai_instance = openai::OpenAI::create("your_api_key");
    bar(openai_instance);
}
```

You can use a [std::reference_wrapper](http://en.cppreference.com/w/cpp/utility/functional/reference_wrapper) as shown in [examples/09-instances.cpp](examples/09-instances.cpp).

This strategy is useful if you have to manage several OpenAI-CPP instances with different secret keys.

## License

[MIT](LICENSE.md)

## Acknowledgment

This work has been mainly inspired by [slacking](https://github.com/coin-au-carre/slacking) and the curl wrapper code from [cpr](https://github.com/libcpr/cpr).

The original **OpenAI-C++** library was created by [Olrea](https://github.com/olrea) and Florian Dang and is available at [https://github.com/olrea/openai-cpp](https://github.com/olrea/openai-cpp). This fork replaces its CURL/nlohmann/json backends with Qt's Core / Network modules while preserving most of the public API.

