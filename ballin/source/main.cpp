#include <bitset>
#include <cassert>
#include <functional>
#include <iostream>
#include <print>
#include <queue>
#include <ranges>
#include <algorithm>
#include <sstream>

namespace ballin {

    namespace {

    auto calculate_edit_distance(std::string_view from, std::string_view to)
    {
        if (from.empty()) return to.size();
        if (to.empty()) return from.size();

        auto const fromTail = from.substr(1);
        auto const toTail   = from.substr(1);

        if (from.front() == to.front())
        {
            return calculate_edit_distance(fromTail, toTail);
        }

        return 1 + std::ranges::min({
            calculate_edit_distance(fromTail, to),
            calculate_edit_distance(toTail, from),
            calculate_edit_distance(fromTail, toTail)
        });
    }

    void handle_non_existing_command(auto const& availableCommands, auto const& commandName)
    {
        std::print("the command `{}` doesn't exist.", commandName);

        auto similarCommands = std::views::keys(availableCommands) | std::views::filter([&] (auto&& value) {
            auto const distance = calculate_edit_distance(commandName, value);
            auto const size     = std::ranges::max(commandName.size(), value.size());
            return  (size - distance) / size * 100 > 70;
        });

        if (!similarCommands.empty())
            std::println(" did you mean:");
        else
            std::print("\n");

        for (auto const command : similarCommands)
        {
            std::println("    - {}", command);
        }
    }

    }

class Command
{
public:
    using argument_t  = std::deque<std::string>;
    using return_t    = std::deque<std::string>;
    using signature_t = std::function<return_t(argument_t)>;

public:
    Command() = default;

    Command(std::string_view const commandName, std::size_t const numberOfArguments, signature_t const commandAction):
        name_m(commandName),
        numberOfArguments_m(numberOfArguments),
        action_m(commandAction)
    {}

    constexpr auto const& arguments() const { return arguments_m; }
    constexpr auto const& number_of_arguments() const { return numberOfArguments_m; }
    constexpr auto& subcommands() const { return subcommands_m; }
    constexpr auto const& name() const { return name_m; }

    auto push_back_argument(std::string_view const argument) { arguments_m.push_back(argument.data()); }
    auto push_front_argument(std::string_view const argument) { arguments_m.push_front(argument.data()); }
    auto push_subcommand(ballin::Command&& subcommand) { subcommands_m.push_back(subcommand); }

    return_t operator()() const { return std::invoke(action_m, arguments_m); }

    return_t operator()(argument_t const arguments) const
    {
        auto localArguments = arguments_m;

        std::ranges::for_each(arguments, [&] (auto&& argument) {
            localArguments.push_back(argument);
        });

        return std::invoke(action_m, localArguments);
    }

private:
    std::string name_m {};
    argument_t arguments_m {};
    std::size_t numberOfArguments_m {};
    signature_t action_m {};

    std::vector<ballin::Command> subcommands_m {};
};

class CommandsMap
{
public:
    constexpr auto const& map() const { return commands_m; }
    constexpr auto contains(std::string_view commandName) const { return commands_m.contains(commandName.data()); }
    constexpr auto const& command(std::string_view commandName) const { return commands_m.at(commandName.data()); }

    void register_command(ballin::Command command)
    {
        assert(commands_m.contains(command.name()) != true);
        commands_m[command.name()] = command;
    }

private:
    std::unordered_map<std::string, ballin::Command> commands_m {};
};

class Interpreter
{
public:
    explicit Interpreter(CommandsMap const& commands):
        availableCommands_m(commands)
    {}

    void enqueue_command(std::string_view input)
    {
        auto commands = input | std::views::split(' ') | std::views::chunk_by([] (auto, auto rhs) { return rhs.front() != '|'; });

        auto fnParseCommand = [this] (auto input) -> std::optional<ballin::Command> {
            auto view            = input | std::views::drop_while([] (auto value) { return value.front()  == '|'; });
            auto const name      = std::ranges::to<std::string>(view.front());
            auto const arguments = std::ranges::to<std::vector<std::string>>(view | std::views::drop(1));

            auto const entry = availableCommands_m.map().find(name);

            if (entry == availableCommands_m.map().end())
            {
                ballin::handle_non_existing_command(availableCommands_m.map(), name);
                return std::nullopt;
            }

            auto command = entry->second;

            for (auto const& argument : arguments)
            {
                command.push_back_argument(argument);
            }

            return command;
        };

        auto masterCommand = fnParseCommand(commands.front());

        if (!masterCommand.has_value()) { return; }

        for (auto const& view : commands | std::views::drop(1))
        {
            auto subcommand = fnParseCommand(view);

            if (!subcommand.has_value()) { return; }

            masterCommand.value().push_subcommand(std::move(subcommand.value()));
        }

        queuedCommands_m.push(masterCommand.value());
    }

    void execute()
    {
        while (!queuedCommands_m.empty())
        {
            auto const& masterCommand = queuedCommands_m.front();

            auto operationResult = masterCommand();

            for (auto const& subcommand : masterCommand.subcommands())
            {
                operationResult = subcommand(operationResult);
            }

            queuedCommands_m.pop();
        }
    }

private:
    std::queue<ballin::Command> queuedCommands_m {};
    ballin::CommandsMap const& availableCommands_m;

};

}

