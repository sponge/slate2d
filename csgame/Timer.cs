using System.Diagnostics;

public static class Timer {
  static Dictionary<string, Stopwatch> ActiveTimers = new();
  static Dictionary<string, List<(long WallTime, double Sample)>> History = new();
  public static void Start(string timerName) {
    var timer = new Stopwatch();
    ActiveTimers[timerName] = timer;
    timer.Start();
  }

  public static double End(string timerName) {
    var timer = ActiveTimers[timerName];
    timer.Stop();
    double ms = timer.Elapsed.TotalMilliseconds;
    ActiveTimers[timerName] = null;
    long now = DateTime.Now.Ticks / TimeSpan.TicksPerMillisecond;

    if (!History.ContainsKey(timerName)) {
      History[timerName] = new List<(long Ms, double Time)>();
    }

    History[timerName].Add((now, ms));

    History[timerName] = History[timerName].FindAll(x => x.WallTime >= now - 5000);

    return ms;
  }

  public static double Max(string timerName) {
    var history = History[timerName];
    return history.Max(x => x.Sample);
  }
}

