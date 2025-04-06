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

Example:

```bash
sudo ./emit-key -k a  # Emits an 'a' key press
```

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
