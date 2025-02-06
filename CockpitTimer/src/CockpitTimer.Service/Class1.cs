using Microsoft.Extensions.Logging;
using System.Collections.Concurrent;

namespace CockpitTimer.Service;

public interface ITimerService
{
    void ReStart(string timer);
    string CurrentTime();
}

public interface IRealTimeClock
{
    int Hour { get; }
    int Minute { get; }
    int Second { get; }
    int Millisecond { get; }
    long TotalSeconds { get; }
    DateTime Now { get; }
}

public class TimerService : ITimerService
{
    private readonly ILogger<TimerService> _logger;
    private readonly IRealTimeClock _realTimeClock;
    private readonly ConcurrentDictionary<string, DateTime> _timers = new();

    public TimerService(ILogger<TimerService> logger, IRealTimeClock realTimeClock)
    {
        _logger = logger;
        _realTimeClock = realTimeClock;
    }

    public string CurrentTime() => $"{_realTimeClock.Hour:D2}:{_realTimeClock.Minute:D2}";

    public void Start()
    {
        _logger.LogInformation("Starting timer service");
    }

    public void ReStart(string timer) {
        if (string.IsNullOrWhiteSpace(timer)) timer = "system";
        var now = _realTimeClock.Now;
        _timers.AddOrUpdate(timer, now, (_, _) => now);
    }
}
