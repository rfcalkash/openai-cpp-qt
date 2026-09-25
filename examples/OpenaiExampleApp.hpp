#ifndef OPENAI_EXAMPLE_APP_HPP_
#define OPENAI_EXAMPLE_APP_HPP_

#include <QCoreApplication>
#include <utility>

// Convenience wrapper around QCoreApplication for the openai-cpp examples.
// It provides a default constructor (no need to manage argc/argv manually)
// and helpers to run the event loop until all outstanding async requests finish.
class OpenaiExampleApp : public QCoreApplication {
public:
    OpenaiExampleApp() : QCoreApplication(dummy_argc_, dummy_argv_), pending_(0) {}

    // Wrap an async callback so that the application quits the event loop
    // once every outstanding request has completed.
    template <typename F>
    auto asyncDone(F&& callback) {
        ++pending_;
        return [this, callback = std::forward<F>(callback)](auto&&... args) mutable {
            callback(std::forward<decltype(args)>(args)...);
            if (--pending_ == 0) {
                quit();
            }
        };
    }

    // Run the event loop only if there are pending async operations.
    int runUntilDone() {
        if (pending_ == 0) return 0;
        return exec();
    }

private:
    static inline char app_name_[] = "openai_example";
    static inline int dummy_argc_ = 1;
    static inline char* dummy_argv_[] = { app_name_, nullptr };

    int pending_;
};

#endif // OPENAI_EXAMPLE_APP_HPP_
