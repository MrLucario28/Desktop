import os
import sys

def main():
    try:
        # Se crea un proceso hijo
        pid = os.fork()
    except OSError as e:
        sys.stderr.write(f"Error al ejecutar fork: {e}\n")
        sys.exit(1)

    if pid == 0:
        # --- PROCESO HIJO ---
        # Imprime números del 10,000 al 1
        for i in range(10000, 0, -1):
            print(f"[HIJO] {i}")
    else:
        # --- PROCESO PADRE ---
        # Imprime números del 1 al 10,000
        for i in range(1, 10001):
            print(f"[PADRE] {i}")
        
        # Espera a que el proceso hijo termine para evitar procesos zombi
        os.waitpid(pid, 0)

if __name__ == "__main__":
    main()