#!/usr/bin/env python3
import sys
import random
import os
import tempfile
import shutil
import subprocess

def modify_line(line):
    if line.endswith('\n'):
        return line[:-1] + "a" + '\n'
    return line + "a"

def get_line_count(filepath):
    result = subprocess.run(['wc', '-l', filepath], capture_output=True, text=True, check=True)
    return int(result.stdout.split()[0])

def process_large_file(input_path, total_lines, probability=0.5, chunk_size=1024*1024):
    target_count = int(total_lines * probability)
    lines_to_modify = set(random.sample(range(total_lines), target_count))
    
    dir_name = os.path.dirname(input_path) or '.'
    temp_fd, temp_path = tempfile.mkstemp(dir=dir_name, suffix='.tmp')
    
    try:
        with open(input_path, 'r', encoding='UTF-8', buffering=chunk_size) as f_in, \
             os.fdopen(temp_fd, 'w', encoding='UTF-8', buffering=chunk_size) as f_out:
            
            modified_count = 0
            for line_num, line in enumerate(f_in):
                if line_num in lines_to_modify:
                    line = modify_line(line)
                    modified_count += 1
                f_out.write(line)
                
                if (line_num + 1) % 1_000_000 == 0:
                    print(f"Fortschritt: {line_num + 1:,}/{total_lines:,}", end='\r', file=sys.stderr)
            
            print(f"\nFertig! {modified_count:,} Zeilen modifiziert.", file=sys.stderr)
        
        shutil.move(temp_path, input_path)
        print("Originaldatei ersetzt.", file=sys.stderr)
        
    except Exception as e:
        if os.path.exists(temp_path):
            os.remove(temp_path)
        raise e

if __name__ == "__main__":
    if len(sys.argv) != 4:
        print(f"Usage: python3 {sys.argv[0]} <file> <line count> <change in decimal>")
        sys.exit(0)

    file_path = sys.argv[1]
    amount_lines = int(sys.argv[2])
    probability = float(sys.argv[3])
    
    if not os.path.exists(file_path):
        print(f"File {file_path} not found!", file=sys.stderr)
        sys.exit(1)
    
    process_large_file(file_path, amount_lines, probability=probability)