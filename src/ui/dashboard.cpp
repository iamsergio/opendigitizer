
#include "dashboard.h"

#include <fmt/format.h>
#include <imgui.h>
#include <implot.h>

#include <fstream>

#include <yaml-cpp/yaml.h>

#include "flowgraph.h"
#include "flowgraph/datasink.h"
#include "yamlutils.h"

namespace DigitizerUi {

namespace {
template<typename T>
inline T randomRange(T min, T max) {
    T scale = rand() / (T) RAND_MAX;
    return min + scale * (max - min);
}

uint32_t randomColor() {
    uint8_t x = randomRange(0.0f, 255.0f);
    uint8_t y = randomRange(0.0f, 255.0f);
    uint8_t z = randomRange(0.0f, 255.0f);
    return x << 24 | y << 16 | z << 8 | 0xff;
}

DashboardSource *unsavedSource() {
    static DashboardSource source = {
        .path    = "Unsaved",
        .isValid = false,
    };
    return &source;
}

} // namespace

Dashboard::Plot::Plot() {
    static int n = 1;
    name         = fmt::format("Plot {}", n++);
}

Dashboard::Dashboard(const std::shared_ptr<DashboardDescription> &desc, FlowGraph *fg)
    : m_desc(desc)
    , m_flowGraph(fg) {
    m_desc->lastUsed             = std::chrono::floor<std::chrono::days>(std::chrono::system_clock::now());

    fg->sourceBlockAddedCallback = [this](Block *b) {
        for (int i = 0; i < b->type->outputs.size(); ++i) {
            auto name = fmt::format("{}.{}", b->name, b->type->outputs[i].name);
            m_sources.insert({ b, i, name, randomColor() });
        }
    };
    fg->blockDeletedCallback = [this](Block *b) {
        for (auto &p : m_plots) {
            p.sources.erase(std::remove_if(p.sources.begin(), p.sources.end(), [=](auto *s) { return s->block == b; }),
                    p.sources.end());
        }
        m_sources.erase(std::remove_if(m_sources.begin(), m_sources.end(), [=](const auto &s) { return s.block == b; }),
                m_sources.end());
    };

    if (desc->source == unsavedSource()) {
        fg->clear();
    } else {
        fg->parse(std::filesystem::path(desc->source->path) / (desc->name + ".grc"));
    }

    // Load is called after parsing the flowgraph so that we already have the list of sources
    load();
}

Dashboard::~Dashboard() {
}

void Dashboard::setNewDescription(const std::shared_ptr<DashboardDescription> &desc) {
    m_desc = desc;
}

void Dashboard::load() {
    if (!m_desc->source->isValid) {
        return;
    }

    auto          path = std::filesystem::path(m_desc->source->path);
    std::ifstream stream(path / (m_desc->name + DashboardDescription::fileExtension), std::ios::in);
    if (!stream.is_open()) {
        return;
    }
    YAML::Node tree = YAML::Load(stream);

#ifdef NDEBUG
#define ERROR_RETURN \
    { \
        fmt::print("Error parsing YAML {}\n", path.native()); \
        abort(); \
    }
#else
#define ERROR_RETURN \
    { \
        fmt::print("Error parsing YAML {}\n", path.native()); \
        return; \
    }
#endif

    auto sources = tree["sources"];
    if (!sources || !sources.IsSequence()) ERROR_RETURN;

    for (const auto &s : sources) {
        if (!s.IsMap()) ERROR_RETURN;

        auto block = s["block"];
        auto port  = s["port"];
        auto name  = s["name"];
        auto color = s["color"];
        if (!block || !block.IsScalar() || !port || !port.IsScalar() || !name || !name.IsScalar() || !color || !color.IsScalar()) ERROR_RETURN;

        auto blockStr = block.as<std::string>();
        auto portNum  = port.as<int>();
        auto nameStr  = name.as<std::string>();
        auto colorNum = color.as<uint32_t>();

        auto source   = std::find_if(m_sources.begin(), m_sources.end(), [&](const auto &s) {
            return s.block->name == blockStr && s.port == portNum;
        });
        if (source == m_sources.end()) {
            fmt::print("Unable to find the source '{}.{}'\n", blockStr, portNum);
            return;
        }

        source->name  = nameStr;
        source->color = colorNum;
    }

    auto plots = tree["plots"];
    if (!plots || !plots.IsSequence()) ERROR_RETURN;

    for (const auto &p : plots) {
        if (!p.IsMap()) ERROR_RETURN;

        auto name    = p["name"];
        auto axes    = p["axes"];
        auto sources = p["sources"];
        if (!name || !name.IsScalar() || !axes || !axes.IsSequence() || !sources || !sources.IsSequence()) ERROR_RETURN;

        m_plots.push_back({});
        auto &plot = m_plots.back();
        plot.name  = name.as<std::string>();

        for (const auto &a : axes) {
            if (!a.IsMap()) ERROR_RETURN;

            auto axis = a["axis"];
            auto min  = a["min"];
            auto max  = a["max"];

            if (!axis || !axis.IsScalar() || !min || !min.IsScalar() || !max || !max.IsScalar()) ERROR_RETURN;

            plot.axes.push_back({});
            auto &ax      = plot.axes.back();

            auto  axisStr = axis.as<std::string>();

            if (axisStr == "X") {
                ax.axis = Plot::Axis::X;
            } else if (axisStr == "Y") {
                ax.axis = Plot::Axis::Y;
            } else {
                fmt::print("Unknown axis {}\n", axisStr);
                return;
            }

            ax.min = min.as<double>();
            ax.max = max.as<double>();
        }

        for (const auto &s : sources) {
            if (!s.IsScalar()) ERROR_RETURN;

            auto str    = s.as<std::string>();
            auto source = std::find_if(m_sources.begin(), m_sources.end(), [&](const auto &s) { return s.name == str; });
            if (source == m_sources.end()) {
                fmt::print("Unable to find source {}\n", str);
            }
            plot.sources.push_back(&*source);
        }
    }

#undef ERROR_RETURN
}

void Dashboard::save() {
    if (!m_desc->source->isValid) {
        return;
    }

    auto path = std::filesystem::path(m_desc->source->path);
    m_flowGraph->save(path / (m_desc->name + ".grc"));

    YAML::Emitter out;
    {
        YamlMap root(out);

        root.write("favorite", m_desc->isFavorite);
        std::chrono::year_month_day ymd(std::chrono::floor<std::chrono::days>(m_desc->lastUsed.value()));
        char                        lastUsed[11];
        fmt::format_to(lastUsed, "{:02}/{:02}/{:04}", static_cast<unsigned>(ymd.day()), static_cast<unsigned>(ymd.month()), static_cast<int>(ymd.year()));
        root.write("lastUsed", lastUsed);

        root.write("sources", [&]() {
            YamlSeq sources(out);

            for (auto &s : m_sources) {
                YamlMap source(out);
                source.write("name", s.name);

                source.write("block", s.block->name);
                source.write("port", s.port);
                source.write("color", s.color);
            }
        });

        root.write("plots", [&]() {
            YamlSeq plots(out);

            for (auto &p : m_plots) {
                YamlMap plot(out);
                plot.write("name", p.name);
                plot.write("axes", [&]() {
                    YamlSeq axes(out);

                    for (const auto &axis : p.axes) {
                        YamlMap a(out);
                        a.write("axis", axis.axis == Plot::Axis::X ? "X" : "Y");
                        a.write("min", axis.min);
                        a.write("max", axis.max);
                    }
                });
                plot.write("sources", [&]() {
                    YamlSeq sources(out);

                    for (auto &s : p.sources) {
                        out << s->name;
                    }
                });
            }
        });
    }

    std::ofstream stream(path / (m_desc->name + DashboardDescription::fileExtension), std::ios::out | std::ios::trunc);
    if (!stream.is_open()) {
        return;
    }
    stream << out.c_str();
}

void Dashboard::newPlot() {
    m_plots.push_back({});
    m_plots.back().axes.push_back({ Plot::Axis::X });
    m_plots.back().axes.push_back({ Plot::Axis::Y });
}

std::shared_ptr<DashboardDescription> DashboardSource::load(const std::string &filename) {
#ifndef EMSCRIPTEN
    auto          path = std::filesystem::path(this->path) / filename;
    std::ifstream stream(path, std::ios::in);
    if (!stream.is_open()) {
        return {};
    }
    YAML::Node tree      = YAML::Load(stream);

    auto favorite = tree["favorite"];
    auto lastUsed = tree["lastUsed"];

    auto getDate  = [](const auto &str) -> decltype(DashboardDescription::lastUsed) {
        if (str.size() < 10) {
            return {};
        }
        int                         year  = std::atoi(str.data());
        unsigned                    month = std::atoi(str.c_str() + 5);
        unsigned                    day   = std::atoi(str.c_str() + 8);

        std::chrono::year_month_day date{ std::chrono::year{ year }, std::chrono::month{ month }, std::chrono::day{ day } };
        return std::chrono::sys_days(date);
    };

    return std::make_shared<DashboardDescription>(DashboardDescription{
            .name       = path.stem(),
            .source     = this,
            .isFavorite = favorite.IsScalar() ? favorite.as<bool>() : false,
            .lastUsed   = lastUsed.IsScalar() ? getDate(lastUsed.as<std::string>()) : std::nullopt });
#endif
    return {};
}

std::shared_ptr<DashboardDescription> DashboardDescription::createEmpty(const std::string &name) {
    return std::make_shared<DashboardDescription>(DashboardDescription{
            .name       = name,
            .source     = unsavedSource(),
            .isFavorite = false,
            .lastUsed   = std::nullopt });
}

} // namespace DigitizerUi