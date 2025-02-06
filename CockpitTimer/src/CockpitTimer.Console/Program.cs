using Spectre.Console.Cli;
using Microsoft.Extensions.DependencyInjection;
using CockpitTimer.Service;
using Microsoft.Extensions.Logging;
using Spectre.Console;
using System.ComponentModel;
using System.Diagnostics.CodeAnalysis;

namespace CockpitTimer.Console;

internal class Program
{
    private static ILogger<Program> Logger { get; }
    private static IServiceCollection Services { get; }
    private static IServiceProvider Container { get; }
    private static MyTypeRegistrar Registrar { get; }
    static Program()
    {
        Services = new ServiceCollection()
            .AddLogging((action) =>
            {
                action.AddSimpleConsole((options) => {
                    options.TimestampFormat = "HH:mm:ss ";
                    options.IncludeScopes = true;
                    options.UseUtcTimestamp = false;
                    options.SingleLine = true;
                });
            })
            .AddSingleton<ITimerService, TimerService>()
            .AddSingleton<IRealTimeClock, RealTimeClock>()
            .AddSingleton<IAnsiConsole>((provider) => AnsiConsole.Console)
            .AddScoped<StartCommand>();

        Container = Services.BuildServiceProvider();
        Logger = Container.GetRequiredService<ILogger<Program>>();
        Registrar = new MyTypeRegistrar(Services);
    }

    public static async Task<int> Main(string[] args)
    {
        Logger.LogInformation("Starting CockpitTimer.Console");

        var console = Container.GetRequiredService<IAnsiConsole>();
        console.Write(
            new FigletText("Cockpit Timer")           
            .LeftJustified()
            .Color(Color.Green));

        return await Commands.GetApp(Registrar).RunAsync(args);
    }
}

public sealed class TypeResolver : ITypeResolver, IDisposable
{
    private readonly IServiceProvider _provider;

    public TypeResolver(IServiceProvider provider)
    {
        _provider = provider ?? throw new ArgumentNullException(nameof(provider));
    }

    public object? Resolve(Type? type)
    {
        if (type == null)
        {
            return null;
        }

        return _provider.GetService(type);
    }

    public void Dispose()
    {
        if (_provider is IDisposable disposable)
        {
            disposable.Dispose();
        }
    }
}

public sealed class MyTypeRegistrar : ITypeRegistrar
{
    private readonly IServiceCollection _builder;

    public MyTypeRegistrar(IServiceCollection builder)
    {
        _builder = builder;
    }

    public ITypeResolver Build()
    {
        return new TypeResolver(_builder.BuildServiceProvider());
    }

#pragma warning disable IL2067 // Target parameter argument does not satisfy 'DynamicallyAccessedMembersAttribute' in call to target method. The parameter of method does not have matching annotations.
    public void Register(
        Type service,
         Type implementation)
    {
        _builder.AddSingleton(service, implementation);
    }
#pragma warning restore IL2067 // Target parameter argument does not satisfy 'DynamicallyAccessedMembersAttribute' in call to target method. The parameter of method does not have matching annotations.

    public void RegisterInstance(Type service, object implementation)
    {
        _builder.AddSingleton(service, implementation);
    }

    public void RegisterLazy(Type service, Func<object>? func)
    {
        ArgumentNullException.ThrowIfNull(func);

        _builder.AddSingleton(service, (provider) => func());
    }
}

public class RealTimeClock : IRealTimeClock
{
    public int Hour => DateTime.Now.Hour;
    public int Minute => DateTime.Now.Minute;
    public int Second => DateTime.Now.Second;
    public int Millisecond => DateTime.Now.Millisecond;
    public long TotalSeconds => (long)(DateTime.Now - DateTime.UnixEpoch).TotalSeconds;
    public DateTime Now => DateTime.Now;
}

// console app
internal static class Commands
{
    public static CommandApp GetApp(ITypeRegistrar registrar)
    {
        var app = new CommandApp(registrar);
        app.Configure(config =>
        {
            config.AddCommand<StartCommand>("start");
        });
        return app;
    }
}

// console tasks
public class TimerSettings : CommandSettings
{
    /// <summary>
    /// Gets or sets the name of the timer. If the name is not specified, 
    /// the user will be prompted for the name.
    /// </summary>
    [CommandOption("-n|--name")]
    [Description("Name of the timer")]
    public string? Name { get; set; }
}

public class StartCommand : AsyncCommand<TimerSettings>
{
    private ITimerService Service { get; }
    private IAnsiConsole Console { get; }

    public StartCommand(ITimerService timerService, IAnsiConsole console) : base()
    {        
        Service = timerService;
        Console = console;
    }

    public override Task<int> ExecuteAsync(CommandContext context, TimerSettings settings) {
        Console.MarkupLine($"[bold green]Starting timer service[/]");
        Service.ReStart(settings.Name);
        Console.MarkupLine($"[bold green]Current time is {Service.CurrentTime()}[/]");
        return Task.FromResult(0);
    }
}