auto register_commands(ballin::CommandsMap& commands)
{
    using argument_t = ballin::Command::argument_t;
    using return_t   = ballin::Command::return_t;

    commands.register_command(ballin::Command
    {
        "quit", 0, [] (argument_t) -> return_t {
            std::exit(EXIT_SUCCESS);
            return {};
        }
    });

    commands.register_command(ballin::Command
    {
        "echo", 1, [] (argument_t arguments) -> return_t {
            std::println("{}", std::ranges::to<std::string>(arguments | std::views::join_with(' ')));
            return {};
        }
    });

    commands.register_command(ballin::Command
    {
        "add", 2, [] (argument_t arguments) -> return_t {
            float lhs {};
            std::stringstream { arguments.at(0) } >> lhs;
            float rhs {};
            std::stringstream { arguments.at(1) } >> rhs;

            std::stringstream stream {};
            stream << (lhs + rhs);

            return { stream.str() };
        }
    });

    commands.register_command(ballin::Command
    {
        "sub", 2, [] (argument_t arguments) -> return_t {
            float lhs {};
            std::stringstream { arguments.at(0) } >> lhs;
            float rhs {};
            std::stringstream { arguments.at(1) } >> rhs;

            std::stringstream stream {};
            stream << (lhs - rhs);

            return { stream.str() };
        }
    });

    commands.register_command(ballin::Command
    {
        "mul", 2, [] (argument_t arguments) -> return_t {
            float lhs {};
            std::stringstream { arguments.at(0) } >> lhs;
            float rhs {};
            std::stringstream { arguments.at(1) } >> rhs;

            std::stringstream stream {};
            stream << (lhs * rhs);

            return { stream.str() };
        }
    });

    commands.register_command(ballin::Command
    {
        "div", 2, [] (argument_t arguments) -> return_t {
            float lhs {};
            std::stringstream { arguments.at(0) } >> lhs;
            float rhs {};
            std::stringstream { arguments.at(1) } >> rhs;

            std::stringstream stream {};
            stream << (lhs / rhs);

            return { stream.str() };
        }
    });

    commands.register_command(ballin::Command
    {
        "hex", 1, [] (argument_t arguments) -> return_t {
            std::size_t value {};
            std::stringstream { arguments.at(0) } >> value;

            std::stringstream stream {};
            stream << std::hex << value;

            return { "0x" + stream.str() };
        }
    });

    commands.register_command(ballin::Command
    {
        "bin", 1, [] (argument_t arguments) -> return_t {
            std::size_t value {};
            std::stringstream { arguments.at(0) } >> value;

            if (value <= std::numeric_limits<std::uint8_t>::max()) { return { "0b" + std::bitset<8>(value).to_string() }; }
            else if (value <= std::numeric_limits<std::uint16_t>::max()) { return { "0b" + std::bitset<16>(value).to_string() }; }
            else if (value <= std::numeric_limits<std::uint32_t>::max()) { return { "0b" + std::bitset<32>(value).to_string() }; }
            else if (value <= std::numeric_limits<std::uint64_t>::max()) { return { "0b" + std::bitset<64>(value).to_string() }; }

            std::unreachable();
        }
    });

    commands.register_command(ballin::Command
    {
        "iota", 2, [] (argument_t arguments) -> return_t {
            std::size_t minimum {};
            std::stringstream { arguments.at(0) } >> minimum;
            std::size_t maximum {};
            std::stringstream { arguments.at(1) } >> maximum;

            std::deque<std::string> result {};

            for (auto index : std::views::iota(minimum, maximum + 1))
            {
                std::stringstream stream {};
                stream << index;
                result.push_back(stream.str());
            }

            return result;
        }
    });

    commands.register_command(ballin::Command
    {
        "apply", std::numeric_limits<std::size_t>::max(), [&] (argument_t arguments) -> return_t {
            auto requestedCommandEntry = commands.map().find(arguments.at(0));

            if (requestedCommandEntry == commands.map().end())
            {
                ballin::handle_non_existing_command(commands.map(), arguments.at(0));
                return {};
            }

            auto requestedCommand = requestedCommandEntry->second;
            auto requestedCommandArguments = std::ranges::to<std::deque>(arguments  | std::views::take(requestedCommand.number_of_arguments()) | std::views::drop(1));

            std::deque<std::string> result {};

            for (auto const& argument : arguments | std::views::drop(requestedCommand.number_of_arguments()))
            {
                requestedCommandArguments.push_front(argument);

                if (auto operationResult = requestedCommand(requestedCommandArguments); !operationResult.empty())
                {
                    result.push_back(operationResult.front());
                }

                requestedCommandArguments.pop_front();
            }

            return result;
        }
    });
}

int main()
{
    ballin::CommandsMap commands {};
    register_commands(commands);

    ballin::Interpreter interpreter { commands };

    std::println("ballin interpreter v0.4.2.0");

    while (true)
    {
        std::string input {};
        std::print(">> ");
        std::getline(std::cin, input);

        interpreter.enqueue_command(input);
        interpreter.execute();
    }
}