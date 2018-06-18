# service_hosting
Windows service wrapper, which can be used to wrap a managed process as a Windows service. This app is especially for developers to run .NET console applications as Windows service. Just create a .NET console app with Ctrl+C handler to receive termination of the process. 

## Why the heck?
* Windows has the weirdness that services have to respond within 30 seconds. Due to the .NET stack, it can take more than 30 seconds to startup a .NET app during system boot. As a result, the service does not start...
* Command-line apps are easier to manage

## Features
* configuration via json file
* restart wrapped process in case of unusual termination
* state logging into Windows Event Log
 
## Example of a console app

```csharp
class Program
{
	static readonly CancellationTokenSource _cancel = new CancellationTokenSource();

	static void Main(string[] args)
	{
		Console.CancelKeyPress += (s, e) =>
		{
			e.Cancel = true;
			_cancel.Cancel();
		};

		while (!_cancel.IsCancellationRequested)
		{
			Console.Write(".");
			Thread.Sleep(5000);
		}
	}
}
```

## Requirements
* Microsoft Visual C++ Redistributable für Visual Studio 2017 https://aka.ms/vs/15/release/vc_redist.x86.exe

## License
[MIT](https://github.com/geheb/service_hosting/blob/master/LICENSE)