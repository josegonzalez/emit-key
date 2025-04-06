# Emit Key

A Linux utility that creates a virtual input device and emits key presses programmatically. This tool is useful for automation, testing, or creating custom keyboard shortcuts.

## Features

- Emits key presses for function keys (F1-F12) and lowercase letters (a-z)
- Can run in one-shot mode or wait for signals
- Supports graceful handling of termination signals
- Lightweight and fast execution

## Requirements

- Linux operating system
- Root access (for uinput device creation)
- C compiler (gcc recommended)

## Installation

1. Clone the repository
2. Compile the program:

   ```bash
   gcc emit-key.c -o emit-key
   ```

3. Make the binary executable:

   ```bash
   chmod +x emit-key
   ```

## Usage

### Basic Usage

```bash
sudo ./emit-key -k <key_name>
```

Where `<key_name>` can be:

- Any function key (F1-F12)
- Any lowercase letter (a-z)
- Any number key (0-9)

Example:

```bash
sudo ./emit-key -k a  # Emits an 'a' key press
```

The `<key_name>` can also be a comma-delimited set of keys to press in order:

```bash
sudo ./emit-key -k p,9,f5,p
```

### Custom Sleep Durations

You can specify custom sleep durations (in microseconds) between key presses using a colon separator:

```bash
sudo ./emit-key -k p,9,f5:500000,p
```

This will:

1. Press 'p'
2. Sleep 100000 microseconds (default)
3. Press '9'
4. Sleep 100000 microseconds (default)
5. Press 'F5'
6. Sleep 500000 microseconds (custom duration)
7. Press 'p'

The default sleep duration between key presses is 100000 microseconds (0.1 seconds). You can override this by specifying a custom duration after any key using the `:duration` syntax.

### Signal Mode

Run the program in signal mode to wait for SIGUSR1 before emitting the key:

```bash
sudo ./emit-key -k <key_name> -s
```

Then send a signal to trigger the key press:

```bash
kill -SIGUSR1 <pid>
```

## Command Line Options

- `-k <key_name>`: Specify the key to emit (required)
- `-s`: Run in signal mode (optional)

## Signal Handling

The program handles the following signals:

- SIGINT (Ctrl+C): Graceful exit
- SIGTERM: Graceful exit
- SIGUSR1: Emit the specified key press (only in signal mode)

## License

This project is open source and available under the MIT License.

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.
