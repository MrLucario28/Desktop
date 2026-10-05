import os
import sys

def main():
    try:
        pid = os.fork()
    except OSError as e:
        sys.stderr.write(f"Error al ejecutar fork: {e}\n")
        sys.exit(1)

    if pid == 0:
        for i in range(10000, 0, -1):
            print(f"[HIJO] {i}")
    else:
        for i in range(1, 10001):
            print(f"[PADRE] {i}")
        
        os.waitpid(pid, 0)

if __name__ == "__main__":
    main()