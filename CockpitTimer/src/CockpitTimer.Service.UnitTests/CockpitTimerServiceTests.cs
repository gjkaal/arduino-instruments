using Castle.Core.Logging;
using Microsoft.Extensions.Logging;
using Moq;
using System.Text;

namespace CockpitTimer.Service.UnitTests;

[TestClass]
public class CockpitTimerServiceTests
{
    private readonly ILogger<TimerService> _logger;
    public CockpitTimerServiceTests()
    {
        using var loggerFactory = LoggerFactory.Create(c => c.AddConsole());
        _logger = loggerFactory.CreateLogger<TimerService>();
    }

    [TestMethod]
    public void CockpitTimerService_CanInitialize()
    {
        var realTimeClock = new Mock<IRealTimeClock>();
        var timerService = new TimerService(_logger, realTimeClock.Object);
        Assert.IsNotNull(timerService);
    }

    [TestMethod]
    public void CockpitTimerService_CanStart()
    {
        var realTimeClock = new Mock<IRealTimeClock>();
        var timerService = new TimerService(_logger, realTimeClock.Object);
        var sb = new StringBuilder();
        
        // Act

        timerService.Start();

        // Assert
        Console.WriteLine(sb.ToString());
        Assert.IsNotNull(timerService);
    }

    [TestMethod]
    public void CockpitTimerService_CanProvideCurrentTime()
    {
        var logger = new Mock<ILogger<TimerService>>();
        var realTimeClock = new Mock<IRealTimeClock>();
        var timerService = new TimerService(logger.Object, realTimeClock.Object);

        const int timeHour = 14;
        const int timeMinute = 5;
        realTimeClock.Setup(clock => clock.Hour).Returns(timeHour);
        realTimeClock.Setup(clock => clock.Minute).Returns(timeMinute);
        realTimeClock.Setup(clock => clock.Now).Returns(new DateTime(2023,12,14, timeHour, timeMinute, 7));

        timerService.Start();
        var timeString = timerService.CurrentTime();
        Assert.AreEqual("14:05", timeString);
    }

}