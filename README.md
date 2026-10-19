# commando

A simple C cli agent to quickly search stuff from the CLI and get code outputs.

Doing this is C was quite dumb but i wanted to so fuck it i guess
but still i just clauded the curling as i dont wanna us 32431531
functions just to make one API call

UPDATE: I should have just done it in python ngl

## USAGE

```bash
./commando -q "Hello what is 2x2?" -p -cp
```

The agent would anser just bare 4 and the -p command would display the price of
the interaction and -cp would copy the output to your clipboard.

```bash
./commando -c "Write a str reverser in python" -cp
```

Instead here the ai would respond with a code block followed by some comments it
deemed necessary here -cp would in the bacground wrap the code in a `<code>` tag
and only copy the code.

```bash
./commando -m sol -q "what is a pointer?"
```

Pick which model answers with -m.

## ENV FILES

Create a `.env` file with your key:

```bash
API_KEY=your_key_here
```

Then create an `env.h` next to the source so the program knows where to find it.
Get the absolute path of your `.env` by running this in the folder that contains it:

```bash
realpath .env
```

Put the output in `env.h`:

```c
#define ENV_PATH "/home/user/projects/commando/.env"
```

## FLAGS

| Flag | Long form | Description |
|------|-----------|-------------|
| `-q` | `--question` | Treat the input as a question. Short, direct answer. |
| `-c` | `--code` | Treat the input as a coding request. Responds with code. |
| `-m` | `--model` | Choose the model. Takes the model name as the next argument. Supported: `astra`, `sol`, `luna`. |
| `-p` | `--price` | Show the cost of the last conversation. |
| `-t` | `--token` | Show token usage / status for the API key. Ignores all other options except `--help`. |
| `-cp` | `--copy` | Copy the AI's output to the clipboard. (not implemented yet) |
| `-h` | `--help` | Show the help message. |

## FUTURE

Add the copy flag it was really funny to explicity give examples
and just not have it implemented in ngl.

Add better JSON API request building so other models (with different
request formats) can be supported.
